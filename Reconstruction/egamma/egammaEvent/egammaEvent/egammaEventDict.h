// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_EGAMMAEVENTDICT_H
#define EGAMMAEVENT_EGAMMAEVENTDICT_H 1

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/ElectronConstituent.h"
#include "egammaEvent/ElectronAssociation.h"
#include "egammaEvent/PhotonContainer.h"
#include "egammaEvent/PhotonAssociation.h"
#include "egammaEvent/egDetailContainer.h"
#include "egammaEvent/EMConvert.h"
#include "egammaEvent/EMShower.h"
#include "egammaEvent/EMTrackMatch.h"
#include "egammaEvent/EMBremFit.h"
#include "egammaEvent/EMTrackFit.h"
#include "egammaEvent/EMErrorDetail.h"
#include "egammaEvent/EMShowerMinimalContainer.h"
#include "egammaEvent/CaloRingsContainer.h"
#include "TSystem.h"

namespace egammaEventDict {

struct temp {
  Navigable<egammaContainer,double> m_nav;
  ElementLink<egammaContainer> m_link;
  std::pair<ElementLink<egammaContainer>,double> m_pair;
  Navigable<ElectronContainer,double> m_navElectron;
  ElementLink<ElectronContainer> m_linkElectron;
  std::pair<ElementLink<ElectronContainer>,double> m_pairElectron;
  Navigable<PhotonContainer,double> m_navPhoton;
  ElementLink<PhotonContainer> m_linkPhoton;
  std::pair<ElementLink<PhotonContainer>,double> m_pairPhoton;
  Navigable<EMShowerMinimalContainer,double> m_nav2;
  ElementLink<EMShowerMinimalContainer> m_link2;
  std::pair<ElementLink<EMShowerMinimalContainer>,double> m_pair2;
  std::pair<egammaPID::PID,double> m_pair3;
  std::pair<egammaPID::PID,unsigned int> m_pair4;
  std::pair<egammaParameters::ParamDef,double> m_pair5;
  std::pair<egammaParameters::ParamDef,bool> m_pair6;
  ElementLinkVector<egDetailContainer> m_linkVector;
  ElementLink<CaloRingsContainer> m_linkRings;

  DataModel_detail::ElementProxy<DataVector<Analysis::Electron> > m_elproxy_1;
  DataModel_detail::ElementProxy<DataVector<Analysis::Photon> >   m_elproxy_2;
  DataModel_detail::ElementProxy<DataVector<egDetail> >           m_elproxy_3;
  DataModel_detail::ElementProxy<DataVector<EMShowerMinimal> >    m_elproxy_4;
  DataModel_detail::ElementProxy<DataVector<egamma> >             m_elproxy_5;
  DataModel_detail::ElementProxy<DataVector<CaloRings> >          m_elproxy_6;

  // Get rid of warnings.
  temp(const temp&);
};


// Ensure that the dictionary for the enums is loaded when this one is.
struct EnumDictLoader
{
  EnumDictLoader();
};
EnumDictLoader::EnumDictLoader()
{
  // ??? W32 doesn't use lib...
  gSystem->Load ("libegammaEnumsDict");
}
EnumDictLoader enumDictLoader;

} //> end namespace egammaEventDict

#endif 
