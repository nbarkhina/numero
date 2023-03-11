make clean_android
$NDK_ROOT/ndk-build --no-print-directory -j$NUMPROC -C ./libnumero/libretro/jni APP_ABI=arm64-v8a
mv libnumero/libretro/libs/arm64-v8a/libretro.so ./numero_libretro_android.so