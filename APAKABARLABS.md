# What apakabarlabs changes in sherpa-onnx

This is apakabarlabs' fork of [k2-fsa/sherpa-onnx](https://github.com/k2-fsa/sherpa-onnx).
`master` stays the same as upstream's; our changes live on `keep-to-the-script`, and the
apps and libraries of apakabarlabs take sherpa-onnx from builds of that branch only.

## Keeping a NeMo transducer to one alphabet

A multilingual model such as Parakeet TDT v3 keeps one vocabulary for every language it
knows, so nothing in it stops a word of one language from coming back in the letters of
another. On `keep-to-the-script` a NeMo transducer can be told the alphabet the speech is
read in, and its greedy search then takes the best token written in that alphabet.

- `OfflineRecognizerConfig.script` names it: `latin`, `cyrillic`, `greek`, or empty for
  none. The C++, Python, JNI, Java and Kotlin configurations all carry it.
- `token-script.h` decides which alphabet a token is written in. The alphabet tables and
  the rule come from [FluidAudio](https://github.com/FluidInference/FluidAudio), Apache 2.0.
- `OfflineRecognizerTransducerNeMoImpl` turns the alphabet into a mask over the vocabulary
  that the greedy search adds to the scores. A token in angle brackets, such as `<blk>`,
  stays available whatever the alphabet.
- `SetConfig` on a NeMo transducer changes the alphabet of a loaded recogniser: it rebuilds
  the greedy search with the new mask and leaves the model and every other setting as
  they are. A caller does not decode while it runs.
- The JNI result carries the log probability of every token (`ysLogProbs`), as the C++,
  Python and C results already did, so an app can report a confidence.

## Packaging

- The Android library ships the ProGuard rule its native code needs: it reads the
  configuration classes and builds the result classes by name over JNI.
- Every build is named after its commit, `<upstream version>.post1+keeptothescript.<commit>`
  (`-keeptothescript.<commit>` where `+` cannot stand), so two builds of the branch never
  answer to one version.

## Workflows

- `apakabarlabs-build.yaml` builds, on every push to `keep-to-the-script`, the Android
  library for arm64-v8a and armeabi-v7a, the JVM archives a check on a Mac hears with,
  and Python wheels for Linux and macOS, and leaves them on a release named
  `keep-to-the-script-<commit>`.
- `apakabarlabs-packages.yaml`, dispatched by hand for a commit, puts that release's Android
  library and JVM archives into this repository's GitHub Packages as
  `fm.apakabar:sherpa-onnx`, `fm.apakabar:sherpa-onnx-jvm` and
  `fm.apakabar:sherpa-onnx-native-lib-osx-aarch64`, without building them again, and checks
  that each published POM names its own coordinates.
- `apakabarlabs-follow-upstream.yaml` keeps `master` equal to upstream's and merges it into
  `keep-to-the-script` every day, so that upstream moving under our change shows as a
  failed run. Upstream is merged in, never rebased onto: others read the branch.
  - It pushes with the secret `FOLLOW_UPSTREAM_TOKEN`, a fine-grained token for this
    repository only, with Contents and Workflows read and write: a run's own
    `GITHUB_TOKEN` may not push a commit that changes `.github/workflows`, and upstream
    changes theirs.
  - The token expires on **2027-10-15**; the organisation allows no more than 366 days.
    Before then the owner issues a new one and replaces the secret, or the daily run
    fails on its first push.
  - A push with this token starts `apakabarlabs-build.yaml`, so every day upstream moves
    our branch is built as well.

## Checks

`make test` builds and runs the tests of our changes: `token-script-test`, and
`offline-recognizer-transducer-nemo-script-test`, which hears the English recording that
comes with `sherpa-onnx-nemo-parakeet-tdt-0.6b-v3-int8`. It fetches that model unless
`MODEL` names an unpacked copy.
