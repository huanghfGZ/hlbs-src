
###############################
# Copyright (C) Anny Wang.
# Copyright (C) Hupu, Inc.
###############################

#
# Makefile
# 动态链接所需库，需.so文件安装到ldconfig加载路径中(/usr/local/lib)
# 需要预先编译lib目录下的log4cpp|tinyxml软件包
#

ROOTPATH=\"/usr/local/webserver/hlbs/agentserver/bin\"

CXX=g++
CXXFLAGS=-std=c++11 -O3 -Wall -Wno-invalid-offsetof -DPREFIX=$(ROOTPATH) -DUSE_LOG4CPP -DLINUX -D_DEBUG_ #-D_MEM_CHECK
LDFLAGS=-L/usr/local/lib -ltinyxml -llog4cpp -lpthread

BASEPATH=../base
COMMPATH=../command
LDPATH=../lib
INCPATH=-I./$(BASEPATH) -I./$(COMMPATH) -I./$(LDPATH)/tinyxml -I/usr/local/include/log4cpp

SRCPATH=./src
OBJECTS=./$(BASEPATH)/wLog.o \
		./$(BASEPATH)/wMisc.o \
		./$(BASEPATH)/wShm.o \
		./$(BASEPATH)/wShmtx.o \
		./$(BASEPATH)/wFile.o \
		./$(BASEPATH)/wSignal.o \
		./$(BASEPATH)/wProcTitle.o \
		./$(BASEPATH)/wMsgQueue.o \
		./$(BASEPATH)/wMemPool.o \
		./$(BASEPATH)/wThread.o \
		./$(BASEPATH)/wSocket.o \
		./$(BASEPATH)/wPing.o \
		./$(BASEPATH)/wChannel.o \
		./$(BASEPATH)/wTcpSocket.o \
		./$(BASEPATH)/wUnixSocket.o \
		./$(BASEPATH)/wTask.o \
		./$(BASEPATH)/wChannelTask.o \
		./$(BASEPATH)/wWorker.o \
		./$(BASEPATH)/wWorkerIpc.o \
		\
		./$(SRCPATH)/SvrQos.o \
		./$(SRCPATH)/AgentMaster.o \
		./$(SRCPATH)/AgentClientTask.o \
		./$(SRCPATH)/AgentServerTask.o \
		./$(SRCPATH)/AgentUnixTask.o \
		./$(SRCPATH)/AgentServer.o \
		./$(SRCPATH)/AgentConfig.o \
		./$(SRCPATH)/AgentMain.o \
		./$(SRCPATH)/DetectThread.o

TARGET=agentsvrd

.PHONY:all clean install

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

%.o:%.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c $< -o $@

%.d:%.cpp
	@set -e; rm -f $@; $(CXX) -MM $< $(INCPATH) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(OBJECTS:.o=.d)

clean:
	-rm -f $(BASEPATH)/*.o $(BASEPATH)/*.d $(BASEPATH)/*.d.*
	-rm -f $(TARGET) $(SRCPATH)/*.o $(SRCPATH)/*.d $(SRCPATH)/*.d.*
	-rm -rf ./bin ./log

install:
	-rm -rf ./bin ./log
	-mkdir ./bin ./log
	-mv $(TARGET) ./bin
