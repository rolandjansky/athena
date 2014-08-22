#Load the command line arguments
NEW_DATASET_SEARCH=${1}
OLD_DATASET_SEARCH=${2}
RUN_NUMBER=${3}
BINARY_LOCATION=${4}/bin/checkReprocessing
CONFIG_LOCATION=${4}/config/HistogramLoad.txt
RESULT_LOCATION=${4}/results

#Set up DQ2
export X509_USER_PROXY=${5}
source /afs/cern.ch/atlas/offline/external/GRID/ddm/DQ2Clients/setup.sh

#Make a temporary location and store the datasets there
JOB_NAME=${USER}SCTDQ${RUN_NUMBER}
mkdir /tmp/${USER}/${JOB_NAME}
cd /tmp/${USER}/${JOB_NAME}
dq2-get ${NEW_DATASET_SEARCH}/
dq2-get ${OLD_DATASET_SEARCH}/
NEW_DATASET=`ls -d ${NEW_DATASET_SEARCH}*`
OLD_DATASET=`ls -d ${OLD_DATASET_SEARCH}*`

#Run the comparison
${BINARY_LOCATION} -newDir ${NEW_DATASET} -oldDir ${OLD_DATASET} -config ${CONFIG_LOCATION} >& ${RUN_NUMBER}.log

#Save the results
mv ${RUN_NUMBER}.log ${RESULT_LOCATION}
mv comparison.root ${RESULT_LOCATION}/${RUN_NUMBER}.root
rm -rf /tmp/${USER}/${JOB_NAME}
