/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#ifndef TrigConf_XMLBunchGroupLoader
#define TrigConf_XMLBunchGroupLoader

#include "TrigConfStorage/IBunchGroupLoader.h"
#include "TrigConfStorage/XMLLoader.h"

namespace TrigConf {

   /**@brief TriggerDB loader of the LVL1 bunch group configuration*/
   class XMLBunchGroupLoader : virtual public IBunchGroupLoader, public XMLLoader {
   public:

      /**@brief constructor
       * @param sm reference to storage manager
       * @param node pointer to the ptree
       */
      XMLBunchGroupLoader(XMLStorageMgr& sm, ptree::value_type* pt)
         : XMLLoader("XMLBunchGroupLoader", sm, pt) {};
    
      /**@brief destructor*/
      virtual ~XMLBunchGroupLoader(){};

      virtual bool load( BunchGroup& data);
   };

}

#endif

