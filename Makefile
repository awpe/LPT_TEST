SRC			= ParallelPort.cpp main.cpp
TARGET		= ParallelPortTest
CXX			= g++
CXXFLAGS	= -ansi -Wall
LIBS		= -lGL -lglut

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LIBS) -o $(TARGET)

clean:
	rm -rf $(TARGET)
