#
# Makefile
# WARNING: relies on invocation setting current working directory to Makefile location
# This is done in .vscode/task.json
#
IS_SIMULATOR		:= 0
PROJECT 			?= n3_box_lvgl_app
MAKEFLAGS 			:= -j $(shell nproc)
SRC_EXT      		:= c
OBJ_EXT				:= o
ifeq (${IS_SIMULATOR}, 1)
	CC				?= gcc
else
	CC 				:= /home/wicevi/N3_WorkSpace/F1C_ENV/buildroot-2023.02.6/output/host/bin/arm-buildroot-linux-musleabi-gcc
endif

SRC_DIR				:= ./
WORKING_DIR			:= ./build
BUILD_DIR			:= $(WORKING_DIR)/obj
BIN_DIR				:= $(WORKING_DIR)/bin

WARNINGS 			:= -Wall -Wextra \
						-Wshadow -Wundef -Wmaybe-uninitialized -Wmissing-prototypes -Wno-discarded-qualifiers \
						-Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized \
						-Wno-unused-parameter -Wno-missing-field-initializers -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default  \
					  	-Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated  \
						-Wempty-body -Wshift-negative-value -Wstack-usage=2048 \
            			-Wtype-limits -Wsizeof-pointer-memaccess -Wpointer-arith

CFLAGS 				:= -O0 -g $(WARNINGS)

# Add simulator define to allow modification of source
DEFINES				:= -D SIMULATOR=${IS_SIMULATOR} -D LV_BUILD_TEST=0

#user
# N3_APP_DIR = /home/wicevi/N3_WorkSpace/lv_sim_vscode_sdl/n3_app
# USER_INC_DIR = $(addprefix -I ,$(dir $(shell find $(N3_APP_DIR) -name "*.h")))
USER_INC_DIR		:=

# Include simulator inc folder first so lv_conf.h from custom UI can be used instead
INC 				:= -I./lv_examples/ -I./lv_drivers -I./lvgl/ -I./
INC 				+= $(USER_INC_DIR)
LDLIBS	 			:= -lm -lpthread 
ifeq (${IS_SIMULATOR}, 1)
	LDLIBS			+= -lSDL2
endif
BIN 				:= $(BIN_DIR)/${PROJECT}

COMPILE				= $(CC) $(CFLAGS) $(INC) $(DEFINES)

# Automatically include all source files
SRCS 				:= $(shell find $(SRC_DIR) -type f -name '*.c' -not -path '*/\.*')
OBJECTS    			:= $(patsubst $(SRC_DIR)%,$(BUILD_DIR)/%,$(SRCS:.$(SRC_EXT)=.$(OBJ_EXT)))

all: default

$(BUILD_DIR)/%.$(OBJ_EXT): $(SRC_DIR)/%.$(SRC_EXT)
	@echo 'Building: $<'
	@mkdir -p $(dir $@)
	@$(COMPILE) -c -o "$@" "$<"

default: $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	@$(CC) -o $(BIN) $(OBJECTS) $(LDFLAGS) ${LDLIBS}

clean:
	rm -rf $(WORKING_DIR)

install: ${BIN}
	adb push ${BIN} /usr/bin/
