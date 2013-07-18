/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/FCALTube.h"



// Constructor
FCALTube::FCALTube(const FCALTile *tile, FCALHVLineConstLink hvLine, double x, double y):tile(tile),hvLine(hvLine),x(x), y(y) {
}

// Destructor
FCALTube::~FCALTube () {
}

// Get the tube position (x)
double FCALTube::getXLocal() const {
  return x;
}

// Get the tube position (y)
double FCALTube::getYLocal() const {
  return y;
}

// Get the tile
const FCALTile *FCALTube::getTile() const {
  return tile;
}

// Get the High Voltage Line
FCALHVLineConstLink FCALTube::getHVLine() const {
  return hvLine;
}
