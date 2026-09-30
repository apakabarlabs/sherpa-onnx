# The native library reads the configuration classes field by field and builds the
# result classes through their constructors, all by name over JNI, so a shrinker that
# renames or drops any of them breaks recognition at run time.
-keep class com.k2fsa.sherpa.onnx.** { *; }
