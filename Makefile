# Makefile format based off of Apollo3 SDK https://github.com/sparkfun/AmbiqSuiteSDK

#### User Defines ####
LIBPATH ?= # Defaults to current directory
BOARD   ?= # Defaults to generic Cortex-M4

ifeq ($(LIBPATH),)
    LIBPATH = .
    $(warning warning: you have not defined LIBPATH so will continue assuming that the library root is at $(SDKPATH))
endif

ifeq ($(BOARD),)	## do something here...
    $(warning warning: no BOARD specified, will fall back to generic Cortex M-4)
endif


COMPILERNAME := gcc

# Global Defines
DEFINES=

# Includes
INCLUDES=
INCLUDES+= -I$(LIBPATH)/include

# VPATH (Paths where source files are located)
VPATH=
VPATH+= $(LIBPATH)/src

# Compilation Units (All .c files to compile)
SRC=
SRC+= armcm4_helpers.c
SRC+= matrix.c

# LIBS (Precompiled libraries to include in linker step)
LIBS=

#### Setup ####
TOOLCHAIN ?= arm-none-eabi
CPU        = cortex-m4
FPU        = fpv4-sp-d16
FABI       = hard

#### Required Executables ####
CC  = $(TOOLCHAIN)-gcc
GCC = $(TOOLCHAIN)-gcc
LD  = $(TOOLCHAIN)-ld
CP  = $(TOOLCHAIN)-objcopy
OD  = $(TOOLCHAIN)-objdump
RD  = $(TOOLCHAIN)-readelf
AR  = $(TOOLCHAIN)-ar
RL  = $(TOOLCHAIN)-ranlib


EXECUTABLES = CC LD CP OD AR RD GCC RL
K := $(foreach exec,$(EXECUTABLES),\
        $(if $(shell which $($(exec)) 2>/dev/null),,\
        $(info $(exec) not found on PATH ($($(exec))).)$(exec)))
$(if $(strip $(value K)),$(info Required Program(s) $(strip $(value K)) not found))

ifneq ($(strip $(value K)),)
all clean:
	$(info Tools $(TOOLCHAIN)-$(COMPILERNAME) not installed.)
	$(RM) -rf bin
else

# Change to create directory for different "board" types
BUILD := $(LIBPATH)/build
$(warning BUILD=$(BUILD))

#### Machinery ####
CSRC = $(filter %.c,$(SRC))

OBJS = $(CSRC:%.c=$(BUILD)/%.o)

DEPS = $(CSRC:%.c=$(BUILD)/%.d)

CFLAGS = -mthumb -mcpu=$(CPU) -mfpu=$(FPU) -mfloat-abi=$(FABI)
CFLAGS+= -ffunction-sections -fdata-sections -ggdb
CFLAGS+= -ffast-math -fgnu89-inline
CFLAGS+= -MMD -MP -Wall -g
CFLAGS+= -O3
CFLAGS+= $(DEFINES) 
CFLAGS+= $(INCLUDES)
CFLAGS+=

LFLAGS = -mthumb -mcpu=$(CPU) -mfpu=$(FPU) -mfloat-abi=$(FABI)
LFLAGS+= -nostartfiles -static
LFLAGS+= 

#### Targets / Rules ####
all: directories $(BUILD)/libmatrix.a

directories:
	@mkdir -p $(BUILD)


$(BUILD)/%.o: %.c
	@echo " Compiling $(COMPILERNAME) $<" ;\
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD)/%.o: %.s $(BUILD)/%.d
	@echo " Assembling $(COMPILERNAME) $<" ;\
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD)/libmatrix.a: $(OBJS)
	$(AR) rcs $@ $<
	$(RL) $@

clean:
	rm -rf $(BUILD)

# Automatically include any generated dependencies
-include $(DEPS) 
endif

.PHONY: directories clean