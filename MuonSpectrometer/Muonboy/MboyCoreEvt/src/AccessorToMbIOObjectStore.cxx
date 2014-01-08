/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/AccessorToMbIOObjectStore.h"

AccessorToMbIOObjectStore::AccessorToMbIOObjectStore(){}
AccessorToMbIOObjectStore::AccessorToMbIOObjectStore(
      MbIOObjectStore&  aMbIOObjectStore
){

   m_MbIOObjectStore  = aMbIOObjectStore  ;

}

AccessorToMbIOObjectStore::~AccessorToMbIOObjectStore(){}

// Nber of typed elements
int AccessorToMbIOObjectStore::GetNberOfDigiMDT         () const{return  m_MbIOObjectStore.GetNberOfDigiMDT          () ;}
int AccessorToMbIOObjectStore::GetNberOfDigiRPC         () const{return  m_MbIOObjectStore.GetNberOfDigiRPC          () ;}
int AccessorToMbIOObjectStore::GetNberOfDigiTGC         () const{return  m_MbIOObjectStore.GetNberOfDigiTGC          () ;}
int AccessorToMbIOObjectStore::GetNberOfDigiCSC         () const{return  m_MbIOObjectStore.GetNberOfDigiCSC          () ;}
int AccessorToMbIOObjectStore::GetNberOfDigiCSCclu      () const{return  m_MbIOObjectStore.GetNberOfDigiCSCclu       () ;}

int AccessorToMbIOObjectStore::GetNberOfMbMuSc() const{return  m_MbIOObjectStore.GetNberOfMbMuSc () ;}

int AccessorToMbIOObjectStore::GetNberOfHoleMDT         () const{return  m_MbIOObjectStore.GetNberOfHoleMDT          () ;}
int AccessorToMbIOObjectStore::GetNberOfHoleRPC         () const{return  m_MbIOObjectStore.GetNberOfHoleRPC          () ;}
int AccessorToMbIOObjectStore::GetNberOfHoleTGC         () const{return  m_MbIOObjectStore.GetNberOfHoleTGC          () ;}
int AccessorToMbIOObjectStore::GetNberOfHoleCSC         () const{return  m_MbIOObjectStore.GetNberOfHoleCSC          () ;}
int AccessorToMbIOObjectStore::GetNberOfHoleCSCclu      () const{return  m_MbIOObjectStore.GetNberOfHoleCSCclu       () ;}

// Nber of elements
int AccessorToMbIOObjectStore::GetNberOfElements        () const{return m_MbIOObjectStore.GetNberOfElements()    ;}
int AccessorToMbIOObjectStore::GetNberOfDigi            () const{return m_MbIOObjectStore.GetNberOfDigi()        ;}

// Get const pointer of a element 
const MbIOObject*       AccessorToMbIOObjectStore::LiIdGetMbIOObject       (int LinearNber) const{ return m_MbIOObjectStore.LiIdGetMbIOObject      (LinearNber);}
const MbIDTag*          AccessorToMbIOObjectStore::LiIdGetMbIDTag          (int LinearNber) const{ return m_MbIOObjectStore.LiIdGetMbIDTag         (LinearNber);}
const MbPOT*            AccessorToMbIOObjectStore::LiIdGetMbPOT            (int LinearNber) const{ return m_MbIOObjectStore.LiIdGetMbPOT           (LinearNber);}
const MbROT*            AccessorToMbIOObjectStore::LiIdGetMbROT            (int LinearNber) const{ return m_MbIOObjectStore.LiIdGetMbROT           (LinearNber);}
const MbHole*           AccessorToMbIOObjectStore::LiIdGetMbHole           (int LinearNber) const{ return m_MbIOObjectStore.LiIdGetMbHole          (LinearNber);}

// Get const pointer of a typed element 
const MbROTMDT*         AccessorToMbIOObjectStore::LiIdGetMbROTMDT         (int LinearNber) const{return m_MbIOObjectStore.LiIdGetMbROTMDT         (LinearNber);}
const MbROTRPC*         AccessorToMbIOObjectStore::LiIdGetMbROTRPC         (int LinearNber) const{return m_MbIOObjectStore.LiIdGetMbROTRPC         (LinearNber);}
const MbROTTGC*         AccessorToMbIOObjectStore::LiIdGetMbROTTGC         (int LinearNber) const{return m_MbIOObjectStore.LiIdGetMbROTTGC         (LinearNber);}
const MbROTCSC*         AccessorToMbIOObjectStore::LiIdGetMbROTCSC         (int LinearNber) const{return m_MbIOObjectStore.LiIdGetMbROTCSC         (LinearNber);}
const MbROTCSCclu*      AccessorToMbIOObjectStore::LiIdGetMbROTCSCclu      (int LinearNber) const{return m_MbIOObjectStore.LiIdGetMbROTCSCclu      (LinearNber);}

const MbMuSc*           AccessorToMbIOObjectStore::LiIdGetMbMuSc           (int LinearNber) const{return m_MbIOObjectStore.LiIdGetMbMuSc           (LinearNber);}

const MbHoleMDT*        AccessorToMbIOObjectStore::LiIdGetMbHoleMDT        (int LinearNber) const{return m_MbIOObjectStore.LiIdGetMbHoleMDT        (LinearNber);}
const MbHoleRPC*        AccessorToMbIOObjectStore::LiIdGetMbHoleRPC        (int LinearNber) const{return m_MbIOObjectStore.LiIdGetMbHoleRPC        (LinearNber);}
const MbHoleTGC*        AccessorToMbIOObjectStore::LiIdGetMbHoleTGC        (int LinearNber) const{return m_MbIOObjectStore.LiIdGetMbHoleTGC        (LinearNber);}
const MbHoleCSC*        AccessorToMbIOObjectStore::LiIdGetMbHoleCSC        (int LinearNber) const{return m_MbIOObjectStore.LiIdGetMbHoleCSC        (LinearNber);}
const MbHoleCSCclu*     AccessorToMbIOObjectStore::LiIdGetMbHoleCSCclu     (int LinearNber) const{return m_MbIOObjectStore.LiIdGetMbHoleCSCclu     (LinearNber);}

const MbIDTagMDT*       AccessorToMbIOObjectStore::LiIdGetMbIDTagMDT       (int LinearNber) const{return m_MbIOObjectStore.LiIdGetMbIDTagMDT       (LinearNber);}
const MbIDTagRPC*       AccessorToMbIOObjectStore::LiIdGetMbIDTagRPC       (int LinearNber) const{return m_MbIOObjectStore.LiIdGetMbIDTagRPC       (LinearNber);}
const MbIDTagTGC*       AccessorToMbIOObjectStore::LiIdGetMbIDTagTGC       (int LinearNber) const{return m_MbIOObjectStore.LiIdGetMbIDTagTGC       (LinearNber);}
const MbIDTagCSC*       AccessorToMbIOObjectStore::LiIdGetMbIDTagCSC       (int LinearNber) const{return m_MbIOObjectStore.LiIdGetMbIDTagCSC       (LinearNber);}
const MbIDTagCSCclu*    AccessorToMbIOObjectStore::LiIdGetMbIDTagCSCclu    (int LinearNber) const{return m_MbIOObjectStore.LiIdGetMbIDTagCSCclu    (LinearNber);}

// Print sorted 
void AccessorToMbIOObjectStore::PrintSortedElements(std::ostream* out) const{ m_MbIOObjectStore.PrintSortedElements(out); }
