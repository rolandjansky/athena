/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfIO/JsonFileLoader.h"
#include "PathResolver/PathResolver.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <sys/stat.h>
#include <filesystem>

using ptree = boost::property_tree::ptree;


TrigConf::JsonFileLoader::JsonFileLoader() : 
   TrigConfMessaging( "JsonFileLoader")
{}

TrigConf::JsonFileLoader::~JsonFileLoader()
{}

StatusCode
TrigConf::JsonFileLoader::loadFile( const std::string & filename,
                                    boost::property_tree::ptree & data,
                                    const std::string & pathToChild )
{

   /*
    * resolution of the file location happens in three steps
    1) ckeck if file exists under the absolute or relative path location
    2) if not and the path is not an absolute one, use the DATAPATH
   */

   std::string file=""; // resolved file name
   struct stat buffer;   
   bool existsAsSpecified = (stat (filename.c_str(), &buffer) == 0); 


   if( existsAsSpecified ) {
      file = filename;
   } else {
      if (filename.find("/")==0) { // absolute location
         TRG_MSG_WARNING("Can not find file with absolute location " << filename);
         return StatusCode::RECOVERABLE;
      } else {
         // check if environment DATAPATH exists
         PathResolver::SearchPathStatus pathStatus = PathResolver::check_search_path ("DATAPATH");
         if ( pathStatus == PathResolver::EnvironmentVariableUndefined ) {
            TRG_MSG_WARNING("Path environment $DATAPATH has not been defined");
            return StatusCode::RECOVERABLE;
         }
         
         // resolve absolute location
         file = PathResolver::find_file (filename, "DATAPATH");   
         if ( file == "" ) {
            TRG_MSG_WARNING("Did not find file " << filename << " in $DATAPATH");
            return StatusCode::RECOVERABLE;
         }
      }
   }

   TRG_MSG_INFO("Reading information from " << file);

   // Load the json file
   try {
      boost::property_tree::read_json(file, data);
   }
   catch (const boost::property_tree::json_parser_error& e) {
      TRG_MSG_WARNING("Could either not locate or parse the file " << file);
      return StatusCode::RECOVERABLE; 
   }

   if( ! pathToChild.empty() ) {
      // the user has specified that s/he wants to access a certain part of the file
      // the pathToChild is specified like this "a.b.c"
      boost::optional<ptree&> subtree = data.get_child_optional(pathToChild);
      if(subtree) {
         data = *subtree;
      } else {
         TRG_MSG_WARNING("Did not find element '" << pathToChild << "' in the file " << file);
         data.clear();
      }
   }

   return StatusCode::SUCCESS;
}


StatusCode
TrigConf::JsonFileLoader::loadFile( const std::string & filename,
                                    DataStructure & data,
                                    const std::string & pathToChild )
{
   boost::property_tree::ptree pt;

   StatusCode sc = this -> loadFile( filename, pt, pathToChild);

   if(!sc.isSuccess())
      return sc;

   data.setData(pt);

   return StatusCode::SUCCESS;
}
