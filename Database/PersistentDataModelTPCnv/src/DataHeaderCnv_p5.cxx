/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/** @file DataHeaderCnv_p5.cxx
 *  @brief This file contains the implementation for the DataHeaderCnv_p5 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: DataHeaderCnv_p5.cxx,v 1.3.2.2 2009-05-22 19:26:54 gemmeren Exp $
 **/

#include "PersistentDataModel/DataHeader.h"
#include "PersistentDataModelTPCnv/DataHeaderCnv_p5.h"
#include "CxxUtils/sgkey_t.h"
#include "boost/algorithm/string.hpp"

DataHeaderElementCnv_p5::DataHeaderElementCnv_p5() {}
DataHeaderElementCnv_p5::~DataHeaderElementCnv_p5() {}

//______________________________________________________________________________
void DataHeaderElementCnv_p5::persToTrans(const DataHeaderElement_p5& pers,
	DataHeaderElement& trans,
	const DataHeaderForm_p5& form,
        unsigned int entry) const
{
   delete trans.m_token; trans.m_token = new Token; trans.m_ownToken = true;
   Token* token = const_cast<Token*>(trans.m_token);
   std::vector<unsigned int>::const_iterator intIter = form.params(entry).begin();
   unsigned int keyIdx = 0U, aliasNum = 0U, clidNum = 0U;
// Translate PoolToken
   if ((*intIter&0x0000FFFF) == 0) {
      ++intIter;
      keyIdx = *intIter; ++intIter;
      aliasNum = *intIter; ++intIter;
      clidNum = *intIter; ++intIter;
      token->fromString(pers.m_token);
   } else {
      const unsigned int keyPos = (unsigned short)(*intIter>>16),
	      version = (unsigned short)(*intIter&0x0000FFFF); ++intIter;
      const unsigned int guidIdx = (unsigned short)(*intIter>>16),
	      classIdx = (unsigned short)(*intIter&0x0000FFFF); ++intIter;
      const unsigned int prefixIdx = (unsigned short)(*intIter>>16),
	      typeIdx = (unsigned short)(*intIter&0x0000FFFF); ++intIter;
// Add Technology and Offsets
      unsigned int tech = 514;
      if (version&0x00000001U) {
         tech = *intIter; ++intIter;
      }
      unsigned int oid1 = *intIter; ++intIter;
      if (version&0x00000010U) {
         keyIdx = *intIter; ++intIter;
         aliasNum = *intIter; ++intIter;
         clidNum = *intIter; ++intIter;
      } else {
         keyIdx = (*intIter>>16), aliasNum = ((*intIter&0x0000FFFF)>>8), clidNum = (*intIter&0x000000FF); ++intIter;
      }
// Append DbGuid
      Guid guid(form.map()[guidIdx]);
      token->setDb(guid);
// Container name, may be optimized
      std::string cntName;
      if (prefixIdx > 0) {
         cntName += form.map()[prefixIdx];
         if (typeIdx > 0) {
            cntName += form.map()[typeIdx];
         }
      }
// Insert key
      if (keyPos > 0) {
         cntName += pers.m_token.substr(0, keyPos - 1) + form.map()[keyIdx] + pers.m_token.substr(keyPos - 1);
      } else {
         cntName += pers.m_token;
      }
      //token->setCont(cntName);
// Append ClassId
      Guid clid(form.map()[classIdx]);
      token->setClassID(clid);
      token->setTechnology(tech);
      token->setOid(Token::OID_t(oid1, pers.m_oid2));
   }
   unsigned int aliasCur = 0U, clidCur = 0U;
   trans.m_key = form.map()[keyIdx];
   trans.m_alias.clear();
   for (std::set<std::string>::const_iterator lastAlias = trans.m_alias.begin();
		   aliasCur < aliasNum; ++aliasCur) {
      lastAlias = trans.m_alias.insert(lastAlias, form.map()[keyIdx + aliasCur + 1]);
   }
   trans.m_pClid = *intIter; ++intIter;
   trans.m_clids.clear();
   const std::vector<unsigned int>::const_iterator intLast = form.params(entry).end();
   for (std::set<CLID>::const_iterator lastClid = trans.m_clids.begin();
		   intIter != intLast && clidCur < clidNum; ++intIter, ++clidCur) {
      lastClid = trans.m_clids.insert(lastClid, *intIter);
   }
   trans.m_hashes.clear();
   for (; intIter != intLast; ++intIter) {
      trans.m_hashes.push_back(*intIter);
   }
}
//______________________________________________________________________________
void DataHeaderElementCnv_p5::transToPers(const DataHeaderElement& trans,
	DataHeaderElement_p5& pers,
	DataHeaderForm_p5& form,
        unsigned int entry) const {
   unsigned int tech = 0U, oid1 = 0U;
   unsigned short guidIdx = 0U, classIdx = 0U, prefixIdx = 0U, typeIdx = 0U, keyPos = 0U;
// Translate PoolToken
   if (trans.getToken() != 0) {
// Database GUID
      const std::string guid = trans.getToken()->dbID().toString();
      for (std::vector<std::string>::const_iterator iter = form.map().begin(), last = form.map().end();
	      iter != last; ++iter, ++guidIdx) {
         if (*iter == guid) break;
      }
      if (guidIdx == form.map().size()) {
         form.insertMap(guid);
      }
// Class GUID
      const std::string clid = trans.getToken()->classID().toString();
      for (std::vector<std::string>::const_iterator iter = form.map().begin(), last = form.map().end();
	      iter != last; ++iter, ++classIdx) {
         if (*iter == clid) break;
      }
      if (classIdx == form.map().size()) {
         form.insertMap(clid);
      }
// Container name, can be optimized
      pers.m_token = trans.getToken()->contID();
// Get Prefix
      std::string::size_type delim = pers.m_token.find_first_of("_/(");
      if (delim != std::string::npos) {
         const std::string persComp1 = pers.m_token.substr(0, delim + 1);
         for (std::vector<std::string>::const_iterator iter = form.map().begin(), last = form.map().end();
	         iter != last; ++iter, ++prefixIdx) {
            if (*iter == persComp1) break;
         }
         if (prefixIdx == form.map().size()) {
            form.insertMap(persComp1);
         }
         pers.m_token = pers.m_token.substr(delim + 1);
// Get TypeName
// Check whether Key only is used for placement
         if (!boost::starts_with (pers.m_token, trans.m_key)) {
            std::string::size_type delim = pers.m_token.find_first_of("/()");
            if (delim != std::string::npos) {
               const std::string persComp2 = pers.m_token.substr(0, delim + 1);
               for (std::vector<std::string>::const_iterator iter = form.map().begin(), last = form.map().end();
	               iter != last; ++iter, ++typeIdx) {
                  if (*iter == persComp2) break;
               }
               if (typeIdx == form.map().size()) {
                  form.insertMap(persComp2);
               }
               pers.m_token = pers.m_token.substr(delim + 1);
            } else if (pers.m_token == "DataHeader") {
               for (std::vector<std::string>::const_iterator iter = form.map().begin(), last = form.map().end();
	               iter != last; ++iter, ++typeIdx) {
                  if (*iter == "DataHeader") break;
               }
               if (typeIdx == form.map().size()) {
                  form.insertMap("DataHeader");
               }
               pers.m_token.clear();
            }
         }
      }
      delim = pers.m_token.rfind(trans.m_key);
      if (delim != std::string::npos) {
         keyPos = delim + 1;
         pers.m_token = pers.m_token.substr(0, delim) + pers.m_token.substr(delim + trans.m_key.size());
      }
      tech = trans.getToken()->technology();
      oid1 = trans.getToken()->oid().first;
      pers.m_oid2 = trans.getToken()->oid().second;
   }
   unsigned int version = 0x0000FFFFU;
   if (tech == 514) {
      version = version&0x0000FFFEU;
   }
   unsigned int keyIdx = form.map().size(), aliasNum = trans.m_alias.size(), clidNum = trans.m_clids.size();
   if (keyIdx < 0x00010000U && aliasNum < 0x00000100U && clidNum < 0x00000100U) {
      version = version&0x0000FFEFU;
   }
   form.insertParam((unsigned int)(keyPos) * 0x00010000U + version, entry);
   form.insertParam((unsigned int)(guidIdx) * 0x00010000U + (unsigned int)(classIdx), entry);
   form.insertParam((unsigned int)(prefixIdx) * 0x00010000U + (unsigned int)(typeIdx), entry);
   if (version&0x00000001U) {
     form.insertParam(tech, entry);
   }
   form.insertParam(oid1, entry);
   if (version&0x00000010U) {
      form.insertParam(keyIdx, entry);
      form.insertParam(aliasNum, entry);
      form.insertParam(clidNum, entry);
   } else {
      form.insertParam(keyIdx * 0x00010000U + aliasNum * 0x00000100U + clidNum, entry);
   }
   form.insertMap(trans.m_key);
   for (std::set<std::string>::const_iterator iter = trans.m_alias.begin(),
		   last = trans.m_alias.end(); iter != last; ++iter) {
      form.insertMap(*iter);
   }
   form.insertParam(trans.m_pClid, entry);
   for (std::set<CLID>::const_iterator iter = trans.m_clids.begin(),
		   last = trans.m_clids.end(); iter != last; ++iter) {
      form.insertParam(*iter, entry);
   }
   for (std::vector<SG::sgkey_t>::const_iterator iter = trans.m_hashes.begin(),
		   last = trans.m_hashes.end(); iter != last; ++iter) {
     form.insertParam(SG::sgkeyShort (*iter), entry);
   }
}
//______________________________________________________________________________
//______________________________________________________________________________
DataHeaderCnv_p5::DataHeaderCnv_p5() {}
DataHeaderCnv_p5::~DataHeaderCnv_p5() {}
//______________________________________________________________________________
void DataHeaderCnv_p5::persToTrans(const DataHeader_p5& pers,
                                   DataHeader& trans,
                                   const DataHeaderForm_p5& dhForm) const
{
  unsigned int entry = 1;
  const unsigned int provSize = dhForm.params(entry)[0];
  trans.m_inputDataHeader.resize(provSize);
  std::vector<DataHeaderElement>::iterator it = trans.m_inputDataHeader.begin();
  std::vector<DataHeaderElement_p5>::const_iterator pit = pers.m_dataHeader.begin();
  for (unsigned int i = 0U; i < provSize; ++i, ++it, ++pit) {
    ++entry;
    m_elemCnv.persToTrans(*pit, *it, dhForm, entry);
  }
  trans.m_dataHeader.resize(pers.m_dataHeader.size() - provSize);
  it = trans.m_dataHeader.begin();
  for (std::vector<DataHeaderElement_p5>::const_iterator last = pers.m_dataHeader.end();
       pit != last; ++it, ++pit) {
    ++entry;
    m_elemCnv.persToTrans(*pit, *it, dhForm, entry);
  }
  trans.setStatus(DataHeader::Input);
}
//______________________________________________________________________________
void DataHeaderCnv_p5::transToPers(const DataHeader& trans,
                                   DataHeader_p5& pers,
                                   DataHeaderForm_p5& dhForm) const
{
   const unsigned int provSize = trans.m_inputDataHeader.size();
   pers.m_dataHeader.resize(provSize + trans.m_dataHeader.size());
   dhForm.resize(provSize + trans.m_dataHeader.size() + 2);
   unsigned int entry = 1;
   dhForm.insertParam(provSize, entry);
   std::vector<DataHeaderElement_p5>::iterator pit = pers.m_dataHeader.begin();
   for (std::vector<DataHeaderElement>::const_iterator it = trans.m_inputDataHeader.begin(),
		   last = trans.m_inputDataHeader.end(); it != last; ++it, ++pit) {
      ++entry;
      m_elemCnv.transToPers(*it, *pit, dhForm, entry);
   }
   for (std::vector<DataHeaderElement>::const_iterator it = trans.m_dataHeader.begin(),
		   last = trans.m_dataHeader.end(); it != last; ++it, ++pit) {
      ++entry;
      m_elemCnv.transToPers(*it, *pit, dhForm, entry);
   }
}
//______________________________________________________________________________
void DataHeaderCnv_p5::insertDHRef(DataHeader_p5& pers,
                                   DataHeaderForm_p5& dhForm,
                                   const std::string& key,
                                   const std::string& strToken) const
{
  Token* token = new Token;
  token->fromString(strToken);
  DataHeaderElement tEle(ClassID_traits<DataHeader>::ID(), key, token);
  DataHeaderElement_p5 pEle;
  unsigned int entry = dhForm.size() + 1;
  m_elemCnv.transToPers(tEle, pEle, dhForm, entry);
  pers.m_dataHeader.push_back(pEle);
}
