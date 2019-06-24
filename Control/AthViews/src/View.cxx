/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthViews/SimpleView.h"
#include "AthViews/View.h"
using namespace SG;

View::View(const std::string& name, const int index, const bool AllowFallThrough, std::string const& storeName) {
  if ( index == -1 ) {
    m_index = 0;
    m_implementation = new SimpleView(name, AllowFallThrough, storeName);
  }
  else {
    m_index = index;
    m_implementation = new SimpleView(name+"_"+std::to_string(index), AllowFallThrough, storeName);
  }
}

View::~View () {
  delete m_implementation;
}
