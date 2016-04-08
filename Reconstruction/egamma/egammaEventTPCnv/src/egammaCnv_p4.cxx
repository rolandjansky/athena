///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egammaCnv_p4.cxx 
// Implementation file for class egammaCnv_p4
/////////////////////////////////////////////////////////////////// 


// STL includes

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaCnv_p4.h"


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

  // Clear
  *trans = egamma (pers->m_author);

  m_momCnv.persToTrans (&pers->m_momentum,
		      &trans->momentumBase(), 
		      msg);

  egamma::momentum_type momentumCluster;
  m_momCnv.persToTrans (&pers->m_momentumCluster,
		      &momentumCluster, 
		      msg);
  trans->setUncombined4Mom (momentumCluster.hlv());

  m_partBaseCnv.persToTrans (&pers->m_particleBase, 
			   &trans->particleBase(), 
			   msg);
    
  // element links
  ElementLink<CaloClusterContainer> cluster;
  m_clusterCnv.persToTrans( &pers->m_cluster, &cluster, msg );
  trans->setClusterElementLink (cluster);

  ElementLinkVector<Rec::TrackParticleContainer> elv;
  m_trackCnv.persToTrans( &pers->m_trackParticle,    
			&elv,
			msg );
  trans->setTrackParticleElementLinkVector(elv);

  ElementLinkVector<VxContainer> vx;
  m_conversionCnv.persToTrans( &pers->m_conversion,    
			     &vx,
			     msg );
  trans->setConversionElementLinkVector(vx);

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

  m_momCnv.transToPers (&trans->momentumBase(), 
		      &pers->m_momentum,
		      msg);

  m_momCnv.transToPers (&trans->get4Mom(egamma::Uncombined),
		      &pers->m_momentumCluster, 
		      msg);

  m_partBaseCnv.transToPers (&trans->particleBase(), 
			   &pers->m_particleBase,
			   msg);

  // element links
  m_clusterCnv.transToPers( &trans->clusterElementLink(), &pers->m_cluster, msg );
  
  m_trackCnv.transToPers( &trans->trackParticleElementLinkVector(),   
			&pers->m_trackParticle,
			msg );

  m_conversionCnv.transToPers( &trans->conversionElementLinkVector(),
			     &pers->m_conversion,
			     msg );

  m_egDetailsCnv.transToPers( &trans->detailElementLinkVector(),  
			    &pers->m_egDetails,
			    msg );

  // author
  pers->m_author = trans->author();
  
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
