/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaHepMCInterface_H
#define AthenaHepMCInterface_H

#include "FadsKinematics/FadsGenerator.h"
#include <string>

class StoreGateSvc;
class MsgStream;

class AthenaHepMCInterface: public FADS::FadsGenerator {
public:
	AthenaHepMCInterface(std::string);
	~AthenaHepMCInterface();
	void Initialize();
	void Terminate();
	HepMC::GenEvent* GenerateAnEvent();
private:
	StoreGateSvc *p_sgSvc;
	MsgStream * m_log;
	MsgStream log();
};

#endif
