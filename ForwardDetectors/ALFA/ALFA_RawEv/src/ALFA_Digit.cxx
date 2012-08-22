/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 
#include "ALFA_RawEv/ALFA_Digit.h"
#include <iostream>

ALFA_Digit::ALFA_Digit():
  m_station(0), 
  m_plate(0),
  m_fiber(0)
{}

ALFA_Digit::ALFA_Digit(const unsigned short station, const unsigned short plate, const unsigned short fiber): m_station(station), m_plate(plate), m_fiber(fiber) {}
