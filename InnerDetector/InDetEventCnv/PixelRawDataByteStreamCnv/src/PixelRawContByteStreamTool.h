/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  PixelRawContByteStreamTool.h
//   Header file for class PixelRawContByteStreamTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//  Version 00-00-39 05/03/2007 Daniel Dobos
///////////////////////////////////////////////////////////////////

#ifndef PIXELRAWDATABYTESTREAMCNV_PIXELRAWCONTRAWEVENTTOOL_H
#define PIXELRAWDATABYTESTREAMCNV_PIXELRAWCONTRAWEVENTTOOL_H

// #define _DEBUG

#include <stdint.h>
#include <string>

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"

#include "ByteStreamData/RawEvent.h" 
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "ByteStreamCnvSvcBase/FullEventAssembler.h" // needed, template class

#include "PixelByteStreamModuleMask.h"
#include "PixelCabling/IPixelCablingSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "ByteStreamCnvSvcBase/SrcIdMap.h" 

#include "PixelConditionsData/PixelCablingCondData.h"
#include "PixelConditionsData/PixelHitDiscCnfgData.h"
#include "StoreGate/ReadCondHandleKey.h"

class PixelRawContByteStreamTool: public AthAlgTool {

  public:
    // constructor
    PixelRawContByteStreamTool(const std::string& type,const std::string& name,const IInterface* parent);

    // destructor 
    virtual ~PixelRawContByteStreamTool();

    // AlgTool InterfaceID
    static const InterfaceID& interfaceID();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    StatusCode convert(PixelRDO_Container* cont,RawEventWrite* re); 

    void fillROD(std::vector<uint32_t>& v32rod, std::vector<const PixelRDORawData*> RDOs, int BCs_per_LVL1ID);

    void packIBLcondensed(std::vector <uint32_t > & v32rod, std::vector <uint32_t > & vRows, std::vector <uint32_t > & vCols, std::vector<int> & vTots);
    void packIBLCondensed(std::vector <uint32_t > & v32rod, const std::vector<const PixelRDORawData*> &rdos_sameIBL_offlineId);
    uint32_t packLinkHeader(uint32_t module, uint32_t bcid, uint32_t lvl1id, uint32_t lvl1idskip, uint32_t errors);
    uint32_t packLinkHeader_IBL(uint32_t module, uint32_t bcid, uint32_t lvl1id, uint32_t feFlag);
    uint32_t packRawDataWord(uint32_t FE, uint32_t row, uint32_t column, uint32_t ToT);
    uint32_t packRawDataWord_IBL(uint32_t row, uint32_t column, int ToT, uint32_t nLink);
    uint32_t packLinkTrailer(uint32_t errors); // for Pixel
    uint32_t packLinkTrailer_IBL(uint32_t FEonSLink, bool timeOutErrorBit, bool condensedModeBit, bool linkMasked); // for IBL

  private:
    ServiceHandle<IPixelCablingSvc> m_pixelCabling;
    const PixelID* m_PixelID;

    const InDetDD::PixelDetectorManager* m_pixelManager;

    FullEventAssembler<SrcIdMap> m_fea; 
    unsigned short m_RodBlockVersion;
    int m_BCs_per_LVL1ID;

    MsgStream m_log;

    SG::ReadCondHandleKey<PixelCablingCondData> m_condCablingKey
    {this, "PixelCablingCondData", "PixelCablingCondData", "Pixel cabling key"};

    SG::ReadCondHandleKey<PixelHitDiscCnfgData> m_condHitDiscCnfgKey
    {this, "PixelHitDiscCnfgData", "PixelHitDiscCnfgData", "Pixel FEI4 HitDiscConfig key"};
};

class OrderRdos {

  private:
    ServiceHandle<IPixelCablingSvc> m_pixelCabling;
    Identifier m_offlineId;

  public:
    OrderRdos(Identifier offlineId, const ServiceHandle<IPixelCablingSvc>& pixelCabling):
      m_pixelCabling("dummy","dummy"),m_offlineId(offlineId) { m_pixelCabling=pixelCabling; }

    // copy constructor
    OrderRdos(const OrderRdos & orderFunct):
      m_pixelCabling("dummy","dummy"),m_offlineId(orderFunct.m_offlineId) { m_pixelCabling=orderFunct.m_pixelCabling; }

    // assignment operator
    OrderRdos& operator= (const OrderRdos &other) { 
      m_pixelCabling = other.m_pixelCabling;
      return *this;
    }

    bool operator() (const PixelRDORawData* rdo0, const PixelRDORawData* rdo1);
};

class OrderInitialRdos {

  private:
    ServiceHandle<IPixelCablingSvc> m_pixelCabling;
    const PixelID * m_PixelID;
    SG::ReadCondHandle<PixelCablingCondData> m_pixCabling;

  public:
    OrderInitialRdos(const ServiceHandle<IPixelCablingSvc>& pixelCabling, const PixelID *pixelID, SG::ReadCondHandle<PixelCablingCondData> &pixCabling):
      m_pixelCabling("dummy","dummy"),m_PixelID(pixelID),m_pixCabling(pixCabling) { m_pixelCabling=pixelCabling; }

    // copy constructor
    OrderInitialRdos(const OrderInitialRdos & orderFunct): 
      m_pixelCabling("dummy","dummy"),m_PixelID(orderFunct.m_PixelID),m_pixCabling(orderFunct.m_pixCabling) { m_pixelCabling=orderFunct.m_pixelCabling; }

    // assignment operator
    OrderInitialRdos& operator= (const OrderInitialRdos &other) { 
      m_pixelCabling = other.m_pixelCabling;
      return *this;
    }

    bool operator() (const PixelRDORawData* rdo0, const PixelRDORawData* rdo1);
};

#endif // PIXELRAWDATABYTESTREAMCNV_PIXELRAWCONTRAWEVENTTOOL_H
