include ../../common/commands.mk
.DEFAULT_GOAL := build

executables := \
	target/swap_bits_cpp_clang \
	swapbits.rs/target/release/swapbits

artifacts := $(executables)

all_runners := $(patsubst %,run[%], $(artifacts))

# Build

.PHONY: build
build: $(artifacts) $(XTIME)

$(XTIME): ../../xtime.cpp
	$(XTIME_BUILD)

target/swap_bits_cpp_clang: swap_bits.cpp | target libnotify
	$(CLANG_CPP_BUILD)

swapbits-rs-toml := swapbits.rs/Cargo.toml
.PHONY: swapbits.rs/target/release/swapbits
swapbits.rs/target/release/swapbits: $(swapbits-rs-toml)
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
	cargo clean --manifest-path $(swapbits-rs-toml)
	-rm -rf target