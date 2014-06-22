#==============================================================
#
# Job options file for Digitization
#
# CTB_G4Sim: H6 CTB (2004) digitization 
#
#==============================================================

# Number of events to be processed (default is 10)
if (not "EvtMax" in dir()):
   EvtMax = 10

if (not "doCaloNoise" in dir()):
   doCaloNoise = TRUE

if (not "PoolHitsInput" in dir()):
   PoolHitsInput = [ "Test.pool.root" ]

if (not "PoolRDOOutput" in dir()):
   PoolRDOOutput = "TestDigi.pool.root"

if (not "SkipEvents" in dir()):
   SkipEvents = 0

import AthenaCommon.AtlasUnixStandardJob

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
# - Select detectors
DetFlags.ID_setOff()
DetFlags.LAr_setOn()
DetFlags.Tile_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff() # not ready !!!!!!!!!!
#DetFlags.LVL1_setOn()

# - Switch off tasks
DetFlags.pileup.all_setOff()
DetFlags.simulate.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()
DetFlags.writeRDOPool.all_setOff()

#DetFlags.writeRDOPool.Calo_setOn()
DetFlags.writeRDOPool.LAr_setOn()

#---------- Global Flags
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('ctbh6')
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('geant4')
globalflags.InputFormat.set_Value_and_Lock('pool')

#--- Event related parameters ---------------------------------

theApp.EvtMax = EvtMax

#--- Pool specific --------------------------------------------
# - General Pool converters
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
# - AthenaPool converter (read  hits)
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
# - EventInfo Converters
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
# - TB specific
#include( "LArG4TBSimEvent/LArG4TBSimEventDict_joboptions.py" )
include( "LArG4TBSimEventAthenaPool/LArG4TBSimEventAthenaPool_joboptions.py" )
theApp.Dlls += [ "TBEventAthenaPoolPoolCnv" ]
theApp.Dlls += [ "LArHV" ]


# - Pool input (Change this to use a different file)
from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'EventSelector'):
      import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = PoolHitsInput

ServiceMgr.EventSelector.SkipEvents = SkipEvents

if DetFlags.writeRDOPool.any_on():
    # Pool Output
    from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
    Stream1 = AthenaPoolOutputStream("Stream1")
    Stream1.ItemList+=["EventInfo#*"];           
    Stream1.ItemList+=["TBEventInfo#*"];           
    Stream1.ItemList+=["McEventCollection#*"]
    Stream1.ForceRead=TRUE;  #force read of output data 

    Stream1.OutputFile = PoolRDOOutput

#--- Initial checks  -------------------------------------------
#print job configuration
DetFlags.Print()
#check job configuration
if not(DetFlags.geometry.any_on()):
    AthError( "this digitization job needs some DetFlags.geometry On" )
if not(DetFlags.digitize.any_on()):
    AthError( "this *digitization* job needs some DetFlags.digitize On" )
if not(DetFlags.writeRDOPool.any_on()):
    log.warning( "this digitization job will not write any RDO object" )

#--- GeoModel stuff -------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-H6-2004-00')
DetDescrVersion = 'ATLAS-H6-2004-00'
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-H6-2004-00"

#--- Conditions global  tag ------
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-CSC-00-00-00"


from DetDescrCnvSvc.DetDescrCnvSvcConf import DetDescrCnvSvc
#--- Detector digitization configuration ----------------------
if DetFlags.LAr_on():
    #   Pool Converters
    include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
    include( "IOVDbSvc/IOVDbSvc_jobOptions.py" )
    include( "LArConditionsCommon/LArConditionsCommon_H6G4_jobOptions.py" )
    # Common for the Calo 
    include.block ( "CaloConditions/LArTTCellMap_ATLAS_jobOptions.py" )
    include.block ( "CaloConditions/CaloTTIdMap_ATLAS_jobOptions.py" )
    # the LAr and Calo detector description package
    include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
    include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
    #include( "LArCondCnv/LArCondCnv_IdMapH6_jobOptions.py" )

    # -- Digitization - LArCalorimeter
    include("LArDetDescr/LArDetDescr_H6_joboptions.py")

    theApp.Dlls += [ "LArG4H6SDTools" ]
    from LArG4H6SD.LArG4H6SDToolsConf import LArTBH6TriggerTimeTool
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += LArTBH6TriggerTimeTool()
    from LArRecUtils.LArOFCToolDefault import LArOFCToolDefault
    theLArOFCTool = LArOFCToolDefault()
    theLArOFCTool.FromDatabase=FALSE
    ToolSvc += theLArOFCTool
    ToolSvc.LArTBH6TriggerTimeTool.isFixed=TRUE
    ToolSvc.LArTBH6TriggerTimeTool.FixedTime = 83.  #maybe this one
    ToolSvc.LArTBH6TriggerTimeTool.OutputLevel = 4

    include( "H6G4Sim/LArDigitization_H6G4_jobOptions.py" )

    DetDescrCnvSvc.DecodeIdDict = TRUE

    include( "LArROD/LArROD_jobOptions.py" )
    LArRawChannelBuilder.Ecut=0

#DetDescrCnvSvc.LArIDFileName = "IdDictParser/IdDictLArCalorimeter_H6_2004.xml"

if DetFlags.writeRDOPool.LAr_on():
          Stream1.ItemList+=["LArRawChannelContainer#*"]
          Stream1.ItemList+=["LArDigitContainer#*"]
          Stream1.ItemList+=["LArHitContainer#*"]
          Stream1.ItemList+=["CaloCalibrationHitContainer#*"]
          Stream1.ItemList+=["LArG4H6FrontHitCollection#*"]
#          Stream1.ItemList+=["LArTBLeakHitCollection#*"]
          Stream1.ItemList+=["LArG4H6WarmTCHitCollection#*"]

theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service("NTupleSvc")
NTupleSvc.Output +=  [ "MYFILE DATAFILE='mynt.root' TYP='ROOT' OPT='NEW'" ]

## Track reco
from LArG4H6SD.LArG4H6SDToolsConf import LArTBH6BeamInfo
BeamTrack =LArTBH6BeamInfo("BeamTrack")
BeamTrack.OutputLevel = 4
BeamTrack.HitsContainer += [ "Movable::Hits" ]
BeamTrack.HitsContainer += [ "Front::Hits" ]
topSequence += BeamTrack
if DetFlags.writeRDOPool.LAr_on():
          Stream1.ItemList+=["TBTrack#*"]
    
# --------- Try my additions ----------------------
#include( "MyInspect/MyInspection_jobOption.py" )
# Open the ntuple


#---  Output printout level -----------------------------------
#output threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#you can override this for individual modules if necessary
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 4
MessageSvc.verboseLimit  = 100000000
MessageSvc.debugLimit  = 100000000
AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
AthenaEventLoopMgr.OutputLevel = 5 #get event printout
ToolSvc.LArOFCToolDefault.OutputLevel = 5

