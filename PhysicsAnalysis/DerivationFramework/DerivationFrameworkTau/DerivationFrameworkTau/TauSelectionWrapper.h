/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////
// TauSelectionWrapper.h  (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TAUSELECTIONWRAPPER_H
#define DERIVATIONFRAMEWORK_TAUSELECTIONWRAPPER_H 1

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  /** @class TauSelectionWrapper
 
      the code used in this implementation is kindly stolen from:
      atlasoff:: ISF/ISF_Core/ISF_Tools

      @author James Catmore -at- cern.ch
     */
  class TauSelectionWrapper : public AthAlgTool, public IAugmentationTool {
   
  public: 
    /** Constructor with parameters */
    TauSelectionWrapper( const std::string& t, const std::string& n, const IInterface* p );
   
    /** Destructor */
    ~TauSelectionWrapper();
   
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
 
    /** Check that the current event passes this filter */
    virtual StatusCode addBranches() const;

  private:
    int m_isTauFlag;
    std::string m_collName;
    std::string m_sgKey;

  }; 
 
}

#endif
