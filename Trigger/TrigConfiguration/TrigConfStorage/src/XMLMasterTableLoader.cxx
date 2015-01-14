/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <typeinfo>

#include "./XMLMasterTableLoader.h"
#include "./XMLBoostHelper.h"

#include "TrigConfStorage/IThresholdConfigLoader.h"
#include "TrigConfStorage/ICTPConfigLoader.h"
#include "TrigConfStorage/IMuctpiLoader.h"

#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Muctpi.h"
#include "L1TopoConfig/L1TopoMenu.h"

using namespace std;

bool TrigConf::XMLMasterTableLoader::load(ThresholdConfig& thrcfg) {

   IThresholdConfigLoader& thrLoader = m_storageMgr.thresholdConfigLoader();
   thrLoader.setLevel(outputLevel());
   return thrLoader.load(thrcfg);
}

bool TrigConf::XMLMasterTableLoader::load(CTPConfig& ctpc) {
   std::string menuname("L1Name");
   readAttribute(pt(), "name", menuname);
   ctpc.setName(menuname);

   ICTPConfigLoader& ctpLoader = m_storageMgr.ctpConfigLoader();
   ctpLoader.setLevel(outputLevel());
   return ctpLoader.load(ctpc);
}

bool TrigConf::XMLMasterTableLoader::load(Muctpi& m) {
   IMuctpiLoader& muctpiLoader = m_storageMgr.muctpiLoader();
   muctpiLoader.setLevel(outputLevel());
   return muctpiLoader.load(m);
}

bool TrigConf::XMLMasterTableLoader::load(TXC::L1TopoMenu& l1topo) {
   IL1TopoMenuLoader& l1topoLoader = m_storageMgr.l1topoMenuLoader();
   l1topoLoader.setLevel(outputLevel());
   return l1topoLoader.load(l1topo);
}
