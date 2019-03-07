# NewRemoteSwitch for Raspberry Pi

This is a port of the [NewRemoteSwitch for Arduino](https://bitbucket.org/fuzzillogic/433mhzforarduino/wiki/Home) (by Randy Simons) to the Raspberry Pi as a Ruby gem.

It can be used to send and receive signals from and to the new A-series [KlikAanKlikUit](https://www.klikaanklikuit.nl/) remote switches. Using a 433MHz transmitter and/or receiver connected to the GPIO ports on a Raspberry Pi. And from Ruby. Isn't that awesome?

## Installation

If you haven't installed Ruby yet, then do that first:

    $ sudo apt-get install ruby ruby-dev

This gem uses the [wiringPi](http://wiringpi.com/) library, which should be installed as well:

    $ sudo apt-get install wiringpi

Install the gem:

    $ sudo gem install new_remote

## Usage

If you want to use this library as a non-root user, then you need to add that user to the `gpio` group:

    $ sudo usermod -a -G gpio username

### Transmitter

```ruby
require 'new_remote'

transmitter = NewRemote::Transmitter.new(address, pin, periodusec = 260, repeats = 4)
```

The `address` parameter can be either a random number, or copied from your existing remote. You can use the `Receiver` to determine your remotes address.

The `pin` parameter is the GPIO pin where the transmitter is connected to. Please note that it uses the [wiringPi pin numbering scheme] (http://wiringpi.com/pins/).

For more details about the parameters, please read the comments in `NewRemoteTransmitter.h`.

The usage of the transmitter is pretty simple and almost needs no further explanation:

```ruby
switch = transmitter.unit(0)   # select first unit (range 0-15)
switch = transmitter.group     # or select group

switch.on                      # switch on devices(s)
switch.off                     # switch off device(s)
switch.dim(8)                  # set dim level (range 0-15); also switches on device(s)
```

### Receiver

The `receive` method loops forever and shows the received codes on the standard output.

```ruby
require 'new_remote'

NewRemote::Receiver.receive(pin, repeats)
```

The `pin` parameter is the GPIO pin where the receiver is connected to. Please note that it uses the [wiringPi pin numbering scheme] (http://wiringpi.com/pins/).

The `repeats` parameter indicates the number of times the same message must be received to be valid.

For more information about the inner working, please read the comments in `NewRemoteReceiver.h`.

## Development

After checking out the repo, run `bin/setup` to install dependencies. You can also run `bin/console` for an interactive prompt that will allow you to experiment.

To install this gem onto your local machine, run `bundle exec rake install`. To release a new version, update the version number in `version.rb`, and then run `bundle exec rake release`, which will create a git tag for the version, push git commits and tags, and push the `.gem` file to [rubygems.org](https://rubygems.org).

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/ppostma/NewRemoteSwitch.

## License

The gem is available as open source under the terms of the [GNU General Public License version 3](https://opensource.org/licenses/GPL-3.0).
