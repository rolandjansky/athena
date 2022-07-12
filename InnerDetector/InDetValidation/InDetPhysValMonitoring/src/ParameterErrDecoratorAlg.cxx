/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ParameterErrDecoratorAlg.cxx
 * @author shaun roe
 **/
#include "ParameterErrDecoratorAlg.h"
#include "TrkEventPrimitives/JacobianThetaPToCotThetaPt.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

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
  decorationNames[kDecorQoperr]="qoperr";
  std::string message{"The following decorations should be added by ParameterErrDecoratorAlg:\n"};
  for (const auto & thisName: decorationNames){
    message += thisName+"\n";
  }
  ATH_MSG_INFO(message);

  IDPVM::createDecoratorKeysAndAccessor(*this, m_trkParticleKey, m_prefix.value(),decorationNames, m_decor);
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
  std::vector< IDPVM::OptionalDecoration<xAOD::TrackParticleContainer,float> >
     floatDecoration( IDPVM::createDecoratorsIfNeeded(*ptracks, m_decor, ctx, msgLvl(MSG::DEBUG)) );
  if (not floatDecoration.empty()){
    for (const xAOD::TrackParticle *pTrkParticle : *ptracks) {
      if (not decorateTrack(*pTrkParticle, floatDecoration) ) {
	ATH_MSG_ERROR("Decoration failed in ParameterErrDecoratorAlg::execute.");
	return StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::SUCCESS;
}

bool
ParameterErrDecoratorAlg::decorateTrack(const xAOD::TrackParticle& particle,
                                        std::vector<IDPVM::OptionalDecoration<xAOD::TrackParticleContainer, float> > &floatDecoration) const {
  ATH_MSG_VERBOSE("Decorate track with errors ");
  bool success(true);
  const AmgSymMatrix(5)  errorMat = particle.definingParametersCovMatrix();
  double mtheta = particle.theta();
  double mqp = particle.qOverP();
  Trk::JacobianThetaPToCotThetaPt TheJac(mtheta, mqp);
  AmgSymMatrix(5) covVert;
  covVert = errorMat.similarity(TheJac);

  // 'safeDecorator' used to prevent a crash in case of adding something which pre-exists.
  // behaviour chosen is to reject quietly
  float d0err = Amg::error(covVert, Trk::d0);
  float z0err = Amg::error(covVert, Trk::z0);
  float phierr = Amg::error(covVert, Trk::phi);
  float thetaerr = Amg::error(covVert, Trk::theta);
  float qoperr = Amg::error(covVert, Trk::qOverP)*1000.;

  IDPVM::decorateOrRejectQuietly(particle,floatDecoration[kDecorD0err],d0err);
  IDPVM::decorateOrRejectQuietly(particle,floatDecoration[kDecorZ0err],z0err);
  IDPVM::decorateOrRejectQuietly(particle,floatDecoration[kDecorPhierr],phierr);
  IDPVM::decorateOrRejectQuietly(particle,floatDecoration[kDecorThetaerr],thetaerr);
  IDPVM::decorateOrRejectQuietly(particle,floatDecoration[kDecorQoperr],qoperr);

  return success;
}
