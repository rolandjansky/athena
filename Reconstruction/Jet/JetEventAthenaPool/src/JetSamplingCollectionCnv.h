///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetSamplingCollectionCnv.h 
// Header file for class JetSamplingCollectionCnv
// Author: S.Binet<binet@cern.ch>
// Modif: Belen.Salvachua@cern.ch  Dec 2008  Add JetSamplingCollection_p2
/////////////////////////////////////////////////////////////////// 
#ifndef RECATHENAPOOL_JETSAMPLINGCOLLECTIONCNV_H 
#define RECATHENAPOOL_JETSAMPLINGCOLLECTIONCNV_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// JetEventTPCnv includes
#include "JetEventTPCnv/JetSamplingCollection_p1.h"
#include "JetEventTPCnv/JetSamplingCollection_p2.h"

// JetEvent includes
#include "JetEvent/JetSamplingCollection.h"


// Forward declaration

// the latest persistent representation type of DataCollection:
typedef JetSamplingCollection_p2  JetSamplingCollection_PERS;

class JetSamplingCollectionCnv: public T_AthenaPoolCustomCnv<
                                      JetSamplingCollection, 
                                      JetSamplingCollection_PERS 
                                   > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<JetSamplingCollectionCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  JetSamplingCollectionCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual JetSamplingCollection_PERS*
    createPersistent( JetSamplingCollection* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual JetSamplingCollection* createTransient();

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline 
JetSamplingCollectionCnv::JetSamplingCollectionCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<JetSamplingCollection, 
			JetSamplingCollection_PERS>(svcLocator)
{}

#endif //> RECATHENAPOOL_JETSAMPLINGCOLLECTIONCNV_H
