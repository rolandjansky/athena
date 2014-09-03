/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// RpcCoincMatrix.h

#ifndef MUONEVENTATHENAPOOL_RPCCOINMATRIX_P1H
#define MUONEVENTATHENAPOOL_RPCCOINMATRIX_P1H


#include <inttypes.h>
#include "MuonEventAthenaPool/RpcFiredChannel_p1.h"

/** Persistent representation of RpcCoinMatrix class
@author Edward.Moyse@cern.ch
*/
class RpcCoinMatrix_p1 : public std::vector<RpcFiredChannel_p1> {

public:
    friend class  RpcCoinMatrixCnv_p1;

    // default constructor
    RpcCoinMatrix_p1() 
    : std::vector<RpcFiredChannel_p1>(), m_id(0), m_onlineId(0), m_crc(0),
    m_fel1Id(0), m_febcId(0) { }
    
    /** virtual destructor for persistency*/
    virtual ~RpcCoinMatrix_p1() {}
    
    /**return the pad Identifier of this CM*/
    uint32_t identify() const { return m_id;}
    
    /** return the online identifier*/
    uint16_t onlineId() const {return m_onlineId; }
    
    uint16_t crc() const {return m_crc; }
    
    uint16_t fel1Id() const {return m_fel1Id; }
    
    uint16_t febcId() const {return m_febcId; }
    
 private:
  uint32_t m_id;
  uint16_t m_onlineId;
  uint16_t m_crc;
  uint16_t m_fel1Id;
  uint16_t m_febcId;

};

#endif






