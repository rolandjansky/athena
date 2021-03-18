/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/** @file DataHeaderCnv.cxx
 *  @brief This file contains the implementation for the DataHeaderCnv class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "DataHeaderCnv.h"

#include "PersistentDataModel/Placement.h"
#include "PersistentDataModel/Token.h"
#include "PersistentDataModel/TokenAddress.h"

#include "PersistentDataModelTPCnv/DataHeaderCnv_p3.h"
#include "PersistentDataModelTPCnv/DataHeaderCnv_p4.h"

#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/FileIncident.h"
#include "AthenaBaseComps/AthCheckMacros.h"

#include <stdexcept>

// cppcheck-suppress uninitMemberVar
DataHeaderCnv::DataHeaderCnv(ISvcLocator* svcloc) :
      DataHeaderCnvBase(svcloc, "DataHeaderCnv")
{
   m_classDesc = RootType( "DataHeader_p6" );
}

//______________________________________________________________________________
DataHeaderCnv::~DataHeaderCnv()
{
   // Remove itself from the IncidentSvc - if it is still around
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", "DataHeaderCnv");
   if( incSvc.retrieve().isSuccess() ) {
      incSvc->removeListener(this, IncidentType::EndInputFile);
   }
}
//______________________________________________________________________________
StatusCode DataHeaderCnv::initialize()
{
   // listen to EndInputFile incidents to clear old DataHeaderForms from the cache
   //Get IncidentSvc
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", "DataHeaderCnv");
   ATH_CHECK( incSvc.retrieve() );
   incSvc->addListener(this, IncidentType::EndInputFile, 0);
   return DataHeaderCnvBase::initialize();
}


void DataHeaderCnv::handle(const Incident& incident)
{
   if( incident.type() == IncidentType::EndInputFile ) {
      // remove cached DHForms that came from the file that is now being closed
      const std::string& guid = static_cast<const FileIncident&>(incident).fileGuid(); 
      auto iter = m_persFormMap.begin();
      while( iter != m_persFormMap.end() ) {
         size_t dbpos = iter->first.find("[DB=");
         if( dbpos != std::string::npos && iter->first.substr(dbpos+4, dbpos+36) == guid ) {
            iter = m_persFormMap.erase( iter );
         } else {
            iter++;
         }
      }
   }
}

//______________________________________________________________________________
/* DH is received by the SharedWriter first and needs the DHForm object both for
   reference (Token) and to add DHElem with the ref to itself.
   These updates will be handled by updateRepRefs() - cache the DH info so it can
   by used later by updateRepRefs
   pObject contains pointer to DataHeader_p6
   pAddress: par[0] contains DH token, par[1] contains placement + DH processingTag as KEY
*/
StatusCode DataHeaderCnv::updateRep(IOpaqueAddress* pAddress, DataObject* pObject)
{
   if( m_sharedWriterCachedDH ) {
      ATH_MSG_ERROR( "updateRep called but the previous DataHeader was not yet processed."
                     << " cached DH Key=" << m_sharedWriterCachedDHKey
                     << " cached DH Ref=" << m_sharedWriterCachedDHToken );
      return StatusCode::FAILURE;
   }
   // remember this DH and finish processing in updateRepRefs()
   m_sharedWriterCachedDH = reinterpret_cast<DataHeader_p6*>( pObject );
   m_sharedWriterCachedDHToken = pAddress->par()[0];
   std::size_t tagBeg = pAddress->par()[1].find("[KEY=") + 5;
   std::size_t tagSize = pAddress->par()[1].find("]", tagBeg) - tagBeg;
   m_sharedWriterCachedDHKey = pAddress->par()[1].substr( tagBeg, tagSize );
   return StatusCode::SUCCESS;
}

//______________________________________________________________________________
/* Attach a DHForm to the previous DataHeader (in SharedWriter server mode)
   Finish writing of the DataHeader by attaching the DHForm to it and by adding
   the self reference. DHForm is passed as pObject and is cached until a new one
   arrives. This method is called for each event after the DH is received.
   pObject is null if there is no new DHForm for this event - in this case the old
   one is used
 */
StatusCode DataHeaderCnv::updateRepRefs(IOpaqueAddress* pAddress, DataObject* pObject)
{
   static const pool::Guid dhf_p6_guid("7BE56CEF-C866-4BEE-9348-A5F34B5F1DAD");
   std::string dhid = pAddress->par()[1];
   if( pAddress && pObject ) {
      this->setToken( pAddress->par()[0] );
      if( !compareClassGuid( dhf_p6_guid ) ) {
         ATH_MSG_ERROR( "updateRepRefs called without DataHeaderForm" );
         return StatusCode::FAILURE;
      }
      // replace the old DHForm
      // will keep this DHForm  until a new one arrives 
      m_sharedWriterCachedDHForm[dhid].reset( reinterpret_cast<DataHeaderForm_p6*>( pObject ) );
      m_sharedWriterCachedDHForm[dhid]->setToken( pAddress->par()[0] );
   }

   if( m_sharedWriterCachedDHForm.find(dhid) == m_sharedWriterCachedDHForm.end() ) {
      ATH_MSG_ERROR( "updateRepRefs: missing DataHeaderForm for DH ID=" << dhid );
      return StatusCode::FAILURE;
   }
   if( !m_sharedWriterCachedDH ) {
      ATH_MSG_ERROR( "updateRepRefs: missing DataHeader" );
      return StatusCode::FAILURE;
   }
   // update the cached DataHeader (can be done until a commit is called)
   m_sharedWriterCachedDH->setDhFormToken( m_sharedWriterCachedDHForm[dhid]->getToken() );
   m_tpOutConverter.insertDHRef( m_sharedWriterCachedDH, m_sharedWriterCachedDHKey,
                                 m_sharedWriterCachedDHToken, *m_sharedWriterCachedDHForm[dhid] );

   // this DataHeader object is now fully processed, so forget it
   m_sharedWriterCachedDH = nullptr;
   return(StatusCode::SUCCESS);
}

   
//______________________________________________________________________________
StatusCode DataHeaderCnv::DataObjectToPool(IOpaqueAddress* pAddr, DataObject* pObj)
{
   DataHeader* obj = nullptr;
   if (!SG::fromStorable(pObj, obj) || obj == nullptr) {
      ATH_MSG_ERROR( "Failed to cast DataHeader to transient type" );
      return(StatusCode::FAILURE);
   }
   // DH placement first:
   Placement dh_placement = setPlacementWithType("DataHeader", pObj->name(), *pAddr->par());
   dh_placement.setAuxString("[KEY=" + obj->getProcessTag() + "]");

   // DHForm placement:
   Placement dhf_placement = setPlacementWithType("DataHeaderForm", pObj->name(), *pAddr->par());
   std::string form_placement_str = dhf_placement.toString();
   // Find or create Form
   std::unique_ptr<DataHeaderForm_p6>& dhForm = m_persFormMap[form_placement_str];
   if (dhForm == nullptr) {
      // create new DHF for this file.  Every new file/container should get its own DHForm
      dhForm = std::make_unique<DataHeaderForm_p6>();
   }

   // Create persistent DH and update Form
   DataHeader_p6* persObj = nullptr;
   try {
      persObj = createPersistent(obj, dhForm.get());
   } catch (std::exception &e) {
      ATH_MSG_FATAL("Failed to convert DataHeader to persistent type: " << e.what());
      return(StatusCode::FAILURE);
   }
   // Set the Ref to the Form, if know (may be updated if a new Form is created below)
   persObj->setDhFormToken( dhForm->getToken() );

   // Queue DH for write - for local writes object can still be updated
   Token* dh_token = m_athenaPoolCnvSvc->registerForWrite(&dh_placement, persObj, m_classDesc);
   if (dh_token == nullptr) {
      ATH_MSG_FATAL("Failed to write DataHeader");
      return(StatusCode::FAILURE);
   }
   keepPoolObj(persObj, dh_placement.fileName());
   // this updates DH and can update Form
   m_tpOutConverter.insertDHRef(persObj, obj->getProcessTag(), dh_token->toString(), *dhForm);

   // Queue Form for write if it was modified (or new)
   if (dhForm->wasModified()) {
      m_wroteDHForm = true;
      static const RootType dhFormType(typeid(*dhForm));
      Token* dhf_token = m_athenaPoolCnvSvc->registerForWrite(&dhf_placement, dhForm.get(), dhFormType);
      if (dhf_token  == nullptr) {
         ATH_MSG_FATAL("Failed to write " << dhFormType.Name());
         return(StatusCode::FAILURE);
      }
      dhForm->setToken(dhf_token->toString());
      dhf_token->release(); dhf_token = nullptr;
      // Update DH with the new Form Ref
      persObj->setDhFormToken(dhForm->getToken());
      dhForm->clearModified();
      ATH_MSG_DEBUG("wrote new DHForm with " << dhForm->sizeObj() << " SG object data");
   } else {
      m_wroteDHForm = false;
   }

   const coral::AttributeList* list = obj->getAttributeList();
   if (list != nullptr) {
      obj->setEvtRefTokenStr(dh_token->toString());
      Placement attr_placement = this->setPlacementWithType("AttributeList", "Token", *pAddr->par());
      const Token* ref_token = m_athenaPoolCnvSvc->registerForWrite(&attr_placement,
	      obj->getEvtRefTokenStr().c_str(),
	      RootType("Token"));
      delete ref_token; ref_token = nullptr;
      for (coral::AttributeList::const_iterator iter = list->begin(), last = list->end(); iter != last; ++iter) {
         attr_placement = this->setPlacementWithType("AttributeList", (*iter).specification().name(), *pAddr->par());
         const Token* attr_token = m_athenaPoolCnvSvc->registerForWrite(&attr_placement,
	         (*iter).addressOfData(),
                 RootType((*iter).specification().type()) );
         delete attr_token; attr_token = nullptr;
      }
   }
   TokenAddress* tokAddr = dynamic_cast<TokenAddress*>(pAddr);
   if (tokAddr != nullptr) {
      tokAddr->setToken(dh_token); dh_token = nullptr;
   } else {
      delete dh_token; dh_token = nullptr;
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}

//______________________________________________________________________________
std::unique_ptr<DataHeader_p5> DataHeaderCnv::poolReadObject_p5()
{
   void* voidPtr1 = nullptr;
   m_athenaPoolCnvSvc->setObjPtr(voidPtr1, m_i_poolToken);
   if (voidPtr1 == nullptr) {
      throw std::runtime_error("Could not get object for token = " + m_i_poolToken->toString());
   }
   std::unique_ptr<DataHeader_p5> header( reinterpret_cast<DataHeader_p5*>(voidPtr1) );

   void* voidPtr2 = nullptr;
   Token mapToken;
   mapToken.fromString( header->dhFormToken() );
   mapToken.setAuxString( m_i_poolToken->auxString() );  // set PersSvc context
   if (mapToken.classID() != Guid::null()) {
      if( header->dhFormMdx() != m_dhFormMdx ) {
         m_athenaPoolCnvSvc->setObjPtr(voidPtr2, &mapToken);
         if (voidPtr2 == nullptr) {
            throw std::runtime_error("Could not get object for token = " + mapToken.toString());
         }
         m_dhInForm5.reset( reinterpret_cast<DataHeaderForm_p5*>(voidPtr2) );
         m_dhFormMdx = header->dhFormMdx();
      }
      header->setDhForm( m_dhInForm5.get() );
   }
   return header;
}

//______________________________________________________________________________
// Read the persistent rep of DataHeader
// Also read DataHeaderForm if not yet cached
std::unique_ptr<DataHeader_p6> DataHeaderCnv::poolReadObject_p6()
{
   void* voidPtr1 = nullptr;
   m_athenaPoolCnvSvc->setObjPtr(voidPtr1, m_i_poolToken);
   if (voidPtr1 == nullptr) {
      throw std::runtime_error("Could not get object for token = " + m_i_poolToken->toString());
   }
   std::unique_ptr<DataHeader_p6> header( reinterpret_cast<DataHeader_p6*>(voidPtr1) );
      
   // see if the DataHeaderForm is already cached
   std::unique_ptr<DataHeaderForm_p6>& dh_form = m_persFormMap[ header->dhFormToken() ];
   if( !dh_form ) {
      // we need to read a new DHF
      void* voidPtr2 = nullptr;
      Token mapToken;
      mapToken.fromString( header->dhFormToken() );
      mapToken.setAuxString( m_i_poolToken->auxString() );  // set PersSvc context
      if (mapToken.classID() != Guid::null()) {
         m_athenaPoolCnvSvc->setObjPtr(voidPtr2, &mapToken);
         if (voidPtr2 == nullptr) {
            throw std::runtime_error("Could not get object for token = " + mapToken.toString());
         }
      }
      dh_form.reset( reinterpret_cast<DataHeaderForm_p6*>(voidPtr2) );
   }
   return header;
}

//______________________________________________________________________________
DataHeader_p6* DataHeaderCnv::createPersistent(DataHeader* transObj, DataHeaderForm_p6* dh_form)
{
   return m_tpOutConverter.createPersistent( transObj, *dh_form );
}

//______________________________________________________________________________
DataHeader* DataHeaderCnv::createTransient() {
   if (this->m_i_poolToken == nullptr) {
      return(nullptr);
   }
   if (this->m_i_poolToken->technology() == 0x00001000) { // Artificial ByteStream DataHeader Token
      DataHeader* dh = new DataHeader();
      std::string bestPfn, fileType;
      m_athenaPoolCnvSvc->getPoolSvc()->lookupBestPfn(this->m_i_poolToken->dbID().toString(), bestPfn, fileType);
      const Token* copy = new Token(this->m_i_poolToken);
      DataHeaderElement dhe(ClassID_traits<DataHeader>::ID(), bestPfn, copy);
      dh->insert(dhe);
      return(dh);
   }
   static const pool::Guid p6_guid("4DDBD295-EFCE-472A-9EC8-15CD35A9EB8D");
   static const pool::Guid p5_guid("D82968A1-CF91-4320-B2DD-E0F739CBC7E6");
   static const pool::Guid p4_guid("9630EB7B-CCD7-47D9-A39B-CBBF4133CDF2");
   static const pool::Guid p3_guid("EC1318F0-8E28-45F8-9A2D-2597C1CC87A6");
   try {
      if( compareClassGuid( p6_guid ) ) {
         std::unique_ptr<DataHeader_p6> header( poolReadObject_p6() );
         auto dh = m_tpInConverter.createTransient( header.get(), *(m_persFormMap[ header->dhFormToken() ]) );
         // To dump the DataHeader uncomment below
         // std::ostringstream ss;  dh->dump(ss); cout << ss.str() << endl;
         return dh;
      } else if (this->compareClassGuid( p5_guid )) {
         std::unique_ptr<DataHeader_p5> obj_p5( poolReadObject_p5() );
         return m_tpInConverter_p5.createTransient( obj_p5.get() );
      } else if (this->compareClassGuid( p4_guid )) {
         std::unique_ptr<DataHeader_p4> obj_p4(this->poolReadObject<DataHeader_p4>());
         DataHeaderCnv_p4 tPconverter_p4;
         return(tPconverter_p4.createTransient(obj_p4.get()));
      } else if (this->compareClassGuid( p3_guid )) {
         std::unique_ptr<DataHeader_p3> obj_p3(this->poolReadObject<DataHeader_p3>());
         DataHeaderCnv_p3 tPconverter_p3;
         return(tPconverter_p3.createTransient(obj_p3.get()));
      }
   } catch (std::exception &e) {
      // upgrade all exceptions to runtime_error (who needs that? MN)
      std::string error = e.what();
      throw std::runtime_error(error);
   }
   return(nullptr);
}
