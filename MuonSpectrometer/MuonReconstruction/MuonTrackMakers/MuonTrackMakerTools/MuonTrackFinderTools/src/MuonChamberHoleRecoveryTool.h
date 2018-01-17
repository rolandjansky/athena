/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONCHAMBERHOLERECOVERYTOOL_H
#define MUON_MUONCHAMBERHOLERECOVERYTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Identifier/Identifier.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "MuonRecToolInterfaces/IMuonHoleRecoveryTool.h"
#include "MuonIdHelpers/MuonStationIndex.h"

#include "TrkTrack/Track.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include <string>
#include <set>
#include <vector>

static const InterfaceID IID_MuonChamberHoleRecoveryTool("Muon::MuonChamberHoleRecoveryTool",1,0);

class MsgStream;
class StoreGateSvc;

class MuonStationIntersectSvc;
class RpcIdHelper;
class MdtIdHelper;
class CscIdHelper;
class TgcIdHelper;

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Muon {
  class IMdtDriftCircleOnTrackCreator;
  class IMuonClusterOnTrackCreator;
  class MuonIdHelperTool;
  class MuonEDMHelperTool;
  class MuonEDMPrinterTool;
}

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Trk {
  class Track;
  class MagneticFieldProperties;
  class IExtrapolator;
  class IResidualPullCalculator;
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
    /** @brief constructor */
    MuonChamberHoleRecoveryTool(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    virtual ~MuonChamberHoleRecoveryTool ();
    
    /** @brief AlgTool initilize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();

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
				std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& newStates ) const;


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


    ServiceHandle<MuonStationIntersectSvc>           m_intersectSvc;      //<! pointer to hole search service
    ToolHandle<Trk::IExtrapolator>                   m_extrapolator;      //!< extrapolator
    ToolHandle<Muon::IMdtDriftCircleOnTrackCreator>  m_mdtRotCreator;     //!< IMdtDriftCircleOnTrackCreator full calibration
    ToolHandle<Muon::IMuonClusterOnTrackCreator>     m_cscRotCreator;     //!< IMuonClusterOnTrackCreator for cscs 
    ToolHandle<Muon::IMuonClusterOnTrackCreator>     m_clusRotCreator;    //!< IMuonClusterOnTrackCreator for trigger hits

    ToolHandle<Trk::IResidualPullCalculator>         m_pullCalculator;     //!< residual pull calculator
    ToolHandle<Muon::MuonIdHelperTool>               m_idHelperTool;       //!< IdHelper tool
    ToolHandle<Muon::MuonEDMHelperTool>              m_helperTool;         //!< EDM Helper tool
    ToolHandle<Muon::MuonEDMPrinterTool>             m_printer;            //!< EDM printer tool

    const MuonGM::MuonDetectorManager*  m_detMgr;

    SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_key_mdt;                                //!< storegate location of the MdtPrepData
    SG::ReadHandleKey<Muon::CscPrepDataContainer> m_key_csc;                                //!< storegate location of the CscPrepData
    SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_key_tgc;                                //!< storegate location of the TgcPrepData
    SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_key_rpc;                                //!< storegate location of the RpcPrepData


    bool m_addMeasurements;
    double m_associationPullCutEta;
    double m_associationPullCutPhi;
    bool m_detectBadSort;
    mutable std::set<MuonStationIndex::ChIndex> m_chamberLayersOnTrack;
    mutable bool m_checkForBadSort;

    double m_adcCut;
  };

}

#endif
