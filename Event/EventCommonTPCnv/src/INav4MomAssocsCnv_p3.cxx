///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// INav4MomAssocsCnv_p3.cxx 
// Implementation file for class INav4MomAssocsCnv_p3
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// NavFourMom includes
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomAssocs.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/INav4MomAssocsCnv_p3.h"

#include "AthenaKernel/getThinningCache.h"

// some useful typedefs
typedef INav4MomAssocs::object_link INav4MomLink_t;


#define LOG_MSG(log,lvl,msg) \
   log << lvl; \
   if( log.level() <= lvl ) log << lvl << msg << endmsg

void 
INav4MomAssocsCnv_p3::persToTrans( const INav4MomAssocs_p3* pers, 
                                   INav4MomAssocs* trans, 
                                   MsgStream& msg ) const
{
   LOG_MSG(msg, MSG::DEBUG, "Loading INav4MomAssocs from persistent state..." );

   *trans = INav4MomAssocs();

  // retrieve underlying association stores
  for ( INav4MomAssocs_p3::INav4MomStores_t::const_iterator 
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
    const INav4MomAssocs_p3::ElemLinkVect_t& val = pers->m_assocs[i].second;
    
    INav4MomLink_t k;
    m_inav4MomLinkCnv.persToTrans( &key, &k, msg );

    for ( std::size_t j = 0, jEnd = val.size(); j != jEnd; ++j ) {
      INav4MomLink_t assocLink;
      m_inav4MomLinkCnv.persToTrans( &val[j], &assocLink, msg );
      trans->addAssociation (k, assocLink);
    }
  }

  LOG_MSG(msg, MSG::DEBUG, "Loaded INav4MomAssocs from persistent state [OK]" );
  return;
}


void 
INav4MomAssocsCnv_p3::transToPers( const INav4MomAssocs* trans, 
                                   INav4MomAssocs_p3* pers, 
                                   MsgStream& msg ) const
{
  LOG_MSG(msg, MSG::DEBUG, "Creating persistent state of INav4MomAssocs...");
  
  // retrieve underlying association stores
  std::vector<DataLink<INav4MomAssocs> > assocStores = trans->getAssocStores();
  pers->m_assocStores.resize( assocStores.size() );
  std::size_t j = 0;
  for (const DataLink<INav4MomAssocs>& l : assocStores) {
    m_assocStoresCnv.transToPers( &l, &pers->m_assocStores[j], msg );
    ++j;
  }
  
  const SG::ThinningCache* cache = SG::getThinningCache();

  j = 0;
  pers->m_assocs.resize( trans->size() );
  INav4MomAssocs::object_iterator begObj = trans->beginObject();
  INav4MomAssocs::object_iterator endObj = trans->endObject();
  for (; begObj != endObj; ++begObj)
  {
    const INav4MomLink_t& key = begObj.getObjectLink();
    INav4MomAssocs_p3::AssocElem_t& persAssoc = pers->m_assocs[j];
    m_inav4MomLinkCnv.transToPers( key, persAssoc.first, cache, msg );
    persAssoc.second.resize( begObj.getNumberOfAssociations() );

    INav4MomAssocs::asso_iterator begAsso = begObj.getFirstAssociation();
    INav4MomAssocs::asso_iterator endAsso = begObj.getLastAssociation();
    size_t i = 0;
    for (; begAsso != endAsso; ++begAsso) {
      const INav4MomAssocs::asso_link asso = begAsso.getLink();
      m_inav4MomLinkCnv.transToPers( asso, persAssoc.second[i], cache, msg );
      ++i;
    }

    ++j;
  }
  
  LOG_MSG(msg, MSG::DEBUG, "Created persistent state of INav4MomAssocs [OK]"); 
  return;
}
