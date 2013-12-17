/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostBase/WriteTrigConfig.h"

using namespace std;

//-----------------------------------------------------------------------------
Anp::WriteTrigConfig::WriteTrigConfig()
  :fDebug(false)
{
}

//-----------------------------------------------------------------------------
Anp::WriteTrigConfig::~WriteTrigConfig()
{
}

//-----------------------------------------------------------------------------
void Anp::WriteTrigConfig::Config(const Registry &reg)
{
  reg.Get("WriteTrigConfig", "Debug",      fDebug);
  reg.Get("WriteTrigConfig", "XmlPathLV1", fXmlPathLV1);
  reg.Get("WriteTrigConfig", "XmlPathHLT", fXmlPathHLT);
}

//-----------------------------------------------------------------------------
void Anp::WriteTrigConfig::Write(const std::vector<TrigConfChain> &cvec)
{
  //
  // Write out configuration
  //
  if(fXmlPathLV1.size() > 4) {
    std::ofstream outf(fXmlPathLV1.c_str());
    if(outf.is_open()) {
      WriteLV1(outf, cvec);
    }
  }

  if(fXmlPathHLT.size() > 4) {
    std::ofstream outf(fXmlPathHLT.c_str());
    if(outf.is_open()) {
      WriteHLT(outf, cvec);
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::WriteTrigConfig::WriteLV1(ostream &os, const std::vector<TrigConfChain> &cvec)
{
  //
  // Write HLT configuration
  //
  os << "<LVL1Config id=\"1\" name=\"\">" << endl
     << "  <TriggerMenu id=\"1\" name=\"\" version=\"1\">" << endl;
  
  for(unsigned i = 0; i < cvec.size(); ++i) {
    const TrigConfChain &conf = cvec.at(i);
    if(conf.getLevelId() != 1) continue;
 
    os << "    <TriggerItem ctpid=\"" << conf.getCounter() << "\" name=\"" << conf.getName() << "\">"
       << "</TriggerItem>" << endl;
  }

  os << "  </TriggerMenu>" << endl
     << "  <PrescaleSet name=\"\" version=\"1\">" << endl;
      
  for(unsigned i = 0; i < cvec.size(); ++i) {
    const TrigConfChain &conf = cvec.at(i);
    if(conf.getLevelId() != 1) continue;

    os << "   <Prescale ctpid=\"" << conf.getCounter() << "\" d=\"0\" m=\"0\" n=\"1\">"
       << conf.getPrescale() << "</Prescale>" << endl;
  }

  os << "  </PrescaleSet>" << endl
     << "</LVL1Config>" << endl;
}

//-----------------------------------------------------------------------------
void Anp::WriteTrigConfig::WriteHLT(ostream &os, const std::vector<TrigConfChain> &cvec)
{
  //
  // Write LV1 configuration
  //
  os << "<?xml version=\"1.0\" ?>" << endl
     << "<HLT_MENU menu_name=\"\" prescale_set_name=\"\">" << endl
     << "  <CHAIN_LIST>" << endl;

  unsigned nwidth = 0;
  for(unsigned i = 0; i < cvec.size(); ++i) {
    const TrigConfChain &conf = cvec.at(i);
    if(conf.getLevelId() != 2 && conf.getLevelId() != 3) { 
      continue;
    }

    nwidth = max<unsigned>(nwidth, conf.getName().size());
  }

  for(unsigned i = 0; i < cvec.size(); ++i) {
    const TrigConfChain &conf = cvec.at(i);
    if(conf.getLevelId() != 2 && conf.getLevelId() != 3) { 
      continue;
    }
    
    os << "    <CHAIN chain_counter=\"" << conf.getCounter() << "\"";

    if     (conf.getCounter() < 10)   os << "   ";
    else if(conf.getCounter() < 100)  os << "  ";
    else if(conf.getCounter() < 1000) os << " ";

    os << " chain_name=\""   << conf.getName() << "\"";
    for(unsigned w = conf.getName().size(); w <= nwidth; ++w) {
      os << " ";
    }

    os  << " pass_through=\"" << conf.getPassThrough() << "\""      
	<< " prescale=\""     << conf.getPrescale()    << "\">"
	<< "</CHAIN>" << endl;
  }

  os << "  </CHAIN_LIST>" << endl
     << "</HLT_MENU>" << endl;
}
