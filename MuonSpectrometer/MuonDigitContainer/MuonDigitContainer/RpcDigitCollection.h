/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RpcDigitCollection.h

#ifndef MUONSPECTROMETER_RPCDIGITCOLLECTION_H
#define MUONSPECTROMETER_RPCDIGITCOLLECTION_H

#include "MuonDigitContainer/RpcDigit.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

class RpcDigitCollection : public DataVector<RpcDigit> 
{

public:  // functions
  //typedef 
  typedef Identifier ID; 
  typedef RpcDigit DIGIT; 
  // Default constructor.
  RpcDigitCollection() 
    : DataVector<RpcDigit>(),m_id(0),m_idHash(0)
	{ };

  // Creates an empty container ready for writing.
  RpcDigitCollection(Identifier id, IdentifierHash idHash)
    : DataVector<RpcDigit>(),m_id(id), m_idHash(idHash)
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

CLASS_DEF(RpcDigitCollection, 4196, 1)

// Class needed only for persistency
typedef DataVector<RpcDigitCollection> RpcDigitCollection_vector;
CLASS_DEF( RpcDigitCollection_vector , 1267635735 , 1 )

#endif
