/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_LutCamLoader
#define TrigConf_LutCamLoader 

#include <string>
#include <sstream>
#include <iostream>

#include "TrigConfStorage/ILutCamLoader.h"
#include "TrigConfStorage/DBLoader.h"

namespace TrigConf {

   class LutCamLoader : virtual public ILutCamLoader, public DBLoader {
   public:

      LutCamLoader( StorageMgr& sm,  coral::ISessionProxy& session) : DBLoader(sm, session) {}
      virtual ~LutCamLoader(){};

      virtual bool load( LutCam& data);

   private:
    
      // template function to convert strings to integral data types
      template <class T>
      bool convert_hex_string(T& t, const std::string& s)
      {
         std::istringstream iss(s);
         return !(iss >> std::hex >> t).fail();
      }

   };
}

#endif
