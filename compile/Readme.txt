----------------20150819 release notes-------------------
1. Change makefile to perfectly support cygwin64 based on Windows platform(Windows 7 professional), be reminded firstly you need to add cygwin64 bin into PATH environment variables.
2. Use make command to generate DLL by following the below steps:
	(1). make clean
	(2). make JniMsmq.h
	(3). make .cpp.obj
	(4). make JNIMSMQ.dll
3. From this version jMSMQClient supports 64bit JVM, and the corresponding jdk is jdk1.6, you can compile yourself to adopt to jdk1.7 and above.

----------------first version release notes--------------
1. This is the original version, if you would like to modify the native layer source code, such as for adopting to 64bit JRE in particular.
2. To compile new native files, you need to install some environment firstly such as .NET or VS Studio, and then change the makefile to fix compile errors.