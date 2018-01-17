/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// sTgcDigitCollection.h

#ifndef MUONSPECTROMETER_STGCDIGITCOLLECTION_H
#define MUONSPECTROMETER_STGCDIGITCOLLECTION_H

#include "MuonDigitContainer/sTgcDigit.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

class sTgcDigitCollection : public DataVector<sTgcDigit> 
{

  public:  // functions
  
  typedef Identifier ID; 
  typedef sTgcDigit DIGIT; 
  // Default constructor.
  sTgcDigitCollection() 
    : DataVector<sTgcDigit>(),m_id(0),m_idHash(0)
  { }

  // Creates an empty container ready for writing.
  sTgcDigitCollection(Identifier id,IdentifierHash idHash) 
    : DataVector<sTgcDigit>(),m_id(id),m_idHash(idHash)
  { }

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

CLASS_DEF(sTgcDigitCollection, 1084450701, 1)

// Class needed only for persistency
typedef DataVector<sTgcDigitCollection> sTgcDigitCollection_vector;
CLASS_DEF( sTgcDigitCollection_vector , 1257206222 , 1 )

#endif
