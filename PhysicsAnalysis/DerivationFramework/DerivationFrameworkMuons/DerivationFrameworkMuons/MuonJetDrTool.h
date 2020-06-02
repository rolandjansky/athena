/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonJetDrTool.h
///////////////////////////////////////////////////////////////////
#ifndef DERIVATIONFRAMEWORK_MuonJetDrTool_H
#define DERIVATIONFRAMEWORK_MuonJetDrTool_H

//#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {
  class MuonJetDrTool : public AthAlgTool, public IAugmentationTool {
    
  public: 
    /** Constructor with parameters */
    MuonJetDrTool( const std::string& t, const std::string& n, const IInterface* p);
    
    /** Destructor */
    ~MuonJetDrTool()=default;
 
    virtual StatusCode addBranches() const override;
    
  private:
    std::string m_muonSGKey;
    std::string m_jetSGKey;
    float m_jetMinPt;

  }; 
}
#endif //
