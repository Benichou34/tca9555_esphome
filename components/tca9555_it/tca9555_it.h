#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"

#include <bitset>

namespace esphome::tca9555_it
{
	class TCA9555Component : public Component, public i2c::I2CDevice
	{
	public:
		void setup() override;
		void loop() override;
		float get_setup_priority() const override;
		void dump_config() override;

		// Methods used by config
		void set_irq_pin(InternalGPIOPin* pin);

		// Methods used by TCA9555GPIOPin class
		void set_pin_inverted(uint8_t pin, bool inverted);
		void set_pin_flags(uint8_t pin, gpio::Flags flags);
		gpio::Flags get_pin_flags(uint8_t pin) const;

		bool digital_read(uint8_t pin) const;
		void digital_write(uint8_t pin, bool value);

	private:
		bool write_register(uint8_t reg, const std::bitset<16>& values);
		bool read_register(uint8_t reg, std::bitset<16>& values);

		std::bitset<16> m_input;
		std::bitset<16> m_output;
		std::bitset<16> m_polarity;
		std::bitset<16> m_configuration = 0xFFFF;
		bool m_update_input = true;
		bool m_update_output = true;
		InternalGPIOPin* m_pin_irq = nullptr;
	};

	// Helper class to expose a TCA9555 pin as an internal input GPIO pin.
	class TCA9555GPIOPin : public GPIOPin, public Parented<TCA9555Component>
	{
	public:
		void setup() override;
		void pin_mode(gpio::Flags flags) override;
		gpio::Flags get_flags() const override;
		bool digital_read() override;
		void digital_write(bool value) override;
		std::string dump_summary() const override;

		// Methods used by config
		void set_pin(uint8_t pin);
		void set_inverted(bool inverted);
		void set_flags(gpio::Flags flags);

	private:
		uint8_t m_pin = 0;
	};
} // namespace esphome::tca9555_it
