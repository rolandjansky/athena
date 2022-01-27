#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__author__ = "Fernando Monticelli, Debottam Gupta, Joao Victor Pinto"
__doc__    = "ConfigSettings"
__all__    = [ "getTrigEgammaKeys" ]

#----------------------------------------------------------------
# Configuration of Electron and Photon chains
#----------------------------------------------------------------

from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
from TrigEDMConfig.TriggerEDMRun3 import recordable


class TrigEgammaKeysBase(object):

      """Base clas to configure TrigEgamma Container names. Containers will be record, collections not"""
      def __init__(self, ion=False):


        """Static class to collect all string manipulation in fast electron sequences """
        self.fastElectronRoIContainer                   = recordable("HLT_Roi_FastElectron")
        self.fastElectronContainer                      = recordable("HLT_FastElectrons")

        """Static class to collect all string manipulation in fast photon sequences """
        self.fastPhotonRoIContainer                     = recordable("HLT_Roi_FastPhoton")
        self.fastPhotonContainer                        = recordable("HLT_FastPhotons")

        """Static class to collect all string manipulation in precision calo sequences """
        self.precisionCaloTopoCollection                = 'HLT_egammaTopoCluster'
        self.precisionCaloEgammaRecCollection           = 'HLT_precisionCaloEgammaRecCollection'
        self.precisionElectronCaloClusterContainer     = recordable("HLT_CaloEMClusters_Electron") if not ion else recordable("HLT_HICaloEMClusters") 
        self.precisionPhotonCaloClusterContainer       = recordable("HLT_CaloEMClusters_Photon") if not ion else recordable("HLT_HICaloEMClusters")
        self.precisionTopoClusterContainer              = recordable("HLT_TopoCaloClustersRoI") if not ion else recordable("HLT_TopoCaloClustersHIRoI") 
        self.precisionEgammaRecCollection               = 'HLT_egammaRecCollection'
        self.precisionEMClusterContainer                = recordable('HLT_TrigEMClusters')

        """Static class to collect all string manipulation in precision photon sequences """
        self.precisionPhotonSuperClusterRecCollection      = 'HLT_PhotonSuperRecCollection'
        self.precisionPhotonSuperClusterCollection      = 'HLT_PhotonSuperClusters'
        self.precisionPhotonContainer                   = recordable('HLT_egamma_Photons')

        """Static class to collect all string manipulation in precision electron sequences """
        self.precisionElectronSuperClusterRecCollection    = 'HLT_ElectronSuperRecCollection'
        self.precisionElectronSuperClusterCollection    = 'HLT_ElectronSuperClusters'
        self.precisionElectronContainer                 = recordable('HLT_egamma_Electrons')

        #
        # Track configuration
        #
        self._IDTrigConfig                              = getInDetTrigConfig( 'electron' )

        #
        # Special sequences
        #
        self.TrigTRTHTCountsContainer                   = recordable("HLT_TrigTRTHTCounts")
        self.egEventShape                               = recordable('HLT_HIEventShapeEG')



      @property
      def precisionTrackingContainer(self):
          return self._IDTrigConfig.tracks_IDTrig()

      @property
      def IDTrigConfig(self):
          return self._IDTrigConfig




class TrigEgammaKeys_LRT(TrigEgammaKeysBase):
    # This class contians modified base configuration class for LRT electron trigger chains
    def __init__(self,ion):
        TrigEgammaKeysBase.__init__(self,ion)
        
        self.fastElectronRoIContainer               = recordable("HLT_Roi_FastElectron_LRT")
        self.fastElectronContainer                  = recordable('HLT_FastElectrons_LRT')
        self.precisionElectronCaloClusterContainer  = recordable("HLT_CaloEMClusters_LRT")
        self.precisionElectronContainer             = recordable('HLT_egamma_Electrons_LRT')
        self._IDTrigConfig                          = getInDetTrigConfig('electronLRT')



class TrigEgammaKeys_GSF(TrigEgammaKeysBase):
    # This class contians modified base configuration class for GSF electron trigger chains
    def __init__(self, ion):
        TrigEgammaKeysBase.__init__(self, ion)
        
        # from HLT_IDTrack_Electron to HLT_IDTrack_Electron by refit alg
        self.precisionElectronTrkCollectionGSF          = 'HLT_IDTrkTrack_Electron_GSF'
        self.precisionElectronTrackParticleContainerGSF = recordable('HLT_IDTrack_Electron_GSF')
        self.precisionElectronContainer                 = recordable('HLT_egamma_Electrons_GSF')


#
# Get keys from variant name
#
def getTrigEgammaKeys(name='', ion=False):

    _d = {
        # Dictionary that maps a string to a configuration setting for electron and photon chains
        ''      : TrigEgammaKeysBase(ion),
        '_LRT'  : TrigEgammaKeys_LRT(ion),
        '_GSF'  : TrigEgammaKeys_GSF(ion),
        }

    if name in _d.keys():
        return _d[name]
    else:
        raise Exception('getTrigEgammaKeys() called with non valid name : ' + name + ' valid names are:' + str(_d.keys()) )



