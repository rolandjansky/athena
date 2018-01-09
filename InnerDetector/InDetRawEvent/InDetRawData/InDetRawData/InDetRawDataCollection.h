/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetRawDataCollection.h
//   Header file for class InDetRawDataCollection
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to contain all the RawDataObject of a detector element:
// module for Pixel or wafer for SCT or layer for TRT
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef INDETRAWDATA_INDETRAWDATACOLLECTION_H
#define INDETRAWDATA_INDETRAWDATACOLLECTION_H

// Base classes
#include "Identifier/Identifiable.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "AthContainers/DataVector.h"


// Needed class
#include "InDetRawData/InDetRawData.h"

template< class RawDataT >
class InDetRawDataCollection : public Identifiable, public DataVector< RawDataT > {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  //   Hashed offline identifier of the DE
  InDetRawDataCollection(const IdentifierHash idHash);

  // Destructor:
  virtual ~InDetRawDataCollection();


  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  virtual Identifier identify() const override final;

  virtual IdentifierHash identifyHash() const override final;

  void setIdentifier(Identifier id);

  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:

  InDetRawDataCollection(const InDetRawDataCollection&);
  InDetRawDataCollection &operator=(const InDetRawDataCollection&);

public:
  // public default constructor needed for I/O, but should not be
  // called from an alg
  InDetRawDataCollection();

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:
  const IdentifierHash m_idHash;
  Identifier m_id; // identifier of the DE
};

/**Overload of << operator for MsgStream for debug output*/
template< class RawDataT >
MsgStream& operator << ( MsgStream& sl, const InDetRawDataCollection< RawDataT >& coll);

/**Overload of << operator for std::ostream for debug output*/ 
template< class RawDataT >
std::ostream& operator << ( std::ostream& sl, const InDetRawDataCollection< RawDataT >& coll);

// member functions that use Collection T
#include"InDetRawData/InDetRawDataCollection.icc"
///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

#endif // INDETRAWDATA_INDETRAWDATACOLLECTION_H
