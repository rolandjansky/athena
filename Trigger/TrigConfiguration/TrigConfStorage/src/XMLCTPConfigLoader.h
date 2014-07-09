/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLCTPConfigLoader
#define TrigConf_XMLCTPConfigLoader

#include "TrigConfStorage/ICTPConfigLoader.h"
#include "TrigConfStorage/XMLLoader.h"

namespace TrigConf {

   class XMLCTPConfigLoader : virtual public ICTPConfigLoader, public XMLLoader {
   public:

      XMLCTPConfigLoader(XMLStorageMgr& sm, const ptree::value_type* pt) :
         XMLLoader("XMLCTPConfigLoader", sm, pt) {};

      /**@brief destructor*/
      virtual ~XMLCTPConfigLoader(){};

      virtual bool load( CTPConfig& data);
   };
}

#endif

