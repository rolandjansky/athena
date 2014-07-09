/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLPrioritySetLoader
#define TrigConf_XMLPrioritySetLoader 

#include "TrigConfStorage/IPrioritySetLoader.h"
#include "TrigConfStorage/XMLLoader.h"

namespace TrigConf {

   /**@brief TriggerDB loader of the LVL1 item priority configuration*/
   class XMLPrioritySetLoader : virtual public IPrioritySetLoader, public XMLLoader {
   public:

      /**@brief constructor
       * @param sm reference to storage manager
       * @param node pointer to DOM node
       */
      XMLPrioritySetLoader( XMLStorageMgr& sm, const ptree::value_type* pt)
         : IPrioritySetLoader(), XMLLoader("XMLPrioritySetLoader", sm, pt) {}
    
      /**@brief destructor*/
      virtual ~XMLPrioritySetLoader(){};

      virtual bool load( PrioritySet& data);
   };
}

#endif

