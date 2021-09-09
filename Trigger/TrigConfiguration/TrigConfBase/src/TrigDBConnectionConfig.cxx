/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @file TrigDBConnectionConfig.cxx
 * @brief Configuration for a Trigger DB connection, with string serialization
 * @author Ricardo Abreu
 * $Id: TrigDBConnectionConfig.cxx 569440 2013-11-08 17:32:05Z ricab $
 */

#include "TrigConfBase/TrigDBConnectionConfig.h"
#include <stdexcept>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>

using PSKeys = TrigConf::TrigDBConnectionConfig::PSKeys;
using namespace boost;
using namespace std;

namespace
{

  /////////////////////
  // parameter names //
  /////////////////////
  const string TYPE     = "type"
             , SERVER   = "server"
             , SMKEY    = "smkey"
             , HLTKEY   = "hltkey"
             , HLTKEYS  = "hltkeys"
             , LVL1KEY  = "lvl1key"
             , SCHEMA   = "schema"
             , USER     = "user"
             , PWD      = "password"
             , RETRPER  = "retrialperiod"
             , MAXRETR  = "maxretrials"
             , USEFRONT = "usefrontier"
             ;

  ///////////////////////
  // Element separator //
  ///////////////////////
  const string SEP = ";";

  //////////////////////////////
  // Regex element framgments //
  //////////////////////////////

  const string e_pair_open(R"(\s*\()")        // pair open: possible space + (
             , e_pair_separate(",")           // pair separator
             , e_pair_close(R"(\)\s*)")        // pair close: ) + possible space
             , e_uint_core_dec("\\d+")         // simple positive decimal
             , e_uint_core_oct("0[0-7]+")      // simple positive octal
             , e_uint_core_hex("0x[\\da-f]+")  // simple positive hex
                                               // (needs case insensitive)
             , e_uint_neg_zero("-0+|-0x0+")    // exceptional zeros (with minus)
             , e_list_open(R"(\s*\[)")         // list open: possible space + [
             , e_list_separate(",")            // list separator
             , e_list_close(R"(\]\s*)")        // list close: ] + possible space

             , e_param_open(R"((?:^|)"         // start of line or
                            + SEP + "|"        // separator or
                            R"((?<=)"          // past
                            + SEP + ""         // separator
                            R"())\s*)")        // all followed by possible space
                                               // (needed when iterating over
                                               // successive parameters)
             , e_param_eq("\\s*=")             // separates param name from val
             , e_param_close("(?:"             // (open non capture group)
                             + SEP +           // either separator or
                             "|$)")            // end of line (close group)
             , e_param_val(R"(\s*(.*?)\s*)")   // captured value surrounded by
                                               // possible spaces
             ;

  ///////////////////////
  // Regex combination //
  ///////////////////////

  // Non negative integer: possible spaces, followed by one of the following:
  //    - possible plus sign followed by hex, oct, or dec representation;
  //    - zero representations with minus sign;
  // Followed by possible spaces
  const string e_uint(R"(\s*(?:\+?(?:)" + e_uint_core_hex + "|"
                                        + e_uint_core_oct + "|"
                                        + e_uint_core_dec + ")|"
                                 + e_uint_neg_zero + R"()\s*)");

  // A pair of non negative integers, with a marking group for each of them
  const string e_pair(e_pair_open + "(" + e_uint + ")"
                      + e_pair_separate + "(" + e_uint + ")"
                      + e_pair_close);

  // an empty list
  const string e_list_empty(e_list_open + "\\s*" + e_list_close);
  // a (possibly empty) list of pairs of non negative integers
  const string e_list(e_list_open + e_pair
                      + "(?:" + e_list_separate + e_pair + ")*"
                      + e_list_close + "|" + e_list_empty);

  // actual regexes
  const regex re_uint(e_uint),
              re_pair(e_pair),
              re_list(e_list);

  //////////////////////////////////////////////////////////////////////////////
  unsigned int getUInt(const string& str, const string& aname)
  {
    int ret = stoi(str, nullptr, 0);
    if(ret < 0)
      throw invalid_argument("The string specifying the " + aname + " does not "
                             "represent a valid non-negative integer: " + str);

    return ret;
  }

  //////////////////////////////////////////////////////////////////////////////
  PSKeys parseHltPsKeys(const string& str)
  {
    PSKeys ret;
    // first validade the format
    if(regex_match(str, re_list))
      // iterate over the pairs that are contained in the list
      for(sregex_iterator it = make_regex_iterator(str, re_pair);
          it != sregex_iterator(); ++it)
        // push back a pair with the two captured integers
        ret.push_back(make_pair(stoi((*it)[1], nullptr, 0),
                                stoi((*it)[2], nullptr, 0)));
    else
      throw invalid_argument("The string specifying the HLT PS Keys is badly "
                             "formated: " + str);
    return ret;
  }

  //////////////////////////////////////////////////////////////////////////////
  string extractValue(const string& name, const string& str)
  {
    string val;
    regex re_param(e_param_open
                   + name
                   + e_param_eq
                   + e_param_val
                   + e_param_close);

    // get to the last occurrence
    for(sregex_iterator it = make_regex_iterator(str, re_param);
        it != sregex_iterator(); ++it)
      val = (*it)[1]; // if no match, we never get here and end up returning ""

    return val;
  }

  //////////////////////////////////////////////////////////////////////////////
  template <typename T>
  void outputParam(ostringstream& oss, const string& name, const T& value,
                   bool first=false)
  {
    if(!first)
      oss << SEP;
    oss << name << "=" << value;
  }
}

////////////////////////////////////////////////////////////////////////////////
TrigConf::
TrigDBConnectionConfig::TrigDBConnectionConfig(DBType type,
                                               const string& server,
                                               unsigned int smKey,
                                               const string& hltPsKeyStr)
  : TrigDBConnectionConfig(type, server, smKey)
{
  setHltKeysFromStr(hltPsKeyStr);
}

////////////////////////////////////////////////////////////////////////////////
TrigConf::
TrigDBConnectionConfig::TrigDBConnectionConfig(const string& connectionStr)
{
  diggestStr(connectionStr);
}

////////////////////////////////////////////////////////////////////////////////
string TrigConf::TrigDBConnectionConfig::toString() const
{
  ostringstream oss;
  outputParam(oss, TYPE, typeToString(), true);
  outputParam(oss, SERVER, m_server);
  outputParam(oss, SMKEY, m_smkey);
  outputParam(oss, LVL1KEY, m_lvl1key);
  outputParam(oss, SCHEMA, m_schema);
  outputParam(oss, USER, m_user);
  outputParam(oss, PWD, m_password);
  outputParam(oss, RETRPER, m_retrialPeriod);
  outputParam(oss, MAXRETR, m_maxRetrials);
  outputParam(oss, USEFRONT, m_useFrontier);

  if(m_hltkey)
    outputParam(oss, HLTKEY, m_hltkey);
  else
    outputParam(oss, HLTKEYS, hltKeysToString());

  return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
void TrigConf::TrigDBConnectionConfig::diggestStr(const string& str)
{
  string val;

  // for each attribute, if a value was provided in the connectionString, set it
  if(!(val = extractValue(TYPE, str)).empty())
    setTypeFromStr(val);
  if(!(val = extractValue(SERVER, str)).empty())
    m_server = val;
  if(!(val = extractValue(SMKEY, str)).empty())
    setSmKeyFromStr(val);
  if(!(val = extractValue(LVL1KEY, str)).empty())
    setLvl1KeyFromStr(val);
  if(!(val = extractValue(SCHEMA, str)).empty())
    m_schema = val;
  if(!(val = extractValue(USER, str)).empty())
    m_user = val;
  if(!(val = extractValue(PWD, str)).empty())
    m_password = val;
  if(!(val = extractValue(RETRPER, str)).empty())
    setRetrialPeriodFromStr(val);
  if(!(val = extractValue(MAXRETR, str)).empty())
    setMaxRetrialsFromStr(val);
  if(!(val = extractValue(USEFRONT, str)).empty())
    setUseFrontierFromStr(val);

  // only one of the following should be specified
  if(!(val = extractValue(HLTKEY, str)).empty())
    setHltKeysFromStr(val);
  if(!m_hltkey && !(val = extractValue(HLTKEYS, str)).empty())
    setHltKeysFromStr(val);
}

////////////////////////////////////////////////////////////////////////////////
void TrigConf::TrigDBConnectionConfig::setTypeFromStr(const string& typeStr)
{
  string low = algorithm::to_lower_copy(typeStr);
  if(low == "oracle")
    m_type = Oracle;
  else if(low == "mysql")
    m_type = MySQL;
  else if(low == "sqlite")
    m_type = SQLite;
  else
    m_type = DBLookup;
}

////////////////////////////////////////////////////////////////////////////////
void
TrigConf::TrigDBConnectionConfig::setHltKeysFromStr(const string& hltKeyStr)
{
  if(regex_match(hltKeyStr, re_uint))
    m_hltkey = stoi(hltKeyStr, nullptr, 0);
  else
    m_hltkeys = parseHltPsKeys(hltKeyStr);
}

////////////////////////////////////////////////////////////////////////////////
void TrigConf::TrigDBConnectionConfig::setSmKeyFromStr(const string& smKeyStr)
{
  m_smkey = getUInt(smKeyStr, SMKEY);
}

////////////////////////////////////////////////////////////////////////////////
void
TrigConf::TrigDBConnectionConfig::setLvl1KeyFromStr(const string& lvl1KeyStr)
{
  m_lvl1key = getUInt(lvl1KeyStr, LVL1KEY);
}

////////////////////////////////////////////////////////////////////////////////
void TrigConf::
TrigDBConnectionConfig::setRetrialPeriodFromStr(const string& retrialPeriodStr)
{
  m_retrialPeriod = getUInt(retrialPeriodStr, RETRPER);
}

////////////////////////////////////////////////////////////////////////////////
void TrigConf::
TrigDBConnectionConfig::setMaxRetrialsFromStr(const string& maxRetrialsStr)
{
  m_maxRetrials = getUInt(maxRetrialsStr, MAXRETR);
}

////////////////////////////////////////////////////////////////////////////////
void TrigConf::
TrigDBConnectionConfig::setUseFrontierFromStr(const std::string& useFrontier)
{
  m_useFrontier = static_cast<bool>(getUInt(useFrontier, USEFRONT));
}

////////////////////////////////////////////////////////////////////////////////
string TrigConf::TrigDBConnectionConfig::hltKeysToString() const
{
  ostringstream oss;

  oss << '[';
  for(PSKeys::const_iterator it = m_hltkeys.begin();
      it != m_hltkeys.end(); ++it)
  {
    if(it != m_hltkeys.begin())
      oss << ',';
    oss << '(' << it->first << ',' << it->second << ')';
  }
  oss << ']';

  return oss.str();
}
