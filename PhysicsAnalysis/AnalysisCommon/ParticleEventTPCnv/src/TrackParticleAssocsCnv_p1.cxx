///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrackParticleAssocsCnv_p1.cxx 
// Implementation file for class TrackParticleAssocsCnv_p1
// Author: C.Anastopoulos
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// NavFourMom includes
#define private public
#define protected public
#include "Particle/TrackParticleContainer.h"
#include "ParticleEvent/TrackParticleAssocs.h"
#undef protected
#undef private

// EventCommonTPCnv includes
#include "ParticleEventTPCnv/TrackParticleAssocsCnv_p1.h"

// some useful typedefs
typedef TrackParticleAssocs::object_link TrackParticleLink_t;

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
TrackParticleAssocsCnv_p1::persToTrans( const TrackParticleAssocs_p1* pers, 
				   TrackParticleAssocs* trans, 
				   MsgStream& msg )
{
   LOG_MSG(msg, MSG::DEBUG, "Loading TrackParticleAssocs from persistent state..." );
  // retrieve underlying association stores
  trans->m_assocStores.clear();
  for ( TrackParticleAssocs_p1::TrackParticleStores_t::const_iterator 
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
    const TrackParticleAssocs_p1::ElemLinkVect_t& val = pers->m_assocs[i].second;

    TrackParticleLink_t k;
    m_TrackParticleLinkCnv.persToTrans( &key, &k, msg );
    //unpack only if valid
    if(k.isValid()){      
      trans->m_associationMap[k] = TrackParticleAssocs::asso_store();
      trans->m_associationMap[k].reserve( val.size() );
      for ( std::size_t j = 0, jEnd = val.size(); j != jEnd; ++j ) {
	TrackParticleLink_t assocLink;
	m_TrackParticleLinkCnv.persToTrans( &val[j], &assocLink, msg );
	trans->m_associationMap[k].push_back( assocLink );
      }
    }
  }
  LOG_MSG(msg, MSG::DEBUG, "Loaded TrackParticleAssocs from persistent state [OK]" );
  return;
}


void 
TrackParticleAssocsCnv_p1::transToPers( const TrackParticleAssocs* trans, 
				   TrackParticleAssocs_p1* pers, 
				   MsgStream& msg )
{
   LOG_MSG(msg, MSG::DEBUG, "Creating persistent state of TrackParticleAssocs...");


   pers->m_assocStores.resize( trans->m_assocStores.size() );
   std::size_t j = 0;
  // retrieve underlying association stores
  for ( std::map<std::string, 
	  TrackParticleAssocs::TrackParticleAssocsLink_t>::const_iterator 
	  i = trans->m_assocStores.begin(),
	  iEnd = trans->m_assocStores.end();
	i != iEnd;
	++i,++j ) {
    m_assocStoresCnv.transToPers( &i->second, 
				  &pers->m_assocStores[j], msg );
  }

  pers->m_assocs.resize( trans->m_associationMap.size() );
  const TrackParticleAssocs::store_type& assocMap = trans->m_associationMap;
  j = 0;
  for ( TrackParticleAssocs::store_type::const_iterator itr = assocMap.begin();
        itr != assocMap.end();
        ++itr,++j ) {
    const TrackParticleLink_t& key = itr->first;
    const TrackParticleAssocs::asso_store& assocs = itr->second;
    const std::size_t iMax = assocs.size();
    TrackParticleAssocs_p1::AssocElem_t& persAssoc = pers->m_assocs[j];

    persAssoc.second.resize( iMax );
    m_TrackParticleLinkCnv.transToPers( &key, &persAssoc.first, msg );


    for ( unsigned int i = 0; i != iMax; ++i ) {
      const TrackParticleLink_t assLink( assocs.elementDataID(i),
				    assocs.elementIndex(i) );
      
      m_TrackParticleLinkCnv.transToPers( &assLink, &persAssoc.second[i], msg );

    }//> loop over associations
  }//> loop over object links

  LOG_MSG(msg, MSG::DEBUG, "Created persistent state of TrackParticleAssocs [OK]"); 
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
