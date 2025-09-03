include ../../common/commands.mk
.DEFAULT_GOAL := build

executables := \
	target/base64_cpp_clang_crypto \
	base64.rs/target/release/base64

artifacts := $(executables)

all_runners := $(patsubst %,run[%], $(artifacts))

# Build

.PHONY: build
build: $(artifacts) $(XTIME)

$(XTIME): ../../xtime.cpp
	$(XTIME_BUILD)

target/base64_cpp_clang_crypto: test-crypto.cpp | target libnotify
	$(CLANG_CPP_BUILD) -lcrypto

base64-rs-toml := base64.rs/Cargo.toml
.PHONY: base64.rs/target/release/base64
base64.rs/target/release/base64: $(base64-rs-toml)
	$(CARGO_BUILD)

# Run
.PHONY: run
run: $(all_runners)

## Common recipe for all runners
.PHONY: $(all_runners)
$(all_runners)::
	$(ECHO_RUN)

## Runners
executable_runners := $(patsubst %,run[%], $(executables))
$(executable_runners):: run[%] : %
	$(EXECUTABLE_RUN)

# Utilities
.PHONY: clean
clean:
	cargo clean --manifest-path $(base64-rs-toml)
	-rm -rf target