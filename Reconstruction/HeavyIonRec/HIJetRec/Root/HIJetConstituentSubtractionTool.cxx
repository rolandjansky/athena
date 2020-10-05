/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetConstituentSubtractionTool.h"
#include "HIJetRec/HIJetRecDefs.h"
#include "xAODJet/JetConstituentVector.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "HIEventUtils/HIEventShapeMap.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

//**********************************************************************

HIJetConstituentSubtractionTool::HIJetConstituentSubtractionTool(const std::string& n)
  :  JetModifierBase(n)
{
}


StatusCode HIJetConstituentSubtractionTool::initialize()
{
   ATH_MSG_VERBOSE("HIJetConstituentSubtractionTool initialize");
   ATH_CHECK( m_eventShapeKey.initialize( !m_eventShapeKey.key().empty()) );
   return StatusCode::SUCCESS;
}

//**********************************************************************

//Fix from conflict beetween d1493284 (master) and 5af8a733 (21.0)
StatusCode HIJetConstituentSubtractionTool::modify(xAOD::JetContainer& jets) const
{
  float E_min=m_subtractorTool->minEnergyForMoments();
  //const jet::cellset_t & badcells = badCellMap.cells() ;

  //retrieve UE
  //Introduction of a read handle for the HIShapeContainer
  SG::ReadHandle<xAOD::HIEventShapeContainer>  readHandleEvtShape ( m_eventShapeKey );

  const xAOD::HIEventShapeContainer* shape=nullptr;
  const HIEventShapeIndex* es_index=nullptr;
  if(m_eventShapeKey.key().compare("") != 0)
  {
    if (!readHandleEvtShape.isValid())
    {
      ATH_MSG_ERROR("Could not retrieve input HIEventShape " << m_eventShapeKey.key() );
      return StatusCode::FAILURE;
    }
    shape = readHandleEvtShape.get();
    es_index=m_eventShapeMapTool->getIndexFromShape(shape); 
    if(es_index==nullptr)
    {
      ATH_MSG_INFO("No HIEventShapeIndex w/ name " << m_eventShapeKey.key() << " adding it to the map");
      HIEventShapeIndex* h=new HIEventShapeIndex();
      h->setBinning(shape);
      es_index=HIEventShapeMap::insert(m_eventShapeKey.key(),*h);
    }
  }

  if(m_modulatorTool->retrieveShape().isFailure())
  {
    ATH_MSG_ERROR("Could not retrieve output shape w/ modulator tool");
    return StatusCode::FAILURE;
  }

  const xAOD::HIEventShape* eshape = nullptr;
  if(m_modulatorTool->getShape(eshape).isFailure())
  {
    ATH_MSG_ERROR("Could not retrieve output shape w/ modulator tool");
    return StatusCode::FAILURE;
  }

  for ( xAOD::JetContainer::iterator ijet=jets.begin(); ijet!=jets.end(); ++ijet)
  {

    xAOD::IParticle::FourMom_t p4_cl;
    xAOD::IParticle::FourMom_t p4_subtr;
    xAOD::IParticle::FourMom_t p4_unsubtr;
    const xAOD::JetConstituentVector constituents = (*ijet)->getConstituents();
    for (xAOD::JetConstituentVector::iterator itr = constituents.begin(); itr != constituents.end(); ++itr)
    {
      m_subtractorTool->subtract(p4_cl,itr->rawConstituent(),shape,es_index,m_modulatorTool, eshape); //modifies p4_cl to be constituent 4-vector AFTER subtraction
      p4_subtr+=p4_cl;

      if( msgLvl(MSG::DEBUG) )
      {
      	const xAOD::CaloCluster* cl=static_cast<const xAOD::CaloCluster*>(itr->rawConstituent());
        //here we can still keep cl->p4 because it's taking the unsubtracted state - moreover is debug
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

    (*ijet)->setJetP4(momentName(),jet4vec);

    if(!momentOnly())
    {
      //hack for now to allow use of pp calib tool skipping pileup subtraction
      //can be skipped in future if custom HI calibration configuration file exists
      (*ijet)->setJetP4("JetPileupScaleMomentum", jet4vec );
      (*ijet)->setJetP4(xAOD::JetEMScaleMomentum, jet4vec);

      (*ijet)->setConstituentsSignalState(HIJetRec::subtractedConstitState());
    }
  }

    return StatusCode::SUCCESS;
}
