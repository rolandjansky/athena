/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBias_TrigSpacePointCounts_V1_H
#define xAODTrigMinBias_TrigSpacePointCounts_V1_H

#include "AthContainers/AuxElement.h"

namespace xAOD {
  
  class TrigSpacePointCounts_v1 : public SG::AuxElement {
  public:
    TrigSpacePointCounts_v1();
    
    /** Destructor */
    ~TrigSpacePointCounts_v1();
    
    /*!getters */
    const std::vector<float>& contentsPixelClusEndcapC() const;
    
    const std::vector<float>& contentsPixelClusBarrel() const;
    
    const std::vector<float>& contentsPixelClusEndcapA() const;
    
    unsigned int pixelClusTotBins() const;
    
    float pixelClusTotMin()const ; 
    
    float pixelClusTotMax() const; 
    
    unsigned int pixelClusSizeBins() const;
    
    float pixelClusSizeMin()const ; 
    
    float pixelClusSizeMax() const; 
     
    unsigned int sctSpEndcapC() const;
    
    unsigned int sctSpBarrel() const;
    
    unsigned int sctSpEndcapA() const;
        
    /*setters*/
    void setContentsPixelClusEndcapC(const std::vector<float> &contentsPixelClusEndcapC );
    
    void setContentsPixelClusBarrel(const std::vector<float> &contentsPixelClusBarrel );
    
    void setContentsPixelClusEndcapA(const std::vector<float> &contentsPixelClusEndcapA );
    
    void setPixelClusTotBins(unsigned int val_N);
    
    void setPixelClusTotMin(float val_min);
    
    void setPixelClusTotMax(float val_max);

    void setPixelClusSizeBins(unsigned int val_N);
    
    void setPixelClusSizeMin(float val_min);
    
    void setPixelClusSizeMax(float val_max);
        
    void setSctSpEndcapC( unsigned int sctEC);
    
    void setSctSpBarrel( unsigned int setB);
    
    void setSctSpEndcapA( unsigned int setEA);
    
  };
  
}

// Setup a CLID for the container... not sure what this is for.
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigSpacePointCounts_v1 , 231210679 , 1 )

#endif


#endif

