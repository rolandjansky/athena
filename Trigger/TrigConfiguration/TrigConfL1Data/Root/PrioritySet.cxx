/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/PrioritySet.h"
#include <iostream>


using namespace std;

TrigConf::PrioritySet::PrioritySet()
   : L1DataBaseclass(),m_Priorities(N_PRIORITIES,"LOW")
{}
  
void
TrigConf::PrioritySet::setPriorities(const std::string p[], const unsigned int size) {
   if (size > N_PRIORITIES) std::cerr 
      << "setPriorities >> number of priorities too high: " << size << std::endl;
   for (unsigned int i=0; i<size; i++) m_Priorities.at(i) = p[i];
}

void
TrigConf::PrioritySet::setPriorities(const std::vector<std::string>& vec) {
   if (vec.size() > N_PRIORITIES) std::cerr 
      << "setPriorities >> number of priorities too high: " << vec.size() << std::endl;
   for (unsigned int i=0; i<vec.size(); ++i) m_Priorities.at(i) = vec[i];
}

void
TrigConf::PrioritySet::print(const std::string& indent, unsigned int detail) const {
   if(detail>=5) {
      cout << indent << "PrioritySet " << name() << endl;
      cout << indent << "lvl1MasterTableId:      " << lvl1MasterTableId() << endl; 
      unsigned int ctpid(0);
      for(const string& priority: m_Priorities)
         cout << "\t " << ctpid++ << "\t" << " priority = " << priority << endl;  
   }
} 


/**
 * Writes the Prescale item to the XML file.
 */
void
TrigConf::PrioritySet::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   indent(xmlfile, indentLevel, indentWidth)
      << "<PrioritySet name=\"" << name() << "\" version=\"" << version() << "\">"
      << endl;
   unsigned int ctpid(0);
   for(const string& priority : m_Priorities) {
      indent(xmlfile, indentLevel + 1, indentWidth)
         << "<Priority ctpid=\"" << ctpid++
         << "\" priority=\"" << priority << "\">"
         << priority << "</Priority>" << endl;
   }
   indent(xmlfile, indentLevel, indentWidth)
      << "</PrioritySet>" << endl;
}

