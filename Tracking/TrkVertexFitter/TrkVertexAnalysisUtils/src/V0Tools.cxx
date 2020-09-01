/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
        V0Tools.cxx - Description in header file

    authors : Evelina Bouhova-Thacker (Lancater University)
    email   : e.bouhova@cern.ch
 
*********************************************************************/

#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "TrkParametersBase/ParametersBase.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "CLHEP/Vector/LorentzVector.h"
namespace Trk
{

  V0Tools::V0Tools(const std::string& t, const std::string& n, const IInterface*  p) : 
   AthAlgTool(t,n,p),
   m_extrapolator("Trk::Extrapolator") 
  {
    declareProperty("Extrapolator", m_extrapolator);
    declareInterface<V0Tools>(this);
  }
 
  V0Tools::~V0Tools() {}
 
  StatusCode V0Tools::initialize()
  {

  //get the extrapolator
  if ( m_extrapolator.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_extrapolator << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endmsg;
  }

    msg(MSG::INFO) << "Initialize successful" << endmsg;
    return StatusCode::SUCCESS; 
  }
 
  StatusCode V0Tools::finalize()
  {
    msg(MSG::INFO) << "Finalize successful" << endmsg;
    return StatusCode::SUCCESS; 
  }


  const xAOD::Vertex * V0Tools::v0Link(const xAOD::Vertex * vxCandidate) const
  {
    const xAOD::Vertex* v0(nullptr);
    const static SG::AuxElement::Accessor< ElementLink< xAOD::VertexContainer > > acc( "V0Link" );
    if ((acc(*vxCandidate)).isValid()) {
      v0 = *(acc(*vxCandidate));
    }
    return v0;
  }

  const xAOD::Vertex * V0Tools::kshortLink(const xAOD::Vertex * vxCandidate) const
  {
    const xAOD::Vertex* v0(nullptr);
    const static SG::AuxElement::Accessor< ElementLink< xAOD::VertexContainer > > acc( "KshortLink" );
    if ((acc(*vxCandidate)).isValid()) {
      v0 = *(acc(*vxCandidate));
    }
    return v0;
  }

  const xAOD::Vertex * V0Tools::lambdaLink(const xAOD::Vertex * vxCandidate) const
  {
    const xAOD::Vertex* v0(nullptr);
    const static SG::AuxElement::Accessor< ElementLink< xAOD::VertexContainer > > acc( "LambdaLink" );
    if ((acc(*vxCandidate)).isValid()) {
      v0 = *(acc(*vxCandidate));
    }
    return v0;
  }

  const xAOD::Vertex * V0Tools::lambdabarLink(const xAOD::Vertex * vxCandidate) const
  {
    const xAOD::Vertex* v0(nullptr);
    const static SG::AuxElement::Accessor< ElementLink< xAOD::VertexContainer > > acc( "LambdabarLink" );
    if ((acc(*vxCandidate)).isValid()) {
      v0 = *(acc(*vxCandidate));
    }
    return v0;
  }

  double V0Tools::invariantMass(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    std::vector<double> masses = {posTrackMass, negTrackMass};

    return invariantMass(vxCandidate,masses);
  }



  double V0Tools::invariantMass(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses) const
  {
    double px = 0., py = 0., pz = 0., e = 0.;
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    for( unsigned int it=0; it<NTrk; it++) {
      if (masses[it] >= 0.) {
        xAOD::TrackParticle::FourMom_t lorentz_trk = track4Momentum(vxCandidate,it,masses[it]);
        px += lorentz_trk.Px();
        py += lorentz_trk.Py();
        pz += lorentz_trk.Pz();
         e += lorentz_trk.E();
      }
    }
    double msq = e*e - px*px - py*py - pz*pz;
    return (msq>0.) ? sqrt(msq) : 0.;
  }

  double V0Tools::invariantMassError(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    std::vector<double> masses = {posTrackMass, negTrackMass};

    return invariantMassError(vxCandidate,masses);
  }
 
  double V0Tools::invariantMassError(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses) const
  {
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    double error = -999999.;
    Amg::MatrixX* fullCov = convertCovMatrix(vxCandidate);
    if (fullCov == nullptr) {
      ATH_MSG_DEBUG("0 pointer for full covariance. Making-up one from the vertex and tracks covariances");
      error = massErrorVxCandidate(vxCandidate,masses);
    } else {
      unsigned int ndim = fullCov->rows();
      if (ndim != 0) {
        if (ndim == 5*NTrk+3 || ndim == 5*NTrk+6) {
          error = massErrorV0Fitter(vxCandidate,masses);
        } else if (ndim == 3*NTrk+3) {
          error = massErrorVKalVrt(vxCandidate,masses);
        }
      }
    }
    delete fullCov;
    return error;
  }
 
  double V0Tools::massErrorV0Fitter(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    std::vector<double> masses = {posTrackMass, negTrackMass};

    return massErrorV0Fitter(vxCandidate,masses);
  }

  double V0Tools::massErrorV0Fitter(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses) const
  {
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    Amg::MatrixX* fullCov = convertCovMatrix(vxCandidate);
    if (fullCov == nullptr) return -999999.;
    unsigned int ndim = fullCov->rows();
    double E=0., Px=0., Py=0., Pz=0.; 
    std::vector<double>phi(NTrk), theta(NTrk), qOverP(NTrk), charge(NTrk), e(NTrk);
    std::vector<double>dm2dphi(NTrk), dm2dtheta(NTrk), dm2dqOverP(NTrk);
    for( unsigned int it=0; it<NTrk; it++) {
      if (masses[it] >= 0.) {
        const Trk::TrackParameters* bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
        double trkCharge = 1.;
        if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
        charge[it] = trkCharge;
        phi[it]    = bPer->parameters()[Trk::phi];
        theta[it]  = bPer->parameters()[Trk::theta];
        qOverP[it] = bPer->parameters()[Trk::qOverP];
        double tmp = 1./(qOverP[it]*qOverP[it]) + masses[it]*masses[it];
        double pe = (tmp>0.) ? sqrt(tmp) : 0.;
        e[it] = pe;
        E  += e[it];
        Px += bPer->momentum()[Trk::px];
        Py += bPer->momentum()[Trk::py];
        Pz += bPer->momentum()[Trk::pz];
      }
    }
    double msq = E*E - Px*Px - Py*Py - Pz*Pz;
    double mass = (msq>0.) ? sqrt(msq) : 0.;

    for( unsigned int it=0; it<NTrk; it++) {
      if (masses[it] >= 0.) {
        dm2dphi[it]    = 2.*(Px*sin(phi[it])-Py*cos(phi[it]))*sin(theta[it])*charge[it]/qOverP[it];
        dm2dtheta[it]  = 2.*(Pz*sin(theta[it])-(Px*cos(phi[it])+Py*sin(phi[it]))*cos(theta[it]))*charge[it]/qOverP[it];
        dm2dqOverP[it] = 2.*(Pz*cos(theta[it])+(Px*cos(phi[it])+Py*sin(phi[it]))*sin(theta[it])-E*charge[it]/(qOverP[it]*e[it]))*charge[it]/(qOverP[it]*qOverP[it]);
      }
    }

    Amg::MatrixX D_vec(5*NTrk,1); D_vec.setZero();
    for( unsigned int it=0; it<NTrk; it++) {
      D_vec(5*it+0,0)  = 0.;
      D_vec(5*it+1,0)  = 0.;
      D_vec(5*it+2,0)  = dm2dphi[it];
      D_vec(5*it+3,0)  = dm2dtheta[it];
      D_vec(5*it+4,0)  = dm2dqOverP[it];
    }
    Amg::MatrixX V0_merr = D_vec.transpose() * fullCov->block(0,0,ndim-3,ndim-3) * D_vec;
    delete fullCov;

    double massVarsq = V0_merr(0,0);
    if (massVarsq <= 0.) ATH_MSG_DEBUG("massError: negative sqrt massVarsq " << massVarsq);
    double massVar = (massVarsq>0.) ? sqrt(massVarsq) : 0.;
    double massErr = massVar/(2.*mass);
    return massErr;
  }
 
  double V0Tools::massErrorVKalVrt(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    std::vector<double> masses = {posTrackMass, negTrackMass};
    return massErrorVKalVrt(vxCandidate,masses);
  }

  double V0Tools::massErrorVKalVrt(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses) const
  {
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    std::vector<xAOD::TrackParticle::FourMom_t> particleMom(NTrk);
    std::vector<Amg::MatrixX> particleDeriv(NTrk);
    xAOD::TrackParticle::FourMom_t totalMom;
    Amg::MatrixX tmpDeriv(3,3); tmpDeriv.setZero();
    Amg::MatrixX* fullCov = convertCovMatrix(vxCandidate);
    if (fullCov == nullptr) return -999999.;
 
    for( unsigned int it=0; it<NTrk; it++){
      if (masses[it] >= 0.) {
        const Trk::TrackParameters* bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
        double phi   =  bPer->parameters()[Trk::phi];
        double theta =  bPer->parameters()[Trk::theta];
        double invP  =  bPer->parameters()[Trk::qOverP];
        double px = cos(phi)*sin(theta)/fabs(invP);
        double py = sin(phi)*sin(theta)/fabs(invP);
        double pz = cos(theta)/fabs(invP);
        double esq = px*px + py*py + pz*pz + masses[it]*masses[it];
        double e = (esq>0.) ? sqrt(esq) : 0.;
        xAOD::TrackParticle::FourMom_t tmp(0,0,0,0);
        tmp.SetPxPyPzE(px,py,pz,e);
        particleMom[it] = tmp;
        totalMom += tmp;

//  d(Px,Py,Pz)/d(Phi,Theta,InvP)
        tmpDeriv(0,0) = - tmp.Py();
        tmpDeriv(1,0) =   tmp.Px();
        tmpDeriv(2,0) =   0.;
        tmpDeriv(0,1) =   cos(phi) * tmp.Pz();
        tmpDeriv(1,1) =   sin(phi) * tmp.Pz();
        tmpDeriv(2,1) = - sin(theta)/fabs(invP);
        tmpDeriv(0,2) = - tmp.Px()/invP;
        tmpDeriv(1,2) = - tmp.Py()/invP;
        tmpDeriv(2,2) = - tmp.Pz()/invP;
        particleDeriv[it] = tmpDeriv;
      }
    }
  
    std::vector<double> Deriv(3*NTrk+3, 0.);
    for(unsigned int it=0; it<NTrk; it++){
      if (masses[it] >= 0.) {
        double dMdPx = ( totalMom.E() * particleMom[it].Px()/particleMom[it].E() - totalMom.Px() ) / totalMom.M();
        double dMdPy = ( totalMom.E() * particleMom[it].Py()/particleMom[it].E() - totalMom.Py() ) / totalMom.M();
        double dMdPz = ( totalMom.E() * particleMom[it].Pz()/particleMom[it].E() - totalMom.Pz() ) / totalMom.M();
  
        double dMdPhi   = dMdPx*particleDeriv[it](0,0) + dMdPy*particleDeriv[it](1,0) + dMdPz*particleDeriv[it](2,0);
        double dMdTheta = dMdPx*particleDeriv[it](0,1) + dMdPy*particleDeriv[it](1,1) + dMdPz*particleDeriv[it](2,1);
        double dMdInvP  = dMdPx*particleDeriv[it](0,2) + dMdPy*particleDeriv[it](1,2) + dMdPz*particleDeriv[it](2,2);
  
        Deriv[3*it + 3 + 0] = dMdPhi;    Deriv[3*it + 3 + 1] = dMdTheta; Deriv[3*it + 3 + 2] = dMdInvP;
      }
    }

    double err = 0;
    for(unsigned int i=0; i<3*NTrk+3; i++){
      for(unsigned int j=0; j<3*NTrk+3; j++){
        err += Deriv[i]*( (*fullCov))(i,j)*Deriv[j];
      }
    }
    delete fullCov;
    if (err <= 0.) ATH_MSG_DEBUG("massError: negative sqrt err " << err);
    return (err>0.) ? sqrt(err) : 0.;
  }

  double V0Tools::massErrorVxCandidate(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    std::vector<double> masses = {posTrackMass, negTrackMass};

    return massErrorVxCandidate(vxCandidate,masses);
  }
 
  double V0Tools::massErrorVxCandidate(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses) const
  {
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    double E=0., Px=0., Py=0., Pz=0.; 
    std::vector<double>phi(NTrk), theta(NTrk), qOverP(NTrk), charge(NTrk), e(NTrk);
    std::vector<double>dm2dphi(NTrk), dm2dtheta(NTrk), dm2dqOverP(NTrk);
    Amg::MatrixX V0_cor(5*NTrk,5*NTrk); V0_cor.setZero();
    for( unsigned int it=0; it<NTrk; it++) {
      if (masses[it] >= 0.) {
        const Trk::TrackParameters* bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
        const AmgSymMatrix(5)* cov_tmp = bPer->covariance();
        V0_cor(5*it+2,5*it+2) = (*cov_tmp)(2,2);
        V0_cor(5*it+2,5*it+3) = (*cov_tmp)(2,3);
        V0_cor(5*it+2,5*it+4) = (*cov_tmp)(2,4);
        V0_cor(5*it+3,5*it+3) = (*cov_tmp)(3,3);
        V0_cor(5*it+3,5*it+4) = (*cov_tmp)(3,4);
        V0_cor(5*it+4,5*it+4) = (*cov_tmp)(4,4);
        V0_cor(5*it+3,5*it+2) = (*cov_tmp)(2,3);
        V0_cor(5*it+4,5*it+2) = (*cov_tmp)(2,4);
        V0_cor(5*it+4,5*it+3) = (*cov_tmp)(3,4);
        double trkCharge = 1.;
        if (bPer->parameters()(Trk::qOverP) < 0.) trkCharge = -1.;
        charge[it] = trkCharge;
        phi[it]    = bPer->parameters()(Trk::phi);
        theta[it]  = bPer->parameters()(Trk::theta);
        qOverP[it] = bPer->parameters()(Trk::qOverP);
        double tmp = 1./(qOverP[it]*qOverP[it]) + masses[it]*masses[it];
        double pe = (tmp>0.) ? sqrt(tmp) : 0.;
        e[it] = pe;
        E  += e[it];
        Px += bPer->momentum()(Trk::px);
        Py += bPer->momentum()(Trk::py);
        Pz += bPer->momentum()(Trk::pz);
      }
    }
    double msq = E*E - Px*Px - Py*Py - Pz*Pz;
    double mass = (msq>0.) ? sqrt(msq) : 0.;

    for( unsigned int it=0; it<NTrk; it++) {
      if (masses[it] >= 0.) {
        dm2dphi[it]    = 2.*(Px*sin(phi[it])-Py*cos(phi[it]))*sin(theta[it])*charge[it]/qOverP[it];
        dm2dtheta[it]  = 2.*(Pz*sin(theta[it])-(Px*cos(phi[it])+Py*sin(phi[it]))*cos(theta[it]))*charge[it]/qOverP[it];
        dm2dqOverP[it] = 2.*(Pz*cos(theta[it])+(Px*cos(phi[it])+Py*sin(phi[it]))*sin(theta[it])-E*charge[it]/(qOverP[it]*e[it]))*charge[it]/(qOverP[it]*qOverP[it]);
      }
    }

    Amg::MatrixX D_vec(5*NTrk,1); D_vec.setZero();
    for( unsigned int it=0; it<NTrk; it++) {
      D_vec(5*it+0,0)  = 0.;
      D_vec(5*it+1,0)  = 0.;
      D_vec(5*it+2,0)  = dm2dphi[it];
      D_vec(5*it+3,0)  = dm2dtheta[it];
      D_vec(5*it+4,0)  = dm2dqOverP[it];
    }

    Amg::MatrixX V0_merr = D_vec.transpose() * V0_cor * D_vec;

    double massVarsq = V0_merr(0,0);
    if (massVarsq <= 0.) ATH_MSG_DEBUG("massError: negative sqrt massVarsq " << massVarsq);
    double massVar = (massVarsq>0.) ? sqrt(massVarsq) : 0.;
    double massErr = massVar/(2.*mass);
    return massErr;
  }

  double V0Tools::invariantMassProbability(const xAOD::Vertex * vxCandidate, double V0Mass, double posTrackMass, double negTrackMass) const
  {
    std::vector<double> masses = {posTrackMass , negTrackMass};

    return invariantMassProbability(vxCandidate,V0Mass,masses);
  }
 
  double V0Tools::invariantMassProbability(const xAOD::Vertex * vxCandidate, double V0Mass, const std::vector<double> &masses) const
  {
    double mass = invariantMass(vxCandidate, masses);
    double massErr = invariantMassError(vxCandidate, masses);
    if(massErr > 0.)
    {
      double chi2 = (V0Mass - mass)*(V0Mass - mass)/(massErr*massErr);
      int ndf = 1;
      Genfun::CumulativeChiSquare myCumulativeChiSquare(ndf);
      if (chi2 > 0.) {
        double achi2prob = 1.-myCumulativeChiSquare(chi2);
        return achi2prob;
      } else {
        ATH_MSG_DEBUG("chi <= 0");
        return -1.;
      }
    }
    else {
      return -1.;
    }
  }
 
  double V0Tools::massProbability(double V0Mass, double mass, double massErr) const
  {
    if(massErr > 0.)
    {
      double chi2 = (V0Mass - mass)*(V0Mass - mass)/(massErr*massErr);
      int ndf = 1;
      Genfun::CumulativeChiSquare myCumulativeChiSquare(ndf);
      if (chi2 > 0.) {
        double achi2prob = 1.-myCumulativeChiSquare(chi2);
        return achi2prob;
      } else {
        ATH_MSG_DEBUG("chi <= 0");
        return -1.;
      }
    }
    else {
      return -1.;
    }
  }

  Amg::Vector3D V0Tools::trackMomentum(const xAOD::Vertex * vxCandidate, unsigned int trkIndex) const
  {
    double px = 0., py = 0., pz = 0.;
    const Trk::TrackParameters* aPerigee = vxCandidate->vxTrackAtVertex()[trkIndex].perigeeAtVertex();
    px = aPerigee->momentum()[Trk::px];
    py = aPerigee->momentum()[Trk::py];
    pz = aPerigee->momentum()[Trk::pz];
    Amg::Vector3D mom(px,py,pz);
    return mom;
  }

  Amg::Vector3D V0Tools::positiveTrackMomentum(const xAOD::Vertex * vxCandidate) const
  {
    Amg::Vector3D mom;
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (NTrk != 2) return mom;
    for( unsigned int it=0; it<NTrk; it++) {
      float trkCharge = vxCandidate->trackParticle(it)->charge();
      if (trkCharge > 0) mom = trackMomentum(vxCandidate,it);
    }
    return mom;
  }

  Amg::Vector3D V0Tools::negativeTrackMomentum(const xAOD::Vertex * vxCandidate) const
  {
    Amg::Vector3D mom;
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (NTrk != 2) return mom;
    for( unsigned int it=0; it<NTrk; it++) {
      float trkCharge = vxCandidate->trackParticle(it)->charge();
      if (trkCharge < 0) mom = trackMomentum(vxCandidate,it);
    }
    return mom;
  }

  Amg::Vector3D V0Tools::V0Momentum(const xAOD::Vertex * vxCandidate) const
  {
    double px = 0., py = 0., pz = 0.;
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    for( unsigned int it=0; it<NTrk; it++) {
      Amg::Vector3D mom_trk = trackMomentum(vxCandidate,it);
      px += mom_trk.x();
      py += mom_trk.y();
      pz += mom_trk.z();
    }
    Amg::Vector3D mom(px,py,pz);
    return mom;
  }

  xAOD::TrackParticle::FourMom_t V0Tools::track4Momentum(const xAOD::Vertex * vxCandidate, unsigned int trkIndex, double mass) const
  {
    Amg::Vector3D mom = trackMomentum(vxCandidate, trkIndex);
    xAOD::TrackParticle::FourMom_t lorentz(0,0,0,0);
    double tmp = mass*mass + mom.x()*mom.x() + mom.y()*mom.y() + mom.z()*mom.z();
    double e = (tmp>0.) ? sqrt(tmp) : 0.;
    lorentz.SetPxPyPzE(mom.x(), mom.y(), mom.z(), e);
    return lorentz;
  }

  xAOD::TrackParticle::FourMom_t V0Tools::positiveTrack4Momentum(const xAOD::Vertex * vxCandidate, double mass) const
  {
    Amg::Vector3D mom = positiveTrackMomentum(vxCandidate);
    xAOD::TrackParticle::FourMom_t lorentz(0,0,0,0);
    double tmp = mass*mass + mom.x()*mom.x() + mom.y()*mom.y() + mom.z()*mom.z();
    double e = (tmp>0.) ? sqrt(tmp) : 0.;
    lorentz.SetPxPyPzE(mom.x(), mom.y(), mom.z(), e);
    return lorentz;
  }

  xAOD::TrackParticle::FourMom_t V0Tools::negativeTrack4Momentum(const xAOD::Vertex * vxCandidate, double mass) const
  {
    Amg::Vector3D mom = negativeTrackMomentum(vxCandidate);
    xAOD::TrackParticle::FourMom_t lorentz(0,0,0,0);
    double tmp = mass*mass + mom.x()*mom.x() + mom.y()*mom.y() + mom.z()*mom.z();
    double e = (tmp>0.) ? sqrt(tmp) : 0.;
    lorentz.SetPxPyPzE(mom.x(), mom.y(), mom.z(), e);
    return lorentz;
  }

  xAOD::TrackParticle::FourMom_t V0Tools::V04Momentum(const xAOD::Vertex * vxCandidate, double V0Mass) const
  {
    Amg::Vector3D mom = V0Momentum(vxCandidate);
    double tmp = V0Mass*V0Mass + mom.x()*mom.x() + mom.y()*mom.y() + mom.z()*mom.z();
    double e = (tmp>0.) ? sqrt(tmp) : 0.;
    xAOD::TrackParticle::FourMom_t lorentz(0,0,0,0);
    lorentz.SetPxPyPzE(mom.x(), mom.y(), mom.z(), e);
    return lorentz;
  }

  float V0Tools::ndof(const xAOD::Vertex * vxCandidate) const
  {
    return vxCandidate->numberDoF();
  }

  float V0Tools::chisq(const xAOD::Vertex * vxCandidate) const
  {
    return vxCandidate->chiSquared();
  }

  double V0Tools::vertexProbability(const xAOD::Vertex * vxCandidate) const
  {
    float dof = ndof(vxCandidate);
    if (dof > 0.) {
      Genfun::CumulativeChiSquare myCumulativeChiSquare(dof);
      float chi = chisq(vxCandidate);
      if (chi > 0.) {
        double chi2prob = 1.-myCumulativeChiSquare(chi);
        return chi2prob;
      } else {
        ATH_MSG_DEBUG("chi <= 0");
        return -1.;
      }
    } else {
      ATH_MSG_DEBUG("dof <= 0");
      return -1.;
    }

  }

  Amg::Vector3D V0Tools::vtx(const xAOD::Vertex * vxCandidate) const
  {
    Amg::Vector3D vertex(vxCandidate->position().x(),vxCandidate->position().y(),vxCandidate->position().z());
    return vertex;
  }

  double V0Tools::rxy(const xAOD::Vertex * vxCandidate) const
  {
    return vxCandidate->position().perp();
  }

  double V0Tools::rxy(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const
  {
    return (vxCandidate->position() - vertex->position()).perp();
  }

  double V0Tools::rxy(const xAOD::Vertex * vxCandidate, const Amg::Vector3D& vertex) const
  {
    return (vxCandidate->position() - vertex).perp();
  }

  double V0Tools::rxy_var(double dx, double dy, const Amg::MatrixX& cov) const
  {
    double rxysq = dx*dx + dy*dy;
    double rxy = (rxysq>0.) ? sqrt(rxysq) : 0.;
    double drdx = dx/rxy;
    double drdy = dy/rxy;
    Amg::MatrixX D_vec(2,1); D_vec.setZero();
    D_vec(0,0)  = drdx;
    D_vec(1,0)  = drdy;
    Amg::MatrixX rxy_err = D_vec.transpose() * cov.block(0,0,2,2) * D_vec;
    double rxyVar = rxy_err(0,0);
    return rxyVar;
  }

  double V0Tools::rxyError(const xAOD::Vertex * vxCandidate) const
  {
    const Amg::MatrixX& cov = vxCandidate->covariancePosition();
    double dx = vxCandidate->position().x();
    double dy = vxCandidate->position().y();
    double rxyVar = rxy_var(dx,dy,cov);
    if (rxyVar <= 0.) ATH_MSG_DEBUG("rxyError: negative sqrt rxyVar " << rxyVar);
    return (rxyVar>0.) ? sqrt(rxyVar) : 0.;
  }

  double V0Tools::rxyError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const
  {
    const Amg::MatrixX cov = vxCandidate->covariancePosition() + vertex->covariancePosition(); 
    auto vert = vxCandidate->position() - vertex->position();
    double dx = vert.x();
    double dy = vert.y();
    double rxyVar = rxy_var(dx,dy,cov);
    if (rxyVar <= 0.) ATH_MSG_DEBUG("rxyError: negative sqrt rxyVar " << rxyVar);
    return (rxyVar>0.) ? sqrt(rxyVar) : 0.;
  }

  double V0Tools::rxyError(const xAOD::Vertex * vxCandidate, const Amg::Vector3D& vertex) const
  {
    const Amg::MatrixX& cov = vxCandidate->covariancePosition();
    auto vert = vxCandidate->position() - vertex;
    double dx = vert.x();
    double dy = vert.y();
    double rxyVar = rxy_var(dx,dy,cov);
    if (rxyVar <= 0.) ATH_MSG_DEBUG("rxyError: negative sqrt rxyVar " << rxyVar);
    return (rxyVar>0.) ? sqrt(rxyVar) : 0.;
  }

  double V0Tools::pT (const xAOD::Vertex * vxCandidate) const
  {
    return V0Momentum(vxCandidate).perp();
  }

  double V0Tools::pTError(const xAOD::Vertex * vxCandidate) const
  {
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    double Px=0., Py=0.; 
    std::vector<double>dpxdqOverP(NTrk), dpxdtheta(NTrk), dpxdphi(NTrk);
    std::vector<double>dpydqOverP(NTrk), dpydtheta(NTrk), dpydphi(NTrk);
    std::vector<double>dPTdqOverP(NTrk), dPTdtheta(NTrk), dPTdphi(NTrk);

    Amg::MatrixX* fullCov = convertCovMatrix(vxCandidate);
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters* bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      double phi    = bPer->parameters()[Trk::phi];
      double theta  = bPer->parameters()[Trk::theta];
      double qOverP = bPer->parameters()[Trk::qOverP];
      dpxdqOverP[it] = -(sin(theta)*cos(phi)*trkCharge)/(qOverP*qOverP);
      dpxdtheta[it]  =  (cos(theta)*cos(phi)*trkCharge)/qOverP;
      dpxdphi[it]    = -(sin(theta)*sin(phi)*trkCharge)/qOverP;
      dpydqOverP[it] = -(sin(theta)*sin(phi)*trkCharge)/(qOverP*qOverP);
      dpydtheta[it]  =  (cos(theta)*sin(phi)*trkCharge)/qOverP;
      dpydphi[it]    =  (sin(theta)*cos(phi)*trkCharge)/qOverP;
      Px += bPer->momentum()[Trk::px];
      Py += bPer->momentum()[Trk::py];
    }
    double PTsq = Px*Px+Py*Py;
    double PT = (PTsq>0.) ? sqrt(PTsq) : 0.;

    for( unsigned int it=0; it<NTrk; it++) {
      dPTdqOverP[it]  = (Px*dpxdqOverP[it]+Py*dpydqOverP[it])/PT;
      dPTdtheta[it]   = (Px*dpxdtheta[it]+Py*dpydtheta[it])/PT;
      dPTdphi[it]     = (Px*dpxdphi[it]+Py*dpydphi[it])/PT;
    }

    unsigned int ndim = 0;
    if (fullCov == nullptr) {
      ndim = 5*NTrk+3;
    } else {
      ndim = fullCov->rows();
    }

    Amg::MatrixX PtErrSq(1,1);
    double PtErrsq = 0.;
    if (ndim == 5*NTrk+3 || ndim == 5*NTrk+6) {
      Amg::MatrixX D_vec(5*NTrk,1); D_vec.setZero();
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(5*it+0,0)  = 0.;
        D_vec(5*it+1,0)  = 0.;
        D_vec(5*it+2,0)  = dPTdphi[it];
        D_vec(5*it+3,0)  = dPTdtheta[it];
        D_vec(5*it+4,0)  = dPTdqOverP[it];
      }
      if (fullCov == nullptr) {
        Amg::MatrixX V0_cov = makeV0Cov(vxCandidate);
        PtErrSq = D_vec.transpose() * V0_cov * D_vec;
      } else {
        PtErrSq = D_vec.transpose() * fullCov->block(0,0,5*NTrk-1,5*NTrk-1) * D_vec;
      }
    } else if (ndim == 3*NTrk+3) {
      Amg::MatrixX D_vec(3*NTrk,1); D_vec.setZero();
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(3*it+0,0)  = dPTdphi[it];
        D_vec(3*it+1,0)  = dPTdtheta[it];
        D_vec(3*it+2,0)  = dPTdqOverP[it];
      }
      PtErrSq = D_vec.transpose() * fullCov->block(3,3,ndim-3,ndim-3) * D_vec;
    }
    delete fullCov;

    PtErrsq = PtErrSq(0,0);
    if (PtErrsq <= 0.) ATH_MSG_DEBUG("ptError: negative sqrt PtErrsq " << PtErrsq);
    return (PtErrsq>0.) ? sqrt(PtErrsq) : 0.;
  }

  Amg::Vector3D V0Tools::pca(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const
  {
    assert(vxCandidate!=0);
    if(nullptr == vxCandidate) {
      Amg::Vector3D p; p.setZero();
      return p;
    }
    const Amg::Vector3D& pv = vertex->position();
    Amg::Vector3D sv = vtx(vxCandidate);
    Amg::Vector3D P = V0Momentum(vxCandidate);
    double p2 = P.mag2();
    double pdr = P.dot((sv - pv));
    return sv - P*pdr/p2;
  }

  double V0Tools::separation(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const
  {
    const Amg::MatrixX& cov = (vxCandidate->covariancePosition() + vertex->covariancePosition()).inverse().eval();
    Amg::Vector3D D_vec; D_vec.setZero();
    auto vert = vxCandidate->position() - vertex->position();
    D_vec(0) = vert.x();
    D_vec(1) = vert.y();
    D_vec(2) = vert.z();
    Amg::MatrixX sepVarsqMat = D_vec.transpose() * cov * D_vec;
    double sepVarsq = sepVarsqMat(0,0); 
    if (sepVarsq <= 0.) ATH_MSG_DEBUG("separation: negative sqrt sepVarsq " << sepVarsq);
    double sepVar = (sepVarsq>0.) ? sqrt(sepVarsq) : 0.;
    return sepVar;
  }

  double V0Tools::separation(const xAOD::Vertex * vxCandidate, const Amg::Vector3D& vertex) const
  {
    const Amg::SymMatrixX& cov = vxCandidate->covariancePosition().inverse().eval();
    Amg::Vector3D D_vec; D_vec.setZero();
    auto vert = vxCandidate->position() - vertex;
    D_vec(0) = vert.x();
    D_vec(1) = vert.y();
    D_vec(2) = vert.z();
    Amg::MatrixX sepVarsqMat = D_vec.transpose() * cov * D_vec;
    double sepVarsq = sepVarsqMat(0,0);
    if (sepVarsq <= 0.) ATH_MSG_DEBUG("separation: negative sqrt sepVarsq " << sepVarsq);
    double sepVar = (sepVarsq>0.) ? sqrt(sepVarsq) : 0.;
    return sepVar;
  }

  double V0Tools::a0xy(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const
  {
    double cosineTheta_xy = cosTheta_xy(vxCandidate,vertex);
    double sinTheta_xy = ((1.-cosineTheta_xy*cosineTheta_xy)>0.) ? sqrt((1.-cosineTheta_xy*cosineTheta_xy)) : 0.;
    return (vtx(vxCandidate)-vertex->position()).perp() * sinTheta_xy;
    //return (vtx(vxCandidate)-vertex->position()).perp() * sqrt(1.-cosineTheta_xy*cosineTheta_xy);
  }

  double V0Tools::a0z(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const
  {
    Amg::Vector3D pv = vertex->position();
    Amg::Vector3D ca_point = pca(vxCandidate,vertex);
    Amg::Vector3D a0_vec = pv - ca_point;
    return a0_vec.z();
  }

  double V0Tools::a0(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const
  {
    double cosineTheta = cosTheta(vxCandidate,vertex);
    double sinTheta = ((1.-cosineTheta*cosineTheta)>0.) ? sqrt((1.-cosineTheta*cosineTheta)) : 0.;
    return (vtx(vxCandidate)-vertex->position()).mag() * sinTheta;
    //return (vtx(vxCandidate)-vertex->position()).mag() * sqrt(1.-cosineTheta*cosineTheta);
  }

  double V0Tools::a0zError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const
  {
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    auto vert = vxCandidate->position() - vertex->position();
    double dx = vert.x();
    double dy = vert.y();
    double dz = vert.z();
    double Px=0., Py=0., Pz=0.;
    std::vector<double>dpxdqOverP(NTrk), dpxdtheta(NTrk), dpxdphi(NTrk);
    std::vector<double>dpydqOverP(NTrk), dpydtheta(NTrk), dpydphi(NTrk);
    std::vector<double>dpzdqOverP(NTrk), dpzdtheta(NTrk);
    std::vector<double>da0dqOverP(NTrk), da0dtheta(NTrk), da0dphi(NTrk);

    Amg::MatrixX* fullCov = convertCovMatrix(vxCandidate);
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters* bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      double phi    = bPer->parameters()[Trk::phi];
      double theta  = bPer->parameters()[Trk::theta];
      double qOverP = bPer->parameters()[Trk::qOverP];
      dpxdqOverP[it] = -(sin(theta)*cos(phi)*trkCharge)/(qOverP*qOverP);
      dpxdtheta[it]  =  (cos(theta)*cos(phi)*trkCharge)/qOverP;
      dpxdphi[it]    = -(sin(theta)*sin(phi)*trkCharge)/qOverP;
      dpydqOverP[it] = -(sin(theta)*sin(phi)*trkCharge)/(qOverP*qOverP);
      dpydtheta[it]  =  (cos(theta)*sin(phi)*trkCharge)/qOverP;
      dpydphi[it]    =  (sin(theta)*cos(phi)*trkCharge)/qOverP;
      dpzdqOverP[it] = -(cos(theta)*trkCharge)/(qOverP*qOverP);
      dpzdtheta[it]  = -(sin(theta)*trkCharge)/qOverP;
      Px += bPer->momentum()[Trk::px];
      Py += bPer->momentum()[Trk::py];
      Pz += bPer->momentum()[Trk::pz];
    }
    double P2 = Px*Px+Py*Py+Pz*Pz;
    double B = Px*dx+Py*dy+Pz*dz;

    double da0dx  = (Px*Pz)/P2;
    double da0dy  = (Py*Pz)/P2;
    double da0dz  = (Pz*Pz)/P2 - 1.;
    double da0dx0 = -da0dx;
    double da0dy0 = -da0dy;
    double da0dz0 = -da0dz;
    for( unsigned int it=0; it<NTrk; it++) {
      double dP2dqOverP = 2.*(Px*dpxdqOverP[it]+Py*dpydqOverP[it]+Pz*dpzdqOverP[it]);
      double dP2dtheta  = 2.*(Px*dpxdtheta[it]+Py*dpydtheta[it]+Pz*dpzdtheta[it]);
      double dP2dphi    = 2.*(Px*dpxdphi[it]+Py*dpydphi[it]);
      da0dqOverP[it] =  (B*(P2*dpzdqOverP[it]-Pz*dP2dqOverP) +
                         Pz*P2*(dx*dpxdqOverP[it]+dy*dpydqOverP[it]+dz*dpzdqOverP[it]))/(P2*P2);
      da0dtheta[it]  =  (B*(P2*dpzdtheta[it]-Pz*dP2dtheta) +
                         Pz*P2*(dx*dpxdtheta[it]+dy*dpydtheta[it]+dz*dpzdtheta[it]))/(P2*P2);
      da0dphi[it]    = -(B*Pz*dP2dphi -
                         Pz*P2*(dx*dpxdphi[it]+dy*dpydphi[it]))/(P2*P2);
    }

    unsigned int ndim = 0;
    if (fullCov != nullptr) {
      ndim = fullCov->rows();
    } else {
      ndim = 5*NTrk+3;
    }

    Amg::MatrixX V0_err;
    if (ndim == 5*NTrk+3 || ndim == 5*NTrk+6) {
      Amg::MatrixX D_vec(5*NTrk+6,1); D_vec.setZero();
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(5*it+0)  = 0.;
        D_vec(5*it+1)  = 0.;
        D_vec(5*it+2)  = da0dphi[it];
        D_vec(5*it+3)  = da0dtheta[it];
        D_vec(5*it+4)  = da0dqOverP[it];
      }
      D_vec(5*NTrk+0) = da0dx;
      D_vec(5*NTrk+1) = da0dy;
      D_vec(5*NTrk+2) = da0dz;
      D_vec(5*NTrk+3) = da0dx0;
      D_vec(5*NTrk+4) = da0dy0;
      D_vec(5*NTrk+5) = da0dz0;

      Amg::MatrixX W_mat(5*NTrk+6,5*NTrk+6); W_mat.setZero();
      if (fullCov != nullptr) {
        W_mat.block(0,0,ndim,ndim) = *fullCov;
      } else {
        Amg::MatrixX V0_cov = makeV0Cov(vxCandidate);
        W_mat.block(0,0,V0_cov.rows(),V0_cov.rows()) = V0_cov;
        W_mat.block(5*NTrk,5*NTrk,3,3) = vxCandidate->covariancePosition();
      }
      W_mat.block(5*NTrk+3,5*NTrk+3,3,3) =  vertex->covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    } else if (ndim == 3*NTrk+3) {
      Amg::MatrixX D_vec(3*NTrk+6,1); D_vec.setZero();
      D_vec(0)  = da0dx;
      D_vec(1)  = da0dy;
      D_vec(2)  = da0dz;
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(3*it+3)  = da0dphi[it];
        D_vec(3*it+4)  = da0dtheta[it];
        D_vec(3*it+5)  = da0dqOverP[it];
      }
      D_vec(3*NTrk+3) = da0dx0;
      D_vec(3*NTrk+4) = da0dy0;
      D_vec(3*NTrk+5) = da0dz0;

      Amg::MatrixX W_mat(3*NTrk+6,3*NTrk+6); W_mat.setZero();
      W_mat.block(0,0,ndim,ndim) = *fullCov;
      W_mat.block(3*NTrk+3,3*NTrk+3,3,3) =  vertex->covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    }

    double a0Errsq = V0_err(0,0);
    if (a0Errsq <= 0.) ATH_MSG_DEBUG("a0Error: negative sqrt a0Errsq " << a0Errsq);
    double a0Err = (a0Errsq>0.) ? sqrt(a0Errsq) : 0.;
    delete fullCov;
    return a0Err;
  }

  double V0Tools::a0xyError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const
  {
    return a0Error(vxCandidate, vertex, false);
  }

  double V0Tools::a0Error(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, bool in3D) const
  {
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    auto vert = vxCandidate->position() - vertex->position();
    double dx = vert.x();
    double dy = vert.y();
    double dz = vert.z();
    double Px=0., Py=0., Pz=0.;
    std::vector<double>dpxdqOverP(NTrk), dpxdtheta(NTrk), dpxdphi(NTrk);
    std::vector<double>dpydqOverP(NTrk), dpydtheta(NTrk), dpydphi(NTrk);
    std::vector<double>dpzdqOverP(NTrk), dpzdtheta(NTrk);
    std::vector<double>da0dqOverP(NTrk), da0dtheta(NTrk), da0dphi(NTrk);

    Amg::MatrixX* fullCov = convertCovMatrix(vxCandidate);
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters* bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      double phi    = bPer->parameters()[Trk::phi];
      double theta  = bPer->parameters()[Trk::theta];
      double qOverP = bPer->parameters()[Trk::qOverP];
      dpxdqOverP[it] = -(sin(theta)*cos(phi)*trkCharge)/(qOverP*qOverP);
      dpxdtheta[it]  =  (cos(theta)*cos(phi)*trkCharge)/qOverP;
      dpxdphi[it]    = -(sin(theta)*sin(phi)*trkCharge)/qOverP;
      dpydqOverP[it] = -(sin(theta)*sin(phi)*trkCharge)/(qOverP*qOverP);
      dpydtheta[it]  =  (cos(theta)*sin(phi)*trkCharge)/qOverP;
      dpydphi[it]    =  (sin(theta)*cos(phi)*trkCharge)/qOverP;
      if ( in3D ) {
        dpzdqOverP[it] = -(cos(theta)*trkCharge)/(qOverP*qOverP);
        dpzdtheta[it]  = -(sin(theta)*trkCharge)/qOverP;
      }
      Px += bPer->momentum()[Trk::px];
      Py += bPer->momentum()[Trk::py];
      Pz += bPer->momentum()[Trk::pz];
    }
    double cosineTheta;
    double a0val;
    if ( in3D ) {
      cosineTheta = cosTheta(vxCandidate,vertex);
      a0val = a0(vxCandidate,vertex);
    } else { // transforms momentum and vertex separation to transverse plane and leads also to zero dXdz derivatives
      cosineTheta = cosTheta_xy(vxCandidate,vertex);
      a0val = a0xy(vxCandidate,vertex);
      Pz = 0.;
      dz = 0.;
    }
    double P = sqrt(Px*Px+Py*Py+Pz*Pz);
    double r = sqrt(dx*dx+dy*dy+dz*dz);

    double da0dx  = (Px/P*r*cosineTheta - dx)/a0val;
    double da0dy  = (Py/P*r*cosineTheta - dy)/a0val;
    double da0dz  = (Pz/P*r*cosineTheta - dz)/a0val;
    double da0dx0 = -da0dx;
    double da0dy0 = -da0dy;
    double da0dz0 = -da0dz;
    for( unsigned int it=0; it<NTrk; it++) {
      da0dqOverP[it] = -(r*cosineTheta/P)*(da0dx*dpxdqOverP[it]+da0dy*dpydqOverP[it]+da0dz*dpzdqOverP[it]);
      da0dtheta[it]  = -(r*cosineTheta/P)*(da0dx*dpxdtheta[it]+da0dy*dpydtheta[it]+da0dz*dpzdtheta[it]);
      da0dphi[it]    = -(r*cosineTheta/P)*(da0dx*dpxdphi[it]+da0dy*dpydphi[it]);
    }

    unsigned int ndim = 0;
    if (fullCov != nullptr) {
      ndim = fullCov->rows();
    } else {
      ndim = 5*NTrk+3;
    }

    Amg::MatrixX V0_err;
    if (ndim == 5*NTrk+3 || ndim == 5*NTrk+6) {
      Amg::MatrixX D_vec(5*NTrk+6,1); D_vec.setZero();
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(5*it+0)  = 0.;
        D_vec(5*it+1)  = 0.;
        D_vec(5*it+2)  = da0dphi[it];
        D_vec(5*it+3)  = da0dtheta[it];
        D_vec(5*it+4)  = da0dqOverP[it];
      }
      D_vec(5*NTrk+0) = da0dx;
      D_vec(5*NTrk+1) = da0dy;
      D_vec(5*NTrk+2) = da0dz;
      D_vec(5*NTrk+3) = da0dx0;
      D_vec(5*NTrk+4) = da0dy0;
      D_vec(5*NTrk+5) = da0dz0;

      Amg::MatrixX W_mat(5*NTrk+6,5*NTrk+6); W_mat.setZero();
      if (fullCov != nullptr) {
        W_mat.block(0,0,ndim,ndim) = *fullCov;
      } else {
        Amg::MatrixX V0_cov = makeV0Cov(vxCandidate);
        W_mat.block(0,0,V0_cov.rows(),V0_cov.rows()) = V0_cov;
        W_mat.block(5*NTrk,5*NTrk,3,3) = vxCandidate->covariancePosition();
      }
      W_mat.block(5*NTrk+3,5*NTrk+3,3,3) = vertex->covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    } else if (ndim == 3*NTrk+3) {
      Amg::MatrixX D_vec(3*NTrk+6,1); D_vec.setZero();
      D_vec(0)  = da0dx;
      D_vec(1)  = da0dy;
      D_vec(2)  = da0dz;
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(3*it+3)  = da0dphi[it];
        D_vec(3*it+4)  = da0dtheta[it];
        D_vec(3*it+5)  = da0dqOverP[it];
      }
      D_vec(3*NTrk+3) = da0dx0;
      D_vec(3*NTrk+4) = da0dy0;
      D_vec(3*NTrk+5) = da0dz0;

      Amg::MatrixX W_mat(3*NTrk+6,3*NTrk+6); W_mat.setZero();
      W_mat.block(0,0,ndim,ndim) = *fullCov;
      W_mat.block(3*NTrk+3,3*NTrk+3,3,3) =  vertex->covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    }

    double a0Errsq = V0_err(0,0);
    if (a0Errsq <= 0.) ATH_MSG_DEBUG("a0Error: negative sqrt a0Errsq " << a0Errsq);
    double a0Err = (a0Errsq>0.) ? sqrt(a0Errsq) : 0.;
    delete fullCov;
    return a0Err;
  }

  double V0Tools::lxy(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const
  {
    auto vert = vxCandidate->position() - vertex->position();
    double dx = vert.x();
    double dy = vert.y();
    Amg::Vector3D mom = V0Momentum(vxCandidate);
    double dxy = (mom.x()*dx + mom.y()*dy)/mom.perp();
    return dxy;
  }

  double V0Tools::lxyError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const
  {
    auto vert = vxCandidate->position() - vertex->position();
    double dx = vert.x();
    double dy = vert.y();
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    double Px=0., Py=0.; 
    std::vector<double>dpxdqOverP(NTrk), dpxdtheta(NTrk), dpxdphi(NTrk);
    std::vector<double>dpydqOverP(NTrk), dpydtheta(NTrk), dpydphi(NTrk);
    std::vector<double>dLxydqOverP(NTrk), dLxydtheta(NTrk), dLxydphi(NTrk);

    Amg::MatrixX* fullCov = convertCovMatrix(vxCandidate);
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters* bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      double phi    = bPer->parameters()[Trk::phi];
      double theta  = bPer->parameters()[Trk::theta];
      double qOverP = bPer->parameters()[Trk::qOverP];
      dpxdqOverP[it] = -(sin(theta)*cos(phi)*trkCharge)/(qOverP*qOverP);
      dpxdtheta[it]  =  (cos(theta)*cos(phi)*trkCharge)/qOverP;
      dpxdphi[it]    = -(sin(theta)*sin(phi)*trkCharge)/qOverP;
      dpydqOverP[it] = -(sin(theta)*sin(phi)*trkCharge)/(qOverP*qOverP);
      dpydtheta[it]  =  (cos(theta)*sin(phi)*trkCharge)/qOverP;
      dpydphi[it]    =  (sin(theta)*cos(phi)*trkCharge)/qOverP;
      Px += bPer->momentum()[Trk::px];
      Py += bPer->momentum()[Trk::py];
    }
    double PTsq = Px*Px+Py*Py;
    double PT = (PTsq>0.) ? sqrt(PTsq) : 0.;
    double LXYoverPT = (Px*dx+Py*dy)/PTsq;

    for( unsigned int it=0; it<NTrk; it++) {
      double dPTdqOverP  = (Px*dpxdqOverP[it]+Py*dpydqOverP[it])/PT;
      double dPTdtheta   = (Px*dpxdtheta[it]+Py*dpydtheta[it])/PT;
      double dPTdphi     = (Px*dpxdphi[it]+Py*dpydphi[it])/PT;
      dLxydqOverP[it] = (dx*dpxdqOverP[it]+dy*dpydqOverP[it])/PT-LXYoverPT*dPTdqOverP;
      dLxydtheta[it]  = (dx*dpxdtheta[it]+dy*dpydtheta[it])/PT-LXYoverPT*dPTdtheta;
      dLxydphi[it]    = (dx*dpxdphi[it]+dy*dpydphi[it])/PT-LXYoverPT*dPTdphi;
    }
    double dLxydx = Px/PT;
    double dLxydy = Py/PT;
    double dLxydx0 = -dLxydx;
    double dLxydy0 = -dLxydy;

    unsigned int ndim = 0;
    if (fullCov != nullptr) {
      ndim = fullCov->rows();
    } else {
      ndim = 5*NTrk+3;
    }

    Amg::MatrixX V0_err;
    if (ndim == 5*NTrk+3 || ndim == 5*NTrk+6) {
      Amg::MatrixX D_vec(5*NTrk+6,1); D_vec.setZero();
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(5*it+0)  = 0.;
        D_vec(5*it+1)  = 0.;
        D_vec(5*it+2)  = dLxydphi[it];
        D_vec(5*it+3)  = dLxydtheta[it];
        D_vec(5*it+4)  = dLxydqOverP[it];
      }
      D_vec(5*NTrk+0) = dLxydx;
      D_vec(5*NTrk+1) = dLxydy;
      D_vec(5*NTrk+2) = 0.;
      D_vec(5*NTrk+3) = dLxydx0;
      D_vec(5*NTrk+4) = dLxydy0;
      D_vec(5*NTrk+5) = 0.;

      Amg::MatrixX W_mat(5*NTrk+6,5*NTrk+6); W_mat.setZero();
      if (fullCov != nullptr) {
        W_mat.block(0,0,ndim,ndim) = *fullCov;
      } else {
        Amg::MatrixX V0_cov = makeV0Cov(vxCandidate);
        W_mat.block(0,0,V0_cov.rows(),V0_cov.rows()) = V0_cov;
        W_mat.block(5*NTrk,5*NTrk,3,3) = vxCandidate->covariancePosition();
      }
      W_mat.block(5*NTrk+3,5*NTrk+3,3,3) = vertex->covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    } else if (ndim == 3*NTrk+3) {
      Amg::MatrixX D_vec(3*NTrk+6,1); D_vec.setZero();
      D_vec(0)  = dLxydx;
      D_vec(1)  = dLxydy;
      D_vec(2)  = 0.;
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(3*it+3) = dLxydphi[it];
        D_vec(3*it+4) = dLxydtheta[it];
        D_vec(3*it+5) = dLxydqOverP[it];
      }
      D_vec(3*NTrk+3) = dLxydx0;
      D_vec(3*NTrk+4) = dLxydy0;
      D_vec(3*NTrk+5) = 0.;

      Amg::MatrixX W_mat(3*NTrk+6,3*NTrk+6); W_mat.setZero();
      W_mat.block(0,0,ndim,ndim) = *fullCov;
      W_mat.block(3*NTrk+3,3*NTrk+3,3,3) = vertex->covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    }

    double LxyErrsq = V0_err(0,0);
    if (LxyErrsq <= 0.) ATH_MSG_DEBUG("lxyError: negative sqrt LxyErrsq " << LxyErrsq);
    delete fullCov;
    return (LxyErrsq>0.) ? sqrt(LxyErrsq) : 0.;
  }

  double V0Tools::lxyz(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const
  {
    auto vert = vxCandidate->position() - vertex->position();
    double dx = vert.x();
    double dy = vert.y();
    double dz = vert.z();
    Amg::Vector3D mom = V0Momentum(vxCandidate);
    double dxyz= (mom.x()*dx + mom.y()*dy + mom.z()*dz)/mom.mag();
    return dxyz;
  }

  double V0Tools::lxyzError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const
  {
    auto vert = vxCandidate->position() - vertex->position();
    double dx = vert.x();
    double dy = vert.y();
    double dz = vert.z();
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    double Px=0., Py=0., Pz=0.; 
    std::vector<double>dpxdqOverP(NTrk), dpxdtheta(NTrk), dpxdphi(NTrk);
    std::vector<double>dpydqOverP(NTrk), dpydtheta(NTrk), dpydphi(NTrk);
    std::vector<double>dpzdqOverP(NTrk), dpzdtheta(NTrk);
    std::vector<double>dLxyzdqOverP(NTrk), dLxyzdtheta(NTrk), dLxyzdphi(NTrk);

    Amg::MatrixX* fullCov = convertCovMatrix(vxCandidate);
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters* bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      double phi    = bPer->parameters()[Trk::phi];
      double theta  = bPer->parameters()[Trk::theta];
      double qOverP = bPer->parameters()[Trk::qOverP];
      dpxdqOverP[it] = -(sin(theta)*cos(phi)*trkCharge)/(qOverP*qOverP);
      dpxdtheta[it]  =  (cos(theta)*cos(phi)*trkCharge)/qOverP;
      dpxdphi[it]    = -(sin(theta)*sin(phi)*trkCharge)/qOverP;
      dpydqOverP[it] = -(sin(theta)*sin(phi)*trkCharge)/(qOverP*qOverP);
      dpydtheta[it]  =  (cos(theta)*sin(phi)*trkCharge)/qOverP;
      dpydphi[it]    =  (sin(theta)*cos(phi)*trkCharge)/qOverP;
      dpzdqOverP[it] = -(cos(theta)*trkCharge)/(qOverP*qOverP);
      dpzdtheta[it]  = -(sin(theta)*trkCharge)/qOverP;
      Px += bPer->momentum()[Trk::px];
      Py += bPer->momentum()[Trk::py];
      Pz += bPer->momentum()[Trk::pz];
    }
    double Psq = Px*Px+Py*Py+Pz*Pz;
    double P = (Psq>0.) ? sqrt(Psq) : 0.;
    double LXYZoverP = (Px*dx+Py*dy+Pz*dz)/Psq;

    for( unsigned int it=0; it<NTrk; it++) {
      double dPdqOverP  = (Px*dpxdqOverP[it]+Py*dpydqOverP[it]+Pz*dpzdqOverP[it])/P;
      double dPdtheta   = (Px*dpxdtheta[it]+Py*dpydtheta[it]+Pz*dpzdtheta[it])/P;
      double dPdphi     = (Px*dpxdphi[it]+Py*dpydphi[it])/P;
      dLxyzdqOverP[it] = (dx*dpxdqOverP[it]+dy*dpydqOverP[it]+dz*dpzdqOverP[it])/P-LXYZoverP*dPdqOverP;
      dLxyzdtheta[it]  = (dx*dpxdtheta[it]+dy*dpydtheta[it]+dz*dpzdtheta[it])/P-LXYZoverP*dPdtheta;
      dLxyzdphi[it]    = (dx*dpxdphi[it]+dy*dpydphi[it])/P-LXYZoverP*dPdphi;
    }
    double dLxyzdx = Px/P;
    double dLxyzdy = Py/P;
    double dLxyzdz = Pz/P;
    double dLxyzdx0 = -dLxyzdx;
    double dLxyzdy0 = -dLxyzdy;
    double dLxyzdz0 = -dLxyzdz;

    unsigned int ndim = 0;
    if (fullCov != nullptr) {
      ndim = fullCov->rows();
    } else {
      ndim = 5*NTrk+3;
    }

    Amg::MatrixX V0_err;
    if (ndim == 5*NTrk+3 || ndim == 5*NTrk+6) {
      Amg::MatrixX D_vec(5*NTrk+6,1); D_vec.setZero();
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(5*it+0)  = 0.;
        D_vec(5*it+1)  = 0.;
        D_vec(5*it+2)  = dLxyzdphi[it];
        D_vec(5*it+3)  = dLxyzdtheta[it];
        D_vec(5*it+4)  = dLxyzdqOverP[it];
      }
      D_vec(5*NTrk+0) = dLxyzdx;
      D_vec(5*NTrk+1) = dLxyzdy;
      D_vec(5*NTrk+2) = dLxyzdz;
      D_vec(5*NTrk+3) = dLxyzdx0;
      D_vec(5*NTrk+4) = dLxyzdy0;
      D_vec(5*NTrk+5) = dLxyzdz0;

      Amg::MatrixX W_mat(5*NTrk+6,5*NTrk+6); W_mat.setZero();
      if (fullCov != nullptr) {
        W_mat.block(0,0,ndim,ndim) = *fullCov;
      } else {
        Amg::MatrixX V0_cov = makeV0Cov(vxCandidate);
        W_mat.block(0,0,V0_cov.rows(),V0_cov.rows()) = V0_cov;
        W_mat.block(5*NTrk,5*NTrk,3,3) = vxCandidate->covariancePosition();
      }
      W_mat.block(5*NTrk+3,5*NTrk+3,3,3) = vertex->covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    } else if (ndim == 3*NTrk+3) {
      Amg::MatrixX D_vec(3*NTrk+6,1); D_vec.setZero();
      D_vec(0)  = dLxyzdx;
      D_vec(1)  = dLxyzdy;
      D_vec(2)  = dLxyzdz;
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(3*it+3) = dLxyzdphi[it];
        D_vec(3*it+4) = dLxyzdtheta[it];
        D_vec(3*it+5) = dLxyzdqOverP[it];
      }
      D_vec(3*NTrk+3) = dLxyzdx0;
      D_vec(3*NTrk+4) = dLxyzdy0;
      D_vec(3*NTrk+5) = dLxyzdz0;

      Amg::MatrixX W_mat(3*NTrk+6,3*NTrk+6); W_mat.setZero();
      W_mat.block(0,0,ndim,ndim) = *fullCov;
      W_mat.block(3*NTrk+3,3*NTrk+3,3,3) = vertex->covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    }

    double LxyzErrsq = V0_err(0,0);
    if (LxyzErrsq <= 0.) ATH_MSG_DEBUG("lxyzError: negative sqrt LxyzErrsq " << LxyzErrsq);
    delete fullCov;
    return (LxyzErrsq>0.) ? sqrt(LxyzErrsq) : 0.;
  }

  double V0Tools::tau(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, double posTrackMass, double negTrackMass) const
  {
    std::vector<double> masses = {posTrackMass, negTrackMass};

    return tau(vxCandidate,vertex,masses);
  }

  double V0Tools::tau(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, const std::vector<double> &masses) const
  {
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    //double CONST = 1000./CLHEP::c_light;
    double CONST = 1000./299.792;
    double M = invariantMass(vxCandidate, masses);
    double LXY = lxy(vxCandidate,vertex);
    double PT = V0Momentum(vxCandidate).perp();
    return CONST*M*LXY/PT;
  }

  double V0Tools::tau(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, double posTrackMass, double negTrackMass, double massV0) const
  {
    std::vector<double> masses = {posTrackMass, negTrackMass};

    return tau(vxCandidate,vertex,masses,massV0);
  }

  double V0Tools::tau(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, const std::vector<double> &masses, double massV0) const
  {
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    Amg::MatrixX cov = tauMassCovariance(vxCandidate,vertex,masses);
    double Tau = tau(vxCandidate,vertex,masses);
    double mass = invariantMass(vxCandidate,masses);
    double descr = cov(0,0)*cov(1,1)-cov(0,1)*cov(0,1);
    double cov_i11 =  cov(1,1)/descr;
    double cov_i12 = -cov(0,1)/descr;
    double deltaTau = -(massV0-mass)*cov_i12/cov_i11;
    return Tau + deltaTau;
  }

  double V0Tools::tau(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, double M) const
  {
    //double CONST = 1000./CLHEP::c_light;
    double CONST = 1000./299.792;
    double LXY = lxy(vxCandidate,vertex);
    double PT = V0Momentum(vxCandidate).perp();
    return CONST*M*LXY/PT;
  }

  double V0Tools::tauError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, double posTrackMass, double negTrackMass) const
  {
    // Tau = CONST*M*(Px*dx+Py*dy)/(PT*PT)
    std::vector<double> masses = {posTrackMass, negTrackMass};

    return tauError(vxCandidate,vertex,masses);
  }

  double V0Tools::tauError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, const std::vector<double> &masses) const
  {
    // Tau = CONST*M*(Px*dx+Py*dy)/(PT*PT)
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    //double CONST = 1000./CLHEP::c_light;
    double CONST = 1000./299.792;
    double PT = V0Momentum(vxCandidate).perp();
    auto vert = vxCandidate->position() - vertex->position();
    double dx = vert.x();
    double dy = vert.y();
    double M = invariantMass(vxCandidate, masses);
    double E=0., Px=0., Py=0., Pz=0.; 
    std::vector<double>dpxdqOverP(NTrk), dpxdtheta(NTrk), dpxdphi(NTrk);
    std::vector<double>dpydqOverP(NTrk), dpydtheta(NTrk), dpydphi(NTrk);
    std::vector<double>dpzdqOverP(NTrk), dpzdtheta(NTrk), dedqOverP(NTrk);
    std::vector<double>dTaudqOverP(NTrk), dTaudtheta(NTrk), dTaudphi(NTrk);

    Amg::MatrixX* fullCov = convertCovMatrix(vxCandidate);
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters* bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      double phi    = bPer->parameters()[Trk::phi];
      double theta  = bPer->parameters()[Trk::theta];
      double qOverP = bPer->parameters()[Trk::qOverP];
      double tmp = 1./(qOverP*qOverP) + masses[it]*masses[it];
      double pe = (tmp>0.) ? sqrt(tmp) : 0.;
      dedqOverP[it]  = -1./(qOverP*qOverP*qOverP*pe);
      dpxdqOverP[it] = -(sin(theta)*cos(phi)*trkCharge)/(qOverP*qOverP);
      dpxdtheta[it]  =  (cos(theta)*cos(phi)*trkCharge)/qOverP;
      dpxdphi[it]    = -(sin(theta)*sin(phi)*trkCharge)/qOverP;
      dpydqOverP[it] = -(sin(theta)*sin(phi)*trkCharge)/(qOverP*qOverP);
      dpydtheta[it]  =  (cos(theta)*sin(phi)*trkCharge)/qOverP;
      dpydphi[it]    =  (sin(theta)*cos(phi)*trkCharge)/qOverP;
      dpzdqOverP[it] = -(cos(theta)*trkCharge)/(qOverP*qOverP);
      dpzdtheta[it]  = -(sin(theta)*trkCharge)/qOverP;
      E  += pe;
      Px += bPer->momentum()[Trk::px];
      Py += bPer->momentum()[Trk::py];
      Pz += bPer->momentum()[Trk::pz];
    }
    double LXY = Px*dx+Py*dy;

    for( unsigned int it=0; it<NTrk; it++) {
      double dMdqOverP   = -(Px*dpxdqOverP[it]+Py*dpydqOverP[it]+Pz*dpzdqOverP[it]-E*dedqOverP[it])/M;
      double dMdtheta    = -(Px*dpxdtheta[it]+Py*dpydtheta[it]+Pz*dpzdtheta[it])/M;
      double dMdphi      = -(Px*dpxdphi[it]+Py*dpydphi[it])/M;
      double dPTdqOverP  =  (Px*dpxdqOverP[it]+Py*dpydqOverP[it])/PT;
      double dPTdtheta   =  (Px*dpxdtheta[it]+Py*dpydtheta[it])/PT;
      double dPTdphi     =  (Px*dpxdphi[it]+Py*dpydphi[it])/PT;
      double dLXYdqOverP =  dx*dpxdqOverP[it]+dy*dpydqOverP[it];
      double dLXYdtheta  =  dx*dpxdtheta[it]+dy*dpydtheta[it];
      double dLXYdphi    =  dx*dpxdphi[it]+dy*dpydphi[it];
      dTaudqOverP[it] =  (LXY*dMdqOverP+M*dLXYdqOverP)/(PT*PT)-(2.*LXY*M*dPTdqOverP)/(PT*PT*PT);
      dTaudtheta[it]  =  (LXY*dMdtheta+M*dLXYdtheta)/(PT*PT)-(2.*LXY*M*dPTdtheta)/(PT*PT*PT);
      dTaudphi[it]    =  (LXY*dMdphi+M*dLXYdphi)/(PT*PT)-(2.*LXY*M*dPTdphi)/(PT*PT*PT);
    }
    double dTaudx = (M*Px)/(PT*PT);
    double dTaudy = (M*Py)/(PT*PT);
    double dTaudx0 = -dTaudx;
    double dTaudy0 = -dTaudy;

    unsigned int ndim = 0;
    if (fullCov != nullptr) {
      ndim = fullCov->rows();
    } else {
      ndim = 5*NTrk+3;
    }

    Amg::MatrixX V0_err;
    if (ndim == 5*NTrk+3 || ndim == 5*NTrk+6) {
      Amg::MatrixX D_vec(5*NTrk+6,1); D_vec.setZero();
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(5*it+0)  = 0.;
        D_vec(5*it+1)  = 0.;
        D_vec(5*it+2)  = dTaudphi[it];
        D_vec(5*it+3)  = dTaudtheta[it];
        D_vec(5*it+4)  = dTaudqOverP[it];
      }
      D_vec(5*NTrk+0) = dTaudx;
      D_vec(5*NTrk+1) = dTaudy;
      D_vec(5*NTrk+2) = 0.;
      D_vec(5*NTrk+3) = dTaudx0;
      D_vec(5*NTrk+4) = dTaudy0;
      D_vec(5*NTrk+5) = 0.;

      Amg::MatrixX W_mat(5*NTrk+6,5*NTrk+6); W_mat.setZero();
      if (fullCov != nullptr) {
        W_mat.block(0,0,ndim,ndim) = *fullCov;
      } else {
        Amg::MatrixX V0_cov = makeV0Cov(vxCandidate);
        W_mat.block(0,0,V0_cov.rows(),V0_cov.rows()) = V0_cov;
        W_mat.block(5*NTrk,5*NTrk,3,3) =  vxCandidate->covariancePosition();
      }
      W_mat.block(5*NTrk+3,5*NTrk+3,3,3) = vertex->covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    } else if (ndim == 3*NTrk+3) {
      Amg::MatrixX D_vec(3*NTrk+6,1); D_vec.setZero();
      D_vec(0)  = dTaudx;
      D_vec(1)  = dTaudy;
      D_vec(2)  = 0.;
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(3*it+3)  = dTaudphi[it];
        D_vec(3*it+4)  = dTaudtheta[it];
        D_vec(3*it+5)  = dTaudqOverP[it];
      }
      D_vec(3*NTrk+3) = dTaudx0;
      D_vec(3*NTrk+4) = dTaudy0;
      D_vec(3*NTrk+5) = 0.;

      Amg::MatrixX W_mat(3*NTrk+6,3*NTrk+6); W_mat.setZero();
      W_mat.block(0,0,ndim,ndim) = *fullCov;
      W_mat.block(3*NTrk+3,3*NTrk+3,3,3) =  vertex->covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    }

    double tauErrsq = V0_err(0,0);
    if (tauErrsq <= 0.) ATH_MSG_DEBUG("tauError: negative sqrt tauErrsq " << tauErrsq);
    double tauErr = (tauErrsq>0.) ? sqrt(tauErrsq) : 0.;
    delete fullCov;
    return CONST*tauErr;
  }

  double V0Tools::tauError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, double posTrackMass, double negTrackMass, double massV0) const
  {
    std::vector<double> masses = {posTrackMass, negTrackMass};

    return tauError(vxCandidate,vertex,masses,massV0);
  }

  double V0Tools::tauError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, const std::vector<double> &masses, double ) const
  {
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    double error = -999999.;
    Amg::MatrixX cov = tauMassCovariance(vxCandidate,vertex,masses);
    double descr = cov(0,0)*cov(1,1)-cov(0,1)*cov(0,1);
    double cov_i11 =  cov(1,1)/descr;
    if (cov_i11 > 0.) error = 1./sqrt(cov_i11);
    return error;
  }

  double V0Tools::tauError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, double M) const
  {
    // Tau = CONST*M*(Px*dx+Py*dy)/(PT*PT)
    //double CONST = 1000./CLHEP::c_light;
    double CONST = 1000./299.792;
    double PT = V0Momentum(vxCandidate).perp();
    auto vecsub = vxCandidate->position() - vertex->position();
    double dx = vecsub.x();
    double dy = vecsub.y();
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    double Px=0., Py=0.; 
    std::vector<double>dpxdqOverP(NTrk), dpxdtheta(NTrk), dpxdphi(NTrk);
    std::vector<double>dpydqOverP(NTrk), dpydtheta(NTrk), dpydphi(NTrk);
    std::vector<double>dPTdtheta(NTrk), dPTdphi(NTrk);
    std::vector<double>dTaudqOverP(NTrk), dTaudtheta(NTrk), dTaudphi(NTrk);

    Amg::MatrixX* fullCov = convertCovMatrix(vxCandidate);
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters* bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      double phi = bPer->parameters()[Trk::phi];
      double theta  = bPer->parameters()[Trk::theta];
      double qOverP = bPer->parameters()[Trk::qOverP];
      dpxdqOverP[it] = -(sin(theta)*cos(phi)*trkCharge)/(qOverP*qOverP);
      dpxdtheta[it]  =  (cos(theta)*cos(phi)*trkCharge)/qOverP;
      dpxdphi[it]    = -(sin(theta)*sin(phi)*trkCharge)/qOverP;
      dpydqOverP[it] = -(sin(theta)*sin(phi)*trkCharge)/(qOverP*qOverP);
      dpydtheta[it]  =  (cos(theta)*sin(phi)*trkCharge)/qOverP;
      dpydphi[it]    =  (sin(theta)*cos(phi)*trkCharge)/qOverP;
      Px += bPer->momentum()[Trk::px];
      Py += bPer->momentum()[Trk::py];
    }
    double LXY = Px*dx+Py*dy;

    for( unsigned int it=0; it<NTrk; it++) {
      double dPTdqOverP  = (Px*dpxdqOverP[it]+Py*dpydqOverP[it])/PT;
      double dPTdtheta   = (Px*dpxdtheta[it]+Py*dpydtheta[it])/PT;
      double dPTdphi     = (Px*dpxdphi[it]+Py*dpydphi[it])/PT;
      double dLXYdqOverP = dx*dpxdqOverP[it]+dy*dpydqOverP[it];
      double dLXYdtheta  = dx*dpxdtheta[it]+dy*dpydtheta[it];
      double dLXYdphi = dx*dpxdphi[it]+dy*dpydphi[it];
      dTaudqOverP[it] = M*dLXYdqOverP/(PT*PT)-(2.*LXY*M*dPTdqOverP)/(PT*PT*PT);
      dTaudtheta[it]  = M*dLXYdtheta/(PT*PT)-(2.*LXY*M*dPTdtheta)/(PT*PT*PT);
      dTaudphi[it]    = M*dLXYdphi/(PT*PT)-(2.*LXY*M*dPTdphi)/(PT*PT*PT);
    }
    double dTaudx = (M*Px)/(PT*PT);
    double dTaudy = (M*Py)/(PT*PT);
    double dTaudx0 = -dTaudx;
    double dTaudy0 = -dTaudy;

    unsigned int ndim = 0;
    if (fullCov != nullptr) {
      ndim = fullCov->rows();
    } else {
      ndim = 5*NTrk+3;
    }

    Amg::MatrixX V0_err;
    if (ndim == 5*NTrk+3 || ndim == 5*NTrk+6) {
      Amg::MatrixX D_vec(5*NTrk+6,1); D_vec.setZero();
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(5*it+0) = 0.;
        D_vec(5*it+1) = 0.;
        D_vec(5*it+2) = dTaudphi[it];
        D_vec(5*it+3) = dTaudtheta[it];
        D_vec(5*it+4) = dTaudqOverP[it];
      }
      D_vec(5*NTrk+0) = dTaudx;
      D_vec(5*NTrk+1) = dTaudy;
      D_vec(5*NTrk+2) = 0.;
      D_vec(5*NTrk+3) = dTaudx0;
      D_vec(5*NTrk+4) = dTaudy0;
      D_vec(5*NTrk+5) = 0.;

      Amg::MatrixX W_mat(5*NTrk+6,5*NTrk+6); W_mat.setZero();
      if (fullCov != nullptr) {
        W_mat.block(0,0,ndim,ndim) = *fullCov;
      } else {
        Amg::MatrixX V0_cov = makeV0Cov(vxCandidate);
        W_mat.block(0,0,V0_cov.rows(),V0_cov.rows()) = V0_cov;
        W_mat.block(5*NTrk,5*NTrk,3,3) = vxCandidate->covariancePosition();
      }
      W_mat.block(5*NTrk+3,5*NTrk+3,3,3) = vertex->covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    } else if (ndim == 3*NTrk+3) {
      Amg::MatrixX D_vec(3*NTrk+6,1); D_vec.setZero();
      D_vec(0)  = dTaudx;
      D_vec(1)  = dTaudy;
      D_vec(2)  = 0.;
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(3*it+3)  = dTaudphi[it];
        D_vec(3*it+4)  = dTaudtheta[it];
        D_vec(3*it+5)  = dTaudqOverP[it];
      }
      D_vec(3*NTrk+3) = dTaudx0;
      D_vec(3*NTrk+4) = dTaudy0;
      D_vec(3*NTrk+5) = 0.;

      Amg::MatrixX W_mat(3*NTrk+6,3*NTrk+6); W_mat.setZero();
      W_mat.block(0,0,ndim,ndim) = *fullCov;
      W_mat.block(3*NTrk+3,3*NTrk+3,3,3) = vertex->covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    }

    double tauErrsq = V0_err(0,0);
    if (tauErrsq <= 0.) ATH_MSG_DEBUG("tauError: negative sqrt tauErrsq " << tauErrsq);
    double tauErr = (tauErrsq>0.) ? sqrt(tauErrsq) : 0.;
    delete fullCov;
    return CONST*tauErr;
  }

  double V0Tools::tau3D(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, const std::vector<double> &masses) const
  {
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    //double CONST = 1000./CLHEP::c_light;
    double CONST = 1000./299.792;
    double M = invariantMass(vxCandidate, masses);
    double LXYZ = lxyz(vxCandidate,vertex);
    double P = V0Momentum(vxCandidate).mag();
    return CONST*M*LXYZ/P;
  }

  double V0Tools::tau3D(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, double M) const
  {
    //double CONST = 1000./CLHEP::c_light;
    double CONST = 1000./299.792;
    double LXYZ = lxyz(vxCandidate,vertex);
    double P = V0Momentum(vxCandidate).mag();
    return CONST*M*LXYZ/P;
  }

  double V0Tools::tau3DError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, const std::vector<double> &masses) const
  {
    // Tau = CONST*M*(Px*dx+Py*dy+Pz*dz)/(P*P)
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    //double CONST = 1000./CLHEP::c_light;
    double CONST = 1000./299.792;
    double P = V0Momentum(vxCandidate).mag();
    auto vert = vxCandidate->position() - vertex->position();
    double dx = vert.x();
    double dy = vert.y();
    double dz = vert.z();
    double M = invariantMass(vxCandidate, masses);
    double E=0., Px=0., Py=0., Pz=0.; 
    std::vector<double>dpxdqOverP(NTrk), dpxdtheta(NTrk), dpxdphi(NTrk);
    std::vector<double>dpydqOverP(NTrk), dpydtheta(NTrk), dpydphi(NTrk);
    std::vector<double>dpzdqOverP(NTrk), dpzdtheta(NTrk), dedqOverP(NTrk);
    std::vector<double>dTaudqOverP(NTrk), dTaudtheta(NTrk), dTaudphi(NTrk);

    Amg::MatrixX* fullCov = convertCovMatrix(vxCandidate);
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters* bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      double phi    = bPer->parameters()[Trk::phi];
      double theta  = bPer->parameters()[Trk::theta];
      double qOverP = bPer->parameters()[Trk::qOverP];
      double tmp = 1./(qOverP*qOverP) + masses[it]*masses[it];
      double pe = (tmp>0.) ? sqrt(tmp) : 0.;
      dedqOverP[it]  = -1./(qOverP*qOverP*qOverP*pe);
      dpxdqOverP[it] = -(sin(theta)*cos(phi)*trkCharge)/(qOverP*qOverP);
      dpxdtheta[it]  =  (cos(theta)*cos(phi)*trkCharge)/qOverP;
      dpxdphi[it]    = -(sin(theta)*sin(phi)*trkCharge)/qOverP;
      dpydqOverP[it] = -(sin(theta)*sin(phi)*trkCharge)/(qOverP*qOverP);
      dpydtheta[it]  =  (cos(theta)*sin(phi)*trkCharge)/qOverP;
      dpydphi[it]    =  (sin(theta)*cos(phi)*trkCharge)/qOverP;
      dpzdqOverP[it] = -(cos(theta)*trkCharge)/(qOverP*qOverP);
      dpzdtheta[it]  = -(sin(theta)*trkCharge)/qOverP;
      E  += pe;
      Px += bPer->momentum()[Trk::px];
      Py += bPer->momentum()[Trk::py];
      Pz += bPer->momentum()[Trk::pz];
    }
    double LXYZ = Px*dx+Py*dy+Pz*dz;

    for( unsigned int it=0; it<NTrk; it++) {
      double dMdqOverP    = -(Px*dpxdqOverP[it]+Py*dpydqOverP[it]+Pz*dpzdqOverP[it]-E*dedqOverP[it])/M;
      double dMdtheta     = -(Px*dpxdtheta[it]+Py*dpydtheta[it]+Pz*dpzdtheta[it])/M;
      double dMdphi       = -(Px*dpxdphi[it]+Py*dpydphi[it])/M;
      double dPdqOverP    =  (Px*dpxdqOverP[it]+Py*dpydqOverP[it]+Pz*dpzdqOverP[it])/P;
      double dPdtheta     =  (Px*dpxdtheta[it]+Py*dpydtheta[it]+Pz*dpzdtheta[it])/P;
      double dPdphi       =  (Px*dpxdphi[it]+Py*dpydphi[it])/P;
      double dLXYZdqOverP =  dx*dpxdqOverP[it]+dy*dpydqOverP[it]+dz*dpzdqOverP[it];
      double dLXYZdtheta  =  dx*dpxdtheta[it]+dy*dpydtheta[it]+dz*dpzdtheta[it];
      double dLXYZdphi    =  dx*dpxdphi[it]+dy*dpydphi[it];
      dTaudqOverP[it]  =  (LXYZ*dMdqOverP+M*dLXYZdqOverP)/(P*P)-(2.*LXYZ*M*dPdqOverP)/(P*P*P);
      dTaudtheta[it]   =  (LXYZ*dMdtheta+M*dLXYZdtheta)/(P*P)-(2.*LXYZ*M*dPdtheta)/(P*P*P);
      dTaudphi[it]     =  (LXYZ*dMdphi+M*dLXYZdphi)/(P*P)-(2.*LXYZ*M*dPdphi)/(P*P*P);
    }
    double dTaudx = (M*Px)/(P*P);
    double dTaudy = (M*Py)/(P*P);
    double dTaudz = (M*Pz)/(P*P);
    double dTaudx0 = -dTaudx;
    double dTaudy0 = -dTaudy;
    double dTaudz0 = -dTaudz;

    unsigned int ndim = 0;
    if (fullCov != nullptr) {
      ndim = fullCov->rows();
    } else {
      ndim = 5*NTrk+3;
    }

    Amg::MatrixX V0_err;
    if (ndim == 5*NTrk+3 || ndim == 5*NTrk+6) {
      Amg::MatrixX D_vec(5*NTrk+6,1); D_vec.setZero();
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(5*it+0) = 0.;
        D_vec(5*it+1) = 0.;
        D_vec(5*it+2) = dTaudphi[it];
        D_vec(5*it+3) = dTaudtheta[it];
        D_vec(5*it+4) = dTaudqOverP[it];
      }
      D_vec(5*NTrk+0) = dTaudx;
      D_vec(5*NTrk+1) = dTaudy;
      D_vec(5*NTrk+2) = dTaudz;
      D_vec(5*NTrk+3) = dTaudx0;
      D_vec(5*NTrk+4) = dTaudy0;
      D_vec(5*NTrk+5) = dTaudz0;

      Amg::MatrixX W_mat(5*NTrk+6,5*NTrk+6); W_mat.setZero();
      if (fullCov != nullptr) {
        W_mat.block(0,0,ndim,ndim) = *fullCov;
      } else {
        Amg::MatrixX V0_cov = makeV0Cov(vxCandidate);
        W_mat.block(0,0,V0_cov.rows(),V0_cov.rows()) = V0_cov;
        W_mat.block(5*NTrk,5*NTrk,3,3) =  vxCandidate->covariancePosition();
      }
      W_mat.block(5*NTrk+3,5*NTrk+3,3,3) = vertex->covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    } else if (ndim == 3*NTrk+3) {
      Amg::MatrixX D_vec(3*NTrk+6,1); D_vec.setZero();
      D_vec(0) = dTaudx;
      D_vec(1) = dTaudy;
      D_vec(2) = dTaudz;
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(3*it+3) = dTaudphi[it];
        D_vec(3*it+4) = dTaudtheta[it];
        D_vec(3*it+5) = dTaudqOverP[it];
      }
      D_vec(3*NTrk+3) = dTaudx0;
      D_vec(3*NTrk+4) = dTaudy0;
      D_vec(3*NTrk+5) = dTaudz0;

      Amg::MatrixX W_mat(3*NTrk+6,3*NTrk+6); W_mat.setZero();
      W_mat.block(0,0,ndim,ndim) = *fullCov;
      W_mat.block(3*NTrk+3,3*NTrk+3,3,3) =  vertex->covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    }

    double tauErrsq = V0_err(0,0);
    if (tauErrsq <= 0.) ATH_MSG_DEBUG("tauError: negative sqrt tauErrsq " << tauErrsq);
    double tauErr = (tauErrsq>0.) ? sqrt(tauErrsq) : 0.;
    delete fullCov;
    return CONST*tauErr;
  }

  double V0Tools::tau3DError(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, double M) const
  {
    // Tau = CONST*M*(Px*dx+Py*dy+Pz*dz)/(P*P)
    //double CONST = 1000./CLHEP::c_light;
    double CONST = 1000./299.792;
    double P = V0Momentum(vxCandidate).mag();
    auto vecsub = vxCandidate->position() - vertex->position();
    double dx = vecsub.x();
    double dy = vecsub.y();
    double dz = vecsub.z();
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    double Px=0., Py=0., Pz=0.; 
    std::vector<double>dpxdqOverP(NTrk), dpxdtheta(NTrk), dpxdphi(NTrk);
    std::vector<double>dpydqOverP(NTrk), dpydtheta(NTrk), dpydphi(NTrk);
    std::vector<double>dpzdqOverP(NTrk), dpzdtheta(NTrk);
    std::vector<double>dTaudqOverP(NTrk), dTaudtheta(NTrk), dTaudphi(NTrk);

    Amg::MatrixX* fullCov = convertCovMatrix(vxCandidate);
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters* bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      double phi    = bPer->parameters()[Trk::phi];
      double theta  = bPer->parameters()[Trk::theta];
      double qOverP = bPer->parameters()[Trk::qOverP];
      dpxdqOverP[it] = -(sin(theta)*cos(phi)*trkCharge)/(qOverP*qOverP);
      dpxdtheta[it]  =  (cos(theta)*cos(phi)*trkCharge)/qOverP;
      dpxdphi[it]    = -(sin(theta)*sin(phi)*trkCharge)/qOverP;
      dpydqOverP[it] = -(sin(theta)*sin(phi)*trkCharge)/(qOverP*qOverP);
      dpydtheta[it]  =  (cos(theta)*sin(phi)*trkCharge)/qOverP;
      dpydphi[it]    =  (sin(theta)*cos(phi)*trkCharge)/qOverP;
      dpzdqOverP[it] = -(cos(theta)*trkCharge)/(qOverP*qOverP);
      dpzdtheta[it]  = -(sin(theta)*trkCharge)/qOverP;
      Px += bPer->momentum()[Trk::px];
      Py += bPer->momentum()[Trk::py];
      Pz += bPer->momentum()[Trk::pz];
    }
    double LXYZ = Px*dx+Py*dy+Pz*dz;

    for( unsigned int it=0; it<NTrk; it++) {
      double dPdqOverP    = (Px*dpxdqOverP[it]+Py*dpydqOverP[it]+Pz*dpzdqOverP[it])/P;
      double dPdtheta     = (Px*dpxdtheta[it]+Py*dpydtheta[it]+Pz*dpzdtheta[it])/P;
      double dPdphi       = (Px*dpxdphi[it]+Py*dpydphi[it])/P;
      double dLXYZdqOverP = dx*dpxdqOverP[it]+dy*dpydqOverP[it]+dz*dpzdqOverP[it];
      double dLXYZdtheta  = dx*dpxdtheta[it]+dy*dpydtheta[it]+dz*dpzdtheta[it];
      double dLXYZdphi    = dx*dpxdphi[it]+dy*dpydphi[it];
      dTaudqOverP[it]  = M*dLXYZdqOverP/(P*P)-(2.*LXYZ*M*dPdqOverP)/(P*P*P);
      dTaudtheta[it]   = M*dLXYZdtheta/(P*P)-(2.*LXYZ*M*dPdtheta)/(P*P*P);
      dTaudphi[it]     = M*dLXYZdphi/(P*P)-(2.*LXYZ*M*dPdphi)/(P*P*P);
    }
    double dTaudx = (M*Px)/(P*P);
    double dTaudy = (M*Py)/(P*P);
    double dTaudz = (M*Pz)/(P*P);
    double dTaudx0 = -dTaudx;
    double dTaudy0 = -dTaudy;
    double dTaudz0 = -dTaudz;

    unsigned int ndim = 0;
    if (fullCov != nullptr) {
      ndim = fullCov->rows();
    } else {
      ndim = 5*NTrk+3;
    }

    Amg::MatrixX V0_err;
    if (ndim == 5*NTrk+3 || ndim == 5*NTrk+6) {
      Amg::MatrixX D_vec(5*NTrk+6,1); D_vec.setZero();
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(5*it+0) = 0.;
        D_vec(5*it+1) = 0.;
        D_vec(5*it+2) = dTaudphi[it];
        D_vec(5*it+3) = dTaudtheta[it];
        D_vec(5*it+4) = dTaudqOverP[it];
      }
      D_vec(5*NTrk+0) = dTaudx;
      D_vec(5*NTrk+1) = dTaudy;
      D_vec(5*NTrk+2) = dTaudz;
      D_vec(5*NTrk+3) = dTaudx0;
      D_vec(5*NTrk+4) = dTaudy0;
      D_vec(5*NTrk+5) = dTaudz0;

      Amg::MatrixX W_mat(5*NTrk+6,5*NTrk+6); W_mat.setZero();
      if (fullCov != nullptr) {
        W_mat.block(0,0,ndim,ndim) = *fullCov;
      } else {
        Amg::MatrixX V0_cov = makeV0Cov(vxCandidate);
        W_mat.block(0,0,V0_cov.rows(),V0_cov.rows()) = V0_cov;
        W_mat.block(5*NTrk,5*NTrk,3,3) = vxCandidate->covariancePosition();
      }
      W_mat.block(5*NTrk+3,5*NTrk+3,3,3) = vertex->covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    } else if (ndim == 3*NTrk+3) {
      Amg::MatrixX D_vec(3*NTrk+6,1); D_vec.setZero();
      D_vec(0)  = dTaudx;
      D_vec(1)  = dTaudy;
      D_vec(2)  = dTaudz;
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(3*it+3) = dTaudphi[it];
        D_vec(3*it+4) = dTaudtheta[it];
        D_vec(3*it+5) = dTaudqOverP[it];
      }
      D_vec(3*NTrk+3) = dTaudx0;
      D_vec(3*NTrk+4) = dTaudy0;
      D_vec(3*NTrk+5) = dTaudz0;

      Amg::MatrixX W_mat(3*NTrk+6,3*NTrk+6); W_mat.setZero();
      W_mat.block(0,0,ndim,ndim) = *fullCov;
      W_mat.block(3*NTrk+3,3*NTrk+3,3,3) = vertex->covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    }

    double tauErrsq = V0_err(0,0);
    if (tauErrsq <= 0.) ATH_MSG_DEBUG("tauError: negative sqrt tauErrsq " << tauErrsq);
    double tauErr = (tauErrsq>0.) ? sqrt(tauErrsq) : 0.;
    delete fullCov;
    return CONST*tauErr;
  }

  double V0Tools::thetaStar(const xAOD::Vertex * vxCandidate, double mass1, double mass2) const
  {
    double theta = 0.;
    xAOD::TrackParticle::FourMom_t V1 = positiveTrack4Momentum(vxCandidate, mass1);
    xAOD::TrackParticle::FourMom_t V2 = negativeTrack4Momentum(vxCandidate, mass2);
    CLHEP::HepLorentzVector v1(V1.Px(),V1.Py(),V1.Pz(),V1.E());
    CLHEP::HepLorentzVector v2(V2.Px(),V2.Py(),V2.Pz(),V2.E());
    CLHEP::HepLorentzVector v0 = v1 + v2;
    CLHEP::Hep3Vector boost_v0 = v0.boostVector();
    boost_v0 *= -1.0;
    v1.boost( boost_v0 );
    v2.boost( boost_v0 );
    theta = v0.angle( v1.vect() );
    return theta;
  }

  double V0Tools::cosThetaStar(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    xAOD::TrackParticle::FourMom_t PosMom = positiveTrack4Momentum(vxCandidate, posTrackMass);
    xAOD::TrackParticle::FourMom_t NegMom = negativeTrack4Momentum(vxCandidate, negTrackMass);
    CLHEP::HepLorentzVector posMom(PosMom.Px(),PosMom.Py(),PosMom.Pz(),PosMom.E());
    CLHEP::HepLorentzVector negMom(NegMom.Px(),NegMom.Py(),NegMom.Pz(),NegMom.E());
    return cosThetaStar(posMom, negMom);
  }

  double V0Tools::cosThetaStar(const CLHEP::HepLorentzVector & posTrack, const CLHEP::HepLorentzVector & negTrack) const
  {
    CLHEP::HepLorentzVector v0(posTrack + negTrack);
    double Mv0   = v0.m();
    double Mplus = posTrack.m();
    double Mminus= negTrack.m();
    double Pv0 = v0.rho();
    double pssq = (Mv0*Mv0-(Mplus+Mminus)*(Mplus+Mminus))*(Mv0*Mv0-(Mplus-Mminus)*(Mplus-Mminus));
    double ps = (pssq>0.) ? sqrt(pssq) : 0.;
    ps /= 2.*Mv0;
    double pp = v0.px()*posTrack.px() + v0.py()*posTrack.py() + v0.pz()*posTrack.pz();
    return ( v0.e()*pp - Pv0*Pv0*posTrack.e())/( Mv0*ps*Pv0);
  }

  //double V0Tools::phiStar(xAOD::Vertex * vxCandidate) const
  double V0Tools::phiStar(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    xAOD::TrackParticle::FourMom_t V_pos = positiveTrack4Momentum(vxCandidate,posTrackMass);
    xAOD::TrackParticle::FourMom_t V_neg = negativeTrack4Momentum(vxCandidate,negTrackMass);
    CLHEP::HepLorentzVector v_pos(V_pos.Px(),V_pos.Py(),V_pos.Pz(),V_pos.E());
    CLHEP::HepLorentzVector v_neg(V_neg.Px(),V_neg.Py(),V_neg.Pz(),V_neg.E());
    return phiStar(v_pos+v_neg,v_pos);
  }

  double V0Tools::phiStar(const CLHEP::HepLorentzVector & v0, const CLHEP::HepLorentzVector & track) const
  {
    double phiStar = -999999.;
    CLHEP::Hep3Vector V0 = v0.getV();
    CLHEP::Hep3Vector trk = track.getV();
    //double v0_rapidity = v0.rapidity();
    trk.rotateZ(-V0.phi());
    trk.rotateY(-V0.theta());
    //if (v0_rapidity < 0.) {
    //  trk.rotateZ(-M_PI);
    //  phiStar = -atan2(trk.y(),trk.x());
    //} else {
    //  phiStar = atan2(trk.y(),trk.x());
    //}
    phiStar = atan2(trk.y(),trk.x());
    return phiStar;
  }

  double V0Tools::cosTheta(const xAOD::Vertex * vxCandidate, const Amg::Vector3D& vertex) const
  {
    auto mom = V0Momentum(vxCandidate);
    auto vtx1 = vtx(vxCandidate);
    vtx1 -= vertex;
    return (mom.dot(vtx1))/(mom.mag()*(vtx1).mag());
  }

  double V0Tools::cosTheta(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const
  {
    auto mom = V0Momentum(vxCandidate);
    auto vtx1 = vtx(vxCandidate);
    vtx1 -= vertex->position();
    return (mom.dot((vtx1)))/(mom.mag()*(vtx1).mag());
  }

  double V0Tools::cosTheta_xy(const xAOD::Vertex * vxCandidate, const Amg::Vector3D& vertex) const
  {
    auto mom = V0Momentum(vxCandidate);
    auto vtx1 = vtx(vxCandidate);
    vtx1 -= vertex;
    double pT = mom.perp();
    return (mom.x()*vtx1.x()+mom.y()*vtx1.y())/(pT*vtx1.perp());
  }

  double V0Tools::cosTheta_xy(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex) const
  {
    auto mom = V0Momentum(vxCandidate);
    auto vtx1 = vtx(vxCandidate);
    vtx1 -= vertex->position();
    double pT = mom.perp();
    return (mom.x()*vtx1.x()+mom.y()*vtx1.y())/(pT*vtx1.perp());
  }

  float V0Tools::charge(const xAOD::Vertex * vxCandidate) const
  {
    float ch = 0.;
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    for( unsigned int it=0; it<NTrk; it++) {
      float trkCharge = vxCandidate->trackParticle(it)->charge();
      //const Trk::TrackParameters* bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
      //float trkCharge = 1.;
      //if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      ch += trkCharge;
    }
    return ch;
  }

  const xAOD::TrackParticle* V0Tools::origTrack(const xAOD::Vertex * vxCandidate, int trkIndex) const
  {
    return vxCandidate->trackParticle(trkIndex);
  }

  const xAOD::TrackParticle* V0Tools::positiveOrigTrack(const xAOD::Vertex * vxCandidate) const
  {
    const xAOD::TrackParticle* origTrk(nullptr);
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (NTrk != 2) return origTrk;
    for( unsigned int it=0; it<NTrk; it++) {
      if(vxCandidate->trackParticle(it)->charge() > 0) origTrk = vxCandidate->trackParticle(it);
    }
    return origTrk;
  }

  const xAOD::TrackParticle* V0Tools::negativeOrigTrack(const xAOD::Vertex * vxCandidate) const
  {
    const xAOD::TrackParticle* origTrk(nullptr);
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (NTrk != 2) return origTrk;
    for( unsigned int it=0; it<NTrk; it++) {
      if(vxCandidate->trackParticle(it)->charge() < 0) origTrk = vxCandidate->trackParticle(it);
    }
    return origTrk;
  }

  /* JRC - NOT POSSIBLE WITH NEW EDM
  Rec::TrackParticle* V0Tools::createParticle(const ExtendedVxCandidate * vxCandidate) const
  {
    //if(charge(vxCandidate) != 0) {
    //  ATH_MSG_DEBUG("sum of charges in vxCandidate is not 0. No Neutral Perigee created");
    //  return NULL;
    //}
    Trk::V0Hypothesis* hypothesis = new Trk::V0Hypothesis(*vxCandidate,0,0,0);
    Rec::TrackParticle* neutral = createParticle(hypothesis);
    delete hypothesis;
    return neutral;
  }
  */

  /* JRC - NOT POSSIBLE WITH NEW EDM 
  Rec::TrackParticle* V0Tools::createParticle(const Trk::V0Hypothesis* v0Hypothesis) const
  {
    const std::vector<Trk::VxTrackAtVertex*> * myTrackVectorPtr=v0Hypothesis->vxTrackAtVertex();
    if (myTrackVectorPtr==0) {
      ATH_MSG_DEBUG("0 pointer for vector of Tracks in VxCandidate. No Neutral Perigee could be created");
      return 0;
    }

    Amg::Vector3D gp(0.,0.,0.), gp0(0.,0.,0.);
    Rec::TrackParticle* nTrkPrt;
    unsigned int NTrk = v0Hypothesis->vxTrackAtVertex()->size();

    double Px=0., Py=0., Pz=0., D0=0., Z0=0., T0=0.; 
    CLHEP::HepVector phi(NTrk,0), theta(NTrk,0), qOverP(NTrk,0);
    CLHEP::HepVector dPxdphi(NTrk,0), dPxdtheta(NTrk,0), dPxdqOverP(NTrk,0);
    CLHEP::HepVector dPydphi(NTrk,0), dPydtheta(NTrk,0), dPydqOverP(NTrk,0);
    CLHEP::HepVector dPzdphi(NTrk,0), dPzdtheta(NTrk,0), dPzdqOverP(NTrk,0);
    CLHEP::HepVector dPhidphi(NTrk,0), dPhidtheta(NTrk,0), dPhidqOverP(NTrk,0);
    CLHEP::HepVector dThetadphi(NTrk,0), dThetadtheta(NTrk,0), dThetadqOverP(NTrk,0);
    CLHEP::HepVector dinvPdphi(NTrk,0), dinvPdtheta(NTrk,0), dinvPdqOverP(NTrk,0);
    CLHEP::HepVector dD0dphi(NTrk,0), dD0dtheta(NTrk,0), dD0dqOverP(NTrk,0);
    CLHEP::HepVector dZ0dphi(NTrk,0), dZ0dtheta(NTrk,0), dZ0dqOverP(NTrk,0);

    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters*  bPer = (*(v0Hypothesis->vxTrackAtVertex()))[it]->perigeeAtVertex();
      phi[it]    = bPer->parameters()[Trk::phi];
      theta[it]  = bPer->parameters()[Trk::theta];
      qOverP[it] = bPer->parameters()[Trk::qOverP];
      double px = bPer->momentum()[Trk::px];
      double py = bPer->momentum()[Trk::py];
      double pz = bPer->momentum()[Trk::pz];
      Px += px;
      Py += py;
      Pz += pz;
      dPxdphi[it] = -py;
      dPydphi[it] =  px;
      dPxdtheta[it] =  cos(phi[it])*pz;
      dPydtheta[it] =  sin(phi[it])*pz;
      dPzdtheta[it] = -sin(theta[it])/fabs(qOverP[it]);
      dPxdqOverP[it] = -px/qOverP[it];
      dPydqOverP[it] = -py/qOverP[it];
      dPzdqOverP[it] = -pz/qOverP[it];
    }

    double Ptsq = Px*Px + Py*Py;
    double Pt = (Ptsq>0.) ? sqrt(Ptsq) : 0.;
    double Psq = Ptsq + Pz*Pz;
    double P = (Psq>0.) ? sqrt(Psq) : 0.;
    double invP = 1./P;
    double Phi = atan2(Py,Px);
    double Theta = acos(Pz/P);
    while ( fabs(Phi) > M_PI ) Phi += ( Phi > 0. ) ? -2.*M_PI : 2.*M_PI;
    while ( Theta > 2.*M_PI ) Theta -= 2.*M_PI;
    while ( Theta < -M_PI ) Theta += M_PI;
    if ( Theta > M_PI ) {
      Theta = 2.*M_PI - Theta;
      if ( Phi >= 0. ) Phi += ( Phi > 0. ) ? -M_PI : M_PI;
    }
    if ( Theta < 0. ) {
      Theta = - Theta;
      if ( Phi >= 0. ) Phi += ( Phi > 0. ) ? -M_PI : M_PI;
    }

    double dD0dx = -sin(Phi);
    double dD0dy =  cos(Phi);
    double dZ0dx = -cos(Phi)/tan(Theta);
    double dZ0dy = -sin(Phi)/tan(Theta);
    double dZ0dz = 1.;

    double dPhidPx = -Py/(Pt*Pt); double dPhidPy = Px/(Pt*Pt);
    double dThetadPx = Px*Pz/(P*P*Pt); double dThetadPy = Py*Pz/(P*P*Pt); double dThetadPz = -Pt/(P*P);

    for( unsigned int it=0; it<NTrk; it++) {
      dPhidphi[it]      = dPhidPx*dPxdphi[it]    + dPhidPy*dPydphi[it];
      dPhidtheta[it]    = dPhidPx*dPxdtheta[it]  + dPhidPy*dPydtheta[it];
      dPhidqOverP[it]   = dPhidPx*dPxdqOverP[it] + dPhidPy*dPydqOverP[it];
      dThetadphi[it]    = dThetadPx*dPxdphi[it]    + dThetadPy*dPydphi[it]    + dThetadPz*dPzdphi[it];
      dThetadtheta[it]  = dThetadPx*dPxdtheta[it]  + dThetadPy*dPydtheta[it]  + dThetadPz*dPzdtheta[it];
      dThetadqOverP[it] = dThetadPx*dPxdqOverP[it] + dThetadPy*dPydqOverP[it] + dThetadPz*dPzdqOverP[it];
      dinvPdphi[it]     = -(Px*dPxdphi[it]    + Py*dPydphi[it]    + Pz*dPzdphi[it]   )*invP/(P*P);
      dinvPdtheta[it]   = -(Px*dPxdtheta[it]  + Py*dPydtheta[it]  + Pz*dPzdtheta[it] )*invP/(P*P);
      dinvPdqOverP[it]  = -(Px*dPxdqOverP[it] + Py*dPydqOverP[it] + Pz*dPzdqOverP[it])*invP/(P*P);
    }

    unsigned int ndim = 0;
    if (v0Hypothesis->fullCovariance() != 0) {
      const Amg::MatrixX* fullCov = v0Hypothesis->fullCovariance();
      ndim = fullCov->rows();
    } else {
      ATH_MSG_DEBUG("0 pointer for full covariance. Making-up one from the vertex and tracks covariances");
    }

    int mdim = 5*NTrk+3;
    if (ndim == 3*NTrk+3) mdim = 3*NTrk+3;
    Amg::MatrixX fullCov(mdim,mdim); fullCov.setZero();
    if (ndim != 0) {
      if (ndim == 5*NTrk+6) {
        fullCov = v0Hypothesis->fullCovariance()->block(0,0,5*NTrk+2,5*NTrk+2);
      } else {
        fullCov = *(v0Hypothesis->fullCovariance());
      }
    } else {
      const Amg::MatrixX& cov = v0Hypothesis->recVertex().covariancePosition();
      // JRC who commented this?
      //const Trk::TrackParameters*  bPer0 = (*(v0Hypothesis->vxTrackAtVertex()))[0]->perigeeAtVertex();
      //const Trk::TrackParameters*  bPer1 = (*(v0Hypothesis->vxTrackAtVertex()))[1]->perigeeAtVertex();
      //const Trk::MeasuredPerigee* mPer0 = dynamic_cast<const Trk::MeasuredPerigee*>(bPer0);
      //const Trk::MeasuredPerigee* mPer1 = dynamic_cast<const Trk::MeasuredPerigee*>(bPer1);
      //Trk::CovarianceMatrix cov0 = mPer0->localErrorMatrix().covariance();
      //Trk::CovarianceMatrix cov1 = mPer1->localErrorMatrix().covariance();
      //fullCov.sub(1, cov0);
      //fullCov.sub(6, cov1);
      //fullCov.sub(11, cov);
      for( unsigned int it=0; it<NTrk; it++) {
        const Trk::TrackParameters*  bPer_tmp = (*(v0Hypothesis->vxTrackAtVertex()))[it]->perigeeAtVertex();
        if (bPer_tmp == 0) continue;
        const AmgSymMatrix(5)* cov_tmp = bPer_tmp->covariance();
        fullCov.block(5*it,5*it,5,5) = *cov_tmp;
      }
      fullCov.block(5*NTrk,5*NTrk,cov.rows(),cov.rows()) = cov;
    }

    Amg::MatrixX DerCovV0(5,5*NTrk+3); DerCovV0.setZero();
    Amg::MatrixX DerCovVK(5,3*NTrk+3); DerCovVK.setZero();

    if (ndim == 5*NTrk+3 || ndim == 5*NTrk+6 || ndim == 0) {
      DerCovV0(0,5*NTrk+0) = dD0dx;
      DerCovV0(0,5*NTrk+1) = dD0dy;
      DerCovV0(1,5*NTrk+0) = dZ0dx;
      DerCovV0(1,5*NTrk+1) = dZ0dy;
      DerCovV0(1,5*NTrk+2) = dZ0dz;
      for( unsigned int it=0; it<NTrk; it++) {
        DerCovV0(2,2+5*it) = dPhidphi[it];
        DerCovV0(2,3+5*it) = dPhidtheta[it];
        DerCovV0(2,4+5*it) = dPhidqOverP[it];
        DerCovV0(3,2+5*it) = dThetadphi[it];
        DerCovV0(3,3+5*it) = dThetadtheta[it];
        DerCovV0(3,4+5*it) = dThetadqOverP[it];
        DerCovV0(4,2+5*it) = dinvPdphi[it];
        DerCovV0(4,3+5*it) = dinvPdtheta[it];
        DerCovV0(4,4+5*it) = dinvPdqOverP[it];
      }
    } else if (ndim == 3*NTrk+3) {
      DerCovVK(0,0) = dD0dx;
      DerCovVK(0,1) = dD0dy;
      DerCovVK(1,0) = dZ0dx;
      DerCovVK(1,1) = dZ0dy;
      DerCovVK(1,2) = dZ0dz;
      for( unsigned int it=0; it<NTrk; it++) {
        DerCovVK(2,3+3*it) = dPhidphi[it];
        DerCovVK(2,4+3*it) = dPhidtheta[it];
        DerCovVK(2,5+3*it) = dPhidqOverP[it];
        DerCovVK(3,3+3*it) = dThetadphi[it];
        DerCovVK(3,4+3*it) = dThetadtheta[it];
        DerCovVK(3,5+3*it) = dThetadqOverP[it];
        DerCovVK(4,3+3*it) = dinvPdphi[it];
        DerCovVK(4,4+3*it) = dinvPdtheta[it];
        DerCovVK(4,5+3*it) = dinvPdqOverP[it];
      }
    }

    AmgSymMatrix(5) SumCov; SumCov.setZero();
    const Trk::NeutralParameters* perigee;
    std::vector<const Trk::NeutralParameters*> tmpPar;

// Perigee at fitted vertex
    if (ndim == 5*NTrk+3 || ndim == 5*NTrk+6 || ndim == 0) SumCov = fullCov.similarity( DerCovV0 );
    if (ndim == 3*NTrk+3) SumCov = fullCov.similarity( DerCovVK );

    gp = v0Hypothesis->recVertex().position();
    PerigeeSurface surface;
    
    perigee = surface.createNeutralParameters( D0, Z0, Phi, Theta, invP, &SumCov );
    tmpPar.push_back(perigee);

// Perigee with respect to (0,0,0)
    gp = gp0;
    D0 = sin(Phi) * (gp.x() - v0Hypothesis->recVertex().position().x()) -
         cos(Phi) * (gp.y() - v0Hypothesis->recVertex().position().y());
    T0 = (gp.x() - v0Hypothesis->recVertex().position().x())*cos(Phi) +
         (gp.y() - v0Hypothesis->recVertex().position().y())*sin(Phi);
    Z0 = -gp.z() + v0Hypothesis->recVertex().position().z() + T0/tan(Theta);

    for( unsigned int it=0; it<NTrk; it++) {
      dD0dphi[it]    =  T0*dPhidphi[it];
      dD0dtheta[it]  =  T0*dPhidtheta[it];
      dD0dqOverP[it] =  T0*dPhidqOverP[it];
      dZ0dphi[it]    = -D0*dPhidphi[it]/tan(Theta)    - T0*dThetadphi[it]/(sin(Theta)*sin(Theta));
      dZ0dtheta[it]  = -D0*dPhidtheta[it]/tan(Theta)  - T0*dThetadtheta[it]/(sin(Theta)*sin(Theta));
      dZ0dqOverP[it] = -D0*dPhidqOverP[it]/tan(Theta) - T0*dThetadqOverP[it]/(sin(Theta)*sin(Theta));
    }

    if (ndim == 5*NTrk+3 || ndim == 5*NTrk+6 || ndim == 0) {
      for( unsigned int it=0; it<NTrk; it++) {
        DerCovV0(0,2+5*it) = dD0dphi[it];
        DerCovV0(0,3+5*it) = dD0dtheta[it];
        DerCovV0(0,4+5*it) = dD0dqOverP[it];
        DerCovV0(1,2+5*it) = dZ0dphi[it];
        DerCovV0(1,3+5*it) = dZ0dtheta[it];
        DerCovV0(1,4+5*it) = dZ0dqOverP[it];
      }
    } else if (ndim == 3*NTrk+3) {
      for( unsigned int it=0; it<NTrk; it++) {
        DerCovVK(0,3+3*it) = dD0dphi[it];
        DerCovVK(0,4+3*it) = dD0dtheta[it];
        DerCovVK(0,5+3*it) = dD0dqOverP[it];
        DerCovVK(1,3+3*it) = dZ0dphi[it];
        DerCovVK(1,4+3*it) = dZ0dtheta[it];
        DerCovVK(1,5+3*it) = dZ0dqOverP[it];
      }
    }

    if (ndim == 5*NTrk+3 || ndim == 5*NTrk+6 || ndim == 0) SumCov = fullCov.similarity( DerCovV0 );
    if (ndim == 3*NTrk+3) SumCov = fullCov.similarity( DerCovVK );

    perigee = surface.createNeutralParameters( D0, Z0, Phi, Theta, invP, &SumCov );

    const Trk::FitQuality* fitQuality = new Trk::FitQuality(v0Hypothesis->recVertex().fitQuality().chiSquared(),
                                                            v0Hypothesis->recVertex().fitQuality().numberDoF());

    nTrkPrt = new Rec::TrackParticle(0, Trk::V0Vtx, v0Hypothesis, new Trk::TrackSummary(),
                                     tmpPar, perigee, fitQuality);
    return nTrkPrt;
  }
  */  

  double V0Tools::invariantMassBeforeFitIP(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    std::vector<double> masses = {posTrackMass, negTrackMass};
    return invariantMassBeforeFitIP(vxCandidate,masses);
  }

  double V0Tools::invariantMassBeforeFitIP(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses) const
  {
    double px = 0., py = 0., pz = 0., e = 0.;
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    for( unsigned int it=0; it<NTrk; it++) {
      if (masses[it] >= 0.) {
        const xAOD::TrackParticle* TP = origTrack(vxCandidate,it);
        if (TP == nullptr) return -999999.;
        px += TP->p4().Px();
        py += TP->p4().Py();
        pz += TP->p4().Pz();
        double pesq = 1./(TP->qOverP()*TP->qOverP()) + masses[it]*masses[it];
        double pe = (pesq>0.) ? sqrt(pesq) : 0.;
        e += pe;
      }
    }
    double msq = e*e - px*px - py*py - pz*pz;
    return (msq>0.) ? sqrt(msq) : 0.;
  }

  double V0Tools::invariantMassBeforeFit(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    std::vector<double> masses = {posTrackMass, negTrackMass};
    return invariantMassBeforeFit(vxCandidate,masses);
  }

  double V0Tools::invariantMassBeforeFit(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses) const
  {
    Trk::PerigeeSurface perigeeSurface(vxCandidate->position());
    double px = 0., py = 0., pz = 0., e = 0.;
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    for( unsigned int it=0; it<NTrk; it++) {
      if (masses[it] >= 0.) {
        const xAOD::TrackParticle* TP = origTrack(vxCandidate,it);
        if (TP == nullptr) return -999999.;
        const Trk::TrackParameters* extrPer = m_extrapolator->extrapolate(*TP, perigeeSurface);
        if (extrPer == nullptr) return -999999.;
        px += extrPer->momentum().x();
        py += extrPer->momentum().y();
        pz += extrPer->momentum().z();
        double pesq = extrPer->momentum().mag2() + masses[it]*masses[it];
        double pe = (pesq>0.) ? sqrt(pesq) : 0.;
        e += pe;
        delete extrPer;
      }
    }
    double msq = e*e - px*px - py*py - pz*pz;
    return (msq>0.) ? sqrt(msq) : 0.;
  }

  double V0Tools::invariantMassBeforeFit(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass, const Amg::Vector3D& vertex) const
  {
    std::vector<double> masses = {posTrackMass, negTrackMass};
    return invariantMassBeforeFit(vxCandidate,masses,vertex);
  }

  double V0Tools::invariantMassBeforeFit(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses, const Amg::Vector3D& vertex) const
  {
    Trk::PerigeeSurface perigeeSurface(vertex);
    double px = 0., py = 0., pz = 0., e = 0.;
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    for( unsigned int it=0; it<NTrk; it++) {
      if (masses[it] >= 0.) {
        const xAOD::TrackParticle* TP = origTrack(vxCandidate,it);
        if (TP == nullptr) return -999999.;
        const Trk::TrackParameters* extrPer = m_extrapolator->extrapolate(*TP, perigeeSurface);
        if (extrPer == nullptr) return -999999.;
        px += extrPer->momentum().x();
        py += extrPer->momentum().y();
        pz += extrPer->momentum().z();
        double pesq = extrPer->momentum().mag2() + masses[it]*masses[it];
        double pe = (pesq>0.) ? sqrt(pesq) : 0.;
        e += pe;
        delete extrPer;
      }
    }
    double msq = e*e - px*px - py*py - pz*pz;
    return (msq>0.) ? sqrt(msq) : 0.;
  }

  double V0Tools::invariantMassErrorBeforeFitIP(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    std::vector<double> masses = {posTrackMass, negTrackMass};

    return invariantMassErrorBeforeFitIP(vxCandidate,masses);
  }

  double V0Tools::invariantMassErrorBeforeFitIP(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses) const
  {
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    double mass = invariantMassBeforeFitIP(vxCandidate, masses);
    double E=0., Px=0., Py=0., Pz=0.; 
    std::vector<double>phi(NTrk), theta(NTrk), qOverP(NTrk), charge(NTrk), e(NTrk);
    std::vector<double>dm2dphi(NTrk), dm2dtheta(NTrk), dm2dqOverP(NTrk);
    Amg::MatrixX V0_cor(5*NTrk,5*NTrk); V0_cor.setZero();
    for( unsigned int it=0; it<NTrk; it++) {
      if (masses[it] >= 0.) {
        const xAOD::TrackParticle* TP = origTrack(vxCandidate,it);
        if (TP == nullptr) return -999999.;
        const xAOD::ParametersCovMatrix_t cov_tmp = TP->definingParametersCovMatrix();
        V0_cor(5*it+2,5*it+2) = cov_tmp(2,2);
        V0_cor(5*it+2,5*it+3) = cov_tmp(2,3);
        V0_cor(5*it+2,5*it+4) = cov_tmp(2,4);
        V0_cor(5*it+3,5*it+3) = cov_tmp(3,3);
        V0_cor(5*it+3,5*it+4) = cov_tmp(3,4);
        V0_cor(5*it+4,5*it+4) = cov_tmp(4,4);
        V0_cor(5*it+3,5*it+2) = cov_tmp(2,3);
        V0_cor(5*it+4,5*it+2) = cov_tmp(2,4);
        V0_cor(5*it+4,5*it+3) = cov_tmp(3,4);
        charge[it] = TP->charge();
        phi[it]    = TP->phi();
        theta[it]  = TP->theta();
        qOverP[it] = TP->qOverP();
        double tmp = 1./(qOverP[it]*qOverP[it]) + masses[it]*masses[it];
        double pe = (tmp>0.) ? sqrt(tmp) : 0.;
        e[it] = pe;
        E  += e[it];
        Px += TP->p4().Px();
        Py += TP->p4().Py();
        Pz += TP->p4().Pz();
      }
    }

    for( unsigned int it=0; it<NTrk; it++) {
      if (masses[it] >= 0.) {
        dm2dphi[it]    = 2.*(Px*sin(phi[it])-Py*cos(phi[it]))*sin(theta[it])*charge[it]/qOverP[it];
        dm2dtheta[it]  = 2.*(Pz*sin(theta[it])-(Px*cos(phi[it])+Py*sin(phi[it]))*cos(theta[it]))*charge[it]/qOverP[it];
        dm2dqOverP[it] = 2.*(Pz*cos(theta[it])+(Px*cos(phi[it])+Py*sin(phi[it]))*sin(theta[it])-E*charge[it]/(qOverP[it]*e[it]))*charge[it]/(qOverP[it]*qOverP[it]);
      }
    }

    Amg::MatrixX D_vec(5*NTrk,1); D_vec.setZero();
    for( unsigned int it=0; it<NTrk; it++) {
      D_vec(5*it+0,0)  = 0.;
      D_vec(5*it+1,0)  = 0.;
      D_vec(5*it+2,0)  = dm2dphi[it];
      D_vec(5*it+3,0)  = dm2dtheta[it];
      D_vec(5*it+4,0)  = dm2dqOverP[it];
    }
    Amg::MatrixX V0_merr = D_vec.transpose() * V0_cor * D_vec;

    double massVarsq = V0_merr(0,0);
    if (massVarsq <= 0.) ATH_MSG_DEBUG("massError: negative sqrt massVarsq " << massVarsq);
    double massVar = (massVarsq>0.) ? sqrt(massVarsq) : 0.;
    double massErr = massVar/(2.*mass);
    return massErr;
  }

  double V0Tools::invariantMassErrorBeforeFit(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    std::vector<double> masses = {posTrackMass, negTrackMass};

    return invariantMassErrorBeforeFit(vxCandidate,masses);
  }

  double V0Tools::invariantMassErrorBeforeFit(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses) const
  {
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    Amg::Vector3D vertex = vxCandidate->position();
    return invariantMassErrorBeforeFit(vxCandidate,masses,vertex);
  }
 
  double V0Tools::invariantMassErrorBeforeFit(const xAOD::Vertex * vxCandidate, double posTrackMass, double negTrackMass, const Amg::Vector3D& vertex) const
  {
    std::vector<double> masses = {posTrackMass, negTrackMass};

    return invariantMassErrorBeforeFit(vxCandidate,masses,vertex);
  }

  double V0Tools::invariantMassErrorBeforeFit(const xAOD::Vertex * vxCandidate, const std::vector<double> &masses, const Amg::Vector3D& vertex) const
  {
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    Trk::PerigeeSurface perigeeSurface(vertex);
    double E=0., Px=0., Py=0., Pz=0.; 
    std::vector<double>phi(NTrk), theta(NTrk), qOverP(NTrk), charge(NTrk), e(NTrk);
    std::vector<double>dm2dphi(NTrk), dm2dtheta(NTrk), dm2dqOverP(NTrk);
    Amg::MatrixX V0_cor(5*NTrk,5*NTrk); V0_cor.setZero();
    for( unsigned int it=0; it<NTrk; it++) {
      if (masses[it] >= 0.) {
        const xAOD::TrackParticle* TP = origTrack(vxCandidate,it);
        if (TP == nullptr) return -999999.;
        const Trk::TrackParameters* extrPer = m_extrapolator->extrapolate(*TP, perigeeSurface);
        if (extrPer == nullptr) return -999999.;
        const AmgSymMatrix(5)* cov_tmp = extrPer->covariance();
        V0_cor(5*it+2,5*it+2) = (*cov_tmp)(2,2);
        V0_cor(5*it+2,5*it+3) = (*cov_tmp)(2,3);
        V0_cor(5*it+2,5*it+4) = (*cov_tmp)(2,4);
        V0_cor(5*it+3,5*it+3) = (*cov_tmp)(3,3);
        V0_cor(5*it+3,5*it+4) = (*cov_tmp)(3,4);
        V0_cor(5*it+4,5*it+4) = (*cov_tmp)(4,4);
        V0_cor(5*it+3,5*it+2) = (*cov_tmp)(2,3);
        V0_cor(5*it+4,5*it+2) = (*cov_tmp)(2,4);
        V0_cor(5*it+4,5*it+3) = (*cov_tmp)(3,4);
        charge[it] = TP->charge();
        phi[it]    = extrPer->parameters()[Trk::phi];
        theta[it]  = extrPer->parameters()[Trk::theta];
        qOverP[it] = extrPer->parameters()[Trk::qOverP];
        double tmp = 1./(qOverP[it]*qOverP[it]) + masses[it]*masses[it];
        double pe = (tmp>0.) ? sqrt(tmp) : 0.;
        e[it] = pe;
        E  += e[it];
        Px += extrPer->momentum().x();
        Py += extrPer->momentum().y();
        Pz += extrPer->momentum().z();
        delete extrPer;
      }
    }
    double msq = E*E - Px*Px - Py*Py - Pz*Pz;
    double mass = (msq>0.) ? sqrt(msq) : 0.;

    for( unsigned int it=0; it<NTrk; it++) {
      if (masses[it] >= 0.) {
        dm2dphi[it]    = 2.*(Px*sin(phi[it])-Py*cos(phi[it]))*sin(theta[it])*charge[it]/qOverP[it];
        dm2dtheta[it]  = 2.*(Pz*sin(theta[it])-(Px*cos(phi[it])+Py*sin(phi[it]))*cos(theta[it]))*charge[it]/qOverP[it];
        dm2dqOverP[it] = 2.*(Pz*cos(theta[it])+(Px*cos(phi[it])+Py*sin(phi[it]))*sin(theta[it])-E*charge[it]/(qOverP[it]*e[it]))*charge[it]/(qOverP[it]*qOverP[it]);
      }
    }

    Amg::MatrixX D_vec(5*NTrk,1); D_vec.setZero();
    for( unsigned int it=0; it<NTrk; it++) {
      D_vec(5*it+0,0)  = 0.;
      D_vec(5*it+1,0)  = 0.;
      D_vec(5*it+2,0)  = dm2dphi[it];
      D_vec(5*it+3,0)  = dm2dtheta[it];
      D_vec(5*it+4,0)  = dm2dqOverP[it];
    }
    Amg::MatrixX V0_merr = D_vec.transpose() * V0_cor * D_vec;

    double massVarsq = V0_merr(0,0);
    if (massVarsq <= 0.) ATH_MSG_DEBUG("massError: negative sqrt massVarsq " << massVarsq);
    double massVar = (massVarsq>0.) ? sqrt(massVarsq) : 0.;
    double massErr = massVar/(2.*mass);
    return massErr;
  }

  double V0Tools::massTauCov(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, double posTrackMass, double negTrackMass) const
  {
    std::vector<double> masses = {posTrackMass, negTrackMass};

    return massTauCov(vxCandidate,vertex,masses);
  }

  double V0Tools::massTauCov(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, const std::vector<double>  &masses) const
  {
    // Tau = CONST*M*(Px*dx+Py*dy)/(PT*PT)
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    //double CONST = 1000./CLHEP::c_light;
    double CONST = 1000./299.792;
    double PT = V0Momentum(vxCandidate).perp();
    auto vert = vxCandidate->position() - vertex->position();
    double dx = vert.x();
    double dy = vert.y();
    double M = invariantMass(vxCandidate, masses);
    double E=0., Px=0., Py=0., Pz=0.; 
    std::vector<double>dpxdqOverP(NTrk), dpxdtheta(NTrk), dpxdphi(NTrk);
    std::vector<double>dpydqOverP(NTrk), dpydtheta(NTrk), dpydphi(NTrk);
    std::vector<double>dpzdqOverP(NTrk), dpzdtheta(NTrk), dedqOverP(NTrk);
    std::vector<double>dMdqOverP(NTrk), dMdtheta(NTrk), dMdphi(NTrk);
    std::vector<double>dTaudqOverP(NTrk), dTaudtheta(NTrk), dTaudphi(NTrk);

    Amg::MatrixX* fullCov = convertCovMatrix(vxCandidate);
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters* bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      double phi    = bPer->parameters()[Trk::phi];
      double theta  = bPer->parameters()[Trk::theta];
      double qOverP = bPer->parameters()[Trk::qOverP];
      double tmp = 1./(qOverP*qOverP) + masses[it]*masses[it];
      double pe = (tmp>0.) ? sqrt(tmp) : 0.;
      dedqOverP[it]  = -1./(qOverP*qOverP*qOverP*pe);
      dpxdqOverP[it] = -(sin(theta)*cos(phi)*trkCharge)/(qOverP*qOverP);
      dpxdtheta[it]  =  (cos(theta)*cos(phi)*trkCharge)/qOverP;
      dpxdphi[it]    = -(sin(theta)*sin(phi)*trkCharge)/qOverP;
      dpydqOverP[it] = -(sin(theta)*sin(phi)*trkCharge)/(qOverP*qOverP);
      dpydtheta[it]  =  (cos(theta)*sin(phi)*trkCharge)/qOverP;
      dpydphi[it]    =  (sin(theta)*cos(phi)*trkCharge)/qOverP;
      dpzdqOverP[it] = -(cos(theta)*trkCharge)/(qOverP*qOverP);
      dpzdtheta[it]  = -(sin(theta)*trkCharge)/qOverP;
      E  += pe;
      Px += bPer->momentum()[Trk::px];
      Py += bPer->momentum()[Trk::py];
      Pz += bPer->momentum()[Trk::pz];
    }
    double LXY = Px*dx+Py*dy;

    for( unsigned int it=0; it<NTrk; it++) {
      dMdqOverP[it]   = -(Px*dpxdqOverP[it]+Py*dpydqOverP[it]+Pz*dpzdqOverP[it]-E*dedqOverP[it])/M;
      dMdtheta[it]    = -(Px*dpxdtheta[it]+Py*dpydtheta[it]+Pz*dpzdtheta[it])/M;
      dMdphi[it]      = -(Px*dpxdphi[it]+Py*dpydphi[it])/M;
      double dPTdqOverP  =  (Px*dpxdqOverP[it]+Py*dpydqOverP[it])/PT;
      double dPTdtheta  =  (Px*dpxdtheta[it]+Py*dpydtheta[it])/PT;
      double dPTdphi     =  (Px*dpxdphi[it]+Py*dpydphi[it])/PT;
      double dLXYdqOverP =  dx*dpxdqOverP[it]+dy*dpydqOverP[it];
      double dLXYdtheta  =  dx*dpxdtheta[it]+dy*dpydtheta[it];
      double dLXYdphi    =  dx*dpxdphi[it]+dy*dpydphi[it];
      dTaudqOverP[it] =  (LXY*dMdqOverP[it]+M*dLXYdqOverP)/(PT*PT)-(2.*LXY*M*dPTdqOverP)/(PT*PT*PT);
      dTaudtheta[it]  =  (LXY*dMdtheta[it]+M*dLXYdtheta)/(PT*PT)-(2.*LXY*M*dPTdtheta)/(PT*PT*PT);
      dTaudphi[it]    =  (LXY*dMdphi[it]+M*dLXYdphi)/(PT*PT)-(2.*LXY*M*dPTdphi)/(PT*PT*PT);
    }
    double dTaudx = (M*Px)/(PT*PT);
    double dTaudy = (M*Py)/(PT*PT);
    double dTaudx0 = -dTaudx;
    double dTaudy0 = -dTaudy;

    unsigned int ndim = 0;
    if (fullCov != nullptr) {
      ndim = fullCov->rows();
    } else {
      ndim = 5*NTrk+3;
    }

    Amg::MatrixX V0_err;
    if (ndim == 5*NTrk+3 || ndim == 5*NTrk+6) {
      Amg::MatrixX D_mat(5*NTrk+6,2); D_mat.setZero();
      for( unsigned int it=0; it<NTrk; it++) {
        D_mat(5*it+0,0)  = 0.;
        D_mat(5*it+1,0)  = 0.;
        D_mat(5*it+2,0)  = CONST*dTaudphi[it];
        D_mat(5*it+3,0)  = CONST*dTaudtheta[it];
        D_mat(5*it+4,0)  = CONST*dTaudqOverP[it];
        D_mat(5*it+0,1)  = 0.;
        D_mat(5*it+1,1)  = 0.;
        D_mat(5*it+2,1)  = dMdphi[it];
        D_mat(5*it+3,1)  = dMdtheta[it];
        D_mat(5*it+4,1)  = dMdqOverP[it];
      }
      D_mat(5*NTrk+0,0) = CONST*dTaudx;
      D_mat(5*NTrk+1,0) = CONST*dTaudy;
      D_mat(5*NTrk+2,0) = 0.;
      D_mat(5*NTrk+3,0) = CONST*dTaudx0;
      D_mat(5*NTrk+4,0) = CONST*dTaudy0;
      D_mat(5*NTrk+5,0) = 0.;
      Amg::MatrixX W_mat(5*NTrk+6,5*NTrk+6); W_mat.setZero();
      if (fullCov != nullptr) {
        W_mat.block(0,0,ndim,ndim) = *fullCov;
      } else {
        Amg::MatrixX V0_cov = makeV0Cov(vxCandidate);
        W_mat.block(0,0,V0_cov.rows(),V0_cov.rows()) = V0_cov;
        W_mat.block(5*NTrk,5*NTrk,3,3) = vxCandidate->covariancePosition();
      }
      W_mat.block(5*NTrk+3,5*NTrk+3,3,3) =  vertex->covariancePosition();
      V0_err = D_mat.transpose() * W_mat * D_mat;
    } else if (ndim == 3*NTrk+3) {
      Amg::MatrixX D_mat(3*NTrk+6,2); D_mat.setZero();
      D_mat(0,0)  = CONST*dTaudx;
      D_mat(1,0)  = CONST*dTaudy;
      D_mat(2,0)  = 0.;
      for( unsigned int it=0; it<NTrk; it++) {
        D_mat(3*it+3,0)  = CONST*dTaudphi[it];
        D_mat(3*it+4,0)  = CONST*dTaudtheta[it];
        D_mat(3*it+5,0)  = CONST*dTaudqOverP[it];
        D_mat(3*it+3,1)  = dMdphi[it];
        D_mat(3*it+4,1)  = dMdtheta[it];
        D_mat(3*it+5,1)  = dMdqOverP[it];
      }
      D_mat(3*NTrk+3,0) = CONST*dTaudx0;
      D_mat(3*NTrk+4,0) = CONST*dTaudy0;
      D_mat(3*NTrk+5,0) = 0.;
      Amg::MatrixX W_mat(3*NTrk+6,3*NTrk+6); W_mat.setZero();
      W_mat.block(0,0,ndim,ndim) = *fullCov;
      W_mat.block(3*NTrk+3,3*NTrk+3,3,3) = vertex->covariancePosition();
      V0_err = D_mat.transpose() * W_mat * D_mat;
    }
    delete fullCov;
    return V0_err(0,1);
  }

  Amg::MatrixX V0Tools::makeV0Cov(const xAOD::Vertex * vxCandidate) const{
      unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
      Amg::MatrixX V0_cov(5*NTrk,5*NTrk); V0_cov.setZero();
      for( unsigned int it=0; it<NTrk; it++){
          const Trk::TrackParameters* bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
          const AmgSymMatrix(5)* cov_tmp = bPer->covariance();
          V0_cov(5*it+2,5*it+2) = (*cov_tmp)(2,2);
          V0_cov(5*it+2,5*it+3) = (*cov_tmp)(2,3);
          V0_cov(5*it+2,5*it+4) = (*cov_tmp)(2,4);
          V0_cov(5*it+3,5*it+3) = (*cov_tmp)(3,3);
          V0_cov(5*it+3,5*it+4) = (*cov_tmp)(3,4);
          V0_cov(5*it+4,5*it+4) = (*cov_tmp)(4,4);
          V0_cov(5*it+3,5*it+2) = (*cov_tmp)(2,3);
          V0_cov(5*it+4,5*it+2) = (*cov_tmp)(2,4);
          V0_cov(5*it+4,5*it+3) = (*cov_tmp)(3,4);
      }
      return V0_cov;
  }
  
  Amg::MatrixX V0Tools::tauMassCovariance(const xAOD::Vertex * vxCandidate, const xAOD::Vertex* vertex, const std::vector<double> &masses) const
  {
    // Tau = CONST*M*(Px*dx+Py*dy)/(PT*PT)
    Amg::MatrixX V0_err;
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return V0_err;
    }
    //double CONST = 1000./CLHEP::c_light;
    double CONST = 1000./299.792;
    double PT = V0Momentum(vxCandidate).perp();
    auto vert = vxCandidate->position() - vertex->position();
    double dx = vert.x();
    double dy = vert.y();
    double M = invariantMass(vxCandidate, masses);
    double E=0., Px=0., Py=0., Pz=0.; 
    std::vector<double>dpxdqOverP(NTrk), dpxdtheta(NTrk), dpxdphi(NTrk);
    std::vector<double>dpydqOverP(NTrk), dpydtheta(NTrk), dpydphi(NTrk);
    std::vector<double>dpzdqOverP(NTrk), dpzdtheta(NTrk), dedqOverP(NTrk);
    std::vector<double>dMdqOverP(NTrk), dMdtheta(NTrk), dMdphi(NTrk);
    std::vector<double>dTaudqOverP(NTrk), dTaudtheta(NTrk), dTaudphi(NTrk);

    Amg::MatrixX* fullCov = convertCovMatrix(vxCandidate);
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters* bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      double phi    = bPer->parameters()[Trk::phi];
      double theta  = bPer->parameters()[Trk::theta];
      double qOverP = bPer->parameters()[Trk::qOverP];
      double tmp = 1./(qOverP*qOverP) + masses[it]*masses[it];
      double pe = (tmp>0.) ? sqrt(tmp) : 0.;
      dedqOverP[it]  = -1./(qOverP*qOverP*qOverP*pe);
      dpxdqOverP[it] = -(sin(theta)*cos(phi)*trkCharge)/(qOverP*qOverP);
      dpxdtheta[it]  =  (cos(theta)*cos(phi)*trkCharge)/qOverP;
      dpxdphi[it]    = -(sin(theta)*sin(phi)*trkCharge)/qOverP;
      dpydqOverP[it] = -(sin(theta)*sin(phi)*trkCharge)/(qOverP*qOverP);
      dpydtheta[it]  =  (cos(theta)*sin(phi)*trkCharge)/qOverP;
      dpydphi[it]    =  (sin(theta)*cos(phi)*trkCharge)/qOverP;
      dpzdqOverP[it] = -(cos(theta)*trkCharge)/(qOverP*qOverP);
      dpzdtheta[it]  = -(sin(theta)*trkCharge)/qOverP;
      E  += pe;
      Px += bPer->momentum()[Trk::px];
      Py += bPer->momentum()[Trk::py];
      Pz += bPer->momentum()[Trk::pz];
    }
    double LXY = Px*dx+Py*dy;

    for( unsigned int it=0; it<NTrk; it++) {
      dMdqOverP[it]   = -(Px*dpxdqOverP[it]+Py*dpydqOverP[it]+Pz*dpzdqOverP[it]-E*dedqOverP[it])/M;
      dMdtheta[it]    = -(Px*dpxdtheta[it]+Py*dpydtheta[it]+Pz*dpzdtheta[it])/M;
      dMdphi[it]      = -(Px*dpxdphi[it]+Py*dpydphi[it])/M;
      double dPTdqOverP  =  (Px*dpxdqOverP[it]+Py*dpydqOverP[it])/PT;
      double dPTdtheta   =  (Px*dpxdtheta[it]+Py*dpydtheta[it])/PT;
      double dPTdphi     =  (Px*dpxdphi[it]+Py*dpydphi[it])/PT;
      double dLXYdqOverP =  dx*dpxdqOverP[it]+dy*dpydqOverP[it];
      double dLXYdtheta  =  dx*dpxdtheta[it]+dy*dpydtheta[it];
      double dLXYdphi    =  dx*dpxdphi[it]+dy*dpydphi[it];
      dTaudqOverP[it] =  (LXY*dMdqOverP[it]+M*dLXYdqOverP)/(PT*PT)-(2.*LXY*M*dPTdqOverP)/(PT*PT*PT);
      dTaudtheta[it]  =  (LXY*dMdtheta[it]+M*dLXYdtheta)/(PT*PT)-(2.*LXY*M*dPTdtheta)/(PT*PT*PT);
      dTaudphi[it]    =  (LXY*dMdphi[it]+M*dLXYdphi)/(PT*PT)-(2.*LXY*M*dPTdphi)/(PT*PT*PT);
    }
    double dTaudx = (M*Px)/(PT*PT);
    double dTaudy = (M*Py)/(PT*PT);
    double dTaudx0 = -dTaudx;
    double dTaudy0 = -dTaudy;

    unsigned int ndim = 0;
    if (fullCov != nullptr) {
      ndim = fullCov->rows();
    } else {
      ndim = 5*NTrk+3;
    }

    if (ndim == 5*NTrk+3 || ndim == 5*NTrk+6) {
      Amg::MatrixX D_mat(5*NTrk+6,2); D_mat.setZero();
      for( unsigned int it=0; it<NTrk; it++) {
        D_mat(5*it+0,0)  = 0.;
        D_mat(5*it+1,0)  = 0.;
        D_mat(5*it+2,0)  = CONST*dTaudphi[it];
        D_mat(5*it+3,0)  = CONST*dTaudtheta[it];
        D_mat(5*it+4,0)  = CONST*dTaudqOverP[it];
        D_mat(5*it+0,1)  = 0.;
        D_mat(5*it+1,1)  = 0.;
        D_mat(5*it+2,1)  = dMdphi[it];
        D_mat(5*it+3,1)  = dMdtheta[it];
        D_mat(5*it+4,1)  = dMdqOverP[it];
      }
      D_mat(5*NTrk+0,0) = CONST*dTaudx;
      D_mat(5*NTrk+1,0) = CONST*dTaudy;
      D_mat(5*NTrk+2,0) = 0.;
      D_mat(5*NTrk+3,0) = CONST*dTaudx0;
      D_mat(5*NTrk+4,0) = CONST*dTaudy0;
      D_mat(5*NTrk+5,0) = 0.;
      Amg::MatrixX W_mat(5*NTrk+6,5*NTrk+6); W_mat.setZero();
      if (fullCov != nullptr) {
        W_mat.block(0,0,ndim,ndim) = *fullCov;
      } else {
        Amg::MatrixX V0_cov = makeV0Cov(vxCandidate);
        W_mat.block(0,0,V0_cov.rows(),V0_cov.rows()) = V0_cov;
        W_mat.block(5*NTrk,5*NTrk,3,3) = vxCandidate->covariancePosition();
      }
      W_mat.block(5*NTrk+3,5*NTrk,3,3) = vertex->covariancePosition();
      V0_err = D_mat.transpose() * W_mat * D_mat;
    } else if (ndim == 3*NTrk+3) {
      Amg::MatrixX D_mat(3*NTrk+6,2); D_mat.setZero();
      D_mat(0,0)  = CONST*dTaudx;
      D_mat(1,0)  = CONST*dTaudy;
      D_mat(2,0)  = 0.;
      for( unsigned int it=0; it<NTrk; it++) {
        D_mat(3*it+3,0)  = CONST*dTaudphi[it];
        D_mat(3*it+4,0)  = CONST*dTaudtheta[it];
        D_mat(3*it+5,0)  = CONST*dTaudqOverP[it];
        D_mat(3*it+3,1)  = dMdphi[it];
        D_mat(3*it+4,1)  = dMdtheta[it];
        D_mat(3*it+5,1)  = dMdqOverP[it];
      }
      D_mat(3*NTrk+3,0) = CONST*dTaudx0;
      D_mat(3*NTrk+4,0) = CONST*dTaudy0;
      D_mat(3*NTrk+5,0) = 0.;
      Amg::MatrixX W_mat(3*NTrk+6,3*NTrk+6); W_mat.setZero();
      W_mat.block(0,0,ndim,ndim) = *fullCov;
      W_mat.block(3*NTrk+3,3*NTrk+3,3,3) = vertex->covariancePosition();
      V0_err = D_mat.transpose() * W_mat * D_mat;
    }
    delete fullCov;
    return V0_err;
  }

  Amg::MatrixX * V0Tools::convertCovMatrix(const xAOD::Vertex * vxCandidate) const
  {
    unsigned int NTrk = vxCandidate->nTrackParticles();
    const std::vector<float> &matrix = vxCandidate->covariance();

    int ndim = 0;

    if ( matrix.size() == (3*NTrk+3)*(3*NTrk+3+1)/2) {
      ndim = 3*NTrk+3;
    } else if (matrix.size() == (5*NTrk+3)*(5*NTrk+3+1)/2) {
      ndim = 5*NTrk+3;
    } else {
      return nullptr;
    }

    Amg::MatrixX* mtx = new Amg::MatrixX(ndim,ndim);

    long int ij=0;
    for (int i=1; i<= ndim; i++) {
      for (int j=1; j<=i; j++){
        if (i==j) {
          (*mtx)(i-1,j-1)=matrix[ij];
        } else {
          (*mtx).fillSymmetric(i-1,j-1,matrix[ij]);
        }
        ij++;
       }
    }
    // NOTE: mtx is a pointer! Take care of deleting it after you do not
    // need it anymore!!!!
    return mtx;
  }

}//end of namespace definitions
