/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
#ifndef EVENTSHAPEREC_EVENTETDENSITYTESTER_H
#define EVENTSHAPEREC_EVENTETDENSITYTESTER_H

#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthAlgorithm.h"

#include "EventShapeInterface/IEventEtDensityProvider.h"

#include "TH1D.h"
#include "TProfile.h"

#include <vector>

class EventEtDensityTester: public AthAlgorithm
{
public:

  EventEtDensityTester(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~EventEtDensityTester();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  
protected:
  
  virtual double getNPV();

private:

  ToolHandleArray<IEventEtDensityProvider> m_tools;
  
  std::string m_histPath;
  std::string m_vtxContainerKey;

  std::vector<TH1D*>     h_densities;
  std::vector<TProfile*> p_densities;

  bool m_fillEvtStore;
  bool m_fillHistos;

  
};

#endif

