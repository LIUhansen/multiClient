# 说明将此文件名改为 makefile
# 库文件放在  include 文件中
# makefile 放在 C C++ 文件所在的目录中 
#

 CC = gcc

# CC 定义编译器
CCFLAGS += -I.
CCFLAGS += -I..
CCFLAGS +=  
CCFLAGS += -lpthread
# 多线程用

 LDFLAGS += -O -g
# 为arm-elf-gcc 添加的 elf 转 FLT 


#CCFLAGS += -Iinclude
# include 是多C C++ 文件所包含的头文件目录(H文件)
# CCFLAGES 是编译选项


LINKOPTS = 
#LINKOPTS = -Llib
# LIB库目录,在PPP目录下

LIBFILES=
#LIBFILES=lib/fiplib.a

# LIBFILES=ppp/ppp.a \
#					kkk/kkk,a\

# LIB库文件所在目录下的文件


CFILES +=  main.c    multiClient.c


# CFILES += demo.c \

# CFILES 是多个C文件

CPPFILES +=

# CPPFILES += ppp.cpp \
# CPPFILES 是多个CPP文件




OBJ += $(CFILES:.c=.o) $(CPPFILES:.c=.o) 

# OBJ 是多个C C++ 文件对应的O文件

EXE = S90Agent
# EXE是我们要生成的目标文件.



all:   $(EXE)
$(EXE)	:	$(OBJ) $(LIBFILES)
	$(CC)   $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(CCFLAGS) $(LDFLAGS) $(LINKOPTS)  $(LIBFILES)  -o $@

clean:
	rm -f  *.o *.gdb $(EXE)
# 清除生成的文件	

.SUFFIXES: .cpp  .c .o
# 通用的.cpp .c .o 文件


.c.o:
	$(CC) -O -o $*.o -c   $(CCFLAGS)   $*.c	

.cpp.o:
	$(C++) -c -o $*.o  $(CCFLAGS) $*.cpp	



	
