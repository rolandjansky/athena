/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CscCalibEvent/CscCalibDataIdHash.h"

/***************************************************
 CscCalibData Hash Id implementation
 This class converts a Csc Identifier into a integer, 
 according to its fields 
 Author: Ketevi A. Assamagan
***************************************************/

#include <cassert> 
#include <iostream> 

/** default contructor */ 
CscCalibDataIdHash::CscCalibDataIdHash( ) {

  IdContext cntx = m_cscHelper->module_context();
  m_context = new IdContext(cntx.prefix_id(), cntx.begin_index(), cntx.end_index());
  m_max     = m_cscHelper->module_hash_max();
}

/** get the hash id given the identifier */
int CscCalibDataIdHash::operator() (const ID& id) const {
  IdentifierHash result;
  m_cscHelper->get_hash(id, result, m_context);
  return (result);
}

/** default destructor */ 
CscCalibDataIdHash::~CscCalibDataIdHash( ) {
  delete m_context;
}
