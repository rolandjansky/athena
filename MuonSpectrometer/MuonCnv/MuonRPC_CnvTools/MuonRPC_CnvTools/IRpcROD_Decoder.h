/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

class IRpcROD_Decoder : virtual public IAlgTool
{
public:
    DeclareInterfaceID( IRpcROD_Decoder, 1, 0 );

    // enter declaration of your interface-defining member functions here
    // bool decodeSL - Used to determine whether sector logic is being decoded too
    virtual StatusCode fillCollections(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag, 
                         			   RpcPadContainer& rdoIdc, const std::vector<IdentifierHash> &collections, RpcSectorLogicContainer*, const bool& decodeSL) const = 0;

};

} // end of namespace

#endif
