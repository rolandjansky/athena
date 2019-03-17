/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/XsThresholdValue.h"
#include "TrigConfL1Data/L1DataDef.h"
#include <iostream>

TrigConf::XsThresholdValue::XsThresholdValue()
   : TriggerThresholdValue()
{}

TrigConf::TriggerThresholdValue*
TrigConf::XsThresholdValue::createCopy() const{
   return new XsThresholdValue(*this);
}

void
TrigConf::XsThresholdValue::print(const std::string& indent, unsigned int /*detail*/) const {
   std::cout << indent << "-----------------------" << std::endl; 
   std::cout << indent << "XsThresholdValue: " << std::endl; 
   printNameIdV(indent);
   std::cout << indent << "\t Type:    " << m_Type << std::endl;
   std::cout << indent << "\t ptcut:   " << m_Ptcut 
             << " count=" << thresholdValueCount() << std::endl;
}

void
TrigConf::XsThresholdValue::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   indent(xmlfile, indentLevel, indentWidth) 
      << "<TriggerThresholdValue em_isolation=\"63\" etamin=\"-49\" etamax=\"49\" had_isolation=\"63\" had_veto=\"63\" name=\"" << name()
      << "\" phimin=\"0\" phimax=\"64\" priority=\""<< m_Priority
      << "\" thresholdval=\"" << m_Ptcut
      << "\" type=\"" << m_Type
      << "\" window=\"0\"/>"
      << std::endl;
}
