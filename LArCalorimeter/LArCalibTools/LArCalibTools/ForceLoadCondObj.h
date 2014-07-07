/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORCELOADCONDOBJ_H
#define FORCELOADCONDOBJ_H

// ForceLoadCondObj.h

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>
#include <string>

class StoreGateSvc;
class IClassIDSvc;

class ForceLoadCondObj: public Algorithm 
{
public:
    ForceLoadCondObj(const std::string& name, ISvcLocator* pSvcLocator);
    ~ForceLoadCondObj();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    //StatusCode stop();

private:

  ServiceHandle<StoreGateSvc> p_detstore;
  ServiceHandle<IClassIDSvc> p_clidsvc;

  std::vector<std::string> m_objectList; 

};

#endif // REGISTRATIONSVC_OUTPUTCONDALG_H
