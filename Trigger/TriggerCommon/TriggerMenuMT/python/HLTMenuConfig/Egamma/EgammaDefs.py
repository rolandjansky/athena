#----------------------------------------------------------------
# Static classes to configure photon chain container names
#----------------------------------------------------------------

class TrigEgammaKeys(object):
      """Static class to collect all string manipulation in Electron sequences """
      from TrigEDMConfig.TriggerEDMRun3 import recordable
      SuperElectronRecCollectionName = 'HLT_ElectronSuperRecCollection'
      outputElectronKey = recordable('HLT_egamma_Electrons')
      SuperPhotonRecCollectionName = 'HLT_PhotonSuperRecCollection'
      EgammaRecKey = 'HLT_egammaRecCollection'
      outputPhotonKey = recordable('HLT_egamma_Photons')
      outputClusterKey = 'HLT_egammaClusters'
      outputTopoSeededClusterKey = 'HLT_egammaTopoSeededClusters'
      TrigEMClusterToolOutputContainer = 'HLT_TrigEMClusterOutput'
      pidVersion = 'ElectronPhotonSelectorTools/trigger/rel21_20180312'

def TrigPhotonSelectors(sel):
    import logging

    mlog = logging.getLogger ('EgammaDefs')

    # Configure the IsEM selectors
    from ElectronPhotonSelectorTools.ConfiguredAsgPhotonIsEMSelectors import ConfiguredAsgPhotonIsEMSelector
    from ROOT import egammaPID
    from ElectronPhotonSelectorTools.TrigEGammaPIDdefs import SelectionDefPhoton
    
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
            'loose'  : SelectionDefPhoton.PhotonLooseEF,
            'medium' : SelectionDefPhoton.PhotonMediumEF,
            'tight'  : SelectionDefPhoton.PhotonTight,
            }

    mlog.info('Configuring photon PID tools...')
    if sel not in SelectorNames:
        mlog.error('No selector defined for working point '+sel+' for photons :-( ')
        return
    else:
        mlog.info('Configuring photon PID for '+sel)
        SelectorTool = ConfiguredAsgPhotonIsEMSelector(SelectorNames[sel], SelectorPID[sel])
        ConfigFile = TrigEgammaKeys.pidVersion+'/'+PhotonToolConfigFile[sel] 
        mlog.info('Configuration file: '+ConfigFile)
        SelectorTool.ConfigFile = ConfigFile
        SelectorTool.ForceConvertedPhotonPID = True
        SelectorTool.isEMMask = PhotonIsEMBits[sel] 
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += SelectorTool

        return SelectorTool

