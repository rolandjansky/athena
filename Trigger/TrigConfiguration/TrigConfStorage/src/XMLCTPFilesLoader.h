/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLCTPFilesLoader
#define TrigConf_XMLCTPFilesLoader
/*
  XMLCTPFilesLoader.h
*/
#include "TrigConfStorage/ICTPFilesLoader.h"
#include "TrigConfStorage/XMLLoader.h"
#include <string>

namespace TrigConf {

   /**@brief TriggerDB loader of the LVL1 LUT and CAM tables*/
   class XMLCTPFilesLoader : virtual public ICTPFilesLoader, public XMLLoader {
   public:

      /**@brief constructor
       * @param sm reference to storage manager
       * @param lut reference to string representation of the LUT table
       * @param cam reference to string representation of the CAM table
       */
      XMLCTPFilesLoader(XMLStorageMgr& sm, const std::string& ctpcoreLUT,
                        const std::string& ctpcoreCAM)
         : ILoader(),
           XMLLoader("XMLCTPFilesLoader", sm,0), 
           m_ctpcoreLUTFile(ctpcoreLUT),
           m_ctpcoreCAMFile(ctpcoreCAM)
      {};

      /**@brief destructor*/
      virtual ~XMLCTPFilesLoader() {}

      virtual bool load( CTPFiles& data);
    
      std::string m_ctpcoreLUTFile;
      std::string m_ctpcoreCAMFile;

   };

}


#endif
