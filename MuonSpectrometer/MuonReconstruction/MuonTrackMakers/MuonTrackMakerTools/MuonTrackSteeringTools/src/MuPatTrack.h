/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUPATTRACK_H
#define MUPATTRACK_H

#include "MuPatSegment.h"
#include "MuPatCandidateBase.h"
#include "MuonStationIndex/MuonStationIndex.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"

#include <mutex>
#include <set>
#include <string>
#include <vector>

namespace Muon {

  /**
      @brief track candidate object.
      The purpose of the track candidate is three folded:
      - provide the generic MuPatCandidateBase interface for tracks
      - keep track of segments used to build track
      - cache additional information that cannot be stored on the track

      The following information is cached:
      - the stage during the pattern recognition the candidate was made (Moore legacy)
      - the pointer to the track
      - a vector of the segments that were used to form the track
      - a vector of the segments that were already tested against the candidate but failed.
        This information can be used to speed up the pat-rec code by avoiding fits that
        were already tried.
   */
  class MuPatTrack : public MuPatCandidateBase {

    friend class MuPatCandidateTool;

  public:
    /** enum to keep track of the life of candidates */
    enum ProcessingStage { Unknown,
                           InitialLoop,
                           LayerRecovery,
                           ExtendedWithSegment,
                           SegmentRecovery,
                           FitRemovedSegment,
                           RefitRemovedSegment,
                           AmbiguityCreateCandidateFromSeeds,
                           AmbiguitySelectCandidates,
                           MatchFail,
                           FitFail,
                           FitWorse,
                           UnassociatedEM,
                           FitRemovedLayer,
                           TrackSelector,
                           KeptUntilEndOfCombi,
                           NumberOfProcessingStages
    };

    /** Convert enum to string */
    static const std::string& processingStageString( ProcessingStage stage );

    /** maximum width of the strings corresponding to the ProcessingStage */
    static unsigned int processingStageStringMaxLen();

    /** @brief constructor taking a vector of MuPatSegment object, the candidate takes ownership of the track
               It will increase the usedInFit counter of the MuPatSegment objects by one. */
    MuPatTrack( const std::vector<MuPatSegment*>& segments, std::unique_ptr<Trk::Track>& track , MuPatSegment* seedSeg=0 );

    /** @brief constructor taking a MuPatSegment object, the candidate takes ownership of the track
               It will increase the usedInFit counter of the MuPatSegment objects by one. */
    MuPatTrack( MuPatSegment* segment, std::unique_ptr<Trk::Track>& track );

    /** @brief constructor taking two MuPatSegment objects, the candidate takes ownership of the track
         It will increase the usedInFit counter of the MuPatSegment objects by one. */
    MuPatTrack( MuPatSegment* segment1, MuPatSegment* segment2, std::unique_ptr<Trk::Track>& track , MuPatSegment* seedSeg=0 );

    /** @brief destructor, decrease the usedInFit counter of all MuPatSegment objects by one */
    ~MuPatTrack();

    /** copying constructor. It will not copy the track, just its pointer (lazy). It will increase the usedInFit counter of the MuPatSegment objects by one. */
    MuPatTrack( const MuPatTrack& can );

    /** assignment operator. It will not copy the track, just its pointer (lazy). It will increase the usedInFit counter of the MuPatSegment objects by one. */
    MuPatTrack& operator=( const MuPatTrack& can );

    /** @brief access to track */
    Trk::Track& track() const;

    /** @brief access to segments */
    const std::vector<MuPatSegment*>& segments() const;

    /** @brief Return pointer to the seed segment */
    MuPatSegment* seedSegment() const;

    /** @brief access to segments */
    const std::vector<MuPatSegment*>& excludedSegments() const;

    /** @brief add segment + the associated new track. Takes ownership of the track. */
    void addSegment( MuPatSegment* segment, std::unique_ptr<Trk::Track>& newTrack );

    /** @brief add segment that does not match the track */
    void addExcludedSegment( MuPatSegment* segment );

    /** @brief returns first track parameters */
    const Trk::TrackParameters& entryPars() const;

    /** @brief returns whether canditate has a momentum measurement */
    bool hasMomentum() const;

    /** @brief returns vector with contained stationIndices in the order they were added */
    std::vector<MuonStationIndex::StIndex> stationsInOrder();

    /** @brief reset chambers on the candidate. Return whether segments were removed. */
    bool resetChambersOnCandidate( const std::set<MuonStationIndex::ChIndex>& chambers );

    /** @brief string containing the names of the segments on the candidate */
    std::string segmentNames() const;

    /** @brief reset the maximum number of objects counter */
    static void resetMaxNumberOfInstantiations();
    /** @brief maximum number of objects of this type in memory */
    static unsigned int maxNumberOfInstantiations();
    /** current number of objects of this type in memory */
    static unsigned int numberOfInstantiations();
    /** @brief reset the copy constructor counter */
    static void resetNumberOfCopies();
    /** @brief number of times the copy constructor was called since last reset */
    static unsigned int numberOfCopies();

  private:
    //
    // private static functions
    //
    /** @brief Keeping track of number of object instances */
    static void addInstance();

    /** @brief Keeping track of number of object instances */
    static void removeInstance();

    /** @brief Initialize s_processingStageStrings & s_processingStageStringMaxLen */
    static void initProcessingStageStrings();

    //
    // private member functions
    //
    /** @brief update track. Candidate takes ownership of track. */
    void updateTrack( std::unique_ptr<Trk::Track>& newTrack );

    /** @brief update segment/track association, if add == true ,will add track to segments else remove it */
    void updateSegments( bool add );

  public:
    //
    // public data members
    //
    ProcessingStage created;
    ProcessingStage lastSegmentChange;

  private:
    //
    // private static data members
    //
    /** current number of objects of this type in memory */
    static unsigned int s_numberOfInstantiations ATLAS_THREAD_SAFE;
    /** maximum number of objects of this type in memory */
    static unsigned int s_maxNumberOfInstantiations ATLAS_THREAD_SAFE;
    /** number of copies made */
    static unsigned int s_numberOfCopies ATLAS_THREAD_SAFE;
    /** maximum width of the strings corresponding to the ProcessingStage */
    static unsigned int s_processingStageStringMaxLen ATLAS_THREAD_SAFE;

    static std::mutex s_mutex; //<! to remove race condition in addInstance

    static std::once_flag s_stageStringsInitFlag;
    static std::vector<std::string> s_processingStageStrings ATLAS_THREAD_SAFE;

    /** @brief increase the segment counters by the passed number */
    void modifySegmentCounters( int change );

    /** @brief check whether track measures momentum */
    bool hasMomentum( const Trk::Track& track ) const;


    std::vector<MuPatSegment*>  m_segments; //<! list of associated segments
    std::vector<MuPatSegment*>  m_excludedSegments; //<! list of associated segments
    std::unique_ptr<Trk::Track> m_track;    //<! associated track
    MuPatSegment*              m_seedSeg; //!< The special segment for this track

  public:
    /** @brief Mboy data members  */
    struct MboyInfo {
      //m_MboyStatus:
      //-1: unknown
      // 0: never reconstructed
      // 1: produced as primary fit
      // 2: produced as secondary fit
      // 3: produced as main fit
      int    m_MboyStatus ;

      int    m_ISC0 ;

      double m_DZT1 ;
      double m_DS1  ;
      double m_DZT2 ;
      double m_DS2  ;

      double m_SMU  ;

      double m_CFI  ;
      double m_SFI  ;
      double m_CFA  ;
      double m_SFA  ;

      double m_Z1   ;
      double m_T1   ;
      double m_S1   ;

      double m_Z2   ;
      double m_T2   ;
      double m_S2   ;

      int m_ndof ;
      double m_chi2 ;
    };
    MboyInfo * mboyInfo;

  }; // class MuPatTrack


  //
  // static inline functions implementations
  //
  inline unsigned int MuPatTrack::numberOfInstantiations() {
    const std::lock_guard<std::mutex> lock(s_mutex);

    return s_numberOfInstantiations;
  }

  inline unsigned int MuPatTrack::maxNumberOfInstantiations() {
    const std::lock_guard<std::mutex> lock(s_mutex);

    return s_maxNumberOfInstantiations;
  }

  inline unsigned int MuPatTrack::numberOfCopies() {
    const std::lock_guard<std::mutex> lock(s_mutex);

    return s_numberOfCopies;
  }

  inline void MuPatTrack::addInstance() {
    const std::lock_guard<std::mutex> lock(s_mutex);

    ++s_numberOfInstantiations;
    if ( s_numberOfInstantiations > s_maxNumberOfInstantiations ) {
      s_maxNumberOfInstantiations = static_cast<unsigned int>(s_numberOfInstantiations);
    }
  }

  inline void MuPatTrack::removeInstance() {
    const std::lock_guard<std::mutex> lock(s_mutex);

    if ( s_numberOfInstantiations > 0 ) --s_numberOfInstantiations;
  }

  inline void MuPatTrack::resetMaxNumberOfInstantiations() {
    const std::lock_guard<std::mutex> lock(s_mutex);

    s_maxNumberOfInstantiations = 0;
  }

  inline void MuPatTrack::resetNumberOfCopies() {
    const std::lock_guard<std::mutex> lock(s_mutex);

    s_numberOfCopies = 0;
  }


  //
  // inline member functions implementations
  //
  inline const std::vector<MuPatSegment*>& MuPatTrack::segments() const {
    return m_segments;
  }

  inline MuPatSegment* MuPatTrack::seedSegment() const {
    return m_seedSeg; // could be a null pointer - do not dereference immediately!!
  }

  inline const std::vector<MuPatSegment*>& MuPatTrack::excludedSegments() const {
    return m_excludedSegments;
  }

  inline Trk::Track& MuPatTrack::track() const {
    return *m_track;
  }

  inline const Trk::TrackParameters& MuPatTrack::entryPars() const {
    const Trk::Perigee* pp = m_track->perigeeParameters();
    if( pp ){
      return *pp;
    }
    assert( !m_track->trackParameters() || m_track->trackParameters()->empty() );
    return *m_track->trackParameters()->front();
  }


  class SortMuPatTrackByQuality {
  public:
    bool operator()( const MuPatTrack* c1, const MuPatTrack* c2 ){

      // prefer candidates with more segments
      if( c1->segments().size() > c2->segments().size() ) return true;
      if( c1->segments().size() < c2->segments().size() ) return false;

      // prefer tracks with fit quality (always expected)
      const Trk::FitQuality* fq1 = c1->track().fitQuality();
      const Trk::FitQuality* fq2 = c2->track().fitQuality();
      if( !fq1 && fq2 ) return false;
      if( fq1  && !fq2 ) return true;

      if( fq1->numberDoF() > fq2->numberDoF() ) return true;
      if( fq1->numberDoF() < fq2->numberDoF() ) return false;

      // select candidate with smallest chi2
      double chi2Ndof1 = fq1->chiSquared()/fq1->numberDoF();
      double chi2Ndof2 = fq2->chiSquared()/fq2->numberDoF();
      return chi2Ndof1 < chi2Ndof2;
    }
  };

} // Muon

#endif
