// sherpa-onnx/csrc/token-script.h
//
// Copyright (c)  2026  Apakabarlabs
#ifndef SHERPA_ONNX_CSRC_TOKEN_SCRIPT_H_
#define SHERPA_ONNX_CSRC_TOKEN_SCRIPT_H_

#include <string>

namespace sherpa_onnx {

enum class Script {
  kAny = 0,
  kLatin,
  kCyrillic,
  kGreek,
};

// Returns kAny for an empty name and for a name no script answers to.
Script ScriptFromName(const std::string &name);

// Returns true when every character of the token is written in that script.
// Characters that belong to no script, such as digits, spaces and punctuation,
// are accepted by every script, and so is a token left with none of its own.
// kAny accepts everything.
bool TokenBelongsToScript(const std::string &token, Script script);

}  // namespace sherpa_onnx

#endif  // SHERPA_ONNX_CSRC_TOKEN_SCRIPT_H_
