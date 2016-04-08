/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ReadDataReentrant.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief 
 */


#undef NDEBUG
#include "ReadDataReentrant.h"

#include <list>
#include <map>
#include <vector>
#include "AthExStoreGateExample/MyDataObj.h"
#include "MyContObj.h"
#include "MapStringFloat.h"
#include "StoreGateExample_ClassDEF.h" /*the CLIDs for the containers*/

#include "GaudiKernel/ISvcLocator.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "AthContainers/DataVector.h"

#include "StoreGate/SGIterator.h"
#include "StoreGate/UpdateHandle.h"
#include "StoreGate/ReadHandle.h"
#include "AthLinks/ElementLink.h"

#include "AthenaKernel/DefaultKey.h"

/////////////////////////////////////////////////////////////////////////////

ReadDataReentrant::ReadDataReentrant(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
  
  declareProperty ("DObjKey3", m_dobjKey3 = std::string("WriteDataReentrant"));
  declareProperty ("CObjKey", m_cobjKey = std::string("cobj"));
  declareProperty ("VFloatKey", m_vFloatKey = std::string("vFloat"));
  //declareProperty ("NonExistingKey", m_nonexistingKey = std::string("FunnyNonexistingKey"));
  declareProperty ("PLinkListKey", m_pLinkListKey = std::string("WriteDataReentrant"));
  declareProperty ("LinkVectorKey", m_linkVectorKey = std::string("linkvec"));
  declareProperty ("TestObjectKey", m_testObjectKey = "testobj");

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadDataReentrant::initialize(){


  ATH_MSG_INFO ("in initialize()");

  ATH_CHECK( m_dobjKey3.initialize() );
  ATH_CHECK( m_cobjKey.initialize() );
  ATH_CHECK( m_vFloatKey.initialize() );
  //ATH_CHECK( m_nonexistingKey.initialize() );
  ATH_CHECK( m_pLinkListKey.initialize() );
  ATH_CHECK( m_linkVectorKey.initialize() );
  ATH_CHECK( m_testObjectKey.initialize() );

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadDataReentrant::execute_r (const EventContext& ctx) const
{

  // An algorithm, like this one, retrieving an object from the StoreGate(SG)
  // can either ask for:
  //    i) the most recent (default) object of a given type 
  //   ii) a specific object of a given type
  //  iii) all objects of a given type
  //This example will show how to perform these three tasks


  ATH_MSG_INFO ("in execute()");

  /////////////////////////////////////////////////////////////////////
  // Part 1: retrieving individual objects from SG
  //
  //   i)Get the most recent (default) object of type MyDataObj
  // we start by creating a *const* pointer: we won't be able
  // to modify the MyDataObj we retrieve.
  //unless you know what you are doing, *always* use const pointers 
   //FIXME const MyDataObj* dobj;

  //We pass the pointer to the StoreGate that will set it to point to
  //the default MyDataObj. More precisely the handle will now contain
  //all infos necessary to access (or create) the default MyDataObj when 
  //we will use it 
  //FIXME new (tag StoreGate-02-12-08) keyless record scheme does not allow it!
  //FIXME  if (StatusCode::SUCCESS != p_SGevent->retrieve(dobj) ) {
  //FIXME    log << MSG::ERROR 
  //FIXME	<< "Could not find default MyDataObj" 
  //FIXME	<< endreq;
  //FIXME    return( StatusCode::FAILURE);
  //FIXME  }
  //FIXME  log << MSG::INFO 
  //FIXME      << "default MyDataObj Val: " << dobj->val() 
  //FIXME      << endreq;

  /////////////////////////////////////////////////////////////////////
  //   ii) Get a specific MyDataObj by providing its key 
  //       (in this case the name of the algo which recorded it)

  //this time we set a *non-const* pointer. *If* we get back a valid
  //pointer we'll be able to modify the content of the underlying transient obj
  SG::UpdateHandle<MyDataObj> dobj3 (m_dobjKey3, ctx);
  dobj3->val(4);

  SG::ReadHandle<TestDataObject> testobj (m_testObjectKey, ctx);
  if (testobj->val() != 10) std::abort();
  
#if 0
  /////////////////////////////////////////////////////////////////////
  // iii) Get all recorded instances of MyDataObj, print out their contents

  // (a SG::ConstIterator is an iterator class that we also use to defer
  //  and control the access to the persistent object)
  SG::ConstIterator<MyDataObj> iMyD;
  SG::ConstIterator<MyDataObj> eMyD;

  if (StatusCode::SUCCESS != evtStore()->retrieve(iMyD, eMyD) ) {
    ATH_MSG_ERROR ("Could not find MyDataObj list");
    return( StatusCode::FAILURE);
  }

  // while(iMyD != eMyD) {
  //   ATH_MSG_INFO ((*iMyD).val());
  //   ++iMyD;
  // }
  ATH_MSG_WARNING ("FIXME loop of ConstIterator bombs");
#endif



////////////////////////////////////////////////////////////////////
// Get the default listof MyContObj, print out its contents

  //the CLID of list<MyContObj> must be defined using the CLASSDEF
  //macros. See StoreGateExample_ClassDEF.h for a few examples
  //If no CLID is defined StoreGate assumes the object is (convertible to a)
  //DataObject. If this is not the case an error message is issued:
  //uncomment below to see how your compiler catches an undefined CLID
  //ERROR  p_SGevent->retrieve(errorH);

  SG::ReadHandle<DataVector<MyContObj> > list (m_cobjKey, ctx);
  for (const MyContObj* obj : *list) {
    float time = obj->time();
    int ID     = obj->id();
    
    ATH_MSG_INFO ("Time: " << time << "  ID: " << ID);
  }

  /////////////////////////////////////////////////////////////////////

// Get the std::vector, print out its contents

  SG::ReadHandle<std::vector<float> > pVec (m_vFloatKey, ctx);
  for (unsigned int it=0; it<pVec->size(); it++) {
    ATH_MSG_INFO ("pVec [" << it << "] = " << (*pVec)[it]);
  }

  /////////////////////////////////////////////////////////////////////
  // test if an object is in the store
  //FIXME if (p_SGevent->contains<MyDataObj>(SG::DEFAULTKEY)) {
  //FIXME     log << MSG::INFO
  //FIXME 	<<"event store contains default MyDataObj"
  //FIXME 	<<endreq;
  //FIXME   } else {
  //FIXME     log << MSG::ERROR
  //FIXME 	<<"event store claims it does not contain default MyDataObj"
  //FIXME      	<<endreq;
  //FIXME     return( StatusCode::FAILURE);
  //FIXME   }	

#if 0
  // test if an object is not in the store
  const SG::ReadHandle<MyDataObj> nonexisting (m_nonexistingKey, ctx);
  if (nonexisting.isValid()) {
    ATH_MSG_ERROR 
      ("event store claims it contains MyDataObj with FunnyNonExistingKey");
    return( StatusCode::FAILURE);
  } else {
    ATH_MSG_INFO
      ("event store does not contain MyDataObj with FunnyNonExistingKey");
  }	
#endif
  
  
  /////////////////////////////////////////////////////////////////////
  // Part 2: retrieving DataLinks

  // Get the list of links, print out its contents

  typedef ElementLink<std::vector<float> > VecElemLink;
  auto pList = SG::makeHandle (m_pLinkListKey, ctx);
  for (const VecElemLink& l : *pList) {
    ATH_MSG_INFO ("ListVecLinks::linked element " << *l);
  }

  // Get the vector of links, print out its contents
  typedef ElementLink<MapStringFloat> MapElemLink;
  SG::ReadHandle<std::vector<MapElemLink> > vectorHandle (m_linkVectorKey, ctx);
  for (const MapElemLink& l : *vectorHandle) {
    ATH_MSG_INFO 
      ("VectorMapLinks::linked element: key " << l.index()
       << " - value " << (*l) 
       << " - stored as " << l);
  }

  //try to "read back" a link
  //    istrstream istr("due 0");
  //    MapElemLink toBeRead;
  //    istr >> toBeRead;
  //    if (!toBeRead.isValid()) {
  //      log << MSG::ERROR 
  //  	<< "Could not read back MapElement" 
  //  	<< endreq;
  //      return( StatusCode::FAILURE);
  //    } else {
  //      log << MSG::INFO <<  "MapElement read back: key " << toBeRead->first
  //  	<< "  value " << toBeRead->second <<endreq;
  //    }

  /////////////////////////////////////////////////////////////////////
  // Part 3: symbolic links and derived types
  // Get all objects as its base class 

#if 0
  SG::ConstIterator<BaseClass> it2;
  SG::ConstIterator<BaseClass> iEnd2;

  if (StatusCode::SUCCESS != evtStore()->retrieve(it2, iEnd2) ) {

    ATH_MSG_ERROR ("Could not find base class list");
    return( StatusCode::FAILURE);
  }

  // not much we can do with an empty base class,
  // so print the path of its persistable companion (the DataBucket)
  ATH_MSG_INFO (" retrieve as BaseClass, print its address: ");
  while(it2 != iEnd2) {
    ATH_MSG_INFO (&*it2);
    ++it2;
  }
#endif
  
  /////////////////////////////////////////////////////////////////////
  // Part 4: Get the event header, print out event and run number

#if 0  
  int event, run;
  
  const EventInfo* evt;
  if (StatusCode::SUCCESS == evtStore()->retrieve(evt))
  {
    event = evt->event_ID()->event_number();
    run = evt->event_ID()->run_number();
    ATH_MSG_INFO (" EventInfo : " 
		  << " event: " << event 
		  << " run: " << run);
  }
  else
  {
    ATH_MSG_ERROR (" Unable to retrieve EventInfo from StoreGate ");
    return StatusCode::SUCCESS;
  }
#endif

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadDataReentrant::finalize() {

  ATH_MSG_INFO ("in finalize()");
  return StatusCode::SUCCESS;
}
