///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TrackParticleAssocsCnv_p1.cxx 
// Implementation file for class TrackParticleAssocsCnv_p1
// Author: C.Anastopoulos
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// NavFourMom includes
#include "Particle/TrackParticleContainer.h"
#include "ParticleEvent/TrackParticleAssocs.h"

// EventCommonTPCnv includes
#include "ParticleEventTPCnv/TrackParticleAssocsCnv_p1.h"

// some useful typedefs
typedef TrackParticleAssocs::object_link TrackParticleLink_t;


#define LOG_MSG(log,lvl,msg) \
   log << lvl; \
   if( log.level() <= lvl ) log << lvl << msg << endmsg

void 
TrackParticleAssocsCnv_p1::persToTrans( const TrackParticleAssocs_p1* pers, 
				   TrackParticleAssocs* trans, 
				   MsgStream& msg ) const
{
   LOG_MSG(msg, MSG::DEBUG, "Loading TrackParticleAssocs from persistent state..." );

   *trans = TrackParticleAssocs();

  // retrieve underlying association stores
  for ( TrackParticleAssocs_p1::TrackParticleStores_t::const_iterator 
	  i = pers->m_assocStores.begin(),
	  iEnd = pers->m_assocStores.end();
	i != iEnd;
	++i ) {
     IAssocStoresCnv_t::DLink_t	dlink;
     m_assocStoresCnv.persToTrans( &*i, &dlink, msg );
     trans->addAssocStore (dlink);
  }

  for ( std::size_t i = 0, iEnd = pers->m_assocs.size(); i != iEnd; ++i ) {
    const ElementLinkInt_p3& key = pers->m_assocs[i].first;
    const TrackParticleAssocs_p1::ElemLinkVect_t& val = pers->m_assocs[i].second;

    TrackParticleLink_t k;
    m_TrackParticleLinkCnv.persToTrans( &key, &k, msg );
    //unpack only if valid
    if(k.isValid()){      
      for ( std::size_t j = 0, jEnd = val.size(); j != jEnd; ++j ) {
	TrackParticleLink_t assocLink;
	m_TrackParticleLinkCnv.persToTrans( &val[j], &assocLink, msg );
        trans->addAssociation (k, assocLink);
      }
    }
  }
  LOG_MSG(msg, MSG::DEBUG, "Loaded TrackParticleAssocs from persistent state [OK]" );
  return;
}


void 
TrackParticleAssocsCnv_p1::transToPers( const TrackParticleAssocs* trans, 
				   TrackParticleAssocs_p1* pers, 
				   MsgStream& msg ) const
{
  LOG_MSG(msg, MSG::DEBUG, "Creating persistent state of TrackParticleAssocs...");


  // retrieve underlying association stores
  std::vector<DataLink<TrackParticleAssocs> > assocStores = trans->getAssocStores();
  pers->m_assocStores.resize( assocStores.size() );
  std::size_t j = 0;
  for (const DataLink<TrackParticleAssocs>& l : assocStores) {
    m_assocStoresCnv.transToPers( &l, &pers->m_assocStores[j], msg );
    ++j;
  }

  j = 0;
  pers->m_assocs.resize( trans->size() );
  TrackParticleAssocs::object_iterator begObj = trans->beginObject();
  TrackParticleAssocs::object_iterator endObj = trans->endObject();
  for (; begObj != endObj; ++begObj)
  {
    const TrackParticleLink_t& key = begObj.getObjectLink();
    TrackParticleAssocs_p1::AssocElem_t& persAssoc = pers->m_assocs[j];
    m_TrackParticleLinkCnv.transToPers( &key, &persAssoc.first, msg );
    persAssoc.second.resize( begObj.getNumberOfAssociations() );

    TrackParticleAssocs::asso_iterator begAsso = begObj.getFirstAssociation();
    TrackParticleAssocs::asso_iterator endAsso = begObj.getLastAssociation();
    size_t i = 0;
    for (; begAsso != endAsso; ++begAsso) {
      const TrackParticleAssocs::asso_link asso = begAsso.getLink();
      m_TrackParticleLinkCnv.transToPers( &asso, &persAssoc.second[i], msg );
      ++i;
    }

    ++j;
  }

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
