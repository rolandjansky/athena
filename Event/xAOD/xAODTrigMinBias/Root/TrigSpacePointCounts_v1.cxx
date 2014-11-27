/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODCore/AuxStoreAccessorMacros.h"

#include "xAODTrigMinBias/versions/TrigSpacePointCounts_v1.h"
#include "xAODTrigMinBias/xAODTrigMinBiasHelperFunc.h"

namespace xAOD {

  // Default ctor
  TrigSpacePointCounts_v1::TrigSpacePointCounts_v1()
    : SG::AuxElement() {
    }

  TrigSpacePointCounts_v1::~TrigSpacePointCounts_v1(){}
  // Simple accessor functions for the "default variable" access pattern

  AUXSTORE_OBJECT_SETTER_AND_GETTER(TrigSpacePointCounts_v1,std::vector<float>,contentsPixelClusEndcapC, setContentsPixelClusEndcapC)
  AUXSTORE_OBJECT_SETTER_AND_GETTER(TrigSpacePointCounts_v1,std::vector<float>,contentsPixelClusBarrel, setContentsPixelClusBarrel)
  AUXSTORE_OBJECT_SETTER_AND_GETTER(TrigSpacePointCounts_v1,std::vector<float>,contentsPixelClusEndcapA, setContentsPixelClusEndcapA)
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigSpacePointCounts_v1, unsigned int, pixelClusTotBins, setPixelClusTotBins)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigSpacePointCounts_v1, float, pixelClusTotMin, setPixelClusTotMin)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigSpacePointCounts_v1, float, pixelClusTotMax, setPixelClusTotMax)
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigSpacePointCounts_v1, unsigned int, pixelClusSizeBins, setPixelClusSizeBins)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigSpacePointCounts_v1, float, pixelClusSizeMin, setPixelClusSizeMin)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigSpacePointCounts_v1, float, pixelClusSizeMax, setPixelClusSizeMax)
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (TrigSpacePointCounts_v1, unsigned int, sctSpEndcapC, setSctSpEndcapC)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (TrigSpacePointCounts_v1, unsigned int, sctSpBarrel, setSctSpBarrel)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (TrigSpacePointCounts_v1, unsigned int, sctSpEndcapA, setSctSpEndcapA)
  
  double TrigSpacePointCounts_v1::pixelClusBarrelSumEntries(float value_x, float value_y, int cutType) const{
    static Accessor< std::vector<float> > acc_contents( "contentsPixelClusBarrel" );
    return xAOD::sumEntries(acc_contents(*this),
			    pixelClusTotBins(),pixelClusTotMin(),pixelClusTotMax(),
			    pixelClusSizeBins(),pixelClusSizeMin(),pixelClusSizeMax(),
			    value_x,value_y,cutType);
  }
    //
  double TrigSpacePointCounts_v1::pixelClusEndcapASumEntries(float value_x, float value_y, int cutType) const{
    static Accessor< std::vector<float> > acc_contents( "contentsPixelClusEndcapA" );
    return xAOD::sumEntries(acc_contents(*this),
			    pixelClusTotBins(),pixelClusTotMin(),pixelClusTotMax(),
			    pixelClusSizeBins(),pixelClusSizeMin(),pixelClusSizeMax(),
			    value_x,value_y,cutType);
  }
    //
  double TrigSpacePointCounts_v1::pixelClusEndcapCSumEntries(float value_x, float value_y, int cutType) const{
    static Accessor< std::vector<float> > acc_contents( "contentsPixelClusEndcapC" );
    return xAOD::sumEntries(acc_contents(*this),
			    pixelClusTotBins(),pixelClusTotMin(),pixelClusTotMax(),
			    pixelClusSizeBins(),pixelClusSizeMin(),pixelClusSizeMax(),
			    value_x,value_y,cutType);
  }
  
}
