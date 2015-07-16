/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////
// RpvMuonMSTool.h  (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_RPVMUONMSTOOL_H
#define DERIVATIONFRAMEWORK_RPVMUONMSTOOL_H 1

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  /** @class RpvMuonMSTool

      the code used in this implementation is kindly stolen from:
      atlasoff:: ISF/ISF_Core/ISF_Tools

      @author Michael Flowerdew -at- cern.ch
     */
  class RpvMuonMSTool : public AthAlgTool, public IAugmentationTool {
   
  public:
    /** Constructor with parameters */
    RpvMuonMSTool( const std::string& t, const std::string& n, const IInterface* p );
   
    /** Destructor */
    ~RpvMuonMSTool();
   
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
 
    /** Check that the current event passes this filter */
    virtual StatusCode addBranches() const;

  private:
    std::string m_collName;
    std::string m_sgPrefix;

  };

}

#endif
