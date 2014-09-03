// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/BAuxStandalone.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Class used for testing aux data reading/writing.
 */


#ifndef DATAMODELTESTDATACOMMON_BAUXSTANDALONE_H
#define DATAMODELTESTDATACOMMON_BAUXSTANDALONE_H


#include "DataModelTestDataCommon/BAux.h"
#include "AthContainersInterfaces/IAuxStoreHolder.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "SGTools/CLASS_DEF.h"
#include "SGTools/BaseInfo.h"


namespace DMTest {


struct BAuxStandalone
  : public BAux,
    public SG::IAuxStoreHolder,
    public SG::IAuxStoreIO
{
  BAuxStandalone (int b = 0) : BAux (b) {}

  using BAux::setStore;
  using BAux::getStore;
  virtual void setStore( SG::IAuxStore* store ) { BAux::setStore(store); }
  virtual SG::IAuxStore* getStore() const { return BAux::getStore(); }
  virtual SG::IAuxStoreHolder::AuxStoreType getStoreType() const { return SG::IAuxStoreHolder::AST_ObjectStore; }
  virtual const void* getIOData (SG::auxid_t auxid) const
  {
    IAuxStoreIO* s = dynamic_cast<SG::IAuxStoreIO*>(getStore());
    if (s) return s->getIOData(auxid);
    return 0;
  }
  virtual const std::type_info* getIOType (SG::auxid_t auxid) const 
  {
    IAuxStoreIO* s = dynamic_cast<SG::IAuxStoreIO*>(getStore());
    if (s) return s->getIOType(auxid);
    return 0;
  }
  virtual const SG::auxid_set_t& getDynamicAuxIDs() const 
  {
    IAuxStoreIO* s = dynamic_cast<SG::IAuxStoreIO*>(getStore());
    if (s) return s->getDynamicAuxIDs();
    static SG::auxid_set_t dum;
    return dum;
  }
};


} // namespace DMTest


SG_BASE(DMTest::BAuxStandalone, DMTest::BAux);
CLASS_DEF (DMTest::BAuxStandalone, 9703, 1)


#endif // not DATAMODELTESTDATACOMMON_BAUXSTANDALONE_H
