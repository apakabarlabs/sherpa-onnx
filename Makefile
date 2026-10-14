# The checks of what keep-to-the-script adds to upstream; see APAKABARLABS.md.
#
# The NeMo transducer test hears a real recording, so it needs the model sherpa-onnx
# publishes as sherpa-onnx-nemo-parakeet-tdt-0.6b-v3-int8. `make test` fetches it into
# the build directory; MODEL names a copy unpacked elsewhere instead.

BUILD ?= build
PARAKEET := sherpa-onnx-nemo-parakeet-tdt-0.6b-v3-int8
MODEL ?= $(BUILD)/models/$(PARAKEET)
TESTS := token-script-test offline-recognizer-transducer-nemo-script-test

.PHONY: test model

test: model
	cmake -S . -B $(BUILD) -D SHERPA_ONNX_ENABLE_TESTS=ON -D CMAKE_BUILD_TYPE=Release -D BUILD_SHARED_LIBS=ON
	cmake --build $(BUILD) --target $(TESTS) -j 8
	$(BUILD)/bin/token-script-test
	SHERPA_ONNX_NEMO_TRANSDUCER_DIR="$(abspath $(MODEL))" $(BUILD)/bin/offline-recognizer-transducer-nemo-script-test

model:
	@test -f "$(MODEL)/tokens.txt" || { \
		mkdir -p "$(BUILD)/models" && \
		curl -L --fail --show-error \
			https://github.com/k2-fsa/sherpa-onnx/releases/download/asr-models/$(PARAKEET).tar.bz2 \
			-o "$(BUILD)/models/$(PARAKEET).tar.bz2" && \
		tar xjf "$(BUILD)/models/$(PARAKEET).tar.bz2" -C "$(BUILD)/models" && \
		rm "$(BUILD)/models/$(PARAKEET).tar.bz2"; }
	@test -f "$(MODEL)/test_wavs/en.wav" || { echo "$(MODEL) has no test_wavs/en.wav" >&2; exit 1; }
