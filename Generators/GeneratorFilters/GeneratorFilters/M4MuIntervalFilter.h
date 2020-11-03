/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef GENERATORFILTERSM4MUINTERVALFILTER_H
#define GENERATORFILTERSM4MuINTERVALFILTER_H

#include "GeneratorModules/GenFilter.h"

class IAtRndmGenSvc;


class M4MuIntervalFilter : public GenFilter {
public:

  M4MuIntervalFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~M4MuIntervalFilter();
  virtual StatusCode filterInitialize();
  virtual StatusCode filterFinalize();
  virtual StatusCode filterEvent();

private:

  double m_maxEta;
  double m_minPt;                           // Rapidity acceptance

  ServiceHandle<IAtRndmGenSvc> m_rand;     // Random number generator

  double m_prob2medium;
  double m_prob2low;
  double m_prob2high;
  double m_m4mulow;
  double m_m4muhigh;
  bool m_ApplyReWeighting; 

public:
  double getEventWeight(double m);
};

#endif
