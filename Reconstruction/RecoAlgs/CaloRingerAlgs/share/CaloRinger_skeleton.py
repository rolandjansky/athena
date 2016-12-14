#!/usr/bin/python
# vim: set fileencoding=utf-8 :

########################### SOME RINGER CONFIG ##########################
#########################################################################
from CaloRingerAlgs.CaloRingerFlags import caloRingerFlags
caloRingerFlags.useAsymBuilder.set_Value_and_Lock(False)
caloRingerFlags.doElectronIdentification.set_Value_and_Lock(False)
caloRingerFlags.doPhotonIdentification.set_Value_and_Lock(False)
caloRingerFlags.OutputLevel.set_Value_and_Lock(DEBUG)
#########################################################################

####################### CHANGE CONFIGURATION HERE  ######################
#########################################################################
localRunOnFolder = '/tmp/nbullacr/mc14_13TeV.147406.PowhegPythia8_AZNLO_Zee.recon.RDO.e3059_s1982_s2008_r5993_tid05320067_00/'
doCaloRinger = True
doDumpStoreGate = False
ManualDetDescrVersion = 'ATLAS-R2-2015-01-01-00' # Set to False or empty if you want it to be automatically set.
ConditionsTag = "OFLCOND-RUN12-SDR-14"
###########################  REC FLAGS  #################################
from RecExConfig.RecFlags import rec
rec.OutputLevel.set_Value_and_Lock(INFO)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doCBNT.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(True)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(True)
rec.doWriteAOD.set_Value_and_Lock(True)
rec.doTrigger.set_Value_and_Lock(False)
rec.doJetMissingETTag.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)
rec.doMuonCombined.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)
rec.doPerfMon.set_Value_and_Lock(False)
rec.doDetailedPerfMon.set_Value_and_Lock(False)
rec.doHist.set_Value_and_Lock(False)
rec.doPyDump.set_Value_and_Lock(False)
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.AllowIgnoreConfigError = False
#########################################################################

#########################################################################
####################### Some autoconfiguration: #########################
# Add files to input picker if running on local:
if localRunOnFolder:
  import os
  # Put dir for your data here:
  f = os.popen('ls '+localRunOnFolder)
  files = []
  for j in f:
    i = j[0:-1]
    files += [localRunOnFolder+i]
  files.sort()
  from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
  numEvt = -1 
  athenaCommonFlags.EvtMax = numEvt
  athenaCommonFlags.FilesInput = files
  athenaCommonFlags.PoolAODOutput = 'AOD.pool.root'
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
#here is an example of the inputFilePeeker to autoconfigure the flags
from RecExConfig.InputFilePeeker import inputFileSummary
globalflags.DataSource = 'data' if inputFileSummary['evt_type'][0] == "IS_DATA" else 'geant4'
if not ManualDetDescrVersion:
  globalflags.DetDescrVersion.set_Value_and_Lock(inputFileSummary['geometry'])
else:
  globalflags.DetDescrVersion.set_Value_and_Lock(ManualDetDescrVersion)
globalflags.ConditionsTag.set_Value_and_Lock(ConditionsTag)
#from AthenaCommon.DetFlags import DetFlags
#DetFlags.detdescr.all_setOff() # skip this line out to leave everything on. But this will take longer
#DetFlags.detdescr.Calo_setOn() # e.g. if I am accessing CaloCellContainer, I need the calo detector description
#include("RecExCond/AllDet_detDescr.py")
#########################################################################

########################### PRE-INCLUDE #################################
#########################################################################
# Include Reconstruction pre-includes here.
#########################################################################

########################### PRE-EXEC ####################################
#########################################################################
# Insert Reconstruction pre-execs here.
#########################################################################

########################### PRE-EXTRA ###################################
#########################################################################
# You may add you own extra stuff in here. An example that may be useful
# follows.
##from AthenaCommon.JobProperties import jobproperties
##jobproperties.Beam.energy.set_Value_and_Lock(7000*Units.GeV)
##jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(27.5)
##jobproperties.Beam.bunchSpacing.set_Value_and_Lock(25)
#########################################################################

################## MAIN REC JOBOPTION INCLUDE: ##########################
#########################################################################
include( "RecExCommon/RecExCommon_topOptions.py" )
#########################################################################

###########################  Ringer!!! ##################################
#########################################################################
if doCaloRinger:
  include('CaloRingerAlgs/CaloRinger_joboptions.py') 
#########################################################################

########################### POST-INCLUDE ################################
#########################################################################
# Insert Reconstruction post-includes here.
#########################################################################

########################### POST-EXEC ###################################
#########################################################################
# Insert Reconstruction post-execs here.
#########################################################################

########################### POST-EXTRA ##################################
#########################################################################
if doDumpStoreGate:
  StoreGateSvc = Service( "StoreGateSvc" )                     
  StoreGateSvc.Dump = True  #true will dump data store contents
  StoreGateSvc.OutputLevel = DEBUG
#########################################################################
