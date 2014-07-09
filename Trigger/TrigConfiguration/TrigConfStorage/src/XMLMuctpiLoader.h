/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLMuctpiLoader
#define TrigConf_XMLMuctpiLoader 

#include "TrigConfStorage/IMuctpiLoader.h"
#include "TrigConfStorage/XMLLoader.h"

namespace TrigConf {

   /**@brief TriggerDB loader of the LVL1 muon hardware configuration*/
   class XMLMuctpiLoader : virtual public IMuctpiLoader, public XMLLoader {
   public:

      /**@brief constructor
       * @param sm reference to storage manager
       * @param pt = pair<key,ptree> 
       */
      XMLMuctpiLoader( XMLStorageMgr& sm, const ptree::value_type* pt) :
         IMuctpiLoader(), XMLLoader("XMLMuctpiLoader", sm, pt) {};

      /**@brief destructor*/
      virtual ~XMLMuctpiLoader(){};

      virtual bool load( Muctpi& data);
   };
}

#endif

