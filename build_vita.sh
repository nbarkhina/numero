make -f Makefile.libretro platform=vita
mv numero_libretro_vita.a ../RetroArch/libretro_vita.a
cd ../RetroArch
rm retroarch_vita.elf
rm retroarch_vita.elf.unstripped.elf
rm retroarch_vita.vpk
rm numero_libretro.self
make -f Makefile.vita
mv retroarch_vita.self numero_libretro.self