/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLJobOptionTableLoader
#define TrigConf_XMLJobOptionTableLoader

#include "TrigConfStorage/IJobOptionTableLoader.h"
#include "TrigConfStorage/XMLLoader.h"

namespace TrigConf {

   /**@brief TriggerDB loader of the HLT job options*/
   class XMLJobOptionTableLoader : virtual public IJobOptionTableLoader, public XMLLoader {
   public:

      /**@brief constructor
       * @param sm reference to storage manager
       * @param node pointer to DOM node
       */
      XMLJobOptionTableLoader(XMLStorageMgr& sm, const ptree::value_type* pt) :
         IJobOptionTableLoader(), XMLLoader("XMLJobOptionTableLoader", sm, pt) {};

      /**@brief destructor*/
      virtual ~XMLJobOptionTableLoader(){};

      bool load( JobOptionTable& jot );
   };

}

#endif

