/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONCHAMBERHOLERECOVERYTOOL_H
#define MUON_MUONCHAMBERHOLERECOVERYTOOL_H

#include "MuonRecToolInterfaces/IMuonHoleRecoveryTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonCondData/MdtCondDbData.h"
#include "TrkTrack/Track.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonStationIntersectSvc/MuonStationIntersectSvc.h"

#include <string>
#include <set>
#include <vector>

static const InterfaceID IID_MuonChamberHoleRecoveryTool("Muon::MuonChamberHoleRecoveryTool",1,0);

namespace Trk {
  class Track;
}

namespace Muon {

  /**
     @brief tool to select tracks

  */
  class MuonChamberHoleRecoveryTool : virtual public IMuonHoleRecoveryTool, public AthAlgTool {
  public:

    /** @brief vector of cluster holes in a given chamber. The first entry of the pair is the gas gap index, the second measuresPhi */
   typedef std::vector< std::pair<int,int> > LayerHoleVec;

  public:
    MuonChamberHoleRecoveryTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MuonChamberHoleRecoveryTool()=default;
    
    StatusCode initialize();

    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuonChamberHoleRecoveryTool; }
    
    /** @brief calculate holes in a given chamber using local straight line extrapolation
	@param pars TrackParameters in the chamber
	@param chId Identifier of the chamber
	@param tubeIds set containing the Identifier of the hits that should not be counted as holes
	@return a vector of hole Identifiers
    */
    std::set<Identifier> holesInMdtChamber( const Amg::Vector3D& position, const Amg::Vector3D& direction, 
					    const Identifier& chId, const std::set<Identifier>& tubeIds ) const;
    
    /** @brief returns a new track with holes recovered */
    Trk::Track* recover( const Trk::Track& track ) const;
    
    // made public
    void createHoleTSOSsForClusterChamber( const Identifier& detElId, const Trk::TrackParameters& pars,
					   std::set<Identifier>& layIds, 
					   std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& states ) const;
    
  private:

    /** @brief returns a new track with holes recovered, actual implementation */
    Trk::Track* recoverTrack( const Trk::Track& track ) const;


    /** @brief Find missing layers in a given detector element. Look whether there is PrepData compatible with the track, if so add it. 
	@param tsit     iterator pointing to the current TSOS
	@param tsis_end iterator pointing to the end of the container
	@param states   vector to hold the TSOS of the hits in this detector element + new holes/hits
	@return an iterator pointing to the last hit in the chamber
    */
    std::vector<const Trk::TrackStateOnSurface*>::const_iterator 
      insertClustersWithHoleSearch( std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsit,
				    std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsit_end,
				    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& newStates ) const;


    /** @brief Find holes in a given MDT chamber. Look whether there is MdtPrepData in the hole-tubes, if so add them. 
	@param tsit     iterator pointing to the current TSOS
	@param tsis_end iterator pointing to the end of the container
	@param states   vector to hold the TSOS of the hits in this chamber + new holes/hits
	@return an iterator pointing to the last MDT hit in the chamber
    */
    std::vector<const Trk::TrackStateOnSurface*>::const_iterator 
      insertMdtsWithHoleSearch( std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsit,
				std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsit_end,
				std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& newStates, std::set<MuonStationIndex::ChIndex> chamberLayersOnTrack ) const;


    // ----- create holes functions per technology ------ //
    
    void createHoleTSOSsForMdtChamber( const Identifier& chId, const Trk::TrackParameters& pars,
				       const Trk::TrackParameters* parsLast,
				       std::set<Identifier>& ids, 
				       std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& states ) const;

    void createHoleTSOSsForCscChamber( const Identifier& detElId, const Trk::TrackParameters& pars,
				       std::set<Identifier>& layIds, 
				       std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& states ) const;

    void createHoleTSOSsForTgcChamber( const Identifier& detElId, const Trk::TrackParameters& pars,
				       std::set<Identifier>& layIds, 
				       std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& states ) const;
    
    void createHoleTSOSsForRpcChamber( const Identifier& detElId, const Trk::TrackParameters& pars,
				       std::set<Identifier>& layIds, 
				       std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& states ) const;
    
    // New Small Wheel
    void createHoleTSOSsForStgcChamber( const Identifier& detElId, const Trk::TrackParameters& pars,
               std::set<Identifier>& layIds, 
               std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& states ) const;

    void createHoleTSOSsForMmChamber( const Identifier& detElId, const Trk::TrackParameters& pars,
 				       std::set<Identifier>& layIds, 
 				       std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& states ) const;
               

    void createHoleTSOSsForClusterChamber( const Identifier& detElId, const Trk::TrackParameters& pars,
					   std::set<Identifier>& layIds, std::set<Identifier>& chHoles, 
					   const std::vector<const MuonCluster*>& prds,
					   std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& states ) const;
    
    LayerHoleVec holesInClusterChamber( const Trk::TrackParameters& pars, const Identifier& detElId,
					const std::set<Identifier>& layIds, unsigned nGasGaps ) const;


    // get collections
    const MdtPrepDataCollection* findMdtPrdCollection( const Identifier& chId ) const;
    const CscPrepDataCollection* findCscPrdCollection( const Identifier& detElId ) const;
    const TgcPrepDataCollection* findTgcPrdCollection( const Identifier& detElId ) const;
    const RpcPrepDataCollection* findRpcPrdCollection( const Identifier& detElId ) const;
    const sTgcPrepDataCollection* findStgcPrdCollection( const Identifier& detElId ) const;
    const MMPrepDataCollection* findMmPrdCollection( const Identifier& detElId ) const;


    ServiceHandle<MuonStationIntersectSvc> m_intersectSvc {this, "MuonStationIntersectSvc", "MuonStationIntersectSvc", "pointer to hole search service"};
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc {this, "edmHelper", "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", "Handle to the service providing the IMuonEDMHelperSvc interface" };
    
    ToolHandle<Muon::MuonEDMPrinterTool> m_printer {this, "EDMPrinter", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};
    ToolHandle<Trk::IExtrapolator> m_extrapolator {this, "Extrapolator", "Trk::Extrapolator/MuonExtrapolator"};
    ToolHandle<Muon::IMdtDriftCircleOnTrackCreator> m_mdtRotCreator {this, "MdtRotCreator", "Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator", "IMdtDriftCircleOnTrackCreator full calibration"};
    ToolHandle<Muon::IMuonClusterOnTrackCreator> m_cscRotCreator {this, "CscRotCreator", "Muon::CscClusterOnTrackCreator/CscClusterOnTrackCreator", "IMuonClusterOnTrackCreator for cscs"};
    ToolHandle<Muon::IMuonClusterOnTrackCreator> m_clusRotCreator {this, "ClusterRotCreator", "Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator", "IMuonClusterOnTrackCreator for trigger hits"};
    ToolHandle<Trk::IResidualPullCalculator> m_pullCalculator {this, "PullCalculator", "Trk::ResidualPullCalculator/ResidualPullCalculator"};

    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
	"MuonDetectorManager", 
	"Key of input MuonDetectorManager condition data"};    

    SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_key_mdt{this,"MdtPrepDataContainer","MDT_DriftCircles","MDT PRDs"};
    SG::ReadHandleKey<Muon::CscPrepDataContainer> m_key_csc{this,"CscPrepDataContainer","CSC_Clusters","CSC PRDS"};
    SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_key_tgc{this,"TgcPrepDataContainer","TGC_Measurements","TGC PRDs"};
    SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_key_rpc{this,"RpcPrepDataContainer","RPC_Measurements","RPC PRDs"};
    SG::ReadHandleKey<Muon::sTgcPrepDataContainer> m_key_stgc{this,"sTgcPrepDataContainer","STGC_Measurements","sTGC PRDs"};
    SG::ReadHandleKey<Muon::MMPrepDataContainer> m_key_mm{this,"MMPrepDataContainer","MM_Measurements","MM PRDs"};
    SG::ReadCondHandleKey<MdtCondDbData> m_condKey{this, "MdtCondKey", "MdtCondDbData", "Key of MdtCondDbData"};

    Gaudi::Property<bool> m_addMeasurements {this, "AddMeasurements", true};
    Gaudi::Property<bool> m_detectBadSort {this, "DetectBadSorting", false};

    Gaudi::Property<double> m_associationPullCutEta {this, "AssociationPullCutEta", 3};
    Gaudi::Property<double> m_associationPullCutPhi {this, "AssociationPullCutPhi", 10};
    Gaudi::Property<double> m_adcCut {this, "AdcCut", 50};
  };

}

#endif
