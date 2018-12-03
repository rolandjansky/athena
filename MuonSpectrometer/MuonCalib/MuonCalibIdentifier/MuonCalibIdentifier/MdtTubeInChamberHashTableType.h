/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Muon Calibration Identifiers
 * -----------------------------------------
 *
 * Author       : Martin Woudstra
 * Creation Date: 09 May 2005
 * Last Update  : 09 May 2005
 ***************************************************************************/

/** holds the MDT chamber type information in the form of a MdtChamberTypeInfo 
 * data member. Is is used by MdtHashTable. 
 */    

#ifndef MUONCALIBIDENTIFIER_MDTTUBEINCHAMBERHASHTABLETYPE_H
# define MUONCALIBIDENTIFIER_MDTTUBEINCHAMBERHASHTABLETYPE_H
// std

// other packages

// this package
#include "MuonCalibIdentifier/MdtChamberTypeInfo.h"
#include "MuonCalibIdentifier/IdentifierHashCalc.h"
#include "MuonCalibIdentifier/MdtTubeInChamberIdFields.h"

class MdtTubeInChamberHashTableType :
   public IdentifierHashCalc<MdtTubeInChamberIdFields> {
public:
   /** define type BaseClass */
   typedef IdentifierHashCalc<MdtTubeInChamberIdFields> BaseClass;
   /** constructor */
   explicit MdtTubeInChamberHashTableType( const MdtChamberTypeInfo& type );
   /** @return true if chambertype is equal (=number of multilayers, layers and tubes) */
   bool operator==( const MdtTubeInChamberHashTableType& ) const;
   /** @return true if chambertype is equal (=number of multilayers, layers and tubes) */
   bool operator==( const MdtChamberTypeInfo& ) const;
   /** Return the number of multilayers in the chamber */
   unsigned int nMultilayers() const;
   /** Return the number of layers per multilayer */
   unsigned int nLayers() const;
   /** Return the number of tubes per layer */
   unsigned int nTubes() const;
   /** Return the total number of tubes in the chamber */
   unsigned int nTotalTubes() const;
 private:
   MdtChamberTypeInfo m_chamberType;
   
};

inline MdtTubeInChamberHashTableType::MdtTubeInChamberHashTableType(
   const MdtChamberTypeInfo& type )
   : m_chamberType( type )
{
   BaseClass::setFieldLimits( MdtTubeInChamberIdFields::kMultilayerField,
			      MdtTubeId::multilayerMin(),
			      MdtTubeId::multilayerMin() + m_chamberType.nMultilayers() - 1 );
   BaseClass::setFieldLimits( MdtTubeInChamberIdFields::kLayerField,
			      MdtTubeId::layerMin(),
			      MdtTubeId::layerMin() + m_chamberType.nLayers() - 1 ); 
   BaseClass::setFieldLimits( MdtTubeInChamberIdFields::kTubeField,
			      MdtTubeId::tubeMin(),
			      MdtTubeId::tubeMin() + m_chamberType.nTubes() - 1 );
}

inline unsigned int MdtTubeInChamberHashTableType::nMultilayers() const {
   return m_chamberType.nMultilayers();
}

inline bool
MdtTubeInChamberHashTableType::operator==( const MdtTubeInChamberHashTableType& rhs ) const {
   return m_chamberType == rhs.m_chamberType;
}

inline bool MdtTubeInChamberHashTableType::operator==( const MdtChamberTypeInfo& rhs ) const {
   return m_chamberType == rhs;
}

inline unsigned int MdtTubeInChamberHashTableType::nLayers() const {
   return m_chamberType.nLayers();
}

inline unsigned int MdtTubeInChamberHashTableType::nTubes() const {
   return m_chamberType.nTubes();
}

inline unsigned int MdtTubeInChamberHashTableType::nTotalTubes() const {
   return m_chamberType.nTotalTubes();
}




#endif // MUONCALIBIDENTIFIER_MDTTUBEINCHAMBERHASHTABLETYPE_H
