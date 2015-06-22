///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExDecayCnv.h 
// Header file for class AthExDecayCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_AthExDECAYCNV_H 
#define ATHEXTHINNING_AthExDECAYCNV_H 

// STL includes


// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// AthExThinning includes
#include "AthExThinning/AthExDecay.h"
#include "AthExThinning/AthExDecay_p1.h"

// Forward declaration

// the latest persistent representation type of DataCollection:
typedef AthExDecay_p1  AthExDecay_PERS;

class AthExDecayCnv: public T_AthenaPoolCustomCnv<
                                          AthExDecay, 
			                  AthExDecay_PERS 
			                  > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<AthExDecayCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  AthExDecayCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual AthExDecay_PERS*
  createPersistent( AthExDecay* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual AthExDecay* createTransient();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected:
};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> ATHEXTHINNING_AthExDECAYCNV_H

