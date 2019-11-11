/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PHYSLITEThinningTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_PHYSLITETHINNINGTOOL_H
#define DERIVATIONFRAMEWORK_PHYSLITETHINNINGTOOL_H 1

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IThinningTool.h"

class IThinningSvc;

namespace DerivationFramework {

  /** @class PHYSLITEThinningTool
  
      @author James Catmore -at- cern.ch
     */
  class PHYSLITEThinningTool : public AthAlgTool, public IThinningTool {
    
  public: 
    /** Constructor with parameters */
    PHYSLITEThinningTool( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~PHYSLITEThinningTool();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    /** Check that the current event passes this filter */
    virtual StatusCode doThinning() const;
 
  private:
    ServiceHandle<IThinningSvc> m_thinningSvc;
    std::vector<std::string> m_containersToThin;
  }; 
  
}


#endif
