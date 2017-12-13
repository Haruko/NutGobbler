CC = gcc
FLAGS = -g

all: Shared.o IPC.o GameLogic.o Server.o Client.o LinkedList.o

Shared.o: Shared.c Shared.h
	${CC} Shared.c ${FLAGS} -c -o ./Out/Shared.o
	
LinkedList.o: ll.c ll.h
	${CC} ll.c ${FLAGS} -c -o ./Out/LinkedList.o

IPC.o: Shared.o IPC.c IPC.h
	${CC} IPC.c ${FLAGS} -c -o ./Out/IPC.o

GameLogic.o: Shared.o GameLogic.c GameLogic.h
	${CC} GameLogic.c ${FLAGS} -c -o ./Out/GameLogic.o

Client.o: IPC.o Shared.o Client.c Client.h
	${CC} Client.c ./Out/Shared.o ./Out/IPC.o ${FLAGS} -o ./Out/Client.o -lncursesw
	
Server.o: IPC.o Shared.o LinkedList.o GameLogic.o Server.c Server.h
	${CC} Server.c ./Out/Shared.o ./Out/IPC.o ./Out/GameLogic.o ./Out/LinkedList.o ${FLAGS} -o ./Out/Server.o -lpthread

clean:
	rm -rf *~ *.o *.dSYM ./Out/*.o
