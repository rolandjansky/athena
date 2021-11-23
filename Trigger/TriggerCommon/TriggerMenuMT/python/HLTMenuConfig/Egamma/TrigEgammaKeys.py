#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__author__ = "Fernando Monticelli, Debottam Gupta, Joao Victor Pinto"
__doc__    = "ConfigSettings"
__all__    = [ "getTrigEgammaKeys" ]

#----------------------------------------------------------------
# Configuration of Electron and Photon chains
#----------------------------------------------------------------

from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
from TrigEDMConfig.TriggerEDMRun3 import recordable


class TrigEgammaKeysBase():

      """Base clas to configure TrigEgamma Container names. Containers will be record, collections not"""
      def __init__(self):


        """Static class to collect all string manipulation in fast electron sequences """
        self.fastElectronRoIContainer                   = recordable("HLT_Roi_FastElectron")
        self.fastElectronContainer                      = recordable("HLT_FastElectrons")

        """Static class to collect all string manipulation in fast photon sequences """
        self.fastPhotonRoIContainer                     = recordable("HLT_Roi_FastPhoton")
        self.fastPhotonContainer                        = recordable("HLT_FastPhotons")

        """Static class to collect all string manipulation in precision calo sequences """
        self.precisionCaloTopoCollection                = 'HLT_egammaTopoCluster'
        self.precisionCaloEgammaRecCollection           = 'HLT_precisionCaloEgammaRecCollection'
        self.precisionCaloClusterContainer              = recordable("HLT_CaloEMClusters")
        self.precisionHICaloClusterContainer            = recordable("HLT_HICaloEMClusters")
        self.egEventShape                               = recordable('HLT_HIEventShapeEG')
        self.precisionEgammaRecCollection               = 'HLT_egammaRecCollection'
        self.precisionEMClusterContainer                = recordable('HLT_TrigEMClusters')

        """Static class to collect all string manipulation in precision photon sequences """
        self.precisionPhotonSuperClusterCollection      = 'HLT_PhotonSuperRecCollection'
        self.precisionPhotonContainer                   = recordable('HLT_egamma_Photons')

        """Static class to collect all string manipulation in precision electron sequences """
        self.precisionElectronSuperClusterCollection    = 'HLT_ElectronSuperRecCollection'
        self.precisionElectronContainer                 = recordable('HLT_egamma_Electrons')

        #
        # Track configuration
        #
        self._IDTrigConfig                              = getInDetTrigConfig( 'electron' )

        #
        # Special sequences
        #
        self.TrigTRTHTCountsContainer                   = recordable("HLT_TrigTRTHTCounts")



      @property
      def precisionTrackingContainer(self):
          return self._IDTrigConfig.tracks_IDTrig()

      @property
      def IDTrigConfig(self):
          return self._IDTrigConfig




class TrigEgammaKeys_LRT(TrigEgammaKeysBase):
    # This class contians modified base configuration class for LRT electron trigger chains
    def __init__(self):
        TrigEgammaKeysBase.__init__(self)
        self.fastElectronRoIContainer       = recordable("HLT_Roi_FastElectron_LRT")
        self.fastElectronContainer          = recordable('HLT_FastElectrons_LRT')
        self.precisionCaloClusterContainer  = recordable("HLT_CaloEMClusters_LRT")
        self.precisionElectronContainer     = recordable('HLT_egamma_Electrons_LRT')
        self._IDTrigConfig                  = getInDetTrigConfig('electronLRT')



class TrigEgammaKeys_GSF(TrigEgammaKeysBase):
    # This class contians modified base configuration class for GSF electron trigger chains
    def __init__(self):
        TrigEgammaKeysBase.__init__(self)
        
        # from HLT_IDTrack_Electron to HLT_IDTrack_Electron by refit alg
        self.precisionElectronTrkCollectionGSF          = 'HLT_IDTrkTrack_Electron_GSF'
        self.precisionElectronTrackParticleContainerGSF = recordable('HLT_IDTrack_Electron_GSF')
        self.precisionElectronContainer                 = recordable('HLT_egamma_Electrons_GSF')


#
# Get keys from variant name
#
def getTrigEgammaKeys(name=''):

    _d = {
        # Dictionary that maps a string to a configuration setting for electron and photon chains
        ''      : TrigEgammaKeysBase(),
        '_LRT'  : TrigEgammaKeys_LRT(),
        '_GSF'  : TrigEgammaKeys_GSF(),
        }

    if name in _d.keys():
        return _d[name]
    else:
        raise Exception('getTrigEgammaKeys() called with non valid name : ' + name + ' valid names are:' + str(_d.keys()) )



