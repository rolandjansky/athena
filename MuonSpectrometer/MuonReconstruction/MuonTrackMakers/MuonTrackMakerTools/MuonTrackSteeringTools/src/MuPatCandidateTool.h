/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUPATCANDIDATETOOL_H
#define MUPATCANDIDATETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "AthenaKernel/SlotSpecificObj.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonTrackMakerUtils/MuonTrackMakerStlTools.h"
#include "TrkParameters/TrackParameters.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include <mutex>
#include <set>
#include <vector>

#include "MuPatHitTool.h" // Needed to enfornce build order for reflex dict
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonCompetingClustersOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonSegmentInfoExtender.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentSelectionTool.h"


class MsgStream;

namespace Trk {
  class Track;
  class MeasurementBase;
  class MagneticFieldProperties;
}

namespace MuonGM {
  class MuonReadoutElement;
}


namespace Muon {

  class MuonSegment;
  class MuPatTrack;
  class MuPatSegment;
  class MuPatCandidateBase;
  class MuonClusterOnTrack;

  static const InterfaceID IID_MuPatCandidateTool("Muon::MuPatCandidateTool",1,0);
  
  /** class to manipulate MuPatCandidateBase objects */
  class MuPatCandidateTool : public AthAlgTool {
  public:
    typedef std::vector<const Trk::MeasurementBase*> MeasVec;
    typedef MeasVec::iterator                        MeasIt;
    typedef MeasVec::const_iterator                  MeasCit;


  public:
    /** default AlgTool constructor */
    MuPatCandidateTool(const std::string&, const std::string&, const IInterface*);
    
    /** destructor */
    ~MuPatCandidateTool() = default;
    
    /** initialize method, method taken from bass-class AlgTool */
    StatusCode initialize();

    /** finialize method, method taken from bass-class AlgTool */
    StatusCode finalize();
    
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuPatCandidateTool; }
    
    /** @brief extend a track candidate with a segment
	@param[in] can   the MuPatTrack to be extended
	@param[in] track the new track, ownership is passed to the candidate
	@param[in] seg   the MuPatSegment to be added, ownership is NOT passed!
        @param[out] true if any segment was removed from the track, false if not
    */
    bool extendWithSegment( MuPatTrack& can, MuPatSegment& segInfo, std::unique_ptr<Trk::Track>& track ) const;

    /** @brief create a track candidate from one segment
	@param[in] seg1 the first MuPatSegment to be added, ownership is NOT passed!
	@param[in] seg2 the second MuPatSegment to be added, ownership is NOT passed!
	@param[in] track the new track, ownership is passed to the candidate
	@param[out] the new candidate, ownership is passed to caller
    */
    std::unique_ptr<MuPatTrack> createCandidate( MuPatSegment& segInfo, std::unique_ptr<Trk::Track>& track ) const;

    /** @brief create a track candidate from two segments
	@param[in] seg1 the first MuPatSegment to be added, ownership is NOT passed!
	@param[in] seg2 the second MuPatSegment to be added, ownership is NOT passed!
	@param[in] track the new track, ownership is passed to the candidate
	@param[out] the new candidate, ownership is passed to caller
    */
    std::unique_ptr<MuPatTrack> createCandidate( MuPatSegment& segInfo1, MuPatSegment& segInfo2, std::unique_ptr<Trk::Track>& track ) const;


    /** @brief create a track candidate from a track 
	@param[in] track the new track, ownership is passed to the candidate
	@param[out] the new candidate, ownership is passed to caller
    */
    std::unique_ptr<MuPatTrack> createCandidate( std::unique_ptr<Trk::Track>& track ) const;

    /** @brief set the new track in the candidate, and update candidate contents. Candidate takes ownership of track.
        Returns whether segments have been removed compared to the pre-existing list of segments. */
    bool updateTrack( MuPatTrack& candidate, std::unique_ptr<Trk::Track>& track ) const;
    
    /** @brief recalculate the chamber indices on the candidate and reset them. Return whether segment has been removed. */
    bool recalculateCandidateSegmentContent( MuPatTrack& candidate ) const;
    
    /** @brief copy a candidate without copying the track (lazy pointer copy)
	@param[in] can the MuPatTrack to be copied
	@param[out] the new candidate, ownership is passed to caller. The new candidate will not own the track (lazy pointer copy)
    */
    std::unique_ptr<MuPatTrack> copyCandidate( MuPatTrack* canIn ) const;

    /** @brief create a MuPatSegment object from a segment
	@param[in] segment  input segment
	@param[out] the MuPatSegment object, ownership is passed to caller
    */
    MuPatSegment* createSegInfo( const MuonSegment& segment ) const;

    /** @brief get list of the readout elements of the hits on the entry */
    std::set<const MuonGM::MuonReadoutElement*> readoutElements( const MuPatCandidateBase& entry ) const;

    /** @brief */
    void cleanUp() const;

    /** @brief print the measurements on the track to string
     *
     * This really belongs in MuonEDMPrinterTool in package MuonRecHelperTools. 
     * However, due to the tight tag collecting policy for release 15.5.0, this change 
     * is not likely to get accepted there.
     * To be moved there as soon as release 16 is open 
     */

    std::string print( const MuPatSegment& segment, int level = 0 ) const;

    std::string print( const std::vector<MuPatSegment*>& segments, int level = 0 ) const;

    std::string print( const MuPatCandidateBase& candidate, int level = 0 ) const;
  
    std::string print( const MuPatTrack& track, int level = 0 ) const;

    std::string print( const std::vector<std::unique_ptr<MuPatTrack> >& tracks, int level = 0 ) const;

  private:

    /** @brief update hits for a MuPatCandidateBase */
    void updateHits( MuPatCandidateBase& entry, const MeasVec& measurements, bool recreateMDT = false, 
		     bool recreateCSC = false, bool createComp = false ) const;

    /** @brief extract MuonClusterOnTrack('s) from measurement */
    void addCluster( const Trk::MeasurementBase& meas, std::vector<const MuonClusterOnTrack*>& rots ) const;

    /** @brief create CompetingMuonClustersOnTracks from ROTs and add them to the MeasVec. NEVER pass mixed eta/phi hits!! */
    void createAndAddCompetingROTs( const std::vector<const MuonClusterOnTrack*>& rots,
                                    MeasVec& hits,
                                    MeasVec& allHits,
                                    MeasVec& measurementsToBeDeleted ) const;
    
    ToolHandle<IMdtDriftCircleOnTrackCreator>         m_mdtRotCreator     
      {this, "MdtRotCreator", "Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator"};     //<! tool to calibrate MDT hits
    ToolHandle<IMuonClusterOnTrackCreator>            m_cscRotCreator     
      {this, "CscRotCreator", "Muon::CscClusterOnTrackCreator/CscClusterOnTrackCreator"};      //<! tool to calibrate CSC hits
    ToolHandle<IMuonCompetingClustersOnTrackCreator>  m_compClusterCreator 
      {this, "CompetingClustersCreator", "Muon::TriggerChamberClusterOnTrackCreator/TriggerChamberClusterOnTrackCreator"}; //<! tool to create competing clusters on track
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ServiceHandle<IMuonEDMHelperSvc>                  m_edmHelperSvc 
      {this, "edmHelper", 
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
      "Handle to the service providing the IMuonEDMHelperSvc interface" };         //<! multipurpose helper tool
    ToolHandle<MuonEDMPrinterTool>                    m_printer           
      {this, "MuonPrinterTool", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"}; //!< tool to print            //<! tool to print EDM objects
    ToolHandle<MuPatHitTool>                          m_hitHandler        
      {this, "HitTool", "Muon::MuPatHitTool/MuPatHitTool", "Tool to manipulate hit lists"};
    ToolHandle<Muon::IMuonSegmentSelectionTool>       m_segmentSelector   
      {this, "SegmentSelector", "Muon::MuonSegmentSelectionTool/MuonSegmentSelectionTool", "Tool to resolve track ambiguities"};

    Gaudi::Property<bool>  m_createCompetingROTsPhi {this,"CreateCompetingROTsPhi" , false };
    Gaudi::Property<bool>  m_createCompetingROTsEta {this,"CreateCompetingROTsEta" , true };
    Gaudi::Property<bool>  m_doMdtRecreation {this,"DoMdtRecreation" , false };
    Gaudi::Property<bool>  m_doCscRecreation {this,"DoCscRecreation" , true };

    // Mutex to protect the contents.
    mutable std::mutex m_mutex{};
    struct CacheEntry {
      EventContext::ContextEvt_t m_evt{EventContext::INVALID_CONTEXT_EVT};
      MeasVec m_measurementsToBeDeleted{}; //<! vector to store measurements owned by the track maker
      void cleanUp() { // Delete measurements to be deleted now
        std::for_each( m_measurementsToBeDeleted.begin(), m_measurementsToBeDeleted.end(), MuonDeleteObject<const Trk::MeasurementBase>() );
        m_measurementsToBeDeleted.clear();
      };      
      ~CacheEntry() { // Destructor deletes measurements to be deleted during finalization
        cleanUp();
      }
    };
    mutable SG::SlotSpecificObj<CacheEntry> m_cache ATLAS_THREAD_SAFE; // Guarded by m_mutex

  };

}


#endif
