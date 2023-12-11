#!/bin/bash
#SBATCH -n 3
#SBATCH --time=00:10:00
#SBATCH --partition=normal

mpirun --allow-run-as-root ./exaustivaMPI