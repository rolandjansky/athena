/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLRandomLoader
#define TrigConf_XMLRandomLoader

#include "TrigConfStorage/IRandomLoader.h"
#include "TrigConfStorage/XMLLoader.h"

namespace TrigConf {

   /**@brief TriggerDB loader of the LVL1 random trigger configuration*/
   class XMLRandomLoader : virtual public IRandomLoader, public XMLLoader {
   public:

      /**@brief constructor
       * @param sm reference to storage manager
       * @param node pointer to DOM node
       */
      XMLRandomLoader(XMLStorageMgr& sm, const ptree::value_type* pt) : 
         IRandomLoader(), XMLLoader("XMLRandomLoader", sm, pt) {}
    
      /**@brief destructor*/
      virtual ~XMLRandomLoader(){};    

      virtual bool load( Random& data);
   };

}

#endif

