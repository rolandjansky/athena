/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonPrepRawDataContainer.h
//   Header file for class MuonPrepRawDataContainer
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Container for RIO  collections for  Muons
///////////////////////////////////////////////////////////////////
// Version 1.0 106/07/2004 Ketevi A. Assamagan - adapted 
// from Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef MUONPREPRAWDATA_MUONPREPDATACONTAINER_H
#define MUONPREPRAWDATA_MUONPREPDATACONTAINER_H

// Base classes
#include "CLIDSvc/CLASS_DEF.h"
#include "EventContainers/IdentifiableContainer.h"

//Needed Classes
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"

#include "MuonPrepRawData/MuonPrepDataCollection.h"

namespace Muon{

/** @brief Template for Muon PRD containers (which are basically collections of MuonPrepDataCollections).*/
template<class CollectionT>
class MuonPrepDataContainer : public IdentifiableContainer<CollectionT> {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:
    
  // Constructor with parameters:
  MuonPrepDataContainer(unsigned int max);
  
  // Destructor:
  virtual ~MuonPrepDataContainer();

   /** return class ID */
   static const CLID& classID() 
   {
     //	static CLID id = CLID_T ; 
     //	return id; 
     return ClassID_traits< MuonPrepDataContainer <CollectionT> > ::ID();
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


  MuonPrepDataContainer(const MuonPrepDataContainer&);
  MuonPrepDataContainer &operator=(const MuonPrepDataContainer&);
  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:

};

///////////////////////////////////////////////////////////////////
// Inline methods:
/////////////////////////////////////////////////////////////////// 
 
typedef MuonPrepDataCollection< MdtPrepData > MdtPrepDataCollection;
typedef MuonPrepDataContainer< MdtPrepDataCollection > MdtPrepDataContainer;
 
typedef MuonPrepDataCollection< RpcPrepData > RpcPrepDataCollection;
typedef MuonPrepDataContainer< RpcPrepDataCollection > RpcPrepDataContainer;
 
typedef MuonPrepDataCollection< TgcPrepData > TgcPrepDataCollection;
typedef MuonPrepDataContainer< TgcPrepDataCollection > TgcPrepDataContainer;

// member functions that use Collection T
#include "MuonPrepRawData/MuonPrepDataContainer.icc"

 
}

#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"

#endif // TRKPREPRAWDATA_PREPRAWDATACONTAINER_H

