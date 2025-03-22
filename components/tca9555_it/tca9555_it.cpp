#include "tca9555_it.h"
#include "esphome/core/log.h"

namespace
{
	constexpr const char* const TAG = "TCA9555";

	constexpr uint8_t TCA9555_INPUT_PORT_REGISTER_0 = 0x00;  // Input Port 0
	constexpr uint8_t TCA9555_OUTPUT_PORT_REGISTER_0 = 0x02; // Output Port 0
	constexpr uint8_t TCA9555_POLARITY_REGISTER_0 = 0x04;    // Polarity Inversion Port 0
	constexpr uint8_t TCA9555_CONFIGURATION_PORT_0 = 0x06;   // Configuration Port 0

	void IRAM_ATTR isr_callback(bool* update_input)
	{
		*update_input = true;
	}
} // namespace

namespace esphome::tca9555_it
{
	void TCA9555Component::setup()
	{
		ESP_LOGCONFIG(TAG, "Setting up I/O expander...");

		if (m_pin_irq)
		{
			ESP_LOGCONFIG(TAG, "IRQ pin: %d", m_pin_irq->get_pin());
			m_pin_irq->pin_mode(gpio::FLAG_INPUT | gpio::FLAG_PULLUP);
			m_pin_irq->setup();
			m_pin_irq->attach_interrupt(isr_callback, &m_update_input, gpio::INTERRUPT_FALLING_EDGE);
		}

		ESP_LOGCONFIG(TAG, "Polarity: %s", m_polarity.to_string().c_str());
		if (!write_register(TCA9555_POLARITY_REGISTER_0, m_polarity))
		{
			ESP_LOGE(TAG, "Write polarity failed!");
			mark_failed();
			return;
		}

		ESP_LOGCONFIG(TAG, "Configuration: %s", m_configuration.to_string().c_str());
		if (!write_register(TCA9555_CONFIGURATION_PORT_0, m_configuration))
		{
			ESP_LOGE(TAG, "Write configuration failed!");
			mark_failed();
			return;
		}

		m_update_input = true;
		m_update_output = true;
	}

	void TCA9555Component::loop()
	{
		if (is_failed())
			return;

		if (m_update_output)
		{
			if (!write_register(TCA9555_OUTPUT_PORT_REGISTER_0, m_output))
			{
				status_set_warning("Failed to write output register");
			}
			else
			{
				status_clear_warning();
				m_update_output = false;
				m_update_input = true;
			}
		}

		if (m_update_input)
		{
			if (m_pin_irq)
				m_update_input = false;

			if (!read_register(TCA9555_INPUT_PORT_REGISTER_0, m_input))
			{
				status_set_warning("Failed to read input register");
			}
			else
			{
				status_clear_warning();
			}
		}
	}

	void TCA9555Component::dump_config()
	{
		ESP_LOGCONFIG(TAG, "TCA9555:");
		LOG_I2C_DEVICE(this)

		if (m_pin_irq)
		{
			LOG_PIN("  IRQ pin: ", m_pin_irq);
		}

		ESP_LOGCONFIG(TAG, "  Inputs       : %s", m_input.to_string().c_str());
		ESP_LOGCONFIG(TAG, "  Outputs      : %s", m_output.to_string().c_str());
		ESP_LOGCONFIG(TAG, "  Polarity     : %s", m_polarity.to_string().c_str());
		ESP_LOGCONFIG(TAG, "  Configuration: %s", m_configuration.to_string().c_str());

		if (is_failed())
		{
			ESP_LOGE(TAG, "Communication failed!");
		}
	}

	float TCA9555Component::get_setup_priority() const
	{
		return setup_priority::IO;
	}

	void TCA9555Component::set_irq_pin(InternalGPIOPin* pin)
	{
		m_pin_irq = pin;
	}

	void TCA9555Component::set_pin_inverted(uint8_t pin, bool inverted)
	{
		m_polarity[pin] = inverted;
	}

	void TCA9555Component::set_pin_flags(uint8_t pin, gpio::Flags flags)
	{
		m_configuration[pin] = (flags == gpio::FLAG_INPUT);
	}

	gpio::Flags TCA9555Component::get_pin_flags(uint8_t pin) const
	{
		return m_configuration[pin] ? gpio::FLAG_INPUT : gpio::FLAG_OUTPUT;
	}

	bool TCA9555Component::digital_read(uint8_t pin) const
	{
		return m_input[pin];
	}

	void TCA9555Component::digital_write(uint8_t pin, bool value)
	{
		if (!m_configuration[pin] && m_output[pin] != value)
		{
			m_output[pin] = value;
			m_update_output = true;
		}
	}

	bool TCA9555Component::write_register(uint8_t reg, const std::bitset<16>& values)
	{
		ESP_LOGV(TAG, "0x%02X: Write REG 0x%02X=%s", this->address_, reg, values.to_string().c_str());
		uint16_t data = static_cast<uint16_t>(values.to_ulong());
		return write_bytes(reg, reinterpret_cast<const uint8_t*>(&data), sizeof(data));
	}

	bool TCA9555Component::read_register(uint8_t reg, std::bitset<16>& values)
	{
		uint16_t data = 0;
		if (!read_bytes(reg, reinterpret_cast<uint8_t*>(&data), sizeof(data)))
		{
			values.reset();
			return false;
		}

		values = data;
		ESP_LOGV(TAG, "0x%02X: Read REG 0x%02X=%s", this->address_, reg, values.to_string().c_str());
		return true;
	}

	// TCA9555GPIOPin class
	void TCA9555GPIOPin::setup()
	{}

	void TCA9555GPIOPin::pin_mode(gpio::Flags flags)
	{
		set_flags(flags);
	}

	gpio::Flags TCA9555GPIOPin::get_flags() const
	{
		return parent_->get_pin_flags(m_pin);
	}

	bool TCA9555GPIOPin::digital_read()
	{
		return parent_->digital_read(m_pin);
	}

	void TCA9555GPIOPin::digital_write(bool value)
	{
		parent_->digital_write(m_pin, value);
	}

	std::string TCA9555GPIOPin::dump_summary() const
	{
		return str_sprintf("%u via TCA9555", m_pin);
	}

	void TCA9555GPIOPin::set_pin(uint8_t pin)
	{
		m_pin = pin;
	}

	void TCA9555GPIOPin::set_inverted(bool inverted)
	{
		parent_->set_pin_inverted(m_pin, inverted);
	}

	void TCA9555GPIOPin::set_flags(gpio::Flags flags)
	{
		parent_->set_pin_flags(m_pin, flags);
	}
} // namespace esphome::tca9555_it
