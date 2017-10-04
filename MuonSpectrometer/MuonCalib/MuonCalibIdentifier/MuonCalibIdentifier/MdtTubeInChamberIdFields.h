/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Identifier hashes
 * -----------------------------------------
 *
 * Author       : Martin Woudstra
 * Creation Date: 28 April 2004
 * Last Update  : 28 April 2005
 ***************************************************************************/

/** Helper class implementation to instantiate the templated IdentifierHashTable class.
    The template parameter T is an external IdentifierHelper class to encode/decode
    the fields in the identifier. */

#ifndef MUONCALIBIDENTIFIER_MDTTUBEINCHAMBERFIELDS_H
# define MUONCALIBIDENTIFIER_MDTTUBEINCHAMBERFIELDS_H
// std

// other packages
// this package
#include "MuonCalibIdentifier/MdtFixedIds.h"
#include "MuonCalibIdentifier/MdtFixedHashes.h"
#include "MuonCalibIdentifier/IdHashTableHelper.h"

class MdtTubeInChamberIdFields : public IdHashTableHelper<3,MdtTubeId,MdtTubeHash> {
public:
   /** define type BaseClasse */
   typedef IdHashTableHelper<3,MdtTubeId,MdtTubeHash> BaseClass;
   /** enum the identifier fiels */
   enum {  kTubeField = 0, kLayerField = 1, kMultilayerField = 2 };
   /** Set identifier and also the fields (using the internal helper). */
   void setAll( const MdtTubeId& id );
   /** Set fields and also the identifier (using the internal helper) */
   void setAll( int fields[3] );
private:
};

inline void MdtTubeInChamberIdFields::setAll( int fields[3] ) {
   BaseClass::setFields( fields );
   MdtTubeId id( fields[kMultilayerField], fields[kLayerField], fields[kTubeField] );
   BaseClass::setId( id );
}

inline void MdtTubeInChamberIdFields::setAll( const MdtTubeId& id ) {
   BaseClass::setId( id );
   BaseClass::setField( kMultilayerField, id.multilayer() );
   BaseClass::setField( kLayerField,      id.layer()      );
   BaseClass::setField( kTubeField,       id.tube()       );
}



#endif // MUONCALIBIDENTIFIER_MDTTUBEINCHAMBERFIELDS_H
