// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ReadDataReentrant.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief 
 */


#ifndef ATHEXSTOREGATEEXAMPLE_READDATAREENTRANT_H
#define ATHEXSTOREGATEEXAMPLE_READDATAREENTRANT_H

#include <string>
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthExStoreGateExample/MyDataObj.h"
#include "StoreGateExample_ClassDEF.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/UpdateHandleKey.h"



class ReadDataReentrant
  : public AthReentrantAlgorithm
{
public:
  ReadDataReentrant (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override final;
  virtual StatusCode execute_r (const EventContext& ctx) const override final;
  virtual StatusCode finalize() override final;
  
private:
  SG::UpdateHandleKey<MyDataObj> m_dobjKey3;
  SG::ReadHandleKey<DataVector<MyContObj> > m_cobjKey;
  SG::ReadHandleKey<std::vector<float> > m_vFloatKey;
  //SG::ReadHandleKey<MyDataObj> m_nonexistingKey;
  SG::ReadHandleKey<std::list<ElementLink<std::vector<float> > > > m_pLinkListKey;
  SG::ReadHandleKey<std::vector<ElementLink<MapStringFloat> > > m_linkVectorKey;
  SG::ReadHandleKey<TestDataObject> m_testObjectKey;
};



#endif // not ATHEXSTOREGATEEXAMPLE_READDATAREENTRANT_H
