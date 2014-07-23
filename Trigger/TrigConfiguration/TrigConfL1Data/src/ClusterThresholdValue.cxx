/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/ClusterThresholdValue.h"
#include <iostream>

using namespace std;

TrigConf::ClusterThresholdValue::ClusterThresholdValue() :
   TriggerThresholdValue(), 
   m_EmIsolation(0), 
   m_HadIsolation(0), 
   m_HadVeto(0)
{}

TrigConf::ClusterThresholdValue::~ClusterThresholdValue()
{}  


TrigConf::TriggerThresholdValue*
TrigConf::ClusterThresholdValue::createCopy() const {
   return new ClusterThresholdValue(*this);
}
 
int
TrigConf::ClusterThresholdValue::emIsolationCount() const {
   int count = static_cast<int>(m_EmIsolation*caloGlobalScaleGeVToCount());
   if (count > fgCaloIsolationOff) count = fgCaloIsolationOff;
   return count;
}

int
TrigConf::ClusterThresholdValue::hadIsolationCount() const {
   int count = static_cast<int>(m_HadIsolation*caloGlobalScaleGeVToCount());
   if (count > fgCaloIsolationOff) count = fgCaloIsolationOff;
   return count;
}

int
TrigConf::ClusterThresholdValue::hadVetoCount() const {
   int count = static_cast<int>(m_HadVeto*caloGlobalScaleGeVToCount());
   if (count > fgCaloIsolationOff) count = fgCaloIsolationOff;
   return count;
}

int
TrigConf::ClusterThresholdValue::thresholdValueCount() const {
   int count = static_cast<int>(m_Ptcut*caloGlobalScaleGeVToCount());
   if (count > fgCaloClusterOff) count = fgCaloClusterOff;
      return count;
}

void
TrigConf::ClusterThresholdValue::print(const std::string& indent, unsigned int /*detail*/) const {
   cout << indent << "-----------------------" << endl; 
   cout << indent << "ClusterThresholdValue: " << endl; 
   printNameIdV(indent);
   cout << indent << "\t Priority:      " << m_Priority << endl;
   cout << indent << "\t Type:          " << m_Type << endl;
   cout << indent << "\t ptcut:         " << m_Ptcut 
        << " count=" << thresholdValueCount() << endl;
   cout << indent << "\t em_isolation:  " << m_EmIsolation 
        << " count=" << emIsolationCount() << endl;
   cout << indent << "\t had_isolation: " << m_HadIsolation 
        << " count=" << hadIsolationCount() << endl;
   cout << indent << "\t had_veto:      " << m_HadVeto 
        << " count=" << hadVetoCount() << endl;
   cout << indent << "\t phi_min:       " << m_PhiMin << endl;
   cout << indent << "\t phi_max:       " << m_PhiMax << endl;
   cout << indent << "\t eta_min:       " << m_EtaMin << endl;
   cout << indent << "\t eta_max:       " << m_EtaMax << endl;
}

void
TrigConf::ClusterThresholdValue::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   indent(xmlfile, indentLevel, indentWidth) 
      << "<TriggerThresholdValue"
      << " em_isolation=\""  << m_EmIsolation  << "\""
      << " etamin=\""        << m_EtaMin       << "\""
      << " etamax=\""        << m_EtaMax       << "\""
      << " had_isolation=\"" << m_HadIsolation << "\""
      << " had_veto=\""      << m_HadVeto      << "\""
      << " name=\""          << name()         << "\""
      << " phimin=\""        << m_PhiMin       << "\""
      << " phimax=\""        << m_PhiMax       << "\""
      << " priority=\""      << m_Priority     << "\""
      << " thresholdval=\""  << m_Ptcut        << "\""
      << " type=\""          << m_Type         << "\""
      << " window=\"0\"/>"
      << endl;
}

