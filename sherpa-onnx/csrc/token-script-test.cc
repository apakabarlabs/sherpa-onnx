// sherpa-onnx/csrc/token-script-test.cc
//
// Copyright (c)  2026  Apakabarlabs

#include "sherpa-onnx/csrc/token-script.h"

#include <string>

#include "gtest/gtest.h"
#include "sherpa-onnx/csrc/text-utils.h"

namespace sherpa_onnx {

namespace {

std::string FromCodepoints(const std::u32string &points) {
  return Utf32ToUtf8(points);
}

}  // namespace

TEST(TokenScript, EachAlphabetTakesItsOwnLetters) {
  EXPECT_TRUE(TokenBelongsToScript("cat", Script::kLatin));
  EXPECT_TRUE(TokenBelongsToScript("кот", Script::kCyrillic));
  EXPECT_TRUE(TokenBelongsToScript("λόγος", Script::kGreek));
}

TEST(TokenScript, NoAlphabetTakesTheLettersOfAnother) {
  EXPECT_FALSE(TokenBelongsToScript("cat", Script::kCyrillic));
  EXPECT_FALSE(TokenBelongsToScript("CAT", Script::kCyrillic));
  EXPECT_FALSE(TokenBelongsToScript("cat", Script::kGreek));
  EXPECT_FALSE(TokenBelongsToScript("CAT", Script::kGreek));
  EXPECT_FALSE(TokenBelongsToScript("кот", Script::kLatin));
  EXPECT_FALSE(TokenBelongsToScript("кот", Script::kGreek));
  EXPECT_FALSE(TokenBelongsToScript("λόγος", Script::kLatin));
}

TEST(TokenScript, ALatinLetterAmongCyrillicOnesIsStillLatin) {
  EXPECT_FALSE(TokenBelongsToScript("миrу", Script::kCyrillic));
  EXPECT_FALSE(TokenBelongsToScript("cat кот", Script::kLatin));
}

TEST(TokenScript, TheWordBoundaryMarkerSaysNothingAboutAnAlphabet) {
  EXPECT_TRUE(TokenBelongsToScript("\xE2\x96\x81increase", Script::kLatin));
  EXPECT_FALSE(TokenBelongsToScript("\xE2\x96\x81Инкрис", Script::kLatin));
  EXPECT_TRUE(TokenBelongsToScript("\xE2\x96\x81", Script::kLatin));
  EXPECT_TRUE(TokenBelongsToScript("\xE2\x96\x81", Script::kCyrillic));
  EXPECT_TRUE(
      TokenBelongsToScript("\xE2\x96\x81\xE2\x96\x81", Script::kGreek));
}

TEST(TokenScript, WhatBelongsToNoAlphabetIsRefusedByNone) {
  EXPECT_TRUE(TokenBelongsToScript("", Script::kGreek));
  EXPECT_TRUE(TokenBelongsToScript(" ", Script::kLatin));
  EXPECT_TRUE(TokenBelongsToScript(".", Script::kCyrillic));
  EXPECT_TRUE(TokenBelongsToScript("42", Script::kGreek));
  EXPECT_TRUE(TokenBelongsToScript("don't", Script::kLatin));
  EXPECT_TRUE(TokenBelongsToScript("мир,", Script::kCyrillic));
}

TEST(TokenScript, AMarkBelongsToTheLetterItSitsOn) {
  EXPECT_TRUE(TokenBelongsToScript(FromCodepoints(U"é"),
                                   Script::kLatin));
  EXPECT_TRUE(TokenBelongsToScript("замо́к", Script::kCyrillic));
  EXPECT_TRUE(TokenBelongsToScript(FromCodepoints(U"С́"),
                                   Script::kCyrillic));
}

TEST(TokenScript, EveryRangeIsNamedByBothItsEnds) {
  EXPECT_TRUE(TokenBelongsToScript(FromCodepoints(U" "), Script::kLatin));
  EXPECT_TRUE(TokenBelongsToScript(FromCodepoints(U""), Script::kLatin));
  EXPECT_FALSE(TokenBelongsToScript(FromCodepoints(U""), Script::kLatin));
  EXPECT_FALSE(TokenBelongsToScript(FromCodepoints(U""), Script::kLatin));
  EXPECT_TRUE(TokenBelongsToScript(FromCodepoints(U" "), Script::kLatin));
  EXPECT_TRUE(TokenBelongsToScript(FromCodepoints(U"ÿ"), Script::kLatin));
  EXPECT_TRUE(TokenBelongsToScript(FromCodepoints(U"Ā"), Script::kLatin));
  EXPECT_TRUE(TokenBelongsToScript(FromCodepoints(U"ſ"), Script::kLatin));
  EXPECT_TRUE(TokenBelongsToScript(FromCodepoints(U"ƀ"), Script::kLatin));
  EXPECT_TRUE(TokenBelongsToScript(FromCodepoints(U"ɏ"), Script::kLatin));
  EXPECT_FALSE(TokenBelongsToScript(FromCodepoints(U"ɐ"), Script::kLatin));
  EXPECT_TRUE(TokenBelongsToScript(FromCodepoints(U"̀"), Script::kLatin));
  EXPECT_TRUE(TokenBelongsToScript(FromCodepoints(U"ͯ"), Script::kLatin));
  EXPECT_FALSE(TokenBelongsToScript(FromCodepoints(U"Ͱ"), Script::kLatin));
  EXPECT_TRUE(TokenBelongsToScript(FromCodepoints(U"Ḁ"), Script::kLatin));
  EXPECT_TRUE(TokenBelongsToScript(FromCodepoints(U"ỿ"), Script::kLatin));

  EXPECT_TRUE(
      TokenBelongsToScript(FromCodepoints(U"Ѐ"), Script::kCyrillic));
  EXPECT_TRUE(
      TokenBelongsToScript(FromCodepoints(U"ӿ"), Script::kCyrillic));
  EXPECT_FALSE(
      TokenBelongsToScript(FromCodepoints(U"Ͽ"), Script::kCyrillic));
  EXPECT_FALSE(
      TokenBelongsToScript(FromCodepoints(U"Ԁ"), Script::kCyrillic));
  EXPECT_TRUE(
      TokenBelongsToScript(FromCodepoints(U"@"), Script::kCyrillic));
  EXPECT_TRUE(
      TokenBelongsToScript(FromCodepoints(U"["), Script::kCyrillic));
  EXPECT_TRUE(
      TokenBelongsToScript(FromCodepoints(U"`"), Script::kCyrillic));
  EXPECT_TRUE(
      TokenBelongsToScript(FromCodepoints(U"{"), Script::kCyrillic));

  EXPECT_TRUE(TokenBelongsToScript(FromCodepoints(U"Ͱ"), Script::kGreek));
  EXPECT_TRUE(TokenBelongsToScript(FromCodepoints(U"Ͽ"), Script::kGreek));
  EXPECT_TRUE(TokenBelongsToScript(FromCodepoints(U"ͩ"), Script::kGreek));
  EXPECT_TRUE(TokenBelongsToScript(FromCodepoints(U"ἀ"), Script::kGreek));
  EXPECT_TRUE(TokenBelongsToScript(FromCodepoints(U"῿"), Script::kGreek));
  EXPECT_FALSE(TokenBelongsToScript(FromCodepoints(U" "), Script::kGreek));
}

TEST(TokenScript, AnAlphabetNobodyNamedTakesEverything) {
  EXPECT_EQ(ScriptFromName(""), Script::kAny);
  EXPECT_EQ(ScriptFromName("runic"), Script::kAny);
  EXPECT_TRUE(TokenBelongsToScript("кот", ScriptFromName("")));
  EXPECT_EQ(ScriptFromName("latin"), Script::kLatin);
  EXPECT_EQ(ScriptFromName("cyrillic"), Script::kCyrillic);
  EXPECT_EQ(ScriptFromName("greek"), Script::kGreek);
}

}  // namespace sherpa_onnx
