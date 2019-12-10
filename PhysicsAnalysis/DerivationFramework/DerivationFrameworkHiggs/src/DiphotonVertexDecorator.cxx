/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// DiphotonVertexDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// To add the diphoton vertex to the evtStore

#include "DerivationFrameworkHiggs/DiphotonVertexDecorator.h"
#include <vector>
#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "PhotonVertexSelection/IPhotonVertexSelectionTool.h"
#include "AthContainers/ConstDataVector.h"
#include "PhotonVertexSelection/IPhotonPointingTool.h"

typedef ElementLink<xAOD::PhotonContainer> phlink_t;

// Constructor
DerivationFramework::DiphotonVertexDecorator::DiphotonVertexDecorator(const std::string& t,
							    const std::string& n,
							    const IInterface* p) : 
  AthAlgTool(t, n, p)
{

  declareInterface<DerivationFramework::IAugmentationTool>(this);
 
  declareProperty("PhotonVertexSelectionTool", m_photonVertexSelectionTool);
  declareProperty("PhotonPointingTool", m_photonPointingTool);
  declareProperty("PhotonContainerKey",    m_photonSGKey="Photons");
  declareProperty("DiphotonVertexContainerKey",    m_diphotonVertexSGKey="HggPrimaryVertices");
  declareProperty("PrimaryVertexContainerKey", m_primaryVertexSGKey="PrimaryVertices");
  declareProperty("RemoveCrack",           m_removeCrack = true);
  declareProperty("MaxEta",                m_maxEta = 2.37);
  declareProperty("MinimumPhotonPt",       m_minPhotonPt = 20*CLHEP::GeV);
  declareProperty("IgnoreConvPointing",    m_ignoreConv = false);

}
  
// Destructor
DerivationFramework::DiphotonVertexDecorator::~DiphotonVertexDecorator() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::DiphotonVertexDecorator::initialize()
{
  ATH_CHECK (m_photonVertexSelectionTool.retrieve() );
  ATH_CHECK (m_photonPointingTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::DiphotonVertexDecorator::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::DiphotonVertexDecorator::addBranches() const
{
  const xAOD::VertexContainer *PV(0); 
  ATH_CHECK( evtStore()->retrieve(PV, m_primaryVertexSGKey) );
  if (PV->size() && PV->at(0)) {
    ATH_MSG_DEBUG( "Default PV " << PV->at(0) << ", type = " << PV->at(0)->vertexType() << " , z = " << PV->at(0)->z()  );
  } else {
    ATH_MSG_WARNING( "No vertex in " << m_primaryVertexSGKey );
  }
  
  // Create shallow copy of the PrimaryVertices container
  std::pair< xAOD::VertexContainer*, xAOD::ShallowAuxContainer* > HggPV = xAOD::shallowCopyContainer( *PV );
  
  ATH_CHECK(evtStore()->record( HggPV.first, m_diphotonVertexSGKey ));
  ATH_CHECK(evtStore()->record( HggPV.second, m_diphotonVertexSGKey + "Aux."));


  // Select the two highest pt photons that pass a preselection
  const xAOD::PhotonContainer *photons(0);
  const xAOD::Photon *ph1 = nullptr, *ph2 = nullptr;
  ATH_CHECK(evtStore()->retrieve(photons, m_photonSGKey));

  for (const xAOD::Photon* ph: *photons)
  {
    if (!PhotonPreselect(ph)) continue;
    if (not ph1 or ph->pt() > ph1->pt()) // new leading photon
    {
      ph2 = ph1;
      ph1 = ph;
    }
    else if (not ph2 or ph->pt() > ph2->pt()) ph2 = ph; // new subleading photon
  }

  const ConstDataVector< xAOD::PhotonContainer > vertexPhotons = {ph1, ph2};

  // decorate MVA variables
  ATH_CHECK( m_photonVertexSelectionTool->decorateInputs(*( vertexPhotons.asDataVector())) );

  // Get the photon vertex if possible
  std::vector<std::pair<const xAOD::Vertex*, float> > vxResult;
  const xAOD::Vertex *newPV = nullptr;
  
  if (ph1 and ph2)
  {
    vxResult = m_photonVertexSelectionTool->getVertex( *( vertexPhotons.asDataVector()) , m_ignoreConv);
    if(vxResult.size()) {
      newPV = vxResult[0].first; //output of photon vertex selection tool must be sorted according to score
    }
  }

  // Decorate the vertices with the NN score
  ATH_MSG_DEBUG("PhotonVertexSelection returns vertex " << newPV << " " << (newPV? Form(" with z = %g", newPV->z()) : "") );
  
  if (newPV) {
    //loop over vertex container; shallow copy has the same order
    for (unsigned int iPV=0; iPV<PV->size(); iPV++) {
      auto vx = PV->at(iPV);
      auto yyvx = (HggPV.first)->at(iPV);
      //reset vertex type
      if (vx == newPV) { //is this the diphoton primary vertex returned from the tool?
	yyvx->setVertexType( xAOD::VxType::PriVtx );
      } else if ( vx->vertexType()==xAOD::VxType::PriVtx || vx->vertexType()==xAOD::VxType::PileUp ) {
	//not overriding the type of dummy vertices of type 0 (NoVtx)
	yyvx->setVertexType( xAOD::VxType::PileUp );
      }
      //decorate score
      for (const auto vxR: vxResult) {
	//find vertex in output from photonVertexSelectionTool
	if ( vx == vxR.first ) {
	  yyvx->auxdata<float>("vertexScore") = vxR.second;
	  yyvx->auxdata<int>("vertexFailType") = m_photonVertexSelectionTool->getFail();
	  yyvx->auxdata<int>("vertexCase") = m_photonVertexSelectionTool->getCase();
	  yyvx->auxdata<phlink_t>("leadingPhotonLink") = phlink_t(*photons, ph1->index());
	  yyvx->auxdata<phlink_t>("subleadingPhotonLink") = phlink_t(*photons, ph2->index());
	  break;
	}
      }
    }
  }
  else {
    //no vertex returned by photonVertexSelectionTool, decorate default PV with fit information
    xAOD::VertexContainer::iterator yyvx_itr;
    xAOD::VertexContainer::iterator yyvx_end = (HggPV.first)->end();
    for(yyvx_itr = (HggPV.first)->begin(); yyvx_itr != yyvx_end; ++yyvx_itr ) {
      if ( (*yyvx_itr)->vertexType()==xAOD::VxType::PriVtx ) {
	(*yyvx_itr)->auxdata<float>("vertexScore") = -9999;
	(*yyvx_itr)->auxdata<int>("vertexFailType") = m_photonVertexSelectionTool->getFail();
	(*yyvx_itr)->auxdata<int>("vertexCase") = m_photonVertexSelectionTool->getCase();
	(*yyvx_itr)->auxdata<phlink_t>("leadingPhotonLink") = (phlink_t()) ;
	(*yyvx_itr)->auxdata<phlink_t>("subleadingPhotonLink") = (phlink_t());
      }
    }

  }

  return StatusCode::SUCCESS;
}

bool DerivationFramework::DiphotonVertexDecorator::PhotonPreselect(const xAOD::Photon *ph) const 
{

  if (!ph) return false;

  if (!ph->isGoodOQ(34214)) return false;

  bool val(false);
  bool defined(false);

  if(ph->isAvailable<char>("DFCommonPhotonsIsEMLoose")){
    defined = true;
    val = static_cast<bool>(ph->auxdata<char>("DFCommonPhotonsIsEMLoose"));
  }
  else{
    defined = ph->passSelection(val, "Loose");
  }

  if(!defined || !val) return false;

  // veto topo-seeded clusters
  if (ph->author(xAOD::EgammaParameters::AuthorCaloTopo35)) return false;

  // Check which variable versions are best...
  const xAOD::CaloCluster *caloCluster(ph->caloCluster());
  double eta = fabs(caloCluster->etaBE(2));

  if (eta > m_maxEta) return false;
  if (m_removeCrack && 1.37 <= eta && eta <= 1.52) return false;

  if (ph->pt() < m_minPhotonPt) return false;
  
  return true;

}

