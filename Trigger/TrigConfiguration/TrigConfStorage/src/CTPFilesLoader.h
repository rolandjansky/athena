/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     CTPFilesLoader.h
//PACKAGE:  TrigConfStorage      
//
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  16. Dec. 2005                                                     
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConf_CTPFilesLoader
#define TrigConf_CTPFilesLoader 

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <sys/types.h>

#include "TrigConfStorage/ICTPFilesLoader.h"
#include "TrigConfStorage/DBLoader.h"

namespace TrigConf {
  
   /**@brief TriggerDB loader of the LVL1 LUT and CAM tables*/
   class CTPFilesLoader : virtual public ICTPFilesLoader, public DBLoader {
   public:

      /**@brief constructor
       *
       * @param sm reference to storage manager
       *
       * @param session reference to the database session
       */
      CTPFilesLoader( StorageMgr& sm,  coral::ISessionProxy& session)
         : DBLoader("CTPFilesLoader", sm, session) {}

      /**@brief destructor*/       
      virtual ~CTPFilesLoader(){};

      virtual bool load( CTPFiles& data);

   private:
    
      bool loadCtpcoreData(CTPFiles&);
      bool loadCtpinData(CTPFiles&);
      bool loadCtpmonData(CTPFiles&);

      int getMenuIdFromMaster(int mid);
      long getFilesIdFromMenu(int mid);
      long getSmxIdFromMenu(int mid);
      void parseHexString(std::vector<u_int>& vec, const std::string& str, bool verbose=false);

      // template function to convert strings to integral data types
      template <class T>
      bool convert_hex_string(T& t, const std::string& s)
      {
         std::istringstream iss(s);
         return !(iss >> std::hex >> t).fail();
      }

      // template function to convert strings to integral data types
      template <class T>
      bool convert_string(T& t, const std::string& s)
      {
         std::istringstream iss(s);
         return !(iss >> t).fail();
      }

   };
}

#endif


