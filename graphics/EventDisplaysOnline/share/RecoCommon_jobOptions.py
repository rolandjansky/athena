#----------------------------------------------- Get the run type from IS
from AthenaCommon.BeamFlags import jobproperties
from EventDisplaysOnline.EventDisplaysConfig import GetRunType
jobproperties.Beam.beamType.set_Value_and_Lock(GetRunType())            

#----------------------------------------------- Set the magnetic field
from AthenaCommon.BFieldFlags import jobproperties
from EventDisplaysOnline.EventDisplaysConfig import GetBFields
(solenoidOn,toroidOn)=GetBFields();
jobproperties.BField.barrelToroidOn.set_Value_and_Lock(toroidOn);
jobproperties.BField.endcapToroidOn.set_Value_and_Lock(toroidOn);
jobproperties.BField.solenoidOn.set_Value_and_Lock(solenoidOn);

# ----------------------------------------------- Run configuration 
from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('data')                        
globalflags.InputFormat.set_Value_and_Lock("bytestream")                  
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-08-00-02')     

# ----------------------------------------------- Use conditions according to beamType
conditionTags = { 'collisions' : "COMCOND-MONP-001-00",
                  'singlebeam' : "COMCOND-MONS-001-00",
                  'cosmics'    : "COMCOND-MONC-003-00" }
globalflags.ConditionsTag.set_Value_and_Lock(conditionTags[jobproperties.Beam.beamType()])     


# ----------------------------------------------- Online flag
from AthenaCommon.AthenaCommonFlags import jobproperties,athenaCommonFlags
athenaCommonFlags.isOnline = True                                        # configures the job for online running
athenaCommonFlags.BSRDOInput.set_Value_and_Lock([])                      # set proper bytestream input (for trigger) 

#make sure ESD's and PoolFileCatalog from multiple jobs don't overwrite each other
import os
from random import randrange
randomString=str(randrange(0,100000))
athenaCommonFlags.PoolESDOutput = 'ESD_' + randomString +'.pool.root'
os.environ['POOL_CATALOG'] = 'xmlcatalog_file:PoolFileCatalog_'+randomString+'.xml'
athenaCommonFlags.BSRDOInput = []


# ----------------------------------------------- Output flags
from RecExConfig.RecFlags import rec

rec.doESD.set_Value_and_Lock(True) 
rec.doAOD.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(True)                                  
rec.doWriteAOD.set_Value_and_Lock(False)                                 
rec.doCBNT.set_Value_and_Lock(False)                                     
rec.doWriteTAG.set_Value_and_Lock(False)                                 

# ----------------------------------------------- Robust tracking for 30 Mar 2010
#from InDetRecExample.InDetJobProperties import InDetFlags
#InDetFlags.doRobustReco.set_Value_and_Lock(True)

# ----------------------------------------------- Reco flags
rec.doInDet.set_Value_and_Lock(True)                                   
rec.doMuon.set_Value_and_Lock(True)                                      # if True and doTrigger=True, set doID, doLAr, doTile to True
rec.doLArg.set_Value_and_Lock(True)                                      # if True, set doID, doTile to True
rec.doTile.set_Value_and_Lock(True)                                      # if True, set doID, doLAr to True

# --- Temporarily disable ZDC as it crashes ( savannah bug #59762 )
rec.UserFlags=["DetFlags.ZDC_setOff()"]

rec.doMonitoring.set_Value_and_Lock(False)

rec.doTrigger.set_Value_and_Lock(True)
rec.doHist.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)

# ---------------------------------------------- Debug flags
rec.doPerfMon.set_Value_and_Lock(False)                                  # optional for performance check
rec.doDetailedPerfMon.set_Value_and_Lock(False)                          # optional for performance check
rec.doNameAuditor.set_Value_and_Lock(False)                              # optional for debugging
rec.doDetStatus.set_Value_and_Lock(False)                                # 

#------------------------------ Cosmics running -> do not use beam-spot
rec.Commissioning.set_Value_and_Lock(True)

# ----------------------------- LAr Online fix
include("RecExOnline/SimpleLarCondFlags.py")

####
# If LAr is running in transaprent mode, these lines need to be commented
# Otherwise, they should be uncommented.
####
#from LArROD.LArRODFlags import larRODFlags                                                                                          
#larRODFlags.readDigits.set_Value_and_Lock(False)

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
