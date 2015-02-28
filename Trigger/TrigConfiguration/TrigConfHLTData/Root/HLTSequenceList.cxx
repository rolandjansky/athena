/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfHLTData/HLTSequenceList.h"
#include "TrigConfHLTData/HLTSequence.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfL1Data/DiffStruct.h"

#include <iostream>
#include <stdexcept>

using namespace std;

TrigConf::HLTSequenceList::HLTSequenceList(){}

TrigConf::HLTSequenceList::HLTSequenceList( vector<HLTSequence*>& sequenceList ) {
   copy( sequenceList.begin(), sequenceList.end(), std::inserter(*this, begin()));
}

TrigConf::HLTSequenceList::~HLTSequenceList( void ) {
   clear();
}

void TrigConf::HLTSequenceList::clear() {
   for(HLTSequence* seq : *this) delete seq;
   HLTSequenceContainer::clear();
}

void TrigConf::HLTSequenceList::addHLTSequence( HLTSequence * sequence) {
   if(sequence==0) return;
   pair<iterator, bool> ins = insert(sequence);
   if(!ins.second) {
      throw runtime_error("Can't insert sequence '" + sequence->name() + "', because a uniqueness constraint is violated");
   }
}


bool TrigConf::HLTSequenceList::hasTE(const std::string& name) {
   return getSequence(name)!=0;
}


TrigConf::HLTSequence*
TrigConf::HLTSequenceList::getSequence(unsigned int id) const {
   const index<seq_hash_id>::type& byid = get<seq_hash_id>();
   index<seq_hash_id>::type::const_iterator seq = byid.find(id);
   if(seq == byid.end()) return 0;
   return *seq;
}

TrigConf::HLTSequence*
TrigConf::HLTSequenceList::getSequence(const std::string& name) const {
   const index<seq_name_hash>::type& byname = get<seq_name_hash>();
   index<seq_name_hash>::type::const_iterator seq = byname.find(name);
   if(seq == byname.end()) return 0;
   return *seq;
}


bool
TrigConf::HLTSequenceList::getLabel(unsigned int id, std::string & label) {
   const index<seq_hash_id>::type& byid = get<seq_hash_id>();
   index<seq_hash_id>::type::const_iterator seq = byid.find(id);
   if(seq != byid.end()) {
      label = (*seq)->name();
      return true; 
   } else {
      label = "";
      return false;
   }
}


bool
TrigConf::HLTSequenceList::getHashIdFromLabel(const char * label, unsigned int & id) {
   const index<seq_name_hash>::type& byname = get<seq_name_hash>();
   index<seq_name_hash>::type::const_iterator seq = byname.find(label);
   if(seq != byname.end()) {
      id = (*seq)->hashId();
      return true;
   } else {
      id = 0;
      return false;
   }
}

TrigConf::DiffStruct*
TrigConf::HLTSequenceList::compareTo(const HLTSequenceList* o) const {
   DiffStruct * ds = new DiffStruct("SEQUENCE_LIST");
   for(HLTSequence* seq : *this) {
      const HLTSequence *o_seq = o->getSequence(seq->name());
      if(o_seq) {
         ds->addSub( seq->compareTo(o_seq) );
      } else {
         ds->addLeftOnlySub( "SEQUENCE", seq->name() );
      }
   }
   for(HLTSequence* o_seq : *o) {
      const HLTSequence *seq = getSequence(o_seq->name());
      if(!seq)
         ds->addRightOnlySub( "SEQUENCE", o_seq->name() );
   }
   if(ds->empty()) {
      delete ds; ds=0;
   }
   return ds;
}

   
void
TrigConf::HLTSequenceList::print(const std::string& indent, unsigned int detail) const {
   if(detail>=1) {
      cout << indent << "HLTSequenceList has " << size() << " sequences" << endl;
      if(detail>=4) {
         cout << indent << "--------------------------------------------------------------------------------" << endl;
         for(HLTSequence* seq : *this)
            seq->print(indent + "  ", detail);
         cout << indent << "================================================================================" << endl;
      }
   }
}
   
std::ostream &
TrigConf::operator<<(std::ostream & o, const TrigConf::HLTSequenceList & sl) {
   o << "- -- HLTSequenceList printout ------------------------------ " << std::endl;
   for(TrigConf::HLTSequence* seq : sl) o << *seq;
   o << "- ---------------------------------------------------------- " << std::endl;
   return o;
}

