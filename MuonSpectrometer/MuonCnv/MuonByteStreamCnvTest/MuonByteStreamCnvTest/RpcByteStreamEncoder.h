/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_RPCBYTESTREAMENCODER_H
#define MUONBYTESTREAM_RPCBYTESTREAMENCODER_H

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigT1RPClogic/RPCbytestream.h"
#include "TrigT1RPChardware/MatrixReadOutStructure.h"
#include "TrigT1RPChardware/PadReadOutStructure.h"
#include "RPCcablingInterface/IRPCcablingSvc.h"
#include "MuonRDO/RpcPadContainer.h"

// Builds a RPCbytestream object from the bare RPC RDO's
// Stefano Rosati
// Jan 2003

// typedefs
typedef unsigned short int ubit16;
typedef std::map < int, CMAreadout, std::less <int> > CMA_Readout;
typedef std::map < int, PADreadout, std::less <int> > PAD_Readout;
typedef std::vector<unsigned int> RpcByteStream;

class RpcByteStreamEncoder {

 public:

  // Constructor and destructor
  RpcByteStreamEncoder(const RpcPadContainer* padContainer);
  ~RpcByteStreamEncoder();

  StatusCode encodeByteStream();

  RpcByteStream* getByteStream() {return m_byteStream;};
  
 private:

  // Encode RDO's
  void addRx();
  void addPad(const RpcPad * pad);
  void addMatrix(const RpcCoinMatrix * matrix);
  void addFiredChannel(const RpcFiredChannel * firedChannel);

  // Add a 16 bit word to the 32 bits bytestream
  void addWord(ubit16 dataWord);

  // Data members
  const RpcPadContainer* m_padContainer;
  RpcByteStream* m_byteStream;
  unsigned int m_byteStreamWord;
  bool m_highest;

};

#endif //  MUONBYTESTREAM_RPCBYTESTREAMENCODER_H
