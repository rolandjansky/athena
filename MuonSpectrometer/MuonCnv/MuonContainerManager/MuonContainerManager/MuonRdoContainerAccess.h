/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDOCONTAINERACCESS_H
#define MUONRDOCONTAINERACCESS_H


#include <string>

class MsgStream;
class ISvcLocator;

class RpcPadContainer;
class MdtCsmContainer;
class TgcRdoContainer;
class CscRawDataContainer;
class RpcSectorLogicContainer;
class StoreGateSvc;

namespace Muon
{

class RPC_RawDataProviderTool;


class MuonRdoContainerAccess
{
    public:
    
    // methods for retrieving the containers; these are public.
    static RpcPadContainer*         retrieveRpcPad(std::string);
    static MdtCsmContainer*         retrieveMdtCsm(std::string);
    static TgcRdoContainer*         retrieveTgcRdo(std::string);
    static CscRawDataContainer*     retrieveCscRaw(std::string);
    static RpcSectorLogicContainer* retrieveRpcSec(std::string);
    
    private:
    
    // methods for registering the containers; these are private accessible only
    // by the raw data conversion tools
    static StatusCode record(RpcPadContainer*,std::string,ISvcLocator*,MsgStream&, StoreGateSvc* storeGate=0);
    static StatusCode record(MdtCsmContainer*,std::string,ISvcLocator*,MsgStream&, StoreGateSvc* storeGate=0);
    static StatusCode record(TgcRdoContainer*,std::string,ISvcLocator*,MsgStream&, StoreGateSvc* storeGate=0);
    static StatusCode record(CscRawDataContainer*,std::string,ISvcLocator*,MsgStream&, StoreGateSvc* storeGate=0);
    static StatusCode record(RpcSectorLogicContainer*,std::string,ISvcLocator*,MsgStream&, StoreGateSvc* storeGate=0);

    friend class RPC_RawDataProviderTool;
    friend class MDT_RawDataProviderTool;
    friend class TGC_RawDataProviderTool;
    friend class CSC_RawDataProviderTool;
};

}


#endif   //MUONRDOCONTAINERACCESS_H
