/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelRodEncoder.h
//   Header file for class PixelRodEncoder
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//  Version 00-00-00 06/03/2007 Daniel Dobos
///////////////////////////////////////////////////////////////////

#ifndef PIXELRAWDATABYTESTREAMCNV_PIXELRODENCODER_H
#define PIXELRAWDATABYTESTREAMCNV_PIXELRODENCODER_H

// #define _DEBUG

#include <inttypes.h>

#include "ByteStreamData/RawEvent.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "PixelByteStreamModuleMask.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "GaudiKernel/ServiceHandle.h"
#include "PixelCabling/IPixelCablingSvc.h"


class PixelRodEncoder 
{
public: 
 
  typedef PixelRDORawData RDO;
  typedef PixelRDO_Container PixelRawContainer; 
  typedef std::vector<const RDO*> VRDO ;

  //constructor 
  PixelRodEncoder();
 
  // destructor 
  ~PixelRodEncoder();
 
  // addRdo
  void addRdo(const RDO * rdo) {
   m_RDOs.push_back(rdo);
  }

  // set Pixel Id Helper
  void set_PixelID(const PixelID * pixelid) {
    m_PixelID = pixelid;
  }
  
  // setCablingSvc
  void set_pixelCabling(const ServiceHandle<IPixelCablingSvc>& pixelCabling) {
    m_pixelCabling = pixelCabling;
  }

  // setRodMinorVersion 
  void setRodMinorVersion (unsigned short RodBlockVersion) {
    m_RodBlockVersion = RodBlockVersion;
  }

  void setPixelDetectorManager(const InDetDD::PixelDetectorManager *m) {
    m_PixelDetectorManager=m;
  }

  // fill the Rod  
  void fillROD(std::vector<uint32_t>& v, MsgStream& log, int BCs_per_LVL1ID); 

  bool IsIBL(const Identifier & pixelId);
  void packIBLcondensed(std::vector <uint32_t > & v32rod, std::vector <uint32_t > & vRows, std::vector <uint32_t > & vCols, std::vector<int> & vTots);
  void packIBLCondensed(std::vector <uint32_t > & v32rod, const VRDO &rdos_sameIBL_offlineId);
  uint32_t packLinkHeader(uint32_t module, uint32_t bcid, uint32_t lvl1id, uint32_t lvl1idskip, uint32_t errors);
  uint32_t packLinkHeader_IBL(uint32_t module, uint32_t bcid, uint32_t lvl1id, uint32_t feFlag);
  uint32_t packRawDataWord(uint32_t FE, uint32_t row, uint32_t column, uint32_t ToT);
  uint32_t packRawDataWord_IBL(uint32_t row, uint32_t column, int ToT, uint32_t nLink);
  uint32_t packLinkTrailer(uint32_t errors); // for Pixel
  uint32_t packLinkTrailer_IBL(uint32_t FEonSLink, bool timeOutErrorBit, bool condensedModeBit, bool linkMasked); // for IBL



private:

  const PixelID * m_PixelID;
  ServiceHandle<IPixelCablingSvc> m_pixelCabling;
  const InDetDD::PixelDetectorManager* m_PixelDetectorManager;

  unsigned short m_RodBlockVersion ;
  VRDO m_RDOs;
  bool m_is_ibl_present;
  bool m_is_ibl_module;
  bool m_is_dbm_module;

};

class OrderRdos {

 private :
  
  ServiceHandle<IPixelCablingSvc> m_pixelCabling;
  Identifier m_offlineId;

 public :
  
 OrderRdos(Identifier offlineId, const ServiceHandle<IPixelCablingSvc>& pixelCabling):
  m_pixelCabling("dummy","dummy") ,  m_offlineId(offlineId) 
  { m_pixelCabling = pixelCabling; }
  
 OrderRdos(const OrderRdos & orderFunct): // copy constructor
  m_pixelCabling("dummy","dummy") , m_offlineId(orderFunct.m_offlineId) 
    {  m_pixelCabling = orderFunct.m_pixelCabling; }
  
 OrderRdos& operator= (const OrderRdos &other) { // assignment operator
    m_pixelCabling = other.m_pixelCabling;
    return *this;
 }

 bool operator () (const PixelRDORawData* rdo0, const PixelRDORawData* rdo1);
  
};

class OrderInitialRdos {

 private :
  
  ServiceHandle<IPixelCablingSvc> m_pixelCabling;
  //  Identifier m_offlineId;
  const PixelID * m_PixelID;

 public :
  
  // OrderInitialRdos(Identifier offlineId, ServiceHandle<IPixelCablingSvc> pixelCabling, const PixelID * pixelID): 
  //  m_pixelCabling("dummy","dummy") ,  m_offlineId(offlineId) , m_PixelID(pixelID)
 OrderInitialRdos(const ServiceHandle<IPixelCablingSvc>& pixelCabling, const PixelID * pixelID):
  m_pixelCabling("dummy","dummy") , m_PixelID(pixelID)
  { m_pixelCabling = pixelCabling; }
  
 OrderInitialRdos(const OrderInitialRdos & orderFunct): // copy constructor
  //  m_pixelCabling("dummy","dummy") , m_offlineId(orderFunct.m_offlineId) , m_PixelID()
  m_pixelCabling("dummy","dummy") , m_PixelID(orderFunct.m_PixelID)
    {  m_pixelCabling = orderFunct.m_pixelCabling; }

 OrderInitialRdos& operator= (const OrderInitialRdos &other) { // assignment operator
    m_pixelCabling = other.m_pixelCabling;
    return *this;
 }
  
  bool operator () (const PixelRDORawData* rdo0, const PixelRDORawData* rdo1);
  
};

#endif
