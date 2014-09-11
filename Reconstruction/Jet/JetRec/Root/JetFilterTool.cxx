/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/JetFilterTool.h"

#include <algorithm>

//**********************************************************************

namespace {

// selection functor
struct JetPtFilter {
  JetPtFilter(double pt) : m_ptcut(pt) {}
  bool operator()(const xAOD::Jet* j1){ return j1->pt() < m_ptcut;}
  double m_ptcut;
};

}  // end unnamed namespace

//**********************************************************************

JetFilterTool::JetFilterTool(const std::string& n)
: asg::AsgTool(n) ,m_ptMin(10000) {
  declareProperty("PtMin", m_ptMin);
}

//**********************************************************************

int JetFilterTool::modify(xAOD::JetContainer& jets) const {
  ATH_MSG_DEBUG(" Filtering at  : " << m_ptMin );
  
  xAOD::JetContainer::iterator itB = jets.begin();
  xAOD::JetContainer::iterator itE = jets.end();
  xAOD::JetContainer::iterator lastFiltered=std::remove_if(itB, itE, JetPtFilter(m_ptMin) );
  
  jets.erase( lastFiltered, itE );

  return 1;
}

//**********************************************************************

