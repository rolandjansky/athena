/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "MuonIdHelpers/MuonStationIndex.h"

#include "Identifier/Identifier.h"
class MsgStream;

namespace Muon {

  /**
     @brief Helper tool that creates muon Identifiers and can be used to print Identifiers 

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
    std::string toString( const Identifier& id ) const;

    /** @brief print all fields up to technology to string */
    std::string toStringTech( const Identifier& id ) const;

    /** @brief print all fields up to stationName to string */
    std::string toStringStation( const Identifier& id ) const;

    /** @brief print all fields up to chamber to string */
    std::string toStringChamber( const Identifier& id ) const;

    /** @brief print all fields up to detector element to string */
    std::string toStringDetEl( const Identifier& id ) const;

    /** @brief print all fields up to gas gap to string */
    std::string toStringGasGap( const Identifier& id ) const;

    /** @brief print chamber name to string */
    std::string chamberNameString( const Identifier& id ) const;

    /** @brief returns whether this is a Muon Identifier or not */
    bool isMuon( const Identifier& id ) const;

    /** @brief returns whether this is a MDT Identifier or not */
    bool isMdt( const Identifier& id ) const;

    /** @brief returns whether this is a MM Identifier or not */
    bool isMM( const Identifier& id ) const;

    /** @brief returns whether this is a CSC Identifier or not */
    bool isCsc( const Identifier& id ) const;

    /** @brief returns whether this is a RPC Identifier or not */
    bool isRpc( const Identifier& id ) const;

    /** @brief returns whether this is a TGC Identifier or not */
    bool isTgc( const Identifier& id ) const;

    /** @brief returns whether this is a sTGC Identifier or not */
    bool issTgc( const Identifier& id ) const;

    /** @brief returns whether this is a sMDT Identifier or not */
    bool issMdt( const Identifier& id ) const;

    /** @brief returns whether this Identifier belongs to an MDT with HPTDC or not */
    bool hasHPTDC( const Identifier& id ) const;

    /** @brief returns whether channel measures phi or not */
    bool measuresPhi( const Identifier& id ) const;
    
    /** @brief returns whether this is an endcap Identifier or not */
    bool isEndcap( const Identifier& id ) const;

    /** @brief returns whether trigger chamber id or not */
    bool isTrigger( const Identifier& id ) const;

    /** @brief returns whether this is a small chamber, always returns true for TGCs */
    bool isSmallChamber( const Identifier& id ) const;

    /** @brief returns gas gap: gasGap for RPC + TGC, wireLayer for CSC, tube for MDT */
    int gasGap( const Identifier& id ) const;

    /** @brief create a chamber ID */
    Identifier chamberId( const Identifier& id) const;

    /** @brief create a detector element ID */
    Identifier detElId( const Identifier& id) const;

    /** @brief create a gasGap ID (will return layer Id for MDTs) */
    Identifier gasGapId( const Identifier& id) const;

    /** @brief create a layer ID, returns tube id for the MDTs */
    Identifier layerId( const Identifier& id) const;

    /** @brief access to MdtIdHelper */
    const MdtIdHelper& mdtIdHelper() const { return *m_mdtIdHelper; }

    /** @brief access to RpcIdHelper */
    const RpcIdHelper& rpcIdHelper() const { return *m_rpcIdHelper; }

    /** @brief access to TgcIdHelper */
    const TgcIdHelper& tgcIdHelper() const { return *m_tgcIdHelper; }

    /** @brief access to CscIdHelper */
    const CscIdHelper& cscIdHelper() const { return *m_cscIdHelper; }

    /** @brief access to TgcIdHelper */
    const sTgcIdHelper& stgcIdHelper() const { return *m_stgcIdHelper; }

    /** @brief access to CscIdHelper */
    const MmIdHelper& mmIdHelper() const { return *m_mmIdHelper; }

    /** @brief calculate chamber index from Identifier */
    MuonStationIndex::ChIndex chamberIndex( const Identifier& id ) const;

    /** @brief calculate station index from Identifier */
    MuonStationIndex::StIndex stationIndex( const Identifier& id ) const;

    /** @brief calculate phi index from Identifier (not supported for MDT hits) */
    MuonStationIndex::PhiIndex phiIndex( const Identifier& id ) const;

    /** @brief calculate detector region index from Identifier */
    MuonStationIndex::DetectorRegionIndex regionIndex( const Identifier& id ) const;

    /** @brief calculate layer index from Identifier */
    MuonStationIndex::LayerIndex layerIndex( const Identifier& id ) const;

    /** @brief calculate layer index from Identifier */
    MuonStationIndex::TechnologyIndex technologyIndex( const Identifier& id ) const;

    /** @brief Return stationPhi for all technologies */
    int stationPhi( const Identifier& id ) const;

    /** @brief Return stationEta for all technologies */
    int stationEta( const Identifier& id ) const;

    /** @brief return sector number 1-16, odd=large, even=small */
    int sector( const Identifier& id ) const;

    bool HasCSC() const;
    bool HasSTgc() const;
    bool HasMM() const;

  private:
    const RpcIdHelper*                  m_rpcIdHelper;
    const TgcIdHelper*                  m_tgcIdHelper;
    const CscIdHelper*                  m_cscIdHelper;
    const MdtIdHelper*                  m_mdtIdHelper;
    const MmIdHelper*                   m_mmIdHelper;
    const sTgcIdHelper*                 m_stgcIdHelper;
    bool                                m_hasCSC;
    bool                                m_hasSTgc;
    bool                                m_hasMM;

    struct StationNameData {
      std::string stationName;
      bool isEndcap;
      bool isSmall;
      MuonStationIndex::ChIndex chIndex;
      MuonStationIndex::StIndex stIndex;
    };
    std::vector<StationNameData> m_stationNameData;
    std::vector<MuonStationIndex::TechnologyIndex> m_technologies;
  };

}

#endif // MUON_IDHELPERTOOL_H
