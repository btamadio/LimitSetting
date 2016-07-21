#!/bin/bash
jobname=$1
mutoy=$2
muhypo=$3
j=$4
number=$5

mkdir -p /project/projectdirs/atlas/btamadio/RPV_SUSY/StatisticalAnalysis89/processing/statistics
rm -rf /project/projectdirs/atlas/btamadio/RPV_SUSY/StatisticalAnalysis89/processing/statistics/limit_toys_${jobname}_mutoy${mutoy}_muhypo${muhypo}_${j}${jobname}_${j}
mkdir /project/projectdirs/atlas/btamadio/RPV_SUSY/StatisticalAnalysis89/processing/statistics/limit_toys_${jobname}_mutoy${mutoy}_muhypo${muhypo}_${j}${jobname}_${j}
cd /project/projectdirs/atlas/btamadio/RPV_SUSY/StatisticalAnalysis89/processing/statistics/limit_toys_${jobname}_mutoy${mutoy}_muhypo${muhypo}_${j}${jobname}_${j}

shopt -s expand_aliases
source /common/atlas/scripts/setupATLAS.sh
setupATLAS
localSetupROOT
cp /project/projectdirs/atlas/btamadio/RPV_SUSY/StatisticalAnalysis89/bin/toy .
cp /project/projectdirs/atlas/btamadio/RPV_SUSY/StatisticalAnalysis89/combinedWS.root .

./toy ${mutoy} ${muhypo} ${j} ${number} >\dev\null

mkdir -p /project/projectdirs/atlas/btamadio/RPV_SUSY/StatisticalAnalysis89/toyoutput/${jobname}/
mv outcome_muSB_${muhypo}_mu_${mutoy}toy_${j}.root /project/projectdirs/atlas/btamadio/RPV_SUSY/StatisticalAnalysis89/toyoutput/${jobname}/

cd ;
rm -rf /project/projectdirs/atlas/btamadio/RPV_SUSY/StatisticalAnalysis89/processing/statistics/limit_toys_${jobname}_mutoy${mutoy}_muhypo${muhypo}_${j}${jobname}_${j} ;