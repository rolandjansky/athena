/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthViews/SimpleView.h"
#include "AthViews/View.h"
using namespace SG;
View::View(const std::string& name ) {
  m_implementation = new SimpleView(name);
}

View::~View () {
  delete m_implementation;
}
