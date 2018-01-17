/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OVERLAYCOMMONALGS_BSFILTER_H
#define OVERLAYCOMMONALGS_BSFILTER_H

#include <string>
#include <map>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"

class BSFilter : public AthAlgorithm
{
 public:

  BSFilter( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~BSFilter();
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 protected:
  int m_All, m_pass, m_EventCounter;
  int m_trigbit;
  std::string m_filterfile;
  class filterinfo{ 
  public:
    int trig, nvtx; double dt;
    int magic;
    filterinfo(): trig(0),nvtx(0), dt(0.0),magic(99999) {}
  };
  std::map< int, std::map < uint64_t, filterinfo > > m_filtermap;

  FILE *m_efile;
  std::string m_EventIdFile;

  ServiceHandle< TrigConf::ITrigConfigSvc > m_trigConf;
};
#endif //OVERLAYCOMMONALGS_BSFILTER_H
