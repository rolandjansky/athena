/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLEXAMPLECONVERTER_EXAMPLETRACK_P1_H
#define ATHENAPOOLEXAMPLECONVERTER_EXAMPLETRACK_P1_H

/** @file ExampleTrack_p1.h
 *  @brief This file contains the class definition for the ExampleTrack_p1 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ExampleTrack_p1.h,v 1.1 2008-03-25 22:03:54 gemmeren Exp $
 **/

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLink_p3.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"
#include "DataModelAthenaPool/Navigable_p2.h"

#include <CLHEP/Geometry/Vector3D.h>
#include <string>

/** @class ExampleTrack_p1
 *  @brief This class provides a dummy hit data object for AthenaPool.
 **/
class ExampleTrack_p1 {

public: // Constructor and Destructor
   /// Default Constructor
   ExampleTrack_p1() : m_pt(0.0), m_eta(0.0), m_phi(0.0), m_detector(""), m_elementlink1(), m_elementlink2(), m_elementlinkvector(), m_navigable(), m_weightednavigable() {}
   /// Destructor
   virtual ~ExampleTrack_p1() {}

private:
   double m_pt;
   double m_eta;
   double m_phi;
   std::string m_detector;

   ElementLinkInt_p3 m_elementlink1, m_elementlink2;
   ElementLinkIntVector_p1 m_elementlinkvector;
   Navigable_p2<uint32_t> m_navigable;
   Navigable_p2<uint32_t, double> m_weightednavigable;
};
#endif
