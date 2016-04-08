
#  Load all the usefull flags we might want to switch
include ("RecExCommon/RecoUsefulFlags.py")

# ----------------------------------------------- Run configuration 
globalflags.DataSource.set_Value_and_Lock('data')                        
globalflags.InputFormat.set_Value_and_Lock("bytestream")                  
globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-ES1CT-002-00')     
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-08-00-02')     

# ----------------------------------------------- Beam type fixed to singlebeam for splash
jobproperties.Beam.beamType.set_Value_and_Lock("singlebeam")

# ----------------------------------------------- Online flag
from AthenaCommon.AthenaCommonFlags import jobproperties,athenaCommonFlags
athenaCommonFlags.isOnline = True                                        # configures the job for online running

# ----------------------------------------------- Input flags
#make sure ESD's and PoolFileCatalog from multiple jobs don't overwrite each other
import os
from random import randrange
randomString=str(randrange(0,100000))
athenaCommonFlags.PoolESDOutput = 'ESD_' + randomString +'.pool.root'
os.environ['POOL_CATALOG'] = 'xmlcatalog_file:PoolFileCatalog_'+randomString+'.xml'
athenaCommonFlags.BSRDOInput.set_Value_and_Lock([])                      # set proper bytestream input (for trigger) 

# ----------------------------------------------- Output flags
rec.doESD.set_Value_and_Lock(True) 
rec.doAOD.set_Value_and_Lock(True)
rec.doWriteESD.set_Value_and_Lock(True)                                  
rec.doWriteAOD.set_Value_and_Lock(False)                                 
rec.doCBNT.set_Value_and_Lock(False)                                     
rec.doWriteTAG.set_Value_and_Lock(False)                                 

# ----------------------------------------------- Reco flags
# ------------ Inner Detector 
rec.doInDet.set_Value_and_Lock(True)                                   
# --- Minimal reco 
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doMinimalReco      = True
InDetFlags.useDCS.set_Value_and_Lock(False)

# ------------ LAr 
rec.doLArg.set_Value_and_Lock(True)                                      # if True, set doID, doTile to True
# --- LAr Online fix
include("RecExOnline/SimpleLarCondFlags.py")

####
# If LAr is running in transaprent mode, these lines need to be commented
# Otherwise, they should be uncommented.
####
#from LArROD.LArRODFlags import larRODFlags                                                                                          
#larRODFlags.readDigits.set_Value_and_Lock(False)

# ------------ Tile
rec.doTile.set_Value_and_Lock(True)                                      # if True, set doID, doLAr to True

# ------------ Calo reco
from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.doEmCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloEMTopoCluster.set_Value_and_Lock(False)

# ------------ Muon Detector 
rec.doMuon.set_Value_and_Lock(True)                                      # if True and doTrigger=True, set doID, doLAr, doTile to True
# ---- muon settings from Rosy
muonRecFlags.doMoore=False
muonCombinedRecFlags.doMuidMuonCollection=False
#muonRecFlags.doMuonboy=False
#muonCombinedRecFlags.doStacoMuonCollection=False
mooreFlags.doSegmentsOnly=True
muonboyFlags.doSegmentsOnly=True
muonRecFlags.doSegmentT0Fit = False
muonCombinedRecFlags.Enabled = False

# ------------ Trigger
rec.doTrigger.set_Value_and_Lock(True)

# ------------ AOD objects
rec.doEgamma.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)
rec.doJetMissingETTag.set_Value_and_Lock(False)
BTaggingFlags.Active=False

# ------------ Monitoring
rec.doMonitoring.set_Value_and_Lock(False)
rec.doHist.set_Value_and_Lock(False)

# ---------------------------------------------- Debug flags
rec.doPerfMon.set_Value_and_Lock(False)                                  # optional for performance check
rec.doDetailedPerfMon.set_Value_and_Lock(False)                          # optional for performance check
rec.doNameAuditor.set_Value_and_Lock(False)                              # optional for debugging
rec.doDetStatus.set_Value_and_Lock(False)                                # 

#---------------------------------------------- Comissioning settings  
rec.Commissioning.set_Value_and_Lock(True)
rec.abortOnUncheckedStatusCode.set_Value_and_Lock(False)

# ----------------------------- Main jobOpt
include("RecExCommon/RecExCommon_topOptions.py")

# ----------------------------- Catalog file
Service("PoolSvc").SortReplicas = False                                  
PoolSvc = Service( "PoolSvc" )
PoolSvc.ReadCatalog += ["xmlcatalog_file:/sw/DbData/poolcond/PoolCat_comcond.xml"]
PoolSvc.ReadCatalog += ["xmlcatalog_file:/det/dqm/AthenaMonitoring/AtlasTier0-15.4.0.2/DbData/poolcond/PoolCat_comcond.xml"]

# ------------------------------ Force run number for test partition    
# Otherwise can't use TrigDecisionTool

if 'ForceRunNumber' in dir() :
  Service("IOVDbSvc").forceRunNumber = ForceRunNumber
  Service("IOVDbSvc").forceLumiblockNumber = 1

# --------------------------------- Printout
globalflags.print_JobProperties()

# ----------------------- Over-writes come at the end
MessageSvc = Service("MessageSvc")
if 'MsgLvl' in dir():
  MessageSvc.OutputLevel = MsgLvl
else:
  MessageSvc.OutputLevel = WARNING
