// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_TRUTHACCESSORS_V1_H
#define XAOD_TRUTHACCESSORS_V1_H

#include "AthContainers/AuxElement.h"
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthParticle.h"
#include <iostream>

/// Helper macro for managing cluster moment Accessor objects
#define DEFINE_ACCESSOR(PARENT, TYPE, NAME )            \
  case PARENT :: NAME:                       \
  {                                                     \
    static SG::AuxElement::Accessor< TYPE > a( #NAME ); \
    return &a;                                          \
  }                                                     \
  break;

#define DEFINE_TEACCESSOR(TYPE, NAME) DEFINE_ACCESSOR(xAOD::TruthEvent_v1, TYPE, NAME)
#define DEFINE_TPACCESSOR(TYPE, NAME) DEFINE_ACCESSOR(xAOD::TruthParticle_v1, TYPE, NAME)


namespace xAOD {


  // Helper functions for managing Truth Accessor objects

  /// This function holds on to Accessor objects that can be used by each
  /// TruthEvent_v1 object at runtime to get/set parameter values on themselves.
  template <class T>
  inline SG::AuxElement::Accessor<T>*
  pdfInfoAccessorV1( TruthEvent_v1::PdfParam type ) {  }

  /// This function holds on to Accessor objects that can be used by each
  /// TruthEvent_v1 object at runtime to get/set parameter values on themselves.
  template <class T>
  inline SG::AuxElement::Accessor<T>*
  heavyIonAccessorV1( TruthEvent_v1::HIParam type ) { }

  /// This function holds on to Accessor objects that can be used by each
  /// TruthParticle_v1 object at runtime to get/set parameter values on themselves.
  inline SG::AuxElement::Accessor<float>*
  polarizationAccessorV1( TruthParticle_v1::PolParam type ) {
    switch( type ) {
      DEFINE_TPACCESSOR(float, THETA);
      DEFINE_TPACCESSOR(float, PHI);
    default:
      std::cerr << "xAOD::TruthParticle_v1::polarizationAccessorV1 ERROR Unknown PolParam (" << type << ") requested" << std::endl;
      return 0;
    }
  }


  // /// @todo Generic case. Maybe return warning? Or use SFINAE to force only supported types
  // template<class T>
  // SG::AuxElement::Accessor< T >*
  // pdfInfoAccessorV1( TruthEvent_v1::PdfParam /*type*/ )
  // { }

  // Int specialization
  template<>
  inline SG::AuxElement::Accessor< int >*
  pdfInfoAccessorV1<int>( TruthEvent_v1::PdfParam type ) {
    switch ( type ) {
      DEFINE_TEACCESSOR(int, PDGID1);
      DEFINE_TEACCESSOR(int, PDGID2);
      DEFINE_TEACCESSOR(int, PDFID1);
      DEFINE_TEACCESSOR(int, PDFID2);
    default:
      std::cerr << "xAOD::TruthEvent_v1::pdfInfoAccessorV1 ERROR Unknown PdfParam (" << type << ") requested" << std::endl;
      return 0;
    }
  }

  // Float specialization
  template<>
  inline SG::AuxElement::Accessor< float >*
  pdfInfoAccessorV1<float>( TruthEvent_v1::PdfParam type ) {
    switch( type ) {
      DEFINE_TEACCESSOR(float, X1);
      DEFINE_TEACCESSOR(float, X2);
      DEFINE_TEACCESSOR(float, Q);
      DEFINE_TEACCESSOR(float, XF1);
      DEFINE_TEACCESSOR(float, XF2);
    default:
      std::cerr << "xAOD::TruthEvent_v1::pdfInfoAccessorV1 ERROR Unknown ParamDef (" << type << ") requested" << std::endl;
      return 0;
    }
  }



  // /// @todo Generic case. Maybe return warning? Or use SFINAE to force only supported types
  // template<class T>
  // SG::AuxElement::Accessor< T >*
  // heavyIonAccessorV1( TruthEvent_v1::HIParam /*type*/ )
  // { }

  // Int specialization
  template<>
  inline SG::AuxElement::Accessor< int >*
  heavyIonAccessorV1<int>( TruthEvent_v1::HIParam type ) {
    switch ( type ) {
      DEFINE_TEACCESSOR(int, NCOLLHARD);
      DEFINE_TEACCESSOR(int, NPARTPROJ);
      DEFINE_TEACCESSOR(int, NPARTTARG);
      DEFINE_TEACCESSOR(int, NCOLL);
      DEFINE_TEACCESSOR(int, SPECTATORNEUTRONS);
      DEFINE_TEACCESSOR(int, SPECTATORPROTONS);
      DEFINE_TEACCESSOR(int, NNWOUNDEDCOLLISIONS);
      DEFINE_TEACCESSOR(int, NWOUNDEDNCOLLISIONS);
      DEFINE_TEACCESSOR(int, NWOUNDEDNWOUNDEDCOLLISIONS);
    default:
      std::cerr << "xAOD::TruthEvent_v1::heavyIonAccessorV1 ERROR Unknown HIParam (" << type << ") requested" << std::endl;
      return 0;
    }
  }

  // Float specialization
  template<>
  inline SG::AuxElement::Accessor< float >*
  heavyIonAccessorV1<float>( TruthEvent_v1::HIParam type ) {
    switch( type ) {
      DEFINE_TEACCESSOR(float, IMPACTPARAMETER);
      DEFINE_TEACCESSOR(float, EVENTPLANEANGLE);
      DEFINE_TEACCESSOR(float, ECCENTRICITY);
      DEFINE_TEACCESSOR(float, SIGMAINELNN);
      DEFINE_TEACCESSOR(float, CENTRALITY);
    default:
      std::cerr << "xAOD::TruthEvent_v1::heavyIonAccessorV1 ERROR Unknown HIParam (" << type << ") requested" << std::endl;
      return 0;
    }
  }



  // // Float only
  // SG::AuxElement::Accessor< float >*
  // polarizationAccessorV1( TruthParticle_v1::PolParam type ) {
  //   switch( type ) {
  //     DEFINE_TPACCESSOR(float, THETA);
  //     DEFINE_TPACCESSOR(float, PHI);
  //   default:
  //     std::cerr << "xAOD::TruthParticle_v1::polarizationAccessorV1 ERROR Unknown PolParam (" << type << ") requested" << std::endl;
  //     return 0;
  //   }
  // }


} // namespace xAOD

#endif
