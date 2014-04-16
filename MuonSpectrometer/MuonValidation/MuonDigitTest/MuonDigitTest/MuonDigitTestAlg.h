/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonDigitTestAlg_H
#define MuonDigitTestAlg_H

#include "GaudiKernel/Algorithm.h"

#include "GaudiKernel/MsgStream.h"

#include <string>
#include <vector>

namespace FADS {
  class Tokenizer;
}

template <class ConcreteAlgorithm> class AlgFactory;

class MuonDigitTestAlg: public Algorithm {

  friend class AlgFactory<MuonDigitTestAlg>;

 public:
  MuonDigitTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~MuonDigitTestAlg();

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
