/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_RDO_ContainerCnv.h"

#include "InDetIdentifier/TRT_ID.h"

#include <memory>

#include <iostream>
#include "CreateTransientTemplate.h"

//================================================================
namespace {
  std::string shortPrint(const TRT_RDO_Container *main_input_TRT, unsigned maxprint=25) {
    std::ostringstream os;
    if(main_input_TRT) {
      for(unsigned i=0; i<maxprint; i++) {
	TRT_RDO_Container::const_iterator p = main_input_TRT->indexFind(i);
	if(p != main_input_TRT->end()) {
	  os<<" "<< (*p)->size();
	}
	else {
	  os<<" *";
	}
      }
    }
    else {
      os<<" [TRT_RDO_Container==NULL]";
    }
    return os.str();
  }
}
//================================================================
StatusCode TRT_RDO_ContainerCnv::initialize() {
   ATH_MSG_INFO("TRT_RDO_ContainerCnv::initialize()");

   StatusCode sc = TRT_RDO_ContainerCnvBase::initialize();
   if (sc.isFailure()) {
     ATH_MSG_FATAL("Cannot initialize cnv base !");
     return StatusCode::FAILURE;
   }

   // get StoreGate service. This is needed only for clients 
   // that register collections directly to the SG instead of adding 
   // them to the container.
   sc = service("StoreGateSvc", m_storeGate);
   if (sc.isFailure()) {
     ATH_MSG_FATAL("StoreGate service not found !");
     return StatusCode::FAILURE;
   }
   
   // get DetectorStore service
   StoreGateSvc *detStore(0);
     if (service("DetectorStore", detStore).isFailure()) {
     ATH_MSG_FATAL("DetectorStore service not found !");
     return StatusCode::FAILURE;
   } else {
       ATH_MSG_DEBUG("Found DetectorStore.");
   }
   
   // Get the trt helper from the detector store
   const TRT_ID* idhelper(0);
   if (detStore->retrieve(idhelper, "TRT_ID").isFailure()) {
     ATH_MSG_FATAL("Could not get TRT_ID helper !");
     return StatusCode::FAILURE;
   } else {
     ATH_MSG_DEBUG("Found the TRT_ID helper.");
   }
   
   m_converter_p0.initialize(idhelper);
   m_converter_p1.initialize(idhelper, m_storeGate);
   m_converter_TP1.initialize(idhelper);
   m_converter_TP2.initialize(idhelper);

   ATH_MSG_DEBUG("Converter initialized");

   return StatusCode::SUCCESS;
}

//================================================================
TRT_RDO_Container_PERS* TRT_RDO_ContainerCnv::createPersistent(TRT_RDO_Container* transCont) {
  ATH_MSG_DEBUG("createPersistent(): main converter. TRANS = "<<shortPrint(transCont));
  // converter_num  is a switch to determine which persistent type to use
  // 1: if concrete type private data is equivalent to InDetRawData_p1
  // 2: for cosmic/TB not implemented
  //
  unsigned int converter_num(1);  
  TRT_RDO_Container::const_iterator it_Coll     = transCont->begin();
  TRT_RDO_Container::const_iterator it_CollEnd  = transCont->end();
  // check one element of the container. The container can't be empty for this... 
  if(it_Coll != it_CollEnd) {
    while (it_Coll != it_CollEnd && (*it_Coll)->size() == 0 ) it_Coll++;
    if(it_Coll != it_CollEnd) {
      const TRT_RDORawData *test((**it_Coll)[0]);
      if(dynamic_cast<const TRT_LoLumRawData*>(test) != 0 ) {
	 ATH_MSG_DEBUG("Found container with TRT_LoLumRawData concrete type objects");
         converter_num=1;
      } else {
	 ATH_MSG_FATAL("Converter not implemented for this concrete type " );
         throw "Converter not implemented";
      }
    } else {
       ATH_MSG_WARNING("Container has only empty collections. Using TP1 converter");
    } 
  } else {
     ATH_MSG_WARNING("Empty container. Using TP1 converter");
  }
  // Switch facility depending on the concrete data type of the contained objects
  // Should do by getting the type_info of one of the contained objects
  TRT_RDO_Container_PERS *persObj(0);
  if(converter_num == 1) { 
     persObj = m_converter_PERS.createPersistent( transCont, msg() );
  } else {
     ATH_MSG_FATAL("This shouldn't happen!! ");
  } 
  ATH_MSG_DEBUG("Success");
  return persObj; 
}
    
//================================================================
TRT_RDO_Container* TRT_RDO_ContainerCnv::createTransient() {

  const pool::Guid   p0_guid("5DB01CED-F6FD-4866-A1C6-E046641E44F5"); // with TRT_LoLumRawData
  const pool::Guid   p1_guid("CFBDB7A8-C788-4EE7-A260-3C8B680234FE"); // with TRT_RDORawData
  const pool::Guid   TP1_guid("DA76970C-E019-43D2-B2F9-25660DCECD9D"); // for t/p separated version with InDetRawDataContainer_p1
  const pool::Guid   TP2_guid("7138342E-0A80-4A32-A387-2842A01C2539"); // for t/p separated version with InDetRawDataContainer_p2
  ATH_MSG_DEBUG("createTransient(): main converter");

  if( compareClassGuid(TP2_guid) ) {
    ATH_MSG_DEBUG("createTransient(): New TP version - TP2 branch");
    std::unique_ptr< InDetRawDataContainer_p2 >   col_vect( poolReadObject< InDetRawDataContainer_p2 >() );
    TRT_RDO_Container *res = m_converter_TP2.createTransient( col_vect.get(), msg() );
    ATH_MSG_DEBUG("createTransient(), TP2 branch: returns TRANS = "<<shortPrint(res));
    return res;
  }
  else if( compareClassGuid(TP1_guid) ) {
    ATH_MSG_DEBUG("createTransient(): New TP version - TP1 branch");
    std::unique_ptr< InDetRawDataContainer_p1 >   col_vect( poolReadObject< InDetRawDataContainer_p1 >() );
    TRT_RDO_Container *res = m_converter_TP1.createTransient( col_vect.get(), msg() );
    ATH_MSG_DEBUG("createTransient(), TP1 branch: returns TRANS = "<<shortPrint(res));
    return res;
  }
  else if( compareClassGuid(p1_guid) ) {
    ATH_MSG_DEBUG("createTransient(): New input file - p1 branch");
    std::unique_ptr< TRT_RDO_Container_p1 > col_vect( poolReadObject< TRT_RDO_Container_p1 >() );
    TRT_RDO_Container *res = m_converter_p1.createTransient( col_vect.get(), msg() );
    ATH_MSG_DEBUG("createTransient(), p1 branch: returns TRANS = "<<shortPrint(res));
    return res;
  }
  else if( compareClassGuid(p0_guid) ) {
    ATH_MSG_DEBUG("createTransient(): Old input file - p0 branch");
    std::unique_ptr< TRT_RDO_Container_p0 >   col_vect( poolReadObject< TRT_RDO_Container_p0 >() );
    TRT_RDO_Container *res = m_converter_p0.createTransient( col_vect.get(), msg() );
    ATH_MSG_DEBUG("createTransient(), p0 branch: returns TRANS = "<<shortPrint(res));
    return res;
  }
  throw std::runtime_error("Unsupported persistent version of TRT_RDO_Container");
}

//================================================================
