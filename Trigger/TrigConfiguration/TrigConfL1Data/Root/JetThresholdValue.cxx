/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/JetThresholdValue.h"
#include "TrigConfL1Data/CaloInfo.h"
#include <iostream>

using namespace std;
using namespace TrigConf;

TrigConf::JetThresholdValue::JetThresholdValue() :
   TriggerThresholdValue()
{}

TrigConf::TriggerThresholdValue*
TrigConf::JetThresholdValue::createCopy() const{
   return new JetThresholdValue(*this);
}

int
JetThresholdValue::thresholdValueCount() const {
   int count = static_cast<int>(m_Ptcut * caloInfo().globalJetScale() );
   if (count > fgCaloJetOff) {
      count = fgCaloJetOff;
   }
   return count;
}

JetWindowSize::Size
JetThresholdValue::windowSize() const {
   if(m_WindowSize != JetWindowSize::NONE)
      return m_WindowSize;

   if (window() == caloInfo().jetWindowSizeLarge() ) {
      return JetWindowSize::LARGE;
   } else if (window() == caloInfo().jetWindowSizeSmall() ) {
      return JetWindowSize::SMALL;
   } else {
      return JetWindowSize::UNDEF;
   }
}

std::string
JetThresholdValue::windowSizeAsString() const {
   JetWindowSize::Size ws = windowSize();
   if(ws == JetWindowSize::LARGE)
      return "LARGE";
   if(ws == JetWindowSize::SMALL)
      return "SMALL";
   if(ws == JetWindowSize::NONE)
      return "NONE";
   return "UNDEF";
}



// prints the DBMucptiConfig object
void
TrigConf::JetThresholdValue::print(const std::string& indent, unsigned int /*detail*/) const {
   cout << indent << "--------------------" << endl; 
   cout << indent << "JetThresholdValue: " << endl; 
   cout << indent << "\t id:       " << id() << endl; 
   cout << indent << "\t Name:     " << name() << endl;
   cout << indent << "\t Version:  " << version() << endl;
   cout << indent << "\t Type:     " << m_Type << endl;
   cout << indent << "\t Priority: " << m_Priority << endl;
   cout << indent << "\t ptcut:    " << m_Ptcut 
        << " count=" << thresholdValueCount() << endl;
   cout << indent << "\t window:   " << m_Window << endl;
   cout << indent << "\t phi_min:  " << m_PhiMin << endl;
   cout << indent << "\t phi_max:  " << m_PhiMax << endl;
   cout << indent << "\t eta_min:  " << m_EtaMin << endl;
   cout << indent << "\t eta_max:  " << m_EtaMax << endl;
}

void
TrigConf::JetThresholdValue::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   indent(xmlfile, indentLevel, indentWidth) 
      << "<TriggerThresholdValue"
      << " em_isolation=\"63\""
      << " etamin=\""        << m_EtaMin       << "\""
      << " etamax=\""        << m_EtaMax       << "\""
      << " had_isolation=\"63\""
      << " had_veto=\"63\""
      << " name=\""          << name()         << "\""
      << " phimin=\""        << m_PhiMin       << "\""
      << " phimax=\""        << m_PhiMax       << "\""
      << " priority=\""      << m_Priority     << "\""
      << " thresholdval=\""  << m_Ptcut        << "\""
      << " type=\""          << m_Type         << "\""
      << " window=\""        << m_Window       << "\""
      << " windowSize=\""        << windowSizeAsString() << "\"" << "/>"
      << endl;
}
