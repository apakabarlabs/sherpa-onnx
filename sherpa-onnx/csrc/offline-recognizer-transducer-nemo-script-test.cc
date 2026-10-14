// sherpa-onnx/csrc/offline-recognizer-transducer-nemo-script-test.cc
//
// Copyright (c)  2026  Apakabarlabs

#include <cstdlib>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "sherpa-onnx/csrc/offline-recognizer.h"
#include "sherpa-onnx/csrc/wave-reader.h"

namespace sherpa_onnx {

namespace {

// The directory sherpa-onnx-nemo-parakeet-tdt-0.6b-v3-int8 unpacks to, with the
// English recording that comes with it in test_wavs.
constexpr const char *kModelDirectory = "SHERPA_ONNX_NEMO_TRANSDUCER_DIR";

OfflineRecognizerConfig Config(const std::string &dir,
                               const std::string &script) {
  OfflineRecognizerConfig config;
  config.model_config.transducer.encoder_filename = dir + "/encoder.int8.onnx";
  config.model_config.transducer.decoder_filename = dir + "/decoder.int8.onnx";
  config.model_config.transducer.joiner_filename = dir + "/joiner.int8.onnx";
  config.model_config.tokens = dir + "/tokens.txt";
  config.model_config.model_type = "nemo_transducer";
  config.model_config.num_threads = 2;
  config.decoding_method = "greedy_search";
  config.script = script;
  return config;
}

bool HasLatinLetter(const std::string &text) {
  for (char c : text) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return true;
  }
  return false;
}

class NeMoTransducerScript : public ::testing::Test {
 protected:
  void SetUp() override {
    const char *dir = std::getenv(kModelDirectory);
    ASSERT_NE(dir, nullptr) << "set " << kModelDirectory
                            << " to an unpacked "
                               "sherpa-onnx-nemo-parakeet-tdt-0.6b-v3-int8";
    dir_ = dir;
    bool ok = false;
    samples_ = ReadWave(dir_ + "/test_wavs/en.wav", &rate_, &ok);
    ASSERT_TRUE(ok) << "could not read " << dir_ << "/test_wavs/en.wav";
  }

  std::string Heard(const OfflineRecognizer &recognizer) const {
    auto stream = recognizer.CreateStream();
    stream->AcceptWaveform(rate_, samples_.data(), samples_.size());
    recognizer.DecodeStream(stream.get());
    return stream->GetResult().text;
  }

  std::string HeardFresh(const std::string &script) const {
    OfflineRecognizer fresh(Config(dir_, script));
    return Heard(fresh);
  }

  void Name(OfflineRecognizer *recognizer, const std::string &script) const {
    OfflineRecognizerConfig config = recognizer->GetConfig();
    config.script = script;
    recognizer->SetConfig(config);
  }

  std::string dir_;
  std::vector<float> samples_;
  int32_t rate_ = 0;
};

}  // namespace

TEST_F(NeMoTransducerScript, AnAlphabetNamedLaterHearsAsOneNamedAtTheStart) {
  OfflineRecognizer recognizer(Config(dir_, "latin"));
  std::string latin = Heard(recognizer);
  ASSERT_TRUE(HasLatinLetter(latin)) << latin;

  Name(&recognizer, "cyrillic");

  EXPECT_EQ(recognizer.GetConfig().script, "cyrillic");
  std::string cyrillic = Heard(recognizer);
  EXPECT_FALSE(HasLatinLetter(cyrillic)) << cyrillic;
  EXPECT_EQ(cyrillic, HeardFresh("cyrillic"));
}

TEST_F(NeMoTransducerScript, NamingNoAlphabetLeavesEveryTokenAvailableAgain) {
  OfflineRecognizer recognizer(Config(dir_, "cyrillic"));

  Name(&recognizer, "");

  EXPECT_EQ(recognizer.GetConfig().script, "");
  EXPECT_EQ(Heard(recognizer), HeardFresh(""));
}

TEST_F(NeMoTransducerScript, NamingTheSameAlphabetAgainChangesNothing) {
  OfflineRecognizer recognizer(Config(dir_, "latin"));
  std::string before = Heard(recognizer);

  Name(&recognizer, "latin");

  EXPECT_EQ(Heard(recognizer), before);
}

TEST_F(NeMoTransducerScript, OnlyTheAlphabetIsTakenFromANewConfig) {
  OfflineRecognizer recognizer(Config(dir_, "latin"));
  OfflineRecognizerConfig loaded = recognizer.GetConfig();
  OfflineRecognizerConfig asked = Config(dir_, "cyrillic");
  asked.decoding_method = "modified_beam_search";
  asked.feat_config.feature_dim = loaded.feat_config.feature_dim + 1;

  recognizer.SetConfig(asked);

  OfflineRecognizerConfig now = recognizer.GetConfig();
  EXPECT_EQ(now.script, "cyrillic");
  EXPECT_EQ(now.decoding_method, loaded.decoding_method);
  EXPECT_EQ(now.feat_config.feature_dim, loaded.feat_config.feature_dim);
  EXPECT_EQ(Heard(recognizer), HeardFresh("cyrillic"));
}

}  // namespace sherpa_onnx
