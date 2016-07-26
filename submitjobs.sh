for j in `seq 1 10`;
 do
   for i in `seq 12 75`;
     do mu=$(echo "scale=1;$i/1"|bc) ;
       qsub -l cvmfs=1 -l h_vmem=2G -l eliza18io=1 runtoy.sh toy_14p3fb_5jSR_MJ600 $mu $mu $j 2000 ;
       qsub -l cvmfs=1 -l h_vmem=2G -l eliza18io=1 runtoy.sh toy_14p3fb_5jSR_MJ600 0.0 $mu $j 2000 ;
 done
done