/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/Random.h"
#include "TrigConfL1Data/PrescaleSet.h"
#include <iostream>
#include <math.h>

using namespace std;

TrigConf::Random::Random() : L1DataBaseclass(),
                             m_rate1( 0 ), m_rate2( 0 ),
                             m_names{ "Random0", "Random1",
                                      "Random2", "Random3" },
                             m_cuts{ 0xffffff, 0xffffff, 0xffffff,
                                     0xffffff }
{}

float
TrigConf::Random::getRateFromCut(unsigned int idx) const {
  int32_t cut = m_cuts[idx];
  float   ucut = abs(cut);
  return ( 0x1000000 / ( ucut + 1 ) );
}

void
TrigConf::Random::print(const std::string& indent, unsigned int detail) const {
   if(detail>=1) {
      cout << indent << "Random "; printNameIdV();
      if(detail>=2) {
         if( m_cuts[0]!=0xffffffff ||
             m_cuts[1]!=0xffffffff ||
             m_cuts[2]!=0xffffffff ||
             m_cuts[3]!=0xffffffff )
         {
            for(int  i=0; i<4; i++) {
               cout << indent << "    " << m_names[i] << ": cut=" << m_cuts[i] << " (prescale=" << PrescaleSet::getPrescaleFromCut(m_cuts[i]) << ")" << endl;
            }
         } else {
            cout << indent << "        Rate 1: " << m_rate1 << endl;
            cout << indent << "        Rate 2: " << m_rate2 << endl;
         }
      }
   }
}  

void
TrigConf::Random::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   if( m_cuts[0]!=0xffffffff ||
       m_cuts[1]!=0xffffffff ||
       m_cuts[2]!=0xffffffff ||
       m_cuts[3]!=0xffffffff )
   {
      indent(xmlfile,indentLevel, indentWidth) 
         << "<Random "
         << "name0=\""<< m_names[0] << "\" cut0=\"" << m_cuts[0] << "\" "
         << "name1=\""<< m_names[1] << "\" cut1=\"" << m_cuts[1] << "\" "
         << "name2=\""<< m_names[2] << "\" cut2=\"" << m_cuts[2] << "\" "
         << "name3=\""<< m_names[3] << "\" cut3=\"" << m_cuts[3] << "\"/>" << endl;
   } else {
      indent(xmlfile,indentLevel, indentWidth) 
         << "<Random name=\"" << name() << "\" rate1=\"" << m_rate1
         << "\" rate2=\"" << m_rate2
         << "\" seed1=\"1\" seed2=\"1\"/>" << endl;
   }
}
