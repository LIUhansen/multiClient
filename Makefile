# ˵�������ļ�����Ϊ makefile
# ���ļ�����  include �ļ���
# makefile ���� C C++ �ļ����ڵ�Ŀ¼�� 
#

 CC = gcc

# CC ���������
CCFLAGS += -I.
CCFLAGS += -I..
CCFLAGS +=  
CCFLAGS += -lpthread
# ���߳���

 LDFLAGS += -O -g
# Ϊarm-elf-gcc ��ӵ� elf ת FLT 


#CCFLAGS += -Iinclude
# include �Ƕ�C C++ �ļ���������ͷ�ļ�Ŀ¼(H�ļ�)
# CCFLAGES �Ǳ���ѡ��


LINKOPTS = 
#LINKOPTS = -Llib
# LIB��Ŀ¼,��PPPĿ¼��

LIBFILES=
#LIBFILES=lib/fiplib.a

# LIBFILES=ppp/ppp.a \
#					kkk/kkk,a\

# LIB���ļ�����Ŀ¼�µ��ļ�


CFILES +=  main.c    multiClient.c


# CFILES += demo.c \

# CFILES �Ƕ��C�ļ�

CPPFILES +=

# CPPFILES += ppp.cpp \
# CPPFILES �Ƕ��CPP�ļ�




OBJ += $(CFILES:.c=.o) $(CPPFILES:.c=.o) 

# OBJ �Ƕ��C C++ �ļ���Ӧ��O�ļ�

EXE = S90Agent
# EXE������Ҫ���ɵ�Ŀ���ļ�.



all:   $(EXE)
$(EXE)	:	$(OBJ) $(LIBFILES)
	$(CC)   $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(CCFLAGS) $(LDFLAGS) $(LINKOPTS)  $(LIBFILES)  -o $@

clean:
	rm -f  *.o *.gdb $(EXE)
# ������ɵ��ļ�	

.SUFFIXES: .cpp  .c .o
# ͨ�õ�.cpp .c .o �ļ�


.c.o:
	$(CC) -O -o $*.o -c   $(CCFLAGS)   $*.c	

.cpp.o:
	$(C++) -c -o $*.o  $(CCFLAGS) $*.cpp	



	
