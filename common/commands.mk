COMMON_FLAGS := -Wall -Wextra -pedantic -Wcast-align -O3 -march=native -flto=auto -Wa,-mbranches-within-32B-boundaries
CLANG_FLAGS := $(COMMON_FLAGS)
LIBNOTIFY_FLAGS := -I../common/libnotify ../common/libnotify/target/libnotify.a
RUSTC_FLAGS := -C target-cpu=native -C llvm-args=--x86-branches-within-32B-boundaries

CLANG_CPP_BUILD =	clang++ $(CLANG_FLAGS) -std=c++23 -stdlib=libc++ -o $@ $^ $(LIBNOTIFY_FLAGS)
RUSTC_BUILD =		rustc $(RUSTC_FLAGS) -C opt-level=3 -C lto -C codegen-units=1 -C panic=abort -C strip=symbols -o $@ $^
RUST_CLIPPY =		clippy-driver -o $@.clippy $^

define CARGO_BUILD =
cargo fmt --manifest-path $<
cargo clippy -q --manifest-path $<
RUSTFLAGS="$(RUSTC_FLAGS)" cargo build -q --manifest-path $< --release
endef

ECHO_RUN = @tput bold; echo "$(MAKE) $@"; tput sgr0
XTIME := ../xtime.rb

EXECUTABLE_RUN =	$(XTIME) $^

.PHONY: libnotify
libnotify:
	$(MAKE) -C ../common/libnotify

rs_fmt := target/.rs_fmt
$(rs_fmt): *.rs | target
	rustfmt $^
	@touch $@

target:
	mkdir -p target