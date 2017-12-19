#include "ISF_FatrasTools/TransportTool.h"
#include "ISF_FatrasTools/TransportEngine.h"
#include "ISF_FatrasTools/McMaterialEffectsUpdator.h"
#include "ISF_FatrasTools/McMaterialEffectsEngine.h"
#include "ISF_FatrasTools/HadIntProcessorParametric.h"
#include "ISF_FatrasTools/McEnergyLossUpdator.h"
#include "ISF_FatrasTools/EnergyLossSamplerBetheHeitler.h"
#include "ISF_FatrasTools/PhotonConversionTool.h"
#include "ISF_FatrasTools/ProcessSamplingTool.h"
#include "ISF_FatrasTools/PhysicsValidationTool.h"

#include "ISF_FatrasTools/MultipleScatteringSamplerHighland.h"
#include "ISF_FatrasTools/MultipleScatteringSamplerGaussianMixture.h"
#include "ISF_FatrasTools/MultipleScatteringSamplerGeneralMixture.h"

DECLARE_COMPONENT( iFatras::TransportTool )
DECLARE_COMPONENT( iFatras::TransportEngine )
DECLARE_COMPONENT( iFatras::McMaterialEffectsUpdator )
DECLARE_COMPONENT( iFatras::McMaterialEffectsEngine )
DECLARE_COMPONENT( iFatras::HadIntProcessorParametric )
DECLARE_COMPONENT( iFatras::McEnergyLossUpdator )
DECLARE_COMPONENT( iFatras::EnergyLossSamplerBetheHeitler )
DECLARE_COMPONENT( iFatras::PhotonConversionTool )
DECLARE_COMPONENT( iFatras::MultipleScatteringSamplerGaussianMixture )
DECLARE_COMPONENT( iFatras::MultipleScatteringSamplerGeneralMixture )
DECLARE_COMPONENT( iFatras::MultipleScatteringSamplerHighland )
DECLARE_COMPONENT( iFatras::ProcessSamplingTool )
DECLARE_COMPONENT( iFatras::PhysicsValidationTool )

