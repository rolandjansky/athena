/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_MuonThresholdSet
#define TrigConf_MuonThresholdSet

#include <iosfwd>
#include <string>
#include "TrigConfL1Data/L1DataBaseclass.h"

namespace TrigConf {  
   class MuonThresholdSet : public L1DataBaseclass {
   public:
      MuonThresholdSet();
      ~MuonThresholdSet() = default;
    
      bool rpcAvailable() const {return m_rpc_avail;}
      bool rpcAvailableOnline() const {return m_rpc_avail_online;}
      void setRpcAvailable( const bool& b ){m_rpc_avail = b;}
      void setRpcAvailableOnline( const bool& b ){m_rpc_avail_online = b;}

      bool tgcAvailable() const {return m_tgc_avail;}
      void setTgcAvailable( const bool& b ){m_tgc_avail = b;}
      bool tgcAvailableOnline() const {return m_tgc_avail_online;}
      void setTgcAvailableOnline( const bool& b ){m_tgc_avail_online = b;}

      int rpcSetId() const {return m_rpc_set_ext_id;}
      void setRpcSetId( const int& i ){m_rpc_set_ext_id = i;}

      const std::string & rpcSetName() const {return m_rpc_set_name;}
      void setRpcSetName( const std::string& s ){m_rpc_set_name = s;}

      int tgcSetId() const {return m_tgc_set_ext_id;}
      void setTgcSetId( const int& i ){m_tgc_set_ext_id = i;}

      const std::string & tgcSetName() const {return m_tgc_set_name;}
      void setTgcSetName( const std::string& s ){m_tgc_set_name = s;}

      int rpcPt1Id() const {return m_rpc_pt1_ext_id;}
      void setRpcPt1Id( const int& i ){m_rpc_pt1_ext_id = i;}
      int rpcPt2Id() const {return m_rpc_pt2_ext_id;}
      void setRpcPt2Id( const int& i ){m_rpc_pt2_ext_id = i;}
      int rpcPt3Id() const {return m_rpc_pt3_ext_id;}
      void setRpcPt3Id( const int& i ){m_rpc_pt3_ext_id = i;}
      int rpcPt4Id() const {return m_rpc_pt4_ext_id;}
      void setRpcPt4Id( const int& i ){m_rpc_pt4_ext_id = i;}
      int rpcPt5Id() const {return m_rpc_pt5_ext_id;}
      void setRpcPt5Id( const int& i ){m_rpc_pt5_ext_id = i;}
      int rpcPt6Id() const {return m_rpc_pt6_ext_id;}
      void setRpcPt6Id( const int& i ){m_rpc_pt6_ext_id = i;}


      virtual void print(const std::string& indent="", unsigned int detail=1) const;

   private:
      bool m_rpc_avail;
      bool m_rpc_avail_online;
      bool m_tgc_avail;
      bool m_tgc_avail_online;

      std::string m_rpc_set_name;
      int m_rpc_set_ext_id;
      int m_rpc_pt1_ext_id;
      int m_rpc_pt2_ext_id;
      int m_rpc_pt3_ext_id;
      int m_rpc_pt4_ext_id;
      int m_rpc_pt5_ext_id;
      int m_rpc_pt6_ext_id;

      std::string m_tgc_set_name;
      int m_tgc_set_ext_id;
   };
}
#endif
