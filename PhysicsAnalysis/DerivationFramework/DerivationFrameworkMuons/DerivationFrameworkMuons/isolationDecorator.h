/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// isolationDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef DERIVATIONFRAMEWORK_isolationDecorator_H
#define DERIVATIONFRAMEWORK_isolationDecorator_H

#include<string>
#include<vector>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "RecoToolInterfaces/ITrackIsolationTool.h"
#include "RecoToolInterfaces/ICaloTopoClusterIsolationTool.h"
#include "ExpressionEvaluation/ExpressionParser.h"

namespace DerivationFramework {
  /** @class isolationDecorator
      @author Dongliang.Zhang@cern.ch
     */
  class isolationDecorator : public AthAlgTool, public IAugmentationTool {
    
  public: 
    /** Constructor with parameters */
    isolationDecorator( const std::string& t, const std::string& n, const IInterface* p);
    
    /** Destructor */
    ~isolationDecorator();
 
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    virtual StatusCode addBranches() const;
    
  private:
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

    ExpressionParsing::ExpressionParser *m_parser;
    std::vector< SG::AuxElement::Decorator< float >* > m_decorators;
  }; 
}
#endif //
