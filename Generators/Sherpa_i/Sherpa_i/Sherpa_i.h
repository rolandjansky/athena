/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SHERPA_I_SHERPA_I_H
#define SHERPA_I_SHERPA_I_H

#include "GaudiKernel/Algorithm.h"
#include "GeneratorModules/GenModule.h"
#include "TruthUtils/GeneratorName.h"


namespace SHERPA {
  class Sherpa;
}

namespace HepMC {
  class GenEvent;
}

class Sherpa_i : public GenModule {
public:
  Sherpa_i(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode genInitialize();
  StatusCode callGenerator();
  StatusCode genFinalize();
  StatusCode fillEvt(HepMC::GenEvent* evt);
  void getParameters(int &argc, char** &argv);

protected:
  
  SHERPA::Sherpa * p_sherpa;

  std::string m_path;
  std::vector<std::string> m_params;
  double m_xsscale;
  std::string m_scalevarref;
};



#include "ATOOLS/Math/Random.H"

class Atlas_RNG: public ATOOLS::External_RNG {
  CLHEP::HepRandomEngine* p_engine;
public:
  Atlas_RNG(CLHEP::HepRandomEngine*);
  double Get();
};


#endif // SHERPA_I_SHERPA_I_H


