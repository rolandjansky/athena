/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONTRACKCLEANER_H
#define MUON_MUONTRACKCLEANER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonTrackMakerUtils/SortTracksByHitNumber.h"

#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecToolInterfaces/IMuonTrackCleaner.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonCompetingClustersOnTrackCreator.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkToolInterfaces/IUpdator.h"

// For magneticfield
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "TrkParameters/TrackParameters.h"

#include <string>
#include <set>

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"

#include "CxxUtils/checker_macros.h"
ATLAS_CHECK_FILE_THREAD_SAFETY;

namespace Trk {
  class Track;
  class TrackStateOnSurface;
  class MeasurementBase;
  class FitQuality;
}

namespace Muon {

  class MuonTrackCleaner : virtual public IMuonTrackCleaner, public AthAlgTool {
  public:
    struct MCTBCleaningInfo {
      Identifier  id;
      Identifier  chId;
      MuonStationIndex::ChIndex chIndex;
      int         useInFit;
      bool        inBounds;
      bool        isNoise;
      bool        copyState;  // if true the state will be copied instead of evaluated for outliers
      double      residual;
      double      pull;
      const Trk::TrackStateOnSurface* originalState;
      const Trk::MeasurementBase*  meas;
      const Trk::TrackParameters*  pars;
      std::unique_ptr<Trk::ResidualPull>     resPull;
      std::unique_ptr<MdtDriftCircleOnTrack> flippedMdt;
      std::unique_ptr<CompetingMuonClustersOnTrack> cleanedCompROT;
      const Trk::FitQuality*       fitQ;

      MCTBCleaningInfo( const Trk::TrackStateOnSurface* orState ) : 
        id(),chId(),chIndex(MuonStationIndex::ChUnknown),useInFit(1),inBounds(true),isNoise(false),copyState(true),residual(1e10),pull(1e10),
        originalState(orState),meas(0),pars(0),resPull(),flippedMdt(),cleanedCompROT(),fitQ(0) {}

      MCTBCleaningInfo( const Identifier& i, const Identifier& chi, MuonStationIndex::ChIndex chIn, bool inB, double r, double p,
      const Trk::TrackStateOnSurface* orState ) : 
        id(i),chId(chi),chIndex(chIn),useInFit(1),inBounds(inB),isNoise(false),copyState(true),residual(r),pull(p),
        originalState(orState),meas(0),pars(0),resPull(),flippedMdt(),cleanedCompROT(),fitQ(0) {}

      MCTBCleaningInfo( const Identifier& i, const Identifier& chi, MuonStationIndex::ChIndex chIn, bool inB, double r, double p,
      const Trk::TrackStateOnSurface* orState, const Trk::MeasurementBase* me,
      const Trk::TrackParameters* par, std::unique_ptr<Trk::ResidualPull>& resP, const Trk::FitQuality* fq ) : 
        id(i),chId(chi),chIndex(chIn),useInFit(1),inBounds(inB),isNoise(false),copyState(false), residual(r), pull(p),
        originalState(orState),meas(me),pars(par),resPull(std::move(resP)),flippedMdt(),cleanedCompROT(),fitQ(fq) {}
    };
    typedef std::vector<MCTBCleaningInfo>   InfoVec;
    typedef InfoVec::iterator               InfoIt;

    typedef std::set<const Trk::MeasurementBase*> MeasSet;
    typedef MeasSet::iterator                     MeasIt;

    /** struct to store return values of chamber removal, contains the new track plus a list the removed hits */
    struct ChamberRemovalOutput {
      ChamberRemovalOutput() : track() {}
      std::unique_ptr<Trk::Track> track;
      Identifier chId;
      std::vector<MCTBCleaningInfo*> removedHits;
    };
    struct SortChamberRemovalResultByChi2Ndof {
      bool operator()(const ChamberRemovalOutput& r1, const ChamberRemovalOutput& r2 ) const {
        SortTracksByChi2Ndof sortTracks;
        return sortTracks( r1.track.get(), r2.track.get() );
      }
    };

    struct ChamberPullInfo {
      ChamberPullInfo() : pullSum(0.),maxPull(0.),nhits(0) {}
      double pullSum;
      double maxPull;
      int    nhits;
    };
    typedef std::map<Identifier,ChamberPullInfo > PullChamberMap;
    typedef PullChamberMap::iterator              PullChamberIt;
    typedef PullChamberMap::const_iterator        PullChamberCit;

    struct EtaPhiHits {
      EtaPhiHits() : neta(0),nphi(0) {}
      int neta;
      int nphi;
    };
    typedef std::map<Identifier,EtaPhiHits>    EtaPhiPerChamberMap;
    typedef EtaPhiPerChamberMap::iterator      EtaPhiPerChamberIt;

    typedef std::vector< std::pair<double,Identifier> > PullChVec;
    typedef PullChVec::iterator                         PullChIt;

    struct SortByAvePull {
      bool operator()( const std::pair<double,Identifier>& entry1, const std::pair<double,Identifier>& entry2 ) const {
        return entry1.first > entry2.first;
      }
    };

    struct ChamberLayerStatistics {
      MuonStationIndex::ChIndex chIndex;
      unsigned int nhits;
      unsigned int noutliers;
      unsigned int ndeltas;
      unsigned int nrecoverableOutliers;
      unsigned int noutBounds;
      ChamberLayerStatistics() : chIndex(MuonStationIndex::ChUnknown), nhits(0), noutliers(0), ndeltas(0), nrecoverableOutliers(0), noutBounds(0) {}
    };

    //struct to hold the internal state information, i.e. the various track properties relevant for the cleaner
    struct CleaningState{
      InfoVec    measInfo; 
      MeasSet    largePullMeasurements;
      MeasSet    flippedMdts;
      MeasSet    largePullPseudoMeasurements;
      
      //need to keep these somewhere while they're being used: couldn't figure out how to make unique_ptr work with Trk::TrackParameter given the inheritance issues
      std::vector<const Trk::TrackParameters*> parsToBeDeleted;

      unsigned int nscatterers;
      unsigned int nhits;
      unsigned int noutliers;

      bool         slFit;
      unsigned int nIdHits;
      unsigned int nPseudoMeasurements;
      unsigned int nPhiConstraints;
    
      unsigned int numberOfFlippedMdts;
      unsigned int numberOfCleanedCompROTs;
      bool         hasOfBoundsOutliers;
      bool         hasVertexConstraint;
      bool         hasSmall;
      bool         hasLarge;

      ChamberPullInfo pullSum;
      ChamberPullInfo pullSumPhi;
      ChamberPullInfo pullSumTrigEta;

      EtaPhiPerChamberMap hitsPerChamber;
      EtaPhiPerChamberMap outBoundsPerChamber;

      PullChamberMap pullSumPerChamber;
      PullChamberMap pullSumPerChamberPhi;
      PullChamberMap pullSumPerChamberEta;

      PullChVec chambersToBeRemoved;
      PullChVec chambersToBeRemovedPhi;

      std::set<MuonStationIndex::StIndex> stations;
      std::set<MuonStationIndex::PhiIndex> phiLayers;

      std::map<MuonStationIndex::ChIndex,ChamberLayerStatistics> chamberLayerStatistics;
    
      std::set<Identifier> chamberRemovalExclusionList;
    CleaningState() : nscatterers(0), nhits(0), noutliers(0), nIdHits(0), nPseudoMeasurements(0), nPhiConstraints(0), numberOfFlippedMdts(0), numberOfCleanedCompROTs(0),
	hasOfBoundsOutliers(false), hasVertexConstraint(false), hasSmall(false), hasLarge(false) {
      pullSum = ChamberPullInfo();
      pullSumPhi = ChamberPullInfo();
      pullSumTrigEta = ChamberPullInfo();
    }
    };

  public:
    /** @brief constructor */
    MuonTrackCleaner(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    ~MuonTrackCleaner () = default;
    
    /** @brief AlgTool initialize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();
    
    /** @brief clean a track, returns a pointer to a new track if successfull.
    If the input track is does not require cleaning a pointer the the initial track is return in which case the 
    user should not delete the old track!
    The caller should ensure the track gets deleted. */
    std::unique_ptr<Trk::Track> clean( Trk::Track& track ) const;

    /** @brief clean a track, returns a pointer to a new track if successfull.
    If the input track is does not require cleaning a pointer the the initial track is return in which case the 
    user should not delete the old track! The cleaning will not clean if all the chambers in the exclusions list 
    are marked as to be deleted.
    The caller should ensure the track gets deleted. */
    std::unique_ptr<Trk::Track> clean( Trk::Track& track, const std::set<Identifier>& chamberRemovalExclusionList ) const;

    /** @brief calculate Residual/Pull for a given MeasurementBase + TrackParameters, ownership is transfered to user */
    const Trk::ResidualPull* calculateResPul( const Trk::MeasurementBase& meas, const Trk::TrackParameters& pars ) const;

  private:

    
    /** @brief clean a track, actual implementation */
    std::unique_ptr<Trk::Track> cleanTrack( Trk::Track* track, CleaningState& state ) const;

    /** @brief calculate the pull given measurement error and track error */
    double calcPull( const double residual, const double locMesCov, const double locTrkCov, const bool& trkStateIsUnbiased ) const;

    /** some useful print routines */
    std::string toString( const Trk::Track track ) const;
    std::string toString( const Trk::ResidualPull& resPull ) const;


    /** clean up competing ROTs that consist out of two clusters */
    std::unique_ptr<Trk::Track> cleanCompROTs( std::unique_ptr<Trk::Track> track, CleaningState& state ) const;

    /** flip signs of MDT hits with large pull if pull if the oppositely signed radius is small */
    std::unique_ptr<Trk::Track> recoverFlippedMdt( std::unique_ptr<Trk::Track> track, CleaningState& state ) const;

    /** remove bad hits from track. The returned track pointer can be zero, pointing to the initial track or a new pointer */
    std::unique_ptr<Trk::Track> hitCleaning( std::unique_ptr<Trk::Track> track, CleaningState& state ) const;

    /** remove bad chamber from track. The returned track pointer can be zero, pointing to the initial track or a new pointer */
    std::unique_ptr<Trk::Track> chamberCleaning( std::unique_ptr<Trk::Track> track, CleaningState& state ) const;

    /** recover outliers that are within the cuts.
	If the additional chamber index is provided, the code will only consider that particular layer
    */
    std::unique_ptr<Trk::Track> outlierRecovery( std::unique_ptr<Trk::Track> track, CleaningState& state, MuonStationIndex::ChIndex* currentIndex = 0 ) const;

    /** check whether hit is an outlier */
    bool isOutsideOnTrackCut( const Identifier& id, double res, double pull, double cutScaleFactor ) const;

    /** remove chamber from track */
    ChamberRemovalOutput removeChamber( Trk::Track* track, Identifier chId, bool removePhi, bool removeEta, CleaningState& state ) const;
    
    /** init cleaner */
    void init( const Trk::Track& track, CleaningState& state ) const;

    //check for station removal
    bool checkStations(CleaningState& state ) const;

    //check for failure due to inner removal
    bool checkInnerConstraint(CleaningState& state ) const;

    //check for not enough phi constraints
    bool checkPhiConstraint(CleaningState& state ) const;

    //unremove hits for next iteration of chamber cleaning
    void unremoveHits(ChamberRemovalOutput& result) const;

    //print track states
    void printStates(Trk::Track* track) const;

    //choose fitter and fit
    std::unique_ptr<Trk::Track> fitTrack(Trk::Track& track,Trk::ParticleHypothesis pHyp,bool slFit) const;

    ToolHandle<Trk::ITrackFitter>                    m_trackFitter       {this, "Fitter", "Trk::GlobalChi2Fitter/MCTBFitterMaterialFromTrack",};
    ToolHandle<Trk::ITrackFitter>                    m_slTrackFitter     {this, "SLFitter", "Trk::GlobalChi2Fitter/MCTBSLFitterMaterialFromTrack"};
    ToolHandle<Trk::IUpdator>                        m_measurementUpdator{this, "MeasurementUpdator", "Trk::KalmanUpdator/MuonMeasUpdator"};
    ToolHandle<Muon::IMdtDriftCircleOnTrackCreator>  m_mdtRotCreator     {this, "MdtRotCreator", "Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator"}; 
    ToolHandle<IMuonCompetingClustersOnTrackCreator> m_compRotCreator    {this, "CompRotCreator", "Muon::TriggerChamberClusterOnTrackCreator/TriggerChamberClusterOnTrackCreator"};
    ToolHandle<Trk::IResidualPullCalculator>         m_pullCalculator    {this, "PullCalculator", "Trk::ResidualPullCalculator/ResidualPullCalculator"};
    ServiceHandle<Muon::IMuonEDMHelperSvc>           m_edmHelperSvc      {this, "edmHelper", 
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
      "Handle to the service providing the IMuonEDMHelperSvc interface" };
    ToolHandle<Muon::MuonEDMPrinterTool>             m_printer           {this, "Printer", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ToolHandle<Trk::IExtrapolator>                   m_extrapolator      {this, "Extrapolator", "Trk::Extrapolator/AtlasExtrapolator"};
    ToolHandle<Trk::IExtrapolator>                   m_slextrapolator      {this, "SlExtrapolator", "Trk::Extrapolator/MuonStraightLineExtrapolator"};
    // Read handle for conditions object to get the field cache
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj",
                                                                               "Name of the Magnetic Field conditions object key"};

    Gaudi::Property<bool> m_useMdtResiCut             {this, "UseMdtResiCut", false};
    Gaudi::Property<double> m_chi2Cut                 {this, "Chi2Cut", 100.};
    Gaudi::Property<double> m_pullCut                 {this, "PullCut", 5.};
    Gaudi::Property<double> m_mdtResiCut              {this, "MdtResiCut", 1.};
    Gaudi::Property<double> m_pullCutPhi              {this, "PullCutPhi", 10.};
    Gaudi::Property<double> m_associationScaleFactor  {this, "AssociationScaleFactor", 0.7};
    Gaudi::Property<unsigned int> m_ncycles           {this, "CleaningCycles", 5};
    Gaudi::Property<double> m_avePullSumPerChamberCut  {this, "MaxAvePullSumPerChamber", 3.5};
    Gaudi::Property<bool> m_recoverOutliers           {this, "RecoverOutliers", true};
    Gaudi::Property<bool> m_flipMdtDriftRadii         {this, "FlipMdtDriftRadii", true};
    Gaudi::Property<bool> m_cleanCompROTs             {this, "CleanCompROTs", true};
    Gaudi::Property<bool> m_onlyUseHitErrorInRecovery {this, "OnlyUseHitErrorInRecovery", true};
    Gaudi::Property<double> m_adcCut                  {this, "AdcCut", 50.};
    Gaudi::Property<bool> m_iterate                   {this, "Iterate", 0.7};
    Gaudi::Property<bool> m_use_slFit                 {this, "UseSLFit", false};
    

    /** helper function to extract chambers that are to be removed */
    bool extractChambersToBeRemoved( CleaningState& state, std::set<Identifier>& chambersToBeRemovedSet, bool usePhi=false ) const;

    std::string print( ChamberLayerStatistics& statistics ) const;
  };

}

#endif 
