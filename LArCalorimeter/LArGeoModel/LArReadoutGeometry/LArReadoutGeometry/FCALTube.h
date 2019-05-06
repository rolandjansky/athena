/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARREADOUTGEOMETRY_FCALTUBE_H
#define LARREADOUTGEOMETRY_FCALTUBE_H

#include "GeoModelKernel/RCBase.h"
#include "LArHV/FCALHVLine.h"

/**
 * @class FCALTube
 * 
 * @brief This class represents an FCAL Tube. The tube has a position and it also has
 * links to the High Voltage Lines.
 */

class FCALTile;

class FCALTube : public RCBase {

 public:

  // Constructor
  FCALTube(const FCALTile *tile, const FCALHVLine& line, double x, double y);

  // Get the tube position (x)
  double getXLocal() const;

  // Get the tube position (y)
  double getYLocal() const;

  // Get the tile
  const FCALTile *getTile() const;

  // Get the High Voltage Line
  const FCALHVLine& getHVLine() const;

 private:

  FCALTube (const FCALTube & right);

  FCALTube & operator=(const FCALTube & right);

  virtual ~FCALTube();

  const FCALTile      *m_tile;        // link to the Tile
  const FCALHVLine&    m_hvLine;      // link to HVLine
  double               m_x;           // nominal x position
  double               m_y;           // nominal y position;
  
  friend class ImaginaryFriend;

};

#endif
