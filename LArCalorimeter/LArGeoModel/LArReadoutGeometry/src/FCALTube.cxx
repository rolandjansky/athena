/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/FCALTube.h"

// Constructor
FCALTube::FCALTube(const FCALTile *tile
		   , const FCALHVLine& hvLine
		   , double x
		   , double y)
  : m_tile(tile)
  , m_hvLine(hvLine)
  , m_x(x)
  , m_y(y) 
{
}

// Destructor
FCALTube::~FCALTube () {
}

// Get the tube position (x)
double FCALTube::getXLocal() const {
  return m_x;
}

// Get the tube position (y)
double FCALTube::getYLocal() const {
  return m_y;
}

// Get the tile
const FCALTile *FCALTube::getTile() const {
  return m_tile;
}

// Get the High Voltage Line
const FCALHVLine& FCALTube::getHVLine() const {
  return m_hvLine;
}
