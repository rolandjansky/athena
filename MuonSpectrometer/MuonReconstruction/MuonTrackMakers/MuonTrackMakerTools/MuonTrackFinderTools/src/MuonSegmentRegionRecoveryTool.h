/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**@class MuonSegmentRegionRecoveryTool
 Tool to recover segments using the region selector
 This tool takes a track as input, extrapolates from the first hit to the next station
 Opens a cone around the center and tries to recover segments in the chambers involved.
 It returns a new track with a refined segment finding
 @author Giuseppe.Salamanna@cern.ch
************************************************/

#ifndef MUON_MUONSEGMENTREGIONRECOVERYTOOL_H
#define MUON_MUONSEGMENTREGIONRECOVERYTOOL_H

#include "MuonRecToolInterfaces/IMuonHoleRecoveryTool.h"
#include "MuonRecToolInterfaces/IMuonTrackSegmentMatchingTool.h"
#include "MuonChamberHoleRecoveryTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkGeometry/TrackingVolume.h" 
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkTrack/Track.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonIdHelpers/MuonStationIndex.h"
#include "IRegionSelector/RegSelEnums.h"

#include "MuonTrackMakerUtils/TrackStateOnSurfaceComparisonFunction.h"

#include <string>
#include <set>
#include <vector>
#include <map>

class MsgStream;

class MdtIdHelper;
class RpcIdHelper;
class TgcIdHelper;
class CscIdHelper;

class IRegSelSvc;
class IRoiDescriptor;
class ITrackingGeometrySvc;
class MuonStationIntersectSvc;

namespace Muon {
  class IMdtDriftCircleOnTrackCreator;
  class IMuonClusterOnTrackCreator;
  class IMuonSeededSegmentFinder;
  class MuonIdHelperTool;
  class MuonEDMHelperTool;
  class MuonEDMPrinterTool;
  class MuonChamberHoleRecoveryTool;
  class IMuonHitSummaryTool;
  class IMuonTrackSegmentMatchingTool;
}

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Trk {
  class Track;
  class TrkDetElementBase;
  class MeasurementBase;
  class TrackStateOnSurface;
  class IExtrapolator;
  class INavigator;    
  class ITrackHoleSearchTool;
  class IResidualPullCalculator;
  class TrackStateOnSurface;
  class ITrackFitter;
}

namespace Muon {

  /**
     @brief Implementation of an IMuonHoleRecoveryTool

     It propagates the track to a chamber where no segments where found and runs the SeededSegmentFinder in a region selected by the region selector
  */
  
  class MuonSegmentRegionRecoveryTool : virtual public IMuonHoleRecoveryTool, public AthAlgTool {
  public:
    typedef std::vector<const Trk::MeasurementBase*> MeasVec;
    struct MuonData {
      std::set<IdentifierHash> mdt;
      std::map<MuonStationIndex::ChIndex, std::set<IdentifierHash> > mdtPerStation;
      std::set<IdentifierHash> mdtTrack;
      std::vector<const MdtPrepDataCollection*> mdtCols;
      std::set<IdentifierHash> rpc;
      std::set<IdentifierHash> rpcTrack;
      std::vector<const RpcPrepDataCollection*> rpcCols;
      std::set<IdentifierHash> tgc;
      std::set<IdentifierHash> tgcTrack;
      std::vector<const TgcPrepDataCollection*> tgcCols;
      std::set<IdentifierHash> csc;
      std::set<IdentifierHash> cscTrack;
      std::vector<const CscPrepDataCollection*> cscCols;
    };
  public:
    /** @brief constructor */
    MuonSegmentRegionRecoveryTool(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    virtual ~MuonSegmentRegionRecoveryTool ();
    
    /** @brief AlgTool initialize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();
    
    /** @brief returns a new track with segments recovered using RegionSelector*/
    Trk::Track* recover( const Trk::Track& track ) const;
    
  
  private:

    /** @brief returns a new track with segments recovered using RegionSelector, actual implementation*/
    Trk::Track* recoverImp( const Trk::Track& track ) const;

    const Trk::TrackParameters* reachableDetEl( const Trk::Track& track, const Trk::TrkDetElementBase& detEl, bool smallerBounds = false ) const;
    /** methods used by recover*/


    //Collect hashes through extrapolation
    void collectCrossedChambers( const Trk::Track& track, MuonData& data ) const;

    //Fill already on track chamber std::set
    void fillOnTrackChambers ( const Trk::Track& theTrack, MuonData& data ) const; 
    //Select hashes of chambers not yet on track
    const Trk::Track* addMissingChambers( const Trk::Track& track, MuonData& data, bool addMdt ) const;

    void addHashes( DETID type, const IRoiDescriptor& roi,
		    std::set<IdentifierHash>& hashes, const std::set<IdentifierHash>& exclusion ) const;

    const Trk::Track* findHoles( const Trk::Track& track, MuonData& data ) const;
    
    const MuonGM::MuonDetectorManager*  m_detMgr;
    
    ToolHandle<IMuonSeededSegmentFinder>       m_seededSegmentFinder;  //!< seeded segment finder
    ToolHandle<IMuonTrackSegmentMatchingTool>  m_trackSegmentMatchingTool; 
    ToolHandle<MuonChamberHoleRecoveryTool>    m_chamberHoleRecoveryTool;  //!< hit-based hole search
    ToolHandle<Trk::IExtrapolator>             m_extrapolator;         //!< extrapolator
    ToolHandle<Trk::ITrackFitter>              m_fitter;               //!< ITrackFitter
    ServiceHandle<MuonStationIntersectSvc>     m_intersectSvc;
    ToolHandle<MuonIdHelperTool>               m_idHelperTool;         //!< IdHelper tool
    ToolHandle<MuonEDMHelperTool>              m_helperTool;           //!< EDM Helper tool
    ToolHandle<MuonEDMPrinterTool>             m_printer;              //!< EDM printer tool
    ToolHandle<IMuonHitSummaryTool>            m_hitSummaryTool;       //!< hit summary tool
    ServiceHandle<IRegSelSvc>                  m_regionSelector;       //!< The region selector

    //properties
    double m_deta;
    double m_dphi;
    bool m_excludeEES;
    bool m_onlyEO;
    bool m_useFitterOutlierLogic;
  };
}

#endif
