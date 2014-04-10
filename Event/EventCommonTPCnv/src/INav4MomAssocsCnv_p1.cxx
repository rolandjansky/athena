///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#define private public
#define protected public
#include "NavFourMom/INav4MomAssocs.h"
#undef protected
#undef private

// EventCommonTPCnv includes
#include "EventCommonTPCnv/INav4MomAssocsCnv_p1.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

void 
INav4MomAssocsCnv_p1::persToTrans( const INav4MomAssocs_p1* persObj, 
				   INav4MomAssocs* transObj, 
				   MsgStream &log )
{
  log << MSG::DEBUG 
      << "Loading INav4MomAssocs from persistent state..."
      << endreq;
  if ( 0 == m_storeGate ) {
    log << MSG::ERROR
	<< "NULL pointer to StoreGateSvc !!" << endreq
	<< "Cannot do anything !!" <<endreq;
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
	  << endreq;
    } else {
      transObj->m_assocStores[*itr] = link;
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

    log << MSG::DEBUG << "\tobjLink=" << objKey << ", " << objLink.second << endreq;
    log << MSG::DEBUG << "\tassLink=" << assKey << ", " << assLink.second << endreq;

    if ( collections.find(objKey) == collections.end() ) {
      collections[objKey] = 0;
      if ( m_storeGate->retrieve( collections[objKey], objKey ).isFailure() ||
           0 == collections[objKey] ) {
        log << MSG::ERROR
            << "Could not retrieve INavigable4MomentumCollection at : "
            << objKey
            << endreq;
        throw std::runtime_error("Could not retrieve an INavigable4MomentumCollection");
      } else {
        log << MSG::DEBUG
            << "Successfully retrieve INav4MomCollection at : "
            << objKey
            << endreq;
      }
    }

    if ( collections.find(assKey) == collections.end() ) {
      collections[assKey] = 0;
      if ( m_storeGate->retrieve( collections[assKey], assKey ).isFailure() ||
           0 == collections[assKey] ) {
        log << MSG::ERROR
            << "Could not retrieve INavigable4MomentumCollection at : "
            << assKey
            << endreq;
        throw std::runtime_error("Could not retrieve an INavigable4MomentumCollection");
      } else {
        log << MSG::DEBUG
            << "Successfully retrieve INav4MomCollection at : "
            << assKey
            << endreq;
      }
    }

    transObj->addAssociation( collections[objKey], objLink.second,
                              collections[assKey], assLink.second );
    
  }
  
  log << MSG::DEBUG 
      << "Loaded INav4MomAssocs from persistent state [OK]"
      << endreq;
  return;
}

void 
INav4MomAssocsCnv_p1::transToPers( const INav4MomAssocs* transObj, 
				   INav4MomAssocs_p1* persObj, 
				   MsgStream &log )
{
  log << MSG::DEBUG 
      << "Creating persistent state of INav4MomAssocs..."
      << endreq;

  // first store the underlying association stores
  for ( std::map<std::string, DataLink<INav4MomAssocs> >::const_iterator itr = transObj->m_assocStores.begin();
	itr != transObj->m_assocStores.end();
	++itr ) {
    persObj->m_assocStores.push_back( itr->second.dataID() );
  }

  const INav4MomAssocs::store_type& assocMap = transObj->m_associationMap;
  
  for ( INav4MomAssocs::store_type::const_iterator itr = assocMap.begin();
        itr != assocMap.end();
        ++itr ) {
    const INav4MomAssocs_p1::Elem_t objLink( itr->first.dataID(), itr->first.index() );
    const INav4MomAssocs::asso_store& assocs = itr->second;

    for ( unsigned int i = 0; i != assocs.size(); ++i ) {
      const INav4MomAssocs_p1::Elem_t     assLink( assocs.elementDataID(i), assocs.elementIndex(i) );
      const INav4MomAssocs_p1::ElemLink_t association( objLink, assLink );
  
      persObj->m_assocs.push_back( association );

    }//> loop over associations
  }//> loop over object links

  log << MSG::DEBUG 
      << "Created persistent state of INav4MomAssocs [OK]"
      << endreq;
  return;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 
