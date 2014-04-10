///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INav4MomAssocsCnv_p3.cxx 
// Implementation file for class INav4MomAssocsCnv_p3
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// NavFourMom includes
#define private public
#define protected public
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomAssocs.h"
#undef protected
#undef private

// EventCommonTPCnv includes
#include "EventCommonTPCnv/INav4MomAssocsCnv_p3.h"

// some useful typedefs
typedef INav4MomAssocs::object_link INav4MomLink_t;

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

#define LOG_MSG(log,lvl,msg) \
   log << lvl; \
   if( log.level() <= lvl ) log << lvl << msg << endreq

void 
INav4MomAssocsCnv_p3::persToTrans( const INav4MomAssocs_p3* pers, 
                                   INav4MomAssocs* trans, 
                                   MsgStream& msg )
{
   LOG_MSG(msg, MSG::DEBUG, "Loading INav4MomAssocs from persistent state..." );

  // retrieve underlying association stores
  trans->m_assocStores.clear();
  for ( INav4MomAssocs_p3::INav4MomStores_t::const_iterator 
          i = pers->m_assocStores.begin(),
          iEnd = pers->m_assocStores.end();
        i != iEnd;
        ++i ) {
    IAssocStoresCnv_t::DLink_t	dlink;
    m_assocStoresCnv.persToTrans( &*i, &dlink, msg );
    trans->m_assocStores[ dlink.dataID() ] = dlink;
  }

  trans->m_associationMap.clear();
  for ( std::size_t i = 0, iEnd = pers->m_assocs.size(); i != iEnd; ++i ) {
    const ElementLinkInt_p3& key = pers->m_assocs[i].first;
    const INav4MomAssocs_p3::ElemLinkVect_t& val = pers->m_assocs[i].second;
    
    INav4MomLink_t k;
    m_inav4MomLinkCnv.persToTrans( &key, &k, msg );
    trans->m_associationMap[k] = INav4MomAssocs::asso_store();
    trans->m_associationMap[k].reserve( val.size() );

    for ( std::size_t j = 0, jEnd = val.size(); j != jEnd; ++j ) {
      INav4MomLink_t assocLink;
      m_inav4MomLinkCnv.persToTrans( &val[j], &assocLink, msg );
      trans->m_associationMap[k].push_back( assocLink );
    }
  }

  LOG_MSG(msg, MSG::DEBUG, "Loaded INav4MomAssocs from persistent state [OK]" );
  return;
}


void 
INav4MomAssocsCnv_p3::transToPers( const INav4MomAssocs* trans, 
                                   INav4MomAssocs_p3* pers, 
                                   MsgStream& msg )
{
  LOG_MSG(msg, MSG::DEBUG, "Creating persistent state of INav4MomAssocs...");
  
  pers->m_assocStores.resize( trans->m_assocStores.size() );
  std::size_t j = 0;
  // retrieve underlying association stores
  for ( std::map<std::string, 
                 INav4MomAssocs::INav4MomAssocsLink_t>::const_iterator 
          i = trans->m_assocStores.begin(),
          iEnd = trans->m_assocStores.end();
        i != iEnd;
        ++i,++j ) {
    m_assocStoresCnv.transToPers( &i->second, 
                                  &pers->m_assocStores[j], msg );
  }
  
  pers->m_assocs.resize( trans->m_associationMap.size() );
  const INav4MomAssocs::store_type& assocMap = trans->m_associationMap;
  j = 0;
  for ( INav4MomAssocs::store_type::const_iterator itr = assocMap.begin();
        itr != assocMap.end();
        ++itr,++j ) {
    const INav4MomLink_t& key = itr->first;
    const INav4MomAssocs::asso_store& assocs = itr->second;
    const std::size_t iMax = assocs.size();
    INav4MomAssocs_p3::AssocElem_t& persAssoc = pers->m_assocs[j];
    
    persAssoc.second.resize( iMax );
    m_inav4MomLinkCnv.transToPers( &key, &persAssoc.first, msg );
    
    
    for ( unsigned int i = 0; i != iMax; ++i ) {
      const INav4MomLink_t assLink( assocs[i].key(), // assocs.elementDataID(i),
                                    assocs.elementIndex(i) );
      
      m_inav4MomLinkCnv.transToPers( &assLink, &persAssoc.second[i], msg );
      
    }//> loop over associations
  }//> loop over object links
  
  LOG_MSG(msg, MSG::DEBUG, "Created persistent state of INav4MomAssocs [OK]"); 
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
