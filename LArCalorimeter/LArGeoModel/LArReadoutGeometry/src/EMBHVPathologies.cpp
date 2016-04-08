/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/EMBHVPathologies.h"
class EMBHVPathologies::Clockwork {
public:
  unsigned int electrodeNumber;
  bool isShorted;
  bool isDisconnected;
};

// Constructor:
EMBHVPathologies::EMBHVPathologies(int electrodeNumber,
				   bool isShorted,
				   bool isDisconnected) : RCBase(), m_c(new Clockwork()) {
  m_c->electrodeNumber=electrodeNumber;
  m_c->isShorted=false;
  m_c->isDisconnected=false;
};

// Destructor:
EMBHVPathologies::~EMBHVPathologies() {
  delete clockwork;
};

// The electrode number:
const unsigned int & EMBHVPathologies::electrodeNumber() const{
  return m_c->electrodeNumber;
}


// Is shorted:
const bool & EMBHVPathologies::isShorted() const {
  return m_c->isShorted;
}

// Is disconnected();
const bool & EMBHVPathologies::isDisconnected() const {
  return m_c->isDisconnected;
}


