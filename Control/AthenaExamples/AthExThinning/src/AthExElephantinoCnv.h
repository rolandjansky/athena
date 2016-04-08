///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExElephantinoCnv.h 
// Header file for class AthExElephantinoCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_AthExELEPHANTINOCNV_H 
#define ATHEXTHINNING_AthExELEPHANTINOCNV_H 

// STL includes


// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// AthExThinning includes
#include "AthExThinning/AthExElephantino.h"
#include "AthExThinning/AthExElephantino_p1.h"

// Forward declaration

// the latest persistent representation type of DataCollection:
typedef AthExElephantino_p1  AthExElephantino_PERS;

class AthExElephantinoCnv: public T_AthenaPoolCustomCnv<
                                          AthExElephantino, 
			                  AthExElephantino_PERS 
			                  > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<AthExElephantinoCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  AthExElephantinoCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual AthExElephantino_PERS*
  createPersistent( AthExElephantino* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual AthExElephantino* createTransient();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected:
};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> ATHEXTHINNING_AthExELEPHANTINOCNV_H

