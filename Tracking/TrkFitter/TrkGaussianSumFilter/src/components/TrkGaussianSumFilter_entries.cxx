#include "TrkGaussianSumFilter/QuickCloseComponentsMultiStateMerger.h"
#include "TrkGaussianSumFilter/MultiComponentStateModeCalculator.h"
#include "TrkGaussianSumFilter/MultiStateMaterialEffectsAdapter.h"
#include "TrkGaussianSumFilter/KullbackLeiblerComponentDistance.h"
#include "TrkGaussianSumFilter/CloseComponentsMultiStateMerger.h"
#include "TrkGaussianSumFilter/LargestWeightsMultiStateMerger.h"
#include "TrkGaussianSumFilter/GsfMaterialMixtureConvolution.h"
#include "TrkGaussianSumFilter/MahalanobisComponentDistance.h"
#include "TrkGaussianSumFilter/MultiComponentStateAssembler.h"
#include "TrkGaussianSumFilter/MultiComponentStateCombiner.h"
#include "TrkGaussianSumFilter/GsfCombinedMaterialEffects.h"
#include "TrkGaussianSumFilter/PosteriorWeightsCalculator.h"
#include "TrkGaussianSumFilter/GsfMaterialEffectsUpdator.h"
#include "TrkGaussianSumFilter/MultipleScatterUpdator.h"
#include "TrkGaussianSumFilter/GsfBetheHeitlerEffects.h"
#include "TrkGaussianSumFilter/GsfMeasurementUpdator.h"
#include "TrkGaussianSumFilter/GsfEnergyLossUpdator.h"
#include "TrkGaussianSumFilter/GaussianSumFitter.h"
#include "TrkGaussianSumFilter/ForwardGsfFitter.h"
#include "TrkGaussianSumFilter/GsfOutlierLogic.h"
#include "TrkGaussianSumFilter/GsfExtrapolator.h"
#include "TrkGaussianSumFilter/GsfSmoother.h"
#include "TrkGaussianSumFilter/BremFind.h"
//#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, QuickCloseComponentsMultiStateMerger )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, MultiComponentStateModeCalculator )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, MultiStateMaterialEffectsAdapter )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, KullbackLeiblerComponentDistance )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, CloseComponentsMultiStateMerger )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, LargestWeightsMultiStateMerger )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, GsfMaterialMixtureConvolution )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, MahalanobisComponentDistance )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, MultiComponentStateAssembler )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, MultiComponentStateCombiner )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, GsfCombinedMaterialEffects )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, PosteriorWeightsCalculator )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, GsfMaterialEffectsUpdator )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, MultipleScatterUpdator )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, GsfBetheHeitlerEffects )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, GsfMeasurementUpdator )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, GsfEnergyLossUpdator )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, GaussianSumFitter )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, ForwardGsfFitter )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, GsfOutlierLogic )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, GsfExtrapolator )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, GsfSmoother )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, BremFind )

