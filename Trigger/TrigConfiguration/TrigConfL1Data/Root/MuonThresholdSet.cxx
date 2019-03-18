/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/MuonThresholdSet.h"
#include <iostream>

using namespace std;

TrigConf::MuonThresholdSet::MuonThresholdSet() : 
   L1DataBaseclass()
{
   m_rpc_avail = false;
   m_rpc_avail_online = false;
   m_tgc_avail = false;
   m_tgc_avail_online = false;

   m_rpc_set_name = "unknown";
   m_rpc_set_ext_id = -1;
   m_rpc_pt1_ext_id = -1;
   m_rpc_pt2_ext_id = -1;
   m_rpc_pt3_ext_id = -1;
   m_rpc_pt4_ext_id = -1;
   m_rpc_pt5_ext_id = -1;
   m_rpc_pt6_ext_id = -1;

   m_tgc_set_name = "unknown";
   m_tgc_set_ext_id = -1;
}

void
TrigConf::MuonThresholdSet::print(const std::string& indent, unsigned int /*detail*/) const {
   cout << indent << "============================================="  << endl;
   cout << indent << "MuonThresholdSet Object for sm key = " << superMasterTableId() << endl;
   cout << indent << "============================================="  << endl;
   printNameIdV(indent);
   cout << indent << "RPC available (online) " << rpcAvailable() 
        << "(" << rpcAvailableOnline() << ")" << endl;
   cout << indent << "TGC available (online) " << tgcAvailable() 
        << "(" << tgcAvailableOnline() << ")" << endl;
   cout << indent << "RPC set name '" << rpcSetName() << "'" << endl;
   cout << indent << "RPC set id " << rpcSetId() << endl;
   cout << indent << "RPC pt1 id " << rpcPt1Id() << endl;
   cout << indent << "RPC pt2 id " << rpcPt2Id() << endl;
   cout << indent << "RPC pt3 id " << rpcPt3Id() << endl;
   cout << indent << "RPC pt4 id " << rpcPt4Id() << endl;
   cout << indent << "RPC pt5 id " << rpcPt5Id() << endl;
   cout << indent << "RPC pt6 id " << rpcPt6Id() << endl;
   cout << indent << "TGC set id " << tgcSetId() << endl;
   cout << indent << "TGC set name '" << tgcSetName() << "'" << endl;
}
