/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//@file IOVDbParser.h
//@brief helper class for IOVDbSvc implementing a simple XML-style parser to parse
// the folder description string or the provided folder description in job options
//@author Richard Hawkings, started 29/11/08
#ifndef IOVDBSVC_IOVDBPARSER_H
#define IOVDBSVC_IOVDBPARSER_H

#include <string>
#include <map>
#include <vector>
#include "GaudiKernel/ClassID.h" //typedef

class MsgStream;

class IOVDbParser {
public:

  IOVDbParser() = delete; 
  IOVDbParser( const std::string &input, MsgStream& log);
  bool isValid() const;
  ///'at' accessor with an optional default; the bool is true if the key was found
  std::pair<std::string, bool>
  at(const std::string & searchKey, const std::string & defaultValue="") const;
  ///original 'getKey' method, now implemented using 'at'
  bool getKey(const std::string& key, const std::string& devvalue,std::string& value) const;
  /// give the folder name contained in the parsed description
  std::string folderName() const;
  std::string key() const;
  bool hasKey() const;
  std::string tag() const;
  std::string eventStoreName() const;
  bool timebaseIs_nsOfEpoch() const;
  std::string cache() const;
  int cachehint() const;
  bool named() const;
  std::string addressHeader() const;
  std::vector<std::string> symLinks() const;
  bool noTagOverride() const;
  CLID classId(MsgStream& msg) const;
  bool onlyReadMetadata() const;
  bool extensible() const;
  
  unsigned applyOverrides(const IOVDbParser& other, MsgStream & log);
  ///return true if this description overrides the timestamp or runlumi
  bool overridesIov(MsgStream& msg) const;
  ///as for overridesIov(), but with a check that the folder time unit is compatible with 
  ///the override time unit
  bool overridesIov(MsgStream& msg, const bool folderIs_nsOfEpoch) const;
  unsigned long long iovOverrideValue(MsgStream& msg) const;
  bool operator==(const IOVDbParser& other) const; 
  std::string toString() const;
  void clean();

private:
  bool m_valid;
  typedef std::map<std::string, std::string> KeyValMap;
  KeyValMap m_keys;
  ///implementation of overridesIov, with or without check on folder compatibility
  bool overridesIovImpl(MsgStream& msg, const bool performFolderCheck,const bool folderIs_nsOfEpoch=true) const;
 
};

inline bool IOVDbParser::isValid() const { return m_valid; }

MsgStream& operator<<(MsgStream& os, const IOVDbParser& fldr); 


#endif //  IOVDBSVC_IOVDBPARSER_H
