/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_isolationDecorator_H
#define DERIVATIONFRAMEWORK_isolationDecorator_H

#include <string>
#include <vector>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "RecoToolInterfaces/ITrackIsolationTool.h"
#include "RecoToolInterfaces/ICaloTopoClusterIsolationTool.h"
#include "ExpressionEvaluation/ExpressionParserUser.h"

namespace DerivationFramework {
  class isolationDecorator : public ExpressionParserUser<AthAlgTool>, public IAugmentationTool {
    
  public: 
    /** Constructor with parameters */
    isolationDecorator( const std::string& t, const std::string& n, const IInterface* p);
    
    /** Destructor */
    virtual ~isolationDecorator()=default;
 
    
    StatusCode  initialize() override;   
    
    StatusCode addBranches() const override;
    
  private:
    StatusCode decorate(const xAOD::IParticle* part, const int iso_type, const float val) const;
    
    std::string m_containerName;
    std::string m_selectionString;
    std::string m_prefix;
    std::string m_selFlag;
    int m_selFlagValue;
  
    /// Athena configured tools
    ToolHandle<xAOD::ITrackIsolationTool> m_trackIsolationTool;
    ToolHandle<xAOD::ICaloTopoClusterIsolationTool> m_caloIsolationTool;

    std::vector<xAOD::Iso::IsolationType> m_ptconeTypes;
    std::vector< int > m_ptcones;
    xAOD::TrackCorrection m_trkCorrList; 
    std::vector<xAOD::Iso::IsolationType> m_topoetconeTypes;
    std::vector<int> m_topoetcones;
    xAOD::CaloCorrection m_caloCorrList;

    std::map<int, SG::AuxElement::Decorator< float > > m_decorators;
    
    
  }; 
}
#endif //
