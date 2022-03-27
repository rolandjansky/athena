/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>
#include <iostream>

#include "TrigConfHLTData/HLTPrescale.h"
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTChainList.h"

using namespace std;
using namespace TrigConf;

TrigConf::HLTPrescale::HLTPrescale(float prescale, float pass_through) :
   m_prescale(prescale),
   m_pass_through(pass_through)
{}


TrigConf::HLTPrescale::~HLTPrescale()
{}


TrigConf::HLTPrescale&
TrigConf::HLTPrescale::setStreamPrescale(const std::string& streamName, float ps) {
   m_stream_prescale[streamName] = ps;
   return *this;
}

TrigConf::HLTPrescale&
TrigConf::HLTPrescale::setRerunPrescale(const std::string& targetName, float ps) {
   m_rerun_prescale[targetName] = ps;
   return *this;
}

std::pair<bool, float>
TrigConf::HLTPrescale::getRerunPrescale(const std::string& targetName) const {
   std::unordered_map<std::string, float>::const_iterator i = m_rerun_prescale.find(targetName);
   bool found = (i!=m_rerun_prescale.end());
   float ps = found ? i->second : 0;
   return std::make_pair(found,ps);
}

std::pair<bool, float>
TrigConf::HLTPrescale::getStreamPrescale(const std::string& streamName) const {
   std::unordered_map<std::string, float>::const_iterator i = m_stream_prescale.find(streamName);
   bool found = i!=m_stream_prescale.end();
   float ps = found ? i->second : 0;
   return std::make_pair(found,ps);
}


//________________________________________________________________________________
std::ostream&
TrigConf::operator<<(std::ostream & o, const TrigConf::HLTPrescale & c) {

   o << "Prescale: " << setw(4) << c.prescale()
     << ", pass-through: " << setw(4) << c.pass_through()
     << endl;
   return o;
}


string HLTPrescale::__str__() const {
   stringstream s;
   s << *this;
   return s.str();
}
