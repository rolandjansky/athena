/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace TrigConf;

HLTSignature::HLTSignature() :
   m_signature_counter(1),
   m_logic(1),
   m_label("")
{}

HLTSignature::HLTSignature( unsigned int signature_counter, int logic,
                            vector<HLTTriggerElement*>&& outputTEs) :
   m_signature_counter(signature_counter),
   m_logic(logic),
   m_outputTEs(std::move(outputTEs)),
   m_label("")
{}


HLTSignature::HLTSignature( const HLTSignature& o) :
   m_signature_counter( o.m_signature_counter ),
   m_logic( o.m_logic ),
   m_label( o.m_label )
{
   // deep copy to ensure ownership (maybe should use shared_ptr in the future)
   for(HLTTriggerElement* te : o.m_outputTEs)
      m_outputTEs.push_back(new HLTTriggerElement(*te));
}


TrigConf::HLTSignature::~HLTSignature() {
   for(HLTTriggerElement* te : m_outputTEs) delete te;
}


void TrigConf::HLTSignature::writeXML(std::ofstream & xmlfile)
{
  xmlfile << "                <SIGNATURE logic=\"" << m_logic << "\" "  
	  << "signature_counter=\"" <<  m_signature_counter  << "\">" 
	  << std::endl;
    
    //write TEs
  for (TrigConf::HLTTriggerElement* outputTE : m_outputTEs) {
    outputTE->writeXML(xmlfile);
  }
  
  xmlfile << "                </SIGNATURE>" << std::endl;

  return;
}

void
TrigConf::HLTSignature::print(const std::string& indent, unsigned int /*detail*/) const {
   cout << indent << "signature " << signature_counter()
        << " (logic=" << logic() << ") output TEs ("<< m_outputTEs.size() <<"):";

   for( HLTTriggerElement* te : m_outputTEs ) {
      if( te!=0 ) cout << *te << " "; else cout << "(0) ";
   }
   cout << endl;
}

std::ostream &
TrigConf::operator<<(std::ostream & o, const TrigConf::HLTSignature & s) {
   o << "signature: " << s.signature_counter() << " (logic=" << s.logic() << ")" << endl;
   o << "outputTE(s)";

   for( HLTTriggerElement* te : s.m_outputTEs ) {
      o << ", ";
      if( te!=0 ) o << *te; else o << "(0)";
   }
   o << endl;
   return o;
}

