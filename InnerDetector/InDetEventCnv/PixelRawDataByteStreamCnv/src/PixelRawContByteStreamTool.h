/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "PixelReadoutGeometry/PixelDetectorManager.h"

#include "ByteStreamCnvSvcBase/FullEventAssembler.h" // needed, template class
#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"

#include "PixelByteStreamModuleMask.h"
#include "InDetIdentifier/PixelID.h"
#include "ByteStreamCnvSvcBase/SrcIdMap.h" 

#include "PixelConditionsData/PixelCablingCondData.h"
#include "PixelConditionsData/PixelHitDiscCnfgData.h"
#include "PixelReadoutGeometry/IPixelReadoutManager.h"
#include "StoreGate/ReadCondHandleKey.h"

class PixelRawContByteStreamTool: public AthAlgTool {

  public:
    // constructor
    PixelRawContByteStreamTool(const std::string& type,const std::string& name,const IInterface* parent);

    // destructor 
    virtual ~PixelRawContByteStreamTool();

    // AlgTool InterfaceID
    static const InterfaceID& interfaceID();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    StatusCode convert(PixelRDO_Container* cont) const;

    void fillROD(std::vector<uint32_t>& v32rod, std::vector<const PixelRDORawData*> RDOs, int BCs_per_LVL1ID) const;

    void packIBLcondensed(std::vector <uint32_t > & v32rod, std::vector <uint32_t > & vRows, std::vector <uint32_t > & vCols, std::vector<int> & vTots) const;
    void packIBLCondensed(std::vector <uint32_t > & v32rod, const std::vector<const PixelRDORawData*> &rdos_sameIBL_offlineId);
    uint32_t packLinkHeader(uint32_t module, uint32_t bcid, uint32_t lvl1id, uint32_t lvl1idskip, uint32_t errors) const;
    uint32_t packLinkHeader_IBL(uint32_t module, uint32_t bcid, uint32_t lvl1id, uint32_t feFlag) const;
    uint32_t packRawDataWord(uint32_t FE, uint32_t row, uint32_t column, uint32_t ToT) const;
    uint32_t packRawDataWord_IBL(uint32_t row, uint32_t column, int ToT, uint32_t nLink) const;
    uint32_t packLinkTrailer(uint32_t errors) const; // for Pixel
    uint32_t packLinkTrailer_IBL(uint32_t FEonSLink, bool timeOutErrorBit, bool condensedModeBit, bool linkMasked) const; // for IBL

  private:
    ServiceHandle<ByteStreamCnvSvc> m_byteStreamCnvSvc
    { this, "ByteStreamCnvSvc", "ByteStreamCnvSvc" };

    ServiceHandle<InDetDD::IPixelReadoutManager> m_pixelReadout
    { this, "PixelReadoutManager", "PixelReadoutManager", "Pixel readout manager" };

    const PixelID* m_PixelID;

    const InDetDD::PixelDetectorManager* m_pixelManager;

    unsigned short m_RodBlockVersion;
    int m_BCs_per_LVL1ID;

    SG::ReadCondHandleKey<PixelCablingCondData> m_condCablingKey
    {this, "PixelCablingCondData", "PixelCablingCondData", "Pixel cabling key"};

    SG::ReadCondHandleKey<PixelHitDiscCnfgData> m_condHitDiscCnfgKey
    {this, "PixelHitDiscCnfgData", "PixelHitDiscCnfgData", "Pixel FEI4 HitDiscConfig key"};
};

class OrderRdos {

  private:
    ServiceHandle<InDetDD::IPixelReadoutManager> m_pixelReadout;
    Identifier m_offlineId;

  public:
    OrderRdos(Identifier offlineId, const ServiceHandle<InDetDD::IPixelReadoutManager> &pixelReadout):
      m_pixelReadout(pixelReadout), m_offlineId(offlineId) {  }

    // copy constructor
    OrderRdos(const OrderRdos & orderFunct) = default;

    // assignment operator
    OrderRdos& operator= (const OrderRdos &other) { 
      m_pixelReadout = other.m_pixelReadout;
      return *this;
    }

    bool operator() (const PixelRDORawData* rdo0, const PixelRDORawData* rdo1);
};

class OrderInitialRdos {

  private:
    ServiceHandle<InDetDD::IPixelReadoutManager> m_pixelReadout;
    const PixelID * m_PixelID;
    SG::ReadCondHandle<PixelCablingCondData> m_pixCabling;

  public:
    OrderInitialRdos(const ServiceHandle<InDetDD::IPixelReadoutManager> &pixelReadout, const PixelID *pixelID, SG::ReadCondHandle<PixelCablingCondData> &pixCabling):
      m_pixelReadout(pixelReadout), m_PixelID(pixelID), m_pixCabling(pixCabling) {  }

    // copy constructor
    OrderInitialRdos(const OrderInitialRdos & orderFunct) = default;

    // assignment operator
    // cppcheck-suppress operatorEqVarError
    OrderInitialRdos& operator= (const OrderInitialRdos &other) { 
      m_pixelReadout = other.m_pixelReadout;
      return *this;
    }

    bool operator() (const PixelRDORawData* rdo0, const PixelRDORawData* rdo1);
};

#endif // PIXELRAWDATABYTESTREAMCNV_PIXELRAWCONTRAWEVENTTOOL_H
