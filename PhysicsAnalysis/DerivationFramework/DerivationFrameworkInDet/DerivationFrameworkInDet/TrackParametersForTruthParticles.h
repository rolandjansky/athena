/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////
// TrackParametersForTruthParticles.h  (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TrackParametersForTruthParticles_H
#define DERIVATIONFRAMEWORK_TrackParametersForTruthParticles_H 

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

class IInDetPhysValDecoratorTool;

namespace DerivationFramework {

  /** @class TrackParametersForTruthParticles
 

     */
  class TrackParametersForTruthParticles : public AthAlgTool, public IAugmentationTool {
   
  public: 
    /** Constructor with parameters */
    TrackParametersForTruthParticles( const std::string& t, const std::string& n, const IInterface* p );
   
    /** Destructor */
    ~TrackParametersForTruthParticles();
   
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
 
    /** Check that the current event passes this filter */
    virtual StatusCode addBranches() const;

  private:
    std::string m_collTruthName;
    std::string m_sgKey1;
		
		ToolHandle<IInDetPhysValDecoratorTool> m_decoTool;
    
		/**  Pt cut on truth particles to be dressed  [MeV] */
    float m_ptCut;

    /**  Only dress primary truth particles */
    bool  m_doPrimary;


  }; 
 
}

#endif
