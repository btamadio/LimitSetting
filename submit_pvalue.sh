#!/bin/bash
for j in `seq 1 60`; do
    qsub -l cvmfs=1 -l h_vmem=2G -l eliza18io=1 runtoy.sh toy_14p3fb_4jSR_MJ800_q0 0.0 0.0 $j 2000 ;
done;