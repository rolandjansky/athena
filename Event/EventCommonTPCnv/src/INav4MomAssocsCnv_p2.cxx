///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// INav4MomAssocsCnv_p2.cxx 
// Implementation file for class INav4MomAssocsCnv_p2
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// NavFourMom includes
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomAssocs.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/INav4MomAssocsCnv_p2.h"

#include "AthenaKernel/getThinningCache.h"

// some useful typedefs
typedef INav4MomAssocs::object_link INav4MomLink_t;


void 
INav4MomAssocsCnv_p2::persToTrans( const INav4MomAssocs_p2* pers, 
				   INav4MomAssocs* trans, 
				   MsgStream& msg ) const
{
  msg << MSG::DEBUG 
      << "Loading INav4MomAssocs from persistent state..."
      << endmsg;

  *trans = INav4MomAssocs();

  // retrieve underlying association stores
  for ( INav4MomAssocs_p2::INav4MomStores_t::const_iterator 
	  i = pers->m_assocStores.begin(),
	  iEnd = pers->m_assocStores.end();
	i != iEnd;
	++i )
  {
    IAssocStoresCnv_t::DLink_t	dlink;
    m_assocStoresCnv.persToTrans( &*i, &dlink, msg );
    trans->addAssocStore (dlink);
  }

  // reset element link converters, and provide container name lookup table
  INav4MomCnv_t::State state (pers->m_contNames);

  for ( std::size_t i = 0, iEnd = pers->m_assocs.size(); i != iEnd; ++i ) {
    const ElementLinkInt_p2& key = pers->m_assocs[i].first;
    const INav4MomAssocs_p2::ElemLinkVect_t& val = pers->m_assocs[i].second;

    INav4MomLink_t k;
    m_inav4MomLinkCnv.persToTrans( state, key, k, msg );

    for ( std::size_t j = 0, jEnd = val.size(); j != jEnd; ++j ) {
      INav4MomLink_t assocLink;
      m_inav4MomLinkCnv.persToTrans( state, val[j], assocLink, msg );
      trans->addAssociation (k, assocLink);
    }
  }

  msg << MSG::DEBUG 
      << "Loaded INav4MomAssocs from persistent state [OK]"
      << endmsg;
  return;
}

void 
INav4MomAssocsCnv_p2::transToPers( const INav4MomAssocs* trans, 
				   INav4MomAssocs_p2* pers, 
				   MsgStream& msg ) const
{
  msg << MSG::DEBUG 
      << "Creating persistent state of INav4MomAssocs..."
      << endmsg;

  // retrieve underlying association stores
  std::vector<DataLink<INav4MomAssocs> > assocStores = trans->getAssocStores();
  pers->m_assocStores.resize( assocStores.size() );
  std::size_t j = 0;
  for (const DataLink<INav4MomAssocs>& l : assocStores) {
    m_assocStoresCnv.transToPers( &l, &pers->m_assocStores[j], msg );
    ++j;
  }

  // reset element link converters, and provide container name lookup table
  INav4MomCnv_t::State state (pers->m_contNames);
  const SG::ThinningCache* cache = SG::getThinningCache();

  j = 0;
  pers->m_assocs.resize( trans->size() );
  INav4MomAssocs::object_iterator begObj = trans->beginObject();
  INav4MomAssocs::object_iterator endObj = trans->endObject();
  for (; begObj != endObj; ++begObj)
  {
    const INav4MomLink_t& key = begObj.getObjectLink();
    INav4MomAssocs_p2::AssocElem_t& persAssoc = pers->m_assocs[j];
    m_inav4MomLinkCnv.transToPers( state, key, persAssoc.first, cache, msg );
    persAssoc.second.resize( begObj.getNumberOfAssociations() );

    INav4MomAssocs::asso_iterator begAsso = begObj.getFirstAssociation();
    INav4MomAssocs::asso_iterator endAsso = begObj.getLastAssociation();
    size_t i = 0;
    for (; begAsso != endAsso; ++begAsso) {
      const INav4MomAssocs::asso_link asso = begAsso.getLink();
      m_inav4MomLinkCnv.transToPers( state, asso, persAssoc.second[i], cache, msg );
      ++i;
    }

    ++j;
  }

  msg << MSG::DEBUG 
      << "Created persistent state of INav4MomAssocs [OK]"
      << endmsg;
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
