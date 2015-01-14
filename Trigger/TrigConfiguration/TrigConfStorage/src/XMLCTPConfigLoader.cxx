/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "./XMLBoostHelper.h"
#include "./XMLCTPConfigLoader.h"
#include "./XMLMenuLoader.h"
#include "./XMLPrescaleSetLoader.h"
#include "./XMLBunchGroupSetLoader.h"
#include "./XMLDeadTimeLoader.h"
#include "./XMLRandomLoader.h"
#include "./XMLPrescaledClockLoader.h"
#include "./XMLPrioritySetLoader.h"

#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/PrescaleSet.h"
#include "TrigConfL1Data/BunchGroupSet.h"
#include "TrigConfL1Data/DeadTime.h"
#include "TrigConfL1Data/Random.h"
#include "TrigConfL1Data/PrescaledClock.h"
#include "TrigConfL1Data/Muctpi.h"

using namespace std;

bool
TrigConf::XMLCTPConfigLoader::load( CTPConfig& ctpc ) {

   if ( !is_a("LVL1Config") ) return false;

   int ival;
   string sval;
   if( readAttribute(pt(), "name", sval) ) ctpc.setName(sval);
   TRG_MSG_INFO("Loading CTP Configuration" << ctpc.name() );

   if( readAttribute(pt(), "id", ival) )          ctpc.setId(ival);
   if( readAttribute(pt(), "ctpVersion", ival) )  ctpc.setCTPVersion(ival);
   if( readAttribute(pt(), "l1Version", ival) )   ctpc.setL1Version(ival);
   TRG_MSG_INFO("CTP version " << ctpc.ctpVersion());
   TRG_MSG_INFO("L1  version " << ctpc.l1Version());

   TRG_MSG_INFO("Adapting the threshold limits to L1 version " << ctpc.l1Version());
   L1DataDef::setMaxThresholdsFromL1Version( ctpc.l1Version() );
   if(outputLevel()<=MSGTC::DEBUG)
      L1DataDef::printMaxThresholds();

   m_storageMgr.setCTPVersion(ctpc.ctpVersion());
   m_storageMgr.setL1Version(ctpc.l1Version());


   if (XMLLoader::getEnv() == XMLLoader::ALL    ||
       XMLLoader::getEnv() == XMLLoader::CTP) {
      IMenuLoader& menuldr = m_storageMgr.menuLoader();
      menuldr.setLevel(outputLevel());
      menuldr.load( ctpc.menu() );

      if(ctpc.l1Version()==0) {
         ctpc.menu().thresholdConfig().attributeThresholdNumbers();
      }


   } // ENV

   // prescales
   XMLPrescaleSetLoader& pss_ldr = dynamic_cast<XMLPrescaleSetLoader&>(m_storageMgr.prescaleSetLoader());
   std::pair < ptree::const_assoc_iterator, ptree::const_assoc_iterator> prescaleSets = pt().equal_range("PrescaleSet");
   ctpc.clearPrescaleSets();
   for (ptree::const_assoc_iterator it = prescaleSets.first; it != prescaleSets.second ; ++it) {
      PrescaleSet ps;
      pss_ldr.setPtree( &*it );
      pss_ldr.load(ps);
      ctpc.setPrescaleSet(ps);
   }

   BunchGroupSet bgs;
   IBunchGroupSetLoader& bgsldr = m_storageMgr.bunchGroupSetLoader();
   bgsldr.setLevel(outputLevel());
   bgsldr.load(bgs);
   ctpc.setBunchGroupSet(bgs);

   DeadTime dt;
   m_storageMgr.deadTimeLoader().load(dt);
   ctpc.setDeadTime(dt);

   Random r;
   m_storageMgr.randomLoader().load(r);
   ctpc.setRandom(r);

   PrescaledClock psc;
   m_storageMgr.prescaledClockLoader().load(psc);
   ctpc.setPrescaledClock(psc);

   if (XMLLoader::getEnv() == XMLLoader::ALL    ||
       XMLLoader::getEnv() == XMLLoader::CTPOnl) {
      PrioritySet priority_set;
      bool load_success = m_storageMgr.prioritySetLoader().load(priority_set);
      if(load_success) {
         ctpc.setPrioritySet(priority_set);
         for(unsigned int ctpid=0; ctpid<priority_set.priorities().size(); ctpid++) {
            TriggerItem *item = ctpc.menu().item(ctpid);
            if(item!=0) {
               string oldprio = priority_set.priorities()[ctpid];
               if(oldprio=="HIGH") {
                  item->setComplexDeadtime(0);
               } else {
                  item->setComplexDeadtime(1);
               }
            }
         }
      }

   } // ENV

   return true;

}

