/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDPVM_safeDecorator_h
#define IDPVM_safeDecorator_h
/**
 * @file safeDecorator.h
 * header file for function of same name
 * @author shaun roe
 * @date 20 July 2016
 */



#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

#include <iostream>


namespace IDPVM {
  // @name roughlyEqual overloaded functions
  // @{
  bool roughlyEqual(const int i, const int j);
  bool roughlyEqual(const unsigned int i, const unsigned int j);
  bool roughlyEqual(const float i, const float j);
  // @}
  // @enum DuplicateBehaviour
  ///Behaviour in case of trying to add a decoration which previously exists
  enum DuplicateBehaviour {
    DO_NOTHING, REJECT_QUIETLY, REJECT_WITH_WARNING, REJECT_WARN_IF_UNEQUAL
  };

  /// Safe decorator for TrackParticles
  template <class T>
  void
  safeDecorator(const xAOD::TrackParticle& particle, const std::string& decorName, const T& value,
                const DuplicateBehaviour duplicate = REJECT_WITH_WARNING) {
    switch (duplicate) {
    //
    case REJECT_WITH_WARNING:
      if (particle.template isAvailable<T>(decorName)) {
        std::cout << "WARNING IDPVM::safeDecorator: " << decorName <<
          " Already exists on this object; update rejected.\n";
      } else {
        particle.template auxdecor<T>(decorName) = value;
      }
      break;

    //
    case REJECT_WARN_IF_UNEQUAL:
      if (particle.template isAvailable<T>(decorName)) {
        const T existing = particle.template auxdata<T>(decorName);
        if (not IDPVM::roughlyEqual(existing, value)) {
          std::cout << "WARNING IDPVM::safeDecorator: " << decorName <<
            " Already exists on this object with a different value.\n";
        }
      } else {
        particle.template auxdecor<T>(decorName) = value;
      }
      break;

    //
    case REJECT_QUIETLY:
      if (not (particle.template isAvailable<T>(decorName))) {
        particle.template auxdecor<T>(decorName) = value;
      }
      break;

    default: // corresponds to DO_NOTHING
      particle.template auxdecor<T>(decorName) = value;
      break;
    }
  }

  /// Safe decorator for TruthParticles
  template <class T>
  void
  safeDecorator(const xAOD::TruthParticle& particle, const std::string& decorName, const T& value,
                const DuplicateBehaviour duplicate = REJECT_WITH_WARNING) {
    switch (duplicate) {
    //
    case REJECT_WITH_WARNING:
      if (particle.template isAvailable<T>(decorName)) {
        std::cout << "WARNING IDPVM::safeDecorator: " << decorName << " Already exists on this object.\n";
      } else {
        particle.template auxdecor<T>(decorName) = value;
      }
      break;

    //
    case REJECT_WARN_IF_UNEQUAL:
      if (particle.template isAvailable<T>(decorName)) {
        const T existing = particle.template auxdata<T>(decorName);
        if (not IDPVM::roughlyEqual(existing, value)) {
          std::cout << "WARNING IDPVM::safeDecorator: " << decorName <<
            " Already exists on this object with a different value.\n";
        }
      } else {
        particle.template auxdecor<T>(decorName) = value;
      }
      break;

    //
    default: // corresponds to DO_NOTHING
      particle.template auxdecor<T>(decorName) = value;
      break;
    }
  }
}
#endif
