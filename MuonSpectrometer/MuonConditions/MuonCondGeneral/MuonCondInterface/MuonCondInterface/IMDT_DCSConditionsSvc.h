/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMDT_DCSCONDITIONSSVC_H
#define IMDT_DCSCONDITIONSSVC_H

#include <algorithm>
#include <string>
#include <vector>

#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/IInterface.h"

//#include "MuonCondSvc/MuonHierarchy.h"
#include "MuonCondInterface/IMDTConditionsSvc.h"

class Identifier;
class IdentifierHash;
class StatusCode;

class IMDT_DCSConditionsSvc : virtual public IMDTConditionsSvc {
public:
    virtual ~IMDT_DCSConditionsSvc() {}

    // map initialization method
    virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS) = 0;

    // virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
    static const InterfaceID& interfaceID();

    // protected:

    // virtual const std::vector<std::string>& deadMultiLayers()=0 ;
    // virtual const std::vector<std::string>& deadTubes()=0 ;
    // virtual const std::vector<std::string>& deadStations()=0 ;

    // virtual const std::vector<Identifier>& deadMultiLayersId()=0 ;
    // virtual const std::vector<Identifier>& deadTubesId()=0 ;
    // virtual const std::vector<Identifier>& deadStationsId()=0 ;

private:
};

inline const InterfaceID& IMDT_DCSConditionsSvc::interfaceID() {
    static const InterfaceID IID_MDT_DCSConditionsSvc("MDT_DCSConditionsSvc", 1, 0);
    return IID_MDT_DCSConditionsSvc;
}
#endif
