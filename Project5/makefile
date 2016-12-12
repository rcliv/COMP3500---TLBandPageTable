CC = g++
FLAGS = -std=c++11 -g
OBJ = VirtualMemoryManager.o tlb.o

VirtualMemoryManager : ${OBJ}
	${CC} ${FLAGS} ${OBJ} -o VirtualMemoryManager

VirtualMemoryManager.o : VirtualMemoryManager.cpp
	${CC} ${FLAGS} -c VirtualMemoryManager.cpp

tlb.o : tlb.cpp tlb.hpp
	${CC} ${FLAGS} -c tlb.cpp

clean:
	rm *.o