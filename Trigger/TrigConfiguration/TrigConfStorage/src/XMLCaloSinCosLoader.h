/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLCaloSinCosLoader
#define TrigConf_XMLCaloSinCosLoader 

#include "TrigConfStorage/ICaloSinCosLoader.h"
#include "TrigConfStorage/XMLLoader.h"

namespace TrigConf {

   /**@brief TriggerDB loader of the LVL1 calorimeter sin and cos
      configuration*/
   class XMLCaloSinCosLoader : virtual public ICaloSinCosLoader, public XMLLoader {
   public:

      /**@brief constructor
       * @param sm reference to storage manager
       * @param node pointer to DOM node
       */
      XMLCaloSinCosLoader(XMLStorageMgr& sm, value_type* pt = 0) :
         ICaloSinCosLoader(), XMLLoader("XMLCaloSinCosLoader", sm, pt) {};

      /**@brief destructor*/
      virtual ~XMLCaloSinCosLoader(){};

      virtual bool load( CaloSinCos& data);
   };
}

#endif
