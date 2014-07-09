/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TrigConf_XMLTriggerTypeLoader
#define TrigConf_XMLTriggerTypeLoader

#include "TrigConfStorage/ILoader.h"
#include "TrigConfStorage/XMLLoader.h"

namespace TrigConf {

   /**@brief TriggerDB loader of the HLT trigger type*/
   class XMLTriggerTypeLoader : virtual public ILoader, public XMLLoader {
   public:

      /**@brief constructor
       * @param sm reference to storage manager
       * @param node pointer to DOM node
       */
      XMLTriggerTypeLoader(XMLStorageMgr& sm, value_type* pt)
         : ILoader(), XMLLoader("XMLTriggerTypeLoader", sm, pt) {};

      /**@brief destructor*/
      virtual ~XMLTriggerTypeLoader(){};

      virtual bool load( TrigConfData& data);
   };
}

#endif

