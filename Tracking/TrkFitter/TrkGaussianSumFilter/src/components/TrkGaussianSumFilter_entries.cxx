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

DECLARE_COMPONENT( Trk::QuickCloseComponentsMultiStateMerger )
DECLARE_COMPONENT( Trk::MultiComponentStateModeCalculator )
DECLARE_COMPONENT( Trk::MultiStateMaterialEffectsAdapter )
DECLARE_COMPONENT( Trk::KullbackLeiblerComponentDistance )
DECLARE_COMPONENT( Trk::CloseComponentsMultiStateMerger )
DECLARE_COMPONENT( Trk::LargestWeightsMultiStateMerger )
DECLARE_COMPONENT( Trk::GsfMaterialMixtureConvolution )
DECLARE_COMPONENT( Trk::MahalanobisComponentDistance )
DECLARE_COMPONENT( Trk::MultiComponentStateAssembler )
DECLARE_COMPONENT( Trk::MultiComponentStateCombiner )
DECLARE_COMPONENT( Trk::GsfCombinedMaterialEffects )
DECLARE_COMPONENT( Trk::PosteriorWeightsCalculator )
DECLARE_COMPONENT( Trk::GsfMaterialEffectsUpdator )
DECLARE_COMPONENT( Trk::MultipleScatterUpdator )
DECLARE_COMPONENT( Trk::GsfBetheHeitlerEffects )
DECLARE_COMPONENT( Trk::GsfMeasurementUpdator )
DECLARE_COMPONENT( Trk::GsfEnergyLossUpdator )
DECLARE_COMPONENT( Trk::GaussianSumFitter )
DECLARE_COMPONENT( Trk::ForwardGsfFitter )
DECLARE_COMPONENT( Trk::GsfOutlierLogic )
DECLARE_COMPONENT( Trk::GsfExtrapolator )
DECLARE_COMPONENT( Trk::GsfSmoother )
DECLARE_COMPONENT( Trk::BremFind )

