/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  ATH_CHECK( m_trkParticleName.initialize() );

  std::vector<std::string> decor_names(kNDecorators);
  decor_names[kDecorD0err]="d0err";
  decor_names[kDecorZ0err]="z0err";
  decor_names[kDecorPhierr]="phierr";
  decor_names[kDecorThetaerr]="thetaerr";
  decor_names[kDecorQopterr]="qopterr";

  IDPVM::createDecoratorKeys(*this, m_trkParticleName, m_prefix, decor_names, m_decor);
  assert( m_decor.size() == kNDecorators);

  return StatusCode::SUCCESS;
}

StatusCode
ParameterErrDecoratorAlg::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode
ParameterErrDecoratorAlg::execute(const EventContext &ctx) const {
  SG::ReadHandle<xAOD::TrackParticleContainer> ptracks(m_trkParticleName, ctx);
  if ((not ptracks.isValid())) {
    return StatusCode::FAILURE;
  }

  std::vector< SG::WriteDecorHandle<xAOD::TrackParticleContainer,float> >
    float_decor( IDPVM::createDecorators<xAOD::TrackParticleContainer,float>(m_decor, ctx) );

  for (const xAOD::TrackParticle *trk_particle : *ptracks) {
    if (not decorateTrack(*trk_particle, float_decor) ) return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

bool
ParameterErrDecoratorAlg::decorateTrack(const xAOD::TrackParticle& particle,
                                         std::vector< SG::WriteDecorHandle<xAOD::TrackParticleContainer,float> > &float_decor) const {
  ATH_MSG_VERBOSE("Decorate track with errors ");
  bool success(true);
  const AmgSymMatrix(5)  errorMat = particle.definingParametersCovMatrix();
  double mtheta = particle.theta();
  double mqp = particle.qOverP();
  Trk::JacobianThetaPToCotThetaPt TheJac(mtheta, mqp);
  AmgSymMatrix(5) covVert;
  covVert = errorMat.similarity(TheJac);
  float_decor[kDecorD0err](particle)=Amg::error(covVert, 0);
  float_decor[kDecorZ0err](particle)=Amg::error(covVert, 1);
  float_decor[kDecorPhierr](particle)=Amg::error(covVert, 2);
  float_decor[kDecorThetaerr](particle)=Amg::error(errorMat, Trk::theta);
  float_decor[kDecorQopterr](particle)=Amg::error(covVert, 4) * 1000.;
  return success;
}

/**
   AmgVector(5) perigeeParams = startPerigee->parameters();
      trkd0    = perigeeParams[Trk::d0];
      trkz0    = perigeeParams[Trk::z0];
      trkphi   = perigeeParams[Trk::phi0];
      trktheta = perigeeParams[Trk::theta];
      //trkcot   = 1./tan(trktheta);
      trketa   = - log(tan(trktheta / 2.0));
      trkqOverPt  = perigeeParams[Trk::qOverP]*1000./sin(trktheta);
      trkpt    = abs(1.0 / trkqOverPt);
      if (trkqOverPt<0) charge=-1;
      else charge=+1;
      const AmgSymMatrix(5) * ErrorMat = startPerigee->covariance();
      double mtheta = (startPerigee->parameters())[Trk::theta];
      double mqp = (startPerigee->parameters())[Trk::qOverP];
      Trk::JacobianThetaPToCotThetaPt TheJac(mtheta,mqp);
      AmgSymMatrix(5) covVert;
      covVert = ErrorMat->similarity(TheJac);
      // trkd0err = sqrt(covVert[0][0]);
      // trkz0err = sqrt(covVert[1][1]);
      // trkphierr = sqrt(covVert[2][2]);
      // trkcoterr = sqrt(covVert[3][3]);
      // trkqopterr = sqrt(covVert[4][4])*1000.;
      // trkthetaerr = ErrorMat->error(Trk::theta);
      trkd0err = Amg::error(covVert,0);
      trkz0err = Amg::error(covVert,1);
      trkphierr = Amg::error(covVert,2);
      //trkcoterr = Amg::error(covVert,3);
      trkqopterr = Amg::error(covVert,4)*1000.;
      trkthetaerr = Amg::error((*ErrorMat),Trk::theta);
 **/
