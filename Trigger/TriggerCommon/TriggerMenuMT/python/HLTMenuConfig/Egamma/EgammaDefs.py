# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------
# Static classes to configure photon chain container names
#----------------------------------------------------------------

from TrigEDMConfig.TriggerEDMRun3 import recordable
from AthenaCommon.Logging import logging
from AthenaCommon import CfgMgr
from ROOT import egammaPID
from ElectronPhotonSelectorTools.ConfiguredAsgPhotonIsEMSelectors import ConfiguredAsgPhotonIsEMSelector

mlog = logging.getLogger ('EgammaDefs')

class TrigEgammaKeys(object):
      """Static class to collect all string manipulation in Electron sequences """
      SuperElectronRecCollectionName = 'HLT_ElectronSuperRecCollection'
      outputElectronKey = recordable('HLT_egamma_Electrons')
      SuperPhotonRecCollectionName = 'HLT_PhotonSuperRecCollection'
      EgammaRecKey = 'HLT_egammaRecCollection'
      outputPhotonKey = recordable('HLT_egamma_Photons')
      outputClusterKey = 'HLT_egammaClusters'
      outputTopoSeededClusterKey = 'HLT_egammaTopoSeededClusters'
      TrigEMClusterToolOutputContainer = 'HLT_TrigEMClusterOutput'
      TrigElectronTracksCollectionName = recordable('HLT_IDTrack_Electron_IDTrig')
      pidVersion = 'rel21_20180312'


class TrigEgammaKeys_GSF(object):
      """Static class to collect all string manipulation in Electron sequences """
      outputElectronKey_GSF = recordable('HLT_egamma_Electrons_GSF')
      outputTrackKey_GSF = 'HLT_IDTrkTrack_Electron_GSF'
      outputTrackParticleKey_GSF = recordable('HLT_IDTrack_Electron_GSF')
      
      # Print configuration once:
      mlog.info("TrigEgammaPidTools version %s", TrigEgammaKeys.pidVersion)

def TrigElectronSelectors():

    # Configure the LH selectors
    #TrigEgammaKeys.pidVersion.set_On()
    ConfigFilePath = 'ElectronPhotonSelectorTools/trigger/'+TrigEgammaKeys.pidVersion
    
    SelectorNames = {
          'lhvloose':'AsgElectronLHVLooseSelector',
          'lhloose':'AsgElectronLHLooseSelector',
          'lhmedium':'AsgElectronLHMediumSelector',
          'lhtight':'AsgElectronLHTightSelector',
          }
     
    ElectronToolConfigFile = {
          'lhvloose':'ElectronLikelihoodVeryLooseTriggerConfig.conf',
          'lhloose':'ElectronLikelihoodLooseTriggerConfig.conf',
          'lhmedium':'ElectronLikelihoodMediumTriggerConfig.conf',
          'lhtight':'ElectronLikelihoodTightTriggerConfig.conf',
          }

    mlog.debug('Configuring electron PID' )
    SelectorTool_vloose=CfgMgr.AsgElectronLikelihoodTool(SelectorNames['lhvloose'])
    SelectorTool_vloose.ConfigFile = ConfigFilePath + '/' + ElectronToolConfigFile['lhvloose']
    SelectorTool_vloose.usePVContainer = False 
    SelectorTool_vloose.skipDeltaPoverP = True

    SelectorTool_loose=CfgMgr.AsgElectronLikelihoodTool(SelectorNames['lhloose'])
    SelectorTool_loose.ConfigFile = ConfigFilePath + '/' + ElectronToolConfigFile['lhloose']
    SelectorTool_loose.usePVContainer = False
    SelectorTool_loose.skipDeltaPoverP = True

    SelectorTool_medium=CfgMgr.AsgElectronLikelihoodTool(SelectorNames['lhmedium'])
    SelectorTool_medium.ConfigFile = ConfigFilePath + '/' + ElectronToolConfigFile['lhmedium']
    SelectorTool_medium.usePVContainer = False
    SelectorTool_medium.skipDeltaPoverP = True

    SelectorTool_tight=CfgMgr.AsgElectronLikelihoodTool(SelectorNames['lhtight'])
    SelectorTool_tight.ConfigFile = ConfigFilePath + '/' + ElectronToolConfigFile['lhtight']
    SelectorTool_tight.usePVContainer = False
    SelectorTool_tight.skipDeltaPoverP = True

    return SelectorTool_vloose, SelectorTool_loose, SelectorTool_medium, SelectorTool_tight


def TrigPhotonSelectors(sel):
    # Configure the IsEM selectors

    SelectorNames = {
            'loose'  : 'LoosePhotonSelector',
            'medium' : 'MediumPhotonSelector',
            'tight'  : 'TightPhotonSelector',
            }
    SelectorPID = {
            'loose'  : egammaPID.PhotonIDLoose,
            'medium' : egammaPID.PhotonIDMedium,
            'tight'  : egammaPID.PhotonIDTight,
            }
    PhotonToolConfigFile = {
            'loose'  : 'PhotonIsEMLooseSelectorCutDefs.conf', 
            'medium' : 'PhotonIsEMMediumSelectorCutDefs.conf', 
            'tight'  : 'PhotonIsEMTightSelectorCutDefs.conf',
            } 
    PhotonIsEMBits = {
            'loose'  : egammaPID.PhotonLooseEF,
            'medium' : egammaPID.PhotonMediumEF,
            'tight'  : egammaPID.PhotonTight,
            }

    if sel not in SelectorNames:
        mlog.error('No selector defined for working point %s for photons :-( ', sel)
        return
    else:
        mlog.debug('Configuring photon PID for %s', sel)
        SelectorTool = ConfiguredAsgPhotonIsEMSelector(SelectorNames[sel], SelectorPID[sel])
        ConfigFilePath = 'ElectronPhotonSelectorTools/trigger/'+TrigEgammaKeys.pidVersion
        ConfigFile = ConfigFilePath + '/' + PhotonToolConfigFile[sel] 
        mlog.debug('Configuration file: %s', ConfigFile)
        SelectorTool.ConfigFile = ConfigFile
        SelectorTool.ForceConvertedPhotonPID = True
        SelectorTool.isEMMask = PhotonIsEMBits[sel] 

        return SelectorTool
