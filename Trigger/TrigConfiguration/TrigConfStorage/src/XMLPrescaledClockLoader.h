/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLPrescaledClockLoader
#define TrigConf_XMLPrescaledClockLoader

#include "TrigConfStorage/IPrescaledClockLoader.h"
#include "TrigConfStorage/XMLLoader.h"

namespace TrigConf {

   /**@brief TriggerDB loader of the LVL1 prescaled clock trigger configuration*/
   class XMLPrescaledClockLoader : virtual public IPrescaledClockLoader, public XMLLoader {
   public:

      /**@brief constructor
       * @param sm reference to storage manager
       * @param node pointer to DOM node
       */
      XMLPrescaledClockLoader(XMLStorageMgr& sm, const ptree::value_type* pt) : 
         XMLLoader("XMLPrescaledClockLoader", sm, pt) {}

      /**@brief destructor*/
      virtual ~XMLPrescaledClockLoader(){};

      virtual bool load( PrescaledClock& data);
   };

}

#endif

