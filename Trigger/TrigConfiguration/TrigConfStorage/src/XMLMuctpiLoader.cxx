/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./XMLMuctpiLoader.h"
#include "./XMLBoostHelper.h"

bool
TrigConf::XMLMuctpiLoader::load( Muctpi& muctpi ) {

   if( ! is_a("MuctpiInfo") ) return false;

   msg() << "XMLMuctpiLoader started loading data." << std::endl;

   int ival(0);
   std::string strval("");

   if( readAttribute(pt(), "name", strval) )   muctpi.setName(strval);
   if( readAttribute(pt(), "id", ival) )       muctpi.setId(ival);
   if( readAttribute(pt(), "version", ival) )  muctpi.setVersion(ival);
   if( readAttribute(pt(), "low_pt", ival) )   muctpi.setLowptThreshold( ival );
   if( readAttribute(pt(), "high_pt", ival) )  muctpi.setHighptThreshold( ival );
   if( readAttribute(pt(), "max_cand", ival) ) muctpi.setMaxCand( ival );
   BOOST_FOREACH(value_type v, pt()) {
      if( v.first == "low_pt"   && getTextContent(v.second, ival) ) muctpi.setLowptThreshold( ival );
      if( v.first == "high_pt"  && getTextContent(v.second, ival) ) muctpi.setHighptThreshold( ival );
      if( v.first == "max_cand" && getTextContent(v.second, ival) ) muctpi.setMaxCand( ival );
   }


   return true;
}

