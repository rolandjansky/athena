/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/MuonThresholdValue.h"
#include <iostream>

using namespace std;

TrigConf::MuonThresholdValue::MuonThresholdValue() :
   TriggerThresholdValue()
{}

TrigConf::TriggerThresholdValue*
TrigConf::MuonThresholdValue::createCopy() const {
   return new MuonThresholdValue(*this);
}

void
TrigConf::MuonThresholdValue::print(const std::string& indent, unsigned int /*detail*/) const {
   cout << indent << "-----------------------" << endl; 
   cout << indent << "MuonThresholdValue: " << endl; 
   cout << indent << "\t id:       " << id() << endl; 
   cout << indent << "\t Name:     " << name() << endl;
   cout << indent << "\t Version:  " << version() << endl;
   cout << indent << "\t Priority: " << m_Priority << endl;
   cout << indent << "\t Type:     " << m_Type << endl;
   cout << indent << "\t ptcut:    " << m_Ptcut 
        << " count=" << thresholdValueCount() << endl;
}

void
TrigConf::MuonThresholdValue::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   indent(xmlfile, indentLevel, indentWidth) 
      << "<TriggerThresholdValue em_isolation=\"63\" etamin=\"-49\" etamax=\"49\" had_isolation=\"63\" had_veto=\"63\" name=\"" << name()
      << "\" phimin=\"0\" phimax=\"64\" priority=\""<< m_Priority
      << "\" thresholdval=\"" << m_Ptcut
      << "\" type=\"" << m_Type
      << "\" window=\"0\"/>"
      << endl;
}

