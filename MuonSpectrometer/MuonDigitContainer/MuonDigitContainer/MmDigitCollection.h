/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MmDigitCollection.h

#ifndef MUONSPECTROMETER_MMDIGITCOLLECTION_H
#define MUONSPECTROMETER_MMDIGITCOLLECTION_H

#include "MuonDigitContainer/MmDigit.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

class MmDigitCollection : public DataVector<MmDigit> 
{

 public:  // functions
  //typedef 
  typedef Identifier ID; 
  typedef MmDigit DIGIT; 
  // Default constructor.
  MmDigitCollection() 
    : DataVector<MmDigit>(),m_id(0),m_idHash(0)
    { };

    // Creates an empty container ready for writing.
    MmDigitCollection(Identifier id,IdentifierHash idHash) 
      : DataVector<MmDigit>(),m_id(id),m_idHash(idHash)
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

CLASS_DEF(MmDigitCollection, 1166674576, 1)

// Class needed only for persistency
typedef DataVector<MmDigitCollection> MmDigitCollection_vector;
CLASS_DEF( MmDigitCollection_vector , 1149160569 , 1 )

#endif
