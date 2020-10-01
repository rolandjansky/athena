/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_ParameterErrDecoratorAlg_H
#define INDETPHYSVALMONITORING_ParameterErrDecoratorAlg_H

/**
 * @file ParameterErrDecoratorAlg.h
 * header file for class of same name
 * @author shaun roe
 * @date 25 April 2014
 **/

#include "xAODTracking/TrackParticleContainer.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include <string>
#include <vector>
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"
#include "GaudiKernel/EventContext.h"

// class to decorate xAOD::TrackParticles with errors on the defining parameters
class ParameterErrDecoratorAlg: public AthReentrantAlgorithm {
public:
  ParameterErrDecoratorAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual
  ~ParameterErrDecoratorAlg();
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute(const EventContext &ctx) const;
private:
  virtual bool decorateTrack(const xAOD::TrackParticle& particle,
                             std::vector< SG::WriteDecorHandle<xAOD::TrackParticleContainer,float> > &float_decor) const;

  Gaudi::Property<std::string> m_prefix
    {this, "Prefix", "", "Decoration prefix to avoid clashes."};

  // need tracks particle key, to compose decoration keys
  SG::ReadHandleKey<xAOD::TrackParticleContainer>  m_trkParticleKey
    {this,"TrackParticleContainerName", "InDetTrackParticles",""};

  enum EDecorations {
    kDecorD0err,
    kDecorZ0err,
    kDecorPhierr,
    kDecorThetaerr,
    kDecorQopterr,
    kNDecorators
  };
  std::vector< SG::WriteDecorHandleKey<xAOD::TrackParticleContainer> > m_decor;

};

#endif
