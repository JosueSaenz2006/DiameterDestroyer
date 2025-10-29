CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

TARGET = solution
SRC = solution.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

test: $(TARGET)
	@echo "Running test1..."
	@./$(TARGET) < test1.txt
	@echo "\nRunning test2..."
	@./$(TARGET) < test2.txt
	@echo "\nRunning test3 (negative weights)..."
	@./$(TARGET) < test3.txt
	@echo "\nRunning test_encrypted..."
	@./$(TARGET) < test_encrypted.txt
	@echo "\nRunning test_complex..."
	@./$(TARGET) < test_complex.txt
	@echo "\nRunning test_comprehensive..."
	@./$(TARGET) < test_comprehensive.txt
	@echo "\nAll tests passed!"

clean:
	rm -f $(TARGET)

.PHONY: all test clean
