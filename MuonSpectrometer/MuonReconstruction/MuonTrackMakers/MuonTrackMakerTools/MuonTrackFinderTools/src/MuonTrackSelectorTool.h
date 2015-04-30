/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONTRACKSELECTOR_H
#define MUON_MUONTRACKSELECTOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "Identifier/Identifier.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"

#include <string>
#include <set>
#include <vector>

static const InterfaceID IID_MuonTrackSelectorTool("Muon::MuonTrackSelectorTool",1,0);

class MsgStream;


namespace Muon {
  class MuonIdHelperTool;
  class MuonEDMHelperTool;
  class MuonEDMPrinterTool;
}

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Trk {
  class Track;
  class ITrackSummaryHelperTool;
}

namespace Muon {

  /**
     @brief tool to select tracks

  */
  class MuonTrackSelectorTool : virtual public Trk::ITrackSelectorTool, public AthAlgTool {
  public:
    /** @brief constructor */
    MuonTrackSelectorTool(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    virtual ~MuonTrackSelectorTool ();
    
    /** @brief AlgTool initilize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();
    
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuonTrackSelectorTool; }

    /** @brief calculate holes in a given chamber using local straight line extrapolation
	@param pars TrackParameters in the chamber
	@param chId Identifier of the chamber
	@param tubeIds set containing the Identifier of the hits that should not be counted as holes
	@return a vector of hole Identifiers
    */
    std::vector<Identifier> holesInChamber( const Trk::TrackParameters& pars, const Identifier& chId, const std::set<Identifier>& tubeIds ) const;
    
    /** @brief returns true if the track satisfies the selection criteria else false */
    bool decision( const Trk::Track& track, const Trk::Vertex* = 0 ) const;

    /** @brief always returns false */
    bool decision( const Trk::TrackParticleBase&, const Trk::Vertex* ) const { return false; }

    /** @brief always returns false */
    bool decision(const xAOD::TrackParticle&,const xAOD::Vertex* ) const { return false; }

  private:
    ToolHandle<Muon::MuonIdHelperTool>               m_idHelperTool;     //!< IdHelper tool
    ToolHandle<Muon::MuonEDMHelperTool>              m_helperTool;       //!< EDM Helper tool
    ToolHandle<Muon::MuonEDMPrinterTool>             m_printer;          //!< EDM printer tool
    ToolHandle<Trk::ITrackSummaryHelperTool>         m_trackSummaryTool; //<! muon id helper

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
    mutable unsigned int m_ntotalTracks;
    mutable unsigned int m_failedChi2NDofCut;
    mutable unsigned int m_failedSingleStationCut;
    mutable unsigned int m_failedRPCAveMinTimeCut;
    mutable unsigned int m_failedRPCAveMaxTimeCut;
    mutable unsigned int m_failedRPCSpreadTimeCut;
    mutable unsigned int m_failedTwoStationsCut; 
    mutable unsigned int m_failedTwoStationsMaxMDTHoleCut;
    mutable unsigned int m_failedTwoStationsMaxHoleCut;
    mutable unsigned int m_failedTwoStationsGoodStationCut;
    mutable unsigned int m_failedTriggerStationCut;
    mutable unsigned int m_failedMaxMDTHoleCut;
    mutable unsigned int m_failedMaxHoleCut;

  };

}

#endif
