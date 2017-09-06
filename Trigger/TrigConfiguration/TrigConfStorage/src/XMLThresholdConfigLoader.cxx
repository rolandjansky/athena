/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./XMLThresholdConfigLoader.h"
#include "./XMLTriggerThresholdLoader.h"
#include "./XMLBoostHelper.h"

#include "TrigConfStorage/XMLStorageMgr.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/CaloInfo.h"

using namespace std;
using namespace TrigConf;


XMLThresholdConfigLoader::XMLThresholdConfigLoader( XMLStorageMgr& sm, const ptree::value_type* pt ) : 
   XMLLoader("XMLThresholdConfigLoader", sm, pt)
{}


bool
XMLThresholdConfigLoader::load( ThresholdConfig& thrcfg ) {

   if ( ! is_a("TriggerThresholdList") ) return false;

   TRG_MSG_DEBUG("Loading ThresholdConfig object")

   // Get the thresholds
   XMLTriggerThresholdLoader* thr_ldr = new XMLTriggerThresholdLoader(m_storageMgr);
   thr_ldr->setLevel(outputLevel());
   for(value_type v: pt() ) {
      thr_ldr->setPtree(&v);
      TriggerThreshold* thr = new TriggerThreshold();
      if (thr_ldr->load(*thr)) {
         TRG_MSG_DEBUG("Adding trigger threshold " << *thr << "(type " << thr->ttype() << ", mapping " << thr->mapping() << ")")
         thrcfg.addTriggerThreshold(thr);
      }
   }
   delete thr_ldr;

   // Read CaloInfo
   ICaloInfoLoader& ci_loader = m_storageMgr.caloInfoLoader();
   ci_loader.setLevel(outputLevel());
   CaloInfo ci;
   if ( ci_loader.load(ci) ) {
      thrcfg.setCaloInfo(ci);
      TriggerThresholdValue::setCaloInfo(ci);
   } else {
      TRG_MSG_ERROR("reading CaloInfo");
   }

   return true;
}

