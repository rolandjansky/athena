/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2006 

#include "TRT_RDO_ContainerCnv_p1.h"

#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetIdentifier/TRT_ID.h"
#include "MsgUtil.h"

#include <string>
#include <iostream>
#include <sstream>
#include "CreateTransientTemplate.h"


//================================================================
namespace {
#if 0
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
#endif

  //----------------------------------------------------------------
  std::string persistentTRT_ToString(const TRT_RDO_colvector& rdoV, unsigned maxprint = 20) {
    std::ostringstream os;
    os<<"TRT_RDO_colvector = ["<<rdoV.size()<<"] ";
    unsigned count = 0;
    for(TRT_RDO_colvector::const_iterator it   = rdoV.begin(); it != rdoV.end(); it++) {
      if(++count>maxprint) break;
       if(*it) {
	 os<<(*it)->size()<<" ";
       }
       else {
	 os<<"* ";
       }
    }
    return os.str();
  }
}

//================================================================
void TRT_RDO_ContainerCnv_p1::transToPers ATLAS_NOT_THREAD_SAFE (const TRT_RDO_Container* trans, TRT_RDO_Container_p1* pers, MsgStream &log) {
  // Copy objects from InDetRawDataContainer collections to simple vector

  unsigned null_count(0);

  // First try iterators on container. If no collections, then try
  // retrieving directly from SG.
  if(trans->begin() != trans->end()) {
    MSG_DEBUG(log,"[p1] using container iterators");
    for(TRT_RDO_Container::const_iterator it=trans->begin(); it != trans->end(); it++) {
      if(*it) {
	pers->push_back(const_cast<TRT_RDO_Container::IDENTIFIABLE*>(*it) );
      }
      else {
	null_count++;
      }
    }
  }
  else {
    // Access directly from SG

    // Retrieve the digits by class, not by name
    const DataHandle<TRT_RDO_Collection > it;
    const DataHandle<TRT_RDO_Collection > last;
    
    // Failure means no collections
    StatusCode sc(m_storeGate->retrieve(it, last));
    if (sc.isFailure()) {
      MSG_DEBUG(log,"[p1]: No RDO collections found !");
    }
    else {
      MSG_DEBUG(log,"[p1] using collections found in the StoreGate");

      for (; it != last; ++it) {
	TRT_RDO_Collection* RDO_Collection = const_cast<TRT_RDO_Collection*>(it.cptr());
	pers->push_back(RDO_Collection);
      }
    }
  }
  
  if(null_count) {
    log<<MSG::WARNING<<"[p1] transToPers(): got "<< null_count<<" NULLs in IdentifiableContainer"<<endmsg;
  }
  
  MSG_DEBUG(log,"[p1] transToPers(): PERS = "<<persistentTRT_ToString(*pers));
}

//================================================================
void TRT_RDO_ContainerCnv_p1::persToTrans(const TRT_RDO_Container_p1* pers, TRT_RDO_Container* trans, MsgStream &log) {

  MSG_DEBUG(log,"[p1] persToTrans(): PERS = "<<persistentTRT_ToString(*pers));

  TRT_RDO_Container_p1::const_iterator it   = pers->begin();
  TRT_RDO_Container_p1::const_iterator last = pers->end();
  for (; it != last; ++it) {
    const TRT_RDO_Container::IDENTIFIABLE *rdoColl = *it;
    IdentifierHash idHash = rdoColl->identifyHash();
    StatusCode sc = trans->addCollection( rdoColl, idHash); 
    if (sc.isFailure()) {
      log << MSG::FATAL << "[p1] persToTrans(): TRT RDOs could not be added to the container!" << endmsg;
      throw std::runtime_error("TRT_RDO_ContainerCnv_p1::persToTrans(): TRT RDOs could not be added to the container!");
    }
  }
}

//================================================================
TRT_RDO_Container* TRT_RDO_ContainerCnv_p1::createTransient(const TRT_RDO_Container_p1* persObj, MsgStream& log) {
    std::unique_ptr<TRT_RDO_Container> trans(new TRT_RDO_Container(m_trtId->straw_layer_hash_max()));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}

//================================================================
TRT_RDO_Container_p1* TRT_RDO_ContainerCnv_p1::createPersistent(const TRT_RDO_Container* transObj, MsgStream &log) {
  std::unique_ptr<TRT_RDO_Container_p1> pers(new TRT_RDO_Container_p1(SG::VIEW_ELEMENTS));
  transToPers(transObj, pers.get(), log);
  return(pers.release());
}

//================================================================
