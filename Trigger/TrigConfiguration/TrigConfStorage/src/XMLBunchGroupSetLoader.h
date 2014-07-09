/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#ifndef TrigConf_XMLBunchGroupSetLoader
#define TrigConf_XMLBunchGroupSetLoader
/*
  XMLBunchGroupSetLoader.h
*/
#include "TrigConfStorage/IBunchGroupSetLoader.h"
#include "TrigConfStorage/XMLLoader.h"

namespace TrigConf {

   /**@brief TriggerDB loader of the LVL1 bunch group set configuration*/
   class XMLBunchGroupSetLoader : public XMLLoader, virtual public IBunchGroupSetLoader {
   public:

      /**@brief constructor
       * @param sm reference to storage manager
       * @param node pointer to DOM node
       */
      XMLBunchGroupSetLoader(XMLStorageMgr& sm, const ptree::value_type* pt) : 
         XMLLoader("XMLBunchGroupSetLoader", sm, pt) {};

      /**@brief destructor*/
      virtual ~XMLBunchGroupSetLoader(){};

      virtual bool load( BunchGroupSet& data);
   };

}

#endif

