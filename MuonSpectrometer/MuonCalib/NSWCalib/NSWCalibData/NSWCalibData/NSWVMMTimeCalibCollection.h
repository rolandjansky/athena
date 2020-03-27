/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef NSWVMMTIMECALIBCOLLECTION_H
#define NSWVMMTIMECALIBCOLLECTION_H

// Base classes
#include "Identifier/Identifiable.h"
#include "Identifier/IdentifierHash.h"
#include "AthContainers/DataVector.h"



namespace Muon{

class NSWVMMTimeCalibCollection : public Identifiable,
                 public DataVector< VMMTimePar > {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  //   Hashed offline identifier of the DE
  NSWVMMTimeCalibCollection(const IdentifierHash idHash);

 /** Default Constructor (for persistency)*/
  NSWVMMTimeCalibCollection();
                     
  /** Destructor:*/
  virtual ~NSWVMMTimeCalibCollection();


  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  // typedef needed for IdentifiableContainer base class
  typedef Identifier ID;

  // identifier of this detector element:
  virtual Identifier identify() const;

  virtual IdentifierHash identifyHash() const;

  virtual void setIdentifier(Identifier id);
  // plottable
  virtual std::string type() const;

  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:

  NSWVMMTimeCalibCollection(const NSWVMMTimeCalibCollection&);
  NSWVMMTimeCalibCollection &operator=(const NSWVMMTimeCalibCollection&);

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:
  const IdentifierHash m_idHash; 
  Identifier m_id; // identifier of the DE
  

};

CLASS_DEF(NSWVMMTimeCalibCollection,1233807773 , 1)

}

#endif 
