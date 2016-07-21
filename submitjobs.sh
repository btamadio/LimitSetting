for j in `seq 1 10`;
 do
   for i in `seq 150 180`;
     do mu=$(echo "scale=1;$i/2"|bc) ;
       qsub -l cvmfs=1 -l h_vmem=2G -l eliza18io=1 runtoy.sh toy_RPV_softjet_4jSR $mu $mu $j 2000 ;
       qsub -l cvmfs=1 -l h_vmem=2G -l eliza18io=1 runtoy.sh toy_RPV_softjet_4jSR 0.0 $mu $j 2000 ;
 done
done