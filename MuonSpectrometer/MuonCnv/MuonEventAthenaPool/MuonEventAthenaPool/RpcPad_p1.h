/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RpcPad_p1.h

#ifndef MUONEVENTATHENAPOOL_RPCPAD_P1H
#define MUONEVENTATHENAPOOL_RPCPAD_P1H

#include "MuonEventAthenaPool/RpcCoinMatrix_p1.h"
#include <inttypes.h>
#include <vector>

class RpcPad_p1 : public std::vector<RpcCoinMatrix_p1> {
    
public:  
    
    /** Default constructor*/
    RpcPad_p1 () : m_id(0), m_idHash(0), m_onlineId(0), m_status(0), m_errorCode(0), m_sector(0)
     {}
    
    /** virtual destructor for persistency*/
    virtual ~RpcPad_p1() {}
    
    uint32_t identify() const { return m_id; }
    uint16_t status() const { return m_status; }
    uint16_t errorCode() const { return m_errorCode; }
    uint16_t onlineId() const {return m_onlineId; }
    int sector() const {return m_sector; }
    
    friend class RpcPadCnv_p1;
    
private:
  uint32_t m_id; 
  uint32_t m_idHash; 
  uint16_t m_onlineId;
  uint16_t m_status;
  uint16_t m_errorCode;
  /** @TODO why is this an int? EJWM*/
  int m_sector;
};

#endif




