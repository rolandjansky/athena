## jO to run H6 TB 2004 simulation
##--------------------------------

if not 'PoolHitsOutput' in dir():
   PoolHitsOutput="H6LAr_MyOutputFile.root"

if not 'EvtMax' in dir():
   EvtMax=10

if not 'CryoXPos' in dir():
   CryoXPos=0.

if not 'TableYPos' in dir():
   TableYPos=0.

if not 'ParticlePDG' in dir():
   ParticlePDG='211'

if not 'Energy' in dir():
   Energy=200000

##--------------------------------
## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
# - Select detectors 
DetFlags.Calo_setOn()
# - MCTruth
DetFlags.Truth_setOn()

#--- AthenaCommon flags ---------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput=PoolHitsOutput

# Number of events to be processed (default is 10)
athenaCommonFlags.EvtMax=EvtMax

# explicitly switch off evgen reading
athenaCommonFlags.PoolEvgenInput.set_Off()

#--- DetDescr flag
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-H6-2004-00')
DetDescrVersion = 'ATLAS-H6-2004-00'
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-H6-2004-00"

#--- Simulation flags -----------------------------------------
# Choose H6 2004 (Emec/HEC/FCAL) testbeam layout
from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags()
SimFlags.SimLayout='tb_LArH6_2004'
SimFlags.load_tbLArH6_flags()

#--- Conditions global  tag ------
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-CSC-00-00-00"


include.block ( "CaloConditions/LArTTCellMap_ATLAS_jobOptions.py" )
include.block ( "CaloConditions/CaloTTIdMap_ATLAS_jobOptions.py" )
#include("LArDetDescr/LArDetDescr_H6_joboptions.py")

# Set the cryostat and table at their (0,0) position:
SimFlags.LArTB_H1CryoXPos.set_Value_and_Lock(CryoXPos)
SimFlags.LArTB_H1TableYPos.set_Value_and_Lock(TableYPos)
SimFlags.LArTB_H1XSmear.set_Value_and_Lock(35.)
SimFlags.LArTB_H1YSmear.set_Value_and_Lock(35.)
SimFlags.LArTB_H6Hec.set_Value_and_Lock(True)
SimFlags.LArTB_H6Emec.set_Value_and_Lock(True)
SimFlags.LArTB_H6Fcal.set_Value_and_Lock(True)
SimFlags.LArTB_H6Coldnose.set_Value_and_Lock(True)
SimFlags.LArTB_H6Run1.set_Value_and_Lock(False)
SimFlags.LArTB_H6Step.set_Value_and_Lock(False)


# Uncomment for calibration run
SimFlags.CalibrationRun='LAr'

SimFlags.EventFilter.set_Off()

#--- Generator flags ------------------------------------------
from AthenaServices.AthenaServicesConf import AtRanluxGenSvc
ServiceMgr += AtRanluxGenSvc()
ServiceMgr.AtRanluxGenSvc.Seeds = ["SINGLE 2000160768 643921183"]

SimFlags.RandomSeedOffset.set_Value_and_Lock(1234)

SimFlags.PhysicsList.set_Value_and_Lock('QGSP_BERT')

#--------- Birk's law on -----------------------
SimFlags.DoLArBirk.set_Value_and_Lock(True)

#---  Output printout level ----------------------------------- 
#output threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 4


include("LArG4TBSimEventAthenaPool/LArG4TBSimEventAthenaPool_joboptions.py")

#--- Generator  ----------------------------------------------
#>## this starts the space to customize ParticleGenerator #### 
include("LArGeoH62004Algs/tbH6Generator.py")

# # enter interactive mode
#theApp.initialize()
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='Ntuple.root' OPT='NEW'" ]


#>## this starts the space to customize the simulation ####### 

# Visualisation
def run_vis():
   from G4AtlasApps import AtlasG4Eng
   Vis1=AtlasG4Eng.G4Eng.menu_Visualization()
   Vis1.set_Parameters('VisDriver','DAWNFILE')
   #Vis1.add_volume2vis('LARTBWORLD::LARTBWORLD')
   Vis1.add_volume2vis('LArMgr::LArGeoTB::EMEC::Excluder')
   Vis1.add_volume2vis('LAr')
   #Vis1.add_volume2vis('LArMgr::LAr::HEC::Module::Depth::Absorber')
   #Vis1.add_volume2vis('LArMgr::LAr::HEC::Module::Depth')
   Vis1.add_volume2vis('LArMgr::LAr::TB::BPCOLD::bpco_plane')
   Vis1.add_volume2vis('LArMgr::LAr::TB::BPCOLD::bpc_mylar')
   Vis1.add_volume2vis('LArMgr::LAr::TB::BPCOLD::bpco_div')
   Vis1.add_volume2vis('LArMgr::LAr::TB::BPCOLD::bpco_plane')
   Vis1.add_volume2Notvis('*')
   Vis1.set_active()
   Vis1.set_VisTrack()

#SimFlags.InitFunctions.add_function("postInit",run_vis)

# - ex1: change the verbosity 
def change_verb():
   from G4AtlasApps import AtlasG4Eng
   AtlasG4Eng.G4Eng.gbl.G4Commands().tracking.verbose(3)

#SimFlags.InitFunctions.add_function("postInit",change_verb)

include("G4AtlasApps/G4Atlas.flat.configuration.py")

#==============================================================
# Job configuration
#==============================================================
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

#--- LArH6 setup description  ----------------------------------
# Adding TB specific output
from AthenaCommon import CfgMgr
CfgMgr.AthenaOutputStream.OutputLevel    = 5
outStreams = AlgSequence( "Streams" )
outStream = outStreams.StreamHITS
outStream.ItemList+=["LArG4H6WarmTCHitCollection#*"]
outStream.ItemList+=["LArG4H6FrontHitCollection#*"]
outStream.ItemList+=["TBEventInfo#*"]
# Another implementation of the same:
#from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
#stream1 = AthenaPoolOutputStream("StreamHITS", jobproperties.AthenaCommonFlags.PoolHitsOutput())
#stream1.OutputLevel    = 5
#stream1.ItemList+=["LArG4H6WarmTCHitCollection#*"]
#stream1.ItemList+=["LArG4H6FrontHitCollection#*"]
#stream1.ItemList+=["TBEventInfo#*"]


# choose filter or printout
svcMgr.GeoModelSvc.DetectorTools.__getitem__("LArDetectorToolH62004").CheckPrim=False
svcMgr.GeoModelSvc.DetectorTools.__getitem__("LArDetectorToolH62004").PrintStep=True


#--- End jobOptions.G4TB_LArH6-2004.py file  ------------------
