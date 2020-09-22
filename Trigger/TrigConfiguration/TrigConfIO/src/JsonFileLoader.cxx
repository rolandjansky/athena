/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfIO/JsonFileLoader.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <sys/stat.h>
#include <filesystem>
#include <cstdlib>
#include <cstring>

using ptree = boost::property_tree::ptree;


TrigConf::JsonFileLoader::JsonFileLoader() : 
   TrigConfMessaging( "JsonFileLoader")
{}

TrigConf::JsonFileLoader::~JsonFileLoader()
{}

/*
  File will be search first absolute (when starting with "/" or
  relative to the current path

  If not found it will be searched in XMLPATH
*/
std::string
TrigConf::JsonFileLoader::findFile(const std::string & filename) const {

   // check for the file as specified
   if( std::filesystem::exists( filename ) ) {
      return filename;
   }

   // check if absolute location has been specified
   if (filename.find("/")==0) {
      TRG_MSG_WARNING("Can not find file with absolute location " << filename);
      return "";
   } 

   // check if environment DATAPATH exists
   const char * dp = std::getenv("XMLPATH");
   if( dp == nullptr ) {
      TRG_MSG_WARNING("Path environment $DATAPATH has not been defined");
      return "";
   }
         
   // resolve location using XMLPATH
   std::string fnCopy(filename);
   char *token = std::strtok( &*fnCopy.begin(), ":");
   while ( token != nullptr ) {
      std::filesystem::path fullname(token); 
      fullname /= filename;
      if( std::filesystem::exists( fullname ) ) {
         return filename;
      }
      // go to the next 
      token = std::strtok( nullptr, " ");
   }

   return "";
}

bool
TrigConf::JsonFileLoader::loadFile( const std::string & filename,
                                    boost::property_tree::ptree & data,
                                    const std::string & pathToChild ) const 
{

   /*
    * resolution of the file location happens in three steps
    1) ckeck if file exists under the absolute or relative path location
    2) if not and the path is not an absolute one, use the DATAPATH
   */

   std::string file = findFile(filename); // resolved file name
   if ( file == "" ) {
      return false;
   }
   TRG_MSG_INFO("Reading information from " << file);

   // Load the json file
   try {
      boost::property_tree::read_json(file, data);
   }
   catch (const boost::property_tree::json_parser_error& e) {
      TRG_MSG_WARNING("Could either not locate or parse the file " << file);
      return false; 
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

   return true;
}


bool
TrigConf::JsonFileLoader::loadFile( const std::string & filename,
                                    DataStructure & data,
                                    const std::string & pathToChild ) const
{
   boost::property_tree::ptree pt;

   if( ! loadFile( filename, pt, pathToChild) )
      return false;

   data.setData(std::move(pt));

   return true;
}



bool
TrigConf::JsonFileLoader::saveFile( const std::string & filename,
                                    const DataStructure & data ) const
{
   if ( filename == "" ) {
      TRG_MSG_ERROR("Could not save to file, as specified filename is empty");
      return false;
   }
   boost::property_tree::write_json(filename, data.data());
   TRG_MSG_INFO("Saved file " << filename);

   return true;
}


std::string
TrigConf::JsonFileLoader::getFileType( const std::string & filename ) const
{
   std::string ft = "UNKNOWN";

   DataStructure data;
   if ( loadFile ( filename, data ) ) {
      ft = data.getAttribute("filetype", /*ignoreIfMissing*/ true, ft);
   }

   return ft;
}


bool
TrigConf::JsonFileLoader::checkTriggerLevel( const std::string & filename,
                                             std::string & level ) const
{
   level = "UNKNOWN";

   DataStructure data;

   bool succeeded = this -> loadFile ( filename, data );

   if ( succeeded ) {
      if (data.hasChild("chains")) {
         level = "HLT";
      } else if (data.hasChild("items")) {
         level = "L1";
      }
   }

   return succeeded;
}
