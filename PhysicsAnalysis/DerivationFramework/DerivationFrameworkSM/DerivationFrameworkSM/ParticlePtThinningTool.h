//////////////////////////////////////////////////////////////////
// ParticlePtThinningTool.h
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_ParticlePtThinningTool_H
#define DERIVATIONFRAMEWORK_ParticlePtThinningTool_H 1

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IThinningTool.h"

class IThinningSvc;

namespace DerivationFramework {

  class ParticlePtThinningTool : public AthAlgTool, public IThinningTool {
    
  public: 
    /** Constructor with parameters */
    ParticlePtThinningTool( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~ParticlePtThinningTool();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    /** Check that the current event passes this filter */
    virtual StatusCode doThinning() const;
 
  private:
    ServiceHandle<IThinningSvc> m_thinningSvc;
    
    // Thinning
    double m_PtCut;
    std::string m_ContainerName;
  }; 
  
}

#endif
