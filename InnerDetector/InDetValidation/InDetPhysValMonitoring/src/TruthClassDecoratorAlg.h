/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_TruthClassDecoratorAlg_H
#define INDETPHYSVALMONITORING_TruthClassDecoratorAlg_H
/**
 * @file TruthClassDecoratorAlg.h
 * @author shaun roe
 * 27 August 2014
 **/

// STL includes
#include <string>
#include "xAODTruth/TruthParticleContainer.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "InDetPhysValMonitoring/IAthSelectionTool.h"

// class to decorate xAOD::TruthParticles with type and origin, required by validation
class TruthClassDecoratorAlg: public AthReentrantAlgorithm {
public:
  TruthClassDecoratorAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TruthClassDecoratorAlg () {/*nop*/
  };
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual StatusCode execute(const EventContext &ctx) const override;

private:
  bool decorateTruth(const xAOD::TruthParticle& particle,
                     std::vector< SG::WriteDecorHandle<xAOD::TruthParticleContainer,int> > &int_decor) const;

  PublicToolHandle<IMCTruthClassifier>        m_truthClassifier
     {this,"MCTruthClassifier","MCTruthClassifier/MCTruthClassifier",""};
  PublicToolHandle<IAthSelectionTool>         m_truthSelectionTool
     {this,"TruthSelectionTool","",""};

  ///TruthParticle container's name needed to create decorators
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthParticleName
    {this, "TruthParticleContainerName",  "TruthParticles", ""};
  
  Gaudi::Property<std::string> m_prefix
    {this, "Prefix", "", "Decoration prefix to avoid clashes."};

  enum EDecorators {
    kDecorTruthType,
    kDecorTruthOrigin,
    kNDecorators
  };
  std::vector<SG::WriteDecorHandleKey<xAOD::TruthParticleContainer> >   m_decor;
};


#endif
