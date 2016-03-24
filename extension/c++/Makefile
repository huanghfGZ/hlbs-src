
###############################
# Copyright (C) Anny.
# Copyright (C) Disvr, Inc.
###############################

CXX=g++

BASEPATH=./base
CXXFLAGS =-g -std=c++11 -fPIC -O2 -pipe -fno-ident -Wno-invalid-offsetof

INCPATH=-I./$(BASEPATH)
LIBS=-shared

TARGET=./debug/libagent_api

OBJECTS=./debug/agent_api.o \
		./debug/wIO.o \
		./debug/wMsgQueue.o \
		./debug/wMisc.o \
		./debug/wSocket.o \
		./debug/wTask.o \
		./debug/wShm.o

all: $(TARGET).so

$(TARGET).so:$(OBJECTS)
	@echo "Compiling $@"
	$(CXX) $^ -o $@ $(LIBS)

./debug/agent_api.o: agent_api.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

./debug/wIO.o: $(BASEPATH)/wIO.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

./debug/wMsgQueue.o: $(BASEPATH)/wMsgQueue.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

./debug/wMisc.o: $(BASEPATH)/wMisc.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

./debug/wSocket.o: $(BASEPATH)/wSocket.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

./debug/wTask.o: $(BASEPATH)/wTask.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

./debug/wShm.o: $(BASEPATH)/wShm.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

clean:
	-rm -rf ./debug
	
dir:
	-mkdir ./debug
	chmod 755 -R ./debug
