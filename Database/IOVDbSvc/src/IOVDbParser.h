/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IOVDbParser.h
// helper class for IOVDbSvc implementing a simple XML-style parser
// Richard Hawkings, started 29/11/08
#ifndef __IOVDBPARSER_H__
#define __IOVDBPARSER_H__

#include <string>
#include <map>

class MsgStream;

class IOVDbParser {
public:

  IOVDbParser() = delete; 
  IOVDbParser(const std::string& input, MsgStream* log);

  bool isValid() const;
  bool getKey(const std::string& key, const std::string& devvalue, 
              std::string& value) const;
  std::string spaceStrip(const std::string& input) const;
  const std::string& folderName() const;
  unsigned applyOverrides(const IOVDbParser& other, MsgStream* log);
  bool operator==(const IOVDbParser& other) const; 
  std::string toString() const;
  void clean();

private:
  MsgStream* m_log;
  bool m_valid;
  typedef std::map<std::string, std::string> KeyValMap;
  KeyValMap m_keys;
};

inline bool IOVDbParser::isValid() const { return m_valid; }

#endif //  __IOVDBPARSER_H__


MsgStream& operator<<(MsgStream& os, const IOVDbParser& fldr); 
