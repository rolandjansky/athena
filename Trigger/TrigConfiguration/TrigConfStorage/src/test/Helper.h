/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfStorage_Helper
#define TrigConfStorage_Helper

#include <string>
#include <vector>
#include <sys/stat.h> 

#include "CoolKernel/IDatabaseSvc.h"


std::string findInPath(const std::string& filename, const std::string & path) {
   struct stat file_stat;
   std::string fullname = path + "/" + filename;
   if (stat (fullname.c_str(), &file_stat) == 0) return fullname;
   return "";
}

std::vector<std::string> splitpath(std::string line, std::string del=":")
{
  std::vector<std::string> res;
  std::string::size_type old_pos = 0, pos = line.find(del,0);
  while( pos != std::string::npos ) {
    res.push_back(line.substr(old_pos,pos-old_pos));
    old_pos = pos + del.size();
    pos = line.find(del, old_pos);
  }
  // last entry
  if (old_pos < line.size())
    res.push_back(line.substr(old_pos,line.size()-old_pos));
  return res;
}

std::string pathresolve(const std::string& filename, const std::string & searchpath) {
   std::string fullname = findInPath(filename,".");
   if( fullname != "" ) { return fullname; }
   std::vector<std::string> listofpaths = splitpath(searchpath);
   for (const std::string& path : listofpaths) {
      fullname = findInPath(filename,path);
      if( fullname != "" ) { return fullname; }      
   }
   return "";
}

std::string xmlpathresolve(const std::string& filename) {
   // if path starts with '/' then it is absolute
   if( !filename.empty() && filename[0] == '/' ) return filename;

   std::string xmlpath = ::getenv("XMLPATH");
   if(filename.find('/')==std::string::npos) {
      // if there is no "/" in the filename we try to things
      std::string fullname = pathresolve(filename, xmlpath);
      if(fullname == "")
         fullname = pathresolve("TriggerMenuXML/"+filename, xmlpath);
      return fullname;
   } else {
      return pathresolve(filename, xmlpath);
   }
}

#endif

