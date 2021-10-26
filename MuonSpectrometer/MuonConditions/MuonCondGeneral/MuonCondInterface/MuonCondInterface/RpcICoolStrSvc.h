/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_RPCICOOLSTRSVC_H
#define MUONCONDSVC_RPCICOOLSTRSVC_H

// based on similar svc for CSCs
// interact with COOL conditions database for simple
// string data

#include "GaudiKernel/Service.h"

#include <string>
#include "GaudiKernel/IInterface.h"
#include "CxxUtils/checker_macros.h"


namespace MuonCalib {

    class RpcCalibData;
    class RpcCondParType;

    class ATLAS_NOT_THREAD_SAFE RpcICoolStrSvc : virtual public IInterface {

        public:
            static const InterfaceID& interfaceID();

            // put data in file into COOL 
            virtual StatusCode putFile(const std::string& filename) const=0;
            virtual StatusCode putOnlineFile(const std::string& filename) const=0;

            //put data back into datafile
            virtual StatusCode makeFile(const std::string& fileName) const = 0;
            virtual StatusCode makeOnlineFile(const std::string& fileName) const = 0;

            virtual StatusCode writeToDB() const=0;
            virtual StatusCode writeToOnlineDB() const=0;

            
    };


    inline const InterfaceID& RpcICoolStrSvc::interfaceID() {
        static const InterfaceID IID_RpcICoolStrSvc("RpcICoolStrSvc",1,0);
        return IID_RpcICoolStrSvc;
    }
}
#endif // MUONCONDSVC_RPCICOOLSTRSVC_H
