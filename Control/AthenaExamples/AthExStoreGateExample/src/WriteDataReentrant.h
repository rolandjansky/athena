// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file WriteDataReentrant.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief 
 */


#ifndef ATHEXSTOREGATEEXAMPLE_WRITEDATAREENTRANT_H
#define ATHEXSTOREGATEEXAMPLE_WRITEDATAREENTRANT_H


#include <string>
#include "AthExStoreGateExample/MyDataObj.h"
#include "StoreGateExample_ClassDEF.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"


class WriteDataReentrant
  : public AthReentrantAlgorithm
{
public:
  WriteDataReentrant (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  virtual StatusCode execute_r (const EventContext& ctx) const override final;

private:
  SG::WriteHandleKey<MyDataObj> m_dobjKey;
  SG::WriteHandleKey<MyDataObj> m_dobjKey2;
  SG::WriteHandleKey<MyDataObj> m_dobjKey3;
  //SG::WriteHandleKey<MyDataObj> m_dobjKey4;
  SG::WriteHandleKey<DataVector<MyContObj> > m_cobjKey;
  SG::WriteHandleKey<std::vector<float> > m_vFloatKey;
  SG::WriteHandleKey<MapStringFloat> m_mKey;
  SG::WriteHandleKey<std::list<ElementLink<std::vector<float> > > > m_pLinkListKey;
  SG::WriteHandleKey<std::vector<ElementLink<MapStringFloat> > > m_linkVectorKey;
  SG::WriteHandleKey<TestDataObject> m_testObjectKey;

  SG::DataObjectSharedPtr<TestDataObject> m_testObject;
  StatusCode onError() const;
};


#endif // not ATHEXSTOREGATEEXAMPLE_WRITEDATAREENTRANT_H
