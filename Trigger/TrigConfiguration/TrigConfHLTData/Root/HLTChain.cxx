/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTStreamTag.h"
#include "TrigConfHLTData/HLTTriggerType.h"
#include "TrigConfHLTUtils/HLTUtils.h"
#include "TrigConfL1Data/DiffStruct.h"
#include "TrigConfL1Data/HelperFunctions.h"

#include "boost/algorithm/string/erase.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;
using namespace TrigConf;

bool HLTChain_lt(const TrigConf::HLTChain* ch1, const TrigConf::HLTChain* ch2) {
   if(ch1!=0 && ch2!=0) { return *ch1 < *ch2; } 
   else { return ch1==0; }
}

TrigConf::HLTChain::HLTChain() :
   TrigConfData(),
   m_chain_hash_id(0),
   m_chain_counter(0),
   m_chain_version(0),
   m_level(""),
   m_lower_chain_name(""),
   m_lower_chain_counter(0),
   m_lower_chain_hash_id(0),
   m_EB_after_step(-1)
{}

HLTChain::HLTChain( const string& chain_name,
                    int chain_counter,
                    int chain_version,
                    const string& level,
                    const string& lower_chain_name,
                    int lower_chain_counter,
                    const vector<HLTSignature*>& signatureList ) :
   TrigConfData(chain_name),
   m_chain_counter      ( chain_counter),
   m_chain_version      ( chain_version),
   m_level              ( level ),
   m_lower_chain_name   ( lower_chain_name ),
   m_lower_chain_counter( lower_chain_counter ),
   m_EB_after_step      (-1 ),
   m_HLTSignatureList   ( signatureList )
{
   setName(chain_name);
   setVersion(chain_version);
   // create signalture id's (labels)
   createSignatureLabels();
   m_chain_hash_id       = HLTUtils::string2hash(chain_name);
   m_lower_chain_hash_id = HLTUtils::string2hash(m_lower_chain_name);
}

// copy constructor
HLTChain::HLTChain( const HLTChain& o ) :
   TrigConfData(o),
   m_chain_hash_id       ( o.m_chain_hash_id ),
   m_chain_counter       ( o.m_chain_counter ),
   m_chain_version       ( o.m_chain_version ),
   m_level               ( o.m_level ),
   m_lower_chain_name    ( o.m_lower_chain_name ),
   m_lower_chain_counter ( o.m_lower_chain_counter ),
   m_lower_chain_counters( o.m_lower_chain_counters ),
   m_EB_after_step       ( o.m_EB_after_step ),
   m_prescales           ( o.m_prescales ),
   m_groups              ( o.m_groups )
{
   // deep copy to ensure ownership
   for(HLTSignature* sig : o.m_HLTSignatureList)
      m_HLTSignatureList.push_back(new HLTSignature(*sig));

   for(HLTTriggerType* tt : o.m_HLTTriggerTypeList)
      m_HLTTriggerTypeList.push_back(new HLTTriggerType(*tt));

   for(HLTStreamTag* tt : o.m_streams_orig)
      m_streams_orig.push_back(new HLTStreamTag(*tt));

   for(HLTStreamTag* tt : o.m_streams)
      m_streams.push_back(new HLTStreamTag(*tt));

   typedef boost::unordered_map<std::string, HLTStreamTag*>::value_type value_type;
   for(value_type strstream : o.m_streams_map)
      m_streams_map.insert(value_type(strstream.first, new HLTStreamTag(*strstream.second) ) );
}


TrigConf::HLTChain::~HLTChain() {
   for(HLTSignature *sig : m_HLTSignatureList) delete sig;
   for(HLTStreamTag* s : m_streams) delete s;
   for(HLTStreamTag* s : m_streams_orig) delete s;
}


bool
TrigConf::HLTChain::hasMultipleLowerChains() const {
   return m_lower_chain_name.find(',')!=std::string::npos;
}


const std::vector<int>&
TrigConf::HLTChain::lower_chain_counters() const {
   return m_lower_chain_counters;
}


namespace TrigConf {
   std::vector<std::string>
   parse(std::string names) {
      TrigConf::removeAllSpaces(names);
      return TrigConf::split(names,",");
   }
}

vector<unsigned int>
TrigConf::HLTChain::lower_chain_hash_ids() const {
   vector<unsigned int> ids;
   vector<std::string> names = parse(m_lower_chain_name);
   for ( unsigned i = names.size(); i-- ; ) { 
      ids.push_back(HLTUtils::string2hash(names[i]));
   }
   return ids;
}


TrigConf::HLTChain&
TrigConf::HLTChain::set_chain_name(const string& chain_name) {
   m_chain_hash_id = HLTUtils::string2hash(chain_name);
   setName(chain_name);
   return *this;
}

// @brief sets lower chain name
// also clears the list of lower chain names (for multiple lower chains)
// adapts the hash_id(s)
TrigConf::HLTChain&
TrigConf::HLTChain::set_lower_chain_name(const string& lower_chain_name) {
   m_lower_chain_name = lower_chain_name;
   m_lower_chain_hash_id = HLTUtils::string2hash(m_lower_chain_name);
   return *this;
}

TrigConf::HLTChain&
TrigConf::HLTChain::set_signatureList(const vector<HLTSignature*>& sigList ) {
   m_HLTSignatureList = sigList;
   createSignatureLabels();
   return *this;
}

unsigned int
HLTChain::lastStep() const {
   if(m_HLTSignatureList.size()==0) 
      return 0;
   return m_HLTSignatureList[m_HLTSignatureList.size()-1]->step();
}

void
HLTChain::shiftStepCounter(int shift) {
   for(HLTSignature* sig : m_HLTSignatureList)
      sig->set_signature_counter( sig->signature_counter() + shift );
}


void
TrigConf::HLTChain::createSignatureLabels() {
   for (HLTSignature* signature : m_HLTSignatureList) {
      std::stringstream os;
      os << chain_name() << "_" << signature->signature_counter();
      signature->set_label( os.str() );
   }
}

namespace {
   TrigConf::HLTStreamTag * findStreamTag(const vector<HLTStreamTag*> & streams, const std::string& streamName) {
      for(auto stream : streams) {
         if(stream->name() == streamName)
            return stream;
      }
      for(auto stream : streams) {
         const string &s(stream->name());
         if( auto p = s.find('.'); (p != string::npos) && 
             (s.compare(p+1, streamName.size(), streamName) == 0) )
            return stream;
      }
      return nullptr;
   }
}

TrigConf::HLTChain&
TrigConf::HLTChain::set_prescales( const HLTPrescale& prescales) { 

   // copy the original (menu) stream prescales to the live set
   for(HLTStreamTag* stream : m_streams_orig) {
      HLTStreamTag* target = m_streams_map[stream->name()];
      target->set_prescale(stream->prescale());
   }

   // copy the stream prescales from the new HLTPprescale to the live set

   for(HLTPrescale::value_type ps : prescales.getStreamPrescales()) {
      const string & streamName = ps.first;

      HLTStreamTag* streamTag = findStreamTag(m_streams, streamName);

      if(streamTag) {
         streamTag->set_prescale(ps.second);
      } else {
         //throw runtime_error(string("Can not set prescale for stream ")+ ps.first + "since chain '" + name() + "' does not write to that stream.");
         cerr << "TrigConf::HLTChain        WARNING  Can not set prescale for stream '" + streamName + "' since chain '" + name() + "' does not write to that stream. Available are" << endl;
         for(auto stream : m_streams) {
            cout << "    Available are " << stream->name() << endl;
         }
      }
   }
   m_prescales = prescales;
   return *this;
}


void
TrigConf::HLTChain::addStream(HLTStreamTag* stream) {
   if(m_streams_map.find(stream->name()) != m_streams_map.end())
      throw runtime_error(string("Can not add stream '")+ stream->name() + "' to chain '" + name() + "', since it exists already.");

   m_streams_orig.push_back(stream);
   HLTStreamTag *cp = new HLTStreamTag(*stream);
   m_streams.push_back(cp);
   m_streams_map.insert(std::pair<std::string, HLTStreamTag*>(cp->name(),cp));

   // copy the stream prescales from the prescales to the live prescales
   std::pair<bool, float> sps = prescales().getStreamPrescale(cp->name());
   if(sps.first)
      cp->set_prescale(sps.second);
}

void
TrigConf::HLTChain::clearStreams() {
   for(HLTStreamTag *s : m_streams_orig) delete s;
   for(HLTStreamTag *s : m_streams) delete s;
   m_streams_orig.clear();
   m_streams.clear();
}

std::pair<bool, float>
TrigConf::HLTChain::stream_prescale(const std::string& streamName) const { 
   boost::unordered_map<std::string, HLTStreamTag*>::const_iterator i = m_streams_map.find(streamName);
   bool found = (i!=m_streams_map.cend());
   float ps = found ? i->second->prescale() : 0;
   return std::make_pair(found,ps);
}

namespace {
   string orderLowerChainNames(const string& vstr) {
      if(vstr.find(',')==string::npos) return vstr;
      vector<string> splstr = split( boost::erase_all_copy(vstr," "), "," );
      sort(splstr.begin(),splstr.end());
      string out("");
      for(const string& s : splstr)
         out += s;
      return out;
   }
}


TrigConf::DiffStruct*
TrigConf::HLTChain::compareTo(const HLTChain* o) const {
   DiffStruct * ds = new DiffStruct("CHAIN");
   ds->check("chain_counter", chain_counter(), o->chain_counter());
   ds->check("lower_chain_name", orderLowerChainNames(lower_chain_name()), orderLowerChainNames(o->lower_chain_name()) );
   ds->check("level", level(), o->level());
   ds->check("prescale", prescales().prescale(), o->prescales().prescale());
   ds->check("pass_through", prescales().pass_through(), o->prescales().pass_through());
//    for(StreamTag* st : streamTagList()) {
//       o->streamTagList() st->name()

//    }


   if(!ds->empty()) {
      ds->name = name();
   } else {
      delete ds; ds=0;
   }
   return ds;

}

//________________________________________________________________________________
void
TrigConf::HLTChain::writeXML(std::ofstream & xmlfile) {
   xmlfile << "        <CHAIN chain_counter=\"" << chain_counter() << "\" "  
           << "chain_name=\"" <<  name()  << "\" "  
           << "level=\"" <<  m_level  << "\" "  
           << "lower_chain_name=\"" << m_lower_chain_name  << "\" "  
           << "pass_through=\"" <<  prescales().pass_through()  << "\" "  
           << "prescale=\"" <<  prescales().prescale()  << "\">"  
           << endl;
  
   xmlfile << "            <PRESCALES"
           << " chain_counter=\"" << chain_counter() << "\""
           << " level=\"" <<  m_level  << "\""
           << " prescale=\"" <<  prescales().prescale()  << "\""  
           << " pass_through=\"" <<  prescales().pass_through()  << "\"";
   if(prescales().getRerunPrescales().size()==0) {
      xmlfile << "/>" << endl;
   } else {
      xmlfile << ">" << endl;
      for(HLTPrescale::value_type rerun_ps : prescales().getRerunPrescales())
         xmlfile << "                <RERUN_PRESCALES target=\"" << rerun_ps.first << "\" prescale=\"" << rerun_ps.second << "\"/>" << endl;     
      xmlfile << "            </PRESCALES>" << endl;
   }

   xmlfile << "            <TRIGGERTYPE_LIST/>" << endl;
    
   xmlfile << "            <STREAMTAG_LIST>" << endl;
   for (const TrigConf::HLTStreamTag* streamTag : m_streams)
      streamTag->writeXML(xmlfile);
   xmlfile << "            </STREAMTAG_LIST>" << endl;
    
   xmlfile << "            <GROUP_LIST>" << endl;  
   for (const string& group : m_groups)
      xmlfile << "                <GROUP name=\"" << group << "\"/>" << endl; 
   xmlfile << "            </GROUP_LIST>" << endl;  

   xmlfile << "            <SIGNATURE_LIST>" << endl;    
   for(TrigConf::HLTSignature* signature : m_HLTSignatureList)
      signature->writeXML(xmlfile);
   xmlfile << "            </SIGNATURE_LIST>" << endl;  
    
   xmlfile << "        </CHAIN>" << endl;
    
   return;
}

void
TrigConf::HLTChain::print(const std::string& indent, unsigned int detail) const {
   if(detail>=2) {
      int dp = cout.precision();
      cout.unsetf(ios_base::floatfield);
      cout.precision(3);
      //cout.setf(std::ios::fixed, std::ios::floatfield); 

      cout << indent << "HLTChain chainid = " << chain_counter() << "  ";
      printNameIdV();
      if(detail>=3) {
         cout << " | counter: " << chain_counter();
         if(level()!="HLT") {
            cout << " | " << level()
                 << " | lower chain: "    << lower_chain_name()
                 << " | lower chain counter: " << lower_chain_counter();
         } else {
            cout << " | CTP input: "    << lower_chain_name()
                 << " | lower chain counter: " << lower_chain_counter();            
         }
         cout << " | prescale: " << prescale()
              << " | pass through: " << pass_through();

         cout << " | rerun: " << prescales().getRerunPrescales().size() << " [ ";
         for(HLTPrescale::value_type rrps : prescales().getRerunPrescales())
            cout << rrps.first << ":" << rrps.second << " ";
         cout << "]" << std::endl;

      }
      if(detail>=4) {
         cout << indent << "        Groups : " << groups().size() << " [ ";
         for(const string& group : groups()) cout << "'" << group << "' ";
         cout << "]" << std::endl;

         cout << indent << "        Trigger type bits : " << triggerTypeList().size() << " [ ";
         for(HLTTriggerType* tt : triggerTypeList()) cout << tt->bit() << " ";
         cout << "]" << std::endl;

         cout << indent << "        Streams : " << streams().size() << endl;
         for(HLTStreamTag* streamTag : streams()) cout << indent << "            " << *streamTag;

         cout << indent << "        Steps : " << signatureList().size() << endl;
         for(HLTSignature* sig : signatureList()) sig->print(indent);
         cout << indent << "---------------------------------------------------------- " << endl;
      }
      
      cout.precision(dp);
   }
}


//________________________________________________________________________________
std::ostream &
TrigConf::operator<<(std::ostream & o, const TrigConf::HLTChain & c) {
   int dp = o.precision();
   o << "- -- HLTChain printout ------------------------------------- " << endl;
   o << "- --   name='"               << c.name()
     << "' [v "                    << c.m_chain_version << "]"
     << " | counter: "             << c.m_chain_counter
     << " | level: "               << c.m_level
     << " | lower_chain_name: "    << c.m_lower_chain_name
     << " | lower_chain_counter: " << c.m_lower_chain_counter;
   if (c.m_level == "HLT")  o << " | EB_after_step:  " << c.m_EB_after_step;
   
   o.precision(3);
   o.setf(std::ios::fixed, std::ios::floatfield); 
   o << " | prescale: "            << c.prescale()
     << " | pass_through: "        << c.pass_through();
   o << " | rerun: " << c.prescales().getRerunPrescales().size() << " [ ";
   for(TrigConf::HLTPrescale::value_type rrps : c.prescales().getRerunPrescales()) o << rrps.first << ":" << rrps.second << " ";
   o << "]" << endl;
   o << endl;

   set<std::string>::const_iterator group = c.m_groups.begin();
   o << "- -- Chain groups : " << c.m_groups.size() << "  [ ";
   for (; group   != c.m_groups.end();   ++group)   o << "'" << *group << "' ";
   o << "]" << std::endl;

   vector<TrigConf::HLTTriggerType*>::const_iterator triggerType = c.m_HLTTriggerTypeList.begin();
   o << "- -- Trigger type bits : " << c.m_HLTTriggerTypeList.size() << "  [ ";
   for (; triggerType != c.m_HLTTriggerTypeList.end(); ++triggerType) o << (*triggerType)->bit() << " ";
   o << "]" << std::endl;


   vector<TrigConf::HLTStreamTag*>::const_iterator streamTag = c.m_streams.begin();
   o << "- -- Trigger streams : " << c.m_streams.size() << std::endl;
   for (; streamTag   != c.m_streams.end();   ++streamTag)   o << *(*streamTag);

   vector<TrigConf::HLTSignature*>::const_iterator signature = c.m_HLTSignatureList.begin();
   for (; signature   != c.m_HLTSignatureList.end();   ++signature)   o << *(*signature);
   o << "- ---------------------------------------------------------- " << endl;

   o.precision(dp);
   return o;
}

string HLTChain::__str__() const {
   stringstream s;
   s << *this;
   return s.str();
}
