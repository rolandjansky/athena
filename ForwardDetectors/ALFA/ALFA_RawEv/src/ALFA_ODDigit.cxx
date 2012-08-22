/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 
#include "ALFA_RawEv/ALFA_ODDigit.h"
#include <iostream>

ALFA_ODDigit::ALFA_ODDigit():
  m_station(0),
  m_side(0),
  m_plate(0),
  m_fiber(0)
{}

ALFA_ODDigit::ALFA_ODDigit(const unsigned short station, const unsigned short side, const unsigned short plate, const unsigned short fiber): m_station(station), m_side(side), m_plate(plate), m_fiber(fiber) {}
