/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _GeoStraightAccSection_h_
#define _GeoStraightAccSection_h_

#include "CLIDSvc/CLASS_DEF.h"
#include "GeoModelKernel/GeoXF.h"
/**
 * @brief Record of All Electrode Straight Pieces
 **/

/**
 * This class records the position and angles of all of the electrode straight 
 * sections.  Initialization is somewhat special.  After instantiation, 
 * one must set the position, angle, etc using one of two types of modifiers.  
 * The first type just records all of the values, so you are in fact filling 
 * an array. The second type uses transfunctions, so you are in fact providing 
 * the recipe to fill that same array, but the array is not filled (or even
 * allocated) till later
 **/

class GeoStraightAccSection {

 public:

  // Constructors:
  GeoStraightAccSection();

  // Destructor:
  ~GeoStraightAccSection();


  // Accessors:
  inline const double & XCent(int stackid, int cellid)      const;
  inline const double & YCent(int stackid, int cellid)      const;
  inline const double & Cosu(int stackid, int cellid)       const;
  inline const double & Sinu(int stackid, int cellid)       const;
  inline const double & HalfLength(int stackid, int cellid) const;


  // Modifiers (type 1)

  double & XCent(int stackid, int cellid) ;
  double & YCent(int stackid, int cellid) ;
  double & Cosu(int stackid, int cellid);
  double & Sinu(int stackid, int cellid);
  double & HalfLength(int stackid, int cellid);

  // Modifiers (type 2)

  void setHalfLength(int stackid, double halfLength);
  void setTransform (int stackid, GeoXF::TRANSFUNCTION TXE);

 private:

  class Clockwork;
  Clockwork *m_c;

  GeoStraightAccSection (const GeoStraightAccSection&);
  GeoStraightAccSection& operator= (const GeoStraightAccSection&);

};

CLASS_DEF(GeoStraightAccSection, 263768096, 1)

#include "LArReadoutGeometry/GeoStraightAccSection.icc"

#endif
