/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MOOTRACKFITTER_H
#define MUON_MOOTRACKFITTER_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkParameters/TrackParameters.h"

#include "TrkTrack/TrackInfo.h"
#include "TrkMeasurementBase/MeasurementBase.h"

#include "TrkFitterInterfaces/ITrackFitter.h"
#include "MuonIdHelpers/MuonStationIndex.h"

#include "MuPatCandidateBase.h"
#include "MuPatHit.h"

#include "TrkDriftCircleMath/SegmentFinder.h"
#include "TrkDriftCircleMath/DCSLFitter.h"
#include "Identifier/Identifier.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"

#include <set>

class MsgStream;

class RpcIdHelper;
class MdtIdHelper;
class CscIdHelper;
class TgcIdHelper;
class Identifier;

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Trk {
  class RIO_OnTrack;
  class PrepRawData;
  class IPropagator;
  class Track;
  class MeasurementBase;
  class PseudoMeasurementOnTrack;
  class Layer;
  class ITrackSummaryHelperTool;
}

namespace Muon {
  class MuonSegment;
  class MuonEDMPrinterTool;
  class MuonIdHelperTool;
  class IMuonTrackCleaner;
  class MuPatHitTool;
  class IMuonSegmentMomentumEstimator;
  class IMuonTrackToSegmentTool;
  class IMdtDriftCircleOnTrackCreator;
  class IMuonSegmentInOverlapResolvingTool;
  class IMuonHitSelector;
  class MuPatCandidateBase;
  class MuPatTrack;
}

static const InterfaceID IID_MooTrackFitter("Muon::MooTrackFitter",1,0);

namespace Muon {

  /** 
      Tool to fit a trajectory using a set of two segments or a track and a segment as input. 
      The tool can be configured to fit either a straight line or all 5 parameters of the track.

      For more details look at the mainpage of this package.
  */
  class MooTrackFitter : public AthAlgTool   {
  public:

    typedef std::vector<const Trk::MeasurementBase*> MeasVec;
    typedef MeasVec::iterator                        MeasIt;
    typedef MeasVec::const_iterator                  MeasCit;

    typedef std::vector<const Trk::PrepRawData*>     PrepVec;
    typedef PrepVec::iterator                        PrepIt;
    typedef PrepVec::const_iterator                  PrepCit;

    typedef std::pair<int,int> SmallLargeChambers;
    typedef std::map<MuonStationIndex::StIndex,SmallLargeChambers> SLStationMap;

    typedef std::vector< std::pair<const Trk::TrackParameters*,const Trk::Layer*> > MaterialLayers;

    struct FitterData {
      FitterData() : nOverlaps(-1),nSmall(-1),nLarge(-1),
		     avePhi(0),phiMin(0),phiMax(0),
		     hasBarrel(false),hasEndcap(false),firstEntry(0),secondEntry(0),startPars(0),
                     firstIsTrack(false),secondIsTrack(false),firstHasMomentum(false),secondHasMomentum(false) {}
      MeasVec phiHits;
      MeasVec etaHits;
      MeasVec measurements;
      MeasVec firstLastMeasurements;

      int nOverlaps;
      int nSmall;
      int nLarge;
      SLStationMap smallLargeChambersPerStation;

      double avePhi;
      double phiMin;
      double phiMax;

      bool hasBarrel;
      bool hasEndcap;
      const MuPatCandidateBase* firstEntry;
      const MuPatCandidateBase* secondEntry;
      std::set<MuonStationIndex::StIndex> stations;
      MuPatHitList hitList;

      MuPatHitList  copyHitList1;
      MuPatHitList  copyHitList2;

      Trk::Perigee* startPars;

      bool firstIsTrack;
      bool secondIsTrack;
      bool firstHasMomentum;
      bool secondHasMomentum;
      std::set<Identifier> mdtIdsFirst;
      std::set<Identifier> mdtIdsSecond;

      int numberOfSLOverlaps() { 
        // check if already initialized
        if( nOverlaps == -1 ){
          nOverlaps = 0;
          nSmall = 0;
          nLarge = 0;
          // loop over SLStationMap and count the number of times there is a station with both S and L 
          SLStationMap::iterator it = smallLargeChambersPerStation.begin();
          SLStationMap::iterator it_end = smallLargeChambersPerStation.end();
          for( ;it!=it_end;++it ){
            if( it->second.first )  ++nSmall;
            if( it->second.second ) ++nLarge;
            if( it->second.first && it->second.second ) ++nOverlaps;
          }
        }
        return nOverlaps;
      }
      int numberOfSmallChambers() {
        if( nSmall < 0 ) numberOfSLOverlaps();
        return nSmall;
      }
      int numberOfLargeChambers() {
        if( nLarge < 0 ) numberOfSLOverlaps();
        return nLarge;
      }

      MaterialLayers materialLayers;
    };

    struct GarbageCan{

      MeasVec measurementsToBeDeleted; //<! vector of measurements created during fitting that should be deleted
      std::vector<const Trk::TrackParameters*> parametersToBeDeleted; //<! vector of parameters created during fitting that should be deleted
      std::vector<const MuPatHit*> mctbHitsToBeDeleted;

      /** clean up memory managed by tool */
      void cleanUp() {
	for( const auto meas : measurementsToBeDeleted ) delete meas;
	measurementsToBeDeleted.clear();

	for(const auto par : parametersToBeDeleted) delete par;
	parametersToBeDeleted.clear();

	for(const auto hit : mctbHitsToBeDeleted) delete hit;
	mctbHitsToBeDeleted.clear();
      }
    };
    
  public:
    /** default AlgTool constructor */
    MooTrackFitter(const std::string&, const std::string&, const IInterface*);
    
    /** destructor */
    ~MooTrackFitter();
    
    /** initialize method, method taken from bass-class AlgTool */
    StatusCode initialize();

    /** finialize method, method taken from bass-class AlgTool */
    StatusCode finalize();
    
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MooTrackFitter; }

    /** @brief fit the hits of two MuPatCandidateBase
        @param firstEntry  the first entry
        @param secondEntry the second entry
        @param externalPhiHits if provided, the external phi hits will be used instead of the phi hits on the segment
        @return a pointer to the resulting track, will return zero if the fit failed.
    */
    Trk::Track* fit( const MuPatCandidateBase& firstEntry, const MuPatCandidateBase& secondEntry, const PrepVec* externalPhiHits ) const; 

    /** refit a MuPatTrack */
    Trk::Track* refit( const MuPatTrack& trkCan ) const;

    /** refit a track */
    Trk::Track* refit( const Trk::Track& track ) const;

    /** impose upper and lower bound on momentum */
    double restrictedMomentum( double momentum ) const;

    /** create perigee parameter to initialize fit */
    Trk::Perigee* createPerigee( const Trk::TrackParameters& firstPars, const Trk::MeasurementBase& firstMeas ) const;

    /** fit track */
    Trk::Track* fit( const Trk::Perigee& startPars, MeasVec& hits, GarbageCan& garbage, Trk::ParticleHypothesis partHypo = Trk::muon, bool prefit = false ) const;

    /** fit track, refit if needed */
    Trk::Track* fitWithRefit( const Trk::Perigee& startPars, MeasVec& hits ) const;

    /** split given track if it crosses the calorimeter volume, code assumes that the track was already extrapolated to the
        muon entry record using the MuonTrackExtrapolationTool. It uses the double perigee to spot the tracks to be split.
    */
    std::pair<Trk::Track*,Trk::Track*> splitTrack( const Trk::Track& track ) const;

    /** construct a track from a list of TSOS and a start parameters */
    Trk::Track* fitSplitTrack( const Trk::TrackParameters& startPars, const std::vector<const Trk::TrackStateOnSurface*>& tsos, GarbageCan& garbage ) const;

  private:
    /** clean and evaluate the track, 
        @return 0 if Track does not forfill criteria, a unique_ptr otherwise (could be to a track which is the same as the current)
    */
    std::unique_ptr<Trk::Track> cleanAndEvaluateTrack( Trk::Track& track, const std::set<Identifier>& excludedChambers ) const;

    
    /** extract all information needed for the fit from the track */
    bool extractData( const MuPatCandidateBase& entry1, const MuPatCandidateBase& entry2, FitterData& fitterData, GarbageCan& garbage ) const;

    /** extract all information from the HitList of a FitterData object */
    bool extractData( FitterData& fitterData, bool usePreciseHits ) const;

    /** check fitterData, add fake phi hits if needed. If provided the reference parameter will be used to calcualte the fake hits */
    bool addFakePhiHits( FitterData& fitterData, const Trk::TrackParameters* referenceParameter, GarbageCan& garbage ) const;

    /** sanity check for entries */
    bool corruptEntry( const MuPatCandidateBase& entry ) const;

    /** get material */
    bool getMaterial( const Trk::TrackParameters& pars, FitterData& fitterData ) const;
    
    /** create a perigee parameter give the input data */
    Trk::Perigee* createStartParameters( FitterData& inputData, GarbageCan& garbage ) const;

    /** get segment from entry */
    const MuonSegment* segmentFromEntry( const MuPatCandidateBase& entry, GarbageCan& garbage ) const;

    /** check whether data has sufficient phi constraints */
    unsigned int hasPhiConstrain( FitterData& inputData) const;

    /** check whether data has sufficient phi constraints */
    unsigned int hasPhiConstrain( const Trk::Track& track ) const;
    
    /** create fake phi hit on the surface of the give measurement */
    const Trk::MeasurementBase* createFakePhiForMeasurement( const Trk::MeasurementBase& measurement, const Amg::Vector3D* overlapPos, 
                                                             const Amg::Vector3D* phiPos, double error, GarbageCan& garbage ) const;

    /** get q/p from entry */
    double qOverPFromEntry( const MuPatCandidateBase& entry ) const;

    /** get q/p using angle + position of the two entries */
    double qOverPFromEntries( const MuPatCandidateBase& firstEntry, const MuPatCandidateBase& secondEntry, GarbageCan& garbage ) const;

    /** calculate phi used to for seeding the fit */
    double phiSeeding( FitterData& fitterData ) const;

    /** calculate theta used for seeding the fit */
    double thetaSeeding( const MuPatCandidateBase& entry, MeasVec& etaHits ) const;

    /** clean phi hits, returns true if anything happened during the cleaning */
    bool cleanPhiHits( double momentum, FitterData& phiHits, const PrepVec* patternPhiHits, GarbageCan& garbage ) const;

    /** check whether mometum of start parameter is ok */
    bool validMomentum( const Trk::TrackParameters& pars ) const;

    /** calculate the minimum and maximum phi value a track could have to pass all eta channels */
    bool getMinMaxPhi( FitterData& fitterData ) const;

    void removeSegmentOutliers( FitterData& fitterData ) const;
    void cleanEntry( const MuPatCandidateBase& entry, std::set<Identifier>& removedIdentifiers  ) const;
    void cleanSegment( const MuonSegment& seg, std::set<Identifier>& removedIdentifiers  ) const;
    void copyHitList( const MuPatHitList& hitList, MuPatHitList& copy, GarbageCan& garbage ) const;

    ToolHandle<Trk::IPropagator>                m_propagator;  //!< propagator
    ToolHandle<Trk::ITrackFitter>               m_trackFitter; //!< fitter
    ToolHandle<Trk::ITrackFitter>               m_trackFitterPrefit; //!< fitter used for prefit
    ToolHandle<MuPatHitTool>                    m_hitHandler; //!< hit handler
    ToolHandle<IMuonSegmentMomentumEstimator>   m_momentumEstimator; //!< tool to estimate track momentum
 
    Trk::RunOutlierRemoval                m_runOutlier;         //!< switch whether to run outlier logics or not
    int                                   m_matEffects;         //!< type of material interaction in extrapolation
    Trk::ParticleHypothesis               m_ParticleHypothesis; //!< nomen est omen 
    Trk::MagneticFieldProperties          m_magFieldProperties; //!< magnetic field properties
    ToolHandle<MuonIdHelperTool>          m_idHelperTool;       //!< id helper tool
    ServiceHandle<IMuonEDMHelperSvc>      m_edmHelperSvc {this, "edmHelper", 
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
      "Handle to the service providing the IMuonEDMHelperSvc interface" };         //!< multi purpose helper tool
    ToolHandle<MuonEDMPrinterTool>        m_printer;            //!< tool to print out EDM objects
    ToolHandle<IMuonTrackToSegmentTool>   m_trackToSegmentTool; //!< helper tool to convert tracks into segments
    ToolHandle<IMdtDriftCircleOnTrackCreator>   m_mdtRotCreator; //!< mdt tube hit creator
    ToolHandle<IMuonHitSelector>          m_phiHitSelector; //!< tool to clean phi hits
    ToolHandle<IMuonTrackCleaner>         m_cleaner;
    ToolHandle<IMuonSegmentInOverlapResolvingTool>   m_overlapResolver;
    ToolHandle<Trk::ITrackSummaryHelperTool>         m_trackSummaryTool; //<! muon track summary helper

    bool         m_slFit;              //<! perform sl fit
    bool         m_seedWithSegmentTheta; //<! seed with theta connecting first + last eta hit
    bool         m_seedWithAvePhi;       //<! seed with average phi of all phi hits
    bool         m_seedPhiWithEtaHits;   //<! seed phi from positions first last eta hit
    bool         m_usePreciseHits;      //<! use actual measurement error
    bool         m_usePrefit;           //<! use prefit 
    bool         m_allowFirstFit;       //<! return the result of the prefit is final fit fails
    double       m_pThreshold;          //<! momentum cut-off. Seeds below the threshold will not be fitted
    bool         m_cosmics;             //<! special treatment for cosmics
    bool         m_cleanPhiHits;        //<! special flag to switch off phi hit cleaning
    unsigned int m_phiHitsMax;          //<! if more than maximum number of phi hits on pattern, no hits will be added
    bool         m_seedAtStartOfTrack;  //<! provide seed parameters at the start of the track
    bool         m_preciseFirstStation; //<! use precise hits in first station to stabalise the fit

    double m_openingAngleCut;  //<! cut on the maximum difference in phi between measurements on the track
    double m_preCleanChi2Cut;  //<! minimum chi2/ndof for a track to be passed to cleaner
    double m_chi2Cut;          //<! minimum chi2/ndof for a track to be accepted

    mutable std::atomic_uint m_nfits;
    mutable std::atomic_uint m_nfailedExtractInital;
    mutable std::atomic_uint m_nfailedMinMaxPhi;
    mutable std::atomic_uint m_nfailedParsInital;
    mutable std::atomic_uint m_nfailedExtractCleaning;
    mutable std::atomic_uint m_nfailedFakeInitial;
    mutable std::atomic_uint m_nfailedTubeFit;
    mutable std::atomic_uint m_noPerigee;
    mutable std::atomic_uint m_nlowMomentum;
    mutable std::atomic_uint m_nfailedExtractPrecise;
    mutable std::atomic_uint m_nfailedFakePrecise;
    mutable std::atomic_uint m_nfailedFitPrecise;
    mutable std::atomic_uint m_nsuccess;    
    Trk::TrackInfo::TrackPatternRecoInfo m_patRecInfo;

    struct StationPhiData {
      unsigned int nphiHits;
      unsigned int nSmallChambers;
      unsigned int nLargeChambers;
    };
    
  };

  
}


#endif
