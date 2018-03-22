/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef ASG_ANALYSIS_ALGORITHMS__ASG_PILEUP_REWEIGHTING_ALG_H
#define ASG_ANALYSIS_ALGORITHMS__ASG_PILEUP_REWEIGHTING_ALG_H

#include <xAODEventInfo/EventInfo.h>
#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AsgAnalysisInterfaces/IPileupReweightingTool.h>
#include <SelectionHelpers/OutOfValidityHelper.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>

namespace CP
{
  /// \brief an algorithm for calling \ref IPileupReweightingTool

  class AsgPileupReweightingAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    AsgPileupReweightingAlg (const std::string& name, 
                             ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// \brief the smearing tool
  private:
    ToolHandle<IPileupReweightingTool> m_pileupReweightingTool;

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the jet collection we run on
  private:
    SysCopyHandle<xAOD::EventInfo> m_eventInfoHandle {
      this, "eventInfo", "EventInfo", "the event info object to run on"};

    /// \brief the helper for OutOfValidity results
  private:
    OutOfValidityHelper m_outOfValidity {this};
  };
}

#endif
