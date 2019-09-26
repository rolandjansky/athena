///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// INav4MomToTrackParticleAssocsCnv_p1.cxx 
// Implementation file for class INav4MomToTrackParticleAssocsCnv_p1
// Author: Karsten Koeneke
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// NavFourMom includes
#include "Particle/TrackParticleContainer.h"
#include "ParticleEvent/INav4MomToTrackParticleAssocs.h"

// EventCommonTPCnv includes
#include "ParticleEventTPCnv/INav4MomToTrackParticleAssocsCnv_p1.h"

// some useful typedefs
typedef INav4MomToTrackParticleAssocs::object_link INav4MomLink_t;
typedef INav4MomToTrackParticleAssocs::asso_link   TrackParticleLink_t;


#define LOG_MSG(log,lvl,msg) \
   log << lvl; \
   if( log.level() <= lvl ) log << lvl << msg << endmsg

void 
INav4MomToTrackParticleAssocsCnv_p1::persToTrans( const INav4MomToTrackParticleAssocs_p1* pers, 
                                                  INav4MomToTrackParticleAssocs* trans, 
                                                  MsgStream& msg ) const
{
   LOG_MSG(msg, MSG::DEBUG, "Loading INav4MomToTrackParticleAssocs from persistent state..." );

   *trans = INav4MomToTrackParticleAssocs();

  // retrieve underlying association stores
  for ( INav4MomToTrackParticleAssocs_p1::TrackParticleStores_t::const_iterator 
          i = pers->m_assocStores.begin(),
          iEnd = pers->m_assocStores.end();
        i != iEnd;
        ++i )
    {
      IAssocStoresCnv_t::DLink_t	dlink;
      m_assocStoresCnv.persToTrans( &*i, &dlink, msg );
      trans->addAssocStore (dlink);
    }

  for ( std::size_t i = 0, iEnd = pers->m_assocs.size(); i != iEnd; ++i )
    {
      const ElementLinkInt_p3& key = pers->m_assocs[i].first;
      const INav4MomToTrackParticleAssocs_p1::ElemLinkVect_t& val = pers->m_assocs[i].second;

      INav4MomLink_t k;
      m_inav4MomLinkCnv.persToTrans( &key, &k, msg );
      //unpack only if valid
      if(k.isValid())
        {      
          for ( std::size_t j = 0, jEnd = val.size(); j != jEnd; ++j )
            {
              TrackParticleLink_t assocLink;
              m_TrackParticleLinkCnv.persToTrans( &val[j], &assocLink, msg );
              trans->addAssociation (k, assocLink);
            }
        }
    }
  LOG_MSG(msg, MSG::DEBUG, "Loaded INav4MomToTrackParticleAssocs from persistent state [OK]" );
  return;
}


void 
INav4MomToTrackParticleAssocsCnv_p1::transToPers( const INav4MomToTrackParticleAssocs* trans, 
                                                  INav4MomToTrackParticleAssocs_p1* pers, 
                                                  MsgStream& msg ) const
{
  LOG_MSG(msg, MSG::DEBUG, "Creating persistent state of INav4MomToTrackParticleAssocs...");

  // retrieve underlying association stores
  std::vector<DataLink<INav4MomToTrackParticleAssocs> > assocStores = trans->getAssocStores();
  pers->m_assocStores.resize( assocStores.size() );
  std::size_t j = 0;
  for (const DataLink<INav4MomToTrackParticleAssocs>& l : assocStores) {
    m_assocStoresCnv.transToPers( &l, &pers->m_assocStores[j], msg );
    ++j;
  }

  j = 0;
  pers->m_assocs.resize( trans->size() );
  INav4MomToTrackParticleAssocs::object_iterator begObj = trans->beginObject();
  INav4MomToTrackParticleAssocs::object_iterator endObj = trans->endObject();
  for (; begObj != endObj; ++begObj)
  {
    const INav4MomLink_t& key = begObj.getObjectLink();
    INav4MomToTrackParticleAssocs_p1::AssocElem_t& persAssoc = pers->m_assocs[j];
    m_inav4MomLinkCnv.transToPers( &key, &persAssoc.first, msg );
    persAssoc.second.resize( begObj.getNumberOfAssociations() );

    INav4MomToTrackParticleAssocs::asso_iterator begAsso = begObj.getFirstAssociation();
    INav4MomToTrackParticleAssocs::asso_iterator endAsso = begObj.getLastAssociation();
    size_t i = 0;
    for (; begAsso != endAsso; ++begAsso) {
      const INav4MomToTrackParticleAssocs::asso_link asso = begAsso.getLink();
      m_TrackParticleLinkCnv.transToPers( &asso, &persAssoc.second[i], msg );
      ++i;
    }

    ++j;
  }

  LOG_MSG(msg, MSG::DEBUG, "Created persistent state of INav4MomToTrackParticleAssocs [OK]"); 
  return;
}
