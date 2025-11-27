# Compiladores
CC      = gcc
MPICC   = mpicc

# Flags
CFLAGS  = -O2 -Wall
LIBS    = -lm

# Arquivos
SRCDIR  = src
BUILDDIR = build

# Fontes
SERIAL_SRC  = $(SRCDIR)/simulacao_serial.c
MPI_SRC     = $(SRCDIR)/simulacao_mpi.c
OMP_SRC     = $(SRCDIR)/simulacao_omp.c

# Binários
SERIAL_BIN  = $(BUILDDIR)/sim_serial
MPI_BIN     = $(BUILDDIR)/sim_mpi
OMP_BIN     = $(BUILDDIR)/sim_omp

# Número de processos MPI
N_PROCS = 6

all: $(SERIAL_BIN) $(MPI_BIN) $(OMP_BIN)

# ---- SERIAL ----
$(SERIAL_BIN): $(SERIAL_SRC)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(SERIAL_SRC) -o $(SERIAL_BIN) $(LIBS)

# ---- MPI ----
$(MPI_BIN): $(MPI_SRC)
	@mkdir -p $(BUILDDIR)
	$(MPICC) $(CFLAGS) $(MPI_SRC) -o $(MPI_BIN) $(LIBS)

# ---- OMP ----
$(OMP_BIN): $(OMP_SRC)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -fopenmp $(OMP_SRC) -o $(OMP_BIN) $(LIBS)

# ---- RUN ----
serial: $(SERIAL_BIN)
	./$(SERIAL_BIN)

mpi: $(MPI_BIN)
	mpirun -np $(N_PROCS) ./$(MPI_BIN)

omp: $(OMP_BIN)
	mpirun -np $(N_PROCS) ./$(MPI_BIN)

clean:
	rm -rf $(BUILDDIR)
