/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ThinningToolExample.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_THINNINGTOOLEXAMPLE_H
#define DERIVATIONFRAMEWORK_THINNINGTOOLEXAMPLE_H 1

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IThinningTool.h"

class IThinningSvc;

namespace DerivationFramework {

  /** @class ThinningToolExample
  
      @author James Catmore -at- cern.ch
     */
  class ThinningToolExample : public AthAlgTool, public IThinningTool {
    
  public: 
    /** Constructor with parameters */
    ThinningToolExample( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~ThinningToolExample();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    /** Check that the current event passes this filter */
    virtual StatusCode doThinning() const;
 
  private:
    ServiceHandle<IThinningSvc> m_thinningSvc;
    mutable unsigned int m_ntot;
    mutable unsigned int m_npass;
    double m_trackPtCut;
  }; 
  
}


#endif
