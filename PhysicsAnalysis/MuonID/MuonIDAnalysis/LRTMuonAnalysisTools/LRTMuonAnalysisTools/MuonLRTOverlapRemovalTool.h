/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LRTMUONANALYSISTOOLS_MuonLRTOverlapRemovalTool_H
#define LRTMUONANALYSISTOOLS_MuonLRTOverlapRemovalTool_H

#include "MuonAnalysisInterfaces/IMuonLRTOverlapRemovalTool.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include <string>
#include <map>

#include "xAODTracking/TrackParticle.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"

#include <AsgTools/ToolHandle.h>
#include <AsgTools/AsgTool.h>
#include <AsgTools/PropertyWrapper.h>

namespace CP {

  /** @brief Class-algorithm for muon particle collection merging*/
  class MuonLRTOverlapRemovalTool : virtual public CP::IMuonLRTOverlapRemovalTool, public asg::AsgTool {
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      /** @brief Standard Algotithm methods:                           */
      ///////////////////////////////////////////////////////////////////

      MuonLRTOverlapRemovalTool(const std::string& name);
      virtual ~MuonLRTOverlapRemovalTool() = default;
      ASG_TOOL_CLASS( MuonLRTOverlapRemovalTool, CP::IMuonLRTOverlapRemovalTool )
      virtual StatusCode initialize();

      /// resolve the overlap between a pair of muons, one from the prompt and one from the LRT pass. 
      /// returns a pair of booleans, indicating whether the prompt and LRT muon, respectively, 
      /// should be retained based on a minimal set of rules.
      virtual std::pair<bool, bool> resolveOverlap( const xAOD::Muon* promptMuon,
                                                  const xAOD::Muon* lrtMuon) const;

      /// checks for overlap between a pair of muons, one from the prompt and one from the LRT pass.
      /// using the same set of rules as resolveOverlap, decorates the muons with a variable MuonLRTOverlapDecision which is
      /// 0 for muons which don't overlap, 1 if they overlap and are discareded, and 2 if they overlap and are retained.
      virtual void checkOverlapAndDecor(const xAOD::Muon* promptMuon,
                                                         const xAOD::Muon* lrtMuon) const;

      /// check the overlap between the prompt and LRT muon collections.
      /// Will populate the two vectors passed by ref with a decision for each muon in 
      /// each collection. "true" entries are to be kept, "false" to be discarded. 
      virtual void checkOverlap(const xAOD::MuonContainer & promptCollection,
                                  const xAOD::MuonContainer &  lrtCollection,
                                  std::vector<bool>& promptMuonsSelectedToKeep,
                                  std::vector<bool>& lrtMuonsSelectedToKeep ) const;

      /// checks the overlap between a pair of muons, one from the prompt and one from the LRT pass.
      /// returns true if they do overlap and false if they don't.
      virtual bool hasOverlap(const xAOD::Muon* promptMuon,
                              const xAOD::Muon* lrtMuon) const;

    private:
      /// This allows to configure the OR strategy in the future, if more than one is supported by MCP. 
      Gaudi::Property<int>  m_strategy{this, "overlapStrategy", CP::IMuonLRTOverlapRemovalTool::defaultStrategy, "Overlap removal strategy to use (0 = default, 1 = write all muons and decorate with overlap type)"}; 
      /// This is the muon selection tool. No particular configuration required, the loosest WP is used as a tie breaker in overlaps 
      ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool{this, "MuonSelectionTool", "", "tool to determine the working point of the muons"};

    }; // end namespace CP

}
#endif // LRTMUONANALYSISTOOLS_MuonLRTOverlapRemovalTool_H
