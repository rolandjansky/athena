/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_TruthClassDecoratorTool_H
#define INDETPHYSVALMONITORING_TruthClassDecoratorTool_H
/**
 * @file TruthClassDecoratorTool.h
 * @author shaun roe
 * 27 August 2014
 **/

// STL includes
#include <string>
#include "InDetPhysValMonitoring/IInDetPhysValDecoratorTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MCTruthClassifier/IMCTruthClassifier.h"

// class to decorate xAOD::TruthParticles with type and origin, required by validation
class TruthClassDecoratorTool: virtual public IInDetPhysValDecoratorTool, public AthAlgTool {
public:
  TruthClassDecoratorTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~TruthClassDecoratorTool () {/*nop*/
  };
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual bool decorateTruth(const xAOD::TruthParticle& particle, const std::string& prefix = "");
private:
  ToolHandle<IMCTruthClassifier>        m_truthClassifier;
};


#endif
