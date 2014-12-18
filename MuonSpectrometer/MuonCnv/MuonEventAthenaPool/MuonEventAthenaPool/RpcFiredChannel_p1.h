/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RpcFiredChannel_p1.h

#ifndef MUONEVENTATHENAPOOL_RPCFIREDCHANNEL_P1H
#define MUONEVENTATHENAPOOL_RPCFIREDCHANNEL_P1H
#include <inttypes.h>

/** Persistent representation of RpcFiredChannel class
@author Edward.Moyse@cern.ch
*/
class RpcFiredChannel_p1 {
public:
    friend class  RpcFiredChannelCnv_p1;

    /** Default constructor.*/
    RpcFiredChannel_p1() : m_bcid(0), m_time(0), m_ijk(0), m_channel(0), m_ovl(0), m_thr(0) {}
    
    /** return bunch crossing id*/
    uint16_t bcid() const { return m_bcid; }
    
    /** return time*/
    uint16_t time() const { return m_time; }
    
    /** return ijk*/
    uint16_t ijk() const { return m_ijk; }
    
    /** return channel*/
    uint16_t channel() const { return m_channel; }
    
    /** return if channels in pivot that caused trigger were in
        chamber overlap region*/
    uint16_t ovl() const {return m_ovl; }
    
    /**return and/or of those hits in the pivot that caused a
        trigger, i.e. belong to a RPC track that was above the lowest (?) of
        the three trigger pt tresholds*/
    uint16_t thr() const {return m_thr; }
    
private:
    //Q. Do we need channel & thr + ovl (see ctors in transient class to see why I wonder). EJWM
    uint16_t m_bcid;
    uint16_t m_time;
    uint16_t m_ijk;
    uint16_t m_channel;
    uint16_t m_ovl;
    uint16_t m_thr;
    
};

#endif
