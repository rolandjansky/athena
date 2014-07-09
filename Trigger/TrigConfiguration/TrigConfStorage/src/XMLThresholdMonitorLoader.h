/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLThresholdMonitorLoader
#define TrigConf_XMLThresholdMonitorLoader
/*
  XMLCTPFilesLoader.h
*/
#include "TrigConfStorage/IThresholdMonitorLoader.h"
#include "TrigConfStorage/XMLLoader.h"
#include <string>

namespace TrigConf {

   /**@brief Dummy XML loader */
   class XMLThresholdMonitorLoader : virtual public IThresholdMonitorLoader, public XMLLoader {
   public:

      /**@brief constructor
       * @param sm reference to storage manager
       */
      XMLThresholdMonitorLoader(XMLStorageMgr& sm) :
         XMLLoader("XMLThresholdMonitorLoader", sm, 0) {};
      virtual ~XMLThresholdMonitorLoader() {}

      virtual bool load( ThresholdMonitor& data);
   };

}


#endif
