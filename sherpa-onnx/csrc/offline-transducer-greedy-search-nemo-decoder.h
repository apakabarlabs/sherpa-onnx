// sherpa-onnx/csrc/offline-transducer-greedy-search-nemo-decoder.h
//
// Copyright (c)  2024  Xiaomi Corporation

#ifndef SHERPA_ONNX_CSRC_OFFLINE_TRANSDUCER_GREEDY_SEARCH_NEMO_DECODER_H_
#define SHERPA_ONNX_CSRC_OFFLINE_TRANSDUCER_GREEDY_SEARCH_NEMO_DECODER_H_

#include <vector>

#include "sherpa-onnx/csrc/offline-transducer-decoder.h"
#include "sherpa-onnx/csrc/offline-transducer-nemo-model.h"

namespace sherpa_onnx {

class OfflineTransducerGreedySearchNeMoDecoder
    : public OfflineTransducerDecoder {
 public:
  // `outside_script` holds one value per token of the vocabulary: 0 for a token
  // written in the alphabet the speech is read in, and -infinity for a token
  // written in another. It is added to the scores before the best token is
  // taken, so the best of the alphabet's own tokens is taken instead. An empty
  // vector leaves every token of the vocabulary available.
  OfflineTransducerGreedySearchNeMoDecoder(
      OfflineTransducerNeMoModel *model, float blank_penalty, bool is_tdt,
      std::vector<float> outside_script = {})
      : model_(model),
        blank_penalty_(blank_penalty),
        is_tdt_(is_tdt),
        outside_script_(std::move(outside_script)) {}

  std::vector<OfflineTransducerDecoderResult> Decode(
      Ort::Value encoder_out, Ort::Value encoder_out_length,
      OfflineStream **ss = nullptr, int32_t n = 0) override;

 private:
  OfflineTransducerNeMoModel *model_;  // Not owned
  float blank_penalty_;
  bool is_tdt_;
  std::vector<float> outside_script_;
};

}  // namespace sherpa_onnx

#endif  // SHERPA_ONNX_CSRC_OFFLINE_TRANSDUCER_GREEDY_SEARCH_NEMO_DECODER_H_
