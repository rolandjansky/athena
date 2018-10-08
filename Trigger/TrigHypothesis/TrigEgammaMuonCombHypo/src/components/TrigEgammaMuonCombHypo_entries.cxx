#include "TrigEgammaMuonCombHypo/TrigL2ElectronMuonAngleHypo.h"
#include "TrigEgammaMuonCombHypo/TrigL2ElectronMuonAngleFexAlgo.h"
#include "TrigEgammaMuonCombHypo/TrigL2PhotonMuonAngleFexAlgo.h"
#include "TrigEgammaMuonCombHypo/TrigEFElectronMuonAngleHypo.h"
#include "TrigEgammaMuonCombHypo/TrigEFElectronMuonAngleFexAlgo.h"
#include "TrigEgammaMuonCombHypo/TrigEFElectronMuonAngleFexRun2Algo.h"
#include "TrigEgammaMuonCombHypo/TrigEFPhotonMuonAngleFexAlgo.h"

#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_ALGORITHM_FACTORY( TrigL2ElectronMuonAngleHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2ElectronMuonAngleFexAlgo )
DECLARE_ALGORITHM_FACTORY( TrigEFElectronMuonAngleHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFElectronMuonAngleFexAlgo ) 
DECLARE_ALGORITHM_FACTORY( TrigEFElectronMuonAngleFexRun2Algo ) 
DECLARE_ALGORITHM_FACTORY( TrigL2PhotonMuonAngleFexAlgo )
DECLARE_ALGORITHM_FACTORY( TrigEFPhotonMuonAngleFexAlgo ) 


DECLARE_FACTORY_ENTRIES( TrigEgammaMuonCombHypo ) {
    DECLARE_ALGORITHM( TrigL2ElectronMuonAngleHypo )
    DECLARE_ALGORITHM( TrigL2ElectronMuonAngleFexAlgo ) 
    DECLARE_ALGORITHM( TrigEFElectronMuonAngleHypo )
    DECLARE_ALGORITHM( TrigEFElectronMuonAngleFexAlgo )
    DECLARE_ALGORITHM( TrigEFElectronMuonAngleFexRun2Algo )
    DECLARE_ALGORITHM( TrigL2PhotonMuonAngleFexAlgo )
    DECLARE_ALGORITHM( TrigEFPhotonMuonAngleFexAlgo )
  }

