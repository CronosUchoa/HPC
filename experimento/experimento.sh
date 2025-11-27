#!/bin/bash

LOG="resultados_experimentos.txt"
MPI_PROCS=6

# Listas de instâncias
LISTA_N=("1000" "2000" "10000")
LISTA_STEPS=("1000" "10000" "20000")

echo "=== EXPERIMENTOS DE SIMULAÇÃO MD ===" > $LOG
echo "Data: $(date)" >> $LOG
echo "-----------------------------------" >> $LOG

# Compila tudo
make clean
make

for N in "${LISTA_N[@]}"; do
    for STEPS in "${LISTA_STEPS[@]}"; do
        
        echo "" >> $LOG
        echo "-------------------------------------------" >> $LOG
        echo "N = $N | NSTEPS = $STEPS" >> $LOG
        echo "-------------------------------------------" >> $LOG
        
        # Atualiza definicoes.h automaticamente
        sed -i "s/^#define N .*/#define N $N/"   include/definicoes.h
        sed -i "s/^#define NSTEPS .*/#define NSTEPS $STEPS/" include/definicoes.h

        # Recompila após alteração
        make -s
        
        echo "[RUN SERIAL] N=$N | NSTEPS=$STEPS"
        TEMPO_SERIAL=$(./build/sim_serial | grep "Tempo total" | awk '{print $3}')
        
        echo "[RUN MPI] N=$N | NSTEPS=$STEPS"
        TEMPO_MPI=$(mpirun -np $MPI_PROCS ./build/sim_mpi | grep "Tempo total" | awk '{print $3}')

        # Salva no log
        echo "Serial: ${TEMPO_SERIAL}s" >> $LOG
        echo "MPI (${MPI_PROCS} processos): ${TEMPO_MPI}s" >> $LOG

        # Speedup
        SPEEDUP=$(echo "$TEMPO_SERIAL / $TEMPO_MPI" | bc -l)
        echo "Speedup MPI: $SPEEDUP" >> $LOG
    done
done

echo "" >> $LOG
echo "=== EXPERIMENTOS FINALIZADOS ===" >> $LOG
