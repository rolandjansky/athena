/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./XMLBunchGroupLoader.h"
#include "./XMLBoostHelper.h"

#include "TrigConfL1Data/BunchGroupSet.h"

bool
TrigConf::XMLBunchGroupLoader::load( BunchGroup& bunchgroup ) {

   if ( !is_a("BunchGroup") ) return false;

   bunchgroup.clear();

   int ival(0);
   std::string strval("");

   if( readAttribute(pt(), "id", ival) )              bunchgroup.setId(ival);
   if( readAttribute(pt(), "name", strval) )          bunchgroup.setName(strval);
   if( readAttribute(pt(), "version", ival) )         bunchgroup.setVersion(ival);
   if( readAttribute(pt(), "internalNumber", ival) )  bunchgroup.setInternalNumber(ival);
   if( readAttribute(pt(), "menuPartition", ival) )   bunchgroup.setPartition(ival);

   // get bunches
   BOOST_FOREACH(const ptree::value_type& v, pt())
      if( readAttribute(v.second, "bunchNumber", ival) ) bunchgroup.addBunch(ival);

   return true;
}
