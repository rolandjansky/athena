/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVC_BYTESTREAMUSERMETADATA_H
#define BYTESTREAMCNVSVC_BYTESTREAMUSERMETADATA_H
/**
 * @file ByteStreamUserMetadata.h
 * @brief class definition for ByteStreamUserMetadata
 */

/**
 * @class ByteStreamUserMetadata
 * @brief Simple Filter algorithm which simply sets pass=true for all events.
 * <br>
 * @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
 * Created May 2004 for DC2 Validation tests
 * <i>No settable properties</i>
 * $Id: ByteStreamUserMetadata.h,v 1.1 2009-02-19 22:04:49 cranshaw Exp $
 */
            
#include <sstream>
#include <map>
#include <string>
#include "CLIDSvc/CLASS_DEF.h"
 
class ByteStreamFreeMetadataString : public std::pair<std::string,std::string>
{
public:
   ByteStreamFreeMetadataString() {}
   ByteStreamFreeMetadataString(std::string f,std::string s) : 
      std::pair<std::string,std::string>(f,s) {}
   static std::string foldersep() {return "::";}
   static std::string datasep() {return "#";}
   // dump to string
   std::string toString() const;
   // reconstruct from string
   bool fromString(std::string s);
};

inline std::string ByteStreamFreeMetadataString::toString() const {
   std::stringstream ss;
   ss << this->first << datasep() << this->second;
   return ss.str();
}

inline bool ByteStreamFreeMetadataString::fromString(std::string s) {
   std::string::size_type pos = s.find(datasep());
   if (pos==std::string::npos) return false;
   else {
      this->first = s.substr(0,pos);
      this->second = s.substr(pos+1);
   }
   return true;
}

namespace ByteStream 
{     
  class FreeMetadata : public std::vector<ByteStreamFreeMetadataString>
  {
  public:
     FreeMetadata() : m_folder("UNKNOWN") {}
     FreeMetadata(std::string f) : m_folder(f) {}
     std::string folder() const {return m_folder;}
     void setFolder(std::string f) {m_folder=f;}
  private:
     std::string m_folder;
  };
  //typedef std::vector<ByteStreamFreeMetadataString> FreeMetadata;
}

CLASS_DEF (ByteStream::FreeMetadata, 88008866, 1)
#endif
