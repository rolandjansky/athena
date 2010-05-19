/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  BCM_RawContByteStreamTool.h
//   Header file for class BCM_RawContByteStreamTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//  Version 00-00-02 05/03/2007 Daniel Dobos for pixels
//  Version 00-00-11 05/02/2009 Daniel Dobos for BCM
///////////////////////////////////////////////////////////////////

#ifndef BCM_RAWDATABYTESTREAMCNV_BCM_RAWCONTBYTESTREAMTOOL_H
#define BCM_RAWDATABYTESTREAMCNV_BCM_RAWCONTBYTESTREAMTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "ByteStreamData/RawEvent.h" 
#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 
#include "ByteStreamCnvSvcBase/SrcIdMap.h" 

class BCM_RDO_Container;
class MsgStream;

class BCM_RawContByteStreamTool: public AthAlgTool {

public:

  // constructor
  BCM_RawContByteStreamTool(const std::string& type, const std::string& name, const IInterface* parent);

  // destructor 
  virtual ~BCM_RawContByteStreamTool();

  // AlgTool InterfaceID
  static const InterfaceID& interfaceID();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  StatusCode convert(BCM_RDO_Container* cont, RawEventWrite* re); 
  
private:

  MsgStream *m_log;
  unsigned int getSourceID(int ChannelID);

  FullEventAssembler<SrcIdMap> m_fea; 
  unsigned short m_RodBlockVersion;
  int m_BCs_per_LVL1ID;
};
#endif
