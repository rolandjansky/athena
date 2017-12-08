// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/BAuxVec.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Class used for testing aux data reading/writing.
 */


#ifndef DATAMODELTESTDATACOMMON_BAUXVEC_H
#define DATAMODELTESTDATACOMMON_BAUXVEC_H


#include "DataModelTestDataCommon/BAux.h"
#include "AthContainers/DataVector.h"
#include "AthContainersInterfaces/IAuxStoreHolder.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "SGTools/CLASS_DEF.h"


namespace DMTest {


//typedef DataVector<BAux> BAuxVec;
class BAuxVec
  : public DataVector<BAux>,
    public SG::IAuxStoreHolder,
    public SG::IAuxStoreIO
{
public:
  using DataVector<BAux>::setStore;
  using DataVector<BAux>::getStore;
  virtual void setStore( SG::IAuxStore* store ) override { DataVector<BAux>::setStore(store); }
  virtual SG::IAuxStore* getStore() override { return DataVector<BAux>::getStore(); }
  virtual const SG::IAuxStore* getStore() const override { return DataVector<BAux>::getStore(); }
  virtual SG::IAuxStoreHolder::AuxStoreType getStoreType() const override { return SG::IAuxStoreHolder::AST_ContainerStore; }
  virtual const void* getIOData (SG::auxid_t auxid) const override
  {
    const IAuxStoreIO* s = dynamic_cast<const SG::IAuxStoreIO*>(getStore());
    if (s) return s->getIOData(auxid);
    return 0;
  }
  virtual const std::type_info* getIOType (SG::auxid_t auxid) const  override
  {
    const IAuxStoreIO* s = dynamic_cast<const SG::IAuxStoreIO*>(getStore());
    if (s) return s->getIOType(auxid);
    return 0;
  }
  virtual const SG::auxid_set_t& getDynamicAuxIDs() const override
  {
    const IAuxStoreIO* s = dynamic_cast<const SG::IAuxStoreIO*>(getStore());
    if (s) return s->getDynamicAuxIDs();
    static const SG::auxid_set_t dum;
    return dum;
  }
};


} // namespace DMTest


//CLASS_DEF (DataVector<DMTest::BAux>, 9701, 1)
CLASS_DEF (DMTest::BAuxVec, 9711, 1)


#endif // not DATAMODELTESTDATACOMMON_BAUXVEC_H
