///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McEventCollectionCnv.h 
// Header file for class McEventCollectionCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef GENERATOROBJECTSATHENAPOOL_MCEVENTCOLLECTIONCNV_H 
#define GENERATOROBJECTSATHENAPOOL_MCEVENTCOLLECTIONCNV_H 

// STL includes

// HepMC / CLHEP includes
#include "GeneratorObjects/McEventCollection.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// GeneratorObjectsTPCnv includes
#include "GeneratorObjectsTPCnv/McEventCollection_p5.h"

// Forward declaration

// the latest persistent representation type of DataCollection:
typedef McEventCollection_p5  McEventCollection_PERS;

class McEventCollectionCnv: public T_AthenaPoolCustomCnv<
                                      McEventCollection, 
			              McEventCollection_PERS 
			           > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<McEventCollectionCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  McEventCollectionCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual McEventCollection_PERS*
  createPersistent( McEventCollection* transCont );
  
  /** Build the transient representation from a persistent one
   */
   virtual McEventCollection* createTransient();

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline McEventCollectionCnv::McEventCollectionCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<McEventCollection, McEventCollection_PERS>(svcLocator)
{}

#endif //> GENERATOROBJECTSATHENAPOOL_MCEVENTCOLLECTIONCNV_H

