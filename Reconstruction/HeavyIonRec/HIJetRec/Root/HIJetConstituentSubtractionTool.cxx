/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetRec/HIJetConstituentSubtractionTool.h"
#include "HIJetRec/HIJetRecDefs.h"
#include "xAODJet/JetConstituentVector.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "HIEventUtils/HIEventShapeMap.h"


//**********************************************************************

HIJetConstituentSubtractionTool::HIJetConstituentSubtractionTool(const std::string& n)
  :  JetModifierBase(n)
{
  declareProperty("EventShapeKey",m_event_shape_key);
  declareProperty("MomentName",m_moment_name=HIJetRec::subtractedJetState());
  declareProperty("SetMomentOnly",m_moment_only=true);
  declareProperty("Subtractor",m_subtractor_tool);
  declareProperty("Modulator",m_modulator_tool);
  declareProperty("ApplyOriginCorrection",m_originCorrection=false);
  declareProperty("VertexContainer",m_vertexContainer="PrimaryVertices");
}


//**********************************************************************


int HIJetConstituentSubtractionTool::modify(xAOD::JetContainer& jets) const
{
  float E_min=m_subtractor_tool->MinEnergyForMoments();
  //const jet::cellset_t & badcells = badCellMap.cells() ;

  //retrieve UE
  const xAOD::HIEventShapeContainer* shape=nullptr;
  const HIEventShapeIndex* es_index=nullptr;
  if(EventShapeKey().compare("")!=0) 
  {
    if(evtStore()->retrieve(shape,EventShapeKey()).isFailure())
    {
      ATH_MSG_ERROR("Could not retrieve input HIEventShape " << EventShapeKey() );
      return StatusCode::FAILURE;
    }
    es_index=HIEventShapeMap::getIndex(EventShapeKey());
    if(es_index==nullptr)
    {
      ATH_MSG_INFO("No HIEventShapeIndex w/ name " << EventShapeKey() << " adding it to the map");
      HIEventShapeIndex* h=new HIEventShapeIndex();
      h->setBinning(shape);
      es_index=HIEventShapeMap::insert(EventShapeKey(),*h);
    }
  }
  if(m_modulator_tool->retrieveShape().isFailure())
  {
    ATH_MSG_ERROR("Could not retrieve output shape w/ modulator tool");
    return StatusCode::FAILURE;
  }

  const xAOD::Vertex* primVertex=nullptr;
  const xAOD::VertexContainer* vertices=nullptr;
  if(m_originCorrection)
  {
    if(evtStore()->retrieve(vertices,m_vertexContainer).isFailure())
    {
      ATH_MSG_ERROR("Could not retrieve VertexContainer " << m_vertexContainer);
      return StatusCode::FAILURE;
    }
    for ( size_t iVertex = 0; iVertex < vertices->size(); ++iVertex ) 
    {
      if(vertices->at(iVertex)->vertexType() == xAOD::VxType::PriVtx) 
      {
	primVertex=vertices->at(iVertex);
	break;
      }
    }
    if(!primVertex)
    {
      ATH_MSG_WARNING("No primary vertices found, using first in container");
      primVertex=vertices->at(0);
    }
  }

  const xAOD::HIEventShape* eshape = nullptr;
  if(m_modulator_tool->getShape(eshape).isFailure())
  {
    ATH_MSG_ERROR("Could not retrieve output shape w/ modulator tool");
    return StatusCode::FAILURE;
  }
  
  bool missingMoment=false;
  bool needsUnsubMoment=false;
  if(jets.size() > 0){
     xAOD::JetFourMom_t tmp;
     needsUnsubMoment = !((*jets.begin())->getAttribute<xAOD::JetFourMom_t>(HIJetRec::unsubtractedJetState(),tmp));
  }

  //check to see if unsubtracted moment has been stored
  for ( xAOD::JetContainer::iterator ijet=jets.begin(); ijet!=jets.end(); ++ijet)
  {

    xAOD::IParticle::FourMom_t p4_cl;
    xAOD::IParticle::FourMom_t p4_subtr;
    xAOD::IParticle::FourMom_t p4_unsubtr;
    const xAOD::Vertex* origin=nullptr;
    if(m_originCorrection)
    {
      if( !(*ijet)->getAssociatedObject<xAOD::Vertex>("OriginVertex", origin) ) 
      {
	origin=primVertex;
	ATH_MSG_DEBUG("Jet has no associated vertex, using PV from container");
      }
    }

    const xAOD::JetConstituentVector constituents = (*ijet)->getConstituents();
    for (xAOD::JetConstituentVector::iterator itr = constituents.begin(); itr != constituents.end(); ++itr) 
    {
      m_subtractor_tool->Subtract(p4_cl,itr->rawConstituent(),shape,es_index,m_modulator_tool, eshape); //modifies p4_cl to be constituent 4-vector AFTER subtraction
      if(m_originCorrection)
      {
	const xAOD::CaloCluster* cl=static_cast<const xAOD::CaloCluster*>(itr->rawConstituent());
	float mag = 0;
	if(cl->isAvailable<float>("HIMag")) mag=cl->auxdataConst<float>("HIMag");
	else 
	{
	  double cm_mag=0;
	  if(cl->retrieveMoment (xAOD::CaloCluster::CENTER_MAG, cm_mag)) mag=cm_mag;
	}
	if(mag!=0.)
	{
	  float eta0=cl->eta0();
	  float phi0=cl->phi0();
	  float radius=mag/std::cosh(eta0);
	  xAOD::IParticle::FourMom_t p4_pos;
	  p4_pos.SetX(radius*std::cos(phi0)-origin->x());
	  p4_pos.SetY(radius*std::sin(phi0)-origin->y());
	  p4_pos.SetZ(radius*std::sinh(eta0)-origin->z());


	  double deta=p4_pos.Eta()-eta0;
	  double dphi=p4_pos.Phi()-phi0;
	  //adjust in case eta/phi are flipped in case of neg E clusters
	  //this method is agnostic wrt convention
	  if(p4_cl.Eta()*eta0 <0.) deta*=-1;

	  double eta_prime=p4_cl.Eta()+deta;
	  double phi_prime=p4_cl.Phi()+dphi;
	  double e_subtr=p4_cl.E();
	  p4_cl.SetPtEtaPhiE(e_subtr/std::cosh(eta_prime),eta_prime,phi_prime,e_subtr);
	}
	else missingMoment=true;
      }

      p4_subtr+=p4_cl;
      if( msgLvl(MSG::DEBUG) ) 
      {
	const xAOD::CaloCluster* cl=static_cast<const xAOD::CaloCluster*>(itr->rawConstituent());
	p4_unsubtr+=cl->p4(HIJetRec::unsubtractedClusterState());
      }
    }
    
    ATH_MSG_DEBUG("Subtracting" 
		  << std::setw(12) << "Before:"
		  << std::setw(10) << std::setprecision(3) << p4_unsubtr.Pt()*1e-3
		  << std::setw(10) << std::setprecision(3) << p4_unsubtr.Eta()
		  << std::setw(10) << std::setprecision(3) << p4_unsubtr.Phi()
		  << std::setw(10) << std::setprecision(3) << p4_unsubtr.E()*1e-3
		  << std::setw(10) << std::setprecision(3) << p4_unsubtr.M()*1e-3
		  << std::setw(12) << "After:"
		  << std::setw(10) << std::setprecision(3) << p4_subtr.Pt()*1e-3
		  << std::setw(10) << std::setprecision(3) << p4_subtr.Eta()
		  << std::setw(10) << std::setprecision(3) << p4_subtr.Phi()
		  << std::setw(10) << std::setprecision(3) << p4_subtr.E()*1e-3
		  << std::setw(10) << std::setprecision(3) << p4_subtr.M()*1e-3);



    
    xAOD::JetFourMom_t jet4vec;
    //if entire jet has negative E, do no subtraction but set to ghost scale
    //prevents cases with large cancellations with small E but pT non-trivial
    if(p4_subtr.E()/std::cosh(p4_subtr.Eta()) < E_min) 
    {
      p4_subtr=p4_unsubtr;
      p4_subtr*=1e-7;//ghost scale
    }
    jet4vec.SetCoordinates(p4_subtr.Pt(),p4_subtr.Eta(),p4_subtr.Phi(),p4_subtr.M());


    (*ijet)->setJetP4(MomentName(),jet4vec);

    xAOD::JetFourMom_t tmp;
    //if(! (*ijet)->getAttribute<xAOD::JetFourMom_t>(HIJetRec::unsubtractedJetState(),tmp) ){
    if(needsUnsubMoment)
       (*ijet)->setJetP4(HIJetRec::unsubtractedJetState(), (*ijet)->jetP4());
//    }
    if(!MomentOnly()) 
    {
      //hack for now to allow use of pp calib tool skipping pileup subtraction
      //can be skipped in future if custom HI calibration configuration file exists
      (*ijet)->setJetP4("JetPileupScaleMomentum", jet4vec );
      (*ijet)->setJetP4(xAOD::JetEMScaleMomentum, jet4vec);

      (*ijet)->setJetP4(jet4vec);
      (*ijet)->setConstituentsSignalState(HIJetRec::subtractedConstitState());
    }
  }
  if(missingMoment) ATH_MSG_WARNING("No origin correction applied, CENTERMAG missing");

  return 1;
}

