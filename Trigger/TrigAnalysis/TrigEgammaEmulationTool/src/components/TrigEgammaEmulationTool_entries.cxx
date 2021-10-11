

#include "TrigEgammaEmulationTool/TrigEgammaEmulationToolMT.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationChain.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationBaseHypoTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationL1CaloHypoTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationFastCaloHypoTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationFastElectronHypoTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationFastPhotonHypoTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationPrecisionCaloHypoTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationPrecisionElectronHypoTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationPrecisionPhotonHypoTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationPrecisionTrackingHypoTool.h"

#include "../TrigEgammaEmulationToolTest.h"

DECLARE_COMPONENT( Trig::TrigEgammaEmulationToolMT )

DECLARE_COMPONENT( Trig::TrigEgammaEmulationChain )
DECLARE_COMPONENT( Trig::TrigEgammaEmulationBaseHypoTool )
DECLARE_COMPONENT( Trig::TrigEgammaEmulationL1CaloHypoTool )
DECLARE_COMPONENT( Trig::TrigEgammaEmulationFastCaloHypoTool )
DECLARE_COMPONENT( Trig::TrigEgammaEmulationFastElectronHypoTool )
DECLARE_COMPONENT( Trig::TrigEgammaEmulationFastPhotonHypoTool )
DECLARE_COMPONENT( Trig::TrigEgammaEmulationPrecisionCaloHypoTool )
DECLARE_COMPONENT( Trig::TrigEgammaEmulationPrecisionElectronHypoTool )
DECLARE_COMPONENT( Trig::TrigEgammaEmulationPrecisionPhotonHypoTool )
DECLARE_COMPONENT( Trig::TrigEgammaEmulationPrecisionTrackingHypoTool )


DECLARE_COMPONENT(TrigEgammaEmulationToolTest)