/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TrigEFMissingET/SignedKinematics.h"
#include <TLorentzVector.h>
#include "xAODBase/IParticle.h"
#include <TVector2.h>

namespace HLT { namespace MET {
  SignedKinematics::SignedKinematics() {}
  SignedKinematics::SignedKinematics(
      double px, double py, double pz, double energy) :
    m_p4(px, py, pz, energy)
  {}

  SignedKinematics::SignedKinematics(const TLorentzVector& tlv) :
    m_p4(tlv.Px(), tlv.Py(), tlv.Pz(), tlv.E() )
  {}

  SignedKinematics::SignedKinematics(const xAOD::IParticle& particle) :
    SignedKinematics(fromEnergyEtaPhiM(
          particle.e(), particle.eta(), particle.phi(), particle.m() ) )
  {}

  SignedKinematics SignedKinematics::fromEnergyEtaPhi(
      double energy, double eta, double phi)
  {
    return fromEnergyEtaPhiM(energy, eta, phi, 0.);
  }

  SignedKinematics SignedKinematics::fromEnergyEtaPhiM(
      double energy, double eta, double phi, double mass)
  {
    double p = energy;
    if (mass != 0) {
      int sgn = (p > 0) - (p < 0);
      p = sgn * sqrt(p*p - mass*mass);
    }
    double pt = p/std::cosh(eta);
    double pz = pt*std::sinh(eta);
    return SignedKinematics(
        pt*std::cos(phi), pt*std::sin(phi), pz, energy);
  }

  SignedKinematics SignedKinematics::fromEtEtaPhi(
      double et, double eta, double phi)
  {
    return fromEtEtaPhiM(et, eta, phi, 0.);
  }

  SignedKinematics SignedKinematics::fromEtEtaPhiM(
      double et, double eta, double phi, double mass)
  {
    return fromEnergyEtaPhiM(et*std::cosh(eta), eta, phi, mass);
  }

  int SignedKinematics::sign() const {
    return (energy() > 0) - (energy() < 0);
  }

  double SignedKinematics::eta() const {
    return sign() * m_p4.Eta();
  }

  double SignedKinematics::phi() const {
    double val = m_p4.Phi();
    if (sign() < 0)
      val += TMath::Pi();
    return TVector2::Phi_0_2pi(val);
  }

  double SignedKinematics::sinPhi() const {
    double thePt = pt();
    // if pt() is 0 then the value is not determined.
    // For this phi = 0
    return (thePt == 0 ? 0 : py() / thePt );
  }
  double SignedKinematics::cosPhi() const {
    double thePt = pt();
    // if pt() is 0 then the value is not determined.
    // Take phi = 0
    return (thePt == 0 ? 1 : px() / thePt );
  }

  double SignedKinematics::sinhEta() const {
    double thePt = pt();
    // if pt is 0 then the value is not determined.
    // Take eta = 0
    return (thePt == 0 ? 0 : pz() / thePt );
  }
  double SignedKinematics::coshEta() const {
    double thePt2 = pt2();
    // if pt is 0 then the value is not determined.
    // Take eta = 0
    if (thePt2 == 0)
      return 1;
    // Otherwise, calculate sinh^2 eta, then use 
    // cosh eta = sqrt(1 + sinh^2 eta)
    return sqrt(1 + pz()*pz() / thePt2);
  }

  double SignedKinematics::p() const {
    return sign() * absP();
  }
  double SignedKinematics::absP() const {
    return sqrt(p2() );
  }
  double SignedKinematics::p2() const {
    return pt2() + pz()*pz();
  }
  double SignedKinematics::pt() const {
    return sign() * absPt();
  }
  double SignedKinematics::absPt() const {
    return sqrt(pt2() );
  }
  double SignedKinematics::pt2() const {
    return px()*px() + py()*py();
  }
  double SignedKinematics::px() const {
    return m_p4.Px();
  }
  double SignedKinematics::py() const {
    return m_p4.Py();
  }
  double SignedKinematics::pz() const {
    return m_p4.Pz();
  }

  double SignedKinematics::energy() const {
    return m_p4.E();
  }
  double SignedKinematics::absEnergy() const {
    return abs(energy() );
  }
  double SignedKinematics::energy2() const {
    return energy()*energy();
  }
  double SignedKinematics::et() const {
    return energy() / coshEta();
  }
  double SignedKinematics::absEt() const {
    return abs(et() );
  }
  double SignedKinematics::et2() const {
    return et()*et();
  }
  double SignedKinematics::ex() const {
    return et() * cosPhi();
  }
  double SignedKinematics::ey() const {
    return et() * sinPhi();
  }
  double SignedKinematics::ez() const {
    return et() * sinhEta();
  }

  double SignedKinematics::m2() const {
    return energy2() - p2();
  }

  SignedKinematics::operator ROOT::Math::PxPyPzEVector() const {
    return m_p4;
  }

  SignedKinematics& SignedKinematics::operator+=(const SignedKinematics& other)
  {
    m_p4.SetPx(px()+other.px() );
    m_p4.SetPy(py()+other.py() );
    m_p4.SetPz(pz()+other.pz() );
    m_p4.SetE(energy()+other.energy() );
    return *this;
  }
  SignedKinematics& SignedKinematics::operator-=(const SignedKinematics& other)
  {
    m_p4.SetPx(px()-other.px() );
    m_p4.SetPy(py()-other.py() );
    m_p4.SetPz(pz()-other.pz() );
    m_p4.SetE(energy()-other.energy() );
    return *this;
  }

  SignedKinematics operator+(const SignedKinematics& lhs, const SignedKinematics& rhs)
  {
    SignedKinematics val(lhs);
    val += rhs;
    return val;
  }
  SignedKinematics operator-(const SignedKinematics& lhs, const SignedKinematics& rhs)
  {
    SignedKinematics val(lhs);
    val -= rhs;
    return val;
  }
} } //> end namespace HLT::MET
