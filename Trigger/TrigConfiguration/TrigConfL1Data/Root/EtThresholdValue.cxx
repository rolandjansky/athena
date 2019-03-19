/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/EtThresholdValue.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/CaloInfo.h"
#include <iostream>

using namespace std;

TrigConf::EtThresholdValue::EtThresholdValue() :
   TriggerThresholdValue()
{}

TrigConf::TriggerThresholdValue*
TrigConf::EtThresholdValue::createCopy() const{
   return new EtThresholdValue(*this);
}

int
TrigConf::EtThresholdValue::thresholdValueCount() const {

   int count = static_cast<int>( m_Ptcut * caloInfo().globalJetScale() );

   if (type()==L1DataDef::jeType() && count > fgCaloJetEtOff) {
      count = fgCaloJetEtOff;
   } else if (type()==L1DataDef::teType() && count > fgCaloEtSumOff) {
      count = fgCaloEtSumOff;
   } else if (type()==L1DataDef::xeType() && count > fgCaloEtMissOff) {
      count = fgCaloEtMissOff;
   }

   return count;
}

void
TrigConf::EtThresholdValue::print(const std::string& indent, unsigned int /*detail*/) const {
   cout << indent << "-----------------------" << endl; 
   cout << indent << "EtThresholdValue: " << endl; 
   printNameIdV(indent);
   cout << indent << "\t Type:    " << m_Type << endl;
   cout << indent << "\t ptcut:   " << m_Ptcut
        << " count=" << thresholdValueCount() << endl;
   cout << indent << "\t etamin:  " << m_EtaMin << endl;
   cout << indent << "\t etamax:  " << m_EtaMax << endl;
   cout << indent << "\t phimin:  " << m_PhiMin << endl;
   cout << indent << "\t phimax:  " << m_PhiMax << endl;
   cout << indent << "\t priority:" << m_Priority << endl;
}

void
TrigConf::EtThresholdValue::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   indent(xmlfile, indentLevel, indentWidth) 
      << "<TriggerThresholdValue em_isolation=\"63\" etamin=\"" << m_EtaMin
      << "\" etamax=\"" << m_EtaMax
      << "\" had_isolation=\"63\" had_veto=\"63\" name=\"" << name()
      << "\" phimin=\"" << m_PhiMin
      << "\" phimax=\"" << m_PhiMax
      << "\" priority=\""<< m_Priority
      << "\" thresholdval=\"" << m_Ptcut
      << "\" type=\"" << m_Type
      << "\" window=\"0\"/>"
      << endl;
}

