/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ParameterErrDecoratorAlg.cxx
 * @author shaun roe
 **/
#include "ParameterErrDecoratorAlg.h"
#include "TrkEventPrimitives/JacobianThetaPToCotThetaPt.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include "safeDecorator.h"

ParameterErrDecoratorAlg::ParameterErrDecoratorAlg(const std::string& name, ISvcLocator* pSvcLocator) :
AthReentrantAlgorithm(name, pSvcLocator) {
}

ParameterErrDecoratorAlg::~ParameterErrDecoratorAlg() {
// nop
}

StatusCode
ParameterErrDecoratorAlg::initialize() {
  ATH_CHECK( m_trkParticleKey.initialize() );

  std::vector<std::string> decorationNames(kNDecorators);
  decorationNames[kDecorD0err]="d0err";
  decorationNames[kDecorZ0err]="z0err";
  decorationNames[kDecorPhierr]="phierr";
  decorationNames[kDecorThetaerr]="thetaerr";
  decorationNames[kDecorQopterr]="qopterr";
  std::string message{"The following decorations should be added by ParameterErrDecoratorAlg:\n"};
  for (const auto & thisName: decorationNames){
    message += thisName+"\n";
  }
  ATH_MSG_INFO(message);
  IDPVM::createDecoratorKeys(*this, m_trkParticleKey, m_prefix, decorationNames, m_decor);
  assert( m_decor.size() == kNDecorators);
  
  return StatusCode::SUCCESS;
}

StatusCode
ParameterErrDecoratorAlg::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode
ParameterErrDecoratorAlg::execute(const EventContext &ctx) const {
  SG::ReadHandle<xAOD::TrackParticleContainer> ptracks(m_trkParticleKey, ctx);
  if ((not ptracks.isValid())) {
    ATH_MSG_ERROR("The track particle container '"<< m_trkParticleKey.key()<<"' could not be read.");
    return StatusCode::FAILURE;
  }
  std::vector< SG::WriteDecorHandle<xAOD::TrackParticleContainer,float> >
    floatDecoration( IDPVM::createDecorators<xAOD::TrackParticleContainer,float>(m_decor, ctx) );
  for (const xAOD::TrackParticle *pTrkParticle : *ptracks) {
    if (not decorateTrack(*pTrkParticle, floatDecoration) ) {
      ATH_MSG_ERROR("Decoration failed in ParameterErrDecoratorAlg::execute.");
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

bool
ParameterErrDecoratorAlg::decorateTrack(const xAOD::TrackParticle& particle,
                                         std::vector< SG::WriteDecorHandle<xAOD::TrackParticleContainer,float> > &floatDecoration) const {
  ATH_MSG_VERBOSE("Decorate track with errors ");
  bool success(true);
  const AmgSymMatrix(5)  errorMat = particle.definingParametersCovMatrix();
  double mtheta = particle.theta();
  double mqp = particle.qOverP();
  Trk::JacobianThetaPToCotThetaPt TheJac(mtheta, mqp);
  AmgSymMatrix(5) covVert;
  covVert = errorMat.similarity(TheJac);
  floatDecoration[kDecorD0err](particle)=Amg::error(covVert, 0);
  floatDecoration[kDecorZ0err](particle)=Amg::error(covVert, 1);
  floatDecoration[kDecorPhierr](particle)=Amg::error(covVert, 2);
  floatDecoration[kDecorThetaerr](particle)=Amg::error(errorMat, Trk::theta);
  floatDecoration[kDecorQopterr](particle)=Amg::error(covVert, 4) * 1000.;
  return success;
}
