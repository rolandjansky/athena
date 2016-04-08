/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMUONRPCRODDECODER_H
#define IMUONRPCRODDECODER_H

#include "GaudiKernel/IAlgTool.h"
#include "ByteStreamData/RawEvent.h"
#include "eformat/SourceIdentifier.h"


using eformat::helper::SourceIdentifier;


class RpcPadContainer;

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

namespace Muon
{

static const InterfaceID IID_IRpcROD_Decoder("Muon::IRpcROD_Decoder", 1, 0);

class IRpcROD_Decoder : virtual public IAlgTool
{
public:
    static const InterfaceID& interfaceID(){ return IID_IRpcROD_Decoder; }

    // enter declaration of your interface-defining member functions here
    virtual StatusCode fillCollections(  const ROBFragment& robFrag, 
                                         RpcPadContainer& rdoIdc,
                                         std::vector<IdentifierHash>collections) const = 0;

};

} // end of namespace

#endif
