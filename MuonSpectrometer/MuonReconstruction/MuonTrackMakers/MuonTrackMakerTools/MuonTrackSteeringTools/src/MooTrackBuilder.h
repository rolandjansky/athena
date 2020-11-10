/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MOOTRACKBUILDER_H
#define MUON_MOOTRACKBUILDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Tools & interfaces
#include "MuonRecToolInterfaces/IMuonSegmentTrackBuilder.h"
#include "MuonRecToolInterfaces/IMuonTrackBuilder.h"
#include "MuonRecToolInterfaces/IMuonTrackRefiner.h"
#include "MuonRecToolInterfaces/IMuonTrackToSegmentTool.h"
#include "MuonRecToolInterfaces/IMuonSeededSegmentFinder.h"
#include "MuonRecToolInterfaces/IMuonHoleRecoveryTool.h"
#include "MuonRecToolInterfaces/IMuonTrackExtrapolationTool.h"
#include "MuonRecToolInterfaces/IMuonErrorOptimisationTool.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecToolInterfaces/IMuonCompetingClustersOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
// For magneticfield
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "TrkTrackSummary/MuonTrackSummary.h"


// Tracking EDM
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkSegment/SegmentCollection.h"

// Local
#include "MooTrackFitter.h"
#include "MooCandidateMatchingTool.h"
#include "MuPatCandidateTool.h"

// Misc
#include <vector>

class MsgStream;

namespace Trk {
  class Track;
  class PrepRawData;
  class IExtendedTrackSummaryTool;
}

namespace Muon {
  class MuonSegment;
  class MuPatCandidateBase;
  class MuPatTrack;
  class MuPatSegment;
}

static const InterfaceID IID_MooTrackBuilder("Muon::MooTrackBuilder",1,0);

namespace Muon {

  /**
      @class MooTrackBuilder

      Tool to combine two segments or a track and a segment to a track.
      In addition it offers the possibility to combine two segments to one larger segment.

      The tool can either be configured to perform a straight line association or an association in the magnetic field.

      The following steps are performed:
      - simple match of the input using the MuonSegmentTrackMatcher
      - if match successfull, fit of the input using the MuonSegmentTrackFitter
      - if a track was found, cleaning of the track using the MuonTrackCleaner
      - final decision whether to keep track or not

      For more details look at the mainpage of this package.
  */
  class MooTrackBuilder :  virtual public IMuonSegmentTrackBuilder, public IMuonTrackBuilder, public IMuonTrackRefiner, public AthAlgTool   {
  public:
    typedef IMuonSegmentTrackBuilder::PrepVec    PrepVec;
    typedef PrepVec::iterator                    PrepIt;
    typedef PrepVec::const_iterator              PrepCit;
  public:
    /** @brief default AlgTool constructor */
    MooTrackBuilder(const std::string&, const std::string&, const IInterface*);

    /** @brief destructor */
    ~MooTrackBuilder() = default;

    /** @brief initialize method, method taken from bass-class AlgTool */
    virtual StatusCode initialize() override;

    /** @brief finialize method, method taken from bass-class AlgTool */
    virtual StatusCode finalize() override;

    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MooTrackBuilder; }

    /** @brief refit track
        @param track the track
        @return a pointer to the resulting track, will return zero if combination failed. Ownership passed to user.
    */
    virtual std::unique_ptr<Trk::Track> refit( Trk::Track& track ) const override;

    /** @brief combine two MCTBCandidateEntries
        @param firstEntry  the first entry
        @param secondEntry the second entry
        @param externalPhiHits if provided, the external phi hits will be used instead of the phi hits on the segment
        @return a pointer to the resulting track, will return zero if combination failed. Ownership passed to user.
    */
    std::unique_ptr<Trk::Track> combine( const MuPatCandidateBase& firstEntry, const MuPatCandidateBase& secondEntry,
                         const PrepVec* patternPhiHits = 0 ) const;

    /** @brief combine two MCTBCandidateEntries
        @param firstEntry  the first entry
        @param secondEntry the second entry
        @param externalPhiHits if provided, the external phi hits will be used instead of the phi hits on the segment
        @return a pointer to the resulting track, will return zero if combination failed. Ownership passed to user.
    */
    MuonSegment* combineToSegment( const MuPatCandidateBase& firstEntry, const MuPatCandidateBase& secondEntry,
                                   const PrepVec* patternPhiHits = 0 ) const;

    /** @brief combine two segments to a super segment
        @param seg1 the first segment
        @param seg2 the second segment
        @param externalPhiHits if provided, the external phi hits will be used instead of the phi hits on the segment
        @return a pointer to the combined segment, will return zero if combination failed. Ownership passed to user.
    */
    virtual MuonSegment* combineToSegment( const MuonSegment& seg1, const MuonSegment& seg2,
                                   const PrepVec* patternPhiHits = 0 ) const override;

    /** @brief combine two segments to a track
        @param seg1 the first segment
        @param seg2 the second segment
        @param externalPhiHits if provided, the external phi hits will be used instead of the phi hits on the segment
        @return a pointer to the resulting track, will return zero if combination failed. Ownership passed to user.
    */
    virtual std::unique_ptr<Trk::Track> combine( const MuonSegment& seg1, const MuonSegment& seg2,
                         const PrepVec* patternPhiHits = 0 ) const override;

    /** @brief combine a track with a segment
        @param track a track
        @param seg a segment
        @param externalPhiHits if provided, the external phi hits will be used instead of the phi hits on the segment
        @return a pointer to the resulting track, will return zero if combination failed. Ownership passed to user.
    */
    virtual std::unique_ptr<Trk::Track> combine( const Trk::Track& track, const MuonSegment& seg,
						 const PrepVec* patternPhiHits = 0 ) const override;

    /** @brief find tracks by redoing the segment finding in the chamber of the segment
        @param track a reference to a Track
        @param seg a reference to a MuonSegment
        @return a pointer to vector of tracks, the ownership of the vector and the tracks is passed to the client calling the tool.
     */
    std::vector<std::unique_ptr<Trk::Track> > combineWithSegmentFinding( const Trk::Track& track, const MuonSegment& seg,
									 const PrepVec* patternPhiHits = 0 ) const;

    /** @brief find tracks by redoing the segment finding in the chamber of the segment
        @param candidate a reference to a MuPatTrack
        @param segInfo a reference to a MuPatSegment
        @return a pointer to vector of tracks, the ownership of the vector and the tracks is passed to the client calling the tool.
     */
    std::vector<std::unique_ptr<Trk::Track> > combineWithSegmentFinding( const MuPatTrack& candidate,
									 const MuPatSegment& segInfo,
									 const PrepVec* patternPhiHits = 0 ) const;


    /** @brief find tracks by redoing the segment finding in the chamber of the segment
        @param candidate a reference to a MuPatTrack
        @param pars predicted track parameters in first chamber
        @param chIds identifiers of the chambers in which the search should be performed (should be in same station layer)
        @return a pointer to vector of tracks, the ownership of the vector and the tracks is passed to the client calling the tool.
     */
    std::vector<std::unique_ptr<Trk::Track> > combineWithSegmentFinding( const MuPatTrack& candidate,
									 const Trk::TrackParameters& pars,
									 const std::set<Identifier>& chIds,
									 const PrepVec* patternPhiHits = 0 ) const; 

    /** @brief find tracks by redoing the segment finding in the chamber of the segment
        @param track a reference to a Track
        @param pars predicted track parameters in first chamber
        @param chIds identifiers of the chambers in which the search should be performed (should be in same station layer)
        @return a pointer to vector of tracks, the ownership of the vector and the tracks is passed to the client calling the tool.
     */
    std::vector<std::unique_ptr<Trk::Track> > combineWithSegmentFinding( const Trk::Track& track,
									 const Trk::TrackParameters& pars,
									 const std::set<Identifier>& chIds,
									 const PrepVec* patternPhiHits = 0 ) const;

    /** @brief find closest TrackParameters to the position. Closest is defined as closest in z in the endcap and
        closest in r in the barrel.
        @param track a reference to a Track
        @param pos a reference to a GlobalPosition
        @return a pointer to TrackParameters, the ownership of the parameters is passed to the client calling the tool.
     */
    virtual Trk::TrackParameters* findClosestParameters( const Trk::Track& track, const Amg::Vector3D& pos ) const override;


    /** @brief find closest TrackParameters to the surface. The distance is calculated along the track
        @param track a reference to a Track
        @param pos a reference to a Surface
        @return a pointer to TrackParameters, the ownership of the parameters is passed to the client calling the tool.
     */
    virtual Trk::TrackParameters* getClosestParameters( const Trk::Track& track, const Trk::Surface& surf ) const override;

    /** @brief find closest TrackParameters to the surface. The distance is calculated along the track
        @param track a reference to a MuPatCandidateBase
        @param pos a reference to a Surface
        @return a pointer to TrackParameters, the ownership of the parameters is passed to the client calling the tool.
     */
    Trk::TrackParameters* getClosestParameters( const MuPatCandidateBase& candidate, const Trk::Surface& surf ) const;


    /** recalibrate hits on track */
    virtual std::unique_ptr<Trk::Track> recalibrateHitsOnTrack( const Trk::Track& track, bool doMdts, bool doCompetingClusters  ) const override;

    /** split given track if it crosses the calorimeter volume, code assumes that the track was already extrapolated to the
        muon entry record using the MuonTrackExtrapolationTool. It uses the double perigee to spot the tracks to be split.
    */
    std::pair<std::unique_ptr<Trk::Track>,std::unique_ptr<Trk::Track> > splitTrack( const Trk::Track& track ) const;

    /** @brief identify whether two track are split */
    bool isSplitTrack( const Trk::Track& track1, const Trk::Track& track2 ) const;

    /** @brief look for split tracks in collection and merge them */
    TrackCollection* mergeSplitTracks( const TrackCollection& tracks ) const;

    /**
        @brief interface for tools to find track in the muon system starting from a vector of segments
        @param segments a vector of input segments in a given chamber layer
        @return a pointer to a vector of MuPatTrack objects, zero if no tracks are found.
                The ownership of the tracks is passed to the client calling the tool.

    */
    virtual std::vector<std::unique_ptr<MuPatTrack> > find( MuPatCandidateBase& candidate, const std::vector<MuPatSegment*>& segments ) const override;

    /** @brief interface for tools which refine the hit content of a given track
        @param track input track
        @return new refined track. Pointer could be zero, ownership passed to caller
    */
    virtual void refine( MuPatTrack& track ) const override;

    void cleanUp() const;


  private:

    void removeDuplicateWithReference( std::unique_ptr<Trk::SegmentCollection>& segments,
                                       std::vector<const MuonSegment*>& referenceSegments ) const;



    DataVector<const Trk::TrackStateOnSurface>::const_iterator
    insertClustersWithCompetingRotCreation( DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit,
                                            DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end,
                                            std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& states
                                            ) const;


    ToolHandle<MooTrackFitter>                        m_fitter              {this, "Fitter", "Muon::MooTrackFitter/MooTrackFitter", "Tool to fit segments to tracks"};
    ToolHandle<MooTrackFitter>                        m_slFitter            {this, "SLFitter", "Muon::MooTrackFitter/MooSLTrackFitter", "Tool to fit segments to tracks"};
    ToolHandle<MuPatCandidateTool>                    m_candidateHandler    {this, "CandidateTool", "Muon::MuPatCandidateTool/MuPatCandidateTool"}; //!< candidate handler
    ToolHandle<MooCandidateMatchingTool>              m_candidateMatchingTool {this, "CandidateMatchingTool", "Muon::MooCandidateMatchingTool/MooCandidateMatchingTool"};
    ToolHandle<IMuonTrackToSegmentTool>               m_trackToSegmentTool  {this, "TrackToSegmentTool", "Muon::MuonTrackToSegmentTool/MuonTrackToSegmentTool"};
    ServiceHandle<IMuonEDMHelperSvc>                  m_edmHelperSvc        {this, "edmHelper",
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
      "Handle to the service providing the IMuonEDMHelperSvc interface" };
    ToolHandle<MuonEDMPrinterTool>                    m_printer             {this, "Printer", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};//!< tool to print out EDM objects;
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ToolHandle<IMuonSeededSegmentFinder>              m_seededSegmentFinder {this, "SeededSegmentFinder", "Muon::MuonSeededSegmentFinder/MuonSeededSegmentFinder"};
    ToolHandle<IMdtDriftCircleOnTrackCreator>         m_mdtRotCreator       {this, "MdtRotCreator", "Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator"};
    ToolHandle<IMuonCompetingClustersOnTrackCreator>  m_compRotCreator      {this, "CompetingClustersCreator", "Muon::TriggerChamberClusterOnTrackCreator/TriggerChamberClusterOnTrackCreator"};
    ToolHandle<Trk::IPropagator>                      m_propagator          {this, "Propagator", "Trk::STEP_Propagator/MuonPropagator"};
    ToolHandle<Trk::IResidualPullCalculator>          m_pullCalculator      {this, "PullCalculator", "Trk::ResidualPullCalculator/ResidualPullCalculator"};
    ToolHandle<IMuonHoleRecoveryTool>                 m_hitRecoverTool      {this, "HitRecoveryTool", "Muon::MuonChamberHoleRecoveryTool/MuonChamberHoleRecoveryTool"};    //<! tool to recover holes on track
    ToolHandle<IMuonHoleRecoveryTool>                 m_muonChamberHoleRecoverTool {this, "ChamberHoleRecoveryTool", "Muon::MuonChamberHoleRecoveryTool/MuonChamberHoleRecoveryTool"};    //<! tool to add holes on track
    ToolHandle<IMuonTrackExtrapolationTool>           m_trackExtrapolationTool  {this, "Extrapolator", "Muon::MuonTrackExtrapolationTool/MuonTrackExtrapolationTool"}; //<! track extrapolation tool

    ToolHandle<IMuonErrorOptimisationTool>            m_errorOptimisationTool {this, "ErrorOptimisationTool", ""};
    ToolHandle<Trk::IExtendedTrackSummaryTool>        m_trackSummaryTool    {this, "TrackSummaryTool", "MuonTrackSummaryTool"};
    // Read handle for conditions object to get the field cache
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj",
                                                                               "Name of the Magnetic Field conditions object key"};
    Trk::MagneticFieldProperties                      m_magFieldProperties  {Trk::FullField}; //!< magnetic field properties

    Gaudi::Property<bool>                             m_doTimeOutChecks     {this,"UseTimeOutGuard" , true };    //!< on/off time out check
    Gaudi::Property<bool>                             m_useExclusionList    {this, "UseExclusionList" , true };   //!< use exclusion list (bit faster at the price of missing chambers)
    Gaudi::Property<bool>                             m_useTrackingHistory  {this, "UseTrackingHistory" , true }; //!< use history of the track finding up to now to avoid creating duplicates
    Gaudi::Property<bool>                             m_recalibrateMDTHits  {this, "RecalibrateMDTHitsOnTrack" , true };
    mutable std::atomic_uint                          m_ncalls {0};
    mutable std::atomic_uint                          m_nTimedOut {0};

  };


}


#endif
