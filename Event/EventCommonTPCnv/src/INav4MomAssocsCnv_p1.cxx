///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// INav4MomAssocsCnv_p1.cxx 
// Implementation file for class INav4MomAssocsCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

// NavFourMom includes
#include "NavFourMom/INav4MomAssocs.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/INav4MomAssocsCnv_p1.h"


void 
INav4MomAssocsCnv_p1::persToTrans( const INav4MomAssocs_p1* persObj, 
				   INav4MomAssocs* transObj, 
				   MsgStream &log ) const
{
  *transObj = INav4MomAssocs();

  log << MSG::DEBUG 
      << "Loading INav4MomAssocs from persistent state..."
      << endmsg;
  if ( 0 == m_storeGate ) {
    log << MSG::ERROR
	<< "NULL pointer to StoreGateSvc !!" << endmsg
	<< "Cannot do anything !!" <<endmsg;
    throw std::runtime_error("NULL pointer to StoreGateSvc !!");
  }

  // retrieve underlying association stores
  for ( std::vector<std::string>::const_iterator itr = persObj->m_assocStores.begin();
	itr != persObj->m_assocStores.end();
	++itr ) {
    DataLink<INav4MomAssocs> link( *itr );
    if ( !link.isValid() ) {
      log << MSG::WARNING
	  << "Could not build DataLink<INav4MomAssocs> from ["
	  << *itr
	  << "] !!"
	  << endmsg;
    } else {
      transObj->addAssocStore (link);
    }
  }

  std::map<std::string,const INavigable4MomentumCollection*> collections;

  for ( INav4MomAssocs_p1::ElemLinkVect_t::const_iterator asso = persObj->m_assocs.begin();
        asso != persObj->m_assocs.end();
        ++asso ) {
    
    const std::pair<std::string,unsigned int>& objLink = asso->first;
    const std::pair<std::string,unsigned int>& assLink = asso->second;

    const std::string& objKey = objLink.first;
    const std::string& assKey = assLink.first;

    log << MSG::DEBUG << "\tobjLink=" << objKey << ", " << objLink.second << endmsg;
    log << MSG::DEBUG << "\tassLink=" << assKey << ", " << assLink.second << endmsg;

    if ( collections.find(objKey) == collections.end() ) {
      const INavigable4MomentumCollection* coll = nullptr;
      collections[objKey] = 0;
      if ( m_storeGate->retrieve( coll, objKey ).isFailure() ||
           nullptr == coll ) {
        log << MSG::ERROR
            << "Could not retrieve INavigable4MomentumCollection at : "
            << objKey
            << endmsg;
        throw std::runtime_error("Could not retrieve an INavigable4MomentumCollection");
      } else {
        log << MSG::DEBUG
            << "Successfully retrieve INav4MomCollection at : "
            << objKey
            << endmsg;
      }
      collections[objKey] = coll;
    }

    if ( collections.find(assKey) == collections.end() ) {
      const INavigable4MomentumCollection* coll = nullptr;
      if ( m_storeGate->retrieve( coll, assKey ).isFailure() ||
           nullptr == coll ) {
        log << MSG::ERROR
            << "Could not retrieve INavigable4MomentumCollection at : "
            << assKey
            << endmsg;
        throw std::runtime_error("Could not retrieve an INavigable4MomentumCollection");
      } else {
        log << MSG::DEBUG
            << "Successfully retrieve INav4MomCollection at : "
            << assKey
            << endmsg;
      }
      collections[assKey] = coll;
    }

    transObj->addAssociation( collections[objKey], objLink.second,
                              collections[assKey], assLink.second );
    
  }
  
  log << MSG::DEBUG 
      << "Loaded INav4MomAssocs from persistent state [OK]"
      << endmsg;
  return;
}

void 
INav4MomAssocsCnv_p1::transToPers( const INav4MomAssocs* transObj, 
				   INav4MomAssocs_p1* persObj, 
				   MsgStream &log ) const
{
  log << MSG::DEBUG 
      << "Creating persistent state of INav4MomAssocs..."
      << endmsg;

  // first store the underlying association stores
  for (const DataLink<INav4MomAssocs>& l : transObj->getAssocStores()) {
    persObj->m_assocStores.push_back( l.dataID() );
  }

  INav4MomAssocs::object_iterator begObj = transObj->beginObject();
  INav4MomAssocs::object_iterator endObj = transObj->endObject();
  for (; begObj != endObj; ++begObj)
  {
    const INav4MomAssocs::object_link& key = begObj.getObjectLink();
    INav4MomAssocs_p1::Elem_t keyElem (key.dataID(), key.index());
    INav4MomAssocs::asso_iterator begAsso = begObj.getFirstAssociation();
    INav4MomAssocs::asso_iterator endAsso = begObj.getLastAssociation();
    for (; begAsso != endAsso; ++begAsso) {
      const INav4MomAssocs::asso_link asso = begAsso.getLink();
      INav4MomAssocs_p1::Elem_t assoElem (asso.dataID(), asso.index());
      const INav4MomAssocs_p1::ElemLink_t association( keyElem, assoElem );
      persObj->m_assocs.push_back( association );
    }
  }

  log << MSG::DEBUG 
      << "Created persistent state of INav4MomAssocs [OK]"
      << endmsg;
  return;
}
