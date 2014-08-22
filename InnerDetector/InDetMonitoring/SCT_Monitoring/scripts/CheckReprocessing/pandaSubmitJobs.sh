#Set up DQ2
source /afs/cern.ch/project/gd/LCG-share/current/etc/profile.d/grid_env.sh
source /afs/cern.ch/atlas/offline/external/GRID/ddm/DQ2Clients/setup.sh

#Set up PANDA. Be happy.
source /afs/cern.ch/atlas/offline/external/GRID/DA/panda-client/latest/etc/panda/panda_setup.sh

#Make a voms proxy
voms-proxy-init -voms atlas --valid 24:0

#Stream definition
STREAM="express_express"

#Find out your grid name
GRID_NAME=`python /afs/cern.ch/atlas/offline/external/GRID/ddm/Operations/utils/dn.py`

#Loop over all runs
for RUN in `cat config/RunList.txt`;
do
	#Get the full names of the data sets
	NEW_DATASET=`dq2-ls "d*$RUN*$STREAM*merge.HIST*1093*_p94_p94/"`
	OLD_DATASET=`dq2-ls "d*$RUN*$STREAM*merge.HIST.r9*/"`

	#Find out the number of files in the old data set
	FILE_NUMBER=`dq2-list-files $OLD_DATASET | wc -l`

	#dq2-list-files adds 4 lines of blurb after listing the files
	let FILE_NUMBER-=4

	#Submit grid job with PandaRun
	#PLEASE NOTE: tha --athenaTag option is somehow required to allow linking of ROOT libraries. ATHENA is not used, thank god.
	prun --exec "./bin/histogramCompare -new %IN -old %IN2" --bexec "make" --outDS user10.$GRID_NAME.sctDataQuality$RUN --inDS $NEW_DATASET --outputs comparison.root --secondaryDSs IN2:$FILE_NUMBER:$OLD_DATASET --athenaTag=14.2.24
done
