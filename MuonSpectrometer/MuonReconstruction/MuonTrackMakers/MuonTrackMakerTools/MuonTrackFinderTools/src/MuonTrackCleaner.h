/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONTRACKCLEANER_H
#define MUON_MUONTRACKCLEANER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonTrackMakerUtils/SortTracksByHitNumber.h"

#include "MuonRecToolInterfaces/IMuonTrackCleaner.h"
#include "MuonIdHelpers/MuonStationIndex.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "TrkParameters/TrackParameters.h"

#include "Identifier/Identifier.h"
#include <string>
#include <set>


class MsgStream;

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Muon {
  class IMdtDriftCircleOnTrackCreator;
  class IMuonCompetingClustersOnTrackCreator;
  class MuonEDMHelperTool;
  class MuonEDMPrinterTool;
  class MuonIdHelperTool;
  class MdtDriftCircleOnTrack;
  class CompetingMuonClustersOnTrack;
}

namespace Trk {
  class Track;
  class TrackStateOnSurface;
  class ResidualPull;
  class ITrackFitter;
  class IUpdator;
  class MeasurementBase;
  class FitQuality;
  class IResidualPullCalculator;
  class IExtrapolator;
}

namespace Muon {

  /**
     @brief Helper tool that creates muon Identifiers and can be used to print Identifiers 

  */
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
      const Trk::ResidualPull*     resPull;
      const MdtDriftCircleOnTrack* flippedMdt;
      const CompetingMuonClustersOnTrack* cleanedCompROT;
      const Trk::FitQuality*       fitQ;

      MCTBCleaningInfo( const Trk::TrackStateOnSurface* orState ) : 
	id(),chId(),chIndex(MuonStationIndex::ChUnknown),useInFit(1),inBounds(true),isNoise(false),copyState(true),residual(1e10),pull(1e10),
	originalState(orState),meas(0),pars(0),resPull(0),flippedMdt(0),cleanedCompROT(0),fitQ(0) {}

      MCTBCleaningInfo( const Identifier& i, const Identifier& chi, MuonStationIndex::ChIndex chIn, bool inB, double r, double p,
			const Trk::TrackStateOnSurface* orState ) : 
	id(i),chId(chi),chIndex(chIn),useInFit(1),inBounds(inB),isNoise(false),copyState(true),residual(r),pull(p),
	originalState(orState),meas(0),pars(0),resPull(0),flippedMdt(0),cleanedCompROT(0),fitQ(0) {}

      MCTBCleaningInfo( const Identifier& i, const Identifier& chi, MuonStationIndex::ChIndex chIn, bool inB, double r, double p,
			const Trk::TrackStateOnSurface* orState, const Trk::MeasurementBase* me,
			const Trk::TrackParameters* par, const Trk::ResidualPull* resP, const Trk::FitQuality* fq ) : 
	id(i),chId(chi),chIndex(chIn),useInFit(1),inBounds(inB),isNoise(false),copyState(false), residual(r), pull(p),
	originalState(orState),meas(me),pars(par),resPull(resP),flippedMdt(0),cleanedCompROT(0),fitQ(fq) {}
    };
    typedef std::vector<MCTBCleaningInfo>   InfoVec;
    typedef InfoVec::iterator               InfoIt;

    typedef std::set<const Trk::MeasurementBase*> MeasSet;
    typedef MeasSet::iterator                     MeasIt;

    struct Counters {
      Counters() :       
	nclean(0),nvertexConstraints(0),nIdTracks(0),nfitsSL(0),nfitsMom(0),
	nchCleaning(0),nfailedChamberCleaning(0),nchTwoChambers(0),nchExclusionList(0),nchAllLost(0),nchOneStationLeft(0),nchMomLoss(0),
	nflipMdt(0),nflipMdtFailed(0),ncleanComp(0),ncleanCompFailed(0),
	nhitCleaning(0),nfailedHitCleaning(0),nhitOneChamberLeft(0),nhitMomLoss(0),nhitTooManyOutliers(0),
	nhitTooFewHits(0),nhitTotCycles(0),nhitFitFailed(0),nhitEndOffCycle(0),
	noutlierCut(0),nsuccess(0)     {}

      unsigned int nclean;
      unsigned int nvertexConstraints;
      unsigned int nIdTracks;
      unsigned int nfitsSL;
      unsigned int nfitsMom;
      unsigned int nchCleaning;
      unsigned int nfailedChamberCleaning;
      unsigned int nchTwoChambers;
      unsigned int nchExclusionList;
      unsigned int nchAllLost;
      unsigned int nchOneStationLeft;
      unsigned int nchMomLoss;
      unsigned int nflipMdt;
      unsigned int nflipMdtFailed;
      unsigned int ncleanComp;
      unsigned int ncleanCompFailed;
      unsigned int nhitCleaning;
      unsigned int nfailedHitCleaning;
      unsigned int nhitOneChamberLeft;
      unsigned int nhitMomLoss;
      unsigned int nhitTooManyOutliers;
      unsigned int nhitTooFewHits;
      unsigned int nhitTotCycles;
      unsigned int nhitFitFailed;
      unsigned int nhitEndOffCycle;
      unsigned int noutlierCut;
      unsigned int nsuccess;
    };

    /** struct to store return values of chamber removal, contains the new track plus a list the removed hits */
    struct ChamberRemovalOutput {
      ChamberRemovalOutput() : track(0) {}
      Trk::Track* track;
      Identifier chId;
      std::vector<MCTBCleaningInfo*> removedHits;
    };
    struct SortChamberRemovalResultByChi2Ndof {
      bool operator()(const ChamberRemovalOutput& r1, const ChamberRemovalOutput& r2 ) const {
	SortTracksByChi2Ndof sortTracks;
	return sortTracks( r1.track, r2.track );
      }
    };

  public:
    /** @brief constructor */
    MuonTrackCleaner(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    ~MuonTrackCleaner ();
    
    /** @brief AlgTool initilize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();
    
    /** @brief clean a track, returns a pointer to a new track if successfull.
	If the input track is does not require cleaning a pointer the the initial track is return in which case the 
	user should not delete the old track!
	The caller should ensure the track gets deleted. */
    Trk::Track* clean( Trk::Track& track ) const;

    /** @brief clean a track, returns a pointer to a new track if successfull.
	If the input track is does not require cleaning a pointer the the initial track is return in which case the 
	user should not delete the old track! The cleaning will not clean if all the chambers in the exclusions list 
	are marked as to be deleted.
	The caller should ensure the track gets deleted. */
    Trk::Track* clean( Trk::Track& track, const std::set<Identifier>& chamberRemovalExclusionList ) const;

    /** @brief calculate Residual/Pull for a given MeasurementBase + TrackParameters, ownership is transfered to user */
    const Trk::ResidualPull* calculateResPul( const Trk::MeasurementBase& meas, const Trk::TrackParameters& pars ) const;

  private:

    
    /** @brief clean a track, actual implementation */
    Trk::Track* cleanTrack( Trk::Track& track ) const;

    /** @brief calculate the pull given measurement error and track error */
    double calcPull( const double residual, const double locMesCov, const double locTrkCov, const bool& trkStateIsUnbiased ) const;

    /** some usefull print routines */
    std::string toString( const Trk::Track track ) const;
    std::string toString( const Trk::ResidualPull& resPull ) const;


    /** clean up competing ROTs that consist out of two clusters */
    Trk::Track* cleanCompROTs( Trk::Track& track ) const;

    /** flip signs of MDT hits with large pull if pull if the oppositely signed radius is small */
    Trk::Track* recoverFlippedMdt( Trk::Track& track ) const;

    /** remove bad hits from track. The returned track pointer can be zero, pointing to the initial track or a new pointer */
    Trk::Track* hitCleaning( Trk::Track& track ) const;

    /** remove bad chamber from track. The returned track pointer can be zero, pointing to the initial track or a new pointer */
    Trk::Track* chamberCleaning( Trk::Track& track ) const;

    /** recover outliers that are within the cuts.
	If the additional chamber index is provided, the code will only consider that particular layer
    */
    Trk::Track* outlierRecovery( Trk::Track& track, MuonStationIndex::ChIndex* currentIndex = 0 ) const;

    /** check whether hit is an outlier */
    bool isOutsideOnTrackCut( const Identifier& id, double res, double pull, double cutScaleFactor ) const;

    /** remove chamber from track */
    ChamberRemovalOutput removeChamber( Trk::Track& track, Identifier chId, bool removePhi, bool removeEta ) const;
    
    /** cleanup memory */
    void cleanUp() const;
    void cleanUpTracks() const;

    /** init cleaner */
    void init( const Trk::Track& track ) const;

    ToolHandle<Trk::ITrackFitter>                    m_trackFitter;
    ToolHandle<Trk::ITrackFitter>                    m_slTrackFitter;
    mutable const Trk::ITrackFitter*                 m_fitter; // pointer to actual fitter to be used
    ToolHandle<Trk::IUpdator>                        m_measurementUpdator;
    ToolHandle<Muon::IMdtDriftCircleOnTrackCreator>  m_mdtRotCreator; 
    ToolHandle<IMuonCompetingClustersOnTrackCreator> m_compRotCreator;
    ToolHandle<Trk::IResidualPullCalculator>         m_pullCalculator;
    ToolHandle<Muon::MuonEDMHelperTool>              m_helper;
    ToolHandle<Muon::MuonEDMPrinterTool>             m_printer;
    ToolHandle<Muon::MuonIdHelperTool>               m_idHelper;
    ServiceHandle<MagField::IMagFieldSvc>            m_magFieldSvc; 
    ToolHandle<Trk::IExtrapolator>                   m_extrapolator;

    bool   m_useMdtResiCut;
    double m_chi2Cut;
    double m_pullCut;
    double m_mdtResiCut;
    double m_pullCutPhi;
    double m_avePullSumPerChamberCut;
    double m_associationScaleFactor;
    unsigned int m_ncycles;
    bool m_recoverOutliers;
    bool m_flipMdtDriftRadii;
    bool m_cleanCompROTs;
    bool m_onlyUseHitErrorInRecovery;
    double m_adcCut;
    bool m_iterate;

    mutable InfoVec    m_measInfo; 
    mutable MeasSet    m_largePullMeasurements;
    mutable MeasSet    m_flippedMdts;
    mutable MeasSet    m_largePullPseudoMeasurements;

    mutable unsigned int m_numberOfFlippedMdts;
    mutable unsigned int m_numberOfCleanedCompROTs;
    mutable bool         m_hasOfBoundsOutliers;

    mutable std::vector<const Trk::ResidualPull*>    m_resPullsToBeDeleted;
    mutable std::vector<const Trk::TrackParameters*> m_parsToBeDeleted;
    mutable std::vector<const Trk::FitQuality*>      m_fitQToBeDeleted;
    mutable std::vector<const Trk::MeasurementBase*> m_measurementsToBeDeleted;
    mutable std::set<Trk::Track*>                    m_trackToBeDeleted;

    mutable unsigned int m_nscatterers;
    mutable unsigned int m_nhits;
    mutable unsigned int m_noutliers;

    mutable bool         m_slFit;
    mutable bool         m_hasVertexConstraint;
    mutable bool         m_hasSmall;
    mutable bool         m_hasLarge;
    mutable unsigned int m_nIdHits;
    mutable unsigned int m_nPseudoMeasurements;
    mutable unsigned int m_nPhiConstraints;
    
    struct ChamberPullInfo {
      ChamberPullInfo() : pullSum(0.),maxPull(0.),nhits(0) {}
      double pullSum;
      double maxPull;
      int    nhits;
    };

    mutable ChamberPullInfo m_pullSum;
    mutable ChamberPullInfo m_pullSumPhi;
    mutable ChamberPullInfo m_pullSumTrigEta;
    
    struct EtaPhiHits {
      EtaPhiHits() : neta(0),nphi(0) {}
      int neta;
      int nphi;
    };

    typedef std::map<Identifier,EtaPhiHits>    EtaPhiPerChamberMap;
    typedef EtaPhiPerChamberMap::iterator      EtaPhiPerChamberIt;

    mutable EtaPhiPerChamberMap m_hitsPerChamber;
    mutable EtaPhiPerChamberMap m_outBoundsPerChamber;


    typedef std::map<Identifier,ChamberPullInfo > PullChamberMap;
    typedef PullChamberMap::iterator              PullChamberIt;
    typedef PullChamberMap::const_iterator        PullChamberCit;
    mutable PullChamberMap m_pullSumPerChamber;
    mutable PullChamberMap m_pullSumPerChamberPhi;
    mutable PullChamberMap m_pullSumPerChamberEta;

    
    struct SortByAvePull {
      bool operator()( const std::pair<double,Identifier>& entry1, const std::pair<double,Identifier>& entry2 ) const {
	return entry1.first > entry2.first;
      }
    };
    typedef std::vector< std::pair<double,Identifier> > PullChVec;
    typedef PullChVec::iterator                         PullChIt;
    mutable PullChVec m_chambersToBeRemoved;
    mutable PullChVec m_chambersToBeRemovedPhi;

    mutable std::set<MuonStationIndex::StIndex> m_stations;
    mutable std::set<MuonStationIndex::PhiIndex> m_phiLayers;

    /** helper function to extract chambers that are to be removed */
    bool extractChambersToBeRemoved( const PullChamberMap& pullSumPerChamber, PullChVec& chambersToBeRemoved, std::set<Identifier>& chambersToBeRemovedSet   ) const;



    struct ChamberLayerStatistics {
      MuonStationIndex::ChIndex chIndex;
      unsigned int nhits;
      unsigned int noutliers;
      unsigned int ndeltas;
      unsigned int nrecoverableOutliers;
      unsigned int noutBounds;
      ChamberLayerStatistics() : chIndex(MuonStationIndex::ChUnknown), nhits(0), noutliers(0), ndeltas(0), nrecoverableOutliers(0), noutBounds(0) {}
    };

    
    std::string print( ChamberLayerStatistics& statistics ) const;
    mutable std::map<MuonStationIndex::ChIndex,ChamberLayerStatistics> m_chamberLayerStatistics;
    
    mutable Counters m_counters;

    mutable std::set<Identifier> m_chamberRemovalExclusionList;
  };

}

#endif 
