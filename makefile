
###########################################################################
#  ���²�������������test1.exe���ļ��Ƚ�С
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
#  ���²�������������test.exe���ļ���СΪ170k�������������У�test1.exe��û�����
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


