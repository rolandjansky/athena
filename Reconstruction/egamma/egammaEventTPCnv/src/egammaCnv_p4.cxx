///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egammaCnv_p4.cxx 
// Implementation file for class egammaCnv_p4
/////////////////////////////////////////////////////////////////// 


// STL includes

// egammaEvent includes
#define private public
#define protected public
#include "egammaEvent/egamma.h"
#undef private
#undef protected

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4ImplEEtaPhiMCnv_p2.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleBaseCnv_p2.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaCnv_p4.h"

typedef ElementLinkCnv_p3<ElementLink<CaloClusterContainer> > ClusterLinkCnv_t;
typedef ElementLinkVectorCnv_p1<ElementLinkVector<Rec::TrackParticleContainer> > TrackLinkCnv_t;
typedef ElementLinkVectorCnv_p1<ElementLinkVector<VxContainer> > ConversionLinkCnv_t;
typedef ElementLinkVectorCnv_p1<ElementLinkVector<egDetailContainer> > EgDetailsCnv_t;

// pre-allocate converters
static P4ImplEEtaPhiMCnv_p2 momCnv;
static ParticleBaseCnv_p2 partBaseCnv;
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

void egammaCnv_p4::persToTrans( const egamma_p4* pers,
				egamma* trans, 
				MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading egamma from persistent state..."
//       << endreq;

  trans->m_author = pers->m_author;

  momCnv.persToTrans (&pers->m_momentum,
		      &trans->momentumBase(), 
		      msg);

  momCnv.persToTrans (&pers->m_momentumCluster,
		      &trans->m_momentumCluster, 
		      msg);

  partBaseCnv.persToTrans (&pers->m_particleBase, 
			   &trans->particleBase(), 
			   msg);
    
  // element links
  clusterCnv.persToTrans( &pers->m_cluster, &trans->m_cluster, msg );

  trackCnv.persToTrans( &pers->m_trackParticle,    
			&trans->m_trackParticle,   
			msg );

  conversionCnv.persToTrans( &pers->m_conversion,    
			     &trans->m_conversion,   
			     msg );

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

    // HACK: before rel 17 PID values of 0 were not stored. Therefore,
    // let's put 0's if OQ or IsEM are missing. (We will not worry
    // about the MVA tools)

    bool found;
    trans->isem(egammaPID::ALL, egammaPID::IsEM, &found);
    if (!found) trans->set_egammaIDint(egammaPID::IsEM, 0);

    trans->isemse(egammaPID::ALL, &found);
    if (!found) trans->set_egammaIDint(egammaPID::SofteIsEM, 0);

    trans->isgoodoq(egammaPID::ALLOQ, &found);
    if (!found) trans->set_egammaIDint(egammaPID::IsGoodOQ, 0);

  }
  //Clearing Rings ElementLink:
  trans->resetRings(); 

//   msg << MSG::DEBUG << "Loaded egamma from persistent state [OK]"
//       << endreq;
  return;
}

void egammaCnv_p4::transToPers( const egamma* trans, 
				egamma_p4* pers, 
				MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of egamma..."
//       << endreq;

  momCnv.transToPers (&trans->momentumBase(), 
		      &pers->m_momentum,
		      msg);

  momCnv.transToPers (&trans->m_momentumCluster,
		      &pers->m_momentumCluster, 
		      msg);

  partBaseCnv.transToPers (&trans->particleBase(), 
			   &pers->m_particleBase,
			   msg);

  // element links
  clusterCnv.transToPers( &trans->m_cluster, &pers->m_cluster, msg );
  
  trackCnv.transToPers( &trans->m_trackParticle,   
			&pers->m_trackParticle,
			msg );

  conversionCnv.transToPers( &trans->m_conversion,   
			     &pers->m_conversion,
			     msg );

  egDetailsCnv.transToPers( &trans->m_egDetails,  
			    &pers->m_egDetails,
			    msg );

  // author
  pers->m_author = trans->m_author;
  
  // egPID (ints)
  {
    std::vector<double>& egID = pers->m_egammaDblPIDs;
    std::vector<unsigned int>& egEnum = pers->m_egammaEnumPIDs;

    unsigned int nbOfEgammaIDs = (int) egammaPID::LastEgammaPID;

    double tmpID;
    for ( unsigned int i=0; i<nbOfEgammaIDs; i++ ) {
      tmpID = trans->egammaID( (egammaPID::PID) i);
      if ( tmpID != egammaPID::EgPidUndefined ) {
	egEnum.push_back(i);
	egID.push_back(tmpID);
      }
    } 
  }

//   msg << MSG::DEBUG << "Created persistent state of egamma [OK]"
//       << endreq;
  return;
}
