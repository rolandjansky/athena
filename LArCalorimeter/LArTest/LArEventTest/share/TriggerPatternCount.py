
#Get run number to monitor.
        echo "Enter run number to monitor:\c"
        read RUNNO

DAQDIR=`source FindRunDir.sh ${RUNNO}`
FILEPREFIX=`source FindRunPrefix.sh ${RUNNO}`

echo "Using run: ${RUNNO}"
echo "Directory:  ${DAQDIR}"
echo "File Prefix: ${FILEPREFIX}"

# And let user specify the number of events to monitor.
echo "Enter number of events to monitor [20000]: \c"
read EVTNO
if [[ ${EVTNO} = "" ]] then 
  EVTNO=20000
fi

runnumber=4462
RawDataDir="/castor/cern.ch/atlas/testbeam/lar/2004/"
RawDataFilePrefix="daq_ROS-41_LargCalib"
theApp.EvtMax = 200

# ***************************************************************
include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )
ByteStreamInputSvc=Service("ByteStreamInputSvc")
ByteStreamInputSvc.InputDirectory +=[RawDataDir]
# Prefix #
ByteStreamInputSvc.FilePrefix  += [RawDataFilePrefix]
# Run number
ByteStreamInputSvc.RunNumber += [runnumber]
include( "TBCnv/TBReadH8BS_jobOptions.py" )
theApp.Dlls += [ "LArEventTest"]
theApp.topAlg+=["TriggerPatternCount"]
TriggerPatternCount=Algorithm("TriggerPatternCount")
TriggerPatternCount.ContainerKey = "FREE"
EventSelector = Service("EventSelector")
EventSelector.SkipEvents = 1
