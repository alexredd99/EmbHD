CFLAGS = -static -mcpu=cortex-m4 -O3 -ffast-math -ggdb -fgnu89-inline -mthumb -mfpu=fpv4-sp-d16

libmatrix.a: armcm4_helpers.o  matrix.o
	arm-none-eabi-ar rcs libmatrix.a armcm4_helpers.o matrix.o
	arm-none-eabi-ranlib libmatrix.a
matrix.o: src/matrix.c
	arm-none-eabi-gcc $(CFLAGS) -c src/matrix.c
armcm4_helpers.o: src/armcm4_helpers.c 
	arm-none-eabi-gcc  $(CFLAGS) -c src/armcm4_helpers.c 



.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

-include $(DEPS)
