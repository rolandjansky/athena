/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATHSEGMENTMAKER_H
#define DCMATHSEGMENTMAKER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonSegment/MuonSegment.h"

#include "TrkDriftCircleMath/DCSLFitter.h"
#include "TrkDriftCircleMath/DCSLHitSelector.h"
#include "TrkDriftCircleMath/MdtChamberGeometry.h"
#include "TrkDriftCircleMath/DCStatistics.h"
#include "TrkDriftCircleMath/Cluster.h"
#include "TrkDriftCircleMath/DriftCircle.h"

#include "TrkSurfaces/Surface.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentTriggerHitAssociator.h"
#include "TrkFitterInterfaces/ITrackFitter.h"

#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include <vector>
#include <set>
#include <string>
#include <list>

namespace Trk {
  class RIO_OnTrack;
  class PlaneSurface;
}

namespace TrkDriftCircleMath {
  class Line;
  class MdtMultiChamberGeometry;
  class Segment;
  class DCSLFitter;
}

namespace MuonGM {
  class MuonDetectorManager;
}

namespace AdjT0 {
  class IAdjustableT0Tool;
}

namespace Muon {
  class IMuonCompetingClustersOnTrackCreator;
  class IMdtDriftCircleOnTrackCreator;
  class IMuonClusterOnTrackCreator;
  class IMdtSegmentFinder;
  class IMuonTrackCleaner;
  class MuonIdHelperTool;
  class MuonEDMHelperTool;
  class MuonEDMPrinterTool;
  class IMuonSegmentFittingTool;
  class IMuonSegmentSelectionTool;
  class IDCSLFitProvider;
  class MdtPrepData;
}

class MdtIdHelper;
class RpcIdHelper;
class TgcIdHelper;
class Identifier;
class MuonStationIntersectSvc;

class MsgStream;

namespace Muon {

class MdtDriftCircleOnTrack;

  /** 
      Function object to sort MuonClusterOnTrack pointers by Identier.
      Usage:
      
      std::vector<const MuonClusterOnTrack*> myClusters;
      
      ...

      std::sort( myClusters.begin(), myClusters.end(), SortClustersById() );
  */
  struct SortClustersById {
    bool operator()( const MuonClusterOnTrack* cl1, const MuonClusterOnTrack* cl2 ){
      return cl1->identify() < cl2->identify();
    }
  };


  /**  
      Function object to sort pairs containing a double and a pointer to a MuonClusterOnTrack. 
      The object is used to sort MuonClusterOnTrack objects along the trajectory of a give segment.
      Usage:
      
      std::vector<std::pair<double,const MuonClusterOnTrack*> > myDistClusters;
      
      ...

      std::sort( myDistClusters.begin(), myDistClusters.end(), SortByDistanceToSegment() );
  */ 
  struct SortByDistanceToSegment
  {
    bool operator()( const std::pair<double,const Trk::MeasurementBase*>& hit1, const std::pair<double,const Trk::MeasurementBase*>& hit2 ){
      return hit1.first < hit2.first;
    }
  };


  /** 
      @class DCMathSegmentMaker

      Implementation of a IMuonSegmentMaker. 

      For more details look at the mainpage of this package.
  */
  class DCMathSegmentMaker :  virtual public IMuonSegmentMaker, public IMuonSegmentTriggerHitAssociator, public AthAlgTool   {
  public:
    // pair of eta-phi hits in the same gasgap
    typedef std::pair<std::vector<const MuonClusterOnTrack*>,std::vector<const MuonClusterOnTrack*> > EtaPhiHitsPair;
    // map to sort hit per gasgap
    typedef std::map<Identifier,EtaPhiHitsPair> IdHitMap;
    typedef IdHitMap::iterator IdHitMapIt;
    typedef std::map<Identifier,IdHitMap> ChIdHitMap;
    typedef ChIdHitMap::iterator ChIdHitMapIt;

    typedef std::vector<const MuonClusterOnTrack*>::const_iterator ROTCit;

    struct HitInXZ {
      HitInXZ(Identifier i, bool isM, bool measP, double lx, double lz, double lxmin, double lxmax, double phmin, double phmax) :
	id(i),isMdt(isM),measPhi(measP),x(lx),z(lz),xmin(lxmin),xmax(lxmax),phimin(phmin),phimax(phmax) {}
      Identifier id;
      bool isMdt;
      bool measPhi;
      double x;
      double z;
      double xmin;
      double xmax;
      double phimin;
      double phimax;
    };

    struct Cluster2D {

      /** constructor taking a single phi hit */
      Cluster2D( const Identifier elId, const Identifier ggId, const Amg::Vector2D& lp, double err,
		 const MuonClusterOnTrack* ecl, const MuonClusterOnTrack* pcl ) :
	detElId(elId),gasGapId(ggId),locPos(lp),error(err),etaHit(ecl),phiHit(pcl) {
	if( ecl || pcl ) {
	  surface().localToGlobal(locPos,Amg::Vector3D::UnitZ(),globalPos); 
	}
	if( pcl ) phiHits.push_back(pcl);
      }
      /** constructor taking a vector of phi hits */
      Cluster2D( const Identifier elId, const Identifier ggId, const Amg::Vector2D& lp, double err,
		 const MuonClusterOnTrack* ecl, const std::vector<const MuonClusterOnTrack*>& phs ) :
	detElId(elId),gasGapId(ggId),locPos(lp),error(err),etaHit(ecl),phiHits(phs) {
	// if phiHits to empty point phiHit to first hit in PhiHits
	phiHit = phiHits.empty() ? 0 : phiHits.front();
	if( ecl || phiHit ) {
	  surface().localToGlobal(locPos,Amg::Vector3D::UnitZ(),globalPos); 
	}
      }
      Identifier                 detElId;
      Identifier                 gasGapId;
      Amg::Vector2D              locPos;
      double                     error; // assume same error for eta and phi           
      const MuonClusterOnTrack*  etaHit;
      const MuonClusterOnTrack*  phiHit;
      std::vector<const MuonClusterOnTrack*> phiHits;
      const Trk::Surface&        surface() const { 
	if( etaHit ) return etaHit->associatedSurface();
	else         return phiHit->associatedSurface();
      }
      Identifier identify() const {
	if( etaHit ) return etaHit->identify();
	else         return phiHit->identify();
      }
      Amg::Vector3D  globalPos;
      bool is2D() const { return etaHit && phiHit; }
      bool corrupt() const { return (!etaHit && !phiHit) || error < 0.01; }
    };
    typedef std::vector<Cluster2D> ClusterVec;
    typedef ClusterVec::iterator   ClusterIt;
    typedef ClusterVec::const_iterator   ClusterCit;

    struct TubeEnds {
      TubeEnds() : lxmin(0), lxmax(0), phimin(0), phimax(0) {}
      double lxmin;
      double lxmax;
      double phimin;
      double phimax;
    };

  public:
    /** default AlgTool constructor */
    DCMathSegmentMaker(const std::string&,const std::string&,const IInterface*);
    
    /** destructor */
    virtual ~DCMathSegmentMaker ();
    
    /** initialize method, method taken from bass-class AlgTool */
    virtual StatusCode initialize();

    /** finialize method, method taken from bass-class AlgTool */
    virtual StatusCode finalize();

    /** find segments starting from a list of RIO_OnTrack objects, implementation of IMuonSegmentMaker interface routine.
 
      Will call:
 
       std::vector<const MuonSegment*>* find( const Amg::Vector3D& gpos, const Amg::Vector3D& gdir,
                                                const std::vector<const MdtDriftCircleOnTrack*>& mdts,
                                              const std::vector<const MuonClusterOnTrack*>&  clusters, bool hasPhiMeasurements);
    */
    std::vector<const MuonSegment*>* find( const std::vector<const Trk::RIO_OnTrack*>& rios ) ;
    
    /** find segments starting from a list of RIO_OnTrack objects in multiple chambers, implementation of IMuonSegmentMaker interface routine 
       Will call:
 
       std::vector<const MuonSegment*>* find( const Amg::Vector3D& gpos, const Amg::Vector3D& gdir,
                                                const std::vector<const MdtDriftCircleOnTrack*>& mdts,
                                              const std::vector<const MuonClusterOnTrack*>&  clusters, bool hasPhiMeasurements);
     */
    std::vector<const MuonSegment*>* find( const std::vector<const Trk::RIO_OnTrack*>& rios1,
					   const std::vector<const Trk::RIO_OnTrack*>& rios2 ) ;
     
   /** find segments starting from:
	- a list of MdtDriftCircleOnTrack objects
	- a list of MuonClusterOnTrack objects

	Implementation of IMuonSegmentMaker interface routine 

	Will call: 

	std::vector<const MuonSegment*>* find( const Amg::Vector3D& gpos, const Amg::Vector3D& gdir,
                                               const std::vector<const MdtDriftCircleOnTrack*>& mdts,
			 		       const std::vector<const MuonClusterOnTrack*>&  clusters, 
					       bool hasPhiMeasurements, double momentum );
    */
    std::vector<const MuonSegment*>* find(const std::vector<const MdtDriftCircleOnTrack*>& mdts,
					  const std::vector<const MuonClusterOnTrack*>&  clusters);

    /** find segments starting from:
	- an estimate of the global position and direction of the particle in the chamber
	- a list of MdtDriftCircleOnTrack
	- a list of MuonClusterOnTrack
	- a boolean to indicate whether the external prediction should be used to set the 
	  $\phi$-direction of the segment
        - an estimate of the momentum of the particle
	
	The global direction is used to perform a seeded search for segments. 
    */
    std::vector<const MuonSegment*>* find( const Amg::Vector3D& gpos, const Amg::Vector3D& gdir,
					   const std::vector<const MdtDriftCircleOnTrack*>& mdts,
					   const std::vector<const MuonClusterOnTrack*>&  clusters, 
					   bool hasPhiMeasurements = false, double momentum = 1e9 );

    /** find segments starting from:
	- a track prediction
	- a list of MdtDriftCircleOnTrack objects in multiple chambers, sorted by chamber 
	- a list of MuonClusterOnTrack objects in multiple chambers, sorted by chamber

	Implementation of IMuonSegmentMaker interface routine 

	Will call:

	std::vector<const MuonSegment*>* find( const Amg::Vector3D& gpos, const Amg::Vector3D& gdir,
                                               const std::vector<const MdtDriftCircleOnTrack*>& mdts,
					       const std::vector<const MuonClusterOnTrack*>&  clusters, 
					       bool hasPhiMeasurements, double momentum );
    */
    std::vector<const MuonSegment*>* find( const Trk::TrackRoad& road,
					   const std::vector< std::vector< const MdtDriftCircleOnTrack* > >& mdts,
					   const std::vector< std::vector< const MuonClusterOnTrack* > >&  clusters, 
					   bool hasPhiMeasurements = false, double momentum = 1e9);       


    /** associate trigger hits to an existing segment
	- a segment 
	- a list of MuonClusterOnTrack objects
	- a flag indicating whether the eta hits should be added

	returns the new segment, owner ship passed to the caller.

	Implementation of IMuonSegmentTriggerHitAssociator interface routine 
    */
    const MuonSegment* associateTriggerHits( const MuonSegment& seg, const std::vector<const MuonClusterOnTrack*>&  clus,
					     bool includeEtaHits ) const;

  private:
    /** helper routine to print Identifers */
    std::string printSP( std::pair<double,double> resPull,const Cluster2D& spacePoint ) const;

    /** apply error scaling for low mometum tracks */
    bool   errorScalingRegion( const Identifier& id ) const;

    /** calculate error scaling factor */
    double errorScaleFactor( const Identifier& id, double curvature, bool hasPhiMeasurements ) const; 

    void calculateHoles( Identifier chid, Amg::Vector3D gpos, Amg::Vector3D gdir, bool hasMeasuredCoordinate ) const;

    TrkDriftCircleMath::DCVec createDCVec( const std::vector<const MdtDriftCircleOnTrack*>& mdts, double errorScale ) const;
    ClusterVec create1DClusters( const std::vector<const MuonClusterOnTrack*>& clusters ) const;
    ClusterVec create2DClusters( const std::vector<const MuonClusterOnTrack*>& clusters ) const;
    
    void handleChamber( IdHitMap& gasGapHitMap ) const;
    ClusterVec createSpacePoints( ChIdHitMap& chIdHitMap ) const;
    ClusterVec createSpacePoints( IdHitMap& gasGapHitMap ) const;
    Cluster2D createSpacePoint( const Identifier& gasGapId, const MuonClusterOnTrack* etaHit, const MuonClusterOnTrack* phiHit ) const;
    Cluster2D createRpcSpacePoint( const Identifier& gasGapId, const MuonClusterOnTrack* etaHit, const std::vector<const MuonClusterOnTrack*>& phiHits ) const;
    Cluster2D createTgcSpacePoint( const Identifier& gasGapId, const MuonClusterOnTrack* etaHit, const MuonClusterOnTrack* phiHit ) const;
    TrkDriftCircleMath::CLVec createClusterVec( const std::vector<const MuonClusterOnTrack*>& clusters, const Identifier& chid ) const;
    
    void associateMDTsToSegment( const Amg::Vector3D& gdir, TrkDriftCircleMath::Segment& segment, 
				 const std::vector<const MdtDriftCircleOnTrack*>& mdts ) const;
    std::pair<int,int> associateClustersToSegment( const TrkDriftCircleMath::Segment& segment, const Identifier& chid ) const;
    DataVector<const Trk::MeasurementBase>* createROTVec() const;

    double distanceToSegment( const TrkDriftCircleMath::Segment& segment, const Amg::Vector3D& hitPos ) const;
    std::pair<double,double> residualAndPullWithSegment( const TrkDriftCircleMath::Segment& segment, const Cluster2D& spacePoint ) const; 

    TrkDriftCircleMath::MdtChamberGeometry createChamberGeometry( const Identifier& chid, const Amg::Transform3D& gToStation ) const;

    const MdtDriftCircleOnTrack* findFirstRotInChamberWithMostHits( const std::vector<const MdtDriftCircleOnTrack*>& mdts ) const;

    bool updateSegmentPhi( const Amg::Vector3D& gpos, const Amg::Vector3D& gdir, 
			   Amg::Vector2D& segLocPos, Trk::LocalDirection& segLocDir, 
			   Trk::PlaneSurface& surf, const std::vector<const Trk::MeasurementBase*>& rots ) const;
    
    /** rotate the angle XZ until all hits are in bounds. 
	returns a pair of bool, double, the bool is false if the code did not update the dXdZ. the double is the updated dXdZ */
    std::pair<bool,double> rotateLocalAngleXZIntoBounds( double xline, double zline, double dXdZ, std::vector<HitInXZ>& hits ) const;

    /** check whether all hits are in bounds in the XZ plane */
    bool checkBoundsInXZ( double xline, double zline, double dXdZ, std::vector<HitInXZ>& hits ) const;

    /** calculate positions of tube ends */
    TubeEnds localTubeEnds( const MdtDriftCircleOnTrack& mdt, const Amg::Transform3D& gToSegment, const Amg::Transform3D& segmentToG ) const;
    
    /** update phi ranges */
    void updatePhiRanges( double phiminNew, double phimaxNew, double& phiminRef, double& phimaxRef ) const;

    /** check whether phi is consistent with segment phi */
    bool checkPhiConsistency( double phi ) const;

    /** update the global direction, keeping the phi of the input road direction but using the local angle YZ */
    Amg::Vector3D updateDirection( double linephi, const Trk::PlaneSurface& surf, 
				   const Amg::Vector3D& roaddir, bool isCurvedSegment ) const;


    void addEtaHits( std::vector<const MuonClusterOnTrack*>& clusters ) const;


    MuonSegment* createSegment( TrkDriftCircleMath::Segment& segment, const Identifier& chid,
				const Amg::Vector3D& roadpos, const Amg::Vector3D& roaddir2,
				const std::vector<const MdtDriftCircleOnTrack*>& mdts, 
				bool hasPhiMeasurements ) const;
    
    const MdtPrepData* findMdt( const Identifier& id ) const;


    /** pointers to IdHelpers */
    const MuonGM::MuonDetectorManager* m_detMgr;       //<! pointer to detector manager
    ServiceHandle<MuonStationIntersectSvc> m_intersectSvc; //<! pointer to hole search service

    ToolHandle<IMdtDriftCircleOnTrackCreator> m_mdtCreator;         //<! mdt rio ontrack creator
    ToolHandle<IMdtDriftCircleOnTrackCreator> m_mdtCreatorT0;       //<! mdt rio ontrack creator
    ToolHandle<IMuonClusterOnTrackCreator>    m_clusterCreator;     //<! cluster rio ontrack creator
    ToolHandle<IMuonCompetingClustersOnTrackCreator> m_compClusterCreator;   //<! competing clusters rio ontrack creator
    ToolHandle<MuonIdHelperTool>              m_idHelperTool;    //<! Id helper tool
    ToolHandle<MuonEDMPrinterTool>            m_printer;         //<! printer helper tool
    ToolHandle<MuonEDMHelperTool>             m_helper;          //<! printer helper tool
    ToolHandle<IMdtSegmentFinder>             m_segmentFinder;   //<! segment finder tool
    ToolHandle<AdjT0::IAdjustableT0Tool>      m_tofTool;         //<! tof tool
    ToolHandle<IMuonSegmentFittingTool>       m_segmentFitter;   //<! segment fitting tool
    ToolHandle<IMuonSegmentSelectionTool>     m_segmentSelectionTool; //<! segment selection tool
    ToolHandle<IDCSLFitProvider>              m_dcslFitProvider;

    double m_sinAngleCut;        //<! cut on the angle between the segment and the prediction
    bool m_debugSpacePoints; //<! additional debug output for space point formation
    bool m_doGeometry; //<! use chamber geometry in segment finding
    bool m_curvedErrorScaling; //<! rescale errors for low momenta
    bool m_doSpacePoints;      //<! use cluster space points for association
    bool m_createCompetingROTsEta;  //<! create competing ROTs for the clusters
    bool m_createCompetingROTsPhi;  //<! create competing ROTs for the clusters
    bool m_refitParameters;      //<! refit segment if there are sufficient phi hits and update the segment parameters
    bool m_addUnassociatedPhiHits; //<! if there are phi hits without associated eta hit add them to segment
    bool m_strictRoadDirectionConsistencyCheck; //<! check if direction of road is consistent with IP (default: true), should be off for cosmics
    double m_maxAssociateClusterDistance; //<! maximum distance for clusters to be associated to segment (default: 3000 (mm))
    bool m_allMdtHoles; //<! add all mdt holes without bound checks
    bool m_removeDeltas; //<! do not add delta electrons to MuonSegmentQuality::holes
    bool m_reject1DTgcSpacePoints; //<! remove 1D tgc space points
    bool m_usePreciseError;
    bool m_outputFittedT0;
    double m_preciseErrorScale;
    bool m_doTimeOutChecks; 
    bool m_recoverBadRpcCabling;
    bool m_updatePhiUsingPhiHits;
    bool m_assumePointingPhi;
    bool m_redo2DFit;

    mutable ClusterVec m_spVec;  //<! vector to store space points
    mutable ClusterVec m_phiVec; //<! vector to store phi clusters not used to create space points

    mutable TrkDriftCircleMath::MdtMultiChamberGeometry* m_multiGeo; //<! pointer to MDT chamber geometry
    mutable TrkDriftCircleMath::DCStatistics             m_dcStatistics; //<! cache statistics on chamber occupancy
    mutable TrkDriftCircleMath::DCSLFitter* m_dcslFitter; //<! pointer to selected fitter
    mutable TrkDriftCircleMath::DCSLFitter  m_defaultFitter; //<! default fitter
    mutable TrkDriftCircleMath::DCSLHitSelector  m_hitSelector; //<! hit selector

    mutable double m_phimin;
    mutable double m_phimax;
    mutable std::set<Identifier> m_deltaVec;      //<! list of tubes with a delta electron
    mutable std::set<Identifier> m_outoftimeVec;  //<! list of tubes with a out-of-time hit
    mutable std::vector<Identifier> m_holeVec;       //<! list of tubes with a hole
    mutable std::vector<std::pair<double,const Trk::MeasurementBase*> > m_rioDistVec; //<! vector to store the distance of a ROT to the segment
    
    mutable bool           m_refit;        //<! if true the segment will be refitted with a 3D fit
    mutable bool           m_isEndcap;     //<! are we in the barrel or endcap?
    mutable Amg::Transform3D m_gToStation;   //<! global to station transform
    mutable Amg::Transform3D m_amdbToGlobal; //<! station to global transform
    mutable std::vector<const Trk::MeasurementBase*> m_measurementsToBeDeleted;
    mutable unsigned int m_nmultipleHitWarnings;
  };
  
}
#endif 

