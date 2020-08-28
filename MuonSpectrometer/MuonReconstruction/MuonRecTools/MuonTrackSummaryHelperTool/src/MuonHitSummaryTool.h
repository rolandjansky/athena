/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONHITSUMMARYTOOL_H
#define MUON_MUONHITSUMMARYTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMuonHitSummaryTool.h"
#include "TrkToolInterfaces/ITrackSummaryHelperTool.h"

namespace Muon {
/**
   @brief Helper tool to extract simple counts from a Trk::Track, Trk::TrackSummary or Trk::MuonTrackSummary

*/
class MuonHitSummaryTool : public AthAlgTool, virtual public IMuonHitSummaryTool {
  public:
    MuonHitSummaryTool(const std::string&, const std::string&, const IInterface*);

    virtual ~MuonHitSummaryTool() = default;

    StatusCode initialize();

    /** @brief Calculate compact summary
        @param track input track
        @return CompactSummary
    */
    virtual CompactSummary summary(const Trk::Track& track) const;

    /** @brief Calculate compact summary
        @param summary TrackSummary of input track
        @return CompactSummary
    */
    virtual CompactSummary summary(const Trk::TrackSummary& summary) const;

    /** @brief Calculate compact summary. Can fail if summary does not have a MuonTrackSummary.
        @param summary MuonTrackSummary of input track
        @return CompactSummary
    */
    virtual CompactSummary summary(const Trk::MuonTrackSummary& summary) const;

    /** @brief Calculate compact summary
        @param segment input MuonSegment
        @return CompactSummary
    */
    virtual CompactSummary summary(const Muon::MuonSegment& segment) const;

    /** @brief Calculate compact summary
        @param segments input vector of MuonSegments
        @return CompactSummary
    */
    virtual CompactSummary summary(const std::vector<const Muon::MuonSegment*>& segments) const;


    /** @brief Calculate compact summary
        @param measurements input vector of MeasurementBase
        @return CompactSummary
    */
    virtual CompactSummary summary(const std::vector<const Trk::MeasurementBase*>& measurements) const;

  private:
    /** helper function to calculate MuonTrackSummary from track */
    void getMuonTrackSummary(Trk::MuonTrackSummary& muonSummary, const Trk::Track& track) const;
    void calculateSummaryCounts(CompactSummary& sum) const;
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc",
                                                        "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ServiceHandle<IMuonEDMHelperSvc>      m_edmHelperSvc{this, "edmHelper", "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
                                                    "Handle to the service providing the IMuonEDMHelperSvc interface"};
    ToolHandle<MuonEDMPrinterTool>        m_printer;
    ToolHandle<Trk::ITrackSummaryHelperTool> m_summaryHelperTool;
};

}  // namespace Muon

#endif  // MUON_IDHELPERTOOL_H
