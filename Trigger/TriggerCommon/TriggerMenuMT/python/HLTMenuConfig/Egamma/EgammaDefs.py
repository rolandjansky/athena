
#----------------------------------------------------------------
# Static classes to configure photon chain container names
#----------------------------------------------------------------

class TrigEgammaKeys(object):
      """Static class to collect all string manipulation in Electron sequences """
      SuperElectronRecCollectionName = "HLT_ElectronSuperRecCollection"
      outputElectronKey = "HLT_egamma_Electrons"
      SuperPhotonRecCollectionName = "HLT_PhotonSuperRecCollection"
      EgammaRecKey = "HLT_egammaRecCollection"
      outputPhotonKey = "HLT_egamma_Photons"
      outputClusterKey = 'HLT_egammaClusters'
      outputTopoSeededClusterKey = 'HLT_egammaTopoSeededClusters'
      TrigEMClusterToolOutputContainer = 'HLT_TrigEMClusterOutput'

