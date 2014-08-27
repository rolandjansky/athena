/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_PTMISSANDORLEPTONFILTER_H
#define GENERATORFILTERS_PTMISSANDORLEPTONFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "GaudiKernel/IToolSvc.h"

/// Filters events based on their missing Pt AND/OR leptons
/// @author Pedro Teixeira-Dias, Ricardo Goncalo, July 2006
/// @author Pauline Gagnon, August 2005
class PtmissAndOrLeptonFilter : public GenFilter {
public:

  PtmissAndOrLeptonFilter(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode filterInitialize();
  StatusCode filterFinalize();
  StatusCode filterEvent();

private:

  std::string m_InputCollectionName;

  bool m_PtmissANDLepton;
  double m_PtminElectron, m_MaxEtaElectron;
  double m_PtminMuon,     m_MaxEtaMuon;
  double m_PtminLostTrack;
  double m_MinEtaLost;
  double m_PtminMissing;

};

#endif
