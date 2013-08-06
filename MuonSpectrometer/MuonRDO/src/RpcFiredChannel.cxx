/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RpcFiredChannel.cxx

#include "MuonRDO/RpcFiredChannel.h"
#include "GaudiKernel/MsgStream.h"

// default constructor

RpcFiredChannel::RpcFiredChannel()
  : m_bcid(0), m_time(0), m_ijk(0), m_channel(0), m_ovl(0), m_thr(0) {}

// constructor

RpcFiredChannel::RpcFiredChannel(ubit16 bcid, ubit16 time, 
				 ubit16 ijk, ubit16 channel)
  : m_bcid(bcid), m_time(time), m_ijk(ijk), m_channel(channel), m_ovl(0), m_thr(0) {}

RpcFiredChannel::RpcFiredChannel(ubit16 bcid, ubit16 time, 
				 ubit16 ijk, ubit16 thr, 
				 ubit16 ovl)
  : m_bcid(bcid), m_time(time), m_ijk(ijk), m_channel(0), m_ovl(ovl), m_thr(thr) {}
 
// public methods.


// private methods.

 MsgStream& operator << ( MsgStream& sl, const RpcFiredChannel& coll) 
 {
     sl << "RpcFiredChannel: "<<", ";
     sl << "bcid() = "<< coll.bcid()<<", ";
     sl << "time() = "<< coll.time()<<", ";
     sl << "ijk() = "<< coll.ijk()<<", ";
     sl << "channel() = "<< coll.channel()<<", ";
     sl << "ovl() = "<< coll.ovl()<<", ";
     sl << "thr() = "<< coll.thr()<<std::endl;
       return sl;
 }
 
 
 std::ostream& operator << ( std::ostream& sl, const RpcFiredChannel& coll) 
 {
     sl << "RpcFiredChannel: "<<", ";
     sl << "bcid() = "<< coll.bcid()<<", ";
     sl << "time() = "<< coll.time()<<", ";
     sl << "ijk() = "<< coll.ijk()<<", ";
     sl << "channel() = "<< coll.channel()<<", ";
     sl << "ovl() = "<< coll.ovl()<<", ";
     sl << "thr() = "<< coll.thr()<<std::endl;
     return sl;
 }
