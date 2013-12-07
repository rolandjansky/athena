/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @file TrigDBConnectionConfig.h
 * @brief Configuration for a Trigger DB connection, with string serialization
 * @author Ricardo Abreu
 * $Id: TrigDBConnectionConfig.h 569440 2013-11-08 17:32:05Z ricab $
 */

#ifndef TRIGDBCONNECTIONCONFIG_H_
#define TRIGDBCONNECTIONCONFIG_H_

#include <string>
#include <vector>
#include <utility>

namespace TrigConf {

struct TrigDBConnectionConfig
{
  enum DBType { Oracle, MySQL, SQLite, DBLookup };
  typedef std::pair<unsigned int, unsigned int> LumiPSPair;
  typedef std::vector<LumiPSPair> PSKeys;

  TrigDBConnectionConfig() = default;
  //   If some parameter is repeated in the string, the last occurrence is the
  // one taken into account
  TrigDBConnectionConfig(const std::string& connectionStr);
  TrigDBConnectionConfig(DBType type,
                         const std::string& server,
                         unsigned int smKey,
                         unsigned int hltPsKey);
  TrigDBConnectionConfig(DBType type,
                         const std::string& server,
                         unsigned int smKey,
                         const PSKeys& hltPsKeyStr);
  TrigDBConnectionConfig(DBType type,
                         const std::string& server,
                         unsigned int smKey,
                         const std::string& hltPsKeyStr);

  void diggestStr(const std::string& str);

  // setters from strings for non string attributes
  void setTypeFromStr(const std::string& typeStr);
  void setSmKeyFromStr(const std::string& smKeyStr);
  // accepts single or multiple keys
  void setHltKeysFromStr(const std::string& hltKeyStr);
  void setLvl1KeyFromStr(const std::string& lvl1KeyStr);
  void setRetrialPeriodFromStr(const std::string& retrialPeriodStr);
  void setMaxRetrialsFromStr(const std::string& maxRetrialsStr);
  void setUseFrontierFromStr(const std::string& useFrontier);

  std::string toString() const;
  std::string typeToString() const;
  std::string hltKeysToString() const;

  // Attributes
  DBType m_type{DBLookup};
  std::string m_server;
  unsigned int m_smkey{0};
  unsigned int m_hltkey{0};
  PSKeys m_hltkeys;
  unsigned int m_lvl1key{0};
  std::string m_schema;
  std::string m_user;
  std::string m_password;
  unsigned int m_retrialPeriod{0};
  unsigned int m_maxRetrials{1};
  bool m_useFrontier{false};

private:
  TrigDBConnectionConfig(DBType type,
                         const std::string& server,
                         unsigned int smKey);

};

}

////////////////////////////////////////////////////////////////////////////////
inline TrigConf::
TrigDBConnectionConfig::TrigDBConnectionConfig(DBType type,
                                               const std::string& server,
                                               unsigned int smKey)
  : m_type(type)
  , m_server(server)
  , m_smkey(smKey)
{
}

////////////////////////////////////////////////////////////////////////////////
inline TrigConf::
TrigDBConnectionConfig::TrigDBConnectionConfig(DBType type,
                                               const std::string& server,
                                               unsigned int smKey,
                                               unsigned int hltPsKey)
  : TrigDBConnectionConfig(type, server, smKey)
{
  m_hltkey = hltPsKey;
}

////////////////////////////////////////////////////////////////////////////////
inline TrigConf::
TrigDBConnectionConfig::TrigDBConnectionConfig(DBType type,
                                               const std::string& server,
                                               unsigned int smKey,
                                               const PSKeys& hltPsKeys)
  : TrigDBConnectionConfig(type, server, smKey)
{
  m_hltkeys = hltPsKeys;
}

////////////////////////////////////////////////////////////////////////////////
inline std::string TrigConf::TrigDBConnectionConfig::typeToString() const
{
  switch(m_type)
  {
  case Oracle: return "oracle";
  case MySQL:  return "mysql";
  case SQLite: return "sqlite";
  case DBLookup: return "dblookup";
  }

  return "UnknownType!";
}

#endif /* TRIGDBCONNECTIONCONFIG_H_ */
