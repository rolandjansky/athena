/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonEvtValidatorAlg_H
#define MuonEvtValidatorAlg_H

#include "GaudiKernel/Algorithm.h"

#include "GaudiKernel/MsgStream.h"

#include <string>
#include <vector>

namespace FADS {
  class Tokenizer;
}

template <class ConcreteAlgorithm> class AlgFactory;

class MuonEvtValidatorAlg: public Algorithm {

  friend class AlgFactory<MuonEvtValidatorAlg>;

 public:
  MuonEvtValidatorAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~MuonEvtValidatorAlg();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

 private:
  MsgStream log;
	
  void createSubAlg(std::string, std::string);
  std::vector<Algorithm*> algoSuite;
  std::vector<std::string> algoNames;
};

#endif
