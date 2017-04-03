/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONSEGMENTTAGTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONSEGMENTTAGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IMuonSegmentTagTool.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include <vector>
#include "TrkParameters/TrackParameters.h"
#include "MuonSegmentTaggerToolInterfaces/IMuTagAmbiguitySolverTool.h"
#include "MuonSegmentTaggerToolInterfaces/IMuTagMatchingTool.h"
#include "MSSurfaces.h"
#include "MuonSegmentTaggerToolInterfaces/IMuTagIMOTool.h"
#include "MuonCombinedEvent/MuonSegmentInfo.h"
#include "xAODMuon/MuonSegmentContainer.h"


#include <string>
namespace Trk {
  class IParticleCaloExtensionTool;
}

namespace Muon {
  class MuonSegment;
  class MuonEDMPrinterTool;
  class MuonIdHelperTool;
  class MuonEDMHelperTool;
  class IMuonSegmentSelectionTool;
}

namespace MuonCombined {

  class MuonSegmentTagTool: public AthAlgTool, virtual public IMuonSegmentTagTool
  {
    using SegmentMap = std::map< const Muon::MuonSegment*, ElementLink<xAOD::MuonSegmentContainer> >;
  public:
    MuonSegmentTagTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~MuonSegmentTagTool(void); // destructor
  
    StatusCode initialize();
    StatusCode finalize();

    /**IMuonSegmentTagTool interface: build muons from ID and MuonSegments */    
    void tag( const InDetCandidateCollection& inDetCandidates, const xAOD::MuonSegmentContainer& segments ) const;
    void tag( const InDetCandidateCollection& inDetCandidates, const std::vector<const Muon::MuonSegment*>& segments, SegmentMap* segmentToxAODSegmentMap ) const;



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
    ToolHandle< Muon::MuonIdHelperTool   > m_idHelper    ;  //!< Pointer to IPropagator
    ToolHandle< Muon::MuonEDMHelperTool  > m_edmHelper    ;  //!< Pointer to IPropagator
    ToolHandle< Muon::IMuonSegmentSelectionTool> m_segmentSelector; 
    ToolHandle<Muon::IMuonSegmentHitSummaryTool> m_hitSummaryTool;

    MSSurfaces*   m_surfaces;     //!< Pointer to a set of abstract surfaces describing MuonSpectrometer levels.
    mutable unsigned int  m_ntotTracks;
    mutable unsigned int  m_nangleMatch;
    mutable unsigned int  m_npmatch;
    mutable unsigned int  m_natMSEntrance;
    mutable unsigned int  m_naccepted;
    mutable std::vector<int> m_extrapolated;
    mutable std::vector<int> m_goodExtrapolated;

  };

}	// end of namespace

#endif


