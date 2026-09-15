// sherpa-onnx/csrc/token-script.cc
//
// Copyright (c)  2026  Apakabarlabs
#include "sherpa-onnx/csrc/token-script.h"

#include <cstdint>

#include "sherpa-onnx/csrc/text-utils.h"

namespace sherpa_onnx {

namespace {

struct Range {
  char32_t low;
  char32_t high;
};

// The word-boundary marker of a SentencePiece vocabulary. It says where a word
// starts rather than what it is written in, so it is passed over.
constexpr char32_t kWordBoundary = 0x2581;

constexpr Range kLatin[] = {
    {0x0020, 0x007F},  // ASCII
    {0x00A0, 0x00FF},  // Latin-1 Supplement
    {0x0100, 0x017F},  // Latin Extended-A
    {0x0180, 0x024F},  // Latin Extended-B
    {0x0300, 0x036F},  // Combining Diacritical Marks
    {0x1E00, 0x1EFF},  // Latin Extended Additional
};

constexpr Range kCyrillic[] = {
    {0x0400, 0x04FF},  // Cyrillic
    {0x0300, 0x036F},  // Combining Diacritical Marks
    {0x0020, 0x007F},  // ASCII punctuation, digits and spaces
};

constexpr Range kGreek[] = {
    {0x0370, 0x03FF},  // Greek and Coptic
    {0x1F00, 0x1FFF},  // Greek Extended
    {0x0300, 0x036F},  // Combining Diacritical Marks
    {0x0020, 0x007F},  // ASCII punctuation, digits and spaces
};

// Letters of the Latin alphabet, which the ranges above admit through ASCII and
// which a script that is not Latin must therefore turn away.
constexpr Range kLatinLetters[] = {
    {0x0041, 0x005A},
    {0x0061, 0x007A},
};

bool Within(const Range *ranges, int32_t count, char32_t c) {
  for (int32_t i = 0; i != count; ++i) {
    if (c >= ranges[i].low && c <= ranges[i].high) return true;
  }
  return false;
}

bool Admits(Script script, char32_t c) {
  switch (script) {
    case Script::kLatin:
      return Within(kLatin, sizeof(kLatin) / sizeof(kLatin[0]), c);
    case Script::kCyrillic:
      if (Within(kLatinLetters,
                 sizeof(kLatinLetters) / sizeof(kLatinLetters[0]), c)) {
        return false;
      }
      return Within(kCyrillic, sizeof(kCyrillic) / sizeof(kCyrillic[0]), c);
    case Script::kGreek:
      if (Within(kLatinLetters,
                 sizeof(kLatinLetters) / sizeof(kLatinLetters[0]), c)) {
        return false;
      }
      return Within(kGreek, sizeof(kGreek) / sizeof(kGreek[0]), c);
    case Script::kAny:
      return true;
  }
  return true;
}

}  // namespace

Script ScriptFromName(const std::string &name) {
  if (name == "latin") return Script::kLatin;
  if (name == "cyrillic") return Script::kCyrillic;
  if (name == "greek") return Script::kGreek;
  return Script::kAny;
}

bool TokenBelongsToScript(const std::string &token, Script script) {
  if (script == Script::kAny) return true;

  std::u32string text = Utf8ToUtf32(token);
  for (char32_t c : text) {
    if (c == kWordBoundary) continue;
    if (!Admits(script, c)) return false;
  }
  return true;
}

}  // namespace sherpa_onnx
