/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfHLTData/HLTPrescaleSetCollection.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"

#include <iomanip>
#include <stdexcept>
#include <iostream>

using namespace std;
using namespace TrigConf;

TrigConf::HLTPrescaleSetCollection::HLTPrescaleSetCollection()
   : m_prescaleSets(),
     m_prescaleSetCollection_mutex()
{}

HLTPrescaleSetCollection::~HLTPrescaleSetCollection() {
   clear();
}

HLTPrescaleSetCollection::HLTPrescaleSetCollection(const HLTPrescaleSetCollection &) {
   // don't use
   throw std::runtime_error("HLTPrescaleSetCollection::operator= is not implemented");
}

HLTPrescaleSetCollection &
HLTPrescaleSetCollection::operator=(const HLTPrescaleSetCollection &) {
   // don't use
   throw std::runtime_error("HLTPrescaleSetCollection::operator= is not implemented");
   return *this;
}


void
HLTPrescaleSetCollection::clear() {
   lock_guard<recursive_mutex> lock(m_prescaleSetCollection_mutex);
   for(const cont& psinfo : m_prescaleSets)
      delete psinfo.pss;
   m_prescaleSets.clear();
}



const TrigConf::HLTPrescaleSet*
TrigConf::HLTPrescaleSetCollection::prescaleSet(unsigned int lumiblock) const {
   return thePrescaleSet(lumiblock);
}

TrigConf::HLTPrescaleSet*
TrigConf::HLTPrescaleSetCollection::thePrescaleSet(unsigned int lumiblock) const {

   lock_guard<recursive_mutex> lock(m_prescaleSetCollection_mutex);

   if(m_prescaleSets.empty())
      return nullptr;

   // check that the first LB of the prescale set collection (minLB) is <= than the requested lumiblock
   unsigned int minLB = m_prescaleSets.begin()->lb;
   if(minLB > lumiblock)
      throw runtime_error("HLTPrescaleSetCollecion access with LBN which is smaller than the first prescale set");

   // look for the right prescale set
   HLTPrescaleSet* pss = m_prescaleSets.begin()->pss;

   for(const cont& psinfo : m_prescaleSets) {
      if( psinfo.lb > lumiblock ) break;
      pss = psinfo.pss;
   }

   // found the PSS that covers the requested LB
   return pss;
}



TrigConf::HLTPrescaleSet*
TrigConf::HLTPrescaleSetCollection::setPrescaleSet( HLTPrescaleSet* pss ) {
   /*
    * Lock mutex while we are inserting a new prescale set.
    * Everybody accessing m_prescaleSetCollection needs to obtain this mutex
    * before accessing this list.
    */
   lock_guard<recursive_mutex> lock(m_prescaleSetCollection_mutex);
   clear();
   m_prescaleSets.insert(m_prescaleSets.begin(), cont(0, pss->id(), pss));
   return pss;
}

void
TrigConf::HLTPrescaleSetCollection::addPrescaleSet( unsigned int lumiblock, HLTPrescaleSet* pss ) {
   return addPrescaleSet(cont(lumiblock, pss?pss->id():0, pss));
}


void
TrigConf::HLTPrescaleSetCollection::addPrescaleSet( const cont& add_psinfo ) {

   std::list<cont>::iterator psinfo_it = m_prescaleSets.begin();
   for(; psinfo_it!=m_prescaleSets.end(); ++psinfo_it)
      if( psinfo_it->lb >= add_psinfo.lb ) break;
   {
      /*
       * Lock mutex while we are inserting a new prescale set.
       * Everybody accessing m_prescaleSetCollection needs to obtain this mutex
       * before accessing this list.
       */
      lock_guard<recursive_mutex> lock(m_prescaleSetCollection_mutex);
      if( (psinfo_it != m_prescaleSets.end()) && (psinfo_it->lb == add_psinfo.lb) ) {
         delete psinfo_it->pss;
         psinfo_it->pss = add_psinfo.pss;
         //if(psinfo_it->pss != 0)
         psinfo_it->psk = add_psinfo.psk;
      } else {
         m_prescaleSets.insert( psinfo_it, add_psinfo );
      }
   }
}


vector<pair<unsigned int, unsigned int> >
TrigConf::HLTPrescaleSetCollection::prescale_keys() const {
   vector<pair<unsigned int, unsigned int> > lbpsk;
   for(const cont& psinfo : m_prescaleSets)
      lbpsk.push_back(std::make_pair(psinfo.lb,psinfo.psk));
   return lbpsk;
}


void
TrigConf::HLTPrescaleSetCollection::set_prescale_key_to_load(unsigned int hltpsk) {
   vector<pair<unsigned int, unsigned int> > lbnpsk;
   lbnpsk.push_back(make_pair(0,hltpsk));
   set_prescale_keys_to_load(lbnpsk);
}

void
TrigConf::HLTPrescaleSetCollection::set_prescale_keys_to_load(const vector<pair<unsigned int, unsigned int> >& lbnpsk) {
   //    if(m_prescaleSets.size()>0) {
   //       throw std::runtime_error("ERROR: HLTPrescaleSetCollection: set_prescale_keys_to_load called on a non-empty collection. clear() needs to be called before");
   //    }
   for(auto & lbpsk : lbnpsk) {
      if( ! contains(lbpsk.first, lbpsk.second)) {
         addPrescaleSet( cont(lbpsk.first, lbpsk.second, 0) ); // register empty prescale sets with lb and psk, so they can later be loaded
      }
   }
}

bool
TrigConf::HLTPrescaleSetCollection::contains(unsigned int lumiblock, unsigned int psk) {
   lock_guard<recursive_mutex> lock(m_prescaleSetCollection_mutex);
   for(const cont& psinfo: m_prescaleSets)
      if( psinfo.lb==lumiblock && psinfo.psk==psk ) return true;
   return false;
}


void
TrigConf::HLTPrescaleSetCollection::print(const std::string& indent, unsigned int detail) const {
   lock_guard<recursive_mutex> lock(m_prescaleSetCollection_mutex);
   if(detail>=1) {
      unsigned int count_loaded(0);
      for(const cont& psinfo : m_prescaleSets)
         if(psinfo.pss!=0) count_loaded++;

      cout << indent << "HLTPrescaleSetCollection has " << size() << " prescale set(s) defined, " << count_loaded << " loaded." << endl;
      if(size()>0) {
         cout << indent << "       LB      PSK       Loaded     Name" << endl;
         for(const cont& psinfo : m_prescaleSets)
            cout << indent << setw(9) << right << psinfo.lb << setw(9) << right << psinfo.psk << "          " << (psinfo.pss!=0?"yes":" no")
		 << "     " << (psinfo.pss!=0?psinfo.pss->name():"") << endl;
      }
      if(detail>=2) {
         for(const cont& psinfo : m_prescaleSets)
            psinfo.pss->print(indent+"  ", detail);
      }
   }
}


std::ostream&
TrigConf::operator<<(std::ostream & o, const TrigConf::HLTPrescaleSetCollection & c) {
   lock_guard<recursive_mutex> lock(c.m_prescaleSetCollection_mutex);
   o << "HLTPrescaleSetCollection has " << c.size() << " prescale sets" << endl;
   if(c.size()>0) {
      o << "       LB   Prescale set key   Loaded   Prescale set" << endl;
      for(const TrigConf::HLTPrescaleSetCollection::cont& psinfo : c.m_prescaleSets)
         o << setw(9) << psinfo.lb << setw(19) << psinfo.psk << "   " << (psinfo.pss!=0?"yes":" no")
           << "   " << (psinfo.pss!=0?psinfo.pss->name():"") << endl;
   }
   return o;
}


string
HLTPrescaleSetCollection::__str__() const {
   stringstream s;
   s << *this;
   return s.str();
}
