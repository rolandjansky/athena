/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthViews/SimpleView.h"
#include "AthViews/View.h"
using namespace SG;
View::View(const std::string& name, bool AllowFallThrough ) {
  m_implementation = new SimpleView(name, AllowFallThrough);
}

View::~View () {
  delete m_implementation;
}
