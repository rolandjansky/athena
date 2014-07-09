/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#ifndef TrigConf_XMLCaloInfoLoader
#define TrigConf_XMLCaloInfoLoader 

#include "TrigConfStorage/ICaloInfoLoader.h"
#include "TrigConfStorage/XMLLoader.h"

namespace TrigConf {

   /**@brief TriggerDB loader of the LVL1 calorimeter trigger configuration*/
   class XMLCaloInfoLoader : virtual public ICaloInfoLoader, public XMLLoader {
   public:

      /**@brief constructor
       * @param sm reference to storage manager
       * @param node pointer to DOM node
       */
      XMLCaloInfoLoader(XMLStorageMgr& sm, const ptree::value_type* pt)
         : ICaloInfoLoader(), XMLLoader("XMLCaloInfoLoader", sm, pt) {};

      /**@brief destructor*/
      virtual ~XMLCaloInfoLoader(){};

      virtual bool load( CaloInfo& data);
   };
}

#endif
