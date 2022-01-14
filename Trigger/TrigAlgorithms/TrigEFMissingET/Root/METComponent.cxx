/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TrigEFMissingET/METComponent.h"
#include <cmath>

namespace HLT { namespace MET {
  METComponent::METComponent(const xAOD::TrigMissingET& met) :
    mpx(met.ex() ),
    mpy(met.ey() ),
    mpz(met.ez() ),
    sumE(met.sumE() ),
    sumEt(met.sumEt() ),
    status(met.flag() )
  {}

  METComponent::METComponent(
      std::size_t idx, const xAOD::TrigMissingET& met) :
    mpx(met.exComponent(idx) ),
    mpy(met.eyComponent(idx) ),
    mpz(met.ezComponent(idx) ),
    sumE(met.sumEComponent(idx) ),
    sumEt(met.sumEtComponent(idx) ),
    status(met.statusComponent(idx) )
  {}

  float METComponent::met() const {
    return sqrt(mpx*mpx+mpy*mpy);
  }

  float METComponent::magnitude() const {
    return sqrt(mpx*mpx+mpy*mpy+mpz*mpz);
  }

  float METComponent::phi() const {
    if (mpy == 0 && mpx == 0)
      return 0;
    return std::atan2(mpy, mpx);
  }

  float METComponent::eta() const {
    if (met() == 0)
      return 0;
    return std::asinh(mpz/met() );
  }

  METComponent operator+(
      const METComponent& lhs,
      const METComponent& rhs)
  {
    METComponent ret(lhs);
    ret += rhs;
    return ret;
  }

  METComponent& METComponent::operator+=(
      const METComponent& other)
  {
    mpx    += other.mpx;
    mpy    += other.mpy;
    mpz    += other.mpz;
    sumE   += other.sumE;
    sumEt  += other.sumEt;
    status |= other.status;
    return *this;
  }

  METComponent& METComponent::operator+=(
      const TLorentzVector& otherP4)
  {
    mpx   -= otherP4.Px();
    mpy   -= otherP4.Py();
    mpz   -= otherP4.Pz();
    sumE  += otherP4.E();
    sumEt += otherP4.Pt();
    return *this;
  }

  METComponent& METComponent::operator+=(
      const SignedKinematics& kin)
  {
    mpx   -= kin.px();
    mpy   -= kin.py();
    mpz   -= kin.pz();
    sumE  += kin.energy();
    sumEt += kin.pt();
    return *this;
  }

  void METComponent::fillMET(xAOD::TrigMissingET& met) const
  {
    met.setEx(mpx);
    met.setEy(mpy);
    met.setEz(mpz);
    met.setSumE(sumE);
    met.setSumEt(sumEt);
    met.setFlag(status);
  }

  void METComponent::fillMETComponent(
      std::size_t ii, xAOD::TrigMissingET& met) const
  {
    met.setExComponent(ii, mpx);
    met.setEyComponent(ii, mpy);
    met.setEzComponent(ii, mpy);
    met.setSumEComponent(ii, sumE);
    met.setSumEtComponent(ii, sumEt);
    met.setStatusComponent(ii, status);
  }

  std::ostream& operator<<(std::ostream& os, const METComponent& component)
  {
    os << "(mpx, mpy, mpz, met, sumEt, sumE) = (" 
      << component.mpx << ", "
      << component.mpy << ", "
      << component.mpz << ", "
      << component.met() << ", "
      << component.sumEt << ", "
      << component.sumE << ") [MeV]";
    return os;
  }
} } //> end namespace HLT::MET
