// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ReadDataReentrant.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Testing reentrant algorithms.
 */


#ifndef ATHEXSTOREGATEEXAMPLE_READDATAREENTRANT_H
#define ATHEXSTOREGATEEXAMPLE_READDATAREENTRANT_H

#include <string>
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthExStoreGateExample/MyDataObj.h"
#include "StoreGateExample_ClassDEF.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/UpdateHandleKey.h"
#include "xAODEventInfo/EventInfo.h"



class ReadDataReentrant
  : public AthReentrantAlgorithm
{
public:
  ReadDataReentrant (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override final;
  virtual StatusCode execute (const EventContext& ctx) const override final;
  virtual StatusCode finalize() override final;
  
private:
  SG::ReadHandleKey<DataVector<MyContObj> > m_cobjKey;
  SG::ReadHandleKey<std::vector<float> > m_vFloatKey;
  SG::ReadHandleKey<std::list<ElementLink<std::vector<float> > > > m_pLinkListKey;
  SG::ReadHandleKey<std::vector<ElementLink<MapStringFloat> > > m_linkVectorKey;
  SG::ReadHandleKey<TestDataObject> m_testObjectKey;
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this, "EvtInfo", "EventInfo", "EventInfo name"};
  SG::ReadHandleKeyArray<MyDataObj> m_dobjKeyArray;
};



#endif // not ATHEXSTOREGATEEXAMPLE_READDATAREENTRANT_H
