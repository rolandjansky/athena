/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file DataHeaderCnv_p6.cxx
 *  @brief This file contains the implementation for the DataHeaderCnv_p6 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: DataHeaderCnv_p6.cxx,v 1.3.2.2 2009-05-22 19:26:54 gemmeren Exp $
 **/

#include "PersistentDataModel/DataHeader.h"
#include "PersistentDataModelTPCnv/DataHeaderCnv_p6.h"

DataHeaderElementCnv_p6::DataHeaderElementCnv_p6() {}
DataHeaderElementCnv_p6::~DataHeaderElementCnv_p6() {}

//______________________________________________________________________________
void DataHeaderElementCnv_p6::persToTrans(const DataHeaderElement_p6* pers,
	DataHeaderElement* trans,
	const DataHeaderForm_p6& form) {
   delete trans->m_token; trans->m_token = new Token;
   Token* token = const_cast<Token*>(trans->m_token);
// Append DbGuid
   token->setDb(form.getDbGuid(pers->m_dbIdx));
   token->setTechnology(form.getDbTech(pers->m_dbIdx));
// Append ClassId
   token->setClassID(form.getObjClassId(pers->m_objIdx));
   token->setOid(Token::OID_t(pers->m_oid1, pers->m_oid2));
// StoreGate
   trans->m_key = form.getObjKey(pers->m_objIdx);
   trans->m_alias = form.getObjAlias(pers->m_objIdx);
   trans->m_pClid = form.getObjType(pers->m_objIdx);
   trans->m_clids = form.getObjSymLinks(pers->m_objIdx);
   trans->m_hashes = form.getObjHashes(pers->m_objIdx);
}
//______________________________________________________________________________
void DataHeaderElementCnv_p6::transToPers(const DataHeaderElement* trans,
	DataHeaderElement_p6* pers,
	DataHeaderForm_p6& form) {
// Translate PoolToken
   if (trans->getToken() != 0) {
// Database GUID & Technology
      DataHeaderForm_p6::DbRecord transDb(trans->getToken()->dbID(), trans->getToken()->technology());
      pers->m_dbIdx = form.insertDb(transDb);
// StoreGate Type/Key & persistent Class GUID
      DataHeaderForm_p6::SgRecord transSg(trans->m_key, trans->m_pClid);
      DataHeaderForm_p6::ObjRecord transObj(trans->getToken()->classID(), transSg);
      pers->m_objIdx = form.insertObj(transObj, trans->m_alias, trans->m_clids, trans->m_hashes);
      pers->m_oid1 = trans->getToken()->oid().first;
      pers->m_oid2 = trans->getToken()->oid().second;
   }
}
//______________________________________________________________________________
//______________________________________________________________________________
DataHeaderCnv_p6::DataHeaderCnv_p6() {}
DataHeaderCnv_p6::~DataHeaderCnv_p6() {}
//______________________________________________________________________________
void DataHeaderCnv_p6::persToTrans(const DataHeader_p6* pers, DataHeader* trans) {
   pers->m_dhForm.start();
   const unsigned int provSize = pers->m_provenanceSize;
   trans->m_inputDataHeader.resize(provSize);
   std::vector<DataHeaderElement>::iterator it = trans->m_inputDataHeader.begin();
   std::vector<DataHeaderElement_p6>::const_iterator pit = pers->m_dataHeader.begin();
   for (unsigned int i = 0U; i < provSize; i++, it++, pit++) {
      m_elemCnv.persToTrans(&(*pit), &(*it), pers->m_dhForm);
      pers->m_dhForm.next();
   }
   trans->m_dataHeader.resize(pers->m_dataHeader.size() - provSize);
   it = trans->m_dataHeader.begin();
   for (std::vector<DataHeaderElement_p6>::const_iterator last = pers->m_dataHeader.end();
		   pit != last; it++, pit++) {
      m_elemCnv.persToTrans(&(*pit), &(*it), pers->m_dhForm);
      pers->m_dhForm.next();
   }
}
//______________________________________________________________________________
void DataHeaderCnv_p6::transToPers(const DataHeader* trans, DataHeader_p6* pers) {
   const unsigned int provSize = trans->m_inputDataHeader.size();
   pers->m_dataHeader.resize(provSize + trans->m_dataHeader.size());
   pers->m_provenanceSize = provSize;
   pers->m_dhForm.resize(provSize + trans->m_dataHeader.size() + 1);
   pers->m_dhForm.start();
   std::vector<DataHeaderElement_p6>::iterator pit = pers->m_dataHeader.begin();
   for (std::vector<DataHeaderElement>::const_iterator it = trans->m_inputDataHeader.begin(),
		   last = trans->m_inputDataHeader.end(); it != last; it++, pit++) {
      m_elemCnv.transToPers(&(*it), &(*pit), pers->m_dhForm);
      pers->m_dhForm.next();
   }
   for (std::vector<DataHeaderElement>::const_iterator it = trans->m_dataHeader.begin(),
		   last = trans->m_dataHeader.end(); it != last; it++, pit++) {
      m_elemCnv.transToPers(&(*it), &(*pit), pers->m_dhForm);
      pers->m_dhForm.next();
   }
}
//______________________________________________________________________________
void DataHeaderCnv_p6::insertDHRef(DataHeader_p6* pers,
	const std::string& key,
	const std::string& strToken) {
   Token* token = new Token;
   token->fromString(strToken);
   DataHeaderElement tEle(ClassID_traits<DataHeader>::ID(), key, token);
   DataHeaderElement_p6 pEle;
   pers->m_dhForm.next();
   m_elemCnv.transToPers(&tEle, &pEle, pers->m_dhForm);
   pers->m_dataHeader.push_back(pEle);
}
