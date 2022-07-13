CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3
LDFLAGS = -lpthread
TARGET = task_test

all: $(TARGET)

$(TARGET): main.o task.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.o task.o $(LDFLAGS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

task.o: task.cpp
	$(CXX) $(CXXFLAGS) -c task.cpp

clean:
	$(RM) $(TARGET)
