#one line command to submit job
# submit to queue $1 script $2.sh with optional arguments $3 $4 $5 $6 $7 $8
# job name and output is built from $2 $3 $4 $5 $6
# requires -R " mem > 600 " to run on machine with more of 400 Mb memory
# examples 
# subm.sh 8nm athenarec prod1 simul_001003 1 10
#==> bsub -q 8nm -o athenarec_prod1_simul_001003_1_10 -Jathenarec_prod1_simul_001003_1_10 athenarec.sh prod1 simul 001003 1 10
# subm.sh 8nm athenarec  3
#==> bsub -q 8nm -o athenarec_3 -Jathenarec_3 athenarec.sh 3
set -x
n=$#
if [ $n -eq 2 ] ; then
   NAME=$2
elif [ $n -eq 3 ] ; then
   NAME=$2_$3
elif [ $n -eq 4 ] ; then
   NAME=$2_$3_$4
elif [ $n -eq 5 ] ; then
   NAME=$2_$3_$4_$5
elif [ $n -ge 6 ] ; then
   NAME=$2_$3_$4_$5_$6
elif [ $n -ge 7 ] ; then
   NAME=$2_$3_$4_$5_$6_$7
elif [ $n -ge 7 ] ; then
   NAME=$2_$3_$4_$5_$6_$7_$8
fi
bsub -R " mem > 600 " -q $1 -o $NAME -J$NAME $2.sh $3 $4 $5 $6 $7 $8


