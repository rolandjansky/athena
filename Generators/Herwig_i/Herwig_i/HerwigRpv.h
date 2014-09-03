/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORMODULESHERWIGRPV_H
#define GENERATORMODULESHERWIGRPV_H
#include "GeneratorModules/GenModule.h"
#include "AcerMC_i/AcerMC_acset.h"
#include "Tauola_i/Atlas_HEPEVT.h"
#include "Herwig_i/Herwig.h"

using std::string;
typedef std::vector<std::string> CommandVector;

/**
@class HerwigRpv
@brief main interface class wrapper to Herwig


Allows the user to generate Herwig events with RPV mod and store the result in the Transient Store.

genInitialize() is used to read parameters

callGenerator() makes the event

genFinalize() writes log files etc

fillEvt(GeneratorEvent* evt) passes the event to HepMC

*/

class HerwigRpv
  :public Herwig 
{
public:
  inline HerwigRpv(const std::string& name, ISvcLocator* pSvcLocator): Herwig(name, pSvcLocator){};
};

#endif
