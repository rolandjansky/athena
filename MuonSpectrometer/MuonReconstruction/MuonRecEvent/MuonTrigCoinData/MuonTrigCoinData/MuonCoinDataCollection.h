/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CoinDataCollection.h
//   Header file for class TrigCoinDataCollection
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONTRIGCOINDATA_MUONCOINDATACOLLECTION_H
#define MUONTRIGCOINDATA_MUONCOINDATACOLLECTION_H

// Base classes
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "AthContainers/DataVector.h"

#include "CLIDSvc/CLASS_DEF.h"

namespace Muon{

template< class CoinDataT >
class MuonCoinDataCollection : public DataVector< CoinDataT > {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  //   Hashed offline identifier of the DE
  MuonCoinDataCollection(const IdentifierHash idHash);

 /** Default Constructor (for persistency)*/
  MuonCoinDataCollection();
                     
  /** Destructor:*/
  virtual ~MuonCoinDataCollection();


  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  // typedef needed for IdentifiableContainer base class
  typedef Identifier ID;

  // identifier of this detector element:
  Identifier identify() const;

  IdentifierHash identifyHash() const;

  void setIdentifier(Identifier id);
  // plottable
  virtual std::string type() const;


  // typedef needed for IdentifiableContainer base class
//  typedef TrigCoinData DIGIT;

  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:

  MuonCoinDataCollection(const MuonCoinDataCollection&);
  MuonCoinDataCollection &operator=(const MuonCoinDataCollection&);

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:
  const IdentifierHash m_idHash; 
  Identifier m_id; // identifier of the DE
  

};
// member functions that use Collection T
#include "MuonTrigCoinData/MuonCoinDataCollection.icc"
///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

}

#endif // MUONTRIGCOINDATA_MUONCOINDATACOLLECTION_H

