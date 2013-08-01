/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECATHENAPOOL_MISSINGETCALOCNV_H 
#define RECATHENAPOOL_MISSINGETCALOCNV_H 
/********************************************************************
NAME:     MissingEtCaloCnv.h
PURPOSE:  Header file for class MissingEtCaloCnv
********************************************************************/

#include "MissingETEvent/MissingEtCalo.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// RecTPCnv includes
#include "RecTPCnv/MissingEtCalo_p3.h"

// Forward declaration

// the latest persistent representation type of DataCollection:
typedef MissingEtCalo_p3  MissingEtCalo_PERS;

class MissingEtCaloCnv: public T_AthenaPoolCustomCnv< MissingEtCalo, MissingEtCalo_PERS > {

  // make the factory for this converter our friend
  friend class CnvFactory<MissingEtCaloCnv>;

 protected:

  /** Create the converter from the service locator
   */
  MissingEtCaloCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual MissingEtCalo_PERS*
    createPersistent( MissingEtCalo* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual MissingEtCalo* createTransient();

};


inline MissingEtCaloCnv::MissingEtCaloCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<MissingEtCalo, MissingEtCalo_PERS>(svcLocator)
{}

#endif //> RECATHENAPOOL_MISSINGETCALOCNV_H
