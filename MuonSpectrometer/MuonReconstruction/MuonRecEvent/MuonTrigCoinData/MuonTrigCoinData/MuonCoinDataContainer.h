/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonCoinDataContainer.h
//   Header file for class MuonCoinDataContainer
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Container for RIO  collections for  Muons
///////////////////////////////////////////////////////////////////
// Version 1.0 106/07/2004 Ketevi A. Assamagan - adapted 
// from Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef MUONTRIGCOINDATA_MUONCOINDATACONTAINER_H
#define MUONTRIGCOINDATA_MUONCOINDATACONTAINER_H

// Base classes
#include "CLIDSvc/CLASS_DEF.h"
#include "EventContainers/IdentifiableContainer.h"

//Needed Classes
#include "MuonTrigCoinData/TgcCoinData.h"

#include "MuonTrigCoinData/MuonCoinDataCollection.h"

namespace Muon{

template<class CollectionT>
class MuonCoinDataContainer : public IdentifiableContainer<CollectionT> {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

    //default for POOL
//      MuonCoinDataContainer();
    
  // Constructor with parameters:
  MuonCoinDataContainer(unsigned int max);
  
  // Destructor:
  virtual ~MuonCoinDataContainer();

   /** return class ID */
   static const CLID& classID() 
   {
     //	static CLID id = CLID_T ; 
     //	return id; 
     return ClassID_traits< MuonCoinDataContainer <CollectionT> > ::ID();
   }

   /** return class ID */
   virtual const CLID& clID() const
    {
      return classID();
    }


  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////



  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:


  MuonCoinDataContainer(const MuonCoinDataContainer&);
  MuonCoinDataContainer &operator=(const MuonCoinDataContainer&);
  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:

};

///////////////////////////////////////////////////////////////////
// Inline methods:
/////////////////////////////////////////////////////////////////// 
 
typedef MuonCoinDataCollection< TgcCoinData > TgcCoinDataCollection;

// member functions that use Collection T
#include "MuonTrigCoinData/MuonCoinDataContainer.icc"

 
}

#endif // MUONTRIGCOINDATA_MUONCOINDATACONTAINER_H

