/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLLutCamLoader
#define TrigConf_XMLLutCamLoader

#include "TrigConfStorage/ILutCamLoader.h"
#include "TrigConfStorage/XMLLoader.h"
#include <string>

namespace TrigConf {

   /**@brief TriggerDB loader of the LVL1 LUT and CAM tables*/
   class XMLLutCamLoader : virtual public ILutCamLoader, public XMLLoader {
   public:

      /**@brief constructor
       * @param sm reference to storage manager
       * @param lut reference to string representation of the LUT table
       * @param cam reference to string representation of the CAM table
       */
      XMLLutCamLoader(XMLStorageMgr& sm, const std::string& lut,
                      const std::string& cam) : 
         ILutCamLoader(),
         XMLLoader("XMLLutCamLoader", sm, 0),
         m_lutFile(lut),
         m_camFile(cam)
      {};
     
      /**@brief destructor*/
      virtual ~XMLLutCamLoader() {}

      virtual bool load( LutCam& data);
    
      std::string m_lutFile;
      std::string m_camFile;

   };

}


#endif
