/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef USERDATASTORECNV_P1
#define USERDATASTORECNV_P1

#include "AthContainers/UserDataStore.h"
#include "DataModelAthenaPool/UserDataStore_p1.h"
#include "DataModelAthenaPool/UDSLabelHashTableCnv_p1.h"


/** @class UserDataStoreObjCnv_p1
 *  @brief T/P converter for UserDataStoreObj
 **/

class UserDataStoreCnv_p1 {
public:
  void persToTrans(const UserDataStore_p1& persObj, UserDataStore& transObj,MsgStream& mLog);

  void transToPers(const UserDataStore& transObj, UserDataStore_p1& persObj,MsgStream& mLog);

private:
  UDSLabelHashTableCnv_p1 m_labelCnv;

  typedef UserDataStore::mapmap_t mapmap_t;

  enum TYPES {
    UINT=0,
    INT,
    FLOAT,
    DOUBLE,
    VUINT,
    VINT,
    VFLOAT,
    VDOUBLE,
    BOOL
  };

};


#endif

