/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TruthValAlg_H
#define TruthValAlg_H

#include "GaudiKernel/Algorithm.h"

#include "GaudiKernel/MsgStream.h"

#include "MuonHitTest/ValAlgInterface.h"

#include <string>

template <class ConcreteAlgorithm> class AlgFactory;

class TruthValAlg: public Algorithm, private ValAlgInterface {

friend class AlgFactory<TruthValAlg>;

public:
	TruthValAlg(const std::string& name, ISvcLocator* pSvcLocator);
	~TruthValAlg();

	StatusCode initialize();
        StatusCode finalize();
        StatusCode execute();

private:
	MsgStream log;
};

#endif
