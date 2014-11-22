/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/DiffStruct.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/HelperFunctions.h"
#include <iostream>

using namespace std;
using namespace TrigConf;

TrigConf::Menu::Menu()
   : L1DataBaseclass()
{}

TrigConf::Menu::~Menu() {
   clear();
}

std::vector<uint16_t>
TrigConf::Menu::bunchgroupMask() const {
   std::vector<uint16_t> bgmask(512);
   for(TriggerItem * item :  item_by_ctpid() )
      bgmask[item->ctpId()] = item->bunchgroupMask();
   return bgmask;
}

void TrigConf::Menu::addTriggerItem(TriggerItem* ti) {
   // they all work:
   // pair<item_by_ctpid_iterator, bool> ins = item_by_ctpid().insert(ti);
   // pair<item_by_name_iterator, bool> ins = item_by_name().insert(ti);
   pair<ItemContainer::iterator, bool> ins = m_TriggerItemVector.push_back(ti);
   if(! ins.second) {
      cerr << "FATAL Menu: insertion of TriggerItem (" << ti->name() << ", ctp ID=" << ti->ctpId() << ") failed, uniqueness constraint violated." << endl;
      throw runtime_error("Menu insertion of TriggerItem failed");
   }
}

void TrigConf::Menu::addThresholdMonitor(ThresholdMonitor* thrm) {
   m_ThresholdMonitorVector.push_back(thrm);
}

void TrigConf::Menu::addPit(PIT* pit) {
   m_PITs.push_back(pit);
}


void TrigConf::Menu::addTip(TIP* tip) {
	m_TIPs.push_back(tip);
}


TrigConf::TriggerItem*
TrigConf::Menu::item(int ctpid) const {
   const item_by_ctpid_t& byctpid = item_by_ctpid();
   item_by_ctpid_iterator item = byctpid.find(ctpid);
   if(item == byctpid.end()) return 0;
   return *item;
}

TrigConf::TriggerItem*
TrigConf::Menu::item(const string& name) const {
   const item_by_name_t& byname = items().get<tag_name_hash>();
   item_by_name_iterator item = byname.find(name);
   if(item == byname.end()) return 0;
   return *item;
}

TrigConf::ThresholdMonitor*
TrigConf::Menu::findThresholdMonitor(unsigned int id) {
   for(ThresholdMonitor* thrm : m_ThresholdMonitorVector)
      if( thrm->id()==id ) return thrm;
   return 0;
}

TrigConf::PIT*
TrigConf::Menu::findPIT(unsigned int id) {
   for(PIT* pit : m_PITs)
      if( pit->id()==id ) return pit;
   return 0;
}

TrigConf::TIP*
TrigConf::Menu::findTIP(unsigned int id) {
   for(TIP* tip : m_TIPs)
      if( tip->id()==id ) return tip;
   return 0;
}


// another interface to access the TriggerThreshold vector
const std::vector<TrigConf::TriggerThreshold*>& 
TrigConf::Menu::thresholdVector() const {
   return m_ThresholdConfig.getThresholdVector();
}

void
TrigConf::Menu::addTriggerThreshold(TrigConf::TriggerThreshold* tt) {
   m_ThresholdConfig.addTriggerThreshold(tt);   
}


TrigConf::DiffStruct*
TrigConf::Menu::compareTo(const Menu* o) const {
   DiffStruct * ds = new DiffStruct("TriggerMenu");
   ds->check("name", name(), o->name());
   for(TriggerItem* item : items() ) {
      TriggerItem* o_item(o->item(item->name()));
      if(o_item) {
         ds->addSub( item->compareTo(o_item) );
      } else {
         ds->addLeftOnlySub( "TriggerItem", item->name() );
      }
   }
   for(TriggerItem* o_item : o->items()) {
      const TriggerItem *item = this->item(o_item->name());
      if(item==0)
         ds->addRightOnlySub( "TriggerItem", o_item->name() );
   }
   if(ds->empty()) {
      delete ds; ds=0;
   }
   return ds;
}



void
TrigConf::Menu::clear() {
   m_ThresholdConfig.clear();

   for( TriggerItem* item : m_TriggerItemVector) delete item;
   m_TriggerItemVector.clear();

   for( ThresholdMonitor* thrMon : m_ThresholdMonitorVector) delete thrMon;
   m_ThresholdMonitorVector.clear();

   for( PIT* pit : m_PITs) delete pit;
   m_PITs.clear();
}

void
TrigConf::Menu::print(const std::string& indent, unsigned int detail) const {
   if(detail>=1) {
      cout << indent << "Trigger menu "; printNameIdV();
      cout << indent << "        number of items       : " << m_TriggerItemVector.size() << endl;
      cout << indent << "        number of thresholds  : " << m_ThresholdConfig.size() << endl;
      cout << indent << "        number of thr monitors: " << m_ThresholdMonitorVector.size() << endl;

      if(detail>=2) {

         cout << indent << "==================================" << endl;
         cout << indent << "Trigger Items:" << endl;
         cout << indent << "==================================" << endl;
         for(TriggerItem* item : m_TriggerItemVector)
            item->print(indent + "  ", detail);

         cout << indent << "==================================" << endl;
         cout << indent << "Trigger Thresholds:" << endl;
         cout << indent << "==================================" << endl;
         m_ThresholdConfig.print(indent + "  ", detail);
      }

      if(detail>=3) {
         cout << indent << "==================================" << endl;
         cout << indent << " The ThresholdMonitorVector:" << endl;
         cout << indent << "==================================" << endl;
         for(ThresholdMonitor* thrm : m_ThresholdMonitorVector)
            thrm->print(indent + "  ");
         if(detail>=4) {
            cout << indent << "==================================" << endl;
            cout << indent << " TriggerType summary:" << endl;
            cout << indent << "==================================" << endl;
            for (int i=0; i<8; ++i) {
               cout << indent << "TriggerType bit " << i << endl;
               for(TriggerItem* item : m_TriggerItemVector)
                  if (item->isTriggerTypeBitOn(i))
                     cout << indent << "  " << item->name() << endl;
            }
         }
         if(detail>=4) {
            cout << indent << "==================================" << endl;
            cout << indent << " BunchGroup mask summary:" << endl;
            cout << indent << "==================================" << endl;
            unsigned int i=0;
            for ( uint16_t m : bunchgroupMask() )
               cout << indent << "  ctpid=" << i++ << ": bgmask=" << uint2bin(m, 16) << endl;
         }
      }
   }
}


void
TrigConf::Menu::writeXMLItems(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   indent(xmlfile, indentLevel, indentWidth) 
      << "<TriggerMenu name=\"" << name()
      << "\" phase=\"lumi\">" << endl;
   for(TriggerItem* item: m_TriggerItemVector)
      item->writeXML(xmlfile, indentLevel+1, indentWidth);
   indent(xmlfile, indentLevel, indentWidth) << "</TriggerMenu>" << endl;
}

namespace {
   
   bool compThr(TriggerThreshold *x, TriggerThreshold *y) { // strict weak ordering: x and y are equivalent if compMon(x,y) and compMon(y,x) are false
      return x->id() < y->id();
   }

}

void
TrigConf::Menu::writeXMLThresholds(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   indent(xmlfile, indentLevel, indentWidth) << "<TriggerThresholdList>" << endl;

   auto sortedThresholds = m_ThresholdConfig.getThresholdVector();
   sort(sortedThresholds.begin(),sortedThresholds.end(),compThr);

   for(TriggerThreshold* thr: sortedThresholds) {
      if(!thr->isInternal())
         thr->writeXML(xmlfile, indentLevel+1, indentWidth);
   }
   indent(xmlfile, indentLevel, indentWidth) << "</TriggerThresholdList>" << endl;
}

namespace {
   
   bool compMon(ThresholdMonitor *x, ThresholdMonitor *y) { // strict weak ordering: x and y are equivalent if compMon(x,y) and compMon(y,x) are false
      if(x->thresholdName() != y->thresholdName() )
         return x->thresholdName() < y->thresholdName();
      return x->multiplicity() < y->multiplicity();
   }

}

void
TrigConf::Menu::writeXMLMonCounters(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   indent(xmlfile, indentLevel, indentWidth) << "<TriggerCounterList>" << endl;

   auto sortedMonitors = m_ThresholdMonitorVector;
   sort(sortedMonitors.begin(),sortedMonitors.end(),compMon);

   for(const ThresholdMonitor * mon: sortedMonitors)
      mon->writeXML(xmlfile, indentLevel+1, indentWidth);
   indent(xmlfile, indentLevel, indentWidth) << "</TriggerCounterList>" << endl;
}



// for python
vector<TriggerItem*>
Menu::itemsV() const {
   vector<TriggerItem*> it(items().size());
   copy(items().begin(), items().end(), it.begin());
   return it;
}
