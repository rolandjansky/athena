/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _DerivationFrameworkInDet_VsiTrackThinningTool_H
#define _DerivationFrameworkInDet_VsiTrackThinningTool_H

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IThinningTool.h"

#include<string>

/** Forward declarations **/
class IThinningSvc;

namespace DerivationFramework {
  
  class VsiTrackThinningTool : public AthAlgTool, public IThinningTool {
    
  public: 
    /** Constructor with parameters */
    VsiTrackThinningTool( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~VsiTrackThinningTool();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    /** Check that the current event passes this filter */
    virtual StatusCode doThinning() const;
 
  private:
    ServiceHandle<IThinningSvc> m_thinningSvc;
    mutable unsigned int m_ntot;
    mutable unsigned int m_npass;
    std::string m_trackContainerName;
    std::string m_vertexContainerName;
  }; 
  
} // end of namespace bracket


#endif /* _DerivationFrameworkInDet_VsiTrackThinningTool_H */
