/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file DataHeaderCnv.cxx
 *  @brief This file contains the implementation for the DataHeaderCnv class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "DataHeaderCnv.h"

#include "PersistentDataModel/Placement.h"
#include "PersistentDataModel/Token.h"
#include "PersistentDataModelTPCnv/DataHeaderCnv_p3.h"
#include "PersistentDataModelTPCnv/DataHeaderCnv_p4.h"
#include "PersistentDataModelTPCnv/DataHeader_p6.h"

#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/FileIncident.h"
#include "AthenaBaseComps/AthCheckMacros.h"

#include <memory>
#include <stdexcept>

DataHeaderCnv::DataHeaderCnv(ISvcLocator* svcloc) :
	T_AthenaPoolCustomCnv<DataHeader, DataHeader_p5>::T_AthenaPoolCustomCnv(svcloc),
	m_TPconverter(),
	m_dhFormMdx(),
	m_dhForm(0) {
}
//______________________________________________________________________________
DataHeaderCnv::~DataHeaderCnv()
{
   // Remove itself from the IncidentSvc - if it is still around
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", "DataHeaderCnv");
   if( incSvc.retrieve().isSuccess() ) {
      incSvc->removeListener(this, IncidentType::EndInputFile);
   }
   delete m_dhForm ; m_dhForm = nullptr;
   for( auto& item : m_persFormMap ) {
      delete item.second;
   } 
}
//______________________________________________________________________________
StatusCode DataHeaderCnv::initialize()
{
   // listen to EndFile incidents to clear old DataHeaderForms from the cache
   //Get IncidentSvc
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", "DataHeaderCnv");
   ATH_CHECK( incSvc.retrieve() );
   incSvc->addListener(this, IncidentType::EndInputFile, 0);
   return DataHeaderCnvBase::initialize();
}

//______________________________________________________________________________
void DataHeaderCnv::handle(const Incident& incident)
{
   if( incident.type() == IncidentType::EndInputFile ) {
      // remove cached DHForms that came from the file that is now being closed
      const std::string& guid = static_cast<const FileIncident&>(incident).fileGuid(); 
      auto iter = m_persFormMap.begin();
      while( iter != m_persFormMap.end() ) {
         size_t dbpos = iter->first.find("[DB=");
         if( dbpos != std::string::npos && iter->first.substr(dbpos+4, dbpos+36) == guid ) {
            delete iter->second;
            iter = m_persFormMap.erase( iter );
         } else {
            iter++;
         }
      }
   }
}

//______________________________________________________________________________
StatusCode DataHeaderCnv::updateRep(IOpaqueAddress* pAddress, DataObject* pObject) {
   if (!m_dhForm) {
      MsgStream log(messageService(), "DataHeaderCnv");
      log << MSG::ERROR << "updateRep called without DataHeaderForm being set" << endreq;
      return(StatusCode::FAILURE);
   }
   std::size_t clidBeg = pAddress->par()[0].find("[CLID=") + 6;
   std::size_t clidSize = pAddress->par()[0].find("]", clidBeg) - clidBeg;
   if (pAddress->par()[0].substr(clidBeg, clidSize) != "D82968A1-CF91-4320-B2DD-E0F739CBC7E6") {
      MsgStream log(messageService(), "DataHeaderCnv");
      log << MSG::ERROR << "updateRep called without DataHeader_p5" << endreq;
      return(StatusCode::FAILURE);
   }
   DataHeader_p5* dataHeader = (DataHeader_p5*)pObject;
   dataHeader->setDhForm(m_dhForm);
   m_dhForm->start();
   for (unsigned int i = 0; i < dataHeader->elements().size(); i++) {
      m_dhForm->next();
   }
   m_dhForm = nullptr;
   std::size_t tagBeg = pAddress->par()[1].find("[KEY=") + 5;
   std::size_t tagSize = pAddress->par()[1].find("]", tagBeg) - tagBeg;
   m_TPconverter.insertDHRef(dataHeader, pAddress->par()[1].substr(tagBeg, tagSize), pAddress->par()[0]);
   std::size_t tokenBeg = pAddress->par()[1].find("[FORM=[") + 6;
   std::size_t tokenSize = pAddress->par()[1].find("]]", tokenBeg) + 1 - tokenBeg;
   dataHeader->setDhFormToken(pAddress->par()[1].substr(tokenBeg, tokenSize));
   return(StatusCode::SUCCESS);
}

//______________________________________________________________________________
StatusCode DataHeaderCnv::updateRepRefs(IOpaqueAddress* pAddress, DataObject* pObject) {
   std::size_t clidBeg = pAddress->par()[0].find("[CLID=") + 6;
   std::size_t clidSize = pAddress->par()[0].find("]", clidBeg) - clidBeg;
   if (pAddress->par()[0].substr(clidBeg, clidSize) != "3397D8A3-BBE6-463C-9F8E-4B3DFD8831FE") {
      MsgStream log(messageService(), "DataHeaderCnv");
      log << MSG::ERROR << "updateRep called without DataHeaderForm_p5" << endreq;
      return(StatusCode::FAILURE);
   }
   m_dhForm = (DataHeaderForm_p5*)pObject;
   return(StatusCode::SUCCESS);
}

//______________________________________________________________________________
StatusCode DataHeaderCnv::DataObjectToPool(DataObject* pObj, const std::string& tname) {
   const std::string className = "DataHeader_p5";
   const std::string classMapName = "DataHeaderForm_p5";
   if (!m_classDesc) {
      MsgStream log(messageService(), "DataHeaderCnv");
      log << MSG::DEBUG << "Retrieve class description for class \"" << className << "\"" << endreq;
      m_classDesc = RootType(className);
   }
   if (!m_mapClassDesc) {
      MsgStream log(messageService(), "DataHeaderCnv");
      log << MSG::DEBUG << "Retrieve class description for map class \"" << classMapName << "\"" << endreq;
      m_mapClassDesc = RootType(classMapName);
   }
   DataHeader* obj = 0;
   bool success = SG::fromStorable(pObj, obj);
   if (!success || obj == 0) {
      MsgStream log(messageService(), "DataHeaderCnv");
      log << MSG::ERROR << "Failed to cast DataHeader to transient type" << endreq;
      return(StatusCode::FAILURE);
   }
   DataHeader_p5* persObj = 0;
   try {
      persObj = createPersistent(obj);
   } catch (std::exception &e) {
      MsgStream log(messageService(), "DataHeaderCnv");
      log << MSG::FATAL << "Failed to convert DataHeader to persistent type: " << e.what() << endreq;
      return(StatusCode::FAILURE);
   }
   m_persObjList.push_back(persObj);
   this->setPlacementWithType("DataHeaderForm", tname);
   const Token* dhf_token = m_athenaPoolCnvSvc->registerForWrite(m_placement, persObj->dhForm(), m_mapClassDesc);
   if (dhf_token == 0) {
      MsgStream log(messageService(), "DataHeaderCnv");
      log << MSG::FATAL << "Failed to write DataHeaderForm = " << persObj->dhForm() << endreq;
      return(StatusCode::FAILURE);
   }
   this->setPlacementWithType("DataHeader", tname);
   Placement placement;
   placement.fromString(m_placement->toString() + "[KEY=" + obj->getProcessTag() + "][FORM=" + dhf_token->toString() + "]");
   const Token* dh_token = m_athenaPoolCnvSvc->registerForWrite(&placement, persObj, m_classDesc);
   if (dh_token == 0) {
      delete dhf_token; dhf_token = 0;
      MsgStream log(messageService(), "DataHeaderCnv");
      log << MSG::FATAL << "Failed to write DataHeader" << endreq;
      return(StatusCode::FAILURE);
   }
   m_TPconverter.insertDHRef(persObj, obj->getProcessTag(), dh_token->toString());
   persObj->setDhFormToken(dhf_token->toString());
   delete dhf_token; dhf_token = 0;
   const coral::AttributeList* list = obj->getAttributeList();
   if (list != 0) {
      obj->setEvtRefTokenStr(dh_token->toString());
      this->setPlacementWithType("AttributeList", "Token");
      const Token* ref_token = m_athenaPoolCnvSvc->registerForWrite(m_placement,
	      obj->getEvtRefTokenStr().c_str(),
	      RootType("Token"));
      delete ref_token; ref_token = 0;
      for (coral::AttributeList::const_iterator iter = list->begin(), last = list->end(); iter != last; ++iter) {
         this->setPlacementWithType("AttributeList", (*iter).specification().name());
         const Token* attr_token = m_athenaPoolCnvSvc->registerForWrite(m_placement,
	         (*iter).addressOfData(),
                 RootType((*iter).specification().type()) );
         delete attr_token; attr_token = 0;
      }
   }
   this->m_o_poolToken = dh_token; // return to converter
   return(StatusCode::SUCCESS);
}
#include <sstream>
using namespace std;
//______________________________________________________________________________
DataHeader_p5* DataHeaderCnv::poolReadObject_p5() {
   DataHeader_p5* pObj = 0;
   const std::string className = "DataHeader_p5";
   const Token* token = this->m_i_poolToken;
   if (token == 0) {
      throw std::runtime_error("There is no valid token for class " + className);
   }
   void* voidPtr1 = 0;
   try {
      m_athenaPoolCnvSvc->setObjPtr(voidPtr1, token);
   } catch (std::exception &e) {
      std::string error = e.what();
      throw std::runtime_error(error);
   }
   if (voidPtr1 == 0) {
      throw std::runtime_error("Could not get object for token = " + token->toString());
   }
   pObj = reinterpret_cast<DataHeader_p5*>(voidPtr1);
   if (pObj == 0) {
      throw std::runtime_error("Failed to cast object for token = " + token->toString());
   }
   void* voidPtr2 = 0;
   Token mapToken;
   mapToken.fromString(pObj->dhFormToken());
   if (mapToken.classID() != Guid::null()) {
      if (pObj->dhFormMdx() != m_dhFormMdx) {
         try {
            m_athenaPoolCnvSvc->setObjPtr(voidPtr2, &mapToken);
         } catch (std::exception &e) {
            std::string error = e.what();
            throw std::runtime_error(error);
         }
         if (voidPtr2 == 0) {
            throw std::runtime_error("Could not get object for token = " + mapToken.toString());
         }
         delete m_dhForm ; m_dhForm = reinterpret_cast<DataHeaderForm_p5*>(voidPtr2);
         m_dhFormMdx = pObj->dhFormMdx();
         if (m_dhForm == 0) {
            throw std::runtime_error("Failed to cast object for token = " + mapToken.toString());
         }
      }
      pObj->setDhForm(m_dhForm);
   }
   return(pObj);
}

//______________________________________________________________________________
// Read the persistent rep of DataHeader_p6 and also DataHeaderForm_p6 if necessary
// Set dh_form pointer to the correct DataHeaderForm, either from file or cache
DataHeader_p6* DataHeaderCnv::poolReadObject_p6( DataHeaderForm_p6* &dh_form )
{
   void* voidPtr1 = nullptr;
   m_athenaPoolCnvSvc->setObjPtr(voidPtr1, m_i_poolToken);
   if (voidPtr1 == nullptr) {
      throw std::runtime_error("Could not get object for token = " + m_i_poolToken->toString());
   }
   DataHeader_p6* pObj = reinterpret_cast<DataHeader_p6*>(voidPtr1);
      
   // see if the DataHeaderForm is already cached
   dh_form = m_persFormMap[ pObj->dhFormToken() ];
   if( !dh_form ) {
      // we need to read a new DHF
      void* voidPtr2 = nullptr;
      Token mapToken;
      mapToken.fromString(pObj->dhFormToken());
      if (mapToken.classID() != Guid::null()) {
         m_athenaPoolCnvSvc->setObjPtr(voidPtr2, &mapToken);
         if (voidPtr2 == nullptr) {
            throw std::runtime_error("Could not get object for token = " + mapToken.toString());
         }
      }
      m_persFormMap[ pObj->dhFormToken() ] = dh_form = reinterpret_cast<DataHeaderForm_p6*>(voidPtr2);
   }
   return pObj;
}

//______________________________________________________________________________
DataHeader_p5* DataHeaderCnv::createPersistent(DataHeader* transObj) {
   DataHeader_p5* persObj = m_TPconverter.createPersistent(transObj);
   return(persObj);
}
//______________________________________________________________________________
DataHeader* DataHeaderCnv::createTransient() {
   if (this->m_i_poolToken == 0) {
      return(0);
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
   if( compareClassGuid(p6_guid) ) {
      DataHeaderForm_p6* dh_form = nullptr;
      std::auto_ptr<DataHeader_p6> obj( poolReadObject_p6( dh_form ) );
      auto dh = m_tpInConverter_p6.createTransient( obj.get(), *dh_form );
      // To dump DH:   ostringstream ss; dh->dump(ss); cout << ss.str() << endl;
      return dh;
   } else if (this->compareClassGuid(p5_guid)) {
      std::auto_ptr<DataHeader_p5> obj_p5(this->poolReadObject_p5());
      return(m_TPconverter.createTransient(obj_p5.get()));
   } else if (this->compareClassGuid(p4_guid)) {
      std::auto_ptr<DataHeader_p4> obj_p4(this->poolReadObject<DataHeader_p4>());
      DataHeaderCnv_p4 tPconverter_p4;
      return(tPconverter_p4.createTransient(obj_p4.get()));
   } else if (this->compareClassGuid(p3_guid)) {
      std::auto_ptr<DataHeader_p3> obj_p3(this->poolReadObject<DataHeader_p3>());
      DataHeaderCnv_p3 tPconverter_p3;
      return(tPconverter_p3.createTransient(obj_p3.get()));
   }
   return(0);
}
