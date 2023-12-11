#!/bin/bash
#SBATCH -n 5
#SBATCH --time=00:10:00
#SBATCH --partition=normal

mpirun --allow-run-as-root ./exaustivaMPI