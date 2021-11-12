# arduino_can_usb_dongle
Like a PCAN USB dongle but much worse

This code supports a Arduino with an Inland (micro center brand) CAN-BUS shield.

The whole idea of this was to get a CAN receive/transmit interface over USB, that works with python-can (it has a convenient serial interface). Kinda like a [PCAN USB Pro FD](https://www.peak-system.com/PCAN-USB-Pro-FD.366.0.html?&L=1) but on the cheap. However, I could never get this to work for longer than about 60 seconds. Feel free to submit a PR if you can fix it.

## Usage

Install python PIP modules `pyserial python-can cantools udsoncan`

Viewing bus: `python -m can.viewer -i serial -c COM3` (if you install cantools, you can use `cantools monitor -b serial -c COM3 path/to/network.dbc` to get a console app with signal decoding, so cool! like PCAN Explorer but free)

Python-can API: `bus = can.interface.Bus(bustype='serial', channel='COM3')`
