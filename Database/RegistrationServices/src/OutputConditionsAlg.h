/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REGISTRATIONSVC_OUTPUTCONDALG_H
#define REGISTRATIONSVC_OUTPUTCONDALG_H
// OutputConditionsAlg.h
// an Algorithm to output conditions data on an AthenaOutputStream and 
// optionally register it in the conditions database
// Richard Hawkings, started 1/9/05, from a skeleton by Walter Lampl

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>
#include <string>

class IAthenaOutputStreamTool;
class StoreGateSvc;
class IClassIDSvc;
class IIOVRegistrationSvc;

class OutputConditionsAlg: public Algorithm 
{
public:
    OutputConditionsAlg(const std::string& name, ISvcLocator* pSvcLocator);
    ~OutputConditionsAlg();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private:

  ServiceHandle<StoreGateSvc> p_detstore;
  ServiceHandle<IClassIDSvc> p_clidsvc;
  ServiceHandle<IIOVRegistrationSvc> p_regsvc;

  uint64_t timeToNano(unsigned long int timesec) const;

  typedef ToolHandle<IAthenaOutputStreamTool> IAthenaOutputStreamTool_t;
  IAthenaOutputStreamTool_t m_streamer;
  //ServiceHandle<IIOVRegistrationSvc> regsvc;
  std::vector<std::string> m_objectList; 
  std::string m_streamName;
  bool par_writeIOV;
  unsigned int par_run1;
  unsigned int par_lumib1;
  unsigned int par_run2;
  unsigned int par_lumib2;
  UnsignedLongProperty par_time1;
  UnsignedLongProperty par_time2;
  bool par_timestamp;
  std::vector<std::string> par_iovtags;
};

#endif // REGISTRATIONSVC_OUTPUTCONDALG_H
