.SUFFIXES: .cs .java

# ============================================
# Java settings
#JAVADIR=c:\sunjdk1.5
JAVADIR=C:/Java/jdk1.6.0

# ============================================
# C settings
#
#VC7=C:\Program Files\Microsoft Visual C++ Toolkit 2003\
#VC7=C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7
#VC7= c:\progra~1\MICROS~1.NET\vc7
#VC= c:\vc7
VC="C:/Program Files/Microsoft SDKs/Windows/v6.0/VC"
#PSDKDIR=c:\progra~1\Microsoft SDK
#PSDKDIR=c:\progra~1\PlatformSDK
PSDKDIR="C:/Program Files/Microsoft SDKs/Windows/v6.0"
#PSDKDIR=$(VC)\PlatformSDK

LIBPATH=LIB
BINPATH=BIN
LIBS= mqrt.lib


CFLAGS= /EHsc /MT /I$(JAVADIR)\include /I$(JAVADIR)\include\win32 /I$(PSDKDIR)\include /I$(VC)\include /W4

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
	javah -classpath . ionic.Msmq.Queue
	copy ionic_Msmq_Queue.h  ..\JniMsmq.h
	cd ..

$(OBJ): JniMsmq.h  MsmqQueue.hpp

.cpp.obj:
	C:\Program Files\Microsoft SDKs\Windows\v6.0\VC\Bin>cl /c  /EHsc /MT /I C:/Java/jdk1.6.0/include /I C:/Java/jdk1.6.0/include/win32 /I "C:/Program Files/Microsoft SDKs/Windows/v6.0"/Include /I "C:/Program Files/Microsoft SDKs/Windows/v6.0/VC"/include /W4 *.cpp

build\ionic\Msmq\Queue.class: Queue.java  build\ionic\Msmq\Message.class build\ionic\Msmq\MessageQueueException.class
	javac -classpath build -g -d build Queue.java 

build\ionic\Msmq\Message.class: Message.java 
	-mkdir build
	javac -g -d build Message.java 

build\ionic\Msmq\MessageQueueException.class: MessageQueueException.java 
	-mkdir build
	javac -g -d build MessageQueueException.java 

build\TestClient.class: TestClient.java TestRunner.java JNIMSMQ.dll
	-mkdir build
	javac -g -classpath build -d build TestClient.java TestRunner.java

