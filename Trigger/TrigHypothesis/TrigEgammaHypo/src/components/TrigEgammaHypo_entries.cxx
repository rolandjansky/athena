#include "TrigEgammaHypo/TrigL2CaloHypo.h"
#include "TrigEgammaHypo/TrigL2ElectronFex.h"
#include "TrigEgammaHypo/TrigL2ElectronHypo.h"
#include "TrigEgammaHypo/TrigL2PhotonFex.h"
#include "TrigEgammaHypo/TrigL2PhotonHypo.h"
#include "TrigEgammaHypo/TrigEFDielectronMassHypo.h"
#include "TrigEgammaHypo/TrigEFDielectronMassFex.h"
#include "TrigEgammaHypo/TrigEFMtAllTE.h"
#include "TrigEgammaHypo/TrigEFCaloCalibFex.h"
#include "TrigEgammaHypo/TrigEFCaloHypo.h"
#include "TrigEgammaHypo/TrigEFElectronHypo.h"
#include "TrigEgammaHypo/TrigEFPhotonHypo.h"
#include "TrigEgammaHypo/TrigEFTrackHypo.h"
#include "TrigEgammaHypo/TrigL2PhotonFex.h"
#include "TrigEgammaHypo/TrigL2CaloLayersHypo.h"

#include "../TrigL2CaloHypoAlg.h"
#include "../TrigL2CaloHypoTool.h"
#include "../TrigL2ElectronFexMT.h"
#include "../TrigL2ElectronHypoAlg.h"
#include "../TrigL2ElectronHypoTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_ALGORITHM_FACTORY( TrigL2CaloHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2ElectronFex )
DECLARE_ALGORITHM_FACTORY( TrigL2ElectronFexMT )
DECLARE_ALGORITHM_FACTORY( TrigL2ElectronHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2PhotonFex )
DECLARE_ALGORITHM_FACTORY( TrigL2PhotonHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFDielectronMassHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFDielectronMassFex )
DECLARE_ALGORITHM_FACTORY( TrigEFMtAllTE )
DECLARE_ALGORITHM_FACTORY( TrigEFCaloCalibFex )
DECLARE_ALGORITHM_FACTORY( TrigEFCaloHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFElectronHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFPhotonHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFTrackHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2CaloLayersHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2CaloHypoAlg )
DECLARE_TOOL_FACTORY( TrigL2CaloHypoTool )
DECLARE_ALGORITHM_FACTORY( TrigL2ElectronHypoAlg )
DECLARE_TOOL_FACTORY( TrigL2ElectronHypoTool )

DECLARE_FACTORY_ENTRIES( TrigEgammaHypo ) {
    DECLARE_ALGORITHM( TrigL2CaloHypo )
    DECLARE_ALGORITHM( TrigL2ElectronFex )
    DECLARE_ALGORITHM( TrigL2ElectronFexMT )
    DECLARE_ALGORITHM( TrigL2ElectronHypo )
    DECLARE_ALGORITHM( TrigL2PhotonFex )
    DECLARE_ALGORITHM( TrigL2PhotonHypo )
    DECLARE_ALGORITHM( TrigEFDielectronMassHypo )
    DECLARE_ALGORITHM( TrigEFDielectronMassFex )
    DECLARE_ALGORITHM( TrigEFMtAllTE )
    DECLARE_ALGORITHM( TrigEFCaloCalibFex )
    DECLARE_ALGORITHM( TrigEFCaloHypo )
    DECLARE_ALGORITHM( TrigEFElectronHypo )
    DECLARE_ALGORITHM( TrigEFPhotonHypo )
    DECLARE_ALGORITHM( TrigEFTrackHypo )
    DECLARE_ALGORITHM( TrigL2CaloLayersHypo )
    DECLARE_ALGORITHM( TrigL2CaloHypoAlg )
    DECLARE_TOOL( TrigL2CaloHypoTool )
    DECLARE_ALGORITHM( TrigL2ElectronHypoAlg )
    DECLARE_TOOL( TrigL2ElectronHypoTool )

}

