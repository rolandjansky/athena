#Set up root
source /afs/cern.ch/sw/lcg/external/gcc/4.3.2/x86_64-slc5/setup.sh
export ROOTSYS=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.28.00/x86_64-slc5-gcc43-opt/root
export PATH=${ROOTSYS}/bin:${PATH}
export LD_LIBRARY_PATH=${ROOTSYS}/lib:${LD_LIBRARY_PATH}

#Compile the code
make
BINARY_LOCATION=${PWD}/bin/histogramCompare
CONFIG_LOCATION=${PWD}/config/HistogramLoad.txt
RESULT_LOCATION=${PWD}/results
mkdir ${RESULT_LOCATION}

#Set up DQ2
source /afs/cern.ch/project/gd/LCG-share/current/etc/profile.d/grid_env.sh
source /afs/cern.ch/atlas/offline/external/GRID/ddm/DQ2Clients/setup.sh

#Make a voms proxy
voms-proxy-init -voms atlas

#Stream definition
STREAM="express_express"

#Loop over all runs
for RUN in `cat config/RunList.txt`;
do
	#Define the data set search strings
	NEW_DATASET_SEARCH="d*${RUN}*${STREAM}*merge.HIST*1774*_p308_p308"
	OLD_DATASET_SEARCH="d*${RUN}*${STREAM}*merge.HIST*1761*_p308_p308"
    #NEW_DATASET_SEARCH="d*${RUN}*${STREAM}*merge.HIST*1647*_p308_p308"
    #OLD_DATASET_SEARCH="d*${RUN}*${STREAM}*merge.HIST*1608*_p288_p288"

	#Make a temporary location and store the datasets there
	mkdir /tmp/${USER}/${JOB_NAME}
	cd /tmp/${USER}/${JOB_NAME}
	dq2-get ${NEW_DATASET_SEARCH}/
	dq2-get ${OLD_DATASET_SEARCH}/
	NEW_DATASET=`ls -d ${NEW_DATASET_SEARCH}*`
	OLD_DATASET=`ls -d ${OLD_DATASET_SEARCH}*`

	#Run the comparison
	${BINARY_LOCATION} -newDir ${NEW_DATASET} -oldDir ${OLD_DATASET} -config ${CONFIG_LOCATION} >& ${RUN}.log

	#Save the results
	mv ${RUN}.log ${RESULT_LOCATION}
	mv comparison.root ${RESULT_LOCATION}/${RUN}.root
	rm -rf /tmp/${USER}/${JOB_NAME}
done
