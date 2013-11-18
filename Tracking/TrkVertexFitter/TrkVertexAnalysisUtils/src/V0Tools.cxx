/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
        V0Tools.cxx - Description in header file

    begin   : 20-03-2007
    authors : Kirill Prokofiev (Sheffield University), Evelina Bouhova-Thacker (Lancater University), Rob Henderson (Lancater University)
    email   : Kirill.Prokofiev@cern.ch, e.bouhova@cern.ch, r.henderson@lancaster.ac.uk
 
*********************************************************************/

#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "TrkParametersBase/ParametersBase.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

// forward includes
#include "TrkV0Vertex/V0Candidate.h"
#include "TrkV0Vertex/V0Hypothesis.h"
#include "VxVertex/ExtendedVxCandidate.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/Vertex.h"
#include "Particle/TrackParticle.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/AmgMatrixPlugin.h"
#include "TrkExInterfaces/IExtrapolator.h"

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
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_extrapolator << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endreq;
  }

    msg(MSG::INFO) << "Initialize successful" << endreq;
    return StatusCode::SUCCESS; 
  }
 
  StatusCode V0Tools::finalize()
  {
    msg(MSG::INFO) << "Finalize successful" << endreq;
    return StatusCode::SUCCESS; 
  }

//  const Trk::V0Hypothesis* V0Tools::findHypothesis(const V0Candidate * v0Candidate, const int pdgID) const
//  {
//    std::vector<Trk::V0Hypothesis *>::const_iterator cand_b = v0Candidate->v0Hypothesis()->begin();
//    std::vector<Trk::V0Hypothesis *>::const_iterator cand_e = v0Candidate->v0Hypothesis()->end();
//    bool found = false;
//    do {
//      if((*cand_b)->hypothesisID() == pdgID)found=true;
//      else ++cand_b;
//    } while (cand_b != cand_e && !found);
//    return (found ? (*cand_b) : 0);
//  }

  const Trk::V0Hypothesis* V0Tools::findHypothesis(const V0Candidate * v0Candidate, const int pdgID) const
  {
    bool found = false;
    const Trk::V0Hypothesis* hypo(0);
    for (std::vector<Trk::V0Hypothesis *>::const_iterator cand_b = v0Candidate->v0Hypothesis()->begin();
         cand_b != v0Candidate->v0Hypothesis()->end() ; ++cand_b) {
      if ((*cand_b)->hypothesisID() == pdgID) {
        found = true;
        hypo = (*cand_b);
        //break;
      }
    }
    return (found ? hypo : 0);
  }

  double V0Tools::invariantMass(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Invariant mass for non-existing vxCandidate requested");
      //throw std::runtime_error("non-existing vxCandidate");
      return -999999.;
    }
    std::vector<double> masses;
    masses.push_back(posTrackMass);
    masses.push_back(negTrackMass);

    return invariantMass(vxCandidate,masses);
  }

  double V0Tools::invariantMass(const ExtendedVxCandidate * vxCandidate, std::vector<double> masses) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Invariant mass for non-existing vxCandidate requested");
      //throw std::runtime_error("non-existing vxCandidate");
      return -999999.;
    }
    double px = 0., py = 0., pz = 0., e = 0.;
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    for( unsigned int it=0; it<NTrk; it++) {
      CLHEP::HepLorentzVector lorentz_trk = track4Momentum(vxCandidate,it,masses[it]);
      px += lorentz_trk.x();
      py += lorentz_trk.y();
      pz += lorentz_trk.z();
       e += lorentz_trk.e();
    }
    double msq = e*e - px*px - py*py - pz*pz;
    return (msq>0.) ? sqrt(msq) : 0.;
  }

  double V0Tools::invariantMassError(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Invariant mass error for non-existing vxCandidate requested");
      return -999999.;
    }
    std::vector<double> masses;
    masses.push_back(posTrackMass);
    masses.push_back(negTrackMass);

    return invariantMassError(vxCandidate,masses);
  }
 
  double V0Tools::invariantMassError(const ExtendedVxCandidate * vxCandidate, std::vector<double> masses) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Invariant mass error for non-existing vxCandidate requested");
      return -999999.;
    }

    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    double error = -999999.;
    if (vxCandidate->fullCovariance() != 0) {
      unsigned int ndim = (vxCandidate->fullCovariance())->rows();
      if (ndim == 5*NTrk+3 || ndim == 5*NTrk+6) {
        error = massErrorV0Fitter(vxCandidate,masses);
      } else if (ndim == 3*NTrk+3) {
        error = massErrorVKalVrt(vxCandidate,masses);
      }
    } else {
      ATH_MSG_DEBUG("0 pointer for full covariance. Making-up one from the vertex and tracks covariances");
      error = massErrorVxCandidate(vxCandidate,masses);
    }
    return error;
  }
 
  double V0Tools::massErrorV0Fitter(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    std::vector<double> masses;
    masses.push_back(posTrackMass);
    masses.push_back(negTrackMass);

    return massErrorV0Fitter(vxCandidate,masses);
  }

  double V0Tools::massErrorV0Fitter(const ExtendedVxCandidate * vxCandidate, std::vector<double> masses) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Invariant mass error for non-existing vxCandidate requested");
      return -999999.;
    }

    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    double E=0., Px=0., Py=0., Pz=0.; 
    CLHEP::HepVector d0(NTrk,0), z0(NTrk,0), phi(NTrk,0), theta(NTrk,0), qOverP(NTrk,0), charge(NTrk,0), e(NTrk,0);
    CLHEP::HepVector dm2dphi(NTrk,0), dm2dtheta(NTrk,0), dm2dqOverP(NTrk,0);
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters*  bPer = (*(vxCandidate->vxTrackAtVertex()))[it]->perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      charge[it] = trkCharge;
      d0[it]     = bPer->parameters()[Trk::d0];
      z0[it]     = bPer->parameters()[Trk::z0];
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
    double msq = E*E - Px*Px - Py*Py - Pz*Pz;
    double mass = (msq>0.) ? sqrt(msq) : 0.;

    for( unsigned int it=0; it<NTrk; it++) {
      dm2dphi[it]    = 2.*(Px*sin(phi[it])-Py*cos(phi[it]))*sin(theta[it])*charge[it]/qOverP[it];
      dm2dtheta[it]  = 2.*(Pz*sin(theta[it])-(Px*cos(phi[it])+Py*sin(phi[it]))*cos(theta[it]))*charge[it]/qOverP[it];
      dm2dqOverP[it] = 2.*(Pz*cos(theta[it])+(Px*cos(phi[it])+Py*sin(phi[it]))*sin(theta[it])-E*charge[it]/(qOverP[it]*e[it]))*charge[it]/(qOverP[it]*qOverP[it]);
    }

    Amg::MatrixX D_vec(5*NTrk,1); D_vec.setZero();
    for( unsigned int it=0; it<NTrk; it++) {
      D_vec(5*it)  = 0.;
      D_vec(5*it)  = 0.;
      D_vec(5*it)  = dm2dphi[it];
      D_vec(5*it)  = dm2dtheta[it];
      D_vec(5*it)  = dm2dqOverP[it];
    }
    Amg::MatrixX V0_merr = D_vec.transpose() * (vxCandidate->fullCovariance()->block(0,0,5,5)) * D_vec;

    double massVarsq = V0_merr(0,0);
    double massVar = (massVarsq>0.) ? sqrt(massVarsq) : 0.;
    double massErr = massVar/(2.*mass);
    return massErr;
  }
 
  double V0Tools::massErrorVKalVrt(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    std::vector<double> masses;
    masses.push_back(posTrackMass);
    masses.push_back(negTrackMass);

    return massErrorVKalVrt(vxCandidate,masses);
  }

  double V0Tools::massErrorVKalVrt(const ExtendedVxCandidate * vxCandidate, std::vector<double> masses) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Invariant mass error for non-existing vxCandidate requested");
      return -999999.;
    }

    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    std::vector<CLHEP::HepLorentzVector> particleMom(NTrk);
    std::vector<CLHEP::HepMatrix> particleDeriv(NTrk);
    CLHEP::HepLorentzVector totalMom;
    CLHEP::HepMatrix tmpDeriv(3,3);
  
    double phi=0.,theta=0.,invP=0.;
    for( unsigned int it=0; it<NTrk; it++){
      const Trk::TrackParameters*  bPer = (*(vxCandidate->vxTrackAtVertex()))[it]->perigeeAtVertex();
      phi   =  bPer->parameters()[Trk::phi];
      theta =  bPer->parameters()[Trk::theta];
      invP  =  bPer->parameters()[Trk::qOverP];
      CLHEP::HepLorentzVector tmp( cos(phi)*sin(theta)/fabs(invP),
                            sin(phi)*sin(theta)/fabs(invP),
                                     cos(theta)/fabs(invP) );
      double esq = tmp.px()*tmp.px() + tmp.py()*tmp.py() + tmp.pz()*tmp.pz() + masses[it]*masses[it];
      double e = (esq>0.) ? sqrt(esq) : 0.;
      tmp.setE(e);
      particleMom[it] = tmp;
      totalMom += tmp;

//  d(Px,Py,Pz)/d(Phi,Theta,InvP)
      tmpDeriv(1,1) = - tmp.py();
      tmpDeriv(2,1) =   tmp.px();
      tmpDeriv(3,1) =   0.;
      tmpDeriv(1,2) =   cos(phi) * tmp.pz();
      tmpDeriv(2,2) =   sin(phi) * tmp.pz();
      tmpDeriv(3,2) = - sin(theta)/fabs(invP);
      tmpDeriv(1,3) = - tmp.px()/invP;
      tmpDeriv(2,3) = - tmp.py()/invP;
      tmpDeriv(3,3) = - tmp.pz()/invP;
      particleDeriv[it] = tmpDeriv;
    }
  
    double dMdPx=0., dMdPy=0., dMdPz=0., dMdPhi=0., dMdTheta=0., dMdInvP=0.;
    std::vector<double> Deriv(3*NTrk+3, 0.);
    for(unsigned int it=0; it<NTrk; it++){
      dMdPx = ( totalMom.e() * particleMom[it].px()/particleMom[it].e() - totalMom.px() ) / totalMom.m();
      dMdPy = ( totalMom.e() * particleMom[it].py()/particleMom[it].e() - totalMom.py() ) / totalMom.m();
      dMdPz = ( totalMom.e() * particleMom[it].pz()/particleMom[it].e() - totalMom.pz() ) / totalMom.m();
  
      dMdPhi   = dMdPx*particleDeriv[it](1,1) + dMdPy*particleDeriv[it](2,1) + dMdPz*particleDeriv[it](3,1);
      dMdTheta = dMdPx*particleDeriv[it](1,2) + dMdPy*particleDeriv[it](2,2) + dMdPz*particleDeriv[it](3,2);
      dMdInvP  = dMdPx*particleDeriv[it](1,3) + dMdPy*particleDeriv[it](2,3) + dMdPz*particleDeriv[it](3,3);
  
      Deriv[3*it + 3 + 0] = dMdPhi;    Deriv[3*it + 3 + 1] = dMdTheta; Deriv[3*it + 3 + 2] = dMdInvP;
    }

    const Amg::MatrixX* fullCov = vxCandidate->fullCovariance();
    double err = 0;
    for(unsigned int i=0; i<3*NTrk+3; i++){
      for(unsigned int j=0; j<3*NTrk+3; j++){
        err += Deriv[i]*( (*fullCov)(i,j))*Deriv[j];
      }
    }
    return (err>0.) ? sqrt(err) : 0.;
  }

  double V0Tools::massErrorVxCandidate(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    std::vector<double> masses;
    masses.push_back(posTrackMass);
    masses.push_back(negTrackMass);

    return massErrorVxCandidate(vxCandidate,masses);
  }
 
  double V0Tools::massErrorVxCandidate(const ExtendedVxCandidate * vxCandidate, std::vector<double> masses) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Invariant mass error for non-existing vxCandidate requested");
      return -999999.;
    }

    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    double E=0., Px=0., Py=0., Pz=0.; 
    CLHEP::HepVector d0(NTrk,0), z0(NTrk,0), phi(NTrk,0), theta(NTrk,0), qOverP(NTrk,0), charge(NTrk,0), e(NTrk,0);
    CLHEP::HepVector dm2dphi(NTrk,0), dm2dtheta(NTrk,0), dm2dqOverP(NTrk,0);
    CLHEP::HepSymMatrix V0_cor(5*NTrk,0);
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters*  bPer = (*(vxCandidate->vxTrackAtVertex()))[it]->perigeeAtVertex();
      const AmgSymMatrix(5)* cov_tmp = bPer->covariance();
      V0_cor(5*it+3,5*it+3) = (*cov_tmp)(1,1);
      V0_cor(5*it+3,5*it+4) = (*cov_tmp)(1,2);
      V0_cor(5*it+3,5*it+5) = (*cov_tmp)(1,3);
      V0_cor(5*it+4,5*it+4) = (*cov_tmp)(2,2);
      V0_cor(5*it+4,5*it+5) = (*cov_tmp)(2,3);
      V0_cor(5*it+5,5*it+5) = (*cov_tmp)(3,3);
      double trkCharge = 1.;
      if (bPer->parameters()(Trk::qOverP) < 0.) trkCharge = -1.;
      charge[it] = trkCharge;
      d0[it]     = bPer->parameters()(Trk::d0);
      z0[it]     = bPer->parameters()(Trk::z0);
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
    double msq = E*E - Px*Px - Py*Py - Pz*Pz;
    double mass = (msq>0.) ? sqrt(msq) : 0.;

    for( unsigned int it=0; it<NTrk; it++) {
      dm2dphi[it]    = 2.*(Px*sin(phi[it])-Py*cos(phi[it]))*sin(theta[it])*charge[it]/qOverP[it];
      dm2dtheta[it]  = 2.*(Pz*sin(theta[it])-(Px*cos(phi[it])+Py*sin(phi[it]))*cos(theta[it]))*charge[it]/qOverP[it];
      dm2dqOverP[it] = 2.*(Pz*cos(theta[it])+(Px*cos(phi[it])+Py*sin(phi[it]))*sin(theta[it])-E*charge[it]/(qOverP[it]*e[it]))*charge[it]/(qOverP[it]*qOverP[it]);
    }

    CLHEP::HepVector D_vec(5*NTrk,0);
    for( unsigned int it=0; it<NTrk; it++) {
      D_vec(5*it+1) = 0.;
      D_vec(5*it+2) = 0.;
      D_vec(5*it+3) = dm2dphi[it];
      D_vec(5*it+4) = dm2dtheta[it];
      D_vec(5*it+5) = dm2dqOverP[it];
    }
    CLHEP::HepMatrix V0_merr = D_vec.T() * V0_cor * D_vec;

    double massVarsq = V0_merr(1,1);
    double massVar = (massVarsq>0.) ? sqrt(massVarsq) : 0.;
    double massErr = massVar/(2.*mass);
    return massErr;
  }

  double V0Tools::invariantMassProbability(const ExtendedVxCandidate * vxCandidate, double V0Mass, double posTrackMass, double negTrackMass) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Invariant mass probability for non-existing vxCandidate requested");
      return -1.;
    }

    std::vector<double> masses;
    masses.push_back(posTrackMass);
    masses.push_back(negTrackMass);

    return invariantMassProbability(vxCandidate,V0Mass,masses);
  }
 
  double V0Tools::invariantMassProbability(const ExtendedVxCandidate * vxCandidate, double V0Mass, std::vector<double> masses) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Invariant mass probability for non-existing vxCandidate requested");
      return -1.;
    }
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
      //ATH_MSG_DEBUG("massErr <= 0");
      //throw std::runtime_error("massErr <= 0");
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
 
  Amg::Vector3D V0Tools::trackMomentum(const ExtendedVxCandidate * vxCandidate, int trkIndex) const
  {
    double px = 0., py = 0., pz = 0.;
    assert(vxCandidate!=0);
    //if(0 == vxCandidate) { ATH_MSG_DEBUG("Non-existing vxCandidate"); }
    if (0 != vxCandidate) {
      const Trk::TrackParameters* aPerigee = (*(vxCandidate->vxTrackAtVertex()))[trkIndex]->perigeeAtVertex();
      px = aPerigee->momentum()[Trk::px];
      py = aPerigee->momentum()[Trk::py];
      pz = aPerigee->momentum()[Trk::pz];
    }
    Amg::Vector3D mom(px,py,pz);
    return mom;
  }

  Amg::Vector3D V0Tools::positiveTrackMomentum(const ExtendedVxCandidate * vxCandidate) const
  {
    Amg::Vector3D mom;
    assert(vxCandidate!=0);
    //if(0 == vxCandidate) { ATH_MSG_DEBUG("Non-existing vxCandidate"); }
    mom = trackMomentum(vxCandidate,0);
    return mom;
  }

  Amg::Vector3D V0Tools::negativeTrackMomentum(const ExtendedVxCandidate * vxCandidate) const
  {
    Amg::Vector3D mom;
    assert(vxCandidate!=0);
    //if(0 == vxCandidate) { ATH_MSG_DEBUG("Non-existing vxCandidate"); }
    mom = trackMomentum(vxCandidate,1);
    return mom;
  }

  Amg::Vector3D V0Tools::V0Momentum(const ExtendedVxCandidate * vxCandidate) const
  {
    assert(vxCandidate!=0);
    //if(0 == vxCandidate) { ATH_MSG_DEBUG("Non-existing vxCandidate"); }
    double px = 0., py = 0., pz = 0.;
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    for( unsigned int it=0; it<NTrk; it++) {
      Amg::Vector3D mom_trk = trackMomentum(vxCandidate,it);
      px += mom_trk.x();
      py += mom_trk.y();
      pz += mom_trk.z();
    }
    Amg::Vector3D mom(px,py,pz);
    return mom;
  }

  CLHEP::HepLorentzVector V0Tools::track4Momentum(const ExtendedVxCandidate * vxCandidate, int trkIndex, double mass) const
  {
    assert(vxCandidate!=0);
    //if(0 == vxCandidate) { ATH_MSG_DEBUG("Non-existing vxCandidate"); }
    Amg::Vector3D mom = trackMomentum(vxCandidate, trkIndex);
    double tmp = mass*mass + mom.x()*mom.x() + mom.y()*mom.y() + mom.z()*mom.z();
    double e = (tmp>0.) ? sqrt(tmp) : 0.;
    CLHEP::HepLorentzVector lorentz(mom.x(), mom.y(), mom.z(), e);
    return lorentz;
  }

  CLHEP::HepLorentzVector V0Tools::positiveTrack4Momentum(const ExtendedVxCandidate * vxCandidate, double mass) const
  {
    assert(vxCandidate!=0);
    //if(0 == vxCandidate) { ATH_MSG_DEBUG("Non-existing vxCandidate"); }
    Amg::Vector3D mom = positiveTrackMomentum(vxCandidate);
    double tmp = mass*mass + mom.x()*mom.x() + mom.y()*mom.y() + mom.z()*mom.z();
    double e = (tmp>0.) ? sqrt(tmp) : 0.;
    CLHEP::HepLorentzVector lorentz(mom.x(), mom.y(), mom.z(), e);
    return lorentz;
  }

  CLHEP::HepLorentzVector V0Tools::negativeTrack4Momentum(const ExtendedVxCandidate * vxCandidate, double mass) const
  {
    assert(vxCandidate!=0);
    //if(0 == vxCandidate) { ATH_MSG_DEBUG("Non-existing vxCandidate"); }
    Amg::Vector3D mom = negativeTrackMomentum(vxCandidate);
    double tmp = mass*mass + mom.x()*mom.x() + mom.y()*mom.y() + mom.z()*mom.z();
    double e = (tmp>0.) ? sqrt(tmp) : 0.;
    CLHEP::HepLorentzVector lorentz(mom.x(), mom.y(), mom.z(), e);
    return lorentz;
  }

  CLHEP::HepLorentzVector V0Tools::V04Momentum(const ExtendedVxCandidate * vxCandidate, double V0Mass) const
  {
    assert(vxCandidate!=0);
    //if(0 == vxCandidate) { ATH_MSG_DEBUG("Non-existing vxCandidate"); }
    Amg::Vector3D mom = V0Momentum(vxCandidate);
    double tmp = V0Mass*V0Mass + mom.x()*mom.x() + mom.y()*mom.y() + mom.z()*mom.z();
    double e = (tmp>0.) ? sqrt(tmp) : 0.;
    CLHEP::HepLorentzVector lorentz(mom.x(), mom.y(), mom.z(), e);
    return lorentz;
  }

  int V0Tools::ndof(const ExtendedVxCandidate * vxCandidate) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("NDoF for non-existing vxCandidate requested");
      return -1;
    }
    return vxCandidate->recVertex().fitQuality().numberDoF();
  }

  double V0Tools::chisq(const ExtendedVxCandidate * vxCandidate) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Chi2 for non-existing vxCandidate requested");
      return -1.;
    }
    return vxCandidate->recVertex().fitQuality().chiSquared();
  }

  double V0Tools::vertexProbability(const ExtendedVxCandidate * vxCandidate) const
  {
    assert(vxCandidate!=0);
    //if(0 == vxCandidate) { ATH_MSG_DEBUG("Non-existing vxCandidate"); }
    int dof = ndof(vxCandidate);
    if (dof > 0.) {
      Genfun::CumulativeChiSquare myCumulativeChiSquare(dof);
      double chi = chisq(vxCandidate);
      if (chi > 0.) {
        double chi2prob = 1.-myCumulativeChiSquare(chi);
        return chi2prob;
      } else {
        ATH_MSG_DEBUG("chi <= 0");
        return -1.;
      }
    } else {
      ATH_MSG_DEBUG("dof <= 0");
      //throw std::runtime_error("dof <= 0");
      return -1.;
    }

  }

  Amg::Vector3D V0Tools::vtx(const ExtendedVxCandidate * vxCandidate) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Vertex position for non-existing vxCandidate requested");
      Amg::Vector3D vertex; vertex.setZero();
      return vertex;
    }
    Amg::Vector3D vertex(vxCandidate->recVertex().position()[0],vxCandidate->recVertex().position()[1],vxCandidate->recVertex().position()[2]);
    return vertex;
  }

  double V0Tools::rxy(const ExtendedVxCandidate * vxCandidate) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Rxy for non-existing vxCandidate requested");
      return -999999.;
    }
    return vxCandidate->recVertex().position().perp();
  }

  double V0Tools::rxy(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Rxy for non-existing vxCandidate requested");
      return -999999.;
    }
    return (vxCandidate->recVertex().position() - vertex.position()).perp();
  }

  double V0Tools::rxy(const ExtendedVxCandidate * vxCandidate, Trk::Vertex& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Rxy for non-existing vxCandidate requested");
      return -999999.;
    }
    return (vxCandidate->recVertex().position() - vertex.position()).perp();
  }

  double V0Tools::rxy(const ExtendedVxCandidate * vxCandidate, Amg::Vector3D& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Rxy for non-existing vxCandidate requested");
      return -999999.;
    }
    return (vxCandidate->recVertex().position() - vertex).perp();
  }

  double V0Tools::rxy_var(double dx, double dy, const Amg::MatrixX& cov) const
  {
    double rxysq = dx*dx + dy*dy;
    double rxy = (rxysq>0.) ? sqrt(rxysq) : 0.;
    double drdx = dx/rxy;
    double drdy = dy/rxy;
    CLHEP::HepVector D_vec(2,0);
    D_vec(1)  = drdx;
    D_vec(2)  = drdy;
    CLHEP::HepSymMatrix rxy_cov(2,0);
    rxy_cov(1,1) = cov(0,0);
    rxy_cov(1,2) = cov(0,1);
    rxy_cov(2,2) = cov(1,1);
    CLHEP::HepMatrix rxy_err = D_vec.T() * rxy_cov * D_vec;
    double rxyVar = rxy_err(1,1);
    return rxyVar;
  }

  double V0Tools::rxyError(const ExtendedVxCandidate * vxCandidate) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Rxy error for non-existing vxCandidate requested");
      return -999999.;
    }
    const Amg::MatrixX& cov = vxCandidate->recVertex().covariancePosition();
    double dx = vxCandidate->recVertex().position().x();
    double dy = vxCandidate->recVertex().position().y();
    double rxyVar = rxy_var(dx,dy,cov);
    return (rxyVar>0.) ? sqrt(rxyVar) : 0.;
  }

  double V0Tools::rxyError(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Rxy error for non-existing vxCandidate requested");
      return -999999.;
    }
    const Amg::MatrixX& cov = vxCandidate->recVertex().covariancePosition() + vertex.covariancePosition();
    double dx = (vxCandidate->recVertex().position() - vertex.position()).x();
    double dy = (vxCandidate->recVertex().position() - vertex.position()).y();
    double rxyVar = rxy_var(dx,dy,cov);
    return (rxyVar>0.) ? sqrt(rxyVar) : 0.;
  }

  double V0Tools::rxyError(const ExtendedVxCandidate * vxCandidate, Trk::Vertex& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Rxy error for non-existing vxCandidate requested");
      return -999999.;
    }
    const Amg::MatrixX cov = vxCandidate->recVertex().covariancePosition();
    double dx = (vxCandidate->recVertex().position() - vertex.position()).x();
    double dy = (vxCandidate->recVertex().position() - vertex.position()).y();
    double rxyVar = rxy_var(dx,dy,cov);
    return (rxyVar>0.) ? sqrt(rxyVar) : 0.;
  }

  double V0Tools::rxyError(const ExtendedVxCandidate * vxCandidate, Amg::Vector3D& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Rxy error for non-existing vxCandidate requested");
      return -999999.;
    }
    const Amg::MatrixX& cov = vxCandidate->recVertex().covariancePosition();
    double dx = (vxCandidate->recVertex().position() - vertex).x();
    double dy = (vxCandidate->recVertex().position() - vertex).y();
    double rxyVar = rxy_var(dx,dy,cov);
    return (rxyVar>0.) ? sqrt(rxyVar) : 0.;
  }

  double V0Tools::pT(const ExtendedVxCandidate * vxCandidate) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    return V0Momentum(vxCandidate).perp();
  }

  double V0Tools::pTError(const ExtendedVxCandidate * vxCandidate) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    Amg::MatrixX V0_cov(5*NTrk,5*NTrk); V0_cov.setZero();
    double Px=0., Py=0.; 
    CLHEP::HepVector phi(NTrk,0), theta(NTrk,0), qOverP(NTrk,0), charge(NTrk,0);
    CLHEP::HepVector dpxdqOverP(NTrk,0), dpxdtheta(NTrk,0), dpxdphi(NTrk,0);
    CLHEP::HepVector dpydqOverP(NTrk,0), dpydtheta(NTrk,0), dpydphi(NTrk,0);
    CLHEP::HepVector dPTdqOverP(NTrk,0), dPTdtheta(NTrk,0), dPTdphi(NTrk,0);

    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters*  bPer = (*(vxCandidate->vxTrackAtVertex()))[it]->perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      charge[it] = trkCharge;
      phi[it]    = bPer->parameters()[Trk::phi];
      theta[it]  = bPer->parameters()[Trk::theta];
      qOverP[it] = bPer->parameters()[Trk::qOverP];
      dpxdqOverP[it] = -(sin(theta[it])*cos(phi[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpxdtheta[it]  =  (cos(theta[it])*cos(phi[it])*charge[it])/qOverP[it];
      dpxdphi[it]    = -(sin(theta[it])*sin(phi[it])*charge[it])/qOverP[it];
      dpydqOverP[it] = -(sin(theta[it])*sin(phi[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpydtheta[it]  =  (cos(theta[it])*sin(phi[it])*charge[it])/qOverP[it];
      dpydphi[it]    =  (sin(theta[it])*cos(phi[it])*charge[it])/qOverP[it];
      Px += bPer->momentum()[Trk::px];
      Py += bPer->momentum()[Trk::py];
      if (vxCandidate->fullCovariance() == 0) V0_cov.block(5*it,5*it,5,5) = *(bPer->covariance());
    }
    double PTsq = Px*Px+Py*Py;
    double PT = (PTsq>0.) ? sqrt(PTsq) : 0.;

    for( unsigned int it=0; it<NTrk; it++) {
      dPTdqOverP[it]  = (Px*dpxdqOverP[it]+Py*dpydqOverP[it])/PT;
      dPTdtheta[it]   = (Px*dpxdtheta[it]+Py*dpydtheta[it])/PT;
      dPTdphi[it]     = (Px*dpxdphi[it]+Py*dpydphi[it])/PT;
    }

    unsigned int ndim = 0;
    if (vxCandidate->fullCovariance() != 0) {
      ndim = (vxCandidate->fullCovariance())->rows();
    } else {
      ndim = 5*NTrk+3;
    }

    Amg::MatrixX V0_err;
    if (ndim == 5*NTrk+3 || ndim == 5*NTrk+6) {
      Amg::VectorX D_vec(5*NTrk); D_vec.setZero();
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(5*it)  = 0.;
        D_vec(5*it+1)  = 0.;
        D_vec(5*it+2)  = dPTdphi[it];
        D_vec(5*it+3)  = dPTdtheta[it];
        D_vec(5*it+4)  = dPTdqOverP[it];
      }
      if (vxCandidate->fullCovariance() != 0) {
        V0_err = D_vec.transpose() * (vxCandidate->fullCovariance()->block(0,0,5,5)) * D_vec;
      } else {
        V0_err = D_vec.transpose() * V0_cov * D_vec;
      }
    } else if (ndim == 3*NTrk+3) {
      Amg::VectorX D_vec(3*NTrk); D_vec.setZero();
      for( unsigned int it=0; it<NTrk; it++) {
        D_vec(3*it)  = dPTdphi[it];
        D_vec(3*it+1)  = dPTdtheta[it];
        D_vec(3*it+2)  = dPTdqOverP[it];
      }
      V0_err = D_vec.transpose() * (vxCandidate->fullCovariance()->block(2,2,3,3)) * D_vec;
    }

    double PtErrsq = V0_err(1,1);
    return (PtErrsq>0.) ? sqrt(PtErrsq) : 0.;
  }

  Amg::Vector3D V0Tools::pca(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      Amg::Vector3D p; p.setZero();
      return p;
    }
    Amg::Vector3D pv = vertex.position();
    Amg::Vector3D sv = vtx(vxCandidate);
    Amg::Vector3D P = V0Momentum(vxCandidate);
    double p2 = P.mag2();
    double pdr = P.dot((sv - pv));
    return sv - P*pdr/p2;
  }

  double V0Tools::separation(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    const Amg::MatrixX& cov = vxCandidate->recVertex().covariancePosition()+vertex.covariancePosition();
    cov.inverse();  
    Amg::MatrixX dr(1,3);
    dr(0,0) = (vxCandidate->recVertex().position() - vertex.position()).x();
    dr(0,1) = (vxCandidate->recVertex().position() - vertex.position()).y();
    dr(0,2) = (vxCandidate->recVertex().position() - vertex.position()).z();
    Amg::MatrixX sep = cov.similarity( dr );
    return (sep(0,0)>0.) ? sqrt(sep(0,0)) : 0.;
    Amg::Vector3D D_vec; D_vec.setZero();
    D_vec(0) = (vxCandidate->recVertex().position() - vertex.position()).x();
    D_vec(1) = (vxCandidate->recVertex().position() - vertex.position()).y();
    D_vec(2) = (vxCandidate->recVertex().position() - vertex.position()).z();
    Amg::MatrixX sepVarsqMat = cov.similarity( D_vec ); 
    double sepVarsq = sepVarsqMat(0,0); 
    if (sepVarsq <= 0.) ATH_MSG_INFO("separation: negative sqrt sepVarsq " << sepVarsq);
    double sepVar = (sepVarsq>0.) ? sqrt(sepVarsq) : 0.;
    return sepVar;
  }

  double V0Tools::separation(const ExtendedVxCandidate * vxCandidate, Amg::Vector3D& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    const Amg::MatrixX& cov = vxCandidate->recVertex().covariancePosition();
    //int fail=0;
    cov.inverse();
    //if (fail!=0) {
    //  ATH_MSG_DEBUG("Failed to invert covariance matrix!");
    //  return -999999.;
    //}
    Amg::MatrixX dr(1,3);
    dr(0,0) = (vxCandidate->recVertex().position() - vertex).x();
    dr(0,1) = (vxCandidate->recVertex().position() - vertex).y();
    dr(0,2) = (vxCandidate->recVertex().position() - vertex).z();
    Amg::MatrixX sep = cov.similarity( dr );
    return (sep(0,0)>0.) ? sqrt(sep(0,0)) : 0.;
    Amg::Vector3D D_vec; D_vec.setZero();
    D_vec(0) = (vxCandidate->recVertex().position() - vertex).x();
    D_vec(1) = (vxCandidate->recVertex().position() - vertex).y();
    D_vec(2) = (vxCandidate->recVertex().position() - vertex).z();
    Amg::MatrixX sepVarsqM = cov.similarity( D_vec ); 
    double sepVarsq = sepVarsqM(0,0);
    if (sepVarsq <= 0.) ATH_MSG_INFO("separation: negative sqrt sepVarsq " << sepVarsq);
    double sepVar = (sepVarsq>0.) ? sqrt(sepVarsq) : 0.;
    return sepVar;
  }

  double V0Tools::a0xy(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    double cosineTheta_xy = cosTheta_xy(vxCandidate,vertex);
    double sinTheta_xy = ((1.-cosineTheta_xy*cosineTheta_xy)>0.) ? sqrt((1.-cosineTheta_xy*cosineTheta_xy)) : 0.;
    return (vtx(vxCandidate)-vertex.position()).perp() * sinTheta_xy;
    //return (vtx(vxCandidate)-vertex.position()).perp() * sqrt(1.-cosineTheta_xy*cosineTheta_xy);
  }

  double V0Tools::a0z(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    Amg::Vector3D pv = vertex.position();
    Amg::Vector3D ca_point = pca(vxCandidate,vertex);
    Amg::Vector3D a0_vec = pv - ca_point;
    return a0_vec.z();
  }

  double V0Tools::a0(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    double cosineTheta = cosTheta(vxCandidate,vertex);
    double sinTheta = ((1.-cosineTheta*cosineTheta)>0.) ? sqrt((1.-cosineTheta*cosineTheta)) : 0.;
    return (vtx(vxCandidate)-vertex.position()).mag() * sinTheta;
    //return (vtx(vxCandidate)-vertex.position()).mag() * sqrt(1.-cosineTheta*cosineTheta);
  }

  double V0Tools::a0zError(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    double dx = (vxCandidate->recVertex().position() - vertex.position()).x();
    double dy = (vxCandidate->recVertex().position() - vertex.position()).y();
    double dz = (vxCandidate->recVertex().position() - vertex.position()).z();
    Amg::MatrixX V0_cov(5*NTrk,1); V0_cov.setZero();
    double Px=0., Py=0., Pz=0.;
    CLHEP::HepVector phi(NTrk,0), theta(NTrk,0), qOverP(NTrk,0), charge(NTrk,0), e(NTrk,0);
    CLHEP::HepVector dpxdqOverP(NTrk,0), dpxdtheta(NTrk,0), dpxdphi(NTrk,0);
    CLHEP::HepVector dpydqOverP(NTrk,0), dpydtheta(NTrk,0), dpydphi(NTrk,0);
    CLHEP::HepVector dpzdqOverP(NTrk,0), dpzdtheta(NTrk,0);
    CLHEP::HepVector dP2dqOverP(NTrk,0), dP2dtheta(NTrk,0), dP2dphi(NTrk,0);
    CLHEP::HepVector da0dqOverP(NTrk,0), da0dtheta(NTrk,0), da0dphi(NTrk,0);

    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters*  bPer = (*(vxCandidate->vxTrackAtVertex()))[it]->perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      charge[it] = trkCharge;
      phi[it]    = bPer->parameters()[Trk::phi];
      theta[it]  = bPer->parameters()[Trk::theta];
      qOverP[it] = bPer->parameters()[Trk::qOverP];
      dpxdqOverP[it] = -(sin(theta[it])*cos(phi[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpxdtheta[it]  =  (cos(theta[it])*cos(phi[it])*charge[it])/qOverP[it];
      dpxdphi[it]    = -(sin(theta[it])*sin(phi[it])*charge[it])/qOverP[it];
      dpydqOverP[it] = -(sin(theta[it])*sin(phi[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpydtheta[it]  =  (cos(theta[it])*sin(phi[it])*charge[it])/qOverP[it];
      dpydphi[it]    =  (sin(theta[it])*cos(phi[it])*charge[it])/qOverP[it];
      dpzdqOverP[it] = -(cos(theta[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpzdtheta[it]  = -(sin(theta[it])*charge[it])/qOverP[it];
      Px += bPer->momentum()[Trk::px];
      Py += bPer->momentum()[Trk::py];
      Pz += bPer->momentum()[Trk::pz];
      if (vxCandidate->fullCovariance() == 0) V0_cov.block(5*it,5*it,5,5) = *(bPer->covariance());
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
      dP2dqOverP[it] = 2.*(Px*dpxdqOverP[it]+Py*dpydqOverP[it]+Pz*dpzdqOverP[it]);
      dP2dtheta[it]  = 2.*(Px*dpxdtheta[it]+Py*dpydtheta[it]+Pz*dpzdtheta[it]);
      dP2dphi[it]    = 2.*(Px*dpxdphi[it]+Py*dpydphi[it]);
      da0dqOverP[it] =  (B*(P2*dpzdqOverP[it]-Pz*dP2dqOverP[it]) +
                         Pz*P2*(dx*dpxdqOverP[it]+dy*dpydqOverP[it]+dz*dpzdqOverP[it]))/(P2*P2);
      da0dtheta[it]  =  (B*(P2*dpzdtheta[it]-Pz*dP2dtheta[it]) +
                         Pz*P2*(dx*dpxdtheta[it]+dy*dpydtheta[it]+dz*dpzdtheta[it]))/(P2*P2);
      da0dphi[it]    = -(B*Pz*dP2dphi[it] -
                         Pz*P2*(dx*dpxdphi[it]+dy*dpydphi[it]))/(P2*P2);
    }

    unsigned int ndim = 0;
    if (vxCandidate->fullCovariance() != 0) {
      ndim = (vxCandidate->fullCovariance())->rows();
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

      Amg::MatrixX W_mat(5*NTrk+6,1); W_mat.setZero();
      if (vxCandidate->fullCovariance() != 0) {
        unsigned int nDimTmp = vxCandidate->fullCovariance()->rows();
	W_mat.block(0,0,nDimTmp,nDimTmp) = *(vxCandidate->fullCovariance());  
      } else {
	unsigned int nDimTmp = V0_cov.rows();
        W_mat.block(0,0,nDimTmp,nDimTmp) =  V0_cov;
        W_mat.block(5*NTrk,5*NTrk,3,3) = vxCandidate->recVertex().covariancePosition();
      }
      W_mat.block(5*NTrk+3,5*NTrk+3,3,3) =  vertex.covariancePosition();
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
      W_mat.block(0,0,ndim,ndim) = *(vxCandidate->fullCovariance());
      W_mat.block(3*NTrk+3,3*NTrk+3,3,3) =  vertex.covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    }

    double a0Errsq = V0_err(0,0);
    double a0Err = (a0Errsq>0.) ? sqrt(a0Errsq) : 0.;
    return a0Err;
  }

  double V0Tools::a0xyError(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const
  {
    return a0Error(vxCandidate, vertex, false);
  }

  double V0Tools::a0Error(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, bool in3D) const
  {

    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    double dx = (vxCandidate->recVertex().position() - vertex.position()).x();
    double dy = (vxCandidate->recVertex().position() - vertex.position()).y();
    double dz = (vxCandidate->recVertex().position() - vertex.position()).z();
    Amg::MatrixX V0_cov(5*NTrk,5*NTrk); V0_cov.setZero();
    double Px=0., Py=0., Pz=0.;
    CLHEP::HepVector phi(NTrk,0), theta(NTrk,0), qOverP(NTrk,0), charge(NTrk,0), e(NTrk,0);
    CLHEP::HepVector dpxdqOverP(NTrk,0), dpxdtheta(NTrk,0), dpxdphi(NTrk,0);
    CLHEP::HepVector dpydqOverP(NTrk,0), dpydtheta(NTrk,0), dpydphi(NTrk,0);
    CLHEP::HepVector dpzdqOverP(NTrk,0), dpzdtheta(NTrk,0);
    CLHEP::HepVector da0dqOverP(NTrk,0), da0dtheta(NTrk,0), da0dphi(NTrk,0);

    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters*  bPer = (*(vxCandidate->vxTrackAtVertex()))[it]->perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      charge[it] = trkCharge;
      phi[it]    = bPer->parameters()[Trk::phi];
      theta[it]  = bPer->parameters()[Trk::theta];
      qOverP[it] = bPer->parameters()[Trk::qOverP];
      dpxdqOverP[it] = -(sin(theta[it])*cos(phi[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpxdtheta[it]  =  (cos(theta[it])*cos(phi[it])*charge[it])/qOverP[it];
      dpxdphi[it]    = -(sin(theta[it])*sin(phi[it])*charge[it])/qOverP[it];
      dpydqOverP[it] = -(sin(theta[it])*sin(phi[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpydtheta[it]  =  (cos(theta[it])*sin(phi[it])*charge[it])/qOverP[it];
      dpydphi[it]    =  (sin(theta[it])*cos(phi[it])*charge[it])/qOverP[it];
      if ( in3D ) {
        dpzdqOverP[it] = -(cos(theta[it])*charge[it])/(qOverP[it]*qOverP[it]);
        dpzdtheta[it]  = -(sin(theta[it])*charge[it])/qOverP[it];
      }
      Px += bPer->momentum()[Trk::px];
      Py += bPer->momentum()[Trk::py];
      Pz += bPer->momentum()[Trk::pz];
      if (vxCandidate->fullCovariance() == 0) V0_cov.block(5*it,5*it,5*it+4,5*it+4) = *(bPer->covariance());
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
    if (vxCandidate->fullCovariance() != 0) {
      ndim = (vxCandidate->fullCovariance())->rows();
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

      Amg::MatrixX W_mat(5*NTrk+6,1); W_mat.setZero();
      if (vxCandidate->fullCovariance() != 0) {
        W_mat.block(0,0,ndim,ndim) =  *(vxCandidate->fullCovariance());
      } else {
        W_mat.block(0,0,V0_cov.rows(),V0_cov.rows()) = V0_cov;
        W_mat.block(5*NTrk,5*NTrk,3,3) = vxCandidate->recVertex().covariancePosition();
      }
      W_mat.block(5*NTrk+3,5*NTrk+3,3,3) = vertex.covariancePosition();
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

      Amg::MatrixX W_mat(3*NTrk+6,1); W_mat.setZero();
      W_mat.block(0,0,ndim,ndim) =  *(vxCandidate->fullCovariance());
      W_mat.block(3*NTrk+3,3*NTrk+3,3,3) =  vertex.covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    }

    double a0Errsq = V0_err(0,0);
    double a0Err = (a0Errsq>0.) ? sqrt(a0Errsq) : 0.;
    return a0Err;
  }

  double V0Tools::lxy(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    double dx = (vxCandidate->recVertex().position() - vertex.position()).x();
    double dy = (vxCandidate->recVertex().position() - vertex.position()).y();
    double dxy = (V0Momentum(vxCandidate).x()*dx + V0Momentum(vxCandidate).y()*dy)/V0Momentum(vxCandidate).perp();
    return dxy;
  }

  double V0Tools::lxyError(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    double dx = (vxCandidate->recVertex().position() - vertex.position()).x();
    double dy = (vxCandidate->recVertex().position() - vertex.position()).y();
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    Amg::MatrixX V0_cov(5*NTrk,5*NTrk); V0_cov.setZero();
    double Px=0., Py=0.; 
    CLHEP::HepVector phi(NTrk,0), theta(NTrk,0), qOverP(NTrk,0), charge(NTrk,0);
    CLHEP::HepVector dpxdqOverP(NTrk,0), dpxdtheta(NTrk,0), dpxdphi(NTrk,0);
    CLHEP::HepVector dpydqOverP(NTrk,0), dpydtheta(NTrk,0), dpydphi(NTrk,0);
    CLHEP::HepVector dPTdqOverP(NTrk,0), dPTdtheta(NTrk,0), dPTdphi(NTrk,0);
    CLHEP::HepVector dLxydqOverP(NTrk,0), dLxydtheta(NTrk,0), dLxydphi(NTrk,0);

    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters*  bPer = (*(vxCandidate->vxTrackAtVertex()))[it]->perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      charge[it] = trkCharge;
      phi[it]    = bPer->parameters()[Trk::phi];
      theta[it]  = bPer->parameters()[Trk::theta];
      qOverP[it] = bPer->parameters()[Trk::qOverP];
      dpxdqOverP[it] = -(sin(theta[it])*cos(phi[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpxdtheta[it]  =  (cos(theta[it])*cos(phi[it])*charge[it])/qOverP[it];
      dpxdphi[it]    = -(sin(theta[it])*sin(phi[it])*charge[it])/qOverP[it];
      dpydqOverP[it] = -(sin(theta[it])*sin(phi[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpydtheta[it]  =  (cos(theta[it])*sin(phi[it])*charge[it])/qOverP[it];
      dpydphi[it]    =  (sin(theta[it])*cos(phi[it])*charge[it])/qOverP[it];
      Px += bPer->momentum()[Trk::px];
      Py += bPer->momentum()[Trk::py];
      if (vxCandidate->fullCovariance() == 0) V0_cov.block(5*it,5*it,5,5) = *(bPer->covariance());
    }
    double PTsq = Px*Px+Py*Py;
    double PT = (PTsq>0.) ? sqrt(PTsq) : 0.;
    double LXYoverPT = (Px*dx+Py*dy)/(PT*PT);

    for( unsigned int it=0; it<NTrk; it++) {
      dPTdqOverP[it]  = (Px*dpxdqOverP[it]+Py*dpydqOverP[it])/PT;
      dPTdtheta[it]   = (Px*dpxdtheta[it]+Py*dpydtheta[it])/PT;
      dPTdphi[it]     = (Px*dpxdphi[it]+Py*dpydphi[it])/PT;
      dLxydqOverP[it] = (dx*dpxdqOverP[it]+dy*dpydqOverP[it])/PT-LXYoverPT*dPTdqOverP[it];
      dLxydtheta[it]  = (dx*dpxdtheta[it]+dy*dpydtheta[it])/PT-LXYoverPT*dPTdtheta[it];
      dLxydphi[it]    = (dx*dpxdphi[it]+dy*dpydphi[it])/PT-LXYoverPT*dPTdphi[it];
    }
    double dLxydx = Px/PT;
    double dLxydy = Py/PT;
    double dLxydx0 = -dLxydx;
    double dLxydy0 = -dLxydy;

    unsigned int ndim = 0;
    if (vxCandidate->fullCovariance() != 0) {
      ndim = (vxCandidate->fullCovariance())->rows();
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

      Amg::MatrixX W_mat(5*NTrk+6,1); W_mat.setZero();
      if (vxCandidate->fullCovariance() != 0) {
        W_mat.block(0,0,ndim,ndim) = *(vxCandidate->fullCovariance());
      } else {
        W_mat.block(0,0,V0_cov.rows(),V0_cov.rows()) = V0_cov;
        W_mat.block(5*NTrk,5*NTrk,3,3) = vxCandidate->recVertex().covariancePosition();
      }
      W_mat.block(5*NTrk+3,5*NTrk+3,3,3) = vertex.covariancePosition();
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

      Amg::MatrixX W_mat(3*NTrk+6,1); W_mat.setZero();
      W_mat.block(0,0,ndim,ndim) = *(vxCandidate->fullCovariance());
      W_mat.block(3*NTrk+3,3*NTrk+3,3,3) = vertex.covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    }

    double LxyErrsq = V0_err(0,0);
    return (LxyErrsq>0.) ? sqrt(LxyErrsq) : 0.;
  }

  double V0Tools::tau(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, double posTrackMass, double negTrackMass) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    std::vector<double> masses;
    masses.push_back(posTrackMass);
    masses.push_back(negTrackMass);

    return tau(vxCandidate,vertex,masses);
  }

  double V0Tools::tau(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, std::vector<double> masses) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    double CONST = 1000./CLHEP::c_light;
    double M = invariantMass(vxCandidate, masses);
    double LXY = lxy(vxCandidate,vertex);
    double PT = V0Momentum(vxCandidate).perp();
    return CONST*M*LXY/PT;
  }

  double V0Tools::tau(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, double posTrackMass, double negTrackMass, double massV0) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    std::vector<double> masses;
    masses.push_back(posTrackMass);
    masses.push_back(negTrackMass);

    return tau(vxCandidate,vertex,masses,massV0);
  }

  double V0Tools::tau(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, std::vector<double> masses, double massV0) const
  {
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
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

  double V0Tools::tau(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, double M) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    double CONST = 1000./CLHEP::c_light;
    double LXY = lxy(vxCandidate,vertex);
    double PT = V0Momentum(vxCandidate).perp();
    return CONST*M*LXY/PT;
  }

  double V0Tools::tauError(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, double posTrackMass, double negTrackMass) const
  {
    // Tau = CONST*M*(Px*dx+Py*dy)/(PT*PT)
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    std::vector<double> masses;
    masses.push_back(posTrackMass);
    masses.push_back(negTrackMass);

    return tauError(vxCandidate,vertex,masses);
  }

  double V0Tools::tauError(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, std::vector<double> masses) const
  {
    // Tau = CONST*M*(Px*dx+Py*dy)/(PT*PT)
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    double CONST = 1000./CLHEP::c_light;
    double PT = V0Momentum(vxCandidate).perp();
    double dx = (vxCandidate->recVertex().position() - vertex.position()).x();
    double dy = (vxCandidate->recVertex().position() - vertex.position()).y();
    double M = invariantMass(vxCandidate, masses);
    Amg::MatrixX V0_cov(5*NTrk,1); V0_cov.setZero();
    double E=0., Px=0., Py=0., Pz=0.; 
    CLHEP::HepVector phi(NTrk,0), theta(NTrk,0), qOverP(NTrk,0), charge(NTrk,0), e(NTrk,0);
    CLHEP::HepVector dpxdqOverP(NTrk,0), dpxdtheta(NTrk,0), dpxdphi(NTrk,0);
    CLHEP::HepVector dpydqOverP(NTrk,0), dpydtheta(NTrk,0), dpydphi(NTrk,0);
    CLHEP::HepVector dpzdqOverP(NTrk,0), dpzdtheta(NTrk,0), dedqOverP(NTrk,0);
    CLHEP::HepVector dMdqOverP(NTrk,0), dMdtheta(NTrk,0), dMdphi(NTrk,0);
    CLHEP::HepVector dPTdqOverP(NTrk,0), dPTdtheta(NTrk,0), dPTdphi(NTrk,0);
    CLHEP::HepVector dLXYdqOverP(NTrk,0), dLXYdtheta(NTrk,0), dLXYdphi(NTrk,0);
    CLHEP::HepVector dTaudqOverP(NTrk,0), dTaudtheta(NTrk,0), dTaudphi(NTrk,0);

    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters*  bPer = (*(vxCandidate->vxTrackAtVertex()))[it]->perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      charge[it] = trkCharge;
      phi[it]    = bPer->parameters()[Trk::phi];
      theta[it]  = bPer->parameters()[Trk::theta];
      qOverP[it] = bPer->parameters()[Trk::qOverP];
      double tmp = 1./(qOverP[it]*qOverP[it]) + masses[it]*masses[it];
      double pe = (tmp>0.) ? sqrt(tmp) : 0.;
      e[it] = pe;
      dedqOverP[it]  = -1./(qOverP[it]*qOverP[it]*qOverP[it]*e[it]);
      dpxdqOverP[it] = -(sin(theta[it])*cos(phi[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpxdtheta[it]  =  (cos(theta[it])*cos(phi[it])*charge[it])/qOverP[it];
      dpxdphi[it]    = -(sin(theta[it])*sin(phi[it])*charge[it])/qOverP[it];
      dpydqOverP[it] = -(sin(theta[it])*sin(phi[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpydtheta[it]  =  (cos(theta[it])*sin(phi[it])*charge[it])/qOverP[it];
      dpydphi[it]    =  (sin(theta[it])*cos(phi[it])*charge[it])/qOverP[it];
      dpzdqOverP[it] = -(cos(theta[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpzdtheta[it]  = -(sin(theta[it])*charge[it])/qOverP[it];
      E  += e[it];
      Px += bPer->momentum()[Trk::px];
      Py += bPer->momentum()[Trk::py];
      Pz += bPer->momentum()[Trk::pz];
      if (vxCandidate->fullCovariance() == 0) V0_cov.block(5*it,5*it,5,5) = *(bPer->covariance());
    }
    double LXY = Px*dx+Py*dy;

    for( unsigned int it=0; it<NTrk; it++) {
      dMdqOverP[it]   = -(Px*dpxdqOverP[it]+Py*dpydqOverP[it]+Pz*dpzdqOverP[it]-E*dedqOverP[it])/M;
      dMdtheta[it]    = -(Px*dpxdtheta[it]+Py*dpydtheta[it]+Pz*dpzdtheta[it])/M;
      dMdphi[it]      = -(Px*dpxdphi[it]+Py*dpydphi[it])/M;
      dPTdqOverP[it]  =  (Px*dpxdqOverP[it]+Py*dpydqOverP[it])/PT;
      dPTdtheta[it]   =  (Px*dpxdtheta[it]+Py*dpydtheta[it])/PT;
      dPTdphi[it]     =  (Px*dpxdphi[it]+Py*dpydphi[it])/PT;
      dLXYdqOverP[it] =  dx*dpxdqOverP[it]+dy*dpydqOverP[it];
      dLXYdtheta[it]  =  dx*dpxdtheta[it]+dy*dpydtheta[it];
      dLXYdphi[it]    =  dx*dpxdphi[it]+dy*dpydphi[it];
      dTaudqOverP[it] =  (LXY*dMdqOverP[it]+M*dLXYdqOverP[it])/(PT*PT)-(2.*LXY*M*dPTdqOverP[it])/(PT*PT*PT);
      dTaudtheta[it]  =  (LXY*dMdtheta[it]+M*dLXYdtheta[it])/(PT*PT)-(2.*LXY*M*dPTdtheta[it])/(PT*PT*PT);
      dTaudphi[it]    =  (LXY*dMdphi[it]+M*dLXYdphi[it])/(PT*PT)-(2.*LXY*M*dPTdphi[it])/(PT*PT*PT);
    }
    double dTaudx = (M*Px)/(PT*PT);
    double dTaudy = (M*Py)/(PT*PT);
    double dTaudx0 = -dTaudx;
    double dTaudy0 = -dTaudy;

    unsigned int ndim = 0;
    if (vxCandidate->fullCovariance() != 0) {
      ndim = (vxCandidate->fullCovariance())->rows();
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

      Amg::MatrixX W_mat(5*NTrk+6,1); W_mat.setZero();
      if (vxCandidate->fullCovariance() != 0) {
        W_mat.block(0,0,ndim,ndim) = *(vxCandidate->fullCovariance());
      } else {
        W_mat.block(0,0,V0_cov.rows(),V0_cov.rows()) = V0_cov;
        W_mat.block(5*NTrk,5*NTrk,3,3) =  vxCandidate->recVertex().covariancePosition();
      }
      W_mat.block(5*NTrk+3,5*NTrk+3,3,3) = vertex.covariancePosition();
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

      Amg::MatrixX W_mat(3*NTrk+6,1); W_mat.setZero();
      W_mat.block(0,0,ndim,ndim) = *(vxCandidate->fullCovariance());
      W_mat.block(3*NTrk+3,3*NTrk+3,3,3) =  vertex.covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    }

    double tauErrsq = V0_err(0,0);
    double tauErr = (tauErrsq>0.) ? sqrt(tauErrsq) : 0.;
    return CONST*tauErr;
  }

  double V0Tools::tauError(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, double posTrackMass, double negTrackMass, double massV0) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    std::vector<double> masses;
    masses.push_back(posTrackMass);
    masses.push_back(negTrackMass);

    return tauError(vxCandidate,vertex,masses,massV0);
  }

  double V0Tools::tauError(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, std::vector<double> masses, double ) const
  {
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
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

  double V0Tools::tauError(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, double M) const
  {
    // Tau = CONST*M*(Px*dx+Py*dy)/(PT*PT)
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    double CONST = 1000./CLHEP::c_light;
    double PT = V0Momentum(vxCandidate).perp();
    double dx = (vxCandidate->recVertex().position() - vertex.position()).x();
    double dy = (vxCandidate->recVertex().position() - vertex.position()).y();
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    Amg::MatrixX V0_cov(5*NTrk,1); V0_cov.setZero();
    double Px=0., Py=0.; 
    CLHEP::HepVector phi(NTrk,0), theta(NTrk,0), qOverP(NTrk,0), charge(NTrk,0);
    CLHEP::HepVector dpxdqOverP(NTrk,0), dpxdtheta(NTrk,0), dpxdphi(NTrk,0);
    CLHEP::HepVector dpydqOverP(NTrk,0), dpydtheta(NTrk,0), dpydphi(NTrk,0);
    CLHEP::HepVector dPTdqOverP(NTrk,0), dPTdtheta(NTrk,0), dPTdphi(NTrk,0);
    CLHEP::HepVector dLXYdqOverP(NTrk,0), dLXYdtheta(NTrk,0), dLXYdphi(NTrk,0);
    CLHEP::HepVector dTaudqOverP(NTrk,0), dTaudtheta(NTrk,0), dTaudphi(NTrk,0);

    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters*  bPer = (*(vxCandidate->vxTrackAtVertex()))[it]->perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      charge[it] = trkCharge;
      phi[it]    = bPer->parameters()[Trk::phi];
      theta[it]  = bPer->parameters()[Trk::theta];
      qOverP[it] = bPer->parameters()[Trk::qOverP];
      dpxdqOverP[it] = -(sin(theta[it])*cos(phi[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpxdtheta[it]  =  (cos(theta[it])*cos(phi[it])*charge[it])/qOverP[it];
      dpxdphi[it]    = -(sin(theta[it])*sin(phi[it])*charge[it])/qOverP[it];
      dpydqOverP[it] = -(sin(theta[it])*sin(phi[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpydtheta[it]  =  (cos(theta[it])*sin(phi[it])*charge[it])/qOverP[it];
      dpydphi[it]    =  (sin(theta[it])*cos(phi[it])*charge[it])/qOverP[it];
      Px += bPer->momentum()[Trk::px];
      Py += bPer->momentum()[Trk::py];
      if (vxCandidate->fullCovariance() == 0) V0_cov.block(5*it,5*it,5,5) =  *(bPer->covariance());
    }
    double LXY = Px*dx+Py*dy;

    for( unsigned int it=0; it<NTrk; it++) {
      dPTdqOverP[it]  = (Px*dpxdqOverP[it]+Py*dpydqOverP[it])/PT;
      dPTdtheta[it]   = (Px*dpxdtheta[it]+Py*dpydtheta[it])/PT;
      dPTdphi[it]     = (Px*dpxdphi[it]+Py*dpydphi[it])/PT;
      dLXYdqOverP[it] = dx*dpxdqOverP[it]+dy*dpydqOverP[it];
      dLXYdtheta[it]  = dx*dpxdtheta[it]+dy*dpydtheta[it];
      dLXYdphi[it]    = dx*dpxdphi[it]+dy*dpydphi[it];
      dTaudqOverP[it] = M*dLXYdqOverP[it]/(PT*PT)-(2.*LXY*M*dPTdqOverP[it])/(PT*PT*PT);
      dTaudtheta[it]  = M*dLXYdtheta[it]/(PT*PT)-(2.*LXY*M*dPTdtheta[it])/(PT*PT*PT);
      dTaudphi[it]    = M*dLXYdphi[it]/(PT*PT)-(2.*LXY*M*dPTdphi[it])/(PT*PT*PT);
    }
    double dTaudx = (M*Px)/(PT*PT);
    double dTaudy = (M*Py)/(PT*PT);
    double dTaudx0 = -dTaudx;
    double dTaudy0 = -dTaudy;

    unsigned int ndim = 0;
    if (vxCandidate->fullCovariance() != 0) {
      ndim = (vxCandidate->fullCovariance())->rows();
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

      Amg::MatrixX W_mat(5*NTrk+6,1); W_mat.setZero();
      if (vxCandidate->fullCovariance() != 0) {
        W_mat.block(0,0,ndim,ndim) = *(vxCandidate->fullCovariance());
      } else {
        W_mat.block(0,0,V0_cov.rows(),V0_cov.rows()) = V0_cov;
        W_mat.block(5*NTrk,5*NTrk,3,3) = vxCandidate->recVertex().covariancePosition();
      }
      W_mat.block(5*NTrk+3,5*NTrk+3,3,3) = vertex.covariancePosition();
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

      Amg::MatrixX W_mat(3*NTrk+6,1); W_mat.setZero();
      W_mat.block(0,0,ndim,ndim) = *(vxCandidate->fullCovariance());
      W_mat.block(3*NTrk+3,3*NTrk+3,3,3) = vertex.covariancePosition();
      V0_err = D_vec.transpose() * W_mat * D_vec;
    }

    double tauErrsq = V0_err(0,0);
    double tauErr = (tauErrsq>0.) ? sqrt(tauErrsq) : 0.;
    return CONST*tauErr;
  }

  double V0Tools::thetaStar(const ExtendedVxCandidate * vxCandidate, double mass1, double mass2) const
  {
    double theta = 0.;
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    CLHEP::HepLorentzVector v1 = track4Momentum(vxCandidate,0,mass1);
    CLHEP::HepLorentzVector v2 = track4Momentum(vxCandidate,1,mass2);
    CLHEP::HepLorentzVector v0 = v1 + v2;
    CLHEP::Hep3Vector boost_v0 = v0.boostVector();
    boost_v0 *= -1.0;
    v1.boost( boost_v0 );
    v2.boost( boost_v0 );
    theta = v0.angle( v1.vect() );
    return theta;
  }

  double V0Tools::cosThetaStar(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    CLHEP::HepLorentzVector posMom = positiveTrack4Momentum(vxCandidate, posTrackMass);
    CLHEP::HepLorentzVector negMom = negativeTrack4Momentum(vxCandidate, negTrackMass);
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

//  double V0Tools::phiStar(const ExtendedVxCandidate * vxCandidate) const
//  {
//    assert(vxCandidate!=0);
//    if(0 == vxCandidate) {
//      return -999999.;
//    }
//    CLHEP::HepLorentzVector posMom = positiveTrack4Momentum(vxCandidate,0.);
//    CLHEP::HepLorentzVector negMom = negativeTrack4Momentum(vxCandidate,0.);
//    return phiStar(posMom, negMom);
//  }
//
//  double V0Tools::phiStar(const CLHEP::HepLorentzVector & posTrack, const CLHEP::HepLorentzVector & negTrack) const
//  {
//    CLHEP::HepLorentzVector v0(posTrack + negTrack);
//    double p1   = posTrack.rho();
//    double p2   = negTrack.rho();
//    double p1p2 = posTrack.px()*negTrack.px() + posTrack.py()*negTrack.py() + posTrack.pz()*negTrack.pz();
//    double c12 = p1p2 / ( p1*p2 );
//    double s12sq = 1. -c12*c12;
//    double s12 = (s12sq>0.) ? sqrt(s12sq) : 0.;
//    double num = (posTrack.py()*negTrack.pz() - posTrack.pz()*negTrack.py()) * v0.py() -
//                 (posTrack.pz()*negTrack.px() - posTrack.px()*negTrack.pz()) * v0.px();
//    return acos(num / ( p1*p2*s12*v0.perp()));
//  }
//
//  double V0Tools::phiStar(const CLHEP::HepLorentzVector & posTrack, const CLHEP::HepLorentzVector & negTrack) const
//  {
//    CLHEP::HepLorentzVector v0(posTrack + negTrack);
//    CLHEP::Hep3Vector N1(v0.py(),-v0.px(),0.);
//    double X = posTrack.py()*negTrack.pz() - posTrack.pz()*negTrack.py();
//    double Y = posTrack.pz()*negTrack.px() - posTrack.px()*negTrack.pz();
//    double Z = posTrack.px()*negTrack.py() - posTrack.py()*negTrack.px();
//    CLHEP::Hep3Vector N2(X,Y,Z);
//    ////double normsq = X*X+Y*Y+Z*Z;
//    ////double norm = (normsq>0.) ? sqrt(normsq) : 0.;
//    ////return acos((N1*N2)/(norm*v0.perp()));
//    //return acos((N1*N2)/(N1.mag()*N2.mag()));
//    double x = -v0.px()*Z;
//    double y = -v0.py()*Z;
//    double z = v0.px()*X + v0.py()*Y;
//    CLHEP::Hep3Vector N3(x,y,z);
//    return asin(N3.mag()/(N1.mag()*N2.mag()));
//  }

  double V0Tools::phiStar(const ExtendedVxCandidate * vxCandidate) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    return phiStar(positiveTrack4Momentum(vxCandidate,0.)+negativeTrack4Momentum(vxCandidate,0.),positiveTrack4Momentum(vxCandidate,0.));
  }

  double V0Tools::phiStar(const CLHEP::HepLorentzVector & v0, const CLHEP::HepLorentzVector & track) const
  {
    CLHEP::Hep3Vector V0 = v0.getV();;
    CLHEP::Hep3Vector trk = track.getV();;
    trk.rotateZ(-V0.phi());
    trk.rotateY(-V0.theta());
    return atan2(trk.y(),trk.x());
  }

  double V0Tools::cosTheta(const ExtendedVxCandidate * vxCandidate, Amg::Vector3D& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    return (V0Momentum(vxCandidate).dot((vtx(vxCandidate)-vertex)))/
           (V0Momentum(vxCandidate).mag()*(vtx(vxCandidate)-vertex).mag());
  }

  double V0Tools::cosTheta(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    return (V0Momentum(vxCandidate).dot((vtx(vxCandidate)-vertex.position())))/
           (V0Momentum(vxCandidate).mag()*(vtx(vxCandidate)-vertex.position()).mag());
  }

  double V0Tools::cosTheta_xy(const ExtendedVxCandidate * vxCandidate, Amg::Vector3D& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    return (V0Momentum(vxCandidate).x()*(vtx(vxCandidate)-vertex).x()+
            V0Momentum(vxCandidate).y()*(vtx(vxCandidate)-vertex).y())/
           (pT(vxCandidate)*(vtx(vxCandidate)-vertex).perp());
  }

  double V0Tools::cosTheta_xy(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    return (V0Momentum(vxCandidate).x()*(vtx(vxCandidate)-vertex.position()).x()+
            V0Momentum(vxCandidate).y()*(vtx(vxCandidate)-vertex.position()).y())/
           (pT(vxCandidate)*(vtx(vxCandidate)-vertex.position()).perp());
  }

  int V0Tools::charge(const ExtendedVxCandidate * vxCandidate) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999;
    }
    int ch = 0;
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParticleBase* pb = origTrackPB(vxCandidate,it);
      const Rec::TrackParticle* tp = dynamic_cast<const Rec::TrackParticle*>(pb);
      if (tp == 0) return -999999;
      ch += tp->charge();
    }
    return ch;
  }

  const Trk::TrackParticleBase* V0Tools::origTrackPB(const VxCandidate * vxCandidate, int trkIndex) const
  {
    const Trk::TrackParticleBase* origTrk(0);
    assert(vxCandidate!=0);
    //if(0 == vxCandidate) { ATH_MSG_DEBUG("Non-existing vxCandidate"); }
    if (0 != vxCandidate) {
      const Trk::ITrackLink* tmpLink = (*(vxCandidate->vxTrackAtVertex()))[trkIndex]->trackOrParticleLink();
      if (!tmpLink) return origTrk;
      const Trk::LinkToTrackParticleBase* tmpTPB = dynamic_cast<const Trk::LinkToTrackParticleBase*>(tmpLink);
      if (tmpTPB && tmpTPB->isValid()) origTrk = tmpTPB->cachedElement();
    }
    return origTrk;
  }

  const Trk::TrackParticleBase* V0Tools::origTrackPB(const ExtendedVxCandidate * vxCandidate, int trkIndex) const
  {
    const Trk::TrackParticleBase* origTrk(0);
    assert(vxCandidate!=0);
    //if(0 == vxCandidate) { ATH_MSG_DEBUG("Non-existing vxCandidate"); }
    if ( 0 != vxCandidate &&
        (static_cast<int>(vxCandidate->vxTrackAtVertex()->size())>trkIndex) &&
        (*(vxCandidate->vxTrackAtVertex()))[trkIndex] ) {
      const Trk::ITrackLink* tmpLink = (*(vxCandidate->vxTrackAtVertex()))[trkIndex]->trackOrParticleLink();
      if (!tmpLink) return origTrk;
      const Trk::LinkToTrackParticleBase* tmpTPB = dynamic_cast<const Trk::LinkToTrackParticleBase*>(tmpLink);
      if (tmpTPB && tmpTPB->isValid()) origTrk = tmpTPB->cachedElement();
    }
    return origTrk;
  }

  const Trk::TrackParticleBase* V0Tools::positiveOrigTrackPB(const ExtendedVxCandidate * vxCandidate) const
  {
    const Trk::TrackParticleBase* origTrk(0);
    assert(vxCandidate!=0);
    //if(0 == vxCandidate) { ATH_MSG_DEBUG("Non-existing vxCandidate"); }
    if(0 != vxCandidate) {
      const Trk::TrackParticleBase* orgPB0 = origTrackPB(vxCandidate,0);
      const Rec::TrackParticle* orgTP0 = dynamic_cast<const Rec::TrackParticle*>(orgPB0);
      if (orgTP0) {
        double qOverP = orgTP0->measuredPerigee()->parameters()[Trk::qOverP];
        const Trk::TrackParticleBase* orgPB1 = origTrackPB(vxCandidate,1);
        if(qOverP >= 0.) {
          origTrk = orgPB0;
        } else {
          origTrk = orgPB1;
        }
      }
    }
    return origTrk;
  }

  const Trk::TrackParticleBase* V0Tools::negativeOrigTrackPB(const ExtendedVxCandidate * vxCandidate) const
  {
    const Trk::TrackParticleBase* origTrk(0);
    assert(vxCandidate!=0);
    //if(0 == vxCandidate) { ATH_MSG_DEBUG("Non-existing vxCandidate"); }
    if(0 != vxCandidate) {
      const Trk::TrackParticleBase* orgPB0 = origTrackPB(vxCandidate,0);
      const Rec::TrackParticle* orgTP0 = dynamic_cast<const Rec::TrackParticle*>(orgPB0);
      if (orgTP0) {
        double qOverP = orgTP0->measuredPerigee()->parameters()[Trk::qOverP];
        const Trk::TrackParticleBase* orgPB1 = origTrackPB(vxCandidate,1);
        if(qOverP >= 0.) {
          origTrk = orgPB1;
        } else {
          origTrk = orgPB0;
        }
      }
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

  double V0Tools::invariantMassBeforeFitIP(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Invariant mass for non-existing vxCandidate requested");
      //throw std::runtime_error("non-existing vxCandidate");
      return -999999.;
    }
    std::vector<double> masses;
    masses.push_back(posTrackMass);
    masses.push_back(negTrackMass);

    return invariantMassBeforeFitIP(vxCandidate,masses);
  }

  double V0Tools::invariantMassBeforeFitIP(const ExtendedVxCandidate * vxCandidate, std::vector<double> masses) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Invariant mass for non-existing vxCandidate requested");
      //throw std::runtime_error("non-existing vxCandidate");
      return -999999.;
    }
    double px = 0., py = 0., pz = 0., e = 0.;
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParticleBase* PB = origTrackPB(vxCandidate,it);
      const Rec::TrackParticle* TP = dynamic_cast<const Rec::TrackParticle*>(PB);
      if (TP == 0) return -999999.;
      px += TP->measuredPerigee()->momentum().x();
      py += TP->measuredPerigee()->momentum().y();
      pz += TP->measuredPerigee()->momentum().z();
      double pesq = TP->measuredPerigee()->momentum().mag2() + masses[it]*masses[it];
      double pe = (pesq>0.) ? sqrt(pesq) : 0.;
      e += pe;
    }
    double msq = e*e - px*px - py*py - pz*pz;
    return (msq>0.) ? sqrt(msq) : 0.;
  }

  double V0Tools::invariantMassBeforeFit(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Invariant mass for non-existing vxCandidate requested");
      //throw std::runtime_error("non-existing vxCandidate");
      return -999999.;
    }
    std::vector<double> masses;
    masses.push_back(posTrackMass);
    masses.push_back(negTrackMass);

    return invariantMassBeforeFit(vxCandidate,masses);
  }

  double V0Tools::invariantMassBeforeFit(const ExtendedVxCandidate * vxCandidate, std::vector<double> masses) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Invariant mass for non-existing vxCandidate requested");
      //throw std::runtime_error("non-existing vxCandidate");
      return -999999.;
    }
    Trk::PerigeeSurface perigeeSurface(vxCandidate->recVertex().position());
    double px = 0., py = 0., pz = 0., e = 0.;
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParticleBase* PB = origTrackPB(vxCandidate,it);
      const Rec::TrackParticle* TP = dynamic_cast<const Rec::TrackParticle*>(PB);
      if (TP == 0) return -999999.;
      const Trk::TrackParameters* extrPer = m_extrapolator->extrapolateDirectly(*TP->measuredPerigee(), perigeeSurface);
      if (extrPer == 0) return -999999.;
      px += extrPer->momentum().x();
      py += extrPer->momentum().y();
      pz += extrPer->momentum().z();
      double pesq = extrPer->momentum().mag2() + masses[it]*masses[it];
      double pe = (pesq>0.) ? sqrt(pesq) : 0.;
      e += pe;
    }
    double msq = e*e - px*px - py*py - pz*pz;
    return (msq>0.) ? sqrt(msq) : 0.;
  }

  double V0Tools::invariantMassBeforeFit(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass, Amg::Vector3D& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Invariant mass for non-existing vxCandidate requested");
      //throw std::runtime_error("non-existing vxCandidate");
      return -999999.;
    }
    std::vector<double> masses;
    masses.push_back(posTrackMass);
    masses.push_back(negTrackMass);

    return invariantMassBeforeFit(vxCandidate,masses,vertex);
  }

  double V0Tools::invariantMassBeforeFit(const ExtendedVxCandidate * vxCandidate, std::vector<double> masses, Amg::Vector3D& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Invariant mass for non-existing vxCandidate requested");
      //throw std::runtime_error("non-existing vxCandidate");
      return -999999.;
    }
    Trk::PerigeeSurface perigeeSurface(vertex);
    double px = 0., py = 0., pz = 0., e = 0.;
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParticleBase* PB = origTrackPB(vxCandidate,it);
      const Rec::TrackParticle* TP = dynamic_cast<const Rec::TrackParticle*>(PB);
      if (TP == 0) return -999999.;
      const Trk::TrackParameters* extrPer = m_extrapolator->extrapolateDirectly(*TP->measuredPerigee(), perigeeSurface);
      if (extrPer == 0) return -999999.;
      px += extrPer->momentum().x();
      py += extrPer->momentum().y();
      pz += extrPer->momentum().z();
      double pesq = extrPer->momentum().mag2() + masses[it]*masses[it];
      double pe = (pesq>0.) ? sqrt(pesq) : 0.;
      e += pe;
    }
    double msq = e*e - px*px - py*py - pz*pz;
    return (msq>0.) ? sqrt(msq) : 0.;
  }

  // JRC - not sure how to achieve this with the new tracking model - no more MeasuredPerigee
  /*
  double V0Tools::invariantMassErrorBeforeFitIP(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Invariant mass error for non-existing vxCandidate requested");
      return -999999.;
    }
    std::vector<double> masses;
    masses.push_back(posTrackMass);
    masses.push_back(negTrackMass);

    return invariantMassErrorBeforeFitIP(vxCandidate,masses);
  }
  */

  // JRC - not sure how to achieve this with the new tracking model - no more MeasuredPerigee
  /*   
  double V0Tools::invariantMassErrorBeforeFitIP(const ExtendedVxCandidate * vxCandidate, std::vector<double> masses) const
  {
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    double mass = invariantMassBeforeFitIP(vxCandidate, masses);
    double E=0., Px=0., Py=0., Pz=0.; 
    CLHEP::HepVector d0(NTrk,0), z0(NTrk,0), phi(NTrk,0), theta(NTrk,0), qOverP(NTrk,0), charge(NTrk,0), e(NTrk,0);
    CLHEP::HepVector dm2dphi(NTrk,0), dm2dtheta(NTrk,0), dm2dqOverP(NTrk,0);
    CLHEP::HepSymMatrix V0_cor(5*NTrk,0);
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParticleBase* PB = origTrackPB(vxCandidate,it);
      const Rec::TrackParticle* TP = dynamic_cast<const Rec::TrackParticle*>(PB);
      if (TP == 0) return -999999.;
      const Trk::MeasuredPerigee* mPer = TP->measuredPerigee();
      Trk::CovarianceMatrix cov_tmp = mPer->localErrorMatrix().covariance();
      V0_cor(5*it+3,5*it+3) = cov_tmp(2,2);
      V0_cor(5*it+3,5*it+4) = cov_tmp(2,3);
      V0_cor(5*it+3,5*it+5) = cov_tmp(2,4);
      V0_cor(5*it+4,5*it+4) = cov_tmp(3,3);
      V0_cor(5*it+4,5*it+5) = cov_tmp(3,4);
      V0_cor(5*it+5,5*it+5) = cov_tmp(4,4);
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      charge[it] = trkCharge;
      d0[it]     = bPer->parameters()[Trk::d0];
      z0[it]     = bPer->parameters()[Trk::z0];
      phi[it]    = bPer->parameters()[Trk::phi];
      theta[it]  = bPer->parameters()[Trk::theta];
      qOverP[it] = bPer->parameters()[Trk::qOverP];
      double tmp = 1./(qOverP[it]*qOverP[it]) + masses[it]*masses[it];
      double pe = (tmp>0.) ? sqrt(tmp) : 0.;
      e[it] = pe;
      E  += e[it];
      Px += bPer->momentum().x();
      Py += bPer->momentum().y();
      Pz += bPer->momentum().z();
    }

    for( unsigned int it=0; it<NTrk; it++) {
      dm2dphi[it]    = 2.*(Px*sin(phi[it])-Py*cos(phi[it]))*sin(theta[it])*charge[it]/qOverP[it];
      dm2dtheta[it]  = 2.*(Pz*sin(theta[it])-(Px*cos(phi[it])+Py*sin(phi[it]))*cos(theta[it]))*charge[it]/qOverP[it];
      dm2dqOverP[it] = 2.*(Pz*cos(theta[it])+(Px*cos(phi[it])+Py*sin(phi[it]))*sin(theta[it])-E*charge[it]/(qOverP[it]*e[it]))*charge[it]/(qOverP[it]*qOverP[it]);
    }

    CLHEP::HepVector D_vec(5*NTrk,0);
    for( unsigned int it=0; it<NTrk; it++) {
      D_vec(5*it+1) = 0.;
      D_vec(5*it+2) = 0.;
      D_vec(5*it+3) = dm2dphi[it];
      D_vec(5*it+4) = dm2dtheta[it];
      D_vec(5*it+5) = dm2dqOverP[it];
    }
    CLHEP::HepMatrix V0_merr = D_vec.T() * V0_cor * D_vec;

    double massVarsq = V0_merr(1,1);
    double massVar = (massVarsq>0.) ? sqrt(massVarsq) : 0.;
    double massErr = massVar/(2.*mass);
    return massErr;
  }
  */ 

  double V0Tools::invariantMassErrorBeforeFit(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Invariant mass error for non-existing vxCandidate requested");
      return -999999.;
    }
    std::vector<double> masses;
    masses.push_back(posTrackMass);
    masses.push_back(negTrackMass);

    return invariantMassErrorBeforeFit(vxCandidate,masses);
  }

  double V0Tools::invariantMassErrorBeforeFit(const ExtendedVxCandidate * vxCandidate, std::vector<double> masses) const
  {
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    Trk::PerigeeSurface perigeeSurface(vxCandidate->recVertex().position());
    double E=0., Px=0., Py=0., Pz=0.; 
    CLHEP::HepVector d0(NTrk,0), z0(NTrk,0), phi(NTrk,0), theta(NTrk,0), qOverP(NTrk,0), charge(NTrk,0), e(NTrk,0);
    CLHEP::HepVector dm2dphi(NTrk,0), dm2dtheta(NTrk,0), dm2dqOverP(NTrk,0);
    CLHEP::HepSymMatrix V0_cor(5*NTrk,0);
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParticleBase* PB = origTrackPB(vxCandidate,it);
      const Rec::TrackParticle* TP = dynamic_cast<const Rec::TrackParticle*>(PB);
      if (TP == 0) return -999999.;
      const Trk::TrackParameters* maybeP = m_extrapolator->extrapolateDirectly(*TP->measuredPerigee(), perigeeSurface);
      if (maybeP == 0) return -999999.;
      const AmgSymMatrix(5)* cov_tmp = maybeP->covariance();
      V0_cor(5*it+3,5*it+3) = (*cov_tmp)(2,2);
      V0_cor(5*it+3,5*it+4) = (*cov_tmp)(2,3);
      V0_cor(5*it+3,5*it+5) = (*cov_tmp)(2,4);
      V0_cor(5*it+4,5*it+4) = (*cov_tmp)(3,3);
      V0_cor(5*it+4,5*it+5) = (*cov_tmp)(3,4);
      V0_cor(5*it+5,5*it+5) = (*cov_tmp)(4,4);
      double trkCharge = 1.;
      if (maybeP->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      charge[it] = trkCharge;
      d0[it]     = maybeP->parameters()[Trk::d0];
      z0[it]     = maybeP->parameters()[Trk::z0];
      phi[it]    = maybeP->parameters()[Trk::phi];
      theta[it]  = maybeP->parameters()[Trk::theta];
      qOverP[it] = maybeP->parameters()[Trk::qOverP];
      double tmp = 1./(qOverP[it]*qOverP[it]) + masses[it]*masses[it];
      double pe = (tmp>0.) ? sqrt(tmp) : 0.;
      e[it] = pe;
      E  += e[it];
      Px += maybeP->momentum().x();
      Py += maybeP->momentum().y();
      Pz += maybeP->momentum().z();
      delete maybeP;
    }
    double msq = E*E - Px*Px - Py*Py - Pz*Pz;
    double mass = (msq>0.) ? sqrt(msq) : 0.;

    for( unsigned int it=0; it<NTrk; it++) {
      dm2dphi[it]    = 2.*(Px*sin(phi[it])-Py*cos(phi[it]))*sin(theta[it])*charge[it]/qOverP[it];
      dm2dtheta[it]  = 2.*(Pz*sin(theta[it])-(Px*cos(phi[it])+Py*sin(phi[it]))*cos(theta[it]))*charge[it]/qOverP[it];
      dm2dqOverP[it] = 2.*(Pz*cos(theta[it])+(Px*cos(phi[it])+Py*sin(phi[it]))*sin(theta[it])-E*charge[it]/(qOverP[it]*e[it]))*charge[it]/(qOverP[it]*qOverP[it]);
    }

    CLHEP::HepVector D_vec(5*NTrk,0);
    for( unsigned int it=0; it<NTrk; it++) {
      D_vec(5*it+1) = 0.;
      D_vec(5*it+2) = 0.;
      D_vec(5*it+3) = dm2dphi[it];
      D_vec(5*it+4) = dm2dtheta[it];
      D_vec(5*it+5) = dm2dqOverP[it];
    }
    CLHEP::HepMatrix V0_merr = D_vec.T() * V0_cor * D_vec;

    double massVarsq = V0_merr(1,1);
    double massVar = (massVarsq>0.) ? sqrt(massVarsq) : 0.;
    double massErr = massVar/(2.*mass);
    return massErr;
  }
 
  double V0Tools::invariantMassErrorBeforeFit(const ExtendedVxCandidate * vxCandidate, double posTrackMass, double negTrackMass, Amg::Vector3D& vertex) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      //ATH_MSG_DEBUG("Invariant mass error for non-existing vxCandidate requested");
      return -999999.;
    }
    std::vector<double> masses;
    masses.push_back(posTrackMass);
    masses.push_back(negTrackMass);

    return invariantMassErrorBeforeFit(vxCandidate,masses,vertex);
  }

  double V0Tools::invariantMassErrorBeforeFit(const ExtendedVxCandidate * vxCandidate, std::vector<double> masses, Amg::Vector3D& vertex) const
  {
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    Trk::PerigeeSurface perigeeSurface(vertex);
    double E=0., Px=0., Py=0., Pz=0.; 
    CLHEP::HepVector d0(NTrk,0), z0(NTrk,0), phi(NTrk,0), theta(NTrk,0), qOverP(NTrk,0), charge(NTrk,0), e(NTrk,0);
    CLHEP::HepVector dm2dphi(NTrk,0), dm2dtheta(NTrk,0), dm2dqOverP(NTrk,0);
    CLHEP::HepSymMatrix V0_cor(5*NTrk,0);
    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParticleBase* PB = origTrackPB(vxCandidate,it);
      const Rec::TrackParticle* TP = dynamic_cast<const Rec::TrackParticle*>(PB);
      if (TP == 0) return -999999.;
      const Trk::TrackParameters* maybeP = m_extrapolator->extrapolateDirectly(*TP->measuredPerigee(), perigeeSurface);
      if (maybeP == 0 ) return -999999.;
      const AmgSymMatrix(5)* cov_tmp = maybeP->covariance();
      V0_cor(5*it+3,5*it+3) = (*cov_tmp)(2,2);
      V0_cor(5*it+3,5*it+4) = (*cov_tmp)(2,3);
      V0_cor(5*it+3,5*it+5) = (*cov_tmp)(2,4);
      V0_cor(5*it+4,5*it+4) = (*cov_tmp)(3,3);
      V0_cor(5*it+4,5*it+5) = (*cov_tmp)(3,4);
      V0_cor(5*it+5,5*it+5) = (*cov_tmp)(4,4);
      double trkCharge = 1.;
      if (maybeP->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      charge[it] = trkCharge;
      d0[it]     = maybeP->parameters()[Trk::d0];
      z0[it]     = maybeP->parameters()[Trk::z0];
      phi[it]    = maybeP->parameters()[Trk::phi];
      theta[it]  = maybeP->parameters()[Trk::theta];
      qOverP[it] = maybeP->parameters()[Trk::qOverP];
      double tmp = 1./(qOverP[it]*qOverP[it]) + masses[it]*masses[it];
      double pe = (tmp>0.) ? sqrt(tmp) : 0.;
      e[it] = pe;
      E  += e[it];
      Px += maybeP->momentum().x();
      Py += maybeP->momentum().y();
      Pz += maybeP->momentum().z();
      delete maybeP;
    }
    double msq = E*E - Px*Px - Py*Py - Pz*Pz;
    double mass = (msq>0.) ? sqrt(msq) : 0.;

    for( unsigned int it=0; it<NTrk; it++) {
      dm2dphi[it]    = 2.*(Px*sin(phi[it])-Py*cos(phi[it]))*sin(theta[it])*charge[it]/qOverP[it];
      dm2dtheta[it]  = 2.*(Pz*sin(theta[it])-(Px*cos(phi[it])+Py*sin(phi[it]))*cos(theta[it]))*charge[it]/qOverP[it];
      dm2dqOverP[it] = 2.*(Pz*cos(theta[it])+(Px*cos(phi[it])+Py*sin(phi[it]))*sin(theta[it])-E*charge[it]/(qOverP[it]*e[it]))*charge[it]/(qOverP[it]*qOverP[it]);
    }

    CLHEP::HepVector D_vec(5*NTrk,0);
    for( unsigned int it=0; it<NTrk; it++) {
      D_vec(5*it+1) = 0.;
      D_vec(5*it+2) = 0.;
      D_vec(5*it+3) = dm2dphi[it];
      D_vec(5*it+4) = dm2dtheta[it];
      D_vec(5*it+5) = dm2dqOverP[it];
    }
    CLHEP::HepMatrix V0_merr = D_vec.T() * V0_cor * D_vec;

    double massVarsq = V0_merr(1,1);
    double massVar = (massVarsq>0.) ? sqrt(massVarsq) : 0.;
    double massErr = massVar/(2.*mass);
    return massErr;
  }

  double V0Tools::massTauCov(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, double posTrackMass, double negTrackMass) const
  {
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    std::vector<double> masses;
    masses.push_back(posTrackMass);
    masses.push_back(negTrackMass);

    return massTauCov(vxCandidate,vertex,masses);
  }

  double V0Tools::massTauCov(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, std::vector<double> masses) const
  {
    // Tau = CONST*M*(Px*dx+Py*dy)/(PT*PT)
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return -999999.;
    }
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return -999999.;
    }
    double CONST = 1000./CLHEP::c_light;
    double PT = V0Momentum(vxCandidate).perp();
    double dx = (vxCandidate->recVertex().position() - vertex.position()).x();
    double dy = (vxCandidate->recVertex().position() - vertex.position()).y();
    double M = invariantMass(vxCandidate, masses);
    Amg::MatrixX V0_cov(5*NTrk,5*NTrk); V0_cov.setZero();
    double E=0., Px=0., Py=0., Pz=0.; 
    CLHEP::HepVector phi(NTrk,0), theta(NTrk,0), qOverP(NTrk,0), charge(NTrk,0), e(NTrk,0);
    CLHEP::HepVector dpxdqOverP(NTrk,0), dpxdtheta(NTrk,0), dpxdphi(NTrk,0);
    CLHEP::HepVector dpydqOverP(NTrk,0), dpydtheta(NTrk,0), dpydphi(NTrk,0);
    CLHEP::HepVector dpzdqOverP(NTrk,0), dpzdtheta(NTrk,0), dedqOverP(NTrk,0);
    CLHEP::HepVector dMdqOverP(NTrk,0), dMdtheta(NTrk,0), dMdphi(NTrk,0);
    CLHEP::HepVector dPTdqOverP(NTrk,0), dPTdtheta(NTrk,0), dPTdphi(NTrk,0);
    CLHEP::HepVector dLXYdqOverP(NTrk,0), dLXYdtheta(NTrk,0), dLXYdphi(NTrk,0);
    CLHEP::HepVector dTaudqOverP(NTrk,0), dTaudtheta(NTrk,0), dTaudphi(NTrk,0);

    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters*  bPer = (*(vxCandidate->vxTrackAtVertex()))[it]->perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      charge[it] = trkCharge;
      phi[it]    = bPer->parameters()[Trk::phi];
      theta[it]  = bPer->parameters()[Trk::theta];
      qOverP[it] = bPer->parameters()[Trk::qOverP];
      double tmp = 1./(qOverP[it]*qOverP[it]) + masses[it]*masses[it];
      double pe = (tmp>0.) ? sqrt(tmp) : 0.;
      e[it] = pe;
      dedqOverP[it]  = -1./(qOverP[it]*qOverP[it]*qOverP[it]*e[it]);
      dpxdqOverP[it] = -(sin(theta[it])*cos(phi[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpxdtheta[it]  =  (cos(theta[it])*cos(phi[it])*charge[it])/qOverP[it];
      dpxdphi[it]    = -(sin(theta[it])*sin(phi[it])*charge[it])/qOverP[it];
      dpydqOverP[it] = -(sin(theta[it])*sin(phi[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpydtheta[it]  =  (cos(theta[it])*sin(phi[it])*charge[it])/qOverP[it];
      dpydphi[it]    =  (sin(theta[it])*cos(phi[it])*charge[it])/qOverP[it];
      dpzdqOverP[it] = -(cos(theta[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpzdtheta[it]  = -(sin(theta[it])*charge[it])/qOverP[it];
      E  += e[it];
      Px += bPer->momentum()[Trk::px];
      Py += bPer->momentum()[Trk::py];
      Pz += bPer->momentum()[Trk::pz];
      if (vxCandidate->fullCovariance() == 0) V0_cov.block(5*it,5*it,5,5) = *(bPer->covariance());
    }
    double LXY = Px*dx+Py*dy;

    for( unsigned int it=0; it<NTrk; it++) {
      dMdqOverP[it]   = -(Px*dpxdqOverP[it]+Py*dpydqOverP[it]+Pz*dpzdqOverP[it]-E*dedqOverP[it])/M;
      dMdtheta[it]    = -(Px*dpxdtheta[it]+Py*dpydtheta[it]+Pz*dpzdtheta[it])/M;
      dMdphi[it]      = -(Px*dpxdphi[it]+Py*dpydphi[it])/M;
      dPTdqOverP[it]  =  (Px*dpxdqOverP[it]+Py*dpydqOverP[it])/PT;
      dPTdtheta[it]   =  (Px*dpxdtheta[it]+Py*dpydtheta[it])/PT;
      dPTdphi[it]     =  (Px*dpxdphi[it]+Py*dpydphi[it])/PT;
      dLXYdqOverP[it] =  dx*dpxdqOverP[it]+dy*dpydqOverP[it];
      dLXYdtheta[it]  =  dx*dpxdtheta[it]+dy*dpydtheta[it];
      dLXYdphi[it]    =  dx*dpxdphi[it]+dy*dpydphi[it];
      dTaudqOverP[it] =  (LXY*dMdqOverP[it]+M*dLXYdqOverP[it])/(PT*PT)-(2.*LXY*M*dPTdqOverP[it])/(PT*PT*PT);
      dTaudtheta[it]  =  (LXY*dMdtheta[it]+M*dLXYdtheta[it])/(PT*PT)-(2.*LXY*M*dPTdtheta[it])/(PT*PT*PT);
      dTaudphi[it]    =  (LXY*dMdphi[it]+M*dLXYdphi[it])/(PT*PT)-(2.*LXY*M*dPTdphi[it])/(PT*PT*PT);
    }
    double dTaudx = (M*Px)/(PT*PT);
    double dTaudy = (M*Py)/(PT*PT);
    double dTaudx0 = -dTaudx;
    double dTaudy0 = -dTaudy;

    unsigned int ndim = 0;
    if (vxCandidate->fullCovariance() != 0) {
      ndim = (vxCandidate->fullCovariance())->rows();
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
      if (vxCandidate->fullCovariance() != 0) {
        W_mat.block(0,0,ndim,ndim) = *(vxCandidate->fullCovariance());
      } else {
        W_mat.block(0,0,V0_cov.rows(),V0_cov.rows());
        W_mat.block(5*NTrk,5*NTrk,3,3) = vxCandidate->recVertex().covariancePosition();
      }
      W_mat.block(5*NTrk+3,5*NTrk+3,3,3) =  vertex.covariancePosition();
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
      W_mat.block(0,0,ndim,ndim) = *(vxCandidate->fullCovariance());
      W_mat.block(3*NTrk+3,3*NTrk+3,3,3) = vertex.covariancePosition();
      V0_err = D_mat.transpose() * W_mat * D_mat;
    }
    return V0_err(0,1);
  }

  Amg::MatrixX V0Tools::tauMassCovariance(const ExtendedVxCandidate * vxCandidate, Trk::RecVertex& vertex, std::vector<double> masses) const
  {
    // Tau = CONST*M*(Px*dx+Py*dy)/(PT*PT)
    Amg::MatrixX V0_err;
    assert(vxCandidate!=0);
    if(0 == vxCandidate) {
      return V0_err;
    }
    unsigned int NTrk = vxCandidate->vxTrackAtVertex()->size();
    if (masses.size() != NTrk) {
      ATH_MSG_DEBUG("The provided number of masses does not match the number of tracks in the vertex");
      return V0_err;
    }
    double CONST = 1000./CLHEP::c_light;
    double PT = V0Momentum(vxCandidate).perp();
    double dx = (vxCandidate->recVertex().position() - vertex.position()).x();
    double dy = (vxCandidate->recVertex().position() - vertex.position()).y();
    double M = invariantMass(vxCandidate, masses);
    Amg::MatrixX V0_cov(5*NTrk,5*NTrk); V0_cov.setZero();
    double E=0., Px=0., Py=0., Pz=0.; 
    CLHEP::HepVector phi(NTrk,0), theta(NTrk,0), qOverP(NTrk,0), charge(NTrk,0), e(NTrk,0);
    CLHEP::HepVector dpxdqOverP(NTrk,0), dpxdtheta(NTrk,0), dpxdphi(NTrk,0);
    CLHEP::HepVector dpydqOverP(NTrk,0), dpydtheta(NTrk,0), dpydphi(NTrk,0);
    CLHEP::HepVector dpzdqOverP(NTrk,0), dpzdtheta(NTrk,0), dedqOverP(NTrk,0);
    CLHEP::HepVector dMdqOverP(NTrk,0), dMdtheta(NTrk,0), dMdphi(NTrk,0);
    CLHEP::HepVector dPTdqOverP(NTrk,0), dPTdtheta(NTrk,0), dPTdphi(NTrk,0);
    CLHEP::HepVector dLXYdqOverP(NTrk,0), dLXYdtheta(NTrk,0), dLXYdphi(NTrk,0);
    CLHEP::HepVector dTaudqOverP(NTrk,0), dTaudtheta(NTrk,0), dTaudphi(NTrk,0);

    for( unsigned int it=0; it<NTrk; it++) {
      const Trk::TrackParameters*  bPer = (*(vxCandidate->vxTrackAtVertex()))[it]->perigeeAtVertex();
      double trkCharge = 1.;
      if (bPer->parameters()[Trk::qOverP] < 0.) trkCharge = -1.;
      charge[it] = trkCharge;
      phi[it]    = bPer->parameters()[Trk::phi];
      theta[it]  = bPer->parameters()[Trk::theta];
      qOverP[it] = bPer->parameters()[Trk::qOverP];
      double tmp = 1./(qOverP[it]*qOverP[it]) + masses[it]*masses[it];
      double pe = (tmp>0.) ? sqrt(tmp) : 0.;
      e[it] = pe;
      dedqOverP[it]  = -1./(qOverP[it]*qOverP[it]*qOverP[it]*e[it]);
      dpxdqOverP[it] = -(sin(theta[it])*cos(phi[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpxdtheta[it]  =  (cos(theta[it])*cos(phi[it])*charge[it])/qOverP[it];
      dpxdphi[it]    = -(sin(theta[it])*sin(phi[it])*charge[it])/qOverP[it];
      dpydqOverP[it] = -(sin(theta[it])*sin(phi[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpydtheta[it]  =  (cos(theta[it])*sin(phi[it])*charge[it])/qOverP[it];
      dpydphi[it]    =  (sin(theta[it])*cos(phi[it])*charge[it])/qOverP[it];
      dpzdqOverP[it] = -(cos(theta[it])*charge[it])/(qOverP[it]*qOverP[it]);
      dpzdtheta[it]  = -(sin(theta[it])*charge[it])/qOverP[it];
      E  += e[it];
      Px += bPer->momentum()[Trk::px];
      Py += bPer->momentum()[Trk::py];
      Pz += bPer->momentum()[Trk::pz];
      if (vxCandidate->fullCovariance() == 0) V0_cov.block(5*it,5*it,5,5) = *(bPer->covariance());
    }
    double LXY = Px*dx+Py*dy;

    for( unsigned int it=0; it<NTrk; it++) {
      dMdqOverP[it]   = -(Px*dpxdqOverP[it]+Py*dpydqOverP[it]+Pz*dpzdqOverP[it]-E*dedqOverP[it])/M;
      dMdtheta[it]    = -(Px*dpxdtheta[it]+Py*dpydtheta[it]+Pz*dpzdtheta[it])/M;
      dMdphi[it]      = -(Px*dpxdphi[it]+Py*dpydphi[it])/M;
      dPTdqOverP[it]  =  (Px*dpxdqOverP[it]+Py*dpydqOverP[it])/PT;
      dPTdtheta[it]   =  (Px*dpxdtheta[it]+Py*dpydtheta[it])/PT;
      dPTdphi[it]     =  (Px*dpxdphi[it]+Py*dpydphi[it])/PT;
      dLXYdqOverP[it] =  dx*dpxdqOverP[it]+dy*dpydqOverP[it];
      dLXYdtheta[it]  =  dx*dpxdtheta[it]+dy*dpydtheta[it];
      dLXYdphi[it]    =  dx*dpxdphi[it]+dy*dpydphi[it];
      dTaudqOverP[it] =  (LXY*dMdqOverP[it]+M*dLXYdqOverP[it])/(PT*PT)-(2.*LXY*M*dPTdqOverP[it])/(PT*PT*PT);
      dTaudtheta[it]  =  (LXY*dMdtheta[it]+M*dLXYdtheta[it])/(PT*PT)-(2.*LXY*M*dPTdtheta[it])/(PT*PT*PT);
      dTaudphi[it]    =  (LXY*dMdphi[it]+M*dLXYdphi[it])/(PT*PT)-(2.*LXY*M*dPTdphi[it])/(PT*PT*PT);
    }
    double dTaudx = (M*Px)/(PT*PT);
    double dTaudy = (M*Py)/(PT*PT);
    double dTaudx0 = -dTaudx;
    double dTaudy0 = -dTaudy;

    unsigned int ndim = 0;
    if (vxCandidate->fullCovariance() != 0) {
      ndim = (vxCandidate->fullCovariance())->rows();
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
      if (vxCandidate->fullCovariance() != 0) {
        W_mat.block(0,0,ndim,ndim) = *(vxCandidate->fullCovariance());
      } else {
        W_mat.block(0,0,V0_cov.rows(),V0_cov.rows());
        W_mat.block(5*NTrk,5*NTrk,3,3) = vxCandidate->recVertex().covariancePosition();
      }
      W_mat.block(5*NTrk+3,5*NTrk,3,3) = vertex.covariancePosition();
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
      W_mat.block(0,0,ndim,ndim) = (*vxCandidate->fullCovariance());
      W_mat.block(3*NTrk+3,3*NTrk+3,3,3) = vertex.covariancePosition();
      V0_err = D_mat.transpose() * W_mat * D_mat;
    }
    return V0_err;
  }


}//end of namespace definitions
