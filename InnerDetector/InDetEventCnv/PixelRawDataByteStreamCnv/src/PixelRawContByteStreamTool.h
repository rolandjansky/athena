/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

//#include "GaudiKernel/IToolSvc.h"

class IPixelCablingSvc;
class PixelID;
class SrcIdMap;
class PixelRodEncoder;

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
  
private:

  ServiceHandle<IPixelCablingSvc> m_pixelCabling;
  const PixelID* m_PixelID;

  const InDetDD::PixelDetectorManager* m_pixelManager;
  
  FullEventAssembler<SrcIdMap> m_fea; 
  unsigned short m_RodBlockVersion;
  int m_BCs_per_LVL1ID;

  MsgStream m_log;
};
#endif // PIXELRAWDATABYTESTREAMCNV_PIXELRAWCONTRAWEVENTTOOL_H
