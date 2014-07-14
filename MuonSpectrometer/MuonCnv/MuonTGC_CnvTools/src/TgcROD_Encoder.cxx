/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Implementation of TgcROD_Encoder class

#include "TgcROD_Encoder.h"

#include "MuonRDO/TgcRdo.h"
#include "TgcByteStream.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include <iostream>

// constructor
Muon::TgcROD_Encoder::TgcROD_Encoder() : m_tgcRdo(0)
{}


// destructor
Muon::TgcROD_Encoder::~TgcROD_Encoder() {}


// Set TgcRdo
void Muon::TgcROD_Encoder::setRdo(const TgcRdo* rdo) {
  m_tgcRdo = rdo;
}

// convert TGC RDO to a vector of 32bit words
void Muon::TgcROD_Encoder::fillROD(std::vector<uint32_t>& v)
{
  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("MessageSvc", msgSvc);
  if(sc.isFailure()) {
    std::cout << "TgcROD_Encoder::fillROD msgSvc unavilable " << std::endl;
    return;
  }
  MsgStream log(msgSvc, "TgcROD_Encoder::fillROD");
  bool debug = (log.level() <= MSG::DEBUG);

  // ROD encoder
  //   // set RDO and encode to ROD data
  std::vector<uint32_t> t_vData;

  TgcByteStream bs;
  bs.rdo2ByteStream(m_tgcRdo, t_vData, log);

  // append to vector and dump
  std::vector<uint32_t>::const_iterator it   = t_vData.begin();
  std::vector<uint32_t>::const_iterator it_e = t_vData.end();
  uint32_t iWord=0;
  if(debug) {
    log << MSG::DEBUG << "************** dumping the words ************" << endreq;
  }
  for(; it!=it_e; ++it) {
    v.push_back(*it);
    if(debug) {
      log << MSG::DEBUG << "word " 
	  << MSG::dec << std::setw(3) << iWord << " = " 
	  << MSG::hex << std::setw(8) << *it 
	  << endreq;
    }
    ++iWord;
  }

  return;
}
