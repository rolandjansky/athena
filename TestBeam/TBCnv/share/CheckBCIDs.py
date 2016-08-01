# Location & number of raw data file:
#runnumber=1000613
runnumber= 1000536
#runnumber = 1000855
RawDataDir="/castor/cern.ch/atlas/testbeam/combined/2004/"
#RawDataDir="/data/calo"
RawDataFilePrefix="daq_SFI-51_calo"

#include( "AthenaCommon/Atlas.UnixStandardJob.py" )


include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )
# File location:
ByteStreamInputSvc=Service("ByteStreamInputSvc")
ByteStreamInputSvc.InputDirectory += [RawDataDir]
ByteStreamInputSvc.FilePrefix += [RawDataFilePrefix]
ByteStreamInputSvc.RunNumber = [runnumber]

theApp.Dlls+=["TBCnv"]
theApp.TopAlg+=["TBCheckBCIDs"]
TBCheckBCIDs=Algorithm("TBCheckBCIDs")
TBCheckBCIDs.OutputLevel=DEBUG


# -- use root histos --
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
#NTOutputFileString="FILE1 DATAFILE='%(file)s' OPT='NEW'" % {"file" : NTOutputFileName}
NTupleSvc.Output = [ "FILE1 DATAFILE='BCIDS.root' OPT='NEW'" ]
#NTupleSvc.Output = [NTOutputFileString]
#NTupleSvc.OutputLevel=DEBUG

# Number of events to be processed (default is 10)
theApp.EvtMax = 100

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = ERROR

# Don't print event number each event
#AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
#AthenaEventLoopMgr.OutputLevel = WARNING





