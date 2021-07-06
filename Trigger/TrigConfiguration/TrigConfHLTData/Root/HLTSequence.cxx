/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfHLTData/HLTSequence.h"
#include "TrigConfL1Data/DiffStruct.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "boost/lexical_cast.hpp"

using namespace std;
using namespace TrigConf;

TrigConf::HLTSequence::HLTSequence() :
   m_outputTE ( 0 ),
   m_topoStartTE ( 0 ),
   m_level( 0 )
{}

TrigConf::HLTSequence::HLTSequence( const vector<HLTTriggerElement*>& inputTEs,
                                    HLTTriggerElement* outputTE,
                                    const vector<string>& algorithms) :
   m_inputTEs  ( inputTEs ),
   m_outputTE  ( outputTE ),
   m_algorithms( algorithms )
{
   setName(outputTE->name());
}

TrigConf::HLTSequence::~HLTSequence( )
{
   for(HLTTriggerElement* te : m_inputTEs)
      delete te;
   delete m_outputTE;
   delete m_topoStartTE;
}

void
TrigConf::HLTSequence::set_outputTE( HLTTriggerElement* outputTE ) {
   m_outputTE = outputTE;
   setName(outputTE->name());
}

unsigned int
TrigConf::HLTSequence::hashId() const {
   return m_outputTE->hashId();
}


void TrigConf::HLTSequence::writeXML(std::ofstream & xmlfile) {

   xmlfile << "        <SEQUENCE";

   xmlfile << " output=\"" << m_outputTE->name()  <<  "\"";

   xmlfile << " input=\"";
   bool first = true;
   for(HLTTriggerElement* te: m_inputTEs) {
      if(first) {first=false;} else {xmlfile <<  " ";}
      xmlfile << te->name();
   }
   xmlfile << "\"";

   xmlfile << " algorithm=\"";   
   first = true;
   for(const string& alg: m_algorithms) {
      if(first) {first=false;} else {xmlfile <<  " ";}
      xmlfile << alg;
   }
   xmlfile << "\"";

   if (m_topoStartTE) {
      xmlfile << " topo_start_from=\"" << m_topoStartTE->name() << "\"";
   }

   xmlfile << "/> " << endl;
   return;
}

std::string
TrigConf::HLTSequence::concise() const {
   std::stringstream str;
   str << "(";
   for (TrigConf::HLTTriggerElement* inputTE : m_inputTEs) {
      str << inputTE->name() << ",";
   }
   str << outputTE()->name() << ")";
   return str.str();
}

TrigConf::DiffStruct*
TrigConf::HLTSequence::compareTo(const HLTSequence* o) const {
   DiffStruct * ds = new DiffStruct("SEQUENCE", name());
   ds->check("number_algs", (unsigned int)algorithms().size(), (unsigned int)o->algorithms().size() );
   ds->check("number_inputs", (unsigned int)inputTEs().size(), (unsigned int)o->inputTEs().size() );
   if(algorithms().size()==o->algorithms().size()) {
      for(size_t i=0; i<algorithms().size(); ++i)
         ds->check("alg_" + boost::lexical_cast<string,size_t>(i), algorithms()[i], o->algorithms()[i] );
   }
   if(ds->empty()) {
      delete ds; ds=0;
   }
   return ds;
}


void
TrigConf::HLTSequence::print(const std::string& indent, unsigned int detail) const {
   if(detail>=4) {
      cout << indent << "HLTSequence "; printNameIdV();
   }
   if(detail>=5) {
      cout << indent << "        inputTEs  : ";
      bool first = true;
      for(HLTTriggerElement* te: m_inputTEs) {
         if(first) {first=false;} else {cout << ", ";}
         cout << *te;
      }
      cout << endl;
      if (m_topoStartTE) {
         cout << indent << "        topoStartTE  " << *m_topoStartTE << endl;
      }
      cout << indent << "        algorithms: ";
      first = true;
      uint width = 0;
      for(const string& alg: m_algorithms) {
         width += alg.size();
         if(first) {first=false;} else {cout << ", "; width+=2; }
         if(width>=140) {
            cout << endl << indent << "                    ";
            width=0;
         }
         cout << alg;
      }
      cout << endl;
      cout << indent << "        outputTE   : " << outputTE()->name() << endl;
   }
}

std::ostream &
TrigConf::operator<<(std::ostream & o, const TrigConf::HLTSequence & s) {
   o << "- -- HLTSequence printout ---------------------------------- " << std::endl;
   o << "- -- inputTEs : ";
   bool first = true;
   for (TrigConf::HLTTriggerElement* inputTE : s.m_inputTEs) {
      if (first)
        first = false;
      else
        o << ", "; 
      o << *inputTE;
   }

   if (s.m_topoStartTE) {
      o << "\n";
      o << "- -- topoStartTE  ";
      o << *s.m_topoStartTE;
   }
   o << "\n- -- algorithms";
   first = true;
   for (const std::string& algorithm : s.m_algorithms) {
      if (first)
        first = false;
      else
        o << ", ";
      o << algorithm;
   }
   o << "\n- -- outputTE   : "; o << *s.outputTE();
   o << "\n- ---------------------------------------------------------- " << std::endl;
   return o;
}


string HLTSequence::__str__() const {
   stringstream s;
   s << *this;
   return s.str();
}
