// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECO_ALGS_JOBOPTIONSDUMPERALG_H
#define RECO_ALGS_JOBOPTIONSDUMPERALG_H

#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>

class JobOptsDumperAlg : public AthAlgorithm {
public:

  using AthAlgorithm::AthAlgorithm;
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override {return StatusCode::SUCCESS;};
    
private:
  // Properties
  Gaudi::Property<std::string> m_fileName {this, "FileName","jobOptions.txt", "Name of the output file"};  
};

#endif 
