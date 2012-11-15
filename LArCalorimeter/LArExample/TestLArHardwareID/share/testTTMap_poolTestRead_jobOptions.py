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

include( "LArIdCnv/LArIdCnv_joboptions.py" )
# following line only needed to avoid error messages
# in LArCablingSvc initialisation (LArCablingSvc subsequently unused)
#include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
include( "CaloCondAthenaPool/CaloTTMap_Pool_Read.py" )


# ............ declare the used top algo.
# --------------------------------------------------------
#load relevant libraries
theApp.Dlls += [ "TestLArHardwareID" ]
theApp.TopAlg += [ "testTTMap_Algo"]

testTTMap_Algo = Algorithm( "testTTMap_Algo" )
testTTMap_Algo.dumpMap = True

CondProxyProvider = Service( "CondProxyProvider" )
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )

newFcal = True
#newFcal = False

if newFcal:
    CondProxyProvider.InputCollections = [ "CaloTTMapNew1.pool.root" ]
    DetDescrCnvSvc.LArIDFileName = "IdDictLArCalorimeter_DC3-05-Comm-01.xml"
else:    
    CondProxyProvider.InputCollections = [ "CaloTTMapOld.pool.root" ]
    DetDescrCnvSvc.LArIDFileName = "IdDictLArCalorimeter.xml"


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




