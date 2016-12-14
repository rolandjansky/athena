#!/usr/bin/python
# vim: set fileencoding=utf-8 :

########################### RINGER CONF #################################
#########################################################################
from CaloRingerAlgs.CaloRingerFlags import caloRingerFlags
caloRingerFlags.useAsymBuilder.set_Value_and_Lock(False)
caloRingerFlags.buildElectronCaloRings.set_Value_and_Lock(True)
caloRingerFlags.minElectronEnergy.set_Value_and_Lock(None)
caloRingerFlags.doElectronIdentification.set_Value_and_Lock(True)
caloRingerFlags.buildPhotonCaloRings.set_Value_and_Lock(True)
caloRingerFlags.minPhotonEnergy.set_Value_and_Lock(None)
caloRingerFlags.OutputLevel.set_Value_and_Lock(VERBOSE)
#########################################################################

####################### CHANGE CONFIGURATION HERE  ######################
#########################################################################
doDumpStoreGate = False
ManualDetDescrVersion = 'ATLAS-R2-2015-01-01-00'
ConditionsTag = "CONDBR2-BLKPA-2016-11"
from AtlasGeoModel.SetGeometryVersion import GeoModelSvc
GeoModelSvc.IgnoreTagSupport = True
GeoModelSvc.AtlasVersion = ManualDetDescrVersion
###########################  REC FLAGS  #################################
from RecExConfig.RecFlags import rec
rec.OutputLevel.set_Value_and_Lock(INFO)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doCBNT.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(True)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doFileMetaData.set_Value_and_Lock(True)
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
from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.PoolRDOInput=jobproperties.AthenaCommonFlags.FilesInput.get_Value()
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
numEvt = 26
if 'file' in dir():
  athenaCommonFlags.PoolRDOInput = [file]
athenaCommonFlags.EvtMax = numEvt
athenaCommonFlags.PoolAODOutput = 'AOD.pool.root'
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
from RecExConfig.InputFilePeeker import inputFileSummary
globalflags.DataSource = 'data' if inputFileSummary['evt_type'][0] == "IS_DATA" else 'geant4'
#########################################################################

########################### PRE-EXEC ####################################
#########################################################################
# Insert Reconstruction pre-execs here.
#########################################################################

########################### PRE-EXTRA ###################################
#########################################################################
# You may add you own extra stuff in here. An example that may be useful
# follows.
#########################################################################

################## MAIN REC JOBOPTION INCLUDE: ##########################
#########################################################################
include( "RecExCommon/RecExCommon_topOptions.py" )
#########################################################################

###########################  Ringer!!! ##################################
#########################################################################
include('CaloRingerAlgs/CaloRinger_jobOptions.py')
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
# Insert extra post here
#########################################################################
