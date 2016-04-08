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

#include "GaudiKernel/GaudiException.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaCnv_p1.h"

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

  // Clear
  *trans = egamma (pers->m_author);

  m_momCnv.persToTrans (&pers->m_momentum,
		      &trans->momentumBase(), 
		      msg);
  m_partBaseCnv.persToTrans (&pers->m_particleBase, 
			   &trans->particleBase(), 
			   msg);
    
  // element links
  ElementLink<CaloClusterContainer> cluster;
  m_clusterCnv.persToTrans( &pers->m_cluster, &cluster, msg );
  trans->setClusterElementLink (cluster);

  ElementLink<Rec::TrackParticleContainer> el;
  m_trackCnv.persToTrans( &pers->m_trackParticle,    
		        &el,
			msg );
  trans->setTrackParticleElementLink(el);

  ElementLink<VxContainer> vx;
  m_conversionCnv.persToTrans( &pers->m_conversion,    
			     &vx,
			     msg );
  trans->setConversionElementLink(vx);

  ElementLinkVector<egDetailContainer> egDetails;
  m_egDetailsCnv.persToTrans( &pers->m_egDetails, 
			    &egDetails, 
			    msg );
  trans->setDetailElementLinkVector (std::move (egDetails));

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
