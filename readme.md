# TCA9555 I/O Expander component for [ESPHome](https://esphome.io)

![Made fot ESPHome](https://esphome.io/_images/made-for-esphome-black-on-white.svg)

This TCA9555 component allows you to use TCA955 I/O expanders
([datasheet](https://www.ti.com/lit/ds/symlink/tca9555.pdf)) in ESPHome. It uses [I²C Bus](https://esphome.io/components/i2c) for communication.

Contrary to the [native ESPHome component](https://esphome.io/components/tca9555.html), this component handles the interrupt signal. By sending an interrupt signal on this line, the TCA955 remote I/O can inform the microcontroller if there is incoming data on its ports without having to poll via the I²C bus.

Once configured, you can use any of the 16 pins (TCA9555) as
pins for your projects. Within ESPHome they emulate a real internal GPIO pin
and can therefore be used with many of ESPHome's components such as the [GPIO
binary](https://esphome.io/components/binary_sensor/gpio) sensor or [GPIO switch](https://esphome.io/components/switch/gpio).

Any option accepting a [Pin Schema](https://esphome.io/guides/configuration-types#config-pin-schema) can theoretically be used, but some more
complicated components that do communication through this I/O expander will
not work.

```yaml
# use this component in your esphome configuration
external_components:
  - source:
      type: git
      url: https://github.com/Benichou34/tca9555_esphome
    
# Example configuration entry
tca9555_it:
  - id: 'TCA9555_hub'
    irq_pin: 1

# Individual outputs
switch:
  - platform: gpio
    name: "TCA9555 Pin #0"
    pin:
      tca9555: tca9555_hub
      # Use pin number 0
      number: 0

# Individual inputs
binary_sensor:
  - platform: gpio
    name: "TCA9555 Pin #1"
    pin:
      tca9555: tca9555_hub
      # Use pin number 1
      number: 1
```

## Import component

You must import this component using the [external components](https://esphome.io/components/external_components.html) feature.

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/Benichou34/tca9555_esphome
```

## Configuration variables

- **id** (**Required**, [ID](https://esphome.io/guides/configuration-types#config-id)): The id to use for this TCA9555 component.
- **address** (*Optional*, int): The I²C address of the driver.
  Defaults to ``0x21``.
- **irq_pin** (*Optional*, [Pin](https://esphome.io/guides/configuration-types#config-pin)): The pin connected to the TCA9555 IRQ line (if connected).

## Pin configuration variables

- **TCA9555** (**Required**, [ID](https://esphome.io/guides/configuration-types#config-id)): The id of the TCA9555 component of the pin.
- **number** (**Required**, int): The pin number.
- **inverted** (*Optional*, boolean): If all read and written values
  should be treated as inverted. Defaults to ``false``.
- **mode** (*Optional*, string): A pin mode to set for the pin at. One of ``INPUT`` or ``OUTPUT``.

## See Also

- [External Components](https://esphome.io/components/external_components.html)
- [I²C Bus](https://esphome.io/components/i2c)
- [GPIO Switch](https://esphome.io/components/switch/gpio)
- [GPIO Binary Sensor](https://esphome.io/components/binary_sensor/gpio)

## Disclaimer

You use this firmware at your own risk. We cannot be held responsible for any damage or injury to materials or persons. The modification of the material entails the loss of the legal guarantee.

## License

[MIT](https://choosealicense.com/licenses/mit/)
