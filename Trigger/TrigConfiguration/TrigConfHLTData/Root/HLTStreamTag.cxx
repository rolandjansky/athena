/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfHLTData/HLTStreamTag.h"
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

TrigConf::HLTStreamTag::HLTStreamTag() :
   m_stream(""),
   m_type(""),
   m_obeyLB(false),
   m_prescale(1.)
{}

TrigConf::HLTStreamTag::HLTStreamTag(const std::string& stream,
                                     const std::string& type,
                                     bool obeyLB,
                                     float prescale) :
   m_stream(stream),
   m_type(type),
   m_obeyLB(obeyLB),
   m_prescale(prescale)
{
   setName(type+"."+stream);
}


bool TrigConf::HLTStreamTag::operator==(const HLTStreamTag & o) {
  return ( stream()   == o.stream() &&
           type()     == o.type()   &&
           obeyLB()   == o.obeyLB() &&
           prescale() == o.prescale() );
}


void TrigConf::HLTStreamTag::writeXML(std::ofstream & xmlfile) const {
   xmlfile << "                <STREAMTAG"
           << " stream=\"" <<  m_stream  << "\""
           << " type=\"" << m_type << "\""
           << " prescale=\"" << m_prescale  << "\""
           << " obeyLB=\"" << (m_obeyLB?"yes":"no") << "\""
           << "/>"  
           << endl;
}


void
TrigConf::HLTStreamTag::print(const std::string& indent, unsigned int /*detail*/) const {
   std::cout << indent << *this;
}



std::ostream &
TrigConf::operator<<(std::ostream & o, const TrigConf::HLTStreamTag & st) {
   int dp = o.precision();
   o << "HLTStreamTag "
     << "stream: '"   << st.m_stream 
     << "' type: '"   << st.m_type
     << "' obeyLB: '"  << (st.m_obeyLB?"yes":"no");
   o.precision(3);
   o.setf(std::ios::fixed, std::ios::floatfield); 
   o << "' prescale: " << st.m_prescale
     << std::endl;
   o.precision(dp);
   return o;
}

