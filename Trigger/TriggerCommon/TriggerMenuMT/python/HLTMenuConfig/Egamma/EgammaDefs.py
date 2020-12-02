# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------
# Static classes to configure photon chain container names
#----------------------------------------------------------------

from TrigEDMConfig.TriggerEDMRun3 import recordable

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
      

def TrigElectronSelectors(sel):
    import logging

    mlog = logging.getLogger ('EgammaDefs')

    # Configure the LH selectors
    from AthenaCommon import CfgMgr
    #TrigEgammaKeys.pidVersion.set_On()
    mlog.info("TrigEgammaPidTools version %s", TrigEgammaKeys.pidVersion)
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
    

    mlog.info('Configuring electron PID tools...')
    if sel not in SelectorNames:
        mlog.error('No selector defined for working point %s for electrons :-( ', sel)
        return
    else:
        mlog.info('Configuring electron PID for %s', sel)
        SelectorTool=CfgMgr.AsgElectronLikelihoodTool(SelectorNames[sel])
        SelectorTool.ConfigFile = ConfigFilePath + '/' + ElectronToolConfigFile[sel]
        SelectorTool.usePVContainer = False 
        SelectorTool.skipDeltaPoverP = True
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += SelectorTool

        return SelectorTool


def TrigPhotonSelectors(sel):
    import logging

    mlog = logging.getLogger ('EgammaDefs')

    # Configure the IsEM selectors
    from ElectronPhotonSelectorTools.ConfiguredAsgPhotonIsEMSelectors import ConfiguredAsgPhotonIsEMSelector
    from ROOT import egammaPID
    
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

    mlog.info('Configuring photon PID tools...')
    if sel not in SelectorNames:
        mlog.error('No selector defined for working point %s for photons :-( ', sel)
        return
    else:
        mlog.info('Configuring photon PID for %s', sel)
        SelectorTool = ConfiguredAsgPhotonIsEMSelector(SelectorNames[sel], SelectorPID[sel])
        ConfigFilePath = 'ElectronPhotonSelectorTools/trigger/'+TrigEgammaKeys.pidVersion
        ConfigFile = ConfigFilePath + '/' + PhotonToolConfigFile[sel] 
        mlog.info('Configuration file: %s', ConfigFile)
        SelectorTool.ConfigFile = ConfigFile
        SelectorTool.ForceConvertedPhotonPID = True
        SelectorTool.isEMMask = PhotonIsEMBits[sel] 
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += SelectorTool

        return SelectorTool
