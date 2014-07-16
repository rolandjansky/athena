/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LabelIndex.h

#include "JetEDM/LabelIndex.h"
#include <map>

using jet::LabelIndex;

typedef std::map<std::string, LabelIndex*> LIMap;
typedef LabelIndex::Index Index;
typedef LabelIndex::Label Label;

//**********************************************************************

LabelIndex& LabelIndex::mutable_instance(Label nam) {
  static LIMap lis;
  LIMap::iterator ili = lis.find(nam);
  if ( ili != lis.end() ) return *ili->second;
  LabelIndex* pli = new LabelIndex(nam);
  lis[nam] = pli;
  return *pli;
}
  
//**********************************************************************

const LabelIndex& LabelIndex::instance(Label nam) {
  return mutable_instance(nam);
}

//**********************************************************************

LabelIndex::LabelIndex(Label nam) : m_name(nam) { }

//**********************************************************************

Index LabelIndex::addLabel(Label lab) {
  Index idx = index(lab);
  if ( idx ) return idx;
  m_labs.push_back(lab);
  xAOD::JetConstitScale conscale = xAOD::CalibratedJetConstituent;
  if ( lab == "EMTopo" ) conscale = xAOD::UncalibratedJetConstituent;
  m_constitScales.push_back(conscale);
  return m_labs.size();
}

//**********************************************************************

Label LabelIndex::name() const {
  return m_name;
}

//**********************************************************************

Label LabelIndex::label(Index idx) const {
  if ( idx < 1 ) return "";
  if ( idx-1 > m_labs.size() ) return "";
  return m_labs[idx - 1];
}

//**********************************************************************

xAOD::JetConstitScale LabelIndex::constitScale(Index idx) const {
  if ( idx < 1 ) return xAOD::CalibratedJetConstituent;
  if ( idx-1 > m_constitScales.size() ) return xAOD::UncalibratedJetConstituent;
  return m_constitScales[idx - 1];
}

//**********************************************************************

Index LabelIndex::index(Label lab) const {
  for ( Index jdx=0; jdx<m_labs.size(); ++jdx ) {
    if ( m_labs[jdx] == lab ) return jdx + 1;
  }
  return 0;
}

//**********************************************************************

LabelIndex::LabelIndex(const LabelIndex&) : m_name("") { }

//**********************************************************************

LabelIndex& LabelIndex::operator=(const Label&) {
  return *this;
}

//**********************************************************************

Index LabelIndex::size() const { return m_labs.size() ; }
