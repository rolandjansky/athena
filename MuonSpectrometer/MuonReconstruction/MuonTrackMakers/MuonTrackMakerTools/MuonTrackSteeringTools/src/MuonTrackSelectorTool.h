/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONTRACKSELECTOR_H
#define MUON_MUONTRACKSELECTOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "TrkToolInterfaces/ITrackSummaryHelperTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"

#include <atomic>
#include <string>
#include <set>
#include <vector>

namespace Trk {
  class Track;
}

namespace Muon {

  /**
     @brief tool to select tracks

  */
  class MuonTrackSelectorTool : public AthAlgTool {
  public:
    /** @brief constructor */
    MuonTrackSelectorTool(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    virtual ~MuonTrackSelectorTool()=default;
    
    /** @brief AlgTool initilize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();
    
    /** @brief calculate holes in a given chamber using local straight line extrapolation
	@param pars TrackParameters in the chamber
	@param chId Identifier of the chamber
	@param tubeIds set containing the Identifier of the hits that should not be counted as holes
	@return a vector of hole Identifiers
    */
    std::vector<Identifier> holesInChamber( const Trk::TrackParameters& pars, const Identifier& chId, const std::set<Identifier>& tubeIds ) const;
    
    /** @brief returns true if the track satisfies the selection criteria else false */
    bool decision( Trk::Track& track ) const;

  private:
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc {this, "edmHelper", 
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
      "Handle to the service providing the IMuonEDMHelperSvc interface" };       //!< EDM Helper tool
    ToolHandle<Muon::MuonEDMPrinterTool> m_printer {this, "EDMPrinter", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool", "helper to nicely print out tracks"};
    ToolHandle<Trk::ITrackSummaryHelperTool> m_trackSummaryTool {this, "TrackSummaryHelperTool", "Muon::MuonTrackSummaryHelperTool/MuonTrackSummaryHelperTool"};

    double m_holeHitRatioCutPerStation;
    double m_chi2NDofCut;
    unsigned int m_minMdtHitsPerStation;
    unsigned int m_maxMdtHolesPerTwoStationTrack;
    unsigned int m_maxMdtHolesPerTrack;
    unsigned int m_minCscHitsPerStation;

    bool m_useRPCHoles;
    bool m_useTGCHoles;
    bool m_useCSCHoles;
    bool m_useMDTHoles;
    bool m_ignoreTriggerHolesInLayersWithHits;
    bool m_useRPCTimeWindow;
    bool m_removeTwoStationTrackWithoutTriggerHits;
    bool m_countMdtOutliersAsHoles;
    bool m_removeSingleStationTracks;
    bool m_tightSingleStationCuts;

    /** internal data structure */
    struct StationData{
      StationData() : isMdt(false),isCsc(false),isNSW(false),isTrigger(false),mdtHasHitsinMl1(false),mdtHasHitsinMl2(false),
		      netaHits(0),nphiHits(0),netaHoles(0),nphiHoles(0),
		      netaTrigHits(0),nphiTrigHits(0),netaTrigHoles(0),nphiTrigHoles(0) {}
      bool isMdt;
      bool isCsc;
      bool isNSW;
      bool isTrigger;
      bool mdtHasHitsinBothMl() { return mdtHasHitsinMl1 && mdtHasHitsinMl2; }
      bool mdtHasHitsinMl1;
      bool mdtHasHitsinMl2;
      unsigned int netaHits;
      unsigned int nphiHits;
      unsigned int netaHoles;
      unsigned int nphiHoles;

      unsigned int netaTrigHits;
      unsigned int nphiTrigHits;
      unsigned int netaTrigHoles;
      unsigned int nphiTrigHoles;
    };

    /** counter for statistics */
    mutable std::atomic_uint m_ntotalTracks;
    mutable std::atomic_uint m_failedChi2NDofCut;
    mutable std::atomic_uint m_failedSingleStationCut;
    mutable std::atomic_uint m_failedRPCAveMinTimeCut;
    mutable std::atomic_uint m_failedRPCAveMaxTimeCut;
    mutable std::atomic_uint m_failedRPCSpreadTimeCut;
    mutable std::atomic_uint m_failedTwoStationsCut; 
    mutable std::atomic_uint m_failedTwoStationsMaxMDTHoleCut;
    mutable std::atomic_uint m_failedTwoStationsMaxHoleCut;
    mutable std::atomic_uint m_failedTwoStationsGoodStationCut;
    mutable std::atomic_uint m_failedTriggerStationCut;
    mutable std::atomic_uint m_failedMaxMDTHoleCut;
    mutable std::atomic_uint m_failedMaxHoleCut;

  };

}

#endif
