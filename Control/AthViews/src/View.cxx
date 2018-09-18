/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthViews/SimpleView.h"
#include "AthViews/View.h"
using namespace SG;
View::View(const std::string& name, bool AllowFallThrough ) {
  m_implementation = new SimpleView(name, AllowFallThrough);
}

View::View(const std::string& name, bool AllowFallThrough, std::string const& storeName) {
  m_implementation = new SimpleView(name, AllowFallThrough, storeName);
}

View::View(const View* original) {
  m_implementation = new SimpleView(original->m_implementation);
}

View::~View () {
  delete m_implementation;
}

std::string View::dump( const std::string& delim ) const {
  const SimpleView * impl = static_cast<const SimpleView*>( m_implementation );
  return impl->dump( delim );
}
