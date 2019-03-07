# frozen_string_literal: true

require "new_remote/new_remote"
require "new_remote/version"

module NewRemote
  class UnitCommand
    def initialize(transmitter, device)
      @transmitter = transmitter
      @device = device
    end

    def on
      @transmitter.send_unit(@device, true)
    end

    def off
      @transmitter.send_unit(@device, false)
    end

    def dim(level)
      @transmitter.send_unit_dim(@device, level)
    end
  end

  class GroupCommand
    def initialize(transmitter)
      @transmitter = transmitter
    end

    def on
      @transmitter.send_group(true)
    end

    def off
      @transmitter.send_group(false)
    end

    def dim(level)
      @transmitter.send_group_dim(level)
    end
  end

  class Transmitter
    def unit(device)
      UnitCommand.new(self, device)
    end

    def group
      GroupCommand.new(self)
    end
  end
end
