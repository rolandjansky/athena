#! /usr/bin/env sh 
###########################################################################
#   Lxplus submission simulation script for MuonOffline Monitoring
#   Nectarios.Chr.Benekos (Illinois)
###########################################################################
source monvariables.sh
########################################################################## 
 
export jobdir=$sample
export logdir=$HOME/logdir/$process/$datatype/$rel/$jobdir/'log'
mkdir -p $logdir
#--- the above variables must (!!! TO BE ADJUSTED !!!)  
########################################################################## 
export submit='bsub -q 1nd -R "select[cpuf>0.7]"' 
export scriptfilename=muondqamonitoring.sh
 
##16-->is the totalnumevents/2K events per job
#for (( i=1; i < 2; i++ )); do 
for i in 1; do
num=$(printf "%.5d" $i)
((num1=($i-1)*$skip))
 
export logfilename=$jobdir.'_'$num'.log'
echo "Running $prefix.00$dstnum.$type" 
echo "==============================================="
echo "submitting job: " $logfilename
echo "directory of log file:" $logdir
echo "just for debugging- filenum - skipevents" _$num $num1
sleep 10
$submit -o $logdir/$logfilename $scriptfilename _$num $num1
echo "==============================================="
done 
