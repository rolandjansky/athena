/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TestGaudiProperty.h,v 1.2 2005-12-15 17:39:36 cranshaw Exp $

#ifndef ATLASHISTORY_TESTGAUDIPROPERTY_H
#define ATLASHISTORY_TESTGAUDIPROPERTY_H

//#include "GaudiKernel/HistoryObj.h"
#include "Gaudi/Property.h"

#include <string>
#include <vector>
#include <utility>
#include <iosfwd>

/** @class TestGaudiProperty TestGaudiProperty.h
 * 
 *  TestGaudiProperty class definition
 *
 *  @author: Charles Leggett
 *
 */

class TestGaudiProperty {

public:

  //typedef std::vector< std::pair<std::string,const Gaudi::Details::PropertyBase*> > PropertyList;
  //typedef std::vector< const Gaudi::Details::PropertyBase* > PropertyList;
  //typedef const Gaudi::Details::PropertyBase* PropertyList;

private:  // data

  //PropertyList m_props;
  //Property     m_p1;
  Gaudi::Details::PropertyBase*    m_pptr;
  const Gaudi::Details::PropertyBase*  m_cptr;
  std::vector<Gaudi::Details::PropertyBase*> m_ncprops;

public:  // functions

  // Constructor.
  TestGaudiProperty();

  // Destructor.
  ~TestGaudiProperty();

  //void addProperty( const std::string&, const Gaudi::Details::PropertyBase* );
  //void addProperty( const Gaudi::Details::PropertyBase* );

  //const PropertyList& properties() const { return m_props; }
  const Gaudi::Details::PropertyBase* property() const { return m_cptr; }

private:

};

// Output stream.
std::ostream& operator<<(std::ostream& lhs, const TestGaudiProperty& rhs);

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( TestGaudiProperty, 241994533, 1 )
#endif
