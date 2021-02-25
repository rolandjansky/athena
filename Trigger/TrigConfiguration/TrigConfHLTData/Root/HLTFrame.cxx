/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfHLTData/HLTFrame.h"

#include "TrigConfL1Data/DiffStruct.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTSequence.h"
#include "TrigConfHLTData/HLTSequenceList.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace TrigConf;


// constructor
HLTFrame::HLTFrame() :
   TrigConfData()
{}


// destructor
HLTFrame::~HLTFrame()
{}


void
HLTFrame::clear() {
   theHLTChainList().clear();
   theHLTSequenceList().clear();
   thePrescaleSetCollection().clear();

   setId(0);
   setName("");
   setVersion(0);
}

bool
HLTFrame::equals(const HLTFrame* other, const std::string& filename) const {
   bool equals = true;
   DiffStruct* ds = compareTo(other);
   if(ds) {
      equals = false;
      std::ofstream xmlfile;
      xmlfile.open(filename.c_str());
      // file will be empty if there are no differences
      xmlfile << "<?xml version=\"1.0\" ?>" << std::endl;
      ds->writeXML(xmlfile);
      xmlfile.close();
      delete ds;
   }
   return equals;
}


TrigConf::DiffStruct*
HLTFrame::compareTo(const HLTFrame* o) const {
   DiffStruct * ds = new DiffStruct("HLT_MENU");
   ds->name = name();
   ds->check("menu_name", name(), o->name());
   std::string pss_name(""), o_pss_name("");
   if(getPrescaleSetCollection().size()==1)
      pss_name= getPrescaleSet()->name();
   if(o->getPrescaleSetCollection().size()==1)
      o_pss_name= o->getPrescaleSet()->name();
   ds->check("prescale_set_name", pss_name, o_pss_name);

   ds->addSub( getHLTSequenceList().compareTo( & o->getHLTSequenceList()) );
   ds->addSub( getHLTChainList().compareTo( & o->getHLTChainList()) );

   if(ds->empty()) {
      delete ds; ds=0;
   }
   return ds;
}


void
HLTFrame::print(const std::string& indent, unsigned int detail) const {
   if(detail>=1) {
      cout << indent << "================================================================================" << endl;
      cout << indent << "HLT Configuration";
      if(smk()>0) cout << " (SMK = " << smk() << ")";
      cout << ": "; printNameIdV(indent);
      getHLTChainList().print(indent, detail);
      getHLTSequenceList().print(indent, detail);
      getPrescaleSetCollection().print(indent, detail);
      cout << indent << "================================================================================" << endl;
   }
}

void
HLTFrame::writeXML(const string & filename) const {
   std::ofstream xmlfile;
   xmlfile.open(filename.c_str());
   xmlfile << "<?xml version=\"1.0\" ?>" << std::endl;
   writeXML(xmlfile);
   xmlfile.close();
}


void
HLTFrame::writeXML(std::ofstream & xmlfile) const {

   string pss_name("");
   if(m_hltPrescaleSets.size()>1) {
      pss_name = "multiple";
   } else if(m_hltPrescaleSets.size()==0) {
      pss_name = "none";
   } else if(m_hltPrescaleSets.prescaleSet(0)==0) {
      pss_name = "not loaded";
   } else {
      pss_name = m_hltPrescaleSets.prescaleSet(0)->name();
   }

   xmlfile << "<HLT_MENU  menu_name=\"" << name() << "\" prescale_set_name=\"" << pss_name << "\">" << endl
           << "    <SEQUENCE_LIST>" << endl;

   for(HLTSequence *seq: getHLTSequenceList())
      seq->writeXML(xmlfile);

   xmlfile << "    </SEQUENCE_LIST>" << endl
           << "    <CHAIN_LIST>" << endl; 

   for(HLTChain *ch: getHLTChainList())
      ch->writeXML(xmlfile);

   xmlfile << "    </CHAIN_LIST>" << endl
           << "</HLT_MENU>" << endl;
}


std::ostream &
TrigConf::operator<<(std::ostream & o, const TrigConf::HLTFrame & f) {
   o << "HLTFrame print: " << std::endl
     << "Running " << (f.mergedHLT() ? "":" not ") << "merged HLT" << std::endl
     << f.getHLTChainList()
     << f.getHLTSequenceList();
   return o;
}


// for python
vector<TrigConf::HLTChain*>
HLTFrame::chainsV() const {
   vector<HLTChain*> ch(chains().size());
   copy(chains().begin(), chains().end(), ch.begin());
   return ch;
}

vector<TrigConf::HLTSequence*>
HLTFrame::sequencesV() const {
   vector<HLTSequence*> seq(sequences().size());
   copy(sequences().begin(), sequences().end(), seq.begin());
   return seq;
}

string HLTFrame::__str__() const {
   stringstream s;
   s << *this;
   return s.str();
}
