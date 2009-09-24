/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLEXAMPLEDATA_EXAMPLEHIT_H
#define ATHENAPOOLEXAMPLEDATA_EXAMPLEHIT_H

/** @file ExampleHit.h
 *  @brief This file contains the class definition for the ExampleHit class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ExampleHit.h,v 1.9 2009-02-23 16:10:30 gemmeren Exp $
 **/

#include "GaudiKernel/DataObject.h"
#include "Navigation/NavigableTerminalNode.h"
#include <CLHEP/Geometry/Vector3D.h>

#include <string>

/** @class ExampleHit
 *  @brief This class provides a dummy hit data object for AthenaPool.
 *  @brief The new ExampleHit after schema evolution: 3 doubles were replaced by CLHEP::Vector3D<double>
 **/
class ExampleHit : public DataObject, public NavigableTerminalNode {

public: // Constructor and Destructor
   /// Default Constructor
   ExampleHit() : m_vec(0.0, 0.0, 0.0), m_detector("") {}
   /// Destructor
   virtual ~ExampleHit() {}

public: // Non-static members
   /// @return the X coordinate.
   double getX() const { return(m_vec.x()); }

   /// @return the Y coordinate.
   double getY() const { return(m_vec.y()); }

   /// @return the Z coordinate.
   double getZ() const { return(m_vec.z()); }

   /// @return the detector string.
   const std::string& getDetector() const { return(m_detector); }

   /// Set the X coordinate.
   /// @param x [IN] X coordinate.
   void setX(double x) { m_vec.setX(x); }

   /// Set the Y coordinate.
   /// @param y [IN] Y coordinate.
   void setY(double y) { m_vec.setY(y); }

   /// Set the Z coordinate.
   /// @param z [IN] Z coordinate.
   void setZ(double z) { m_vec.setZ(z); }

   /// Set the detector string.
   /// @param detector [IN] detector string.
   void setDetector(std::string detector) { m_detector = detector; }

private:
   HepGeom::Vector3D<double> m_vec;
   std::string m_detector;
};
#endif
