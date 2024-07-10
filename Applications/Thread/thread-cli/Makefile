default:
	mkdir -p build
	cd build && cmake -G 'Unix Makefiles' -DOT_COMPILE_WARNING_AS_ERROR=ON -DQORVO_PLATFORM=qpg6105 -DCMAKE_BUILD_TYPE=MinSizeRel -DOT_PLATFORM=external -DOT_SLAAC=ON -DCMAKE_TOOLCHAIN_FILE=../src/qpg6105/arm-none-eabi.cmake ../../../..
	cd build && make ot-cli-ftd
	arm-none-eabi-objcopy -O ihex "build/bin/ot-cli-ftd" "build/bin/qpg6105-ot-cli-ftd.hex"