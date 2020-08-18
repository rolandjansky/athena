/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONIDHELPERSVC_H
#define MUON_IMUONIDHELPERSVC_H

#include "GaudiKernel/IService.h"

#include <string>

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonIdHelpers/MmIdHelper.h"

#include "MuonStationIndex/MuonStationIndex.h"

#include "Identifier/Identifier.h"

namespace Muon {

  /**
     @brief Interface for Helper service that creates muon Identifiers and can be used to print Identifiers 

  */
  class IMuonIdHelperSvc : virtual public IService {
  public:
    DeclareInterfaceID(IMuonIdHelperSvc, 1, 0);

    /** @brief destructor */
    virtual ~IMuonIdHelperSvc() {}

    /** @brief print all fields to string */
    virtual std::string toString( const Identifier& id ) const = 0;

    /** @brief print all fields up to technology to string */
    virtual std::string toStringTech( const Identifier& id ) const = 0;

    /** @brief print all fields up to stationName to string */
    virtual std::string toStringStation( const Identifier& id ) const = 0;

    /** @brief print all fields up to chamber to string */
    virtual std::string toStringChamber( const Identifier& id ) const = 0;

    /** @brief print all fields up to detector element to string */
    virtual std::string toStringDetEl( const Identifier& id ) const = 0;

    /** @brief print all fields up to gas gap to string */
    virtual std::string toStringGasGap( const Identifier& id ) const = 0;

    /** @brief print chamber name to string */
    virtual std::string chamberNameString( const Identifier& id ) const = 0;

    /** @brief returns whether this is a Muon Identifier or not */
    virtual bool isMuon( const Identifier& id ) const = 0;

    /** @brief returns whether this is a MDT Identifier or not */
    virtual bool isMdt( const Identifier& id ) const = 0;

    /** @brief returns whether this is a MM Identifier or not */
    virtual bool isMM( const Identifier& id ) const = 0;

    /** @brief returns whether this is a CSC Identifier or not */
    virtual bool isCsc( const Identifier& id ) const = 0;

    /** @brief returns whether this is a RPC Identifier or not */
    virtual bool isRpc( const Identifier& id ) const = 0;

    /** @brief returns whether this is a TGC Identifier or not */
    virtual bool isTgc( const Identifier& id ) const = 0;

    /** @brief returns whether this is a sTGC Identifier or not */
    virtual bool issTgc( const Identifier& id ) const = 0;

    /** @brief returns whether this is a sMDT Identifier or not */
    virtual bool issMdt( const Identifier& id ) const = 0;

    /** @brief returns whether this Identifier belongs to an MDT with HPTDC or not */
    virtual bool hasHPTDC( const Identifier& id ) const = 0;

    /** @brief returns whether channel measures phi or not */
    virtual bool measuresPhi( const Identifier& id ) const = 0;
    
    /** @brief returns whether this is an endcap Identifier or not */
    virtual bool isEndcap( const Identifier& id ) const = 0;

    /** @brief returns whether trigger chamber id or not */
    virtual bool isTrigger( const Identifier& id ) const = 0;

    /** @brief returns whether this is a small chamber, always returns true for TGCs */
    virtual bool isSmallChamber( const Identifier& id ) const = 0;

    /** @brief returns gas gap: gasGap for RPC + TGC, wireLayer for CSC, tube for MDT */
    virtual int gasGap( const Identifier& id ) const = 0;

    /** @brief create a chamber ID */
    virtual Identifier chamberId( const Identifier& id) const = 0;

    /** @brief create a detector element ID */
    virtual Identifier detElId( const Identifier& id) const = 0;

    /** @brief create a gasGap ID (will return layer Id for MDTs) */
    virtual Identifier gasGapId( const Identifier& id) const = 0;

    /** @brief create a layer ID, returns tube id for the MDTs */
    virtual Identifier layerId( const Identifier& id) const = 0;

    /** @brief access to MdtIdHelper */
    virtual const MdtIdHelper& mdtIdHelper() const = 0;

    /** @brief access to RpcIdHelper */
    virtual const RpcIdHelper& rpcIdHelper() const = 0;

    /** @brief access to TgcIdHelper */
    virtual const TgcIdHelper& tgcIdHelper() const = 0;

    /** @brief access to CscIdHelper */
    virtual const CscIdHelper& cscIdHelper() const = 0;

    /** @brief access to TgcIdHelper */
    virtual const sTgcIdHelper& stgcIdHelper() const = 0;

    /** @brief access to CscIdHelper */
    virtual const MmIdHelper& mmIdHelper() const = 0;

    /** @brief calculate chamber index from Identifier */
    virtual MuonStationIndex::ChIndex chamberIndex( const Identifier& id ) const = 0;

    /** @brief calculate station index from Identifier */
    virtual MuonStationIndex::StIndex stationIndex( const Identifier& id ) const = 0;

    /** @brief calculate phi index from Identifier (not supported for MDT hits) */
    virtual MuonStationIndex::PhiIndex phiIndex( const Identifier& id ) const = 0;

    /** @brief calculate detector region index from Identifier */
    virtual MuonStationIndex::DetectorRegionIndex regionIndex( const Identifier& id ) const = 0;

    /** @brief calculate layer index from Identifier */
    virtual MuonStationIndex::LayerIndex layerIndex( const Identifier& id ) const = 0;

    /** @brief calculate layer index from Identifier */
    virtual MuonStationIndex::TechnologyIndex technologyIndex( const Identifier& id ) const = 0;

    /** @brief Return stationPhi for all technologies */
    virtual int stationPhi( const Identifier& id ) const = 0;

    /** @brief Return stationEta for all technologies */
    virtual int stationEta( const Identifier& id ) const = 0;

    /** @brief Return stationName for all technologies */
    virtual int stationName( const Identifier& id ) const = 0;

    /** @brief Return stationRegion for all technologies */
    virtual int stationRegion( const Identifier& id ) const = 0;

    /** @brief return sector number 1-16, odd=large, even=small */
    virtual int sector( const Identifier& id ) const = 0;

    virtual bool hasRPC() const = 0;
    virtual bool hasTGC() const = 0;
    virtual bool hasMDT() const = 0;
    virtual bool hasCSC() const = 0;
    virtual bool hasSTgc() const = 0;
    virtual bool hasMM() const = 0;
  };

}

#endif // MUON_IMUONIDHELPERSVC_H
