/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLEXAMPLECONVERTER_EXAMPLEHIT_P1_H
#define ATHENAPOOLEXAMPLECONVERTER_EXAMPLEHIT_P1_H

/** @file ExampleHit_p1.h
 *  @brief This file contains the class definition for the ExampleHit_p1 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ExampleHit_p1.h,v 1.1 2008-03-25 22:03:54 gemmeren Exp $
 **/

#include <CLHEP/Geometry/Vector3D.h>
#include <string>

/** @class ExampleHit_p1
 *  @brief This class provides a dummy hit data object for AthenaPool.
 **/
class ExampleHit_p1 {

public: // Constructor and Destructor
   /// Default Constructor
   ExampleHit_p1() : m_vec(0.0, 0.0, 0.0), m_detector("") {}
   /// Destructor
   virtual ~ExampleHit_p1() {}

private:
   HepGeom::Vector3D<double> m_vec;
   std::string m_detector;
};
#endif
