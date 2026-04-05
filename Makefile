CXX = /opt/homebrew/bin/g++-15
CXXFLAGS = -std=c++20 -fmodules-ts -Wall -g
EXEC = a.out
SOURCES = $(shell cat order.txt)
OBJECTS = ${SOURCES:.cpp=.o}

${EXEC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC}


.PHONY: clean

clean:
	rm ${OBJECTS} ${EXEC}

