/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONSEGMENTTAGTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONSEGMENTTAGTOOL_H

#include "MuonCombinedToolInterfaces/IMuonSegmentTagTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MSSurfaces.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonSegmentTaggerToolInterfaces/IMuTagAmbiguitySolverTool.h"
#include "MuonSegmentTaggerToolInterfaces/IMuTagMatchingTool.h"
#include "MuonSegmentTaggerToolInterfaces/IMuTagIMOTool.h"
#include "MuonCombinedEvent/MuonSegmentInfo.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentSelectionTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentHitSummaryTool.h"

#include <array>
#include <atomic>
#include <string>
#include <vector>

namespace Muon {
  class MuonSegment;
}

namespace MuonCombined {

  class MuonSegmentTagTool: public AthAlgTool, virtual public IMuonSegmentTagTool
  {
    using SegmentMap = std::map< const Muon::MuonSegment*, ElementLink<xAOD::MuonSegmentContainer> >;
  public:
    MuonSegmentTagTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~MuonSegmentTagTool()=default;
  
    StatusCode initialize();
    StatusCode finalize();

    /**IMuonSegmentTagTool interface: build muons from ID and MuonSegments */    
    void tag( const InDetCandidateCollection& inDetCandidates, const xAOD::MuonSegmentContainer& segments, InDetCandidateToTagMap* tagMap ) const;
    void tag( const InDetCandidateCollection& inDetCandidates, const std::vector<const Muon::MuonSegment*>& segments, SegmentMap* segmentToxAODSegmentMap, InDetCandidateToTagMap* tagMap ) const;



  private:

    void printTable( std::vector< std::string > didEx ,  
		     std::vector< std::string > segStation , 
		     std::vector< std::vector<std::string> > segToSurf  ) const; //!< method for extra DEBUG

    float m_CutTheta           ;//!< Track/Segment pairing
    float m_CutPhi             ;//!< Track/Segment pairing 
    float m_HitInfoFor2ndCoord ;//!< Track/Segment pairing
    bool  m_doSegmentsFilter;      //!< flag to switch segment filtering on/off (for trigger)
    bool  m_doTable;
    bool  m_doBidirectional ;   //!< flag to toggle to bidirectional extrapolation.
    bool  m_doPtDependentPullCut; //!< flag to enable the pT-dependent pull cut
    bool m_removeLowPLowFieldRegion; //! boolean to remove track with p < 6 GeV in eta 1.4-17 region
    bool m_useSegmentPreselection; // use loose ID / segment matching to avoid extrapolations
    int  m_segmentQualityCut; // minimum segment quality
    unsigned int m_nmdtHits;
    unsigned int m_nmdtHoles;
    unsigned int m_nmdtHitsML;
    bool m_triggerHitCut; // apply Trigger hit cut if trigger hits are expected
    bool m_makeMuons; // switch off the making of muons (temporarily)
    bool m_ignoreSiAssocated; //!< If true, ignore InDetCandidates which are SiAssociated

    // helpers, managers, tools
    ToolHandle<Muon::MuonEDMPrinterTool>            m_printer;


    ToolHandle< IMuTagMatchingTool       > p_MuTagMatchingTool        ;  //!< Pointer to MuTagMatching Tool 
    ToolHandle< IMuTagAmbiguitySolverTool> p_MuTagAmbiguitySolverTool ;  //!< Pointer to MuTagAmbiguitySolverTool
    ToolHandle <Trk::IParticleCaloExtensionTool> m_caloExtensionTool; //!< Tool to make the step-wise extrapolation
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc {this, "edmHelper", 
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
      "Handle to the service providing the IMuonEDMHelperSvc interface" };  //!< Pointer to IPropagator
    ToolHandle< Muon::IMuonSegmentSelectionTool> m_segmentSelector; 
    ToolHandle<Muon::IMuonSegmentHitSummaryTool> m_hitSummaryTool;

    MSSurfaces*   m_surfaces;     //!< Pointer to a set of abstract surfaces describing MuonSpectrometer levels.
    mutable std::atomic_uint m_ntotTracks;
    mutable std::atomic_uint m_nangleMatch;
    mutable std::atomic_uint m_npmatch;
    mutable std::atomic_uint m_natMSEntrance;
    mutable std::atomic_uint m_naccepted;
    mutable std::array<std::atomic_int, 15> m_extrapolated ATLAS_THREAD_SAFE
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // 15 is maximum possible size. Guarded by atomicity
    mutable std::array<std::atomic_int, 15> m_goodExtrapolated ATLAS_THREAD_SAFE
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // 15 is maximum possible size. Guarded by atomicity

  };

}	// end of namespace

#endif


