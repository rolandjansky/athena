# --- Steering flags -----------------------------------------
if not "XCryo" in dir():
   XCryo = 0.

if not "YTable" in dir():
   YTable = 0.

if not "BeamE" in dir():
   BeamE = " log 200. 300000."

if not "Xsmear" in dir():
   Xsmear =  50.

if not "Ysmear" in dir():
   Ysmear = 50.   

if not "EvtMax" in dir():
   EvtMax = 10   

if not "G4seed" in dir():
   G4seed = 1234

if not "GSeed1" in dir():
   GSeed1 = "2000160768"

if not "GSeed2" in dir():
   GSeed2 = "643921183"

if not "ParticlePDG" in dir():
   ParticlePDG = "211"

if not "PhysicsList" in dir():
   PhysicsList = "QGSP_BERT"

if not "PoolHitsOutput" in dir():
   PoolHitsOutput = 'H6LAr_MyOutputFile.root'

if not "SingleRun" in dir():
   SingleRun = False

if not "doFilter" in dir():
   doFilter = False

if not "doBirk" in dir():
   doBirk = True

# -------- end of steering flags ------------------
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
athenaCommonFlags.EvtMax=EvtMax
athenaCommonFlags.PoolEvgenInput.set_Off()

#--- DetDescr flag
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = 'ATLAS-H6-2004-00'
DetDescrVersion = 'ATLAS-H6-2004-00'
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-H6-2004-00"

#--- Simulation flags -----------------------------------------
#from G4AtlasApps.tbLArH6_flags import *
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
#simFlags.import_JobProperties('G4AtlasApps.tbLArH6_flags')
# Choose H6 2004 (Emec/HEC/FCAL) testbeam layout
simFlags.SimLayout='tb_LArH6_2004'
simFlags.load_tbLArH6_flags()

#--- Conditions global  tag ------
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-CSC-00-00-00"


include.block ( "CaloConditions/LArTTCellMap_ATLAS_jobOptions.py" )
include.block ( "CaloConditions/CaloTTIdMap_ATLAS_jobOptions.py" )
#include("LArDetDescr/LArDetDescr_H6_joboptions.py")

# Set the cryostat and table at their (0,0) position:
simFlags.LArTB_H1CryoXPos.set_Value_and_Lock(XCryo)
simFlags.LArTB_H1TableYPos.set_Value_and_Lock(YTable)
simFlags.LArTB_H1XSmear.set_Value_and_Lock(Xsmear)
simFlags.LArTB_H1YSmear.set_Value_and_Lock(Ysmear)
simFlags.LArTB_H6Hec.set_Value_and_Lock(True)
simFlags.LArTB_H6Emec.set_Value_and_Lock(True)
simFlags.LArTB_H6Fcal.set_Value_and_Lock(True)
simFlags.LArTB_H6Coldnose.set_Value_and_Lock(True)
simFlags.LArTB_H6Run1.set_Value_and_Lock(False)
if doFilter:
   simFlags.LArTB_H6Step.set_Value_and_Lock(True)
else:
   simFlags.LArTB_H6Step.set_Value_and_Lock(False)
if doBirk:
   simFlags.DoLArBirk.set_Value_and_Lock(True)
else:   
   simFlags.DoLArBirk.set_Value_and_Lock(False)


# Uncomment for calibration run
simFlags.CalibrationRun='LAr'

simFlags.EventFilter.set_Off()

#--- Generator flags ------------------------------------------
from AthenaServices.AthenaServicesConf import AtRanluxGenSvc
ServiceMgr += AtRanluxGenSvc()
ServiceMgr.AtRanluxGenSvc.Seeds = ["SINGLE "+ GSeed1 + " " + GSeed2]
simFlags.RandomSeedOffset.set_Value_and_Lock(G4seed)

# Energy is in Mev!!!
if SingleRun:
   try:
      Energy=float(BeamE)
   except:
      print "Wrong energy literal: ",BeamE," using 100 GeV !!!"
      Energy=100000.

simFlags.PhysicsList.set_Value_and_Lock(PhysicsList)


#---  Output printout level ----------------------------------- 
#output threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 4


include("LArG4TBSimEventAthenaPool/LArG4TBSimEventAthenaPool_joboptions.py")
#--- Generator  ----------------------------------------------
#>## this starts the space to customize ParticleGenerator #### 
if SingleRun:
   include("LArGeoH62004Algs/tbH6Generator.py")
else:  
   include("LArGeoH62004Algs/tbH6Generator-prod.py")

# # enter interactive mode
#theApp.initialize()
#theApp.Dlls += [ "RootHistCnv" ]
#theApp.HistogramPersistency = "ROOT"
#NTupleSvc = Service( "NTupleSvc" )
#NTupleSvc.Output = [ "FILE1 DATAFILE='Ntuple.root' OPT='NEW'" ]


include("G4AtlasApps/G4Atlas.flat.configuration.py")

#==============================================================
# Job configuration
#==============================================================
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

# Adding TB specific output
from AthenaCommon import CfgMgr
CfgMgr.AthenaOutputStream.OutputLevel    = 5
outStreams = AlgSequence( "Streams" )
outStream = outStreams.StreamHITS
outStream.ItemList+=["LArG4H6WarmTCHitCollection#*"]
outStream.ItemList+=["LArG4H6FrontHitCollection#*"]
outStream.ItemList+=["TBEventInfo#*"]

if doFilter:
  svcMgr.GeoModelSvc.DetectorTools.__getitem__("LArDetectorToolH62004").CheckPrim=True

