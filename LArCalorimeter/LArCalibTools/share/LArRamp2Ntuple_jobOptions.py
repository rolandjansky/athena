##############################################
# Job options to dump the Ramps to a ntuple. #
##############################################

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.em_setOn()

from AthenaCommon.GlobalFlags  import GlobalFlags
GlobalFlags.DetGeo.set_ctbh8()
GlobalFlags.DataSource.set_data()
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

include( "LArDetDescr/LArDetDescr_H8_joboptions.py" )
#include ("LArIdCnv/LArIdCnv_joboptions.py")
#include ("LArConditionsCommon/LArIdMap_H8_jobOptions.py")

EventSelector = Service( "EventSelector" )
EventSelector.RunNumber=1000942
EventSelector.EventsPerRun=1
EventSelector.FirstEvent=1

LArTB04FolderTag_Calib     = "TB04-Default"
LArTB04FolderTag_Pedestal  = "TB04-Default"
LArTB04FolderTag_Ramp      = "TB04-Default"
LArTB04FolderTag_OFC_RTM   = "TB04-Default"
LArTB04FolderTag_OFC_TCM   = "TB04-Default"

include ( "LArConditionsCommon/LArConditionsCommon_H8_jobOptions.py")

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

from LArCalibTools.LArCalibToolsConf import LArRamps2Ntuple
LArRamps2Ntuple=LArRamps2Ntuple("LArRamps2Ntuple")
LArRamps2Ntuple.ContainerKey = ["HIGH","MEDIUM","LOW"]
LArRamps2Ntuple.RawRamp=False
LArRamps2Ntuple.SaveAllSamples=False

topSequence+= LArRamps2Ntuple


theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='LArRamp.root' OPT='NEW'" ]

AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel=ERROR

theApp.EvtMax=1
svcMgr.MessageSvc.OutputLevel=DEBUG

#DetStore=Service("DetectorStore");
#DetStore.dump=TRUE

