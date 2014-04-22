/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAGMETADATAKEY_H
#define TAGMETADATAKEY_H

///////////////////////////////////////////////////////////////////////
//
// @class TagMetadataKey
// @author Jack.Cranshaw@cern.ch
//
// Class to translate a key+qualifier into a string
//   to use as a single key with CollectionMetadata.
// Followup tools can then 'key' off of the key or key+qualifier
//   Example: r11111_thisStuff_bit6 key = thisStuff, qual = (r=11111,b=6)
//
///////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

class TagMetadataKey 
{
public:
   //friend class ICollectionMetadataHelper;
   static std::string sep()      {return ";";}
   static std::string keymark()  {return "Key@";}
   static std::string qualmark() {return "Qual@";}
   static std::string iovmark()  {return "IOV@";}
   static std::string vermark()  {return "Ver@";}

   TagMetadataKey();
   TagMetadataKey(std::string key, std::string qual="", bool versioned=false);

   void setEncoded(bool flag);
   void setKey(std::string key);
   void setQual(std::string qual);
   void setIOV(std::string iov);
   void setVersion(std::string ver);

   bool isVersioned() {return m_verstat;}

   bool encoded() const;
   std::string key() const;
   std::string qual() const;
   std::string iov() const;
   std::string ver() const;

   bool fromString(std::string comp);
   std::string toString() const;

private:
   std::string getpayload(const std::string key, const std::string comp);

   std::string m_key;
   std::string m_qual;
   std::string m_iov;
   std::string m_ver;
   bool m_verstat;  // verstat is true if the key is ALLOWED to be versioned
   bool m_encoded;
};

inline
TagMetadataKey::TagMetadataKey() : m_verstat(false), m_encoded(true)
{}

inline
TagMetadataKey::TagMetadataKey(std::string key, std::string qual, bool versioned) : 
      m_key(key), m_qual(qual), m_verstat(versioned), m_encoded(true)
{if (m_verstat) m_ver="0";}

inline void 
TagMetadataKey::setEncoded(bool flag) {m_encoded=flag;}

inline void 
TagMetadataKey::setKey(std::string key) {m_key=key;}

inline void 
TagMetadataKey::setQual(std::string qual) {m_qual=qual;}

inline void 
TagMetadataKey::setIOV(std::string iov) {m_iov=iov;}

inline void 
TagMetadataKey::setVersion(std::string ver) {m_ver = ver;}

inline bool
TagMetadataKey::encoded() const {return m_encoded;}

inline std::string 
TagMetadataKey::key()     const {return m_key;}

inline std::string 
TagMetadataKey::qual()    const {return m_qual;}

inline std::string 
TagMetadataKey::iov()     const {return m_iov;}

inline std::string 
TagMetadataKey::ver()     const {return m_ver;}

inline bool 
TagMetadataKey::fromString(std::string comp)
{
   bool ready = false;
   char sp = sep()[0];
   int nseps = std::count(comp.begin(),comp.end(),sp);
   // All marks should be denoted with an @
   int nmarks = std::count(comp.begin(),comp.end(),'@');
   //std::cout << "seps =" << nseps <<" marks=" << nmarks << std::endl;
   if (nmarks > 0 && nseps == nmarks) { 
      // first check for key. if no key then ready is false
      std::string payload = getpayload(keymark(),comp);
      if (payload != "BAD") {
         m_encoded=true;
         m_key = payload;
         // check for qualifier
         payload = getpayload(qualmark(),comp);
         if (payload != "BAD") m_qual = payload;
         else m_qual.clear();
         // check for iov
         payload = getpayload(iovmark(),comp);
         if (payload != "BAD") m_iov = payload;
         else m_iov.clear();
         // check for version
         payload = getpayload(vermark(),comp);
         if (payload != "BAD") m_ver = payload;
         else m_ver.clear();
         ready = true;
      }
   }
   else {
      m_key = comp;
      m_encoded = false;
      std::cerr << "WARNING: No markers found in " 
                << comp << ". Treating as unencoded single key." << std::endl;
   }
   return ready;
}

inline std::string 
TagMetadataKey::toString() const
{
   std::stringstream out;
   if (m_encoded) {
      out << keymark() << m_key << sep();
      if (m_qual.size()>0) out << qualmark() << m_qual << sep();
      if (m_iov.size()>0)  out << iovmark()  << m_iov  << sep();
      if (m_verstat && m_ver.size()>0) {
         out << vermark() << m_ver << sep();
      }
   }
   else out << m_key;
   return out.str();
}

inline std::string
TagMetadataKey::getpayload(const std::string key, const std::string comp)
{
   std::string bad("BAD");
   std::string payload;
   // Look for named key being considered
   std::string::size_type start = comp.find(key);
   if (start != std::string::npos) {
      // if key exists, then look for endmarker, need at least one after the @
      //   starting at key name position
      std::string::size_type payend = comp.find(sep(),start);
      std::string::size_type paystart = comp.find('@',start);
      if (paystart<payend) {
         // grab substring between @ and sep()
         payload = comp.substr(start+keymark().size(),payend-start-keymark().size());
      }
      else {
         std::cerr << "WARNING: missing sep() for key" << std::endl;
         return bad;
      }
   }
   return payload;
}

#endif
