/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyMuonTrackBuilderTester_H
#define MboyMuonTrackBuilderTester_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

//Can not be forwarded due to typedef
#include "TrkSegment/Segment.h"
#include "TrkSegment/SegmentCollection.h"

//Can not be forwarded due to typedef
#include "TrkParameters/TrackParameters.h"

namespace Muon {
  class MuPatCandidateBase;
  class MuPatSegment;
  class MuPatTrack;  
  class MuonSegment;  
  class MuonEDMHelperTool;
  class IMuonSegmentSelectionTool;
  class MuonIdHelperTool;
}

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthTool/MboyAthToolMisc.h"

class IMboyAthToolHelper;

#include "MboyAthTool/MboyMuonTrackBuilder.h"

  /**
   @class MboyMuonTrackBuilderTester

  @author samusog@cern.ch
  
  */

class MboyMuonTrackBuilderTester:public AthAlgorithm {
public:
    MboyMuonTrackBuilderTester(const std::string& name, ISvcLocator* pSvcLocator);
   ~MboyMuonTrackBuilderTester();
   
public:
///////////////////////////////////

   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:
///////////////////////////////////
   void TryFind(
                const Trk::Track* pTrack ,
                std::vector<Muon::MuPatTrack*>*& pVectorOfMuPatTrack_StagePrevious , 
                std::vector<int>&  VectorOrdored ,
                std::vector<int>&  VectorOfKounterOnMuPatSegment ,
                std::vector<Muon::MuPatSegment*>& VectorOfMuPatSegmentLocal ,
                int StageNber
               );

   void DumpMuPatTrackS(const std::vector<Muon::MuPatTrack*>* pVectorOfMuPatTrack);
   
   void DumpMuPatTrack(const Muon::MuPatTrack* pMuPatTrack);

   void DumpMuPatSegmentS(const std::vector<Muon::MuPatSegment*>* pVectorOfMuPatSegment);
   
   void DumpMuPatSegment(const Muon::MuPatSegment* pMuPatSegment);

   int ScoreCommonHits( 
                       const Muon::MuPatSegment* pMuPatSegment,
                       const Trk::Track*    pTrack
                      );

   void sort(
             std::vector<double>& VectorOfDist2IPD,
             std::vector<int>&    VectorOrdored
            );

   Muon::MuPatSegment* createSegInfo( const Muon::MuonSegment& segment ) const ;

   void CompareTrkTrack(
                        const Trk::Track* pTrack_Original ,
                        const Trk::Track* pTrack_Refined  
                       );

   std::string m_SegmentCollectionLocation ; //!< Location of the SegmentCollection
   std::string m_TrackCollectionLocation   ; //!< Location of the TrackCollectionLocation

//JFL: Taken from MuPatCandidateTool::createSegInfo Wed May  4 17:16:24 CEST 2011
   ToolHandle<Muon::MuonIdHelperTool>                      m_idHelperTool;       //<! tool to assist with Identifiers
   ToolHandle<Muon::IMuonSegmentSelectionTool>       m_segmentSelector;    //<! tool to resolve track ambiguities
   ToolHandle<Muon::MuonEDMHelperTool>                     m_helperTool;         //<! multipurpose helper tool

   ToolHandle< IMboyAthToolHelper >   p_OwnEDMHelper          ; //!< Pointer On IMboyAthToolHelper
   ToolHandle< Muon::IMuonSegmentInfoExtender > p_IMuonSegmentInfoExtender  ; //!< Pointer On IMuonSegmentInfoExtender
   ToolHandle< Muon::IMuonTrackBuilder > p_IMuonTrackBuilder ; //!< Pointer On IMuonTrackBuilder
   ToolHandle< Muon::IMuonTrackRefiner > p_IMuonTrackRefiner ; //!< Pointer On IMuonTrackRefiner
   ToolHandle< Muon::MboyMuonTrackBuilder > p_MboyMuonTrackBuilder ; //!< Pointer On Muon::MboyMuonTrackBuilder


};

#endif

