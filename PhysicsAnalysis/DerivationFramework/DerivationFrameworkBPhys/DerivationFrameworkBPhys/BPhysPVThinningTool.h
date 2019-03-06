/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BPhysPVThinningTool.h
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_BPhysPVThinningTool_H
#define DERIVATIONFRAMEWORK_BPhysPVThinningTool_H 1

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IThinningTool.h"

class IThinningSvc;

namespace DerivationFramework {

  
  class BPhysPVThinningTool : public AthAlgTool, public IThinningTool {
    
  public: 
    /** Constructor with parameters */
    BPhysPVThinningTool( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~BPhysPVThinningTool();
    
    // Athena algtool's Hooks
    virtual StatusCode  initialize() override;
    virtual StatusCode  finalize() override;
    
    /** Check that the current event passes this filter */
    virtual StatusCode doThinning() const override;
 
  private:
    ServiceHandle<IThinningSvc> m_thinningSvc;
    std::vector<std::string> m_BPhyCandList;
    bool m_keepTracks;
    std::string m_TrackContainerName;
    std::string m_PVContainerName;
    mutable unsigned int m_ntot;
    mutable unsigned int m_npass;
    mutable unsigned int m_tracks_kept;
  }; 
  
}


#endif
