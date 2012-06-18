///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egammaCnv_p1.cxx 
// Implementation file for class egammaCnv_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// egammaEvent includes
#define private public
#define protected public
#include "egammaEvent/egamma.h"
#undef private
#undef protected

#include "GaudiKernel/GaudiException.h"

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4ImplEEtaPhiMCnv_p1.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleBaseCnv_p1.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaCnv_p1.h"

typedef ElementLinkCnv_p1<ElementLink<CaloClusterContainer> > ClusterLinkCnv_t;
typedef ElementLinkCnv_p1<ElementLink<Rec::TrackParticleContainer> > TrackLinkCnv_t;
typedef ElementLinkCnv_p1<ElementLink<VxContainer> > ConversionLinkCnv_t;
typedef ElementLinkVectorCnv_p1<ElementLinkVector<egDetailContainer> > EgDetailsCnv_t;

// pre-allocate converters
static P4ImplEEtaPhiMCnv_p1 momCnv;
static ParticleBaseCnv_p1 partBaseCnv;
static ClusterLinkCnv_t   clusterCnv;
static TrackLinkCnv_t     trackCnv;
static ConversionLinkCnv_t     conversionCnv;
static EgDetailsCnv_t     egDetailsCnv;

// some constants used to keep consistency over business of egPID conversion
// --hack-hack-hack-- (ugly-ugly)

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

void egammaCnv_p1::persToTrans( const egamma_p1* pers,
				egamma* trans, 
				MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading egamma from persistent state..."
//       << endreq;

  trans->m_author = pers->m_author;

  momCnv.persToTrans (&pers->m_momentum,
		      &trans->momentumBase(), 
		      msg);
  partBaseCnv.persToTrans (&pers->m_particleBase, 
			   &trans->particleBase(), 
			   msg);
    
  // element links
  clusterCnv.persToTrans( &pers->m_cluster, &trans->m_cluster, msg );

  ElementLink<Rec::TrackParticleContainer> el;
  trackCnv.persToTrans( &pers->m_trackParticle,    
		        &el,
			msg );
  trans->m_trackParticle.push_back(el);   

  ElementLink<VxContainer> em;
  conversionCnv.persToTrans( &pers->m_conversion,    
			     &em,
			     msg );
  trans->m_conversion.push_back(em);

  egDetailsCnv.persToTrans( &pers->m_egDetails, 
			    &trans->m_egDetails, 
			    msg );

  // egPID filling (doubles)
  {
    const std::vector<double>& egID = pers->m_egammaDblPIDs;
    const std::vector<unsigned int>& egEnum = pers->m_egammaEnumPIDs;

    // if you crash here, then there is a data corruption...
    assert( egEnum.size() == egID.size() );

    for ( unsigned int i=0; i< egID.size(); i++ ) {
      trans->set_egammaID( (egammaPID::PID) egEnum[i],      egID[i] );
    }
  }
  //Clearing Rings ElementLink:
  trans->resetRings(); 

//   msg << MSG::DEBUG << "Loaded egamma from persistent state [OK]"
//       << endreq;
  return;
}

void egammaCnv_p1::transToPers( const egamma* /*trans*/, 
				egamma_p1* /*pers*/, 
				MsgStream& /*msg*/ ) 
{

  throw GaudiException("Deprecated method, we have evolved to use egamma_p2.Please migrate to use egammaCnv_p2::transToPers","egammaCnv_p1::transToPers", StatusCode::FAILURE);
  return;
}
