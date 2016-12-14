/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEEDEDSEGMENTFINDER_H
#define MUON_MUONSEEDEDSEGMENTFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "MuonRecToolInterfaces/IMuonSeededSegmentFinder.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkGeometry/MagneticFieldMode.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"

#include "TrkParameters/TrackParameters.h"

#include <string>
#include <set>
#include <vector>

class MsgStream;
class StoreGateSvc;

class RpcIdHelper;
class MdtIdHelper;
class CscIdHelper;
class TgcIdHelper;

namespace Muon {
  class IMdtDriftCircleOnTrackCreator;
  class MuonIdHelperTool;
  class MuonEDMPrinterTool;
  class MdtDriftCircleOnTrack;
  class MuonSegment;
  class IMuonSegmentMaker;
  class MdtPrepData;
}

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Trk {
  class IPropagator;
}

namespace Muon {

  /**
     @brief seeded segment finder that gets its data directly from storegate for a give set of Identifiers or IdentifierHashes

  */
  class MuonSeededSegmentFinder : virtual public IMuonSeededSegmentFinder, public AthAlgTool {
  public:
    /** @brief constructor */
    MuonSeededSegmentFinder(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    ~MuonSeededSegmentFinder ();
    
    /** @brief AlgTool initilize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();
    
    /** @brief find segments in a set of chambers starting from seeding TrackParameters */
    std::vector<const MuonSegment*>* find( const Trk::TrackParameters& pars, const std::set<Identifier>& chIds ) const;
    
    /** @brief find segments in a set of chambers starting from seeding TrackParameters (version with Hashes) */
    std::vector<const MuonSegment*>* find( const Trk::TrackParameters& pars, const std::set<IdentifierHash>& chIdHs ) const;

    /** @brief find segments in a set of MdtPrepData starting from seeding TrackParameters */
    std::vector<const MuonSegment*>* find( const Trk::TrackParameters& pars, const std::vector<const MdtPrepData*>& mdtPrds ) const;


    /** @brief retrieve MDT PRD collections for the given hashes */
    void extractMdtPrdCols( const std::set<IdentifierHash>& chIdHs, std::vector<const MdtPrepDataCollection*>& target  ) const;

    /** @brief retrieve RPC PRD collections for the given hashes */
    void extractRpcPrdCols( const std::set<IdentifierHash>& chIdHs, std::vector<const RpcPrepDataCollection*>& target ) const;

    /** @brief retrieve TGC PRD collections for the given hashes */
    void extractTgcPrdCols( const std::set<IdentifierHash>& chIdHs, std::vector<const TgcPrepDataCollection*>& target ) const;

    /** @brief retrieve CSC PRD collections for the given hashes */
    void extractCscPrdCols( const std::set<IdentifierHash>& chIdHs, std::vector<const CscPrepDataCollection*>& target ) const;

  private:
    /** @brief retrieve the MdtPrepDataCollections for the give Identifiers */
    std::vector<const MdtPrepData*> extractPrds( const std::set<Identifier>& chIds ) const;

    /** @brief retrieve the MdtPrepDataCollections for the give Identifier Hashes */
    std::vector<const MdtPrepData*> extractPrds( const std::set<IdentifierHash>& chIdHs ) const;

    /** @brief select a set of Mdt hits and calibrate them */ 
    void selectAndCalibrate( const Trk::TrackParameters& pars, 
			     const std::vector<const MdtPrepData*>& mdtPrdCols,
			     std::vector<const MdtDriftCircleOnTrack*>& mdtROTs,
			     bool& doHoleSearch ) const;

    /** @brief select and calibrate a single MdtPrepData */
    const MdtDriftCircleOnTrack* handleMdtPrd( const Trk::TrackParameters& pars, 
					       const MdtPrepData& mdtPrd,
					       bool& doHoleSearch ) const;


    const MuonGM::MuonDetectorManager*  m_detMgr;

    ToolHandle<Muon::IMuonSegmentMaker>              m_segMaker;           //!< actual segment maker with hole search
    ToolHandle<Muon::IMuonSegmentMaker>              m_segMakerNoHoles;    //!< actual segment maker no hole search
    ToolHandle<Trk::IPropagator>                     m_propagator;         //!< propagator
    ToolHandle<Muon::IMdtDriftCircleOnTrackCreator>  m_mdtRotCreator;      //!< IMdtDriftCircleOnTrackCreator
    Trk::MagneticFieldProperties                     m_magFieldProperties; //!< magnetic field properties
    ToolHandle<Muon::MuonIdHelperTool>               m_idHelper;       //!< IdHelper tool
    ToolHandle<Muon::MuonEDMPrinterTool>             m_printer;            //!< EDM printer tool

    std::string m_key_mdt;                                //!< storegate location of the MdtPrepData
    mutable const Muon::MdtPrepDataContainer* m_mdtPrdContainer;  //!< pointer to the MdtPrepData container    

    std::string m_key_rpc;                                //!< storegate location of the RpcPrepData
    mutable const Muon::RpcPrepDataContainer* m_rpcPrdContainer;  //!< pointer to the RpcPrepData container    

    std::string m_key_tgc;                                //!< storegate location of the TgcPrepData
    mutable const Muon::TgcPrepDataContainer* m_tgcPrdContainer;  //!< pointer to the TgcPrepData container    

    std::string m_key_csc;                                //!< storegate location of the CscPrepData
    mutable const Muon::CscPrepDataContainer* m_cscPrdContainer;  //!< pointer to the CscPrepData container    

    double m_adcCut;
    double m_maxSigma;
  };

}

#endif
