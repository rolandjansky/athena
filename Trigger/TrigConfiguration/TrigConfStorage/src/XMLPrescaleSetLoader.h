/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLPrescaleSetLoader
#define TrigConf_XMLPrescaleSetLoader 

#include "TrigConfStorage/IPrescaleSetLoader.h"
#include "TrigConfStorage/XMLLoader.h"

namespace TrigConf {

   /**@brief TriggerDB loader of the LVL1 prescale set*/
   class XMLPrescaleSetLoader : virtual public IPrescaleSetLoader, public XMLLoader {
   public:

      XMLPrescaleSetLoader( XMLStorageMgr& sm, const ptree::value_type* pt) : 
         IPrescaleSetLoader(), XMLLoader( "XMLPrescaleSetLoader", sm, pt)
      {}

      /**@brief destructor*/
      virtual ~XMLPrescaleSetLoader(){};

      virtual bool load( PrescaleSet& data);
   };
}

#endif

