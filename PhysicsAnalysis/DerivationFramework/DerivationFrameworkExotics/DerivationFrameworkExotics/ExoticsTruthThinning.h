/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExoticsThinningTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EXOTICSTRUTHTHINNING_H
#define DERIVATIONFRAMEWORK_EXOTICSTRUTHTHINNING_H 1

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IThinningTool.h"

class IThinningSvc;

namespace DerivationFramework {

  /** @class ExoticsThinningTool
  
      @author James Catmore -at- cern.ch
     */
  class ExoticsTruthThinning : public AthAlgTool, public IThinningTool {
    
  public: 
    /** Constructor with parameters */
    ExoticsTruthThinning( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~ExoticsTruthThinning();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    /** Check that the current event passes this filter */
    virtual StatusCode doThinning() const;
 
  }; 
  
}


#endif
