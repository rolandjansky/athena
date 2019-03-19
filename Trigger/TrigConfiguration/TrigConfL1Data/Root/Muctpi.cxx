/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/Muctpi.h"
#include <iostream>

using namespace std;

TrigConf::Muctpi::Muctpi() :
   L1DataBaseclass(), 
   m_LowptThreshold(0),
   m_HighptThreshold(0),
   m_MaxCand(0)
{}
  
void
TrigConf::Muctpi::print(const std::string& indent, unsigned int /*detail*/) const {
   cout << indent << "Muctpi: " << endl; 
   cout << indent << "\tlvl1 master table id:                " << lvl1MasterTableId() << endl; 
   printNameIdV(indent);
   cout << indent << "\tlow pt threshold:  " << m_LowptThreshold << endl;
   cout << indent << "\thigh pt threshold: " << m_HighptThreshold << endl;
   cout << indent << "\tmaximal cadidates: " << m_MaxCand << endl;
}  

void
TrigConf::Muctpi::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   indent(xmlfile, indentLevel, indentWidth) << "<MuctpiInfo name=\"" << name() 
                                             << "\" low_pt=\"" << m_LowptThreshold 
                                             << "\" high_pt=\"" << m_HighptThreshold
                                             << "\" max_cand=\"" << m_MaxCand << "\"/>" << endl;
}
