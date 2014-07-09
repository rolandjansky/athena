/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./XMLBunchGroupSetLoader.h"
#include "./XMLBunchGroupLoader.h"
#include "./XMLBoostHelper.h"

#include "TrigConfL1Data/BunchGroupSet.h"

bool
TrigConf::XMLBunchGroupSetLoader::load( BunchGroupSet& bunchgroupset ) {

   if ( !is_a("BunchGroupSet") ) { return false; }

   int ival(0);
   std::string strval("");

   if( readAttribute(pt(), "id", ival) )              bunchgroupset.setId(ival);
   if( readAttribute(pt(), "name", strval) )          bunchgroupset.setName(strval);
   if( readAttribute(pt(), "menuPartition", ival) )   bunchgroupset.setMenuPartition(ival);
   if( readAttribute(pt(), "version", ival) )         bunchgroupset.setVersion(ival);

   XMLBunchGroupLoader* bgldr = new XMLBunchGroupLoader(m_storageMgr, 0);
   BunchGroup bg;
   BOOST_FOREACH(value_type v, pt()) {
      if(v.first=="<xmlattr>") continue;
      bgldr->setPtree(&v);
      bgldr->load(bg);
      bunchgroupset.addBunchGroup(bg); // adds a copy
   }
   delete bgldr;
   return true;
}

