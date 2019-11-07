#include "TrkGaussianSumFilter/QuickCloseComponentsMultiStateMerger.h"
#include "TrkGaussianSumFilter/GsfMaterialMixtureConvolution.h"
#include "TrkGaussianSumFilter/MultiComponentStateAssembler.h"
#include "TrkGaussianSumFilter/MultiComponentStateCombiner.h"
#include "TrkGaussianSumFilter/GsfCombinedMaterialEffects.h"
#include "TrkGaussianSumFilter/PosteriorWeightsCalculator.h"
#include "TrkGaussianSumFilter/GsfMaterialEffectsUpdator.h"
#include "TrkGaussianSumFilter/MultipleScatteringEffects.h"
#include "TrkGaussianSumFilter/GsfBetheHeitlerEffects.h"
#include "TrkGaussianSumFilter/GsfMeasurementUpdator.h"
#include "TrkGaussianSumFilter/GsfEnergyLossEffects.h"
#include "TrkGaussianSumFilter/GaussianSumFitter.h"
#include "TrkGaussianSumFilter/ForwardGsfFitter.h"
#include "TrkGaussianSumFilter/GsfExtrapolator.h"
#include "TrkGaussianSumFilter/GsfSmoother.h"

DECLARE_COMPONENT( Trk::QuickCloseComponentsMultiStateMerger )
DECLARE_COMPONENT( Trk::GsfMaterialMixtureConvolution )
DECLARE_COMPONENT( Trk::MultiComponentStateAssembler )
DECLARE_COMPONENT( Trk::MultiComponentStateCombiner )
DECLARE_COMPONENT( Trk::GsfCombinedMaterialEffects )
DECLARE_COMPONENT( Trk::PosteriorWeightsCalculator )
DECLARE_COMPONENT( Trk::GsfMaterialEffectsUpdator )
DECLARE_COMPONENT( Trk::MultipleScatteringEffects )
DECLARE_COMPONENT( Trk::GsfBetheHeitlerEffects )
DECLARE_COMPONENT( Trk::GsfMeasurementUpdator )
DECLARE_COMPONENT( Trk::GsfEnergyLossEffects )
DECLARE_COMPONENT( Trk::GaussianSumFitter )
DECLARE_COMPONENT( Trk::ForwardGsfFitter )
DECLARE_COMPONENT( Trk::GsfExtrapolator )
DECLARE_COMPONENT( Trk::GsfSmoother )
