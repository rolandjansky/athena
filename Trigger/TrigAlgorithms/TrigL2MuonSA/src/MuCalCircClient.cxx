/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <string>

#include "circ/Circ.h"
#include "circ/Circservice.h"

#include "MuCalDecode/CalibEvent.h"
#include "MuCalDecode/CalibUti.h"

#include "MuCalCircClient.h"

TrigL2MuonSA::MuCalCircClient::MuCalCircClient (unsigned short port, std::string &buffer_name, unsigned int buffer_size)
  : m_port (port), m_bufferName (buffer_name), m_bufferSize (buffer_size)
{

  std::vector<char> writable(m_bufferName.begin(), m_bufferName.end());
  writable.push_back('\0');

  //  char *name = const_cast <char *> (m_bufferName.c_str()); 
  if ( ( m_cid = CircOpenCircConnection_t (m_port, &*writable.begin(), m_bufferSize) ) < 0)
  {
    TrigL2MuonSA::Circexception e;
    throw e;
  }
}

TrigL2MuonSA::MuCalCircClient::~MuCalCircClient ()
{
  std::vector<char> writable(m_bufferName.begin(), m_bufferName.end());
  writable.push_back('\0');
  if (m_cid != -1)
    CircCloseCircConnection_t (m_port,  &*writable.begin() , m_cid);
}

bool TrigL2MuonSA::MuCalCircClient::dumpToCirc (LVL2_MUON_CALIBRATION::CalibEvent &event) const
{
  char *ptr;
  bool success = false;
  uint16_t event_size = event.size ();


  if ((ptr = CircReserve_t (m_cid, event_size )) != (char *) -1)
  {

    uint8_t *buff = reinterpret_cast <uint8_t *> (ptr);
    event.dumpWords (buff, event_size);

    (void) CircValidate_t (m_cid, ptr, event_size);
    success = true;
  }

  return success;
}
