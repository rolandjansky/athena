/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCBYTESTREAMDECODER_H
#define RPCBYTESTREAMDECODER_H

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigT1RPClogic/RPCbytestream.h"
#include "TrigT1RPChardware/MatrixReadOutStructure.h"
#include "MuonRDO/RpcPad.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "RPC_CondCabling/RpcCablingCondData.h"

// Decode a RPCbytestream object and build the bare RPC RDO's
// Stefano Rosati
// Jan 2003

// typedefs
typedef std::map < int, CMAreadout, std::less <int> > CMA_Readout;
typedef std::map < int, PADreadout, std::less <int> > PAD_Readout;

class RpcByteStreamDecoder {

 public:
  // Constructor and destructor
  RpcByteStreamDecoder(const RPCbytestream * p_bytestream, const RpcCablingCondData* readCdo, const RpcIdHelper* rpcId, MsgStream* log=nullptr);
  ~RpcByteStreamDecoder();

  // Decode the bytestream coming from the LVL1 simulation
  StatusCode decodeByteStream();

  // Retrieve the vector of RDO's (RpcPad)
  std::vector<RpcPad*>* getPads() {return m_rpcpads;};

  // Printout the RDO's for testing
  void print();

 private:

  // Pad and Matrix decoding functions
  RpcPad * decodePad(PADreadout& pad);
  RpcCoinMatrix * decodeMatrix(MatrixReadOut* matrix, Identifier& id);

  // Data members
  const RPCbytestream* m_bytestream;
  const RpcCablingCondData* m_cabling;
  const RpcIdHelper* m_rpcIdHelper;
  std::vector<RpcPad*>* m_rpcpads;

  MsgStream* m_log;
  bool m_debug;
  bool m_verbose;

  RpcByteStreamDecoder & operator=(const RpcByteStreamDecoder &right);
  RpcByteStreamDecoder(const RpcByteStreamDecoder&);

};

#endif //  RPCBYTESTREAMDECODER_H







