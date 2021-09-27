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
  void compilePlugin(std::string);

protected:
  
  SHERPA::Sherpa * p_sherpa;

  /// Sherpa base settings (read from base fragment file) and run card snippet (from JO file)
  std::map<std::string,std::string> m_inputfiles;

  /// List of needed OpenLoops process libraries (from JO file)
  std::vector<std::string> m_openloopslibs;

  /// List of any additional needed files, e.g. custom libraries, PDF sets (from JO file)
  std::vector<std::string> m_extrafiles;

  /// Number of cores recommended for multi-core integration file
  int m_ncores;

  /// Memory required for integration/evgen
  double m_memorymb;

  /// Optional code for plugin library to compile and load at run time
  std::string m_plugincode;

  /// Variation weight cap factor
  double m_variation_weight_cap;

  double m_xsscale;
  bool m_cleanup;
};



#include "ATOOLS/Math/Random.H"

class Atlas_RNG: public ATOOLS::External_RNG {
  CLHEP::HepRandomEngine* p_engine;
public:
  Atlas_RNG(CLHEP::HepRandomEngine*);
  double Get();
};


#endif // SHERPA_I_SHERPA_I_H


