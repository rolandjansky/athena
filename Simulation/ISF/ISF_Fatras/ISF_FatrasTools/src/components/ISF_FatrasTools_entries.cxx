#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TransportTool.h"
#include "../TransportEngine.h"
#include "../McMaterialEffectsUpdator.h"
#include "../McMaterialEffectsEngine.h"
#include "../HadIntProcessorParametric.h"
#include "../McEnergyLossUpdator.h"
#include "../EnergyLossSamplerBetheHeitler.h"
#include "../PhotonConversionTool.h"
#include "../ProcessSamplingTool.h"
#include "../PhysicsValidationTool.h"

#include "../MultipleScatteringSamplerHighland.h"
#include "../MultipleScatteringSamplerGaussianMixture.h"
#include "../MultipleScatteringSamplerGeneralMixture.h"

DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , TransportTool )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , TransportEngine )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , McMaterialEffectsUpdator )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , McMaterialEffectsEngine )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , HadIntProcessorParametric )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , McEnergyLossUpdator )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , EnergyLossSamplerBetheHeitler )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , PhotonConversionTool )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , MultipleScatteringSamplerGaussianMixture )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , MultipleScatteringSamplerGeneralMixture )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , MultipleScatteringSamplerHighland )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , ProcessSamplingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras , PhysicsValidationTool )

DECLARE_FACTORY_ENTRIES( ISF_FatrasTools ) {
  DECLARE_NAMESPACE_TOOL( iFatras ,  TransportTool )
  DECLARE_NAMESPACE_TOOL( iFatras ,  TransportEngine )
}

