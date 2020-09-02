# flags to configure RecExCommon
# Default values are set only if the variables do not exist yet.
# To configure it there are several possibilities, from the most
# recommended to the least 
#  - specify values to be changed with the -c option
#    athena -c "readG3=True ; EvtMax = 100" RecExCommon_topOptions.py
#    Note: please use \" if a " is needed inside the command line
#    athena -c "include(\"RecExCommon/RecExCommon_flags.py\") ; DetFlags.Muon_setOff()" RecExCommon_topOptions.py
#  - create a file myconfig.py with the values to be changed
#     (myconfig.py ould be copied/edited from RecExCommon_flags.py),
#    then run
#    athena myconfig.py RecExCommon_topOptions.py
#  - edit/modify RecExCommon_topOptions.py and add the new values
#    specification inside
#  - edit/modify the default values in RecExCommon_flags.py 
# prevent the current file to be included a second time
include.block("RecExCond/RecExCommon_flags.py")


from AthenaCommon.Logging import logging
from RecExConfig.RecFlags import rec
from AthenaCommon.Resilience import treatException,protectedInclude

logRecExCommon_flags = logging.getLogger( 'RecExCommon_flags' )

# default value for old flag compatibility
if not 'oldFlagDefault' in dir():
   oldFlagDefault=True
if not oldFlagDefault:
   logRecExCommon_flags.info("old flags default will be IGNORED !. Also switching oldFlagCompatibility to False" )
   oldFlagCompatibility=False



# start using the new job properties
from AthenaCommon.JobProperties import jobproperties

# mgallas 
from AthenaCommon.AthenaCommonFlags import jobproperties,athenaCommonFlags
# mgallas end  


#(can be disabled by doing:
# athenaCommonFlags.RuntimeStrictness.set_Value_and_Lock('none')

from AthenaCommon.Constants import *

# trick : dir() should be done before the import of AthenaCommonFlags
# so that the default in RecExCommonFlags supersedes the default in
# AthenaCommonFlags
varInit=dir()


# compat layer : check if some obsolete variables have been set
   


# some very general flags, EvtMax, SkipEvents, PoolESDInput etc...
#include ("AthenaCommon/AthenaCommonFlags.py")
# prevent this file to be included a second time
#include.block("AthenaCommon/AthenaCommonFlags.py")
# temporary cut and paste here as I cannot control the settings
# be careful that default value are in fact overriden by new flags
AthenaCommonFlags = {
   'EvtMax' : 5,    # number of events to be processed
   'SkipEvents' : 0,    # number of events to be skipped      
   'PoolEvgenInput' : ["Evgen.pool.root"],
   'PoolHitsInput' : ["Hits.pool.root"],
   'PoolRDOInput' :  ["RDO.pool.root"], # input RDO file
   'PoolESDInput' : [ "ESD.pool.root" ], # pool ESD input file
   'PoolAODInput' : [ "AOD.pool.root" ], # pool AOD input file
   'PoolTAGInput' : [ "TAG.pool.root" ], # pool TAG input file   
   'PoolEvgenOutput' : "Event.pool.root", # pool evgen file 
   'PoolHitsOutput' : "Hits.pool.root", # pool evgen file 
   'PoolRDOOutput' : "RDO.pool.root", # pool evgen file 
   'PoolESDOutput' : "ESD.pool.root", # pool ESD output file
   'PoolAODOutput' : "AOD.pool.root", # pool AOD output file
   'PoolTAGOutput' : "TAG.pool.root" # pool TAG output file                
   }

# set variable to default if not already set
# do not override
#for o in [ o for o in AthenaCommonFlags.keys() if not o in varInit ]:
#   exec '%s = AthenaCommonFlags[ "%s" ]' % (o,o)




# use python ntuple to store variables (from wim)
# these are default values
# temporary hack change the default of athenaCommonFlags

#FIXME testing isDefault is non standard. People should lock it if they want to set their value.
if athenaCommonFlags.PoolRDOInput.isDefault():
   athenaCommonFlags.PoolRDOInput=["LFN:top_default.pool.root"]
if athenaCommonFlags.BSRDOInput.isDefault():
   athenaCommonFlags.BSRDOInput=["./dc2_mixing_all_phys_0003621_file100002.data"]



# be careful that default value are in fact overriden by new flags
RecExCommonFlags = {
      'BSRDOInput' :      ["./dc2_mixing_all_phys_0003621_file100002.data"],
      'PoolRDOInput' :  ["LFN:top_default.pool.root"], # input RDO file 
      'BSRDOOutput' : "", # BS output file name
      # (LFN for logical file name, otherwise specify full path (preceded by rfio: for castor files)
      # see RecExCommon_links_zc.sh for a few more test files
      # be careful to set correctly DetDescrVersion 
      'readTAG' : False, # if true use collection (to be done in *addition* to readESD if relevant) (collections are to be created first with:
      # athena.py -c " In=[\"LFN:zee_RDO_extract.pool\"] ; Out=\"MyColl\" " TagCollectionTest/MakeSimpleCollection.py      
      'RootNtupleOutput' : "ntuple.root", # Root Ntuple output  
      'RootHistoOutput' : "histo.root", # Root Histogram output       
      'readESD' : False,  # if run on ESD file
      'readAOD' : False,  # if read AOD file
      'readRDO' : True,  # if read RDO file      
      'doWriteBS' : False, # if write out byte stream file 
      'doWriteRDO' : False, # if write out RDO file 
      'doWriteESD' : True, # if write out ESD file
      'doWriteAOD' : True, # if write out AOD file
      'doWriteTAG' : True, # if write out TAG file 
      'doDPD' : False, # if compute DPD objects and writes them
      'doAOD' : True, # if compute AOD objects (without necessary writing them)
      'doESD' : True, # if compute ESD objects (without necessary writing them)
      'doRestrictedESD' : False, # if write restricted ESD
      'doAODall' : True, # write all ID track particles if available
      'doAODCaloCells' : True, # make AOD cell container
      'OutputLevel': INFO,  # general msg output level VERBOSE DEBUG INFO
      'doTimeLimit': False, # if stop execution when allocated time elapsed
      'doTruth' : True, # id use the truth
      'doDumpMC' : False, # if dump mc truth in log file
      'doNameAuditor' : False, # if NameAuditor switched on (one line print out per alg)
      'doShowSizeStatistics' : False, # if print pool object size stat of input file
      'noESDTrigger' : False, # specify that ESD has no trigger info (hack!)
      'doFileMetaData' : True, # specify that meta data should be written to file      
      'doEdmMonitor' : False, # if monitor edm leak
      'doDumpTES' : False, # if dump storegate transient event store
      'doDumpTDS' : False, # if dump storegate transient detector store
      'doFloatingPointException' : True, # if enable floating-point exception
      'doDumpPoolInputContent' : False, # if dump pool input object list
      'doDumpProperties' : False, # if dump all algs properties
      'doDetailedAuditor' : False, # if monitor alg/tools and services memory and CPU time
      'doSGAuditor' : False, # if monitor data flow, see https://twiki.cern.ch/twiki/bin/view/Atlas/DataFlowMonitoring 
      'doPerfMon' : True, # if monitor software performance, see https://twiki.cern.ch/twiki/bin/view/Atlas/PerfMonComps
      'doDetailedPerfMon' : False, # if monitor software performance, see https://twiki.cern.ch/twiki/bin/view/Atlas/PerfMonComps
      'doCheckDictionary' : False, # if check dictionary
      'doMonitoring': False, # if activate monitoring
      'doCBNT' : True, # if output combined ntuple
      'CBNTAthenaAware' : True, # if CBNT is AthenaAware
      'doHeavyIon' : False, # if run HeavyIon reconstruction
      'doLowPt' : False, # if special low pt reco
      'doMinimalRec' : False, # if run minimal reconstruction
      'doHist' : True, # if output some histograms
      'doJiveXML' : False, # if output xml file for atlantis
      'doPersint' : False, # if output ascci file for persint
      'LoadGeometry' : True, # loads ATLAS detector geometry and magnetic field
      'UserAlgs' : [],
      'DPDMakerScripts' : [],
      'AMITag' : "",
      'doESDReconstruction' : False
      } # user algorithm(s) e.g  ["MyPackage/MyAlg_jobOptions.py"]


# set variable to default if not already set
#if oldFlagDefault:
#   for o in [ o for o in RecExCommonFlags.keys() if not o in varInit ]:
#      exec '%s = RecExCommonFlags[ "%s" ]' % (o,o)


# Algorithm switches (separated so that they can be manipulated all together
# be careful that default value are in fact overriden by new flags
RecExCommonAlgs = {
      'doTrackRecordFilter' : True,
      'donewTracking' : True,   
      'doBackTracking' : True,      
      'doEmCluster' : True,
      'doCaloCluster' : False,
      'doCaloTopoCluster' : True,
      'doMoore' : True,
      'doMuonboy' : True,
      'doEgamma' : True,
      'doJetRec' : True,
      'doTauRec' : True,
      'doMuonIDStandAlone' : True,
      'doMuonIDCombined' : True,
      'doMuidLowPt' : False,      
      'doMuGirl' : True,
      'doStaco' : True,
      'doMuTag' : True,
      'doTileMuID' : True,
      'doCaloTrkMuId' : True,
      'doMuonSpShower' : True,      
      'doMissingET' : True,
      'doObjMissingET' : False,
      'doMissingETSig' : False,
      'doEFlow' : False,
      'doEFlowJet' : False,
      'doTrigger' : True,
      'doAtlfast' : False
}
   



#if oldFlagDefault:
#   if doESD :
#      for o in [ o for o in RecExCommonAlgs.keys() if not o in varInit ]:
#         exec '%s = RecExCommonAlgs[ "%s" ]' % (o,o)      
#   else:
#      logRecExCommon_flags.info("By default switch off all algorithms")
#      # make sure RecExCommonAlgs is consistent (necessary for clarity   
#      for o in [ o for o in RecExCommonAlgs.keys() if o in varInit ]:
#         exec 'RecExCommonAlgs["%s"] = %s' % (o,o)
#      for o in [ o for o in RecExCommonAlgs.keys() if not o in varInit ]:
#         exec '%s = False' % (o)



# merge Algs flag with the other flags for future reference
RecExCommonFlags.update (AthenaCommonFlags)
RecExCommonFlags.update (RecExCommonAlgs)


#FilesInput() triggers full auto-configuration. Only for a transition period.
if len(athenaCommonFlags.FilesInput())>0:
   logRecExCommon_flags.info("Full auto-configuration has been triggered by FilesInput()")
   rec.AutoConfiguration.append('everything')

#Optional auto-configuration
if len(rec.AutoConfiguration())>0:
    from RecExConfig.AutoConfiguration import ConfigureFromListOfKeys
    ConfigureFromListOfKeys(rec.AutoConfiguration())
    include ("RecExConfig/AutoConfigConsistencyCheck.py")



#special commisioning job options
if rec.Commissioning():
   include("RecExCommission/RecExCommission.py")

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
   

varInit=dir()

# mgallas 
logRecExCommon_flags.info('starting the migration to JobProperties. WILL SOON BE DISABLED.')

# first update RecExCommonFlags value according to variable set
for i in RecExCommonFlags.keys(): 
   if i in varInit:
      i_value=eval(str(i))
      if not i_value == RecExCommonFlags[i]:
         logRecExCommon_flags.error( 'updating %s to value %s . WILL SOON NOT WORK ANYMORE ! Please update to filling directly jp.Rec.blah or jp.AthenaCommonFlags.blah  ' ,i,i_value)
         RecExCommonFlags[i]=i_value
#   else:
#      raise RuntimeError, ( ' Variable %s is in RecExCommonFlags but not in varInit. Impossible !', i)

if rec.OutputLevel() <= DEBUG:
   printfunc ("RecExCommon Flags values:")
   printfunc (RecExCommonFlags)
# at this step RecExCommonFlags is consistent with the variables
# i.e. doTruth=True ==> RecExCommonFlags['doTruth"]=True


# now update the job properties from the RecExCommonFlags
#  only if steering from old flags
from RecExConfig.RecFlags  import jobproperties, rec


from RecExConfig.RecAlgsFlags  import recAlgs

from AthenaCommon.GlobalFlags import globalflags

# only necessary during migration
try:
   from CaloRec.CaloRecFlags import jobproperties
   CaloRecFlagsOK=True
except Exception:
   treatException("Could not instantiate CaloRecFlags ")
   CaloRecFlagsOK=False

try:
   from InDetRecExample.InDetJobProperties import InDetFlags
   # change default here
   InDetFlags.doPrintConfigurables=False
except Exception:
   treatException("Could not instantiate InDetFlags ")


try:
   from MuonRecExample.MuonRecFlags import muonRecFlags
except Exception:
   treatException("Could not instantiate muonRecFlags ")

try:
   from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
except Exception:
   treatException("Could not instantiate muonCombinedRecFlags ")


try:
   from tauRec.tauRecFlags import jobproperties
   tauRecFlagsOK=True
except Exception:
   treatException("Could not instantiate tauRecFlags ")
   tauRecFlagsOK=False

try:
   from JetRec.JetRecFlags import jobproperties
   JetRecFlagsOK=True
except Exception:
   JetRecFlagsOK=False
   pass

try:
   from egammaRec.egammaRecFlags import jobproperties
   egammaRecFlagsOK=True
except Exception:
   treatException("Could not instantiate egammaRecFlags ")
   egammaRecFlagsOK=False

#printfunc (jobproperties)

varInit=dir()

# only take old flag if explicitly set
if not rec.oldFlagTopSteering() :
   logRecExCommon_flags.info( 'Steering from old flags disabled!')
else:
   logRecExCommon_flags.info( 'Synchronising  old flags to new flags')
   for i in RecExCommonFlags.keys():
    inew=i
    # hack handle name mismatch
    if i=="donewTracking":
       inew="doNewTracking"
    # hack do not map doTrigger to rec.doTrigger but to recAlgs.doTrigger   
    # FIXME why not the contrary
    if jobproperties.Rec.__dict__.keys().__contains__(i) :
       if i in varInit:
          i_value = eval ('%s' % (i))
          data={'JobProperties.Rec':{i:i_value}}
          jobproperties._log.info('RecExCommon_flags:: migration data %s',data)  
          jobproperties.set_JobProperties(data)
       else:
          jpvalue = getattr(jobproperties.Rec, i)()
          i_value=RecExCommonFlags.get(i)
          if i_value!=jpvalue:
             logRecExCommon_flags.info('inconsistency: for flag %s new flag: %s old flag:%s',i,jpvalue,i_value)

    if(jobproperties.MuonCombinedRec.__dict__.keys().__contains__(inew)):
       if i in varInit:
          # i_value=RecExCommonFlags.get(i)
          i_value = eval('%s' % (i))
          data={'JobProperties.MuonCombinedRec':{inew:i_value}}
          jobproperties._log.info('RecExCommon_flags:: migration LOCKED data %s',data)  
          jobproperties.set_JobProperties(data)
          #for RecAlgFlags : lock the value
          getattr(jobproperties.MuonCombinedRec, inew)._locked = True
       else:
          jpvalue = getattr(jobproperties.MuonCombinedRec, inew)()
          i_value=RecExCommonFlags.get(i)
          if i_value!=jpvalue:
             logRecExCommon_flags.info('inconsistency: for flag %s new flag: %s old flag:%s',i,jpvalue,i_value)
    if(jobproperties.Rec.RecAlgs.__dict__.keys().__contains__(i)) :
       if i in varInit:
          # i_value=RecExCommonFlags.get(i)
          i_value = eval('%s' % (i))
          data={'JobProperties.Rec.RecAlgs':{i:i_value}}
          jobproperties._log.info('RecExCommon_flags:: migration LOCKED data %s',data)  
          jobproperties.set_JobProperties(data)
          #for RecAlgFlags : lock the value
          getattr(recAlgs,i)._locked = True
       else:
          jpvalue = getattr(jobproperties.Rec.RecAlgs, i)()
          i_value=RecExCommonFlags.get(i)
          if i_value!=jpvalue:
             logRecExCommon_flags.info('inconsistency: for flag %s new flag: %s old flag:%s',i,jpvalue,i_value)
    if(CaloRecFlagsOK and jobproperties.CaloRecFlags.__dict__.keys().__contains__(i)):
       if i in varInit:
          # i_value=RecExCommonFlags.get(i)
          i_value = eval ('%s' % (i))
          data={'JobProperties.CaloRecFlags':{i:i_value}}
          jobproperties._log.info('RecExCommon_flags:: migration LOCKED data %s',data)  
          jobproperties.set_JobProperties(data)
          #for RecAlgFlags : lock the value
          getattr(jobproperties.CaloRecFlags, i)._locked = True
       else:
          jpvalue = getattr(jobproperties.CaloRecFlags, i)()
          i_value=RecExCommonFlags.get(i)
          if i_value!=jpvalue:
             logRecExCommon_flags.info('inconsistency: for flag %s new flag: %s old flag:%s',i,jpvalue,i_value)
    elif(tauRecFlagsOK and jobproperties.tauRecFlags.__dict__.keys().__contains__(i)):
       if i in varInit:
          # i_value=RecExCommonFlags.get(i)
          i_value = eval('%s' % (i))
          data={'JobProperties.tauRecFlags':{i:i_value}}
          jobproperties._log.info('RecExCommon_flags:: migration LOCKED data %s',data)  
          jobproperties.set_JobProperties(data)
          #for RecAlgFlags : lock the value
          getattr(jobproperties.tauRecFlags, i)._locked = True
       else:
          jpvalue = getattr(jobproperties.tauRecFlags, i)()
          i_value=RecExCommonFlags.get(i)
          if i_value!=jpvalue:
             logRecExCommon_flags.info('inconsistency: for flag %s new flag: %s old flag:%s',i,jpvalue,i_value)
    # hack, do not migrate indet doPerfMon         
    if(jobproperties.InDetJobProperties.__dict__.keys().__contains__(inew)) and inew!= 'doPerfMon':
       if i in varInit:
          # i_value=RecExCommonFlags.get(i)
          i_value = eval ('%s' % (i))
          data={'JobProperties.InDetJobProperties':{inew:i_value}}
          jobproperties._log.info('RecExCommon_flags:: migration LOCKED data %s',data)  
          jobproperties.set_JobProperties(data)
          #for RecAlgFlags : lock the value
          getattr(jobproperties.InJetJobProperties, inew)._locked = True
       else:
          jpvalue = getattr(jobproperties.InDetJobProperties, inew)()
          i_value=RecExCommonFlags.get(i)
          if i_value!=jpvalue:
             logRecExCommon_flags.info('inconsistency: for flag %s new flag: %s old flag:%s',i,jpvalue,i_value)
    if(jobproperties.MuonRec.__dict__.keys().__contains__(inew)):
       if i in varInit:
          # i_value=RecExCommonFlags.get(i)
          i_value = eval('%s' % (i))
          data={'JobProperties.MuonRec':{inew:i_value}}
          jobproperties._log.info('RecExCommon_flags:: migration LOCKED data %s',data)  
          jobproperties.set_JobProperties(data)
          #for RecAlgFlags : lock the value
          getattr(jobproperties.MuonRec, inew)._locked = True
       else:
          jpvalue = getattr(jobproperties.MuonRec, inew)()
          i_value=RecExCommonFlags.get(i)
          if i_value!=jpvalue:
             logRecExCommon_flags.info('inconsistency: for flag %s new flag: %s old flag:%s',i,jpvalue,i_value)
    if(jobproperties.Global.__dict__.keys().__contains__(i)):
       if i in varInit:
          i_value = eval('%s' % (i))
          data={'JobProperties.Global':{i:i_value}}
          jobproperties._log.info('RecExCommon_flags:: migration data %s',data)  
          jobproperties.set_JobProperties(data)

       else:
          i_value=RecExCommonFlags.get(i)
          jpvalue = getattr(jobproperties.Global, i)()
          if i_value!=jpvalue:
             logRecExCommon_flags.info('inconsistency: for flag %s new flag: %s old flag:%s',i,jpvalue,i_value)

    if(jobproperties.AthenaCommonFlags.__dict__.keys().__contains__(i)):
       if i in varInit:
          i_value = eval('%s' % (i))
          data={'JobProperties.AthenaCommonFlags':{i:i_value}}
          jobproperties._log.info('RecExCommon_flags:: migration data %s',data)
          jobproperties.set_JobProperties(data)
       else:   
          i_value=RecExCommonFlags.get(i)
          jpvalue = getattr(jobproperties.AthenaCommonFlags, i)()
          if i!="PoolRDOInput":
            if i_value!=jpvalue:
               logRecExCommon_flags.info('inconsistency: for flag %s new flag: %s old flag:%s',i,jpvalue,i_value)
          else:   
            if i_value!=jpvalue:
               logRecExCommon_flags.info ('Modified value for flag %s new flag: %s old flag:%s',i,jpvalue,i_value)


# some special migration (temporary)
if rec.doFloatingPointException() and not athenaCommonFlags.isOnline():
   from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
   athenaCommonFlags.RuntimeStrictness='abort'
else:
   from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
   athenaCommonFlags.RuntimeStrictness='none'

#enable floating point exception
## if not athenaCommonFlags.isOnline():
##    from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
##    athenaCommonFlags.RuntimeStrictness='abort'




#FIXME temporary
if 'doJetRec' in varInit:
   try:
      from JetRec.JetRecFlags import jobproperties
      jobproperties.JetRecFlags.Enabled.set_Value(doJetRec)
   except Exception:
      treatException("Could not set JetRecFlags.Enabled")

      


varInit=dir()
#delete again !
if not rec.oldFlagCompatibility():
   printfunc ("oldFlagCompatibility==True : Deleting all standalone flags!")
   for i in RecExCommonFlags.keys(): 
      # delete old flags unless required otherwise
      if i in varInit:
         printfunc ("now deleting:",i)
         exec ('del %s' % i)




if athenaCommonFlags.FilesInput()==[]:
   from RecExConfig.RecoFunctions import InputFileNames
   athenaCommonFlags.FilesInput=InputFileNames()
   logRecExCommon_flags.info("athenaCommonFlags.FilesInput override input file specification")
# from now on athenaCommonFlags.FilesInput() can reliably be used to have input file list

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



   
#now read input file to set up some flags accordingly if possible
# load an OKS with the input data file
from AthenaCommon.Include import excludeTracePattern
excludeTracePattern.append("*/PyUtils/*")


# if autoconfig not used, switched of doTruth for data
if globalflags.DataSource=='data' and not 'DoTruth' in rec.AutoConfiguration():
   rec.doTruth=False


try:
   if rec.doTrigger():
      from TriggerJobOpts.TriggerFlags import TriggerFlags, sync_Trigger2Reco
except Exception:
   logRecExCommon_flags.info("Could not load TriggerFlags. OK only if AtlasReconstruction. Switching of rec.doTrigger")
   rec.doTrigger=False


#load commissioning flags here (major cleanup needed)
if rec.Commissioning():
   #--------------------------------------------------------------
   # Common flags with RecExCommon
   #--------------------------------------------------------------
   #if rec.readRDO:
   #   include( "RecExCommission/RecExCommissionCommonFlags_jobOptions.py" )


   # ---------------------------------------------------------------------------
   # AthenaCommonFlags
   # ---------------------------------------------------------------------------

   # start using the new job properties
   from AthenaCommon.JobProperties import jobproperties

   # AthenaCommon flags
   from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags

   # -----------------------------------------------------------------------
   # being general reconstruction flags
   # -----------------------------------------------------------------------
   from RecExConfig.RecFlags  import jobproperties, rec

   athenaCommonFlags.PoolInputQuery = "EventNumber>10"
   # GlobalFlags 
   from AthenaCommon.GlobalFlags import globalflags

   rec.RootHistoOutput = "monitoring.root"	
   rec.RootNtupleOutput = "ntuple.root"	


   # ----------------------------------------------------------------------
   # being flags to switch algorithm on/off
   # ----------------------------------------------------------------------
   from RecExConfig.RecAlgsFlags  import recAlgs

   #recAlgs.doTrigger 

   #
   # hack...
   #
   from RecExConfig.RecFlags import rec
   if not 'InDetKeys' in dir():
       #
       # --- setup StoreGate keys (JobProperties!)
       #
       printfunc ("InDetRec_jobOptions: InDetKeys not set before - I import them now")
       from InDetRecExample.InDetKeys import InDetKeys



   #--------------------------------------------------------------
   # Internal flags for ATLAS RecExCommission
   #--------------------------------------------------------------
   
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

               
   from AthenaMonitoring.DQMonFlags import DQMonFlags
   if rec.doESD() and DQMonFlags.doCTPMon() and rec.doTrigger():
      include("TrigT1CTMonitoring/CTPFlags.py")

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



logRecExCommon_flags.info('done the mapping in between the old flags and JobProperties')
# mgallas end 



# if production switch off resilience
if rec.Production():
   athenaCommonFlags.AllowIgnoreConfigError=False
   from RecExConfig.RecConfFlags import recConfFlags   
   recConfFlags.AllowIgnoreConfigError=False
#take default values
#recConfFlags.AllowIgnoreExistingDataObject=True
#recConfFlags.AllowIgnoreConfigError=True
#recConfFlags.AllowDisable=True



if not rec.oldFlagTopSteering and rec.oldFlagCompatibility:
   raise RuntimeError ("rec.oldFlagTopSteering false inconsistent with rec.oldFlagCompatibility true")


   # put back oldFlagCompatibility which has just been deleted ! Needed further down.   
   #oldFlagCompatibility=False   

#now some compatibility set up and check




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

   try:      
      from CaloRec.CaloRecFlags import jobproperties
      jobproperties.CaloRecFlags.Enabled=False
   except Exception:
      treatException("Could not instantiate CaloRecFlags")

   try:
      from InDetRecExample.InDetJobProperties import InDetFlags
      jobproperties.InDetJobProperties.disableInDetReco=True
   except Exception:
      treatException("Could not instantiate InDetFlags")

   try:
      from MuonRecExample.MuonRecFlags import muonRecFlags
      jobproperties.MuonRec.Enabled=False
   except Exception:
      treatException("Could not instantiate muonRecFlags")

   try:
      from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags 
      jobproperties.MuonCombinedRec.Enabled=False 
   except Exception: 
      treatException("Could not instantiate muonCombinedRecFlags") 

   try:
      from egammaRec.egammaRecFlags import jobproperties
      jobproperties.egammaRecFlags.Enabled=False
   except Exception:
      treatException("Could not instantiate egammaRecFlags")

   try:
      from JetRec.JetRecFlags import jobproperties
      jobproperties.JetRecFlags.Enabled=False
   except Exception:
      treatException("Could not instantiate JetRecFlags")
      

   try:
      from tauRec.tauRecFlags import jobproperties
      jobproperties.tauRecFlags.Enabled=False
   except Exception:
      treatException("Could not instantiate tauRecFlags")


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
   try:
      jobproperties.egammaRecFlags.Enabled=False
   except Exception:
      pass

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


# move up, need to know earlier if trigger is on
#try:
#   if rec.doTrigger():
#      from TriggerJobOpts.TriggerFlags import TriggerFlags, sync_Trigger2Reco
#except Exception:
#   logRecExCommon_flags.info("Could not load TriggerFlags. OK only if AtlasReconstruction. Switching of rec.doTrigger")
#   rec.doTrigger=False


if not rec.doTrigger:
   #DR should use finer grain
   #rec.doDPD=False
   rec.doTagRawSummary=False
   rec.doBeamBackgroundFiller=False

   #FIXME a separate container to be used eventually
   recAlgs.doTrigger=False
   if _AODFlagsAvailable:
      AODFlags.Trigger=False # obsolete

   

#obsolete
readMuonDigit=False

# internal tracking configuration
#if not 'doNotLoadInDetFlags' in varInit:
#   doNotLoadInDetFlags=False


# FIXME functionality is needed !!!
##    try:
##       if rec.doLowPt:
##          include ("InDetRecExample/ConfiguredInDetCutValues.py")
##          InDetCutValues=ConfiguredInDetCutValues(minPT=0.1*GeV)

##       include ( "InDetRecExample/ConfiguredInDetFlags.py" )
##    except Exception:
##       logRecExCommon_flags.warning("ConfiguredInDetFlags unavailable. ")
##       doNotLoadInDetFlags=True

#if not doNotLoadInDetFlags:

try:
   from InDetRecExample.InDetJobProperties import InDetFlags
   if not rec.doTruth():
      InDetFlags.doTruth       = False
except Exception:
   treatException("Could not instantiate InDetFlags=>switch off ID ")
   DetFlags.ID_setOff()




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
 


# if want to write tag : at least switch on trigger configuration
doTriggerConfigOnly=False
if rec.doWriteTAG() and not recAlgs.doTrigger() and not ('EventTagFlags' in dir() and not EventTagFlags.doTrigger()) and rec.doTrigger():
   try:
      from TriggerJobOpts.TriggerFlags import TriggerFlags
      doTriggerConfigOnly=True
      jobproperties.Trigger.doTriggerConfigOnly=True
   except:
      logRecExCommon_flags.info("TriggerJobOpts.TriggerFlags not available.")

if not recAlgs.doTrigger() and not rec.readAOD() and not rec.readESD() and rec.doWriteTAG():
    logRecExCommon_flags.warning("no trigger and running from RDO: cannot write out tag, switching of trigger part of tag")
    include ('EventTagAlgs/EventTagFlags.py') 
    EventTagFlags.set_TriggerOff() 

if rec.doWriteTAG():
   include ('EventTagAlgs/EventTagFlags.py') 
   if not rec.doEgamma():
      EventTagFlags.set_ElectronOff()
      EventTagFlags.set_PhotonOff()
   if not rec.doJetMissingETTag():
      EventTagFlags.set_MissingETOff()
      EventTagFlags.set_ParticleJetOff()
   if not rec.doMuon():
      EventTagFlags.set_MuonOff()
   if not rec.doTau():
      EventTagFlags.set_TauJetOff()
   
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
   # myathpath=os.getenv("CMTPATH")
   myathpath=os.getenv("LD_LIBRARY_PATH")
   pos=myathpath.find("LCGCMT_")
   if(pos>-1):
      rec.LCGCMTVersion=myathpath[pos+7:pos+10]

if rec.doTrigger():
   from TriggerJobOpts.TriggerFlags import TriggerFlags, sync_Trigger2Reco
   sync_Trigger2Reco()
      

class landmine:
   def __nonzero__(self):
      raise RuntimeError ("usage of this flag is forbidden!")

#delete again !
varInit=dir()
if not rec.oldFlagCompatibility():
   printfunc ("oldFlagCompatibility==True : Deleting all standalone flags! and forbidding use of old flags")
   for i in RecExCommonFlags.keys(): 
      if i in varInit:
         printfunc ("deleting again:",i)
         exec ('del %s' % i)
      # if true this will prevent reuse of old flags   
      # May 2011: actually this does not seem to work anymore. Just ignored.
      if rec.oldFlagLandMine():
         globals()[i] = landmine()

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
