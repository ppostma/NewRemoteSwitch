#include <ruby.h>
#include <iostream>
#include <csignal>

#include "NewRemoteReceiver.h"
#include "NewRemoteTransmitter.h"

typedef void (*sighandler_t)(int);

static sighandler_t old_sig_int_handler, old_sig_term_handler;
static bool signal_received = false;

static void wrap_receiver_callback(NewRemoteCode);
static VALUE wrap_receiver_receive(VALUE, VALUE, VALUE);

static void wrap_transmitter_free(NewRemoteTransmitter *);
static VALUE wrap_transmitter_allocate(VALUE);
static VALUE wrap_transmitter_initialize(int, VALUE *, VALUE);
static VALUE wrap_transmitter_send_group(VALUE, VALUE);
static VALUE wrap_transmitter_send_unit(VALUE, VALUE, VALUE);
static VALUE wrap_transmitter_send_group_dim(VALUE, VALUE);
static VALUE wrap_transmitter_send_unit_dim(VALUE, VALUE, VALUE);

static void new_remote_signal_handler(int s) {
	/* Restore previous signal handler on a SIGINT/SIGTERM. */
	signal(SIGINT, old_sig_int_handler);
	signal(SIGTERM, old_sig_term_handler);

	signal_received = true;
}

static void wrap_receiver_callback(NewRemoteCode code) {
	std::cout << "Address: " <<  code.address << ", ";

	if (code.groupBit) {
		std::cout << "group, ";
	} else {
		std::cout << "unit: " << static_cast<int>(code.unit) << ", ";
	}

	switch (code.switchType) {
	case NewRemoteCode::off:
		std::cout << "off, ";
		break;
	case NewRemoteCode::on:
		std::cout << "on, ";
		break;
	case NewRemoteCode::dim:
		std::cout << "dim, ";
		break;
	}

	if (code.dimLevelPresent) {
		std::cout << "dim level: " << static_cast<int>(code.dimLevel) << ", ";
	}

	std::cout << "period: " << code.period << "us." << std::endl;
}

static VALUE wrap_receiver_receive(VALUE self, VALUE pin, VALUE minRepeats) {
	NewRemoteReceiver::init(NUM2INT(pin), NUM2INT(minRepeats), wrap_receiver_callback);

	/* Wait forever until receiving SIGINT/SIGTERM. */
	while (!signal_received) {
		sleep(1);
	}

	return self;
}

static void wrap_code_free(NewRemoteCode *code) {
	ruby_xfree(code);
}

static VALUE wrap_code_allocate(VALUE klass) {
	return Data_Wrap_Struct(klass, NULL, wrap_code_free, ruby_xmalloc(sizeof(NewRemoteCode)));
}

static NewRemoteTransmitter *getNewRemoteTransmitter(VALUE self) {
        NewRemoteTransmitter *transmitter;

        Data_Get_Struct(self, NewRemoteTransmitter, transmitter);

	return transmitter;
}

static void wrap_transmitter_free(NewRemoteTransmitter *transmitter) {
	transmitter->~NewRemoteTransmitter();
	ruby_xfree(transmitter);
}

static VALUE wrap_transmitter_allocate(VALUE klass) {
	return Data_Wrap_Struct(klass, NULL, wrap_transmitter_free, ruby_xmalloc(sizeof(NewRemoteTransmitter)));
}

static VALUE wrap_transmitter_initialize(int argc, VALUE *argv, VALUE self) {
	NewRemoteTransmitter *transmitter = getNewRemoteTransmitter(self);
	VALUE address, pin, periodusec, repeats;

	rb_scan_args(argc, argv, "22", &address, &pin, &periodusec, &repeats);

	if (NIL_P(periodusec)) {
		periodusec = INT2NUM(260);
	}

	if (NIL_P(repeats)) {
		repeats = INT2NUM(4);
	}

	new(transmitter) NewRemoteTransmitter(NUM2INT(address), NUM2INT(pin), NUM2INT(periodusec), NUM2INT(repeats));

	return Qnil;
}

static VALUE wrap_transmitter_send_group(VALUE self, VALUE state) {
	NewRemoteTransmitter *transmitter = getNewRemoteTransmitter(self);

	transmitter->sendGroup(RTEST(state));

	return self;
}

static VALUE wrap_transmitter_send_unit(VALUE self, VALUE device, VALUE state) {
	NewRemoteTransmitter *transmitter = getNewRemoteTransmitter(self);

	transmitter->sendUnit(NUM2INT(device), RTEST(state));

	return self;
}

static VALUE wrap_transmitter_send_group_dim(VALUE self, VALUE dimLevel) {
	NewRemoteTransmitter *transmitter = getNewRemoteTransmitter(self);

	transmitter->sendGroupDim(NUM2INT(dimLevel));

	return self;
}

static VALUE wrap_transmitter_send_unit_dim(VALUE self, VALUE device, VALUE dimLevel) {
	NewRemoteTransmitter *transmitter = getNewRemoteTransmitter(self);

	transmitter->sendDim(NUM2INT(device), NUM2INT(dimLevel));

	return self;
}

extern "C" void Init_new_remote() {
	VALUE NewRemoteModule = rb_define_module("NewRemote");

        /* Receiver class definitions. */
	VALUE ReceiverClass = rb_define_class_under(NewRemoteModule, "Receiver", rb_cObject);

	rb_define_singleton_method(ReceiverClass, "receive", RUBY_METHOD_FUNC(wrap_receiver_receive), 2);

	/* Transmitter class definitions. */
	VALUE TransmitterClass = rb_define_class_under(NewRemoteModule, "Transmitter", rb_cObject);

	rb_define_alloc_func(TransmitterClass, wrap_transmitter_allocate);
	rb_define_method(TransmitterClass, "initialize", RUBY_METHOD_FUNC(wrap_transmitter_initialize), -1);
	rb_define_method(TransmitterClass, "send_group", RUBY_METHOD_FUNC(wrap_transmitter_send_group), 1);
	rb_define_method(TransmitterClass, "send_unit", RUBY_METHOD_FUNC(wrap_transmitter_send_unit), 2);
	rb_define_method(TransmitterClass, "send_group_dim", RUBY_METHOD_FUNC(wrap_transmitter_send_group_dim), 1);
	rb_define_method(TransmitterClass, "send_unit_dim", RUBY_METHOD_FUNC(wrap_transmitter_send_unit_dim), 2);

	if (wiringPiSetup() == -1) {
		std::cerr << "WiringPi setup failed." << std::endl;
		exit(1);
	}

	/* Catch SIGINT/SIGTERM with our own signal handler. */
	old_sig_int_handler = signal(SIGINT, new_remote_signal_handler);
	old_sig_term_handler = signal(SIGTERM, new_remote_signal_handler);
}
