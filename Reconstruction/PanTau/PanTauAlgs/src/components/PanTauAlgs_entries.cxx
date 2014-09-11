#include "GaudiKernel/DeclareFactoryEntries.h"

#include "PanTauAlgs/Tool_InformationStore.h"
#include "PanTauAlgs/Tool_HelperFunctions.h"
#include "PanTauAlgs/Tool_InputConverter.h"
#include "PanTauAlgs/Tool_TauConstituentGetter.h"
#include "PanTauAlgs/Tool_TauConstituentSelector.h"
#include "PanTauAlgs/Tool_FeatureExtractor.h"
#include "PanTauAlgs/Tool_ModeDiscriminator.h"
#include "PanTauAlgs/Tool_DecayModeDeterminator.h"
#include "PanTauAlgs/Tool_FourMomentumCalculator.h"
#include "PanTauAlgs/Tool_DetailsArranger.h"
#include "PanTauAlgs/Alg_SeedBuilder.h"


DECLARE_NAMESPACE_TOOL_FACTORY( PanTau, Tool_InformationStore )
DECLARE_NAMESPACE_TOOL_FACTORY( PanTau, Tool_HelperFunctions )
DECLARE_NAMESPACE_TOOL_FACTORY( PanTau, Tool_InputConverter )
DECLARE_NAMESPACE_TOOL_FACTORY( PanTau, Tool_TauConstituentGetter )
DECLARE_NAMESPACE_TOOL_FACTORY( PanTau, Tool_TauConstituentSelector )
DECLARE_NAMESPACE_TOOL_FACTORY( PanTau, Tool_FeatureExtractor )
DECLARE_NAMESPACE_TOOL_FACTORY( PanTau, Tool_ModeDiscriminator )
DECLARE_NAMESPACE_TOOL_FACTORY( PanTau, Tool_DecayModeDeterminator )
DECLARE_NAMESPACE_TOOL_FACTORY( PanTau, Tool_FourMomentumCalculator )
DECLARE_NAMESPACE_TOOL_FACTORY( PanTau, Tool_DetailsArranger )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PanTau, Alg_SeedBuilder )



DECLARE_FACTORY_ENTRIES( PanTauAlgs ) {

DECLARE_NAMESPACE_TOOL( PanTau, Tool_InformationStore )
DECLARE_NAMESPACE_TOOL( PanTau, Tool_HelperFunctions )
DECLARE_NAMESPACE_TOOL( PanTau, Tool_InputConverter )
DECLARE_NAMESPACE_TOOL( PanTau, Tool_TauConstituentGetter )
DECLARE_NAMESPACE_TOOL( PanTau, Tool_TauConstituentSelector )
DECLARE_NAMESPACE_TOOL( PanTau, Tool_FeatureExtractor )
DECLARE_NAMESPACE_TOOL( PanTau, Tool_ModeDiscriminator )
DECLARE_NAMESPACE_TOOL( PanTau, Tool_DecayModeDeterminator )
DECLARE_NAMESPACE_TOOL( PanTau, Tool_FourMomentumCalculator )
DECLARE_NAMESPACE_TOOL( PanTau, Tool_DetailsArranger )
DECLARE_NAMESPACE_ALGORITHM( PanTau ,  Alg_SeedBuilder )

}   
