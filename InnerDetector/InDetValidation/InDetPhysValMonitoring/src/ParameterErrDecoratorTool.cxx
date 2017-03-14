/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ParameterErrDecoratorTool.cxx
 * @author shaun roe
 **/
#include "ParameterErrDecoratorTool.h"
#include "TrkEventPrimitives/JacobianThetaPToCotThetaPt.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"



ParameterErrDecoratorTool::ParameterErrDecoratorTool(const std::string& type, const std::string& name,
                                                     const IInterface* parent) :
  AthAlgTool(type, name, parent) {
  declareInterface<IInDetPhysValDecoratorTool>(this);
}

ParameterErrDecoratorTool::~ParameterErrDecoratorTool() {
// nop
}

StatusCode
ParameterErrDecoratorTool::initialize() {
  if (AlgTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }

  //

  return StatusCode::SUCCESS;
}

StatusCode
ParameterErrDecoratorTool::finalize() {
  return StatusCode::SUCCESS;
}

bool
ParameterErrDecoratorTool::decorateTrack(const xAOD::TrackParticle& particle, const std::string& prefix) {
  ATH_MSG_VERBOSE("Decorate track with errors ");
  bool success(true);
  const AmgSymMatrix(5)  errorMat = particle.definingParametersCovMatrix();
  double mtheta = particle.theta();
  double mqp = particle.qOverP();
  Trk::JacobianThetaPToCotThetaPt TheJac(mtheta, mqp);
  AmgSymMatrix(5) covVert;
  covVert = errorMat.similarity(TheJac);
  float trkd0err = Amg::error(covVert, 0);
  float trkz0err = Amg::error(covVert, 1);
  float trkphierr = Amg::error(covVert, 2);
  float trkthetaerr = Amg::error(errorMat, Trk::theta);
  float trkqopterr = Amg::error(covVert, 4) * 1000.;
  //
  particle.auxdecor<float>(prefix + "d0err") = trkd0err;
  particle.auxdecor<float>(prefix + "z0err") = trkz0err;
  particle.auxdecor<float>(prefix + "phierr") = trkphierr;
  particle.auxdecor<float>(prefix + "thetaerr") = trkthetaerr;
  particle.auxdecor<float>(prefix + "qopterr") = trkqopterr;
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
