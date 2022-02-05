/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/** @file DataHeaderCnv_p6.cxx
 *  @brief This file contains the implementation for the DataHeaderCnv_p6 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "PersistentDataModel/DataHeader.h"
#include "PersistentDataModelTPCnv/DataHeader_p6.h"
#include "PersistentDataModelTPCnv/DataHeaderCnv_p6.h"
#include <climits>

using FullElement  = DataHeader_p6::FullElement;

//______________________________________________________________________________
void DataHeaderCnv_p6::persToElem( const DataHeader_p6* pers, unsigned p_idx,
                                    DataHeaderElement* trans, const DataHeaderForm_p6& form )
{
   delete trans->m_token;  trans->m_token = nullptr;
   int obj_idx = pers->m_shortElements[p_idx];
   if( obj_idx != INT32_MIN ) {
      Token* token = new Token;
      trans->m_token = token;
      trans->m_ownToken = true;
      unsigned db_idx = 0;
      unsigned long long oid2 = 0;
      if( obj_idx >= 0 ) {
         db_idx = pers->m_commonDbIndex;
         oid2   = pers->m_commonOID2;
      } else {
         const FullElement &full_el = pers->m_fullElements[ -1 - obj_idx ];
         db_idx = full_el.dbIdx;
         obj_idx = full_el.objIdx;
         oid2 = full_el.oid2;
      }

      if( form.sizeDb() <= db_idx || form.sizeObj() <= (size_t)obj_idx ) {
         throw std::runtime_error("DataHeader/DataHeaderForm indices out of range. DataHeader can NOT be read!");
      }
      // Append DbGuid
      token->setDb(         form.getDbGuid( db_idx ) );
      token->setCont(       form.getObjContainer( obj_idx ) );
      token->setTechnology( form.getDbTech( db_idx ) );
      // Append ClassId
      token->setClassID(    form.getObjClassId(obj_idx) );
      token->setOid( Token::OID_t( form.getObjOid1(obj_idx), oid2) );
      // StoreGate
      trans->m_key = form.getObjKey( obj_idx );
      trans->m_alias = form.getObjAlias( obj_idx );
      trans->m_pClid = form.getObjType( obj_idx );
      trans->m_clids = form.getObjSymLinks( obj_idx );
      trans->m_hashes = form.getObjHashes( obj_idx );
   }
}

//______________________________________________________________________________
DataHeader* DataHeaderCnv_p6::createTransient(const DataHeader_p6* pers, const DataHeaderForm_p6& form)
{
   DataHeader* trans = new DataHeader();
   const unsigned int provSize = pers->m_provenanceSize;
   trans->m_inputDataHeader.resize(provSize);
   trans->m_dataHeader.resize(pers->m_shortElements.size() - provSize - 1); // Take into account self reference
   unsigned i = 0;
   for( auto& elem : trans->m_dataHeader ) {
      persToElem( pers, i++, &elem, form );
   }
   for( auto& elem : trans->m_inputDataHeader ) {
      persToElem( pers, i++, &elem, form );
   }
   trans->m_dataHeader.resize(pers->m_shortElements.size() - provSize); // Add self reference, which was appended to end
   auto& elem = trans->m_dataHeader.back();
   persToElem( pers, i++, &elem, form );
   trans->setStatus(DataHeader::Input);
   return trans;
}


//______________________________________________________________________________
void DataHeaderCnv_p6::elemToPers(const DataHeaderElement* trans,
                                  DataHeader_p6* pers,
                                  DataHeaderForm_p6& form)
{
   // Translate PoolToken
   const Token *token =  trans->getToken();
   if( !token ) {
      // store marker for NO Token
      pers->m_shortElements.push_back( INT32_MIN );
   } else {
      // Database GUID & Technology
      DataHeaderForm_p6::DbRecord  db_rec( token->dbID(), token->technology() );
      unsigned db_idx = form.insertDb( db_rec );
      // StoreGate Type/Key & persistent Class GUID
      DataHeaderForm_p6::ObjRecord transObj( token->classID(), token->contID(), trans->m_key,
                                             trans->m_pClid, token->oid().first );
      unsigned obj_idx = form.insertObj(transObj, trans->m_alias, m_SGAliasFiltering,
                                        trans->m_clids, trans->m_hashes);
      unsigned long long oid2 = token->oid().second;

      // first element sets the common DB
      if( pers->m_shortElements.empty() ) {
         // first element - set the common DB and OID2 values
         pers->m_commonDbIndex = db_idx;
         pers->m_commonOID2 = oid2;
      }
      if( db_idx == pers->m_commonDbIndex && oid2 == pers->m_commonOID2 ) {
         // Can use short DH element
         pers->m_shortElements.push_back( obj_idx );
      } else {
         // need to use full DH element
         // store the index (as negative) to the full element in the short vector
         pers->m_shortElements.push_back( -1 - pers->m_fullElements.size() );
         pers->m_fullElements.push_back( FullElement(oid2, db_idx, obj_idx) );
      }
   }
}

//______________________________________________________________________________
DataHeader_p6* DataHeaderCnv_p6::createPersistent(const DataHeader* trans, DataHeaderForm_p6& form)
{
   DataHeader_p6* pers = new DataHeader_p6();
   const unsigned int provSize = trans->m_inputDataHeader.size();
   pers->m_provenanceSize = provSize;

   pers->m_shortElements.reserve( provSize + trans->m_dataHeader.size() );
   form.resize(provSize + trans->m_dataHeader.size() + 1);
   for( const auto& transElem: trans->m_dataHeader ) {
      elemToPers( &transElem, pers, form );
   }
   for( const auto& transElem: trans->m_inputDataHeader ) {
      elemToPers( &transElem, pers, form );
   }
   return pers;
}

//______________________________________________________________________________
void DataHeaderCnv_p6::insertDHRef( DataHeader_p6* pers,
                                    const std::string& key, const std::string& tokstr,
                                    DataHeaderForm_p6& form )
{
   Token* token = new Token();
   token->fromString( tokstr );
   DataHeaderElement tEle(ClassID_traits<DataHeader>::ID(), key, token);
   elemToPers( &tEle, pers, form );
}
