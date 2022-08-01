/*
 Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#ifndef IMUONLRTOVERLAPREMOVALTOOL__H
#define IMUONLRTOVERLAPREMOVALTOOL__H

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include <xAODEventInfo/EventInfo.h>

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"

// Local include(s):

/**
 * @mainpage MuonLRTOverlapRemovalTool 
 *
*/
namespace CP {

    class   IMuonLRTOverlapRemovalTool : public virtual asg::IAsgTool {
        ///
        /// @class IMuonLRTOverlapRemovalTool
        /// @brief Interface class.
        /// Abstract interface class. Provides the user interface for the
        /// MuonLRTOverlapRemovalTool class.

        ASG_TOOL_INTERFACE(CP::IMuonLRTOverlapRemovalTool)

        public:

        /// Allow to specify a number of MCP-supported overlap removal strategies.
        typedef enum {
            defaultStrategy=0, //default strategy which does a basic overlap removal.
            passThroughAndDecorate=1 //writes all the muons in the combined container irrespective of overlap, 
                                //and adds a decoration `MuonLRTOverlapDecision` which contains overlap information for all muons.
        } overlapStrategy;

        /// resolve the overlap between a pair of muons, one from the prompt and one from the LRT pass. 
        /// returns a pair of booleans, indicating whether the prompt and LRT muon, respectively, 
        /// should be retained based on a minimal set of rules.
        virtual std::pair<bool, bool> resolveOverlap(const xAOD::Muon* promptMuon,
                                                   const xAOD::Muon* lrtMuon) const = 0;

        /// checks for overlap between a pair of muons, one from the prompt and one from the LRT pass.
        /// using the same set of rules as resolveOverlap, decorates the muons with a variable `MuonLRTOverlapDecision` which is
        /// 0 for muons which don't overlap, 1 if they overlap and are discareded, and 2 if they overlap and are retained.
        virtual void checkOverlapAndDecor(const xAOD::Muon* promptMuon,
                                                           const xAOD::Muon* lrtMuon) const = 0;

        /// check the overlap between the prompt and LRT muon collections.
        /// Will populate the two vectors passed by ref with a decision for each muon in 
        /// each collection. "true" entries are to be kept, "false" to be discarded. 
        virtual void checkOverlap(const xAOD::MuonContainer & promptCollection,
                                  const xAOD::MuonContainer & lrtCollection,
                                  std::vector<bool>& promptMuonsSelectedToKeep,
                                  std::vector<bool>& lrtMuonsSelectedToKeep ) const = 0;

        /// checks the overlap between a pair of muons, one from the prompt and one from the LRT pass.
        /// returns true if they do overlap and false if they don't.
        virtual bool hasOverlap(const xAOD::Muon* promptMuon,
                              const xAOD::Muon* lrtMuon) const = 0;                              
    };
    // class IMuonLRTOverlapRemovalTool

}  // namespace CP

#endif /* IMUONLRTOVERLAPREMOVALTOOL__H */
