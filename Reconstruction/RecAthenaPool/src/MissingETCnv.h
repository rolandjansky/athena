/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECATHENAPOOL_MISSINGETCNV_H 
#define RECATHENAPOOL_MISSINGETCNV_H 
/********************************************************************
NAME:     MissingETCnv.h
PACKAGE:  offline/Reconstruction/RecAthenaPool
PURPOSE:  Header file for class MissingETCnv
********************************************************************/

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// RecTPCnv includes
// #include "RecTPCnv/MissingET_p1.h" //not needed here
// #include "RecTPCnv/MissingET_p2.h" //not needed here
#include "RecTPCnv/MissingET_p3.h"
#include "MissingETEvent/MissingET.h"

// Forward declaration

// the latest persistent representation type of DataCollection:
typedef MissingET_p3  MissingET_PERS;

class MissingETCnv: public T_AthenaPoolCustomCnv< MissingET, MissingET_PERS >
{

  // make the factory for this converter our friend
  friend class CnvFactory<MissingETCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  MissingETCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual MissingET_PERS* createPersistent( MissingET* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual MissingET* createTransient();

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline MissingETCnv::MissingETCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<MissingET, MissingET_PERS>(svcLocator)
{}

#endif //> RECATHENAPOOL_MISSINGETCNV_H
