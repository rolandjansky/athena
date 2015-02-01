/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODHIEvent/versions/HIEventShape_v1.h"
#include "xAODHIEvent/versions/HIEventShapeContainer_v1.h"
#include "xAODCore/AuxStoreAccessorMacros.h"



namespace xAOD {

  HIEventShape_v1::HIEventShape_v1()
    : SG::AuxElement() {
     
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( HIEventShape_v1, float, Et, setEt )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( HIEventShape_v1, float, area, setArea )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( HIEventShape_v1, float, rho, setRho )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( HIEventShape_v1, int, nCells, setNcells )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( HIEventShape_v1, std::vector<float>, Et_cos, setEt_cos )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( HIEventShape_v1, std::vector<float>, Et_sin, setEt_sin )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( HIEventShape_v1, float, etaMin, setEtaMin )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( HIEventShape_v1, float, etaMax, setEtaMax )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( HIEventShape_v1, int, layer, setLayer )


  static SG::AuxElement::Accessor< std::vector< ElementLink< HIEventShapeContainer_v1 > > >
  linkAcc( "HIsubEventLink" );
  void HIEventShape_v1::toPersistent() {

    // Check if the ElementLink variable is available, and writable:
    if( linkAcc.isAvailableWritable( *this ) ) {
      // Prepare the ElementLink objects for ROOT I/O:
      std::vector< ElementLink< HIEventShapeContainer_v1 > >::iterator
	el_itr = linkAcc( *this ).begin();
      std::vector< ElementLink< HIEventShapeContainer_v1 > >::iterator
	el_end = linkAcc( *this ).end();
      for( ; el_itr != el_end; ++el_itr ) {
	el_itr->toPersistent();
      }
    }

    return;
  }



} // namespace xAOD

