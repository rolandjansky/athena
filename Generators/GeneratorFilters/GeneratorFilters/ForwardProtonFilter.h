/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_FORWARDPROTONFILTER_H
#define GENERATORFILTERS_FORWARDPROTONFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Description: Filter based on kinematics of forward proton
/// @author Rafal Staszewski, Jul 2011
class ForwardProtonFilter : public GenFilter {
public:

  ForwardProtonFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

  double m_xiMin;
  double m_xiMax;
  double m_ptMin;
  double m_ptMax;
  double m_E0;
  bool m_DoubleTag;
  bool m_Single_tagA;
  bool m_Single_tagC;

};

#endif
