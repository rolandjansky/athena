/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_ILoader
#define TrigConf_ILoader

#include "TrigConfBase/MsgStream.h"

namespace TrigConf {

   class TrigConfData;

   /**
    * @brief Interface class for loaders 
    */
   class ILoader {
   public:
      virtual ~ILoader() {}

      /**
       * @brief Load the configuration data from the configuration
       * source 
       *
       * @param ctpc reference to the data object to be filled
       * 
       * @returns @c true if the loading was successful, @c false
       * otherwise 
       */
      //virtual bool load( TrigConfData& data ) = 0;
      
      // setting log level
      virtual void setLevel(MSGTC::Level lvl) = 0;
      virtual MSGTC::Level outputLevel() const = 0;

      virtual int verbose() const = 0;

      virtual void setVerbose(int v) = 0;
   };
}

#endif
