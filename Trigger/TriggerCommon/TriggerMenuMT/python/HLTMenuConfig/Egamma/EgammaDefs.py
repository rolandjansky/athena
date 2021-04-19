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

class TrigEgammaKeys_LRT(object):
      """Static class to collect all string manipulation in Electron_LRT sequences """
      outputElectronKey_LRT = recordable('HLT_egamma_Electrons_LRT')
      TrigElectronTracksCollectionName_LRT = recordable('HLT_IDTrack_ElectronLRT_IDTrig')

class TrigEgammaKeys_GSF(object):
      """Static class to collect all string manipulation in Electron sequences """
      outputElectronKey_GSF = recordable('HLT_egamma_Electrons_GSF')
      outputTrackKey_GSF = 'HLT_IDTrkTrack_Electron_GSF'
      outputTrackParticleKey_GSF = recordable('HLT_IDTrack_Electron_GSF')
      


#
# Electron DNN Selectors
#
#def createTrigEgammaPrecisionElectronDNNSelectors(ConfigFilePath=None):
# We should include the DNN here


#
# Electron LH Selectors
#
def createTrigEgammaPrecisionElectronLHSelectors(ConfigFilePath=None):

    # Configure the LH selectors
    #TrigEgammaKeys.pidVersion.set_On()
    if not ConfigFilePath:
      ConfigFilePath = 'ElectronPhotonSelectorTools/trigger/'+TrigEgammaKeys.pidVersion

    import collections
    SelectorNames = collections.OrderedDict({
          'lhtight':'AsgElectronLHTightSelector',
          'lhmedium':'AsgElectronLHMediumSelector',
          'lhloose':'AsgElectronLHLooseSelector',
          'lhvloose':'AsgElectronLHVLooseSelector',
          })
     
    ElectronToolConfigFile = collections.OrderedDict({
          'lhtight':'ElectronLikelihoodTightTriggerConfig.conf',
          'lhmedium':'ElectronLikelihoodMediumTriggerConfig.conf',
          'lhloose':'ElectronLikelihoodLooseTriggerConfig.conf',
          'lhvloose':'ElectronLikelihoodVeryLooseTriggerConfig.conf',
          })

    selectors = []
    mlog.debug('Configuring electron PID' )
    for pidname, name in SelectorNames.items():
      SelectorTool = CfgMgr.AsgElectronLikelihoodTool(name)
      SelectorTool.ConfigFile = ConfigFilePath + '/' + ElectronToolConfigFile[pidname]
      SelectorTool.usePVContainer = False 
      SelectorTool.skipDeltaPoverP = True
      selectors.append(SelectorTool)

    return selectors



#
# Photon IsEM selectors
#
def createTrigEgammaPrecisionPhotonSelectors(ConfigFilePath=None):

    if not ConfigFilePath:
      ConfigFilePath = 'ElectronPhotonSelectorTools/trigger/'+TrigEgammaKeys.pidVersion

    import collections
    # Configure the IsEM selectors
    SelectorNames = collections.OrderedDict( {
            'tight'  : 'TightPhotonSelector',
            'medium' : 'MediumPhotonSelector',
            'loose'  : 'LoosePhotonSelector',
            } )
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

    selectors = []
    for sel, name in SelectorNames.items():
        mlog.debug('Configuring photon PID for %s', sel)
        SelectorTool = ConfiguredAsgPhotonIsEMSelector(name, SelectorPID[sel])
        ConfigFile = ConfigFilePath + '/' + PhotonToolConfigFile[sel] 
        mlog.debug('Configuration file: %s', ConfigFile)
        SelectorTool.ConfigFile = ConfigFile
        SelectorTool.ForceConvertedPhotonPID = True
        SelectorTool.isEMMask = PhotonIsEMBits[sel] 
        selectors.append(SelectorTool)

    return selectors



#
# Electron/Photon ringer NN selectors
#
def createTrigEgammaFastCaloSelectors(doPhotons=False, ConfigFilePath='RingerSelectorTools/TrigL2_20210227_r3'):

    from RingerSelectorTools.RingerSelectorToolsConf import Ringer__AsgRingerSelectorTool 
    from AthOnnxruntimeService.AthOnnxruntimeServiceConf import AthONNX__ONNXRuntimeSvc
    from AthenaCommon.AppMgr import ServiceMgr
    import collections
    # add ONNX into app service mgr
    ServiceMgr += AthONNX__ONNXRuntimeSvc()
    from AthenaCommon.Logging import logging
    log = logging.getLogger('TrigEgammaFastCaloSelectors') 

    SelectorNames = collections.OrderedDict( {
        "Electrons": collections.OrderedDict({
            'tight'    : 'AsgElectronFastCaloRingerTightSelectorTool',
            'medium'   : 'AsgElectronFastCaloRingerMediumSelectorTool',
            'loose'    : 'AsgElectronFastCaloRingerLooseSelectorTool',
            'vloose'   : 'AsgElectronFastCaloRingerVeryLooseSelectorTool',
            }),
        "Photons": collections.OrderedDict({
            'tight'  : 'AsgPhotonFastCaloRingerTightSelectorTool',
            'medium' : 'AsgPhotonFastCaloRingerMediumSelectorTool',
            'loose'  : 'AsgPhotonFastCaloRingerLooseSelectorTool',
          })
    } )
    
    ToolConfigFile = {
        "Electrons" : collections.OrderedDict({
          'tight'   :'ElectronRingerTightTriggerConfig.conf',
          'loose'   :'ElectronRingerLooseTriggerConfig.conf',
          'medium'  :'ElectronRingerMediumTriggerConfig.conf',
          'vloose'  :'ElectronRingerVeryLooseTriggerConfig.conf',
          }),
        "Photons" : collections.OrderedDict({
          'tight' :'PhotonRingerTightTriggerConfig.conf',
          'medium':'PhotonRingerMediumTriggerConfig.conf',
          'loose' :'PhotonRingerLooseTriggerConfig.conf',
        })
    }
    cand = 'Photons' if doPhotons else 'Electrons'
    selectors = []
    for pidname , name in SelectorNames[cand].items():
      log.debug('Configuring electron ringer PID for %s', pidname)
      SelectorTool=Ringer__AsgRingerSelectorTool(name)
      SelectorTool.ConfigFile = ConfigFilePath + '/' + ToolConfigFile[cand][pidname]
      selectors.append(SelectorTool)
    return selectors
