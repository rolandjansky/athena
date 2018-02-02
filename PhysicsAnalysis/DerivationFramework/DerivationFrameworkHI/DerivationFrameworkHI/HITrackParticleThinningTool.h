/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ThinningToolExample.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_HITRACKPARTICLETHINNINGTOOL_H
#define DERIVATIONFRAMEWORK_HITRACKPARTICLETHINNINGTOOL_H


#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include <string>
// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "AsgTools/ToolHandle.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

class IThinningSvc;

namespace DerivationFramework {

  class HITrackParticleThinningTool : public AthAlgTool, public IThinningTool
  {
    
  public: 
    HITrackParticleThinningTool( const std::string& t, const std::string& n, const IInterface* p );
    ~HITrackParticleThinningTool();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    /** Check that the current event passes this filter */
    virtual StatusCode doThinning() const;
 
  private:
    ServiceHandle<IThinningSvc> m_thinningSvc;
    std::string m_TP_key;
    std::string m_vertex_key;
    std::string m_vertex_scheme;
    ToolHandle< InDet::IInDetTrackSelectionTool > m_trkSelTool; //!< track selection tool which can be optionally used for N_trk and sum pt cuts
  }; 
  
}


#endif
