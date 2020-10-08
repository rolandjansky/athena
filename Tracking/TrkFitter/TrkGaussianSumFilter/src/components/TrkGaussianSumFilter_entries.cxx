#include "TrkGaussianSumFilter/GaussianSumFitter.h"
#include "TrkGaussianSumFilter/GsfBetheHeitlerEffects.h"
#include "TrkGaussianSumFilter/GsfCombinedMaterialEffects.h"
#include "TrkGaussianSumFilter/GsfExtrapolator.h"
#include "TrkGaussianSumFilter/GsfMaterialEffectsUpdator.h"
#include "TrkGaussianSumFilter/GsfMaterialMixtureConvolution.h"

DECLARE_COMPONENT( Trk::GsfMaterialMixtureConvolution )
DECLARE_COMPONENT( Trk::GsfCombinedMaterialEffects )
DECLARE_COMPONENT( Trk::GsfMaterialEffectsUpdator )
DECLARE_COMPONENT( Trk::GsfBetheHeitlerEffects )
DECLARE_COMPONENT( Trk::GaussianSumFitter )
DECLARE_COMPONENT( Trk::GsfExtrapolator )
