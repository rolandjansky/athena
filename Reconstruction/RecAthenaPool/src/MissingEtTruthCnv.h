/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECATHENAPOOL_MISSINGETTRUTHCNV_H 
#define RECATHENAPOOL_MISSINGETTRUTHCNV_H 
/********************************************************************
NAME:     MissingEtTruthCnv.h
PACKAGE:  offline/Reconstruction/RecAthenaPool
PURPOSE:  Header file for class MissingEtTruthCnv
********************************************************************/

#include "MissingETEvent/MissingEtTruth.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// RecTPCnv includes
// #include "RecTPCnv/MissingEtTruth_p1.h"// not needed here
// #include "RecTPCnv/MissingEtTruth_p2.h" // not needed here
#include "RecTPCnv/MissingEtTruth_p3.h"


// the latest persistent representation type of DataCollection:
typedef MissingEtTruth_p3  MissingEtTruth_PERS;

class MissingEtTruthCnv: public T_AthenaPoolCustomCnv<
                                      MissingEtTruth, 
                                      MissingEtTruth_PERS 
                                   > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<MissingEtTruthCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  MissingEtTruthCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual MissingEtTruth_PERS*
    createPersistent( MissingEtTruth* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual MissingEtTruth* createTransient();

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline MissingEtTruthCnv::MissingEtTruthCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<MissingEtTruth, MissingEtTruth_PERS>(svcLocator)
{}

#endif //> RECATHENAPOOL_MISSINGETTRUTHCNV_H
