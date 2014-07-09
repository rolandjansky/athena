/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     XMLThresholdConfigLoader.h 
//PACKAGE:  TrigConfStorage
//
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  31. Oct. 2005
//                                  
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////
#ifndef TrigConf_XMLThresholdConfigLoader
#define TrigConf_XMLThresholdConfigLoader

#include "TrigConfStorage/IThresholdConfigLoader.h"
#include "TrigConfStorage/XMLLoader.h"

namespace TrigConf {
  
   /**@brief TriggerDB loader of the LVL1 trigger threshold configuration*/
   class XMLThresholdConfigLoader : virtual public IThresholdConfigLoader, 
                                    public XMLLoader {
   public:

      /**@brief constructor
       *
       * @param sm reference to storage manager
       *
       * @param node pointer to DOM node
       */
      XMLThresholdConfigLoader( XMLStorageMgr& sm, const ptree::value_type* pt );

      /**@brief destructor*/
      virtual ~XMLThresholdConfigLoader(){};

      virtual bool load( ThresholdConfig& data);
   };
}

#endif

