include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

theApp.Dlls += [ "GaudiAud" ]
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors =  [ "ChronoAuditor" ]

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

#newFcal = True
newFcal = False

if newFcal:
    DetDescrVersion = "ATLAS-CSC-02-00-00"
else:    
    DetDescrVersion = "ATLAS-CSC-01-00-00"


include( "LArIdCnv/LArIdCnv_joboptions.py" )
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
include( "CaloConditions/LArTTCellMap_ATLAS_jobOptions.py" )

# ............ declare the used top algo.
# --------------------------------------------------------
#load relevant libraries
theApp.Dlls += [ "TestLArHardwareID" ]
theApp.TopAlg += [ "testTTMap_Algo"]

testTTMap_Algo = Algorithm( "testTTMap_Algo" )
testTTMap_Algo.dumpMap = True

#CondProxyProvider = Service( "CondProxyProvider" )
#DetDescrCnvSvc = Service( "DetDescrCnvSvc" )


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 2
MessageSvc.debugLimit      = 200000
MessageSvc.useColors   = TRUE

theApp.EvtMax = 1
EventSelector = Service( "EventSelector" )
EventSelector.RunNumber=1




