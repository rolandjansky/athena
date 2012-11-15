include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

theApp.Dlls += [ "GaudiAud" ]
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors =  [ "ChronoAuditor" ]

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_geant4()

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.detdescr.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

include( "AtlasGeoModel/SetGeometryVersion.py" )
include( "AtlasGeoModel/GeoModelInit.py" )

include( "CaloCondAthenaPool/CaloTTMap_Pool_Read.py" )


# ............ declare the used top algo.
# --------------------------------------------------------
#load relevant libraries
theApp.Dlls += [ "TestLArHardwareID" ]
theApp.TopAlg += [ "updateTTMap_Algo"]

updateTTMap_Algo = Algorithm( "updateTTMap_Algo" )
updateTTMap_Algo.dumpMap = True


# input map
ProxyProviderSvc = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
CondProxyProvider = Service( "CondProxyProvider" )
CondProxyProvider.InputCollections = [ "LArTTCellMap-DC3-05.pool.root" ]

# output map
include( "CaloCondAthenaPool/CaloTTMap_Pool_Write.py" )
Stream1.OutputFile = "LArTTCellMap-H8.pool.root"

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 3
MessageSvc.debugLimit      = 200000
MessageSvc.useColors   = TRUE

theApp.EvtMax = 1
EventSelector = Service( "EventSelector" )
EventSelector.RunNumber=1




