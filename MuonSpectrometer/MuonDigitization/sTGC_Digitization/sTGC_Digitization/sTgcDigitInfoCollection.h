/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSPECTROMETER_STGCDIGITINFOCOLLECTION_H
#define MUONSPECTROMETER_STGCDIGITINFOCOLLECTION_H

#include "sTGC_Digitization/sTgcDigitInfo.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

class sTgcDigitInfoCollection : public DataVector<sTgcDigitInfo>
{

 public:  // functions

  typedef Identifier ID;
  typedef sTgcDigitInfo DIGITINFO;
  // Default constructor.
  sTgcDigitInfoCollection()
    : DataVector<sTgcDigitInfo>(),m_id(0),m_idHash(0)
    { };

    // Creates an empty container ready for writing.
    sTgcDigitInfoCollection(Identifier id,IdentifierHash idHash)
      : DataVector<sTgcDigitInfo>(),m_id(id),m_idHash(idHash)
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

CLASS_DEF(sTgcDigitInfoCollection, 1084451812, 1)

// Class needed only for persistency
typedef DataVector<sTgcDigitInfoCollection> sTgcDigitInfoCollection_vector;
CLASS_DEF( sTgcDigitInfoCollection_vector , 1257207333 , 1 )

#endif

