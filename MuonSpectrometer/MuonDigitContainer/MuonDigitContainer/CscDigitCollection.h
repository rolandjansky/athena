/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscDigitCollection.h

#ifndef MUONSPECTROMETER_CSCDIGITCOLLECTION_H
#define MUONSPECTROMETER_CSCDIGITCOLLECTION_H

#include "MuonDigitContainer/CscDigit.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

class CscDigitCollection : public DataVector<CscDigit> 
{

public:  // functions
  //typedef 
  typedef Identifier ID; 
  typedef CscDigit DIGIT; 
  // Default constructor.
  CscDigitCollection() 
    : DataVector<CscDigit>(),m_id(0),m_idHash(0)
  { m_samplingPhase = false; };
  
  // Creates an empty container ready for writing.
  CscDigitCollection(Identifier id, IdentifierHash idHash) 
    : DataVector<CscDigit>(),m_id(id),m_idHash(idHash)
  { m_samplingPhase = false; };
  
  Identifier     identify()          const { return m_id; }
  IdentifierHash identifierHash()    const { return m_idHash; }
  bool           samplingPhase()     const { return m_samplingPhase; }
  void           set_samplingPhase() { m_samplingPhase = true; }
  private:
    Identifier     m_id; 
    IdentifierHash m_idHash; 
    bool m_samplingPhase;
};

CLASS_DEF(CscDigitCollection, 4198, 1)

// Class needed only for persistency
typedef DataVector<CscDigitCollection> CscDigitCollection_vector;
CLASS_DEF( CscDigitCollection_vector , 1282354891 , 1 )

#endif
