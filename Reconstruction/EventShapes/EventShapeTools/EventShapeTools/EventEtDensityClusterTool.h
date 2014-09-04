// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPETOOLS_EVENTETDENSITYCLUSTERTOOL_H
#define EVENTSHAPETOOLS_EVENTETDENSITYCLUSTERTOOL_H

#include "EventShapeTools/EventEtDensityProviderBase.h"

class EventEtDensityClusterTool : public EventEtDensityProviderBase
{
public:

  EventEtDensityClusterTool(const std::string& type,const std::string& name,
			    const IInterface* pParent);
  virtual ~EventEtDensityClusterTool();


protected:

  virtual StatusCode executeAction();
  virtual StatusCode executeAction(double eta);
  virtual StatusCode executeAction(double etaMin,double etaMax);

private:

  double m_clusterEtMin;
  bool   m_useAreaFourMom;
  
  std::string m_evtShapeKey;

};
#endif
