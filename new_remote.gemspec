# frozen_string_literal: true

lib = File.expand_path('lib', __dir__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require "new_remote/version"

Gem::Specification.new do |spec|
  spec.name          = "new_remote"
  spec.version       = NewRemote::VERSION
  spec.authors       = ["Peter Postma"]
  spec.email         = ["peter.postma@gmail.com"]

  spec.summary       = "NewRemoteSwitch for Arduino ported to Raspberry Pi"
  spec.homepage      = "https://github.com/ppostma/NewRemoteSwitch"
  spec.license       = "GPL-3.0"

  # Specify which files should be added to the gem when it is released.
  # The `git ls-files -z` loads the files in the RubyGem that have been added into git.
  spec.files         = Dir.chdir(File.expand_path(__dir__)) do
    `git ls-files -z`.split("\x0").reject { |f| f.match(%r{^(test|spec|features)/}) }
  end
  spec.bindir        = "exe"
  spec.extensions    = %w[ext/new_remote/extconf.rb]
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.require_paths = ["lib"]

  spec.add_development_dependency "bundler", "~> 2.0"
  spec.add_development_dependency "rake", "~> 10.0"
  spec.add_development_dependency "rake-compiler", "~> 1.0"
end
