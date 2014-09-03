#submit a set of job using subm.sh
export PRODNAME=prod1
export DATASET=001003
subm.sh 8nh athenarec $PRODNAME simul $DATASET 1  10
subm.sh 8nh athenarec $PRODNAME simul $DATASET 11 20
subm.sh 8nh athenarec $PRODNAME simul $DATASET 21 30
subm.sh 8nh athenarec $PRODNAME simul $DATASET 31 40
subm.sh 8nh athenarec $PRODNAME simul $DATASET 41 50


