# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
## @file DataQualityTools/python/DQMonFlags.py
## @purpose Python module to hold common flags to configure JobOptions
##

""" DQMonFlags
    Python module to hold flags for Data Quality monitoring

"""

__author__ = "S. Schaetzel"
__version__= "$Revision: 1.11 $"
__doc__    = "DQMonFlags"

__all__    = [ "DQMonFlags" ]

# kindly stolen from InDetKeys by A. Wildauer

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes
list=[]
class doMonitoring(JobProperty):
    """ Global switch for monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doMonitoring]

class doGlobalMon(JobProperty):
    """ Switch for global combined monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doGlobalMon]

class doLVL1CaloMon(JobProperty):
    """ Switch for L1 calo monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doLVL1CaloMon]

class doCTPMon(JobProperty):
    """ Switch for CTP monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doCTPMon]

class doHLTMon(JobProperty):
    """ Switch for HLT monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doHLTMon]

class doPixelMon(JobProperty):
    """ Switch for pixel monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doPixelMon]

class doSCTMon(JobProperty):
    """ Switch for SCT monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doSCTMon]

class doTRTMon(JobProperty):
    """ Switch for TRT monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doTRTMon]

class doTRTElectronMon(JobProperty):
    """ Switch for TRT electron monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doTRTElectronMon]

class doInDetGlobalMon(JobProperty):
    """ Switch for inner detector global monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doInDetGlobalMon]

class doInDetAlignMon(JobProperty):
    """ Switch for inner detector alignment monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doInDetAlignMon]

class doInDetPerfMon(JobProperty):
    """ Switch for inner detector performance monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doInDetPerfMon]

class doTileMon(JobProperty):
    """ Switch for tile calorimeter monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doTileMon]

class doLArMon(JobProperty):
    """ Switch for LAr calorimeter monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doLArMon]

class doCaloMon(JobProperty):
    """ Switch for combined calorimeter monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doCaloMon]

class doEgammaMon(JobProperty):
    """ Switch for e/gamma monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doEgammaMon]

class doJetMon(JobProperty):
    """ Switch for jet monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doJetMon]

class doMissingEtMon(JobProperty):
    """ Switch for missing E_T monitoring"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doMissingEtMon]

class doTauMon(JobProperty):
    """ Switch for tau monitoring"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doTauMon]

class doJetTagMon(JobProperty):
    """ Switch for jet-tag monitoring"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doJetTagMon]

class doMuonRawMon(JobProperty):
    """ Switch for muon spectrometer raw data monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doMuonRawMon]

class doMuonTrackMon(JobProperty):
    """ Switch for muon track monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doMuonTrackMon]

class doMuonAlignMon(JobProperty):
    """ Switch for muon alignment monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doMuonAlignMon]

class doMuonSegmentMon(JobProperty):
    """ Switch for muon segment monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[doMuonSegmentMon]

class doMuonPhysicsMon(JobProperty):
    """ Switch for muon physics monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[doMuonPhysicsMon]

class doMuonTrkPhysMon(JobProperty):
    """ Switch for muon track physics monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doMuonTrkPhysMon]

class doMuonCombinedMon(JobProperty):
    """ Switch for muon combined monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doMuonCombinedMon]

class doLucidMon(JobProperty):
    """ Switch for LUCID monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doLucidMon]

class doAFPMon(JobProperty):
    """Switch for AFP monitoring"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doAFPMon]

class doHIMon(JobProperty):
    """ Switch for dedicated Heavy Ion monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[doHIMon]

class doStreamAwareMon(JobProperty):
    """ Switch for stream-aware monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[doStreamAwareMon]

class monType(JobProperty):
    """ type of monitoring (BS, ESD)"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'BS_ESD'
list+=[monType]

class monManEnvironment(JobProperty):
    """ MonManager environment """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'tier0ESD'
list+=[monManEnvironment]

class monManFileKey(JobProperty):
    """ MonManager file key """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'CombinedMonitoring'
list+=[monManFileKey]

class monManDataType(JobProperty):
    """ MonManager data type """
    statusOn=True
    allowedTypes=['str']
    StoredValue='collisions'
list+=[monManDataType]

class monManLBsInLowStatInterval(JobProperty):
    """ number of LBs in low statistics interval """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 20
list+=[monManLBsInLowStatInterval]

class monManLBsInMediumStatInterval(JobProperty):
    """ number of LBs in medium statistics interval """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 60
list+=[monManLBsInMediumStatInterval]

class monManLBsInHighStatInterval(JobProperty):
    """ number of LBs in high statistics interval """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 180
list+=[monManLBsInHighStatInterval]

class monManManualDataTypeSetup(JobProperty):
    """ MonManager ManualDataTypeSetup """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[monManManualDataTypeSetup]

class monManManualRunLBSetup(JobProperty):
    """ MonManager ManualRunLBSetup """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[monManManualRunLBSetup]

class monManRun(JobProperty):
    """ MonManager Run """
    statusOn=True
    allowedTypes=['int']
    StoredValue=1
list+=[monManRun]

class monManLumiBlock(JobProperty):
    """ MonManager LumiBlock """
    statusOn=True
    allowedTypes=['int']
    StoredValue=1
list+=[monManLumiBlock]

class useTrigger(JobProperty):
    """ use trigger (trigger-aware monitoring) """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[useTrigger]

class monManMaxPrescale(JobProperty):
    """ MonManager maxPrescale """
    statusOn=True
    allowedTypes=['int']
    StoredValue=1000000
list+=[monManMaxPrescale]

class triggerChainMap(JobProperty):
    """ Trigger map of toolname -> chain """
    statusOn=True
    allowedTypes=['dict']
    StoredValue={}
list+=[triggerChainMap]    

class monManStream(JobProperty):
    """ MonManager data processing stream """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'express'
list+=[monManStream]

class histogramFile(JobProperty):
    """ output file with histograms """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'Monitor.root'
list+=[histogramFile]

class nameTrigDecTool(JobProperty):
    """ name of trigger decision tool """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TrigDecisionTool'
list+=[nameTrigDecTool]

class nameTrigTransTool(JobProperty):
    """ name of trigger translator tool """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'monTrigTransTool'
list+=[nameTrigTransTool]

class monToolPostExec(JobProperty):
    """ MonManager environment """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = None
list+=[monToolPostExec]

class disableAtlasReadyFilter(JobProperty):
    """ Disable DQAtlasReadyFilterTool (will always return true if this is set) """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[disableAtlasReadyFilter]

class disableFilledBunchFilter(JobProperty):
    """ Disable DQFilledBunchFilterTool (will always return true if this is set) """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[disableFilledBunchFilter]

class enableLumiAccess(JobProperty):
    """ If set to false, will force lumi tool access off """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
list+=[enableLumiAccess]

class includeInCleaning(JobProperty):
    """ Tools matching regexes in this list will have event cleaning tool set up """
    statusOn=True
    allowedTypes=['list']
    StoredValue=[ '.*JetMonitoring.*', '.*METMonTool.*', '.*tauMonTool.*',
                  '.*DQTGlobalWZFinder.*', '.*jetTagMon.*', '.*phMonTool.*',
                  '.*elMonTool.*', '.*fwdMonTool.*' ]
list+=[includeInCleaning]

class specialCleaningConfiguration(JobProperty):
    """ Special event cleaning configurations (no regexes) """
    statusOn=True
    allowedTypes=['dict']
    StoredValue={}
list+=[specialCleaningConfiguration]

class doNewMonitoring(JobProperty):
    """ Global switch for monitoring """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
list+=[doNewMonitoring]

##-----------------------------------------------------------------------------
## 2nd step
## Definition of the DQMon flag container
class DQMonFlagsCont(JobPropertyContainer):
    """Container for Data Quality Monitoring flags
    """
    def set_All_Off(self):
        for j in list:
            if j.__name__.startswith('do') and j.__name__.endswith('Mon'):
                j.StoredValue = False
    pass

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(DQMonFlagsCont)

##-----------------------------------------------------------------------------
## 4th step
## adding properties to DQMonFlags container
for j in list:
   jobproperties.DQMonFlagsCont.add_JobProperty(j)

##-----------------------------------------------------------------------------
## 5th step
## short-cut 
DQMonFlags = jobproperties.DQMonFlagsCont
