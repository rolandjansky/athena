/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef DERIVATIONFRAMEWORK_HIGAPSKIMMINGTOOL_H
#define DERIVATIONFRAMEWORK_HIGAPSKIMMINGTOOL_H

////////////////////////////////////////////////////////////////////////////////
///
/// \class HIGapSkimming
/// \author Aaron Angerami <angerami@cern.ch>
/// \date June, 2020
///
////////////////////////////////////////////////////////////////////////////////


#include <string>
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace HI
{
  class ITopoClusterSelectionTool;
}
namespace InDet
{
  class IInDetTrackSelectionTool;
}
class IJetSelector;

namespace DerivationFramework {

  class HIGapSkimmingTool : public AthAlgTool, public ISkimmingTool {

  public: 
    HIGapSkimmingTool( const std::string& t, const std::string& n, const IInterface* p );

    ~HIGapSkimmingTool();

    // AthAlgTool interface
    StatusCode  initialize() override;
    StatusCode  finalize() override;

    // ISkimmingTool interface
    virtual bool eventPassesFilter() const override;

  private:

    std::string m_clusterContainerName;
    std::string m_trackContainerName;
    ToolHandle<HI::ITopoClusterSelectionTool> m_tcSelectionTool;
    ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelectionTool;
    float m_gapMin;

    ToolHandle<IJetSelector> m_jetSelectionTool;
    std::string m_jetContainerName;
    bool m_useJetGaps;
    bool m_doJetSelection;

    float m_maxSumGapCut; //larger of two sum gaps must be GREATER THAN this quantity
    float m_minSumGapCut; //smaller of two sum gaps must be LESS THAN this quantity
    float m_maxEdgeGapCut; //larger of two edge gaps must be GREATER THAN this quantity
    float m_minEdgeGapCut; //smaller of two edge gaps must be LESS THAN this quantity

  }; 

}

#endif // DERIVATIONFRAMEWORK_HIGAPSKIMMINGTOOL_H
