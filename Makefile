libmatrix.a: armcm4_helpers.o  matrix.o
	arm-none-eabi-ar rcs libmatrix.a armcm4_helpers.o matrix.o
	arm-none-eabi-ranlib libmatrix.a
matrix.o: src/matrix.c include/matrix.h
	arm-none-eabi-gcc -static -mcpu=cortex-m4 -O3 -ggdb -fgnu89-inline -c include/matrix.h src/matrix.c
armcm4_helpers.o: src/armcm4_helpers.c include/armcm4_helpers.h
	arm-none-eabi-gcc -static -mcpu=cortex-m4 -O3 -ffast-math -ggdb -fgnu89-inline -c src/armcm4_helpers.c include/armcm4_helpers.h



.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

-include $(DEPS)
