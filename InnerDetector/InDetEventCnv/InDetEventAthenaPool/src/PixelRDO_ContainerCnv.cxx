/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelRDO_ContainerCnv.h"
#include "MsgUtil.h"
#include "InDetIdentifier/PixelID.h"

#include <memory>

#include <iostream>

#include "CreateTransientTemplate.h"

//================================================================
namespace {
  std::string shortPrint(const PixelRDO_Container *main_input_Pixel, unsigned maxprint=25) {
    std::ostringstream os;
    if(main_input_Pixel) {
      for(unsigned i=0; i<maxprint; i++) {
	PixelRDO_Container::const_iterator p = main_input_Pixel->indexFind(i);
	if(p != main_input_Pixel->end()) {
	  os<<" "<< (*p)->size();
	}
	else {
	  os<<" *";
	}
      }
    }
    else {
      os<<" [PixelRDO_Container==NULL]";
    }
    return os.str();
  }
}
//================================================================
StatusCode PixelRDO_ContainerCnv::initialize() {
   StatusCode sc = PixelRDO_ContainerCnvBase::initialize();
   if (sc.isFailure()) {
     ATH_MSG_FATAL("PixelRDO_ContainerCnvBase::initialize() returned failure !");
     return StatusCode::FAILURE;
   }

   ATH_MSG_INFO("PixelRDO_ContainerCnv::initialize()");

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
   
   // Get the pixel helper from the detector store
   const PixelID* idhelper(0);
   if (detStore->retrieve(idhelper, "PixelID").isFailure()) {
     ATH_MSG_FATAL("Could not get PixelID helper !");
     return StatusCode::FAILURE;
   } else {
     ATH_MSG_DEBUG("Found the PixelID helper.");
   }
   
   m_converter_p0.initialize(idhelper);
   m_converter_TP1.initialize(idhelper);
   m_converter_TP2.initialize(idhelper);

   ATH_MSG_DEBUG("Converter initialized");

   return StatusCode::SUCCESS;
}

//================================================================
PixelRDO_Container_PERS* PixelRDO_ContainerCnv::createPersistent(PixelRDO_Container* transCont) {
  ATH_MSG_DEBUG("createPersistent(): main converter. TRANS = "<<shortPrint(transCont));

  // converter_num  is a switch to determine which persistent type to use
  // 1: if concrete type private data is equivalent to InDetRawData_p1
  // 2: for cosmic/TB not implemented
  //
  unsigned int converter_num(1);  
  PixelRDO_Container::const_iterator it_Coll     = transCont->begin();
  PixelRDO_Container::const_iterator it_CollEnd  = transCont->end();
  // check one element of the container. The container can't be empty for this... 
  if(it_Coll != it_CollEnd) {
    while (it_Coll != it_CollEnd && (*it_Coll)->size() == 0 ) it_Coll++;
    if(it_Coll != it_CollEnd) {
      const PixelRDORawData *test((**it_Coll)[0]);
      if(dynamic_cast<const Pixel1RawData*>(test) != 0 ) {
	 ATH_MSG_DEBUG("Found container with Pixel1RawData concrete type objects");
         converter_num=1;
      } else {
	 ATH_MSG_FATAL("Converter not implemented for this concrete type ");
         throw "Converter not implemented";
      }
    } else {
       ATH_MSG_DEBUG("Container has only empty collections. Using TP1 converter");
    } 
  } else {
     ATH_MSG_DEBUG("Empty container. Using TP1 converter");
  }
  // Switch facility depending on the concrete data type of the contained objects
  // Should do by getting the type_info of one of the contained objects
  PixelRDO_Container_PERS *persObj(0);
  if(converter_num == 1) { 
     persObj = m_converter_PERS.createPersistent( transCont, msg() );
  } else {
     ATH_MSG_FATAL("This shouldn't happen!! ");
  } 
  ATH_MSG_DEBUG("Success");
  return persObj; 
}
    
//================================================================
PixelRDO_Container* PixelRDO_ContainerCnv::createTransient() {

  const pool::Guid   p0_guid("7F2C09B6-0B47-4957-8BBA-EDC665A290AC"); // with Pixel1RawData
  const pool::Guid   TP1_guid("DA76970C-E019-43D2-B2F9-25660DCECD9D"); // for t/p separated version with InDetRawDataContainer_p1
  const pool::Guid   TP2_guid("7138342E-0A80-4A32-A387-2842A01C2539"); // for t/p separated version with InDetRawDataContainer_p2
  ATH_MSG_DEBUG("createTransient(): main converter");

  if( compareClassGuid(TP2_guid) ) {
    ATH_MSG_DEBUG("createTransient(): New TP version - TP2 branch");
                                                                                                                                                             
    std::unique_ptr< InDetRawDataContainer_p2 >   col_vect( poolReadObject< InDetRawDataContainer_p2 >() );
    PixelRDO_Container *res = m_converter_TP2.createTransient( col_vect.get(), msg() );
                                                                                                                                                             
    ATH_MSG_DEBUG("createTransient(), TP2 branch: returns TRANS = "<<shortPrint(res));
                                                                                                                                                             
    return res;


  }
  else if( compareClassGuid(TP1_guid) ) {
    ATH_MSG_DEBUG("createTransient(): New TP version - TP1 branch");
                                                                                                                                                             
    std::unique_ptr< InDetRawDataContainer_p1 >   col_vect( poolReadObject< InDetRawDataContainer_p1 >() );
    PixelRDO_Container *res = m_converter_TP1.createTransient( col_vect.get(), msg() );
                                                                                                                                                             
    ATH_MSG_DEBUG("createTransient(), TP1 branch: returns TRANS = "<<shortPrint(res));
                                                                                                                                                             
    return res;


  }
  else if( compareClassGuid(p0_guid) ) {
    ATH_MSG_DEBUG("createTransient(): Old input file - p0 branch");

    std::unique_ptr< PixelRDO_Container_p0 >   col_vect( poolReadObject< PixelRDO_Container_p0 >() );
    PixelRDO_Container *res = m_converter_p0.createTransient( col_vect.get(), msg() );

    ATH_MSG_DEBUG("createTransient(), p0 branch: returns TRANS = "<<shortPrint(res));

    return res;
  }
  throw std::runtime_error("Unsupported persistent version of PixelRDO_Container");
}

//================================================================
