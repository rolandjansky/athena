/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TgcDigitCollection.h

#ifndef MUONSPECTROMETER_TGCDIGITCOLLECTION_H
#define MUONSPECTROMETER_TGCDIGITCOLLECTION_H

#include "MuonDigitContainer/TgcDigit.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

class TgcDigitCollection : public DataVector<TgcDigit> 
{

public:  // functions
  //typedef 
  typedef Identifier ID; 
  typedef TgcDigit DIGIT; 
  // Default constructor.
  TgcDigitCollection() 
    : DataVector<TgcDigit>(),m_id(0),m_idHash(0)
	{ };

  // Creates an empty container ready for writing.
  TgcDigitCollection(Identifier id, IdentifierHash idHash) 
    : DataVector<TgcDigit>(),m_id(id),m_idHash(idHash)
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

CLASS_DEF(TgcDigitCollection, 4197, 1)

// Class needed only for persistency
typedef DataVector<TgcDigitCollection> TgcDigitCollection_vector;
CLASS_DEF( TgcDigitCollection_vector , 1095153488 , 1 )

#endif
