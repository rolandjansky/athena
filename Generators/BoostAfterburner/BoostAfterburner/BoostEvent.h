/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __BOOSTEVENT_H__
#define __BOOSTEVENT_H__

#include "AthenaBaseComps/AthAlgorithm.h"

class BoostEvent : public AthAlgorithm
{
public:
  BoostEvent(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  std::string m_inkey;
  std::string m_outkey;
  double m_beta_x;
  double m_beta_y;
  double m_beta_z;

  


};


#endif
