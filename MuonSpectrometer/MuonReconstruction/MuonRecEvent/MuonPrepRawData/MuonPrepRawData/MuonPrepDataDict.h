/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPREPDATADICT_H
#define MUONPREPDATADICT_H

#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/CscStripPrepData.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonPrepRawData/sTgcPrepData.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "AthLinks/ElementLink.h"

#define EXPAND_DLV(T, suffix) \
  ::DataLinkVector< Muon::MuonPrepDataCollection< T > > m_dummy_dlv_ ## suffix ; \
  ::DataLinkVector< Muon::MuonPrepDataCollection< T > >::DataHolder m_dummy_holder_ ## suffix ; \
  ::DataLinkVector< Muon::MuonPrepDataCollection< T > >::Entry m_dummy_entry_ ## suffix ; \
  ::DataLinkVector< Muon::MuonPrepDataCollection< T > >::iterator m_dummy_iter_ ## suffix; \
  ::ElementLink< T ## Container> m_dummy_el_ ## suffix

namespace MuonPrepRawData_dict {
  using namespace Muon; 
struct tmp
{
    DataVector<Trk::PrepRawData> tmpDV0;
    DataVector<CscPrepData> tmpDV1;
    DataVector<CscStripPrepData> tmpDV1a;
  DataVector<MdtPrepData> tmpDV2;
  DataVector<RpcPrepData> tmpDV3;
  DataVector<TgcPrepData> tmpDV4;
  DataVector<sTgcPrepData> tmpDV5;
  DataVector<MMPrepData> tmpDV6;
  CscPrepDataContainer tmpCont1;
  MdtPrepDataContainer tmpCont2;
  RpcPrepDataContainer tmpCont3;
  TgcPrepDataContainer tmpCont4;
  sTgcPrepDataContainer tmpCont5;
  MMPrepDataContainer tmpCont6;

  DataVector<Muon::MuonPrepDataCollection<Muon::CscPrepData> >    dummy1;
  std::vector<Muon::MuonPrepDataCollection<Muon::CscPrepData>* >  dummy2;
  DataVector<Muon::MuonPrepDataCollection<Muon::CscStripPrepData> >    dummy1a;
  std::vector<Muon::MuonPrepDataCollection<Muon::CscStripPrepData>* >  dummy2a;
   DataVector<Muon::MuonPrepDataCollection<Muon::RpcPrepData> >    dummy3;
   std::vector<Muon::MuonPrepDataCollection<Muon::RpcPrepData>* >  dummy4;
   DataVector<Muon::MuonPrepDataCollection<Muon::TgcPrepData> >    dummy5;
   std::vector<Muon::MuonPrepDataCollection<Muon::TgcPrepData>* >  dummy6;
   DataVector<Muon::MuonPrepDataCollection<Muon::MdtPrepData> >    dummy7;
   std::vector<Muon::MuonPrepDataCollection<Muon::MdtPrepData>* >  dummy8;
   DataVector<Muon::MuonPrepDataCollection<Muon::sTgcPrepData> >    dummy9;
   std::vector<Muon::MuonPrepDataCollection<Muon::sTgcPrepData>* >  dummy10;
   DataVector<Muon::MuonPrepDataCollection<Muon::MMPrepData> >    dummy11;
   std::vector<Muon::MuonPrepDataCollection<Muon::MMPrepData>* >  dummy12;

//  EXPAND_DLV(Muon::CscPrepData, csc_prep_data);
//  EXPAND_DLV(Muon::RpcPrepData, rpc_prep_data);
//  EXPAND_DLV(Muon::TgcPrepData, tgc_prep_data);
//  EXPAND_DLV(Muon::sTgcPrepData, stgc_prep_data);
//  EXPAND_DLV(Muon::MMPrepData, mm_prep_data);
//  EXPAND_DLV(Muon::MdtPrepData, mdt_prep_data);
};

}

#undef EXPAND_DLV

#endif
