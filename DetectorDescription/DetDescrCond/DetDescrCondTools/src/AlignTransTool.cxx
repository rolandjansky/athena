/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AlignTransTool.cxx
// an AlgTool to manage the AlignableTransforms in the conditions store
// Richard Hawkings, started 25/9/03

#include <algorithm>
#include "GaudiKernel/ToolFactory.h"
#include "StoreGate/StoreGateSvc.h"
#include "Identifier/Identifier.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "DetDescrConditions/AlignableTransformContainer.h"
#include "DetDescrCondTools/AlignTransTool.h"

// magic copied from TRT_IdentifierConversionTool
static const InterfaceID 
  IID_IDETCOND_AlignTransTool("AlignTransTool",1,0);

AlignTransTool::AlignTransTool(const std::string& type,
      const std::string& name, const IInterface* parent)
      : AlgTool(type,name,parent), m_log(msgSvc(),name),
      p_condstore(0),m_lastkey("null")
{
  declareInterface<AlignTransTool>(this);
}

AlignTransTool::~AlignTransTool()
{}

const InterfaceID& AlignTransTool::interfaceID()
{ return IID_IDETCOND_AlignTransTool; }

StatusCode AlignTransTool::initialize() 
{
  m_log << MSG::INFO << "AlignTransTool initialize method called" << endreq;
  // get storegate access to conditions store
  if (StatusCode::SUCCESS!=service("DetectorStore",p_condstore))
    m_log << MSG::FATAL << "Detector store not found" << endreq;

  m_keyvec.clear();

  return StatusCode::SUCCESS;
}

StatusCode AlignTransTool::finalize()
{
  m_log << MSG::INFO << "AlignTransTool finalize method called" << endreq;
  return StatusCode::SUCCESS;
}

const AlignableTransform* AlignTransTool::getptr(const std::string key) const {
  // obtain pointer to AlignableTransform object with given key, either 
  // directly or in parent collection
  // first look directly
  const AlignableTransform* pat;
  if (StatusCode::SUCCESS!=p_condstore->retrieve(pat,key)) {
    // didn't find it, try to find the corresponding collection by stripping
    // the leaf name
    std::string keyc=key.substr(0,key.find_last_of("/"));
    const AlignableTransformContainer* patc;
    pat=0;
    if (StatusCode::SUCCESS==p_condstore->retrieve(patc,keyc)) {
      // search in the container
      for (DataVector<AlignableTransform>::const_iterator dva=patc->begin();
	   dva!=patc->end();++dva) {
	if ((*dva)->tag()==key) {
	  pat=*dva;
	  break;
	}
      }
    }
  }
  return pat;
}

const HepGeom::Transform3D AlignTransTool::getTransform(const Identifier& ident, 
    const std::string& key) const
{
  m_log << MSG::DEBUG << "get transform called for key" << key << endreq;
  // retreive the object from conditions store
  const AlignableTransform* pat;
  HepGeom::Transform3D trans;
  if (!(pat=getptr(key))) {
    m_log << MSG::WARNING << "Cannot find AlignableTransform for key " << 
        key <<       " return identity" << endreq;
    return HepGeom::Transform3D();
  } else {
    if (pat==0) m_log << MSG::ERROR << 
		  "Pointer to AlignableTransform is null" << endreq;
    // if asked for same key as before, skip the sorted check
    if (key!=m_lastkey) {
      // check if the object has already been sorted - find it in keylist
      AlignTransKey obj(key);
      key_itr keyloc=find(m_keyvec.begin(),m_keyvec.end(),obj);
      // if not, put it on the end, unsorted by default
      if (keyloc==m_keyvec.end()) keyloc=m_keyvec.insert(keyloc,obj);
      if (!(keyloc->sorted())) {
	// asking to sort is not really const
        (const_cast<AlignableTransform*>(pat))->sortv();
        keyloc->set_sorted(true);
        m_log << MSG::DEBUG << "Sorted vector for key" << key << endreq;
      }
      m_lastkey=key;
    } else {
      m_log << MSG::DEBUG << "Skip sort check - same key" << m_lastkey 
	    << endreq;
    }
    // find the corresponding tranformation 
    AlignableTransform::AlignTransMem_citr it=pat->findIdent(ident);
    if (it==pat->end()) {
      m_log << MSG::WARNING << "Identifier " << ident.getString() << 
	" not found" << endreq;
    } else {
      trans=it->transform();
    }
  }
  return trans;
}

StatusCode AlignTransTool::reset(const std::string& key) {
  m_log << MSG::DEBUG << "Reset AlignTrans for key: " << key << endreq;
  // look for the object already in the keylist, if not found it will
  // be sorted by the getTransform method when it is first called
  // so only need to resort if has already been seen
  AlignTransKey obj(key);
  key_itr keyloc=find(m_keyvec.begin(),m_keyvec.end(),obj);
  if (keyloc!=m_keyvec.end()) {
    keyloc->set_sorted(true);
    const AlignableTransform* pat;
    if (!(pat=getptr(key))) {
      m_log << MSG::WARNING << "Cannot find AlignableTransform for key " << 
	key << endreq;
      return StatusCode::FAILURE;
    } else {
      // apply sort to a const ref
      (const_cast<AlignableTransform*>(pat))->sortv();
    }
  }
  return StatusCode::SUCCESS;
}

