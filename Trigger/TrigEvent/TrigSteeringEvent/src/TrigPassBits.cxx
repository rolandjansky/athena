/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TrigSteeringEvent/TrigPassBits.h"
TrigPassBits::TrigPassBits()
  : m_container_ptr(0), m_decisions(0) {}

TrigPassBits::TrigPassBits(unsigned sz, const void* cont)
  : m_container_ptr(cont),  m_decisions(sz) {}

void TrigPassBits::markPassing( unsigned position, const void* cont) {
  if (m_container_ptr && cont != m_container_ptr)
    throw std::runtime_error("When marking that object passed the trigger discovered that TrigPassBits was created for other container");

  if ( position >= m_decisions.size()) 
    throw std::runtime_error("When marking that object passed the trigger discovered to small bit storage");
  
  m_decisions[position] = true;
}

bool TrigPassBits::isPassing( unsigned position ) const {
  if (  position >= m_decisions.size()) 
    throw std::runtime_error("When checking that object passed the trigger discovered to small bit storage");

  return m_decisions[position];
}
