#Set up root
export ROOTSYS=/afs/cern.ch/sw/lcg/external/root/5.18.00/slc4_amd64_gcc34/root
export PATH=${ROOTSYS}/bin:${PATH}
export LD_LIBRARY_PATH=${ROOTSYS}/lib:${LD_LIBRARY_PATH}

#Compile the code
make
mkdir results

#Make a voms proxy and store it somewhere safe
source /afs/cern.ch/project/gd/LCG-share/current/etc/profile.d/grid_env.sh
voms-proxy-init -voms atlas
PROXY_COPY=${HOME}/CopyOfGridProxy
cp ${X509_USER_PROXY} ${PROXY_COPY}

#Stream definition
STREAM="express_express"

#Loop over all runs
for RUN_NUMBER in `cat config/RunList.txt`;
do
	#Define the data set search strings
	NEW_DATASET_SEARCH="d*${RUN_NUMBER}*${STREAM}*merge.HIST*1093*_p94_p94"
	OLD_DATASET_SEARCH="d*${RUN_NUMBER}*${STREAM}*merge.HIST.r9*"
	JOB_NAME=${USER}SCTDQ${RUN_NUMBER}

	#Submit the job to lxbatch
	bsub -J $JOB_NAME -q 1nh singleJob.sh ${NEW_DATASET_SEARCH} ${OLD_DATASET_SEARCH} ${RUN_NUMBER} ${PWD} ${PROXY_COPY}
done
