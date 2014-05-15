/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MdtDigitCollection.h

#ifndef MUONSPECTROMETER_MDTDIGITCOLLECTION_H
#define MUONSPECTROMETER_MDTDIGITCOLLECTION_H

#include "MuonDigitContainer/MdtDigit.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

class MdtDigitCollection : public DataVector<MdtDigit> 
{

public:  // functions
  //typedef 
  typedef Identifier ID; 
  typedef MdtDigit DIGIT; 
  // Default constructor.
  MdtDigitCollection() 
    : DataVector<MdtDigit>(),m_id(0),m_idHash(0)
	{ };

  // Creates an empty container ready for writing.
  MdtDigitCollection(Identifier id,IdentifierHash idHash) 
    : DataVector<MdtDigit>(),m_id(id),m_idHash(idHash)
	{ };

  Identifier identify() const
    {
	return m_id;
    }

  IdentifierHash identifierHash() const
    {
	return m_idHash;
    }

  private:
    Identifier     m_id; 
    IdentifierHash m_idHash; 

};

CLASS_DEF(MdtDigitCollection, 4195, 1)

// Class needed only for persistency
typedef DataVector<MdtDigitCollection> MdtDigitCollection_vector;
CLASS_DEF( MdtDigitCollection_vector , 1163423195 , 1 )

#endif
