/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMUONRPCRODDECODER_H
#define IMUONRPCRODDECODER_H

#include "GaudiKernel/IAlgTool.h"
#include "ByteStreamData/RawEvent.h"
#include "eformat/SourceIdentifier.h"

class RpcPadContainer;
class RpcSectorLogicContainer;
class IdentifierHash;

namespace Muon
{

static const InterfaceID IID_IRpcROD_Decoder("Muon::IRpcROD_Decoder", 1, 0);

class IRpcROD_Decoder : virtual public IAlgTool
{
public:
    static const InterfaceID& interfaceID()
    {
        return IID_IRpcROD_Decoder;
    }

    // enter declaration of your interface-defining member functions here
    virtual StatusCode fillCollections(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag, 
                         RpcPadContainer& rdoIdc,
			 const std::vector<IdentifierHash> &collections, RpcSectorLogicContainer*) const = 0;

    virtual void setSLdecodingRequest() = 0;
};

} // end of namespace

#endif
