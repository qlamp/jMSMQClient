.SUFFIXES: .cs .java

# ============================================
# Java settings
JAVADIR=C:/Java/jdk1.6.0
JAVAC=$(JAVADIR)/bin/javac.exe

# ============================================
# C settings
#
#VC7=C:\Program Files\Microsoft Visual C++ Toolkit 2003\
#VC7=C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7
#VC7= c:\progra~1\MICROS~1.NET\vc7
#VC= c:\vc7
VC="C:\Program Files\Microsoft SDKs\Windows\v6.0\VC"
#PSDKDIR=c:\progra~1\Microsoft SDK
#PSDKDIR=c:\progra~1\PlatformSDK
PSDKDIR="C:\Program Files\Microsoft SDKs\Windows\v6.0"
#PSDKDIR=$(VC)\PlatformSDK

LIBPATH=LIB/x64
BINPATH=Bin/x64
LIBS= mqrt.lib


CFLAGS= /EHsc /MT /I $(JAVADIR)/include /I $(JAVADIR)/include/win32 /I $(PSDKDIR)/include /I $(VC)/INCLUDE /W4

CC = $(VC)/$(BINPATH)/cl.exe
LINK=$(VC)/$(BINPATH)/link.exe

SRC = \
	MsmqQueueNativeMethods.cpp \
	MsmqQueue.cpp

OBJ = $(SRC:.cpp=.obj)


# ============================================
# build targets
#
default: JNIMSMQ.dll build\TestClient.class

all: JNIMSMQ.dll

JNIMSMQ.dll:	$(OBJ)
	$(LINK) /DLL /libpath:$(PSDKDIR)/$(LIBPATH)  /libpath:$(VC)/$(LIBPATH) /OUT:JNIMSMQ.dll $(OBJ) $(LIBS)

JniMsmq.h: build\ionic\Msmq\Queue.class
	cd  build
	-rm ionic_Msmq_Queue.h
	$(JAVADIR)/bin/javah -classpath . ionic.Msmq.Queue
	cp ionic_Msmq_Queue.h  JniMsmq.h
	cd ..

$(OBJ): JniMsmq.h  MsmqQueue.hpp

.cpp.obj:
	$(CC) /c $(CFLAGS) *.cpp

build\ionic\Msmq\Queue.class: Queue.java  build\ionic\Msmq\Message.class build\ionic\Msmq\MessageQueueException.class
	$(JAVAC) -classpath build -g -d build Queue.java 

build\ionic\Msmq\Message.class: Message.java 
	-mkdir build
	$(JAVAC) -g -d build Message.java 

build\ionic\Msmq\MessageQueueException.class: MessageQueueException.java 
	$(JAVAC) -g -d build MessageQueueException.java 

build\TestClient.class: TestClient.java TestRunner.java JNIMSMQ.dll
	-mkdir build
	$(JAVAC) -g -classpath build -d build TestClient.java TestRunner.java
clean: 
	-rm -r build
	-rm -f $(OBJ) JNIMSMQ.dll JNIMSMQ.lib JNIMSMQ.exp JniMsmq.h
