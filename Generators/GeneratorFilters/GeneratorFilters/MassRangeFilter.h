/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_MASSFILTER2_H
#define GENERATORFILTERS_MASSFILTER2_H

#include "GeneratorModules/GenFilter.h"

/// Filters on two particles' invariant mass.
///
/// Allows the user to search for the events where 2 particles of user defined
/// type that will pass if they have p_t, eta, and invariant mass in the
/// specified range.
/// If there are more than 1 combinations which pass the eta and pT
/// selection in the event, take the pair which has the largest invariant mass and then
/// apply the invariant mass selection in order to avoid duplication. (Original MassFilter
/// takes the events where at least one pair passes the invariant mass selection.)
/// default is 2 muons with p_t>=10 GeV, |eta|<=10, and Mass between 0-14TeV.
/// Always allow same sign events.
///
/// @author Jeremy Love and Akimasa Ishikawa, Sep 2010
class MassRangeFilter : public GenFilter {
public:

  MassRangeFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

  double m_Ptmin;
  double m_Ptmin2;
  double m_EtaRange;
  double m_EtaRange2;
  double m_InvMassMin;
  double m_InvMassMax;
  int    m_PartId;
  int    m_PartId2;
  int    m_PartStatus;

};

#endif
