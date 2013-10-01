///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExFatObjectCnv.h 
// Header file for class AthExFatObjectCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_AthExFATOBJECTCNV_H 
#define ATHEXTHINNING_AthExFATOBJECTCNV_H 

// STL includes


// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// AthExThinning includes
#include "AthExThinning/AthExFatObject.h"
#include "AthExThinning/AthExFatObject_p1.h"

// Forward declaration

// the latest persistent representation type of DataCollection:
typedef AthExFatObject_p1  AthExFatObject_PERS;

class AthExFatObjectCnv: public T_AthenaPoolCustomCnv<
                                          AthExFatObject, 
			                  AthExFatObject_PERS 
			                  > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<AthExFatObjectCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  AthExFatObjectCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual AthExFatObject_PERS*
  createPersistent( AthExFatObject* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual AthExFatObject* createTransient();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected:
};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> ATHEXTHINNING_ATHEXFATOBJECTCNV_H

