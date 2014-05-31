/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawEvent/LArTTL1.h"

LArTTL1::LArTTL1(const HWIdentifier& onlineId, const Identifier& offlineId, const std::vector<float>& sampleValues):  
  m_onlineId(onlineId) ,
  m_offlineId(offlineId) ,
  m_samples(sampleValues) 
  {  }

const HWIdentifier& 
LArTTL1::ttOnlineID() const 
{ return m_onlineId; }

const Identifier& 
LArTTL1::ttOfflineID() const 
{ return m_offlineId; }

short 
LArTTL1::nsamples() const 
{ return m_samples.size(); }

const std::vector<float> & 
LArTTL1::samples() const 
{ return m_samples; }

LArTTL1::~LArTTL1() 
{ }

LArTTL1::LArTTL1() 
{ }

