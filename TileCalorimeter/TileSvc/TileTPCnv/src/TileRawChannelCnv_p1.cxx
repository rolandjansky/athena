///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileRawChannelCnv_p1.cxx 
// Implementation file for class TileRawChannelCnv_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 

#define private public
#define protected public
#include "TileEvent/TileRawChannel.h"
#undef private
#undef protected

#include "TileTPCnv/TileRawChannelCnv_p1.h"
//#include "iostream"

void
TileRawChannelCnv_p1::persToTrans(const TileRawChannel_p1* persObj, TileRawChannel* transObj, MsgStream &log)
{
  transObj->m_adc_hwid = HWIdentifier(Identifier32(persObj->m_channelID));

  unsigned int length = persObj->m_length;
  unsigned int l1 =  length        & 0xFF;
  unsigned int l2 = (length >> 8)  & 0xFF;
  unsigned int l3 = (length >> 16) & 0xFF;
  unsigned int l4 = (length >> 24) & 0xFF;
   
  std::vector<float>::const_iterator it = persObj->m_data.begin();

  if (l1>0) {
    transObj->m_amplitude.reserve(l1);
    for (unsigned int i=0; i<l1; ++i) {
      transObj->m_amplitude.push_back( *it++ );
    }
  } else {
    transObj->m_amplitude.push_back( 0.0 );
  }
  
  if (l2>0) {
    transObj->m_time.reserve(l2);
    for (unsigned int i=0; i<l2; ++i) {
      transObj->m_time.push_back( *it++ );
    }
  } else {
    transObj->m_time.push_back( 0.0 );
  }

  if (l3>0) {
    transObj->m_quality.reserve(l3);
    for (unsigned int i=0; i<l3; ++i) {
      transObj->m_quality.push_back( *it++ );
    }
  } else {
    transObj->m_quality.push_back( 0.0 );
  }

  if (l4>0) 
    transObj->m_pedestal = (*it++);
  else
    transObj->m_pedestal = 0.0;

  if (it != persObj->m_data.end()) {
    log << MSG::ERROR << "TileRawChannelCnv_p1::persToTrans wrong size of data vector: "
        << persObj->m_data.size() << MSG::hex << "0x" << persObj->m_length << MSG::dec << endreq;
  }
}


void
TileRawChannelCnv_p1::transToPers(const TileRawChannel* transObj, TileRawChannel_p1* persObj, MsgStream &/*log*/)
{
  persObj->m_channelID = transObj->m_adc_hwid.get_identifier32().get_compact();

  unsigned int l1 = transObj->m_amplitude.size(); if (l1>0xFF) l1=0xFF;
  unsigned int l2 = transObj->m_time.size();      if (l2>0xFF) l2=0xFF;
  unsigned int l3 = transObj->m_quality.size();   if (l3>0xFF) l3=0xFF;
  unsigned int l4 = 1;

  if (l1==1 && transObj->m_amplitude[0]==0.0) l1=0;
  if (l2==1 && transObj->m_time[0]==0.0)      l2=0;
  if (l3==1 && transObj->m_quality[0]==0.0)   l3=0;
  if (         transObj->m_pedestal==0.0)     l4=0;

  persObj->m_length = (l4<<24 | l3<<16 | l2<<8 | l1);
  
  if (persObj->m_length) {

    persObj->m_data.reserve(l1+l2+l3+l4);

    for (unsigned int i=0; i<l1; ++i) {
      persObj->m_data.push_back( transObj->m_amplitude[i] );
    }

    for (unsigned int i=0; i<l2; ++i) {
      persObj->m_data.push_back( transObj->m_time[i] );
    }

    for (unsigned int i=0; i<l3; ++i) {
      persObj->m_data.push_back( transObj->m_quality[i] );
    }

    if (l4>0) {
      persObj->m_data.push_back( transObj->m_pedestal );
    }

  } else {
    persObj->m_data.clear();
  }
  
//  if (l1==0 || l2==0 || l3==0 || l4==0)
//    std::cout << "id=0x" << std::hex << persObj->m_channelID  
//              << " length=0x" << persObj->m_length << std::dec 
//              << " amp=" <<transObj->m_amplitude[0]
//              << " time="<<transObj->m_time[0]
//              << " qual="<<transObj->m_quality[0]
//              << " ped=" <<transObj->m_pedestal
//              << std::endl;
}
