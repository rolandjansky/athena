/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUPATCANDIDATETOOL_H
#define MUPATCANDIDATETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "TrkParameters/TrackParameters.h"
#include <vector>
#include <set>

#include "MuPatHitTool.h" // Needed to enfornce build order for reflex dict

class MsgStream;

class MdtIdHelper;
class IIncidentSvc;

namespace Trk {
  class Track;
  class MeasurementBase;
  class MagneticFieldProperties;
}

namespace MuonGM {
  class MuonReadoutElement;
}


namespace Muon {

  class IMdtDriftCircleOnTrackCreator;
  class IMuonClusterOnTrackCreator;
  class IMuonCompetingClustersOnTrackCreator;
  class IMuonSegmentSelectionTool;
  class IMuonSegmentInfoExtender;
  class MuonIdHelperTool;
  class MuonEDMHelperTool;
  class MuonEDMPrinterTool;
  class MuonSegment;
  class MuPatTrack;
  class MuPatSegment;
  class MuPatCandidateBase;
  class MuonClusterOnTrack;

  static const InterfaceID IID_MuPatCandidateTool("Muon::MuPatCandidateTool",1,0);
  
  /** class to manipulate MuPatCandidateBase objects */
  class MuPatCandidateTool : public AthAlgTool, virtual public IIncidentListener {
  public:
    typedef std::vector<const Trk::MeasurementBase*> MeasVec;
    typedef MeasVec::iterator                        MeasIt;
    typedef MeasVec::const_iterator                  MeasCit;


  public:
    /** default AlgTool constructor */
    MuPatCandidateTool(const std::string&, const std::string&, const IInterface*);
    
    /** destructor */
    ~MuPatCandidateTool();
    
    /** initialize method, method taken from bass-class AlgTool */
    StatusCode initialize();

    /** finialize method, method taken from bass-class AlgTool */
    StatusCode finalize();
    
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuPatCandidateTool; }
    
    /** @brief extend a track candidate with a segment
	@input can   the MuPatTrack to be extended
	@input track the new track, ownership is passed to the candidate
	@input seg   the MuPatSegment to be added, ownership is NOT passed!
        @output true if any segment was removed from the track, false if not
    */
    bool extendWithSegment( MuPatTrack& can, MuPatSegment& segInfo, const Trk::Track* track ) const;

    /** @brief create a track candidate from one segment
	@input seg1 the first MuPatSegment to be added, ownership is NOT passed!
	@input seg2 the second MuPatSegment to be added, ownership is NOT passed!
	@input track the new track, ownership is passed to the candidate
	@output the new candidate, ownership is passed to caller
    */
    MuPatTrack* createCandidate( MuPatSegment& segInfo, const Trk::Track* track ) const;

    /** @brief create a track candidate from two segments
	@input seg1 the first MuPatSegment to be added, ownership is NOT passed!
	@input seg2 the second MuPatSegment to be added, ownership is NOT passed!
	@input track the new track, ownership is passed to the candidate
	@output the new candidate, ownership is passed to caller
    */
    MuPatTrack* createCandidate( MuPatSegment& segInfo1, MuPatSegment& segInfo2, const Trk::Track* track ) const;


    /** @brief create a track candidate from a track 
	@input track the new track, ownership is passed to the candidate
	@output the new candidate, ownership is passed to caller
    */
    MuPatTrack* createCandidate( const Trk::Track* track ) const;

    /** @brief set the new track in the candidate, and update candidate contents. Candidate takes ownership of track.
        Returns whether segments have been removed compared to the pre-existing list of segments. */
    bool updateTrack( MuPatTrack& candidate, const Trk::Track* track ) const;
    
    /** @brief recalculate the chamber indices on the candidate and reset them. Return whether segment has been removed. */
    bool recalculateCandidateSegmentContent( MuPatTrack& candidate ) const;
    
    /** @brief copy a candidate without copying the track (lazy pointer copy)
	@input can the MuPatTrack to be copied
	@output the new candidate, ownership is passed to caller. The new candidate will not own the track (lazy pointer copy)
    */
    MuPatTrack* copyCandidate( MuPatTrack& canIn ) const;

    /** @brief copy a candidate and transfer the track ownwership from the old to the new candidate.
	@input can the MuPatTrack to be copied. After the copy, it will no longer own the track (but keeps pointer to it)
	@output the new candidate, ownership is passed to caller. The new candidate will own the track.
    */
    MuPatTrack* copyCandidateAndTransferTrack( MuPatTrack& canIn ) const;

    /** @brief create a MuPatSegment object from a segment
	@input segment  input segment
	@output the MuPatSegment object, ownership is passed to caller
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

    std::string print( const std::vector<MuPatTrack*>& tracks, int level = 0 ) const;

    /**  incident service handle for EndEvent */
    void handle(const Incident& inc);// maybe in the future clear per event
    
  private:

    /** @brief update hits for a MuPatCandidateBase */
    void updateHits( MuPatCandidateBase& entry, const MeasVec& measurements, bool recreateMDT = false, 
		     bool recreateCSC = false, bool createComp = false ) const;

    /** @brief extract MuonClusterOnTrack('s) from measurement */
    void addCluster( const Trk::MeasurementBase& meas, std::vector<const MuonClusterOnTrack*>& rots ) const;

    /** @brief create CompetingMuonClustersOnTracks from ROTs and add them to the MeasVec. NEVER pass mixed eta/phi hits!! */
    void createAndAddCompetingROTs( const std::vector<const MuonClusterOnTrack*>& rots, MeasVec& hits, MeasVec& allHits ) const;
    
    ToolHandle<IMdtDriftCircleOnTrackCreator>         m_mdtRotCreator;      //<! tool to calibrate MDT hits
    ToolHandle<IMuonClusterOnTrackCreator>            m_cscRotCreator;      //<! tool to calibrate CSC hits
    ToolHandle<IMuonCompetingClustersOnTrackCreator>  m_compClusterCreator; //<! tool to create competing clusters on track
    ToolHandle<MuonIdHelperTool>                      m_idHelperTool;       //<! tool to assist with Identifiers
    ToolHandle<MuonEDMHelperTool>                     m_helperTool;         //<! multipurpose helper tool
    ToolHandle<MuonEDMPrinterTool>                    m_printer;            //<! tool to print EDM objects
    ToolHandle<MuPatHitTool>                          m_hitHandler;         //<! tool to manipulate hit lists
    ToolHandle<Muon::IMuonSegmentSelectionTool>       m_segmentSelector;    //<! tool to resolve track ambiguities
    ToolHandle<Muon::IMuonSegmentInfoExtender>        m_segmentExtender;    //<! tool to extend the segment information
    ServiceHandle< IIncidentSvc >                     m_incidentSvc;

    //const MdtIdHelper* m_mdtIdHelper;

    bool m_createCompetingROTsPhi;
    bool m_createCompetingROTsEta;
    bool m_doMdtRecreation;
    bool m_doCscRecreation;

    mutable MeasVec m_measurementsToBeDelete;   //<! vector to store measurements owned by the track maker

  };

}


#endif
