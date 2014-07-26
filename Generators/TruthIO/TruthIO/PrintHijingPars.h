/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHIO_PRINTHIJINGPARS_H
#define TRUTHIO_PRINTHIJINGPARS_H

#include "GeneratorModules/GenBase.h"

class PrintHijingPars:public GenBase {
public:
        PrintHijingPars(const std::string& name, ISvcLocator* pSvcLocator);
        StatusCode initialize();
        StatusCode execute();
        StatusCode finalize();

private:

  // Setable Properties:-
  std::string m_key; 
  bool  m_VerboseOutput;
  int  m_firstEvt;
  int  m_lastEvt;
};

#endif

