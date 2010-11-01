/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLEXAMPLECONVERTER_EXAMPLEHIT_P0_H
#define ATHENAPOOLEXAMPLECONVERTER_EXAMPLEHIT_P0_H

/** @file ExampleHit_p0.h
 *  @brief This file contains the class definition for the ExampleHit_p0 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ExampleHit_p0.h,v 1.4 2009-02-25 14:48:44 gemmeren Exp $
 **/

#include <string>

/** @class ExampleHit_p0
 *  @brief This class provides a dummy hit data object for AthenaPool.
 **/
class ExampleHit_p0 {

public: // Constructor and Destructor
   /// Default Constructor
   ExampleHit_p0() : m_detector(""), m_x(0.0), m_y(0.0), m_z(0.0) {}
   /// Destructor
   virtual ~ExampleHit_p0() {}

friend class ExampleHitStreamer_p0;

public: // Non-static members
   /// @return the X coordinate.
   double getX() const { return(m_x); }
   /// @return the Y coordinate.
   double getY() const { return(m_y); }
   /// @return the Z coordinate.
   double getZ() const { return(m_z); }

   /// @return the detector string.
   const std::string getDetector() const { return(m_detector); }

private:
   std::string m_detector;
   double m_x;
   double m_y;
   double m_z;
};
#endif
