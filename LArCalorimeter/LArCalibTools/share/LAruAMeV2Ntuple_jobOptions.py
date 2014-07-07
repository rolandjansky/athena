#########################################################
# Job options to dump the LArADC2MeV factor to a ntuple.#
#########################################################


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

theApp.Dlls += ["LArCalibTools"]

theApp.TopAlg += [ "LAruA2MeV2Ntuple" ]

theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='uA2MEV.root' OPT='NEW'" ]

AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel=ERROR

theApp.EvtMax=1
MessageSvc.OutputLevel=INFO

DetStore=Service("DetectorStore");
DetStore.dump=TRUE

