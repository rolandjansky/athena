# flags to configure RecExCommon
# Default values are set only if the variables do not exist yet.
# To configure it there are several possibilities, from the most
# recommended to the least 
#  - specify values to be changed with the -c option
#    athena -c "readG3=True ; EvtMax = 100" RecExCommon_topOptions.py
#    Note: please use \" if a " is needed inside the command line
#    athena -c "include(\"RecExCond/RecExCommon_flags.py\") ; DetFlags.Muon_setOff()" RecExCommon_topOptions.py
#  - create a file myconfig.py with the values to be changed
#    then run
#    athena myconfig.py RecExCommon_topOptions.py
#  - edit/modify RecExCommon_topOptions.py and add the new values
#    specification inside
#  - edit/modify the default values in RecExCommon_flags.py 
# prevent the current file to be included a second time
include.block("RecExCond/RecExCommon_flags.py")


from AthenaCommon.Logging import logging
from RecExConfig.RecFlags import rec

logRecExCommon_flags = logging.getLogger( 'RecExCommon_flags' )

# start using the new job properties
from AthenaCommon.AthenaCommonFlags import jobproperties,athenaCommonFlags

from PyUtils.moduleExists import moduleExists


#FilesInput() triggers full auto-configuration. Only for a transition period.
if len(athenaCommonFlags.FilesInput())>0:
   logRecExCommon_flags.info("Full auto-configuration has been triggered by FilesInput()")
   rec.AutoConfiguration.append('everything')

#Optional auto-configuration
if len(rec.AutoConfiguration())>0:
    from RecExConfig.AutoConfiguration import ConfigureFromListOfKeys
    ConfigureFromListOfKeys(rec.AutoConfiguration())
    from RecExConfig.PyComps import AutoConfigConsistencyCheckSvc
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    svcMgr += AutoConfigConsistencyCheckSvc("AutoConfigConsistencyCheckSvc")

#special commisioning job options
if rec.Commissioning():
   include("RecExCond/MinimalCommissioningSetup.py")
   rec.ScopingLevel.set_Value_and_Lock(1)

if rec.triggerStream().startswith('express'):
   logRecExCommon_flags.info('Disabling track slimming for express stream.')
   include("RecJobTransforms/NoTrackSlimming.py")


#Some special (monitoring) setting for Heavy Ions:
if rec.doHeavyIon():
   logRecExCommon_flags.info('Heavy Ion case: Calling HIEarlyIncludes.py')
   include("HIRecExample/HIEarlyIncludes.py")

if rec.doHIP():
   logRecExCommon_flags.info('Heavy Ion p+Pb case: Calling HIPEarlyIncludes.py')
   include("HIRecExample/HIPEarlyIncludes.py")
   

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from AthenaCommon.GlobalFlags import globalflags

if moduleExists ('CaloRec'):
   from CaloRec.CaloRecFlags import jobproperties
if moduleExists ('InDetRecExample'):
   from InDetRecExample.InDetJobProperties import InDetFlags
   InDetFlags.doPrintConfigurables=False  # change default here
if moduleExists ('MuonRecExample'):
   from MuonRecExample.MuonRecFlags import muonRecFlags
if moduleExists ('MuonCombinedRecExample'):
   from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
if moduleExists ('tauRec'):
   from tauRec.tauRecFlags import jobproperties
if moduleExists ('JetRec'):
   from JetRec.JetRecFlags import jobproperties

# some special migration (temporary)
if rec.doFloatingPointException() and not athenaCommonFlags.isOnline():
   from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
   athenaCommonFlags.RuntimeStrictness='abort'
else:
   from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
   athenaCommonFlags.RuntimeStrictness='none'

#FIXME testing isDefault is non standard. People should lock it if they want to set their value.
if athenaCommonFlags.PoolRDOInput.isDefault():
   athenaCommonFlags.PoolRDOInput=["LFN:top_default.pool.root"]

#FIXME temporary
if 'doJetRec' in dir():
   from JetRec.JetRecFlags import jobproperties
   jobproperties.JetRecFlags.Enabled.set_Value(doJetRec)


if athenaCommonFlags.FilesInput()==[]:
   from RecExConfig.RecoFunctions import InputFileNames
   athenaCommonFlags.FilesInput=InputFileNames()
   logRecExCommon_flags.info("athenaCommonFlags.FilesInput override input file specification")

# now synchronise other flags 
 
if rec.doWriteRDO():
   logRecExCommon_flags.info( "write RDO, do not make ESD AOD TAG CBNT" )
   # switch off all algorithms
   rec.doESD=False
   rec.doAOD=False
   rec.doWriteESD=False
   rec.doWriteAOD=False
   rec.doWriteTAG=False            
   rec.doCBNT=False
   if rec.readESD or rec.readAOD:
      raise RuntimeError ("Cannot have readESD or readAOD if doWriteRDO")

# in case several read specified, favour the highest one
if rec.readAOD():
   logRecExCommon_flags.info( "read AOD : all algs switched off " )

   # switch off all algorithms
   rec.doESD=False
   rec.doAOD=False   
   rec.doWriteESD=False
   rec.doWriteAOD=False   
   if rec.readRDO:
      logRecExCommon_flags.info( "readAOD : switch readRDO off " )
      rec.readRDO=False
   if rec.readESD():
      logRecExCommon_flags.info( "readESD : switch readRDO off " )
      rec.readRDO=False


if rec.readESD() :
   logRecExCommon_flags.info( "read ESD : all algs switched off " )
   # switch off all algorithms
   rec.doESD=False
   rec.doWriteESD=False
   
   if rec.readRDO():
      logRecExCommon_flags.info( "readESD : switch readRDO off " )
      rec.readRDO=False


# consistency check in case of lock
_allRead=rec.readRDO()+rec.readESD()+rec.readAOD()
if _allRead !=1 :
   raise RuntimeError (" one and only one of readRDO (%s)  readESD (%s) readAOD (%s) should be on !!!" % (rec.readRDO(),rec.readESD(),rec.readAOD()))


from AthenaCommon.Include import excludeTracePattern
excludeTracePattern.append("*/PyUtils/*")

# if autoconfig not used, switched of doTruth for data
if globalflags.DataSource=='data' and not 'DoTruth' in rec.AutoConfiguration():
   rec.doTruth=False

#load commissioning flags here (major cleanup needed)
if rec.Commissioning():

   # ---------------------------------------------------------------------------
   # AthenaCommonFlags
   # ---------------------------------------------------------------------------

   # start using the new job properties
   from AthenaCommon.JobProperties import jobproperties

   # AthenaCommon flags
   from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

   # -----------------------------------------------------------------------
   # being general reconstruction flags
   # -----------------------------------------------------------------------
   from RecExConfig.RecFlags  import jobproperties, rec

   athenaCommonFlags.PoolInputQuery = "EventNumber>10"
   # GlobalFlags 
   from AthenaCommon.GlobalFlags import globalflags

   rec.RootHistoOutput = "monitoring.root"	
   rec.RootNtupleOutput = "ntuple.root"	


   from InDetRecExample.InDetKeys import InDetKeys

   # Number of events to be processed
   theApp.EvtMax = athenaCommonFlags.EvtMax()


   if rec.readESD():
       rec.doHist = False

   if athenaCommonFlags.isOnline():
       rec.doWriteTAG = False
       rec.doWriteTAGCOM = False    
       rec.doWriteESD = False
       rec.doHist=False
       
   #--------------------------------------------------------------
   # Internal flags for each subsystem
   #--------------------------------------------------------------


   # MuDet
   if rec.doMuon() and rec.doESD():

       from MuonRecExample.MuonRecFlags import muonRecFlags
       from MuonCnvExample.MuonCalibFlags import muonCalibFlags

               
   # FIXME obsolete flags (should be removed)!!!
   # ------------------------------------
   # import GlobalFlags from RecExCommon
   # ------------------------------------
   # new flag
   rec.Commissioning=True
   rec.ScopingLevel=4 

   if (globalflags.ConditionsTag() == ''):
      logRecExCommon_flags.warning('No globalflags.ConditionsTag() is set!!!')
      
   from IOVDbSvc.CondDB import conddb
   conddb.setGlobalTag(globalflags.ConditionsTag())



# if production switch off resilience
if rec.Production():
   athenaCommonFlags.AllowIgnoreConfigError=False
   from RecExConfig.RecConfFlags import recConfFlags   
   recConfFlags.AllowIgnoreConfigError=False

include ("RecExCond/RecExCommon_DetFlags.py")

if rec.Commissioning():
   if athenaCommonFlags.isOnline():
      # switch off dcs for all detectors
      try:
          # need AthenaCommon
          DetFlags.dcs.all_setOff()   
      except Exception:
          logRecExCommon_flags.warning("could not set DetFlags.dcs")
          pass


# if bytestream no truth
if globalflags.InputFormat.is_bytestream():
   logRecExCommon_flags.info("No truth in bytestream, setting doTruth=False")    
   rec.doTruth=False   

import os
# edm monitor (so far only id)
if os.environ.get('CMTCONFIG','').endswith('-dbg'):
  rec.doEdmMonitor=True 
else:
  rec.doEdmMonitor=False 


if not rec.doESD():
   # switch off all algorithms unless was explicitly switched on
   for algflag in jobproperties.Rec.RecAlgs.__dict__.keys():
   ##   # ugly but necessary
      if not algflag in ["__name__","_context_name"]:
         data={'JobProperties.Rec.RecAlgs':{algflag:False}}
         jobproperties._log.info('RecExCommon_flags:: migration data %s',data)  
         jobproperties.set_JobProperties(data)

   if moduleExists ('CaloRec'):
      from CaloRec.CaloRecFlags import jobproperties
      jobproperties.CaloRecFlags.Enabled=False

   if moduleExists ('InDetRecExample'):
      from InDetRecExample.InDetJobProperties import InDetFlags
      jobproperties.InDetJobProperties.disableInDetReco=True

   if moduleExists ('MuonRecExample'):
      from MuonRecExample.MuonRecFlags import muonRecFlags
      jobproperties.MuonRec.Enabled=False

   if moduleExists ('MuonCombinedRecExample'):
      from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
      jobproperties.MuonCombinedRec.Enabled=False

   if moduleExists ('JetRec'):
      from JetRec.JetRecFlags import jobproperties
      jobproperties.JetRecFlags.Enabled=False

   if moduleExists ('tauRec'):
      from tauRec.tauRecFlags import jobproperties
      jobproperties.tauRecFlags.Enabled=False


if rec.doESD() and rec.doHeavyIon():
   include ("HIRecExample/heavyion_flagsESD.py")
if rec.doAOD() and rec.doHeavyIon():
   include ("HIRecExample/heavyion_flagsAOD.py")


if rec.doESD() and rec.doHIP():
   include ("HIRecExample/hip_flagsESD.py")
if rec.doAOD() and rec.doHIP():
   include ("HIRecExample/hip_flagsAOD.py")


# minimal reconstruction defaults 


if rec.doMinimalRec():
   include ("RecExCond/MinimalRec_flagsESD.py")


# now translate all the rec.doInDet doCalo etc... into lower level flags
# not complete yet

_AODFlagsAvailable=True
try:
   from ParticleBuilderOptions.AODFlags import AODFlags
except Exception:
   _AODFlagsAvailable=False
   logRecExCommon_flags.info("AODFlags not available. Ok if AtlasReconstruction")


if not rec.doLucid:
   DetFlags.Lucid_setOff()

if not rec.doZdc:
   DetFlags.ZDC_setOff()


if not rec.doAlfa:
   DetFlags.ALFA_setOff()

if not rec.doAFP:
   DetFlags.AFP_setOff()

if not rec.doAFP:
   DetFlags.AFP_setOff()

if not rec.doFwdRegion:  
   DetFlags.FwdRegion_setOff()

if not rec.doInDet:
   #DR should use finer grain
   #rec.doDPD=False
   rec.doTagRawSummary=False
   rec.doBeamBackgroundFiller=False
   DetFlags.ID_setOff()
   from InDetRecExample.InDetJobProperties import InDetFlags
   InDetFlags.Enabled=False
   if _AODFlagsAvailable:
      AODFlags.FastTrackParticle=False
else:
   if rec.Production:
      from InDetRecExample.InDetJobProperties import InDetFlags
   
if not rec.doLArg:
   DetFlags.LAr_setOff()

if not rec.doTile:
   DetFlags.Tile_setOff()
   try:
      from CaloRec.CaloRecFlags import jobproperties
      jobproperties.CaloRecFlags.doTileMuId=False
   except Exception:
      
      pass

if not rec.doCalo:
   #DR should use finer grain
   #rec.doDPD=False
   rec.doTagRawSummary=False
   rec.doBeamBackgroundFiller=False
   DetFlags.Calo_setOff()
   try:
      from CaloRec.CaloRecFlags import jobproperties
      jobproperties.CaloRecFlags.Enabled=False
   except Exception:
      pass


if not rec.doMuon:
   #DR should use finer grain
   #rec.doDPD=False
   rec.doTagRawSummary=False
   rec.doBeamBackgroundFiller=False
   DetFlags.Muon_setOff()
   #FIXME redundant
   recAlgs.doTrackRecordFilter=False
   from MuonRecExample.MuonRecFlags import muonRecFlags
   jobproperties.MuonRec.Enabled=False

if not rec.doMuon and not rec.doCalo and not rec.doInDet:
   rec.doTagRawSummary=False
   rec.doBeamBackgroundFiller=False

if not rec.doJetMissingETTag:
   recAlgs.doMissingET=False
   recAlgs.doObjMissingET=False
   recAlgs.doMissingETSig=False
   recAlgs.doEFlow=False
   recAlgs.doEFlowJet=False   
   try:
      from JetRec.JetRecFlags import jobproperties
      jobproperties.JetRecFlags.Enabled.set_Value(False)
   except Exception:
      pass

      
   if _AODFlagsAvailable:
      AODFlags.ParticleJet=False
      AODFlags.JetTag=False
      AODFlags.MissingEtTruth=False
      AODFlags.TruthParticleJet=False   
   
if not rec.doEgamma:

   if _AODFlagsAvailable:
      AODFlags.egammaTrackSlimmer=False
   
if not rec.doMuonCombined:
   #FIXME a separate container to be used eventually
   from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
   muonCombinedRecFlags.Enabled=False
   recAlgs.doMuonSpShower=False # not (yet?) handled by MuonCombinedRecFlags
   recAlgs.doMuidLowPt=False    # not (yet?) handled by MuonCombinedRecFlags -- OBSOLETE FLAG ???
   

if not rec.doTau:
   try:
      jobproperties.tauRecFlags.Enabled=False
   except Exception:
      pass


if not rec.doTrigger:
   #DR should use finer grain
   #rec.doDPD=False
   rec.doTagRawSummary=False
   rec.doBeamBackgroundFiller=False

   if _AODFlagsAvailable:
      AODFlags.Trigger=False # obsolete


from InDetRecExample.InDetJobProperties import InDetFlags
if not rec.doTruth():
   InDetFlags.doTruth       = False


if not rec.doTruth() or ( globalflags.DetDescrVersion()[0:3]=="DC1" or globalflags.DetDescrVersion()[0:3]=="DC2" )  :
      logRecExCommon_flags.info("TrackRecordFilter alg switched off ")
      recAlgs.doTrackRecordFilter=False




# AOD Flags
if rec.doWriteAOD() and not rec.doAOD() and not rec.readAOD():
   logRecExCommon_flags.warning("doAOD False and doWriteAOD True and readAOD False. Most likely meaninglesss !! ")


if rec.doWriteTAGCOM() and not rec.Commissioning():
   logRecExCommon_flags.info("Not commissioning, switching of doWriteTAGCOM ")
   rec.doWriteTAGCOM=False

# If no AOD objects made, then do not write out a TAG
if not rec.doAOD() and not rec.readAOD() and rec.doWriteTAG():
    logRecExCommon_flags.warning("no AOD objects available, cannot write out tag, switching rec.doWriteTAG=False")
    rec.doWriteTAG=False


if  rec.readTAG():
    logRecExCommon_flags.warning("readingTAG switching rec.doWriteTAG=False")
    rec.doWriteTAG=False
 

if (rec.doAOD() or rec.doWriteAOD()) and _AODFlagsAvailable:
   from ParticleBuilderOptions.AODFlags import AODFlags

   # make sure AOD truth are off if truth is off
   if not rec.doTruth():
      AODFlags.SpclMC=False
      AODFlags.TruthParticleJet=False
      AODFlags.FastSimulation=False


   if rec.doAOD():   
      AODFlags.FastTrackParticle=rec.doTruth()      


   AODFlags.print_JobProperties()

# determine LCGCMT version, needed for switching on/off pool file optimizations
if(os.getenv("LCGCMTVERS")!=None):
   rec.LCGCMTVersion=os.getenv("LCGCMTVERS")
else:
   myathpath=os.getenv("LD_LIBRARY_PATH")
   pos=myathpath.find("LCGCMT_")
   if(pos>-1):
      rec.LCGCMTVersion=myathpath[pos+7:pos+10]

if rec.doTrigger():
   from TriggerJobOpts.TriggerFlags import sync_Trigger2Reco
   sync_Trigger2Reco()

del _AODFlagsAvailable

if len(rec.UserFlags())>0:
    from PyJobTransformsCore.trfutil import StringToExec
    allFlags=StringToExec(rec.UserFlags())
    DetFlagsModified=False
    for uFlags in allFlags:
       logRecExCommon_flags.info("Executing flag mod command %s" % uFlags)       
       exec(uFlags)
       if uFlags.find("DetFlags")>=0:
          DetFlagsModified=True
          
    del allFlags
    if DetFlagsModified:
       logRecExCommon_flags.info("new value of DetFlags")       
       DetFlags.Print()
