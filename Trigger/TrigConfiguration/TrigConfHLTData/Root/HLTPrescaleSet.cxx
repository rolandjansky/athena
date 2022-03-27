/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>
#include <iostream>

#include "TrigConfHLTData/HLTPrescaleSet.h"
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTChainList.h"

using namespace std;
using namespace TrigConf;

TrigConf::HLTPrescaleSet::HLTPrescaleSet() :
   m_iovstart(0),
   m_iovend(0)
{}

TrigConf::HLTPrescaleSet::~HLTPrescaleSet() {}

TrigConf::HLTPrescale&
TrigConf::HLTPrescaleSet::setPrescale(const HLTPrescale& sc, unsigned int chain_counter, HLTLevel level) {
   ScalingMap_t& map = m_scalers[static_cast<unsigned int>(level)];
   ScalingMap_t::iterator s = map.find(chain_counter);
   if(s==map.end()) {
      s = map.insert(ScalingMap_t::value_type(chain_counter, sc)).first;
   } else {
      s->second = sc;
   }
   return s->second;
}


bool
TrigConf::HLTPrescaleSet::hasPrescale( unsigned int counter, HLTLevel level ) const {
   const ScalingMap_t& map_for_level = m_scalers[static_cast<unsigned int>(level)];
   return !map_for_level.empty() && map_for_level.count(counter)>0;
}


const TrigConf::HLTPrescale&
TrigConf::HLTPrescaleSet::getPrescale(unsigned int chain_counter, HLTLevel level) const {
   const ScalingMap_t& map = m_scalers[static_cast<unsigned int>(level)];
   ScalingMap_t::const_iterator s = map.find(chain_counter);
   if(s==map.end()) {
      cerr << "HLTPrescaleSet::getPrescale: chain counter " << chain_counter << " in level " << level << " has no prescales" << endl;
      throw std::runtime_error("HLTPrescaleSet::getPrescale: chain has no prescales defined");
   }
   return s->second;
}

TrigConf::HLTPrescale&
TrigConf::HLTPrescaleSet::thePrescale(unsigned int chain_counter, HLTLevel level) {
   ScalingMap_t& map = m_scalers[static_cast<unsigned int>(level)];
   ScalingMap_t::iterator s = map.find(chain_counter);
   if(s==map.end())
      s = map.insert(ScalingMap_t::value_type(chain_counter, HLTPrescale())).first;

   return s->second;
}


//________________________________________________________________________________
void
TrigConf::HLTPrescaleSet::setIOV(uint32_t start_run, uint32_t start_lb, uint32_t end_run, uint32_t end_lb ) {
   m_iovstart = start_run; m_iovstart <<= 32; m_iovstart += start_lb;
   m_iovend   = end_run;   m_iovend   <<= 32; m_iovend   += end_lb;
}

//________________________________________________________________________________
void
TrigConf::HLTPrescaleSet::setIOV(uint64_t start, uint64_t end ) {
   m_iovstart = start;
   m_iovend   = end;
}

//________________________________________________________________________________
// note that end_run/end_lb specifies the first lb for which the HLT PSS is not valid anymore (COOL design)  
void
TrigConf::HLTPrescaleSet::getIOV(uint32_t& start_run, uint32_t& start_lb, uint32_t& end_run, uint32_t& end_lb ) const {
   start_lb  = m_iovstart & 0xFFFFFFFF;
   start_run = m_iovstart>>32;
   end_lb    = m_iovend & 0xFFFFFFFF;
   end_run   = m_iovend>>32;
}

//________________________________________________________________________________
bool
TrigConf::HLTPrescaleSet::isValid(uint32_t run, uint32_t lb ) const {
   unsigned long long iovtime = run;
   iovtime <<= 32;
   iovtime += lb;
   
   return iovtime >= m_iovstart && iovtime<m_iovend; // m_iovend is not part of the IOV
}

//________________________________________________________________________________
void
TrigConf::HLTPrescaleSet::reset() {
   for(unsigned int i = 0; i<3; ++i )
      m_scalers[i].clear();
}

//________________________________________________________________________________
void TrigConf::HLTPrescaleSet::print(const std::string& indent, unsigned int detail) const {
   if(detail>=1) {
      cout << indent << "HLTPrescaleSet "; printNameIdV();
      cout << indent << "  L2 scalers  " << size(L2) << endl
           << indent << "  EF scalers  " << size(EF) << endl
           << indent << "  HLT scalers " << size(HLT) << endl;
      if(detail>=3) {
         cout.unsetf(ios_base::floatfield);
         for(unsigned int i=L2; i<=HLT; i++) {
            HLTLevel level = HLTLevel(i);
            if(size(level)==0) continue;
            if(level!=HLT)
               cout << indent << "        " << (level==L2?"L2":"EF") << " prescales:" << endl;

            const ScalingMap_t& map = m_scalers[static_cast<unsigned int>(level)];

            for(uint cc=0; cc<8192;cc++) {
               auto s_iter = map.find(cc);
               if(s_iter==map.end()) continue;
               const HLTPrescale& s = s_iter->second;
               cout << indent << "    Chain counter: " << setw(4) << cc
                    << ", prescale: " << setw(4) << s.prescale()
                    << ", pass-through: " << setw(4) << s.pass_through();
               if( s.getRerunPrescales().size()>0 ) {
                  cout << ", rerun prescales: ";
                  for( HLTPrescale::PrescaleMap_t::value_type ps: s.getRerunPrescales())
                     cout << ps.first << ": " << ps.second << ", ";
               }
               if( s.getStreamPrescales().size()>0 ) {
                  cout << ", stream prescales: ";
                  for( HLTPrescale::PrescaleMap_t::value_type str_ps: s.getStreamPrescales())
                     cout << str_ps.first << ": " << str_ps.second << ", ";
               }
               cout << endl;
            }
         }
      }
   }
}


//________________________________________________________________________________
std::ostream&
TrigConf::operator<<(std::ostream & o, const TrigConf::HLTPrescaleSet & c) {
   o << "HLTPrescaleSet " << c.name();
   if(c.id()>0 || c.version()>0) o << " (id=" << c.id() << "/v=" << c.version() << ")";
   o << endl;
   if(c.comment()!="") o << "Comment    :    " << c.comment() << endl;

   for(unsigned int i = TrigConf::L2; i <= TrigConf::HLT; i++) {
      TrigConf::HLTLevel level = TrigConf::HLTLevel(i);
      if(c.size(level)==0) continue;
      o << (level==TrigConf::L2?"L2":"EF") << " prescales:" << endl;
      for(const TrigConf::HLTPrescaleSet::ScalingMap_t::value_type& sc: c.getPrescales(level)) {
         o << "Chain counter: " << setw(4) << sc.first
           << ", prescale: " << setw(4) << sc.second.prescale()
           << ", pass-through: " << setw(4) << sc.second.pass_through()
           << endl;
      }
   }
   o << "---------------------------------------------------------- " << endl;
   return o;
}

string HLTPrescaleSet::__str__() const {
   stringstream s;
   s << *this;
   return s.str();
}
