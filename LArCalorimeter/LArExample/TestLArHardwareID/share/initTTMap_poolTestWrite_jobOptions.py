include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

theApp.Dlls += [ "GaudiAud" ]
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors =  [ "ChronoAuditor" ]

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_geant4()

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.detdescr.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

#DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
#DetDescrCnvSvc.IdDictFromRDB = False    //doesn't work anymore ?!

# setup GeoModel
DetDescrVersion="ATLAS-DC3-05"
#DetDescrVersion="ATLAS-CSC-02-00-00"
include( "AtlasGeoModel/SetGeometryVersion.py" )
include( "AtlasGeoModel/GeoModelInit.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloTTDetMgrDetDescrCnv_joboptions.py" )
include( "CaloConditions/LArTTCellMap_ATLAS_jobOptions.py" )


theApp.Dlls += [ "TestLArHardwareID" ]
theApp.TopAlg += ["initTTMap_Algo" ] # test algorithm
initTTMap_Algo = Algorithm( "initTTMap_Algo" )
initTTMap_Algo.dumpMap = True

if 'initFcalFromFile' not in dir():
# "new FCAL" DC3-05-Comm-01
    initFcalFromFile = True
# "old FCAL" DC3-02
#    initFcalFromFile = False
print "initialized initFcalFromFile ",initFcalFromFile

include( "CaloCondAthenaPool/CaloTTMap_Pool_Write.py" )

# Stream1 = default output stream
if initFcalFromFile:
    print "will initialize FCAL part from external file"
    initTTMap_Algo.initFromFile="ON"
    Stream1.OutputFile = "LArTTCellMap-DC3-05.pool.root"
    DetDescrCnvSvc.LArIDFileName = "IdDictLArCalorimeter_DC3-05-Comm-01.xml"    #doesn't work anymore ?!
else:    
    print "will initialize FCAL part as the rest of the detector"
    initTTMap_Algo.initFromFile="OFF"
    Stream1.OutputFile = "LArTTCellMap-DC3-02.pool.root"
    DetDescrCnvSvc.LArIDFileName = "IdDictLArCalorimeter.xml"    #doesn't work anymore ?!




#/--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 2
MessageSvc.debugLimit      = 300000
#--------------------------------------------------------------
EventSelector = Service( "EventSelector" )
EventSelector.RunNumber=1

theApp.EvtMax=1





