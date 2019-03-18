/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  PixelRawContByteStreamCnv.h
//   Header file for class PixelRawContByteStreamCnv
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////
// classID() - returning PixelRDO_Container ID
// createObj() - creates PixelRDO_Container
// createRep() - convert Pixel_RDO in the container into ByteStream
///////////////////////////////////////////////////////////////////
//  Version 00-00-39 05/03/2007 Daniel Dobos
///////////////////////////////////////////////////////////////////

#ifndef PIXELBYTESTREAM_PIXELRAWCONTRAWEVENTCNV_H
#define PIXELBYTESTREAM_PIXELRAWCONTRAWEVENTCNV_H

#include <stdint.h>
#include <string>

#include "GaudiKernel/Converter.h"


#include "ByteStreamData/RawEvent.h" 

#include "InDetRawData/InDetRawDataCLASS_DEF.h"

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h"

class ByteStreamAddress;
class PixelRawContByteStreamTool;
class IByteStreamEventAccess;
class PixelRDORawData;


class PixelRawContByteStreamCnv: public Converter {
 public:

  typedef InDetRawDataCollection<Pixel1RawData> COLLECTION; // define collection format here

  PixelRawContByteStreamCnv(ISvcLocator* svcloc);

  virtual StatusCode initialize() override;

  //! this creates the RawEvent fragments for Pixel
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr) override;

  /// Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static long storageType();
  static const CLID& classID();

private: 

  PixelRawContByteStreamTool* m_PixelRawContBSTool;

  IByteStreamEventAccess* m_ByteStreamEventAccess; 

  StoreGateSvc* m_StoreGate;

  MsgStream m_log;

};
#endif // PIXELBYTESTREAM_PXIELRAWCONTRAWEVENTCNV_H



