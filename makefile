
###########################################################################
#  以下参数，用于生成test1.exe，文件比较小
########################################################################### 
CFLAGS  := \
	$(INCLUDE_PATHS) $(DEFINES) \
	-Wall -Wno-missing-braces \
	-Os \
	-ffunction-sections -fdata-sections \
	-mno-stack-arg-probe \
	#-fno-stack-check \
	#-fno-stack-protector \
	
LDFLAGS := \
	$(LINKER_PATHS) \
	-s \
	-Wl,--subsystem,windows \
	#-nostdlib \	

###########################################################################
#  以下参数，用于生成test.exe，文件大小为170k，可以正常运行；test1.exe则没有输出
########################################################################### 
CU_INCLUDE_PATH := /usr/local/include
CU_STATIC_LIB_PATH := /usr/local/lib
CU_STATIC_LIB += -lcunit -lwsock32

EXE_TARGET := auto_send_socket.exe
SRC_FILES := auto_send_socket_ver01.c

#$(LDFLAGS) $(CFLAGS)
##gcc $(CFLAGS) $(LDFLAGS) -I$(CU_INCLUDE_PATH) -o test1 test.c -L$(CU_STATIC_LIB_PATH) -l$(CU_STATIC_LIB)
all:
	gcc -I$(CU_INCLUDE_PATH) -o $(EXE_TARGET) $(SRC_FILES) -L$(CU_STATIC_LIB_PATH) $(CU_STATIC_LIB)
	
clean:
	rm -rf $(EXE_TARGET)


