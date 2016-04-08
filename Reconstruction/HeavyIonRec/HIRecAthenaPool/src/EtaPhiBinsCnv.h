/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECATHENAPOOL_ETAPHIBINSCNV_H 
#define RECATHENAPOOL_ETAPHIBINSCNV_H 
/********************************************************************

NAME:     EtaPhiBinsCnv.h
PACKAGE:  offline/Reconstruction/RecAthenaPool

AUTHORS:  S. Resconi
CREATED:  Jul 2007

PURPOSE:  Header file for class EtaPhiBinsCnv
********************************************************************/


// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// RecTPCnv includes
#include "HIRecTPCnv/EtaPhiBins_p1.h"

// Forward declaration

#include "HIGlobal/EtaPhiBins.h"

// the latest persistent representation type of DataCollection:
typedef EtaPhiBins_p1  EtaPhiBins_PERS;

class EtaPhiBinsCnv: public T_AthenaPoolCustomCnv<
                                      EtaPhiBins, 
                                      EtaPhiBins_PERS 
                                   > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<EtaPhiBinsCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  EtaPhiBinsCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual EtaPhiBins_PERS*
    createPersistent( EtaPhiBins* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual EtaPhiBins* createTransient();

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline EtaPhiBinsCnv::EtaPhiBinsCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<EtaPhiBins, EtaPhiBins_PERS>(svcLocator)
{}

#endif //> RECATHENAPOOL_ETAPHIBINSCNV_H
