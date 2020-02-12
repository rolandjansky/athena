/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 *
 * @brief Helper class to describe the kinematics of an object that can have
 * negative energies.
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_SIGNEDKINEMATICS_H
#define TRIGEFMISSINGET_SIGNEDKINEMATICS_H

#include "Math/Vector4D.h"

// Forward declares
class TLorentzVector;

namespace xAOD {
  class IParticle;
}

namespace HLT { namespace MET {

  /**
   * @brief Class to describe the kinematics of an object that can have negative
   * energies.
   *
   * This class is used instead of TLorentzVector anywhere that there could be a
   * negative energy. This is to get complete control of the behaviour in that
   * case as TLorentzVector behaves oddly.
   *
   * Algebraic operators are provided but it's worth noting that negative-energy
   * objects behaviour a little oddly (they are not valid four-momenta after
   * all). This should only really affect the mass.
   *
   * The sign of the vector is defined as sign(E) where E is the scalar sum of
   * all constituents energies
   */
  class SignedKinematics {
    public:
      SignedKinematics();
      /// Constructor from px, py, pz and E
      SignedKinematics(double px, double py, double pz, double energy);
      /// Construct from a TLorentzVector
      SignedKinematics(const TLorentzVector& tlv);
      /// Construct from an IParticle
      SignedKinematics(const xAOD::IParticle& particle);
      /// Factory function to construct from energy, eta, phi (massless)
      static SignedKinematics fromEnergyEtaPhi(
          double energy, double eta, double phi);
      /// Factory function to construct from energy eta, phi and m
      static SignedKinematics fromEnergyEtaPhiM(
          double energy, double eta, double phi, double mass);
      /// Factory function to construct from et, eta, phi (massless)
      static SignedKinematics fromEtEtaPhi(
          double et, double eta, double phi);
      /// Factory function to construct from et eta, phi and m
      static SignedKinematics fromEtEtaPhiM(
          double et, double eta, double phi, double mass);

      /// The sign of the kinematics
      int sign() const;

      /// Direction
      double eta() const;
      double phi() const;
      /// Provide accessors for sin and cos phi
      double sinPhi() const;
      double cosPhi() const;
      /// Provide accessors for sinh and cosh eta
      double sinhEta() const;
      double coshEta() const;

      /// Momentum values
      /// (signed) momentum
      double p() const;
      /// unsigned momentum
      double absP() const;
      double p2() const;
      /// (signed) pt
      double pt() const;
      /// unsigned pt
      double absPt() const;
      double pt2() const;
      double px() const;
      double py() const;
      double pz() const;

      /// Energy values
      /// (signed) energy
      double energy() const;
      /// unsigned energy
      double absEnergy() const;
      double energy2() const;
      /// (signed) et
      double et() const;
      /// Unsigned et
      double absEt() const;
      double et2() const;
      double ex() const;
      double ey() const;
      double ez() const;

      /// The squared mass. There is no guarantee that this will be > 0
      double m2() const;

      explicit operator ROOT::Math::PxPyPzEVector() const;

      /// Add another SignedKinematics to this
      SignedKinematics& operator+=(const SignedKinematics& other);
      /// Subtract a SignedKinematics from this (exact opposite of the above
      /// function.
      SignedKinematics& operator-=(const SignedKinematics& other);
    private:
      /// The actual kinematics
      ROOT::Math::PxPyPzEVector m_p4;
  }; //> end class SignedKinematics
  /// 'free' sum operator
  SignedKinematics operator+(const SignedKinematics& lhs, const SignedKinematics& rhs);
  /// 'free' difference operator
  SignedKinematics operator-(const SignedKinematics& lhs, const SignedKinematics& rhs);

} } //> end namespace HLT::MET
#endif //> !TRIGEFMISSINGET_SIGNEDKINEMATICS_H
