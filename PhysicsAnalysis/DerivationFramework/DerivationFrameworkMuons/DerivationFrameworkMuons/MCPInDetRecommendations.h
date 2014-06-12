/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MCPInDetRecommendations.h  (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_MCPINDETRECOMMENDATIONS_H
#define DERIVATIONFRAMEWORK_MCPINDETRECOMMENDATIONS_H 1

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  /** @class MCPInDetRecommendations
  
      the code used in this implementation is kindly stolen from:
      atlasoff:: ISF/ISF_Core/ISF_Tools

      @author James Catmore -at- cern.ch
     */
  class MCPInDetRecommendations : public AthAlgTool, public IAugmentationTool {
    
  public: 
    /** Constructor with parameters */
    MCPInDetRecommendations( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~MCPInDetRecommendations();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
 
    /** Check that the current event passes this filter */
    virtual StatusCode addBranches() const;

  private:
    // inline functions
    StatusCode retrieveFromStoreGate(	std::vector<int> *&, std::vector<int> *&, std::vector<int> *&,
					std::vector<int> *&, std::vector<int> *&, std::vector<int> *&, std::vector<int> *&,
					std::vector<int> *&, std::vector<float> *&) const;
    bool muonPassesSelection(int, int, int, int, int, int, int, int, float) const;
    
    int m_pixAndDead;
    int m_sctAndDead;
    int m_pixAndSCTHoles;
    float m_trtEtaMin;
    float m_trtEtaMax;
    int m_trtHitsPlusOutliers;
    float m_trtOutliersFraction;
    std::string m_sgKey;
    std::string m_collName;
    unsigned int m_nRequired;
    mutable unsigned int m_npass, m_ntot; 

  }; 
  
}


#endif
