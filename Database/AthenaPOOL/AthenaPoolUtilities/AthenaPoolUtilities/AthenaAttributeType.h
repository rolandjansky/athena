/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLUTILITIES_ATHENAATTRIBUTETYPE_H
#define ATHENAPOOLUTILITIES_ATHENAATTRIBUTETYPE_H

#include <string>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <map>
#include <set>

//////////////////////////////////////////////////////////////////////////////
//
// AthenaAttributeType puts a structure into the annotation field 
//    in an AthenaAttributeSpecification(name,type,annotation)
//
//      - typename (redundant with type in AthenaAttributeSpecification)
//      - unit (place for the units of this particular variable
//      - group (a superset of variables like Electron, EventID, Test, ...)
//
//  It makes an encoded string available for storage in annotation in the form
//      KEY1=VALUE1;KEY2=VALUE2;...
//  Only keys registered in the constructor (TYPE,UNIT,GRP) are allowed. 
//
//////////////////////////////////////////////////////////////////////////////

class AthenaAttributeType
{
public:
  // default
  AthenaAttributeType(){;}
  // based on allowed keys
  AthenaAttributeType(std::string t, std::string u, std::string g);
  // based on encoded string like what is returned by info()
  //AthenaAttributeType(std::string encoded); 
 
  // returns encoded strings
  std::string info() const;
  std::string extraInfo() const; // does not include type since is redundant
  
  // set methods
  void setType(const std::string& info) {setInfoForKey("TYPE",info);}
  void setUnit(const std::string& info) {setInfoForKey("UNIT",info);}
  void setGroup(const std::string& info) {setInfoForKey("GRP",info);}
  void fromString(const std::string& info);

  // get methods
  std::string typeName() const;
  std::string unit() const;
  std::string group() const;

  // return allowed keys
  std::set<std::string> getKeys() {return m_keys;}

  bool operator==(const AthenaAttributeType& rhs) const;
  bool operator!=(const AthenaAttributeType& rhs) const;
  
private:

  bool setInfoForKey(const std::string& key, const std::string& info);

  std::map<std::string,std::string> m_keyedInfo;
  std::set<std::string> m_keys;
};

inline
AthenaAttributeType::AthenaAttributeType(std::string t, 
                                         std::string u="UNKNOWN",
                                         std::string g="UNKNOWN")
{
   m_keyedInfo.clear();
   m_keys.clear();
   m_keys.insert("TYPE");
   m_keys.insert("UNIT");
   m_keys.insert("GRP");
   m_keyedInfo.insert(std::make_pair("TYPE",t));
   m_keyedInfo.insert(std::make_pair("UNIT",u));
   m_keyedInfo.insert(std::make_pair("GRP",g));
}

inline
bool AthenaAttributeType::setInfoForKey(const std::string& key,
                                        const std::string& info)
{
   if ( m_keys.find(key) != m_keys.end() ) {
      std::map<std::string,std::string>::iterator i = m_keyedInfo.find(key);
      if (i != m_keyedInfo.end()) i->second=info;
      else m_keyedInfo.insert(std::make_pair(key,info));
      return true;
   }
   else return false;
}

inline void
AthenaAttributeType::fromString(const std::string& i)
{
   m_keyedInfo.clear();
   m_keys.clear();

   // search for type first
   std::set<std::string> keys;
   keys.insert("TYPE");
   keys.insert("UNIT");
   keys.insert("GRP");
   std::string::size_type flagpos, endpos;
   for (std::set<std::string>::iterator it = keys.begin(); it!=keys.end(); ++it) {
      flagpos = i.find(*it);
      if (flagpos != std::string::npos) {
         endpos = i.find(";",flagpos);
         if (endpos != std::string::npos) {
            this->setInfoForKey(*it,i.substr(flagpos,endpos));
         }
      }
   }
}

inline
std::string AthenaAttributeType::typeName() const
{
   std::map<std::string,std::string>::const_iterator temp = m_keyedInfo.find("TYPE");
   if (temp!=m_keyedInfo.end()) return temp->second;
   else return std::string("NOTFOUND");
}

inline
std::string AthenaAttributeType::unit() const
{
   std::map<std::string,std::string>::const_iterator temp = m_keyedInfo.find("UNIT");
   if (temp!=m_keyedInfo.end()) return temp->second;
   else return std::string("NOTFOUND");
}

inline
std::string AthenaAttributeType::group() const
{
   std::map<std::string,std::string>::const_iterator temp = m_keyedInfo.find("GRP");
   if (temp!=m_keyedInfo.end()) return temp->second;
   else return std::string("NOTFOUND");
}

inline
std::string AthenaAttributeType::info() const
{
  std::ostringstream os;
  os << "TYPE="<<typeName()<<";"<<"UNIT="<<unit()<<";"<<"GRP="<<group()<<";";
  return os.str();
}

inline
std::string AthenaAttributeType::extraInfo() const
{
  std::ostringstream os;
  os << "UNIT="<<unit()<<";"<<"GRP="<<group()<<";";
  return os.str();
}

inline
bool AthenaAttributeType::operator==(const AthenaAttributeType& rhs) const
{
   return (this->typeName()==rhs.typeName()) &&
          (this->unit()==rhs.unit()) &&
          (this->group()==rhs.group());
}

inline
bool AthenaAttributeType::operator!=(const AthenaAttributeType& rhs) const
{
   return !(*this==rhs);
}

#endif
