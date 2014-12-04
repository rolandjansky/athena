/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////
// TrackParametersAtPV.h  (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TRACKPARAMETERSATPV_H
#define DERIVATIONFRAMEWORK_TRACKPARAMETERSATPV_H 

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  /** @class TrackParametersAtPV
 
      the code used in this implementation is kindly stolen from:
      atlasoff:: ISF/ISF_Core/ISF_Tools

      @author James Catmore -at- cern.ch
     */
  class TrackParametersAtPV : public AthAlgTool, public IAugmentationTool {
   
  public: 
    /** Constructor with parameters */
    TrackParametersAtPV( const std::string& t, const std::string& n, const IInterface* p );
   
    /** Destructor */
    ~TrackParametersAtPV();
   
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
 
    /** Check that the current event passes this filter */
    virtual StatusCode addBranches() const;

  private:
    std::string m_collTrackName;
    std::string m_collVertexName;
    std::string m_sgKey1;

  }; 
 
}

#endif
