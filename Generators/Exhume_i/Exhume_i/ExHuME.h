/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXHUME_H
#define EXHUME_H
#include "GeneratorModules/GenModule.h"

#include "Exhume_i/Event.h"
#include "Exhume_i/CrossSection.h"
#include "Exhume_i/PythiaInterface.h"
//#include "Exhume_i/HepG.h"
#include "Exhume_i/TwoSpace.h"
#include "Exhume_i/Higgs.h"
#include "HepMC/HEPEVT_Wrapper.h"
#include "HepMC/IO_HEPEVT.h"
#include "Tauola_i/Atlas_HEPEVT.h"
#include <string>
#include <map>

/////////////////////////////////////////////////////////////////////////////

class ExHuME:public GenModule {
 public:
  ExHuME (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode genInitialize();
  StatusCode callGenerator();
  StatusCode genFinalize();
  StatusCode fillEvt(HepMC::GenEvent* evt);
  
 private:
  void SetProcess();
  void SetExhumeParameters();
  Exhume::PythiaInterface * myPythiaInterface;
  Exhume::Event * myEvent;
  Exhume::CrossSection *myCrossSection;
  Exhume::Higgs * myHiggs;
  Exhume::TwoSpace * myTwoSpace;
  double MassMin,MassMax,ETMin,CosThetaMax;
  std::string ProcessName;
  int HiggsDecayChannel;
  int ProcessID;
  int m_events;
  int m_efficiency_et_cut;
  int RandomSeed;
  std::string ExhumeRunControl;
  std::string PythiaSusyParameters;
  
  bool dirty_xs;
  bool m_PhotosLinked;
  bool m_TauolaLinked;
  std::map<std::string,int> ProcessList;

  void store_Atlas_HEPEVT(void);
  static Atlas_HEPEVT* atlas_HEPEVT;

    
};

#endif

