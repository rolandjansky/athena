/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REGISTRATIONSVC_OUTPUTCONDALG_H
#define REGISTRATIONSVC_OUTPUTCONDALG_H
// OutputConditionsAlg.h
// an Algorithm to output conditions data on an AthenaOutputStream and 
// optionally register it in the conditions database
// Richard Hawkings, started 1/9/05, from a skeleton by Walter Lampl

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IOVTime.h"
#include <vector>
#include <string>

class IAthenaOutputStreamTool;
class IClassIDSvc;
class IIOVRegistrationSvc;

class OutputConditionsAlg: public AthAlgorithm 
{
public:

    OutputConditionsAlg(const std::string& name, ISvcLocator* pSvcLocator);
    ~OutputConditionsAlg();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private:

   ServiceHandle<IClassIDSvc> p_clidsvc{this,"ClassIDSvc","ClassIDSvc"};
   ServiceHandle<IIOVRegistrationSvc> p_regsvc{this, "IOVRegistrationSvc","IOVRegistrationSvc"};

  uint64_t timeToNano(unsigned long int timesec) const;

  typedef ToolHandle<IAthenaOutputStreamTool> IAthenaOutputStreamTool_t;
  IAthenaOutputStreamTool_t m_streamer;

  Gaudi::Property<std::vector<std::string> >  m_objectList{this,"ObjectList",{},"List of object to be written","OrderedSet<std::string>"};
  Gaudi::Property<std::string> m_streamName{this,"StreamName","ConditionsAlgStream"};
  Gaudi::Property<bool> m_par_writeIOV{this,"WriteIOV",false};
  Gaudi::Property<unsigned int> m_par_run1 {this,"Run1",IOVTime::MINRUN,"IOV start (run-number)"};
  Gaudi::Property<unsigned int> m_par_lumib1 {this,"LB1",IOVTime::MINEVENT,"IOV start (LB-number)"};
  Gaudi::Property<unsigned int> m_par_run2 {this,"RUN2",IOVTime::MAXRUN,"IOV end (run-number)"};
  Gaudi::Property<unsigned int> m_par_lumib2 {this,"LB2",IOVTime::MAXEVENT,"IOV end (LB-number)"};
  UnsignedLongProperty m_par_time1 {this,"Time1",IOVTime::MINTIMESTAMP,"IOV start (timestamp)"};
  UnsignedLongProperty m_par_time2 {this,"Time2",IOVTime::MAXEVENT,"IOV end (timestamp)"};
  Gaudi::Property<bool> m_par_timestamp {this,"UseTime",false,"IOV in second or Run/LB"};
  Gaudi::Property<std::vector<std::string> > m_par_iovtags {this,"IOVTagList",{},"List of Tags to be written","OrderedSet<std::string>"};
};

#endif // REGISTRATIONSVC_OUTPUTCONDALG_H
