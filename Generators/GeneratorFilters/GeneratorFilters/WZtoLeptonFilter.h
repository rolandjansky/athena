/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_WTOLEPTONFILTER_H
#define GENERATORFILTERS_WTOLEPTONFILTER_H

#include "GeneratorModules/GenFilter.h"

/// Filter based on leptons from W boson
/// @todo Check generator compatibility
/// @author Lian-You SHAN, Apr 2005
/// @author Michael Heldmann, Jan 2003
class WZtoLeptonFilter : public GenFilter {
public:

  WZtoLeptonFilter(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode filterInitialize();
  StatusCode filterFinalize();
  StatusCode filterEvent();

private:

  int m_tesit;
  int m_signal;
  int m_survival;
  int m_Nlepton;
  int m_wzlepton;
  int m_reconeffi;
  int m_samesign;

  // The prefix m should be understood as minimum for Pt and maximum for Eta (AB: *sigh*)
  double m_Pt_e;
  double m_Pt_mu;

  double m_Eta_e;
  double m_Eta_mu;

  int m_AthenaCalls ;
  double m_tot_wghts, m_wPass ;
  double catevti[45] ;
  double catevtf[45];

};

#endif
