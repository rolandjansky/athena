/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetRec/HIJetConstituentSubtractionTool.h"
#include "HIJetRec/HIJetRecDefs.h"
#include "xAODJet/JetConstituentVector.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
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
}


//**********************************************************************


int HIJetConstituentSubtractionTool::modify(xAOD::JetContainer& jets) const
{
  float E_min=m_subtractor_tool->MinEnergyForMoments();
  //const jet::cellset_t & badcells = badCellMap.cells() ;

  //retrieve UE
  const xAOD::HIEventShapeContainer* shape=0;
  if(EventShapeKey().compare("")==0) 
  {
    ATH_MSG_INFO("No HIEventShape specified, skipping tool.");
    return 0;
  }
  if(evtStore()->retrieve(shape,EventShapeKey()).isFailure())
  {
    ATH_MSG_ERROR("Could not retrieve input HIEventShape " << EventShapeKey() );
    return 0;
  }

  const HIEventShapeIndex* es_index=HIEventShapeMap::getIndex(EventShapeKey());
  if(es_index==nullptr)
  {
    ATH_MSG_INFO("No HIEventShapeIndex w/ name " << EventShapeKey() << " adding it to the map");
    HIEventShapeIndex* h=new HIEventShapeIndex();
    h->setBinning(shape);
    es_index=HIEventShapeMap::insert(EventShapeKey(),*h);
  }

  const xAOD::HIEventShape* eshape = nullptr;
  if(m_modulator_tool->getShape(eshape).isFailure())
  {
    ATH_MSG_ERROR("Could not retrieve output shape w/ modulator tool");
    return 0;
  }
  
  //check to see if unsubtracted moment has been stored
  for ( xAOD::JetContainer::iterator ijet=jets.begin(); ijet!=jets.end(); ++ijet)
  {

    xAOD::IParticle::FourMom_t p4_cl;
    xAOD::IParticle::FourMom_t p4_subtr;
    xAOD::IParticle::FourMom_t p4_unsubtr;
    
    const xAOD::JetConstituentVector constituents = (*ijet)->getConstituents();
    for (xAOD::JetConstituentVector::iterator itr = constituents.begin(); itr != constituents.end(); ++itr) 
    {
      m_subtractor_tool->Subtract(p4_cl,itr->rawConstituent(),shape,es_index,m_modulator_tool, eshape); //modifies p4_cl to be constituent 4-vector AFTER subtraction
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
    if(p4_subtr.E()/std::cosh(p4_subtr.Eta()) < E_min) 
    {
      p4_subtr=p4_unsubtr;
      p4_subtr*=1e-7;//ghost scale
    }
    jet4vec.SetCoordinates(p4_subtr.Pt(),p4_subtr.Eta(),p4_subtr.Phi(),p4_subtr.M());


    (*ijet)->setJetP4(MomentName(),jet4vec);

    if(!MomentOnly()) 
    {
      (*ijet)->setJetP4(HIJetRec::unsubtractedJetState(), (*ijet)->jetP4());
      //hack for now to allow use of pp calib tool skipping pileup subtraction
      //can be skipped in future if custom HI calibration configuration file exists
      (*ijet)->setJetP4("JetPileupScaleMomentum", jet4vec );
      (*ijet)->setJetP4(xAOD::JetEMScaleMomentum, jet4vec);

      (*ijet)->setJetP4(jet4vec);
      (*ijet)->setConstituentsSignalState(HIJetRec::subtractedConstitState());
    }
  }

  return 1;
}

