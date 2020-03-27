/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_FOURTAULEPLEPHADHADFILTER_H
#define GENERATORFILTERS_FOURTAULEPLEPHADHADFILTER_H

#include "GeneratorModules/GenFilter.h"

/// @author Rachel Emma Clarke SMITH
/// Filter H->XX->4tau->2 leptonic tau decays, 2 hadronic tau decays

class FourTauLepLepHadHadFilter : public GenFilter {
public:

  FourTauLepLepHadHadFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterFinalize();
  virtual StatusCode filterEvent();

private:

};

#endif


