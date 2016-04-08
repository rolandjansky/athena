/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// trackIsolationDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef DERIVATIONFRAMEWORK_trackIsolationDecorator_H
#define DERIVATIONFRAMEWORK_trackIsolationDecorator_H

#include<string>
#include<vector>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "RecoToolInterfaces/ITrackIsolationTool.h"

namespace DerivationFramework {
  /** @class trackIsolationDecorator
      @author christopher.young@cern.ch
     */
  class trackIsolationDecorator : public AthAlgTool, public IAugmentationTool {
    
  public: 
    /** Constructor with parameters */
    trackIsolationDecorator( const std::string& t, const std::string& n, const IInterface* p);
    
    /** Destructor */
    ~trackIsolationDecorator();
 
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    virtual StatusCode addBranches() const;
    
  private:
    std::string m_containerName;
    std::string m_prefix;
  
    /// Athena configured tools
    ToolHandle<xAOD::ITrackIsolationTool> m_trackIsolationTool;

    std::vector<xAOD::Iso::IsolationType> m_ptconeTypes;
    std::vector< int > m_ptcones;
    xAOD::TrackCorrection m_trkCorrList; 

    std::vector< SG::AuxElement::Decorator< float >* > m_decorators;
  }; 
}
#endif //
