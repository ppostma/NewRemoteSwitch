# frozen_string_literal: true

require "mkmf"

have_library("wiringPi")
have_library("stdc++")

$CFLAGS << " -std=c++14 -Wall -Wextra -Wpedantic"

create_makefile("new_remote/new_remote")
