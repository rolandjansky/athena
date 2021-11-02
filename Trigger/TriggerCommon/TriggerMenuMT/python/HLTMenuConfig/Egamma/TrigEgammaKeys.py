#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__author__ = "Fernando Monticelli, Debottam Gupta"
__doc__    = "ConfigSettings"
__all__    = [ "getTrigEgammaKeys" ]

#----------------------------------------------------------------
# Configuration of Electron and Photon chains
#----------------------------------------------------------------

from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
from TrigEDMConfig.TriggerEDMRun3 import recordable
from AthenaCommon.Logging import logging

log = logging.getLogger(__name__)

def getTrigEgammaKeys(name=''):
    if name in _TrigEgammaConfigurations:
        return _TrigEgammaConfigurations[name]
    else:
        # OK that name is not valid so lets prepare a list of valid names to throw in the exception msg:
        validnames = []
        for key in _TrigEgammaConfigurations:
            validnames.append(key)
        raise Exception('getTrigEgammaKeys() called with non valid name : ' + name + ' valid names are:' + str(validnames))



class TrigEgammaKeys_electrons_LRT(TrigEgammaKeys_base):
    # This class contians modified base configuration class for LRT electron trigger chains
    def __init__(self):
        TrigEgammaKeys_base.__init__(self)
        self._outputElectronKey = 'HLT_egamma_Electrons_LRT'
        self._IDTrigConfig = getInDetTrigConfig('electronLRT')

class TrigEgammaKeys_electrons_GSF(TrigEgammaKeys_base):
    # This class contians modified base configuration class for GSF electron trigger chains
    def __init__(self):
        TrigEgammaKeys_base.__init__(self)
        self._outputElectronKey = 'HLT_egamma_Electrons_GSF'
        self._outputTrackKey = 'HLT_IDTrkTrack_Electron_GSF'
        self._outputTrackParticleKey = 'HLT_IDTrack_Electron_GSF'




class TrigEgammaKeys_base():
      """Base clas to configure TrigEgamma Container names, tuning names and other configurations"""
      def __init__(self):
        # First Egamma, calo and common configuration stuff
        self._EgammaRecKey = 'HLT_egammaRecCollection'
        self._TrigEMClusterToolOutputContainer = 'HLT_TrigEMClusters'
        self._outputTopoSeededClusterKey = 'HLT_egammaTopoSeededClusters'

        # Tracking configuration stuff now
        self._IDTrigConfig = getInDetTrigConfig( 'electron' )
        self._outputTrackKey = 'HLT_IDTrkTrack_Electron'
        self._outputTrackParticleKey = recordable('HLT_IDTrack_Electron')

        # Electron Specific
        self._SuperElectronRecCollectionName = 'HLT_ElectronSuperRecCollection'
        self._outputElectronKey = 'HLT_egamma_Electrons'

        # Photon specific
        self._SuperPhotonRecCollectionName = 'HLT_PhotonSuperRecCollection'
        self._outputPhotonKey = 'HLT_egamma_Photons'

        # Tunning configuration
        self._pidVersion = 'rel22_20210611'
        self._dnnVersion = 'mc16_20210430'
        self._ringerVersion = 'TrigL2_20210702_r4'

                                                
      @property
      def SuperElectronRecCollectionName():  
          return self._SuperElectronRecCollectionName

      @property
      def outputElectronKey():               
          return recordable(self._outputElectronKey)

      @property
      def SuperPhotonRecCollectionName():    
          return self._SuperPhotonRecCollectionName

      @property
      def EgammaRecKey():                    
          return self._EgammaRecKey

      @property
      def outputPhotonKey():                 
          return recordable(self._outputPhotonKey)

      @property
      def outputTopoSeededClusterKey():      
          return self._outputTopoSeededClusterKey

      @property
      def TrigEMClusterToolOutputContainer():
          return recordable(self._TrigEMClusterToolOutputContainer)

      @property
      def TrigElectronTracksCollectionName():
          return self._IDTrigConfig().tracks_IDTrig()

      @property
      def pidVersion():                      
          return self._pidVersion

      @property
      def dnnVersion():                      
          return self._dnnVersion

      @property
      def ringerVersion():                   
          return self._ringerVersion


      @property
      def outputTrackKey():
        return self._outputTrackKey

      @property
      def  outputTrackParticleKey():
        return recordable(self._outputTrackParticleKey)
      


_TrigEgammaConfigurations = {
        # Dictionary that maps a string to a configuration setting for electron and photon chains
        '': TrigEgammaKeys_base(),
        '_LRT': TrigEgammaKeys_electrons_LRT(),
        '_GSF': TrigEgammaKeys_electrons_GSF(),
        }


