/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONIDHELPERTOOL_H
#define MUON_MUONIDHELPERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include <string>

static const InterfaceID IID_MuonIdHelperTool("Muon::MuonIdHelperTool",1,0);

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include "MuonIdHelpers/MuonStationIndex.h"

#include "Identifier/Identifier.h"
class MsgStream;

namespace Muon {

  /**
     @brief Helper tool that acts as a proxy to IMuonIdHelperSvc to facilitate migration from public tool to service

  */
  class MuonIdHelperTool : public AthAlgTool {
  public:
    /** @brief constructor */
    MuonIdHelperTool(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    ~MuonIdHelperTool ();
    
    /** @brief AlgTool initilize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();
    
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuonIdHelperTool; }

    /** @brief print all fields to string */
    std::string toString( const Identifier& id ) const { return m_muonIdHelperSvc->toString(id); }

    /** @brief print all fields up to technology to string */
    std::string toStringTech( const Identifier& id ) const { return m_muonIdHelperSvc->toStringTech(id); }

    /** @brief print all fields up to stationName to string */
    std::string toStringStation( const Identifier& id ) const { return m_muonIdHelperSvc->toStringStation(id); }

    /** @brief print all fields up to chamber to string */
    std::string toStringChamber( const Identifier& id ) const { return m_muonIdHelperSvc->toStringChamber(id); }

    /** @brief print all fields up to detector element to string */
    std::string toStringDetEl( const Identifier& id ) const { return m_muonIdHelperSvc->toStringDetEl(id); }

    /** @brief print all fields up to gas gap to string */
    std::string toStringGasGap( const Identifier& id ) const { return m_muonIdHelperSvc->toStringGasGap(id); }

    /** @brief print chamber name to string */
    std::string chamberNameString( const Identifier& id ) const { return m_muonIdHelperSvc->chamberNameString(id); }

    /** @brief returns whether this is a Muon Identifier or not */
    bool isMuon( const Identifier& id ) const { return m_muonIdHelperSvc->isMuon(id); }

    /** @brief returns whether this is a MDT Identifier or not */
    bool isMdt( const Identifier& id ) const { return m_muonIdHelperSvc->isMdt(id); }

    /** @brief returns whether this is a MM Identifier or not */
    bool isMM( const Identifier& id ) const { return m_muonIdHelperSvc->isMM(id); }

    /** @brief returns whether this is a CSC Identifier or not */
    bool isCsc( const Identifier& id ) const { return m_muonIdHelperSvc->isCsc(id); }

    /** @brief returns whether this is a RPC Identifier or not */
    bool isRpc( const Identifier& id ) const { return m_muonIdHelperSvc->isRpc(id); }

    /** @brief returns whether this is a TGC Identifier or not */
    bool isTgc( const Identifier& id ) const { return m_muonIdHelperSvc->isTgc(id); }

    /** @brief returns whether this is a sTGC Identifier or not */
    bool issTgc( const Identifier& id ) const { return m_muonIdHelperSvc->issTgc(id); }

    /** @brief returns whether channel measures phi or not */
    bool measuresPhi( const Identifier& id ) const { return m_muonIdHelperSvc->measuresPhi(id); }
    
    /** @brief returns whether this is an endcap Identifier or not */
    bool isEndcap( const Identifier& id ) const { return m_muonIdHelperSvc->isEndcap(id); }

    /** @brief returns whether trigger chamber id or not */
    bool isTrigger( const Identifier& id ) const { return m_muonIdHelperSvc->isTrigger(id); }

    /** @brief returns whether this is a small chamber, always returns true for TGCs */
    bool isSmallChamber( const Identifier& id ) const { return m_muonIdHelperSvc->isSmallChamber(id); }

    /** @brief returns gas gap: gasGap for RPC + TGC, wireLayer for CSC, tube for MDT */
    int gasGap( const Identifier& id ) const { return m_muonIdHelperSvc->gasGap(id); }

    /** @brief create a chamber ID */
    Identifier chamberId( const Identifier& id) const { return m_muonIdHelperSvc->chamberId(id); }

    /** @brief create a detector element ID */
    Identifier detElId( const Identifier& id) const { return m_muonIdHelperSvc->detElId(id); }

    /** @brief create a gasGap ID (will return layer Id for MDTs) */
    Identifier gasGapId( const Identifier& id) const { return m_muonIdHelperSvc->gasGapId(id); }

    /** @brief create a layer ID, returns tube id for the MDTs */
    Identifier layerId( const Identifier& id) const { return m_muonIdHelperSvc->layerId(id); }

    /** @brief access to MdtIdHelper */
    const MdtIdHelper& mdtIdHelper() const { return m_muonIdHelperSvc->mdtIdHelper(); }

    /** @brief Test presence of MdtIdHelper */
    bool hasMdtIdHelper() const { return m_mdtIdHelper != nullptr; }

    /** @brief access to RpcIdHelper */
    const RpcIdHelper& rpcIdHelper() const { return m_muonIdHelperSvc->rpcIdHelper(); }

    /** @brief Test presence of RpcIdHelper */
    bool hasRpcIdHelper() const { return m_rpcIdHelper != nullptr; }

    /** @brief access to TgcIdHelper */
    const TgcIdHelper& tgcIdHelper() const { return m_muonIdHelperSvc->tgcIdHelper(); }

    /** @brief Test presence of TgcIdHelper */
    bool hasTgcIdHelper() const { return m_tgcIdHelper != nullptr; }

    /** @brief access to CscIdHelper */
    const CscIdHelper& cscIdHelper() const { return m_muonIdHelperSvc->cscIdHelper(); }

    /** @brief Test presence of CscIdHelper */
    bool hasCscIdHelper() const { return m_cscIdHelper != nullptr; }

    /** @brief access to TgcIdHelper */
    const sTgcIdHelper& stgcIdHelper() const { return m_muonIdHelperSvc->stgcIdHelper(); }

    /** @brief Test presence of sTgcIdHelper */
    bool hassTgcIdHelper() const { return m_stgcIdHelper != nullptr; }

    /** @brief access to CscIdHelper */
    const MmIdHelper& mmIdHelper() const { return m_muonIdHelperSvc->mmIdHelper(); }

    /** @brief Test presence of sTgcIdHelper */
    bool hasMmIdHelper() const { return m_mmIdHelper != nullptr; }

    /** @brief calculate chamber index from Identifier */
    MuonStationIndex::ChIndex chamberIndex( const Identifier& id ) const { return m_muonIdHelperSvc->chamberIndex(id); }

    /** @brief calculate station index from Identifier */
    MuonStationIndex::StIndex stationIndex( const Identifier& id ) const { return m_muonIdHelperSvc->stationIndex(id); }

    /** @brief calculate phi index from Identifier (not supported for MDT hits) */
    MuonStationIndex::PhiIndex phiIndex( const Identifier& id ) const { return m_muonIdHelperSvc->phiIndex(id); }

    /** @brief calculate detector region index from Identifier */
    MuonStationIndex::DetectorRegionIndex regionIndex( const Identifier& id ) const { return m_muonIdHelperSvc->regionIndex(id); }

    /** @brief calculate layer index from Identifier */
    MuonStationIndex::LayerIndex layerIndex( const Identifier& id ) const { return m_muonIdHelperSvc->layerIndex(id); }

    /** @brief calculate layer index from Identifier */
    MuonStationIndex::TechnologyIndex technologyIndex( const Identifier& id ) const { return m_muonIdHelperSvc->technologyIndex(id); }

    /** @brief Return stationPhi for all technologies */
    int stationPhi( const Identifier& id ) const { return m_muonIdHelperSvc->stationPhi(id); }

    /** @brief Return stationEta for all technologies */
    int stationEta( const Identifier& id ) const { return m_muonIdHelperSvc->stationEta(id); }

    /** @brief return sector number 1-16, odd=large, even=small */
    int sector( const Identifier& id ) const { return m_muonIdHelperSvc->sector(id); }

    bool hasRPC() const { return m_muonIdHelperSvc->hasRPC(); }
    bool hasTGC() const { return m_muonIdHelperSvc->hasTGC(); }
    bool hasMDT() const { return m_muonIdHelperSvc->hasMDT(); }
    bool hasCSC() const { return m_muonIdHelperSvc->hasCSC(); }
    bool hasSTgc() const { return m_muonIdHelperSvc->hasSTgc(); }
    bool hasMM() const { return m_muonIdHelperSvc->hasMM(); }

  private:
    ServiceHandle<Muon::IMuonIdHelperSvc> m_muonIdHelperSvc{this, "idHelper", 
      "Muon::MuonIdHelperSvc/MuonIdHelperSvc", "Handle to the service providing the IMuonIdHelperSvc interface"};
  };

}

#endif // MUON_IDHELPERTOOL_H
