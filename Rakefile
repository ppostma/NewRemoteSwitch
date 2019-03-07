# frozen_string_literal: true

require "bundler/gem_tasks"
require "rake/extensiontask"

Rake::ExtensionTask.new("new_remote") do |ext|
  ext.ext_dir = "ext/new_remote"
  ext.lib_dir = "lib/new_remote"
end

task default: [:clean, :compile]
