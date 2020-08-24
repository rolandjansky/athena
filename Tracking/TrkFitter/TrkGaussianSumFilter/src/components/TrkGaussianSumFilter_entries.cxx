#include "TrkGaussianSumFilter/GsfMaterialMixtureConvolution.h"
#include "TrkGaussianSumFilter/GsfCombinedMaterialEffects.h"
#include "TrkGaussianSumFilter/GsfMaterialEffectsUpdator.h"
#include "TrkGaussianSumFilter/GsfBetheHeitlerEffects.h"
#include "TrkGaussianSumFilter/GaussianSumFitter.h"
#include "TrkGaussianSumFilter/ForwardGsfFitter.h"
#include "TrkGaussianSumFilter/GsfExtrapolator.h"
#include "TrkGaussianSumFilter/GsfSmoother.h"

DECLARE_COMPONENT( Trk::GsfMaterialMixtureConvolution )
DECLARE_COMPONENT( Trk::GsfCombinedMaterialEffects )
DECLARE_COMPONENT( Trk::GsfMaterialEffectsUpdator )
DECLARE_COMPONENT( Trk::GsfBetheHeitlerEffects )
DECLARE_COMPONENT( Trk::GaussianSumFitter )
DECLARE_COMPONENT( Trk::ForwardGsfFitter )
DECLARE_COMPONENT( Trk::GsfExtrapolator )
DECLARE_COMPONENT( Trk::GsfSmoother )
