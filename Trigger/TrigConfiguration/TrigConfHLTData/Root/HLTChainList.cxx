/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfHLTData/HLTUtils.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"
#include "TrigConfL1Data/DiffStruct.h"

#include "TrigConfL1Data/HelperFunctions.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"

#include <iostream>
#include <algorithm>
#include <iterator>

using namespace std;

TrigConf::HLTChainList::HLTChainList()
{}


TrigConf::HLTChainList::HLTChainList( const vector<HLTChain*>& chainList ) {
   copy(chainList.begin(), chainList.end(), std::inserter(*this, begin()));
}


TrigConf::HLTChainList::~HLTChainList() {
   clear();
}

void TrigConf::HLTChainList::clear() {
   for(HLTChain* ch : *this) {
      delete ch;
   }
   HLTChainContainer::clear();
}

bool
TrigConf::HLTChainList::addHLTChain(TrigConf::HLTChain* chain ) {
   pair<iterator, bool> ins = insert(chain);
   if(!ins.second)
      throw runtime_error("Can't insert chain '" + chain->name() + "', because a uniqueness constraint is violated");
   return ins.second;
}



TrigConf::HLTChain*
TrigConf::HLTChainList::chain( const std::string& name ) const {
   const index<name_hash>::type& byname = get<name_hash>();
   index<name_hash>::type::const_iterator ch = byname.find(name);
   if(ch == byname.end()) return 0;
   return *ch;
}


TrigConf::HLTChain*
TrigConf::HLTChainList::chain( unsigned int chaincounter, HLTLevel level ) const {
   const index<counter>::type& bycounter = get<counter>();
   index<counter>::type::const_iterator ch = bycounter.find(boost::make_tuple(chaincounter,level));
   if(ch == bycounter.end()) return 0;
   return *ch;
}


void
TrigConf::HLTChainList::applyPrescaleSet(const HLTPrescaleSet* pss) {
   for(HLTChain *ch: *this) {
      if(ch->level_enum() == HLTLevel::HLT ) {
         // merged menu
         if (pss->hasPrescale( ch->chain_counter(), HLTLevel::HLT ) ) {
            // merged prescales (Run 2)
            const HLTPrescale& sc = pss->getPrescale( ch->chain_counter(), HLTLevel::HLT );
            ch->set_prescales( sc );
         } else {
            // prescale set has not been merged yet (LS 1)
            if(ch->mergeCounter.l2==0 && ch->mergeCounter.ef==0) {
               throw runtime_error("Merged HLT chain has no info about L2 or EF counter");
            } else if(ch->mergeCounter.l2==0) {
               const HLTPrescale& efps = pss->getPrescale( ch->mergeCounter.ef, HLTLevel::EF );
               ch->set_prescales( efps );
            } else if(ch->mergeCounter.ef==0) {
               const HLTPrescale& l2ps = pss->getPrescale( ch->mergeCounter.ef, HLTLevel::L2 );
               ch->set_prescales( l2ps );
            } else {
               const HLTPrescale& l2ps = pss->getPrescale( ch->mergeCounter.l2, HLTLevel::L2 );
               const HLTPrescale& efps = pss->getPrescale( ch->mergeCounter.ef, HLTLevel::EF );
               HLTTEUtils::mergeL2EFPrescales( ch, l2ps, efps);
            }
         }
      } else {
         // menu not merged
         const HLTPrescale& sc = pss->getPrescale( ch->chain_counter(), str2lvl(ch->level()) );
         ch->set_prescales( sc );
      }
   }
}


TrigConf::HLTPrescaleSet*
TrigConf::HLTChainList::extractPrescaleSet() const {
   HLTPrescaleSet* pss = new HLTPrescaleSet();
   for(HLTChain *ch : *this)
      pss->setPrescale(ch->prescales(), ch->chain_counter(), ch->level_enum());
   return pss;
}



//________________________________________________________________________________
void
TrigConf::HLTChainList::setL2LowerChainCounter(const CTPConfig* ctpcfg) {
   if(ctpcfg==0)
      throw runtime_error("setL2LowerChainCounter: no CTPConfig object available");

   const TrigConf::Menu& menu = ctpcfg->menu();

   std::map<std::string,int> ctpFromName;
   for(const TriggerItem* item : menu.itemVector())
      ctpFromName[item->name()] = item->ctpId();
     
   for(HLTChain* ch : *this) {
      if(ch->level() == "EF") continue;
      std::string low_chain_names = ch->lower_chain_name();
      TrigConf::removeAllSpaces(low_chain_names);
      vector<string> low_chain_names_V = split(low_chain_names,",");
      std::vector<int> lccs;
      for(string lowerChainName : low_chain_names_V)
         lccs.push_back(ctpFromName[lowerChainName]);
      ch->set_lower_chain_counter(low_chain_names_V.size()==1 ? lccs[0] : -1);
      ch->set_lower_chain_counters(lccs);
   }
}


// fill the lower chain counter for all EF chains
void
TrigConf::HLTChainList::setEFLowerChainCounter() {
   for(HLTChain *ch : *this) {
      if(ch->level()!="EF" || ch->lower_chain_name()=="") continue;
      HLTChain * l2ch = chain(ch->lower_chain_name());
      if(l2ch==0) {
         cerr << "ERROR: Did not find lower chain name '" << ch->lower_chain_name() << "' for chain '" << ch->chain_name() << "'" << endl;
         continue;
      }
      ch->set_lower_chain_counter( l2ch->chain_counter() );
   }      
}

TrigConf::DiffStruct*
TrigConf::HLTChainList::compareTo(const HLTChainList* o) const {
   DiffStruct * ds = new DiffStruct("CHAIN_LIST");
   for(HLTChain* ch : *this) {
      HLTChain *o_ch = o->chain(ch->name());
      if(o_ch) {
         ds->addSub( ch->compareTo(o_ch) );
      } else {
         ds->addLeftOnlySub( "CHAIN", ch->name() );
      }
   }
   for(HLTChain* o_ch : *o) {
      const HLTChain *ch = chain(o_ch->name());
      if(!ch)
         ds->addRightOnlySub( "CHAIN", o_ch->name() );
   }
   if(ds->empty()) {
      delete ds; ds=0;
   }
   return ds;
}


void
TrigConf::HLTChainList::print(const std::string& indent, unsigned int detail) const {
   if(detail>=1) {
      cout << indent << "HLTChainList has " << size() << " chains" << endl;
      if(detail>=2) {
         for(HLTChain* ch : *this)
            ch->print(indent + "  ", detail);
         cout << indent << "- ---------------------------------------------------------- " << endl;
      }
   }
}


std::ostream &
TrigConf::operator<<(std::ostream & o, const TrigConf::HLTChainList & cl) {
  o << "- -- HLTChainList [" << cl.size() << " chains] -------------------------------- " << std::endl;
  for(TrigConf::HLTChain* ch : cl)
     o << *ch;
  o << "- ---------------------------------------------------------- " << std::endl;
  return o;
}

