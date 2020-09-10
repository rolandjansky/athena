/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AugmentationExample.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_HIGLOBALAUGMENTATIONTOOL_H
#define DERIVATIONFRAMEWORK_HIGLOBALAUGMENTATIONTOOL_H


#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AsgTools/ToolHandle.h"
#include <string>
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

class IThinningSvc;

namespace DerivationFramework {

  class HIGlobalAugmentationTool : public AthAlgTool, public IAugmentationTool {
    
  public: 
    HIGlobalAugmentationTool(const std::string& t, const std::string& n, const IInterface* p);
    ~HIGlobalAugmentationTool();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    virtual StatusCode addBranches() const;
 
  private:
    int m_nHarmonic;
    std::string m_TP_key;
    ToolHandleArray< InDet::IInDetTrackSelectionTool > m_trkSelTools; //!< track selection tool which can be optionally used for N_trk and sum pt cuts
    std::vector<std::string>  m_cutLevels;
    
    // Set up the decorators
    std::vector< SG::AuxElement::Decorator< float >> m_decFCalEtA_Qnx;
    std::vector< SG::AuxElement::Decorator< float >> m_decFCalEtA_Qny;
    std::vector< SG::AuxElement::Decorator< float >> m_decFCalEtC_Qnx;
    std::vector< SG::AuxElement::Decorator< float >> m_decFCalEtC_Qny;
    
    std::vector< SG::AuxElement::Decorator< float >> m_decHalfFCalEtA_Qnx;
    std::vector< SG::AuxElement::Decorator< float >> m_decHalfFCalEtA_Qny;
    std::vector< SG::AuxElement::Decorator< float >> m_decHalfFCalEtC_Qnx;
    std::vector< SG::AuxElement::Decorator< float >> m_decHalfFCalEtC_Qny;

    std::vector< SG::AuxElement::Decorator< int >> m_decTrack_count;
  
  }; 
  
}


#endif
