/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfStorage/XMLLoader.h"

using namespace std;
using namespace TrigConf;

XMLLoader::ENV
XMLLoader::m_env = XMLLoader::ALL;

XMLLoader::XMLLoader( const std::string& name, 
                      XMLStorageMgr& sm, 
                      const ptree::value_type* data ) : 
   TrigConfMessaging( name ),
   m_storageMgr( sm ),
   m_data( data )
{
   setLevel( sm.msg().level() );
}


XMLLoader::XMLLoader( XMLStorageMgr& sm,
                      const ptree::value_type* data ) : 
   XMLLoader("XMLLoader", sm, data)
{}


void
XMLLoader::setLevel(MSGTC::Level lvl) {
   msg().setLevel(lvl);

   switch(lvl) {
   case MSGTC::ALWAYS: m_verbose = 5; break;
   case MSGTC::VERBOSE: m_verbose = 4; break;
   case MSGTC::DEBUG: m_verbose = 3; break;
   case MSGTC::INFO: m_verbose = 2; break;
   case MSGTC::WARNING:
   case MSGTC::ERROR:
   case MSGTC::FATAL: m_verbose = 0; break;
   default: m_verbose = 0;
   }
}

bool
XMLLoader::is_a(const string& elementtag) const {
   if(m_data == 0) {
      TRG_MSG_ERROR("Can't load, ptree is NULL");
      return false;
   }
   if( m_data->first != elementtag ) {
      TRG_MSG_ERROR("Can't load, ptree is not a '" << elementtag <<"', but a '" << m_data->first << "'.");
      return false;
   }
   return true;
}


const boost::property_tree::ptree & 
XMLLoader::pt() const {
   if(m_data==0) {
      static ptree pt;
      return pt;
   }
   return m_data->second;
}

const string & 
XMLLoader::key() const {
   if(m_data==0) {
      static string k("");
      return k;
   }
   return m_data->first;
}
