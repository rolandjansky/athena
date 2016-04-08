/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef USERDATASTORECNV_H
#define USERDATASTORECNV_H


#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "DataModelAthenaPool/UserDataStoreCnv_p1.h"
#include "AthContainers/UserDataStore.h"
#include "GaudiKernel/MsgStream.h"

/** @class UserDataStoreCnv 
 *  @brief Top-Level T/P converter for UserDataStore object
 **/
	
class UserDataStoreCnv  : public T_AthenaPoolCustomCnv<UserDataStore, UserDataStore_p1 > {
  friend class CnvFactory<UserDataStoreCnv>;

 protected:
  UserDataStoreCnv(ISvcLocator* svcloc);
  UserDataStore_p1*  createPersistent(UserDataStore* transCont);
  UserDataStore*     createTransient ();

 private:
    UserDataStoreCnv_p1 m_converterP1;
    pool::Guid m_guidP1;
    MsgStream m_log;

};	

#endif
