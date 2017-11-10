/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PrepRawDataCollection.h
//   Header file for class PrepRawDataCollection
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to contain all the RIOs of a detector element:
// module for Pixel or wafer for SCT or layer for TRT, 
// for Muons:
///////////////////////////////////////////////////////////////////
// Version 1.0 15/07/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef MUONPREPRAWDATA_MUONPREPDATACOLLECTION_H
#define MUONPREPRAWDATA_MUONPREPDATACOLLECTION_H

// Base classes
#include "Identifier/Identifiable.h"
#include "Identifier/IdentifierHash.h"
#include "AthContainers/DataVector.h"


// Needed class
#include "TrkPrepRawData/PrepRawData.h"

namespace Muon{

/**@brief Template to hold collections of MuonPrepRawData objects. 
The collections must each have an IdentifierHash, which are used to identify them*/
template< class PrepDataT >
class MuonPrepDataCollection : public Identifiable,
                 public DataVector< PrepDataT > {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  //   Hashed offline identifier of the DE
  MuonPrepDataCollection(const IdentifierHash idHash);

 /** Default Constructor (for persistency)*/
  MuonPrepDataCollection();
                     
  /** Destructor:*/
  virtual ~MuonPrepDataCollection();


  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  // typedef needed for IdentifiableContainer base class
  typedef Identifier ID;

  // identifier of this detector element:
  virtual Identifier identify() const override final;

  virtual IdentifierHash identifyHash() const override final;

  virtual void setIdentifier(Identifier id);
  // plottable
  virtual std::string type() const;


  // typedef needed for IdentifiableContainer base class
//  typedef PrepRawData DIGIT;

  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:

  MuonPrepDataCollection(const MuonPrepDataCollection&);
  MuonPrepDataCollection &operator=(const MuonPrepDataCollection&);

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:
  const IdentifierHash m_idHash; 
  Identifier m_id; // identifier of the DE
  

};
// member functions that use Collection T
#include"MuonPrepRawData/MuonPrepDataCollection.icc"
///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

}

#endif // MUONPREPRAWDATA_MUONPERDATACOLLECTION_H

