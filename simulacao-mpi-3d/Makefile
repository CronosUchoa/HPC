CC = mpicc
CXX = mpicxx
CFLAGS = -O2 -Wall -Iinclude
CXXFLAGS = $(CFLAGS)
LIBS = -lSDL2 -lGL -lGLEW -lGLU -lm

SRCDIR = src
INCDIR = include
BUILDDIR = build
TEST_TARGET = $(BUILDDIR)/teste
TARGET = $(BUILDDIR)/sim
TEST_OBJ = $(BUILDDIR)/teste.o

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

N_PROCS = 6

$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILDDIR)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@


$(TEST_TARGET): $(TEST_OBJ)
	$(CXX) $(TEST_OBJ) -o $(TEST_TARGET) $(LIBS)

$(BUILDDIR)/teste.o: $(SRCDIR)/teste.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)


run: $(TARGET)
	mpirun -np $(N_PROCS) ./$(TARGET)

visu: $(TARGET)
	mpirun -np $(N_PROCS) ./$(TEST_TARGET)

