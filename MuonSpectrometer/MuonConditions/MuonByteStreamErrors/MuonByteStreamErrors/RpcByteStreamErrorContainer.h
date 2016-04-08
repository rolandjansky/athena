/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 MuonByteStreamErrors package
 RpcByteStreamErrorContainer class
 -----------------------------------------
 ***************************************************************************/


#ifndef MUONBYTESTREAMERRORS_RPCBYTESTREAMERRORCONTAINER_H
#define MUONBYTESTREAMERRORS_RPCBYTESTREAMERRORCONTAINER_H


#include "Identifier/IdentifierHash.h"
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"


namespace Muon {
    enum RpcBSerrorTypes { 
      NoError,
      CMHeaderErrors,       // CM related error (from 0 to 8 in a pad)
      CMSubHeaderErrors,    // CM related error 
      CMFooterErrors,       // CM related error 
      CRC8checkFailures,    // CM related error 
      CMmissing,            // CM related error 
      RXHeaderErrors,       // PAD related error (0 or 1 on a pad)
      RXSubHeaderErrors,
      PADHeaderErrors,
      PADSubHeaderErrors,
      PADPreFooterErrors,
      PADFooterErrors,
      PADmissing,
      SLHeaderErrors,
      SLSubHeaderErrors,
      SLFooterErrors,
      RXFooterErrors,
      Rpc_NUM_ERROR_TYPES  // always have this one last, so we can use it as a loop index
  };
    // For a given pad (uniquely identified by an IdentifierHash: first item or key in the pair) the error code (second item in the pair) keep the whole 
    // information on the errors met in the pad fragment decoding;
    // 
    // If there are CM errors in the pad, the error code is built as ValueCM=100+sum(value*10^cmid) over all cmids with errors; 
    //         value in the range 1-5, i.e. CMHeaderErrors - CRC8checkFailures; 
    // If there's a Pad error, the error code is set to ValuePad+ValueCM, with ValuePad in the range 5-Rpc_NUM_ERROR_TYPES-1
 
  class RpcByteStreamErrorContainer 
  {
  public: 
    typedef DataVector<std::pair<IdentifierHash, int> >::const_iterator rpcErrorIterator;

    RpcByteStreamErrorContainer();
    RpcByteStreamErrorContainer( const DataVector<std::pair<IdentifierHash, int> > & errorVector );
    ~RpcByteStreamErrorContainer();

    void clear(); 
    void addError(IdentifierHash, int); 
    int  getError(IdentifierHash)  const; 
    std::vector<IdentifierHash> getPadsWithError() const; 

    const DataVector<std::pair<IdentifierHash, int> >& getAllErrors() const;

  private:
    DataVector<std::pair<IdentifierHash, int> > m_errorVector; 
  };

}

CLASS_DEF( Muon::RpcByteStreamErrorContainer , 1200798491 , 1 )
#endif // MUONBYTESTREAMERRORS_RPCBYTESTREAMERRORCONTAINER_H

