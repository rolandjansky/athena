/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
//
// $Id: TestGaudiProperty.cxx,v 1.3 2006-08-02 09:36:39 schaffer Exp $
//
// GaudiHistory/TestGaudiProperty.cpp
//
// Contains history information for a job. Imports all environment vars
// and info from uname
//
//
// @author: Charles Leggett
//
///////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/System.h"
#include "Gaudi/Property.h"
#include "AthenaPoolTestData/TestGaudiProperty.h"

#include <cstdlib>
#include <iostream>

using std::string;
using std::ostream;
using std::endl;
using std::vector;

//**********************************************************************
// Member functions.
//**********************************************************************

// Constructor.

TestGaudiProperty::TestGaudiProperty() : m_pptr(0), m_cptr(0)
{
  m_ncprops.push_back(m_pptr);
}

//**********************************************************************

// Destructor.

TestGaudiProperty::~TestGaudiProperty() {
}
/*
void
//TestGaudiProperty::addProperty(const std::string& client, const Gaudi::Details::PropertyBase* prop) {
TestGaudiProperty::addProperty(const Gaudi::Details::PropertyBase* prop) {
//  if (m_props.find(prop) == m_props.end()) {
    //m_props.push_back( prop);
    //m_ncprops.push_back( const_cast<Gaudi::Details::PropertyBase*>(prop));
    //m_p1 = *prop;
    //m_pptr = m_ncprops[0];
    m_cptr = prop;
    //m_cptr = m_props[0];
//  }
}
*/

//**********************************************************************
// Free functions.
//**********************************************************************

// Output stream.

ostream& operator<<(ostream& lhs, const TestGaudiProperty& /*rhs*/) {
  lhs << "Property: [" << endl;
  lhs << "DUMB:  ";
  //rhs.property()->nameAndValueAsStream(lhs);
  lhs << endl;
  lhs << "]" << endl;
  return lhs;
}

//**********************************************************************
