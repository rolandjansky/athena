/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// DiphotonVertexDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// To add the diphoton vertex to the evtStore

#include "DerivationFrameworkHiggs/DiphotonVertexDecorator.h"
#include <vector>
#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "PhotonVertexSelection/IPhotonVertexSelectionTool.h"
#include "AthContainers/ConstDataVector.h"
#include "PhotonVertexSelection/IPhotonPointingTool.h"

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
  // Create and record a vertex container with a copy of the diphoton vertex
  // or the hardest vertex
  xAOD::VertexContainer* vxContainer = new xAOD::VertexContainer;
  xAOD::VertexAuxContainer* vxAuxContainer  = new xAOD::VertexAuxContainer;
  vxContainer->setStore(vxAuxContainer);   

  ATH_CHECK(evtStore()->record(vxContainer, m_diphotonVertexSGKey));
  ATH_CHECK(evtStore()->record(vxAuxContainer, m_diphotonVertexSGKey + "Aux."));

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
  bool fromDiphoton = false;
  if (ph1 and ph2)
  {
    vxResult = m_photonVertexSelectionTool->getVertex( *( vertexPhotons.asDataVector()) , m_ignoreConv);
    if(vxResult.size()) fromDiphoton = true;
  }

  // Add the hardest vertex if needed
  if (not vxResult.size())
  {
    const xAOD::VertexContainer *PV(0); 
    ATH_CHECK( evtStore()->retrieve(PV, m_primaryVertexSGKey) );
    for (const xAOD::Vertex* vx : *PV) {
      if (vx->vertexType()==xAOD::VxType::PriVtx) {
        vxResult.push_back( std::make_pair(vx, -9999.) );
        break;
      }
     }
  }

  // Decorate the vertices with the NN score
  for (const auto vxR: vxResult)
  {
    vxR.first->auxdecor<float>("vertexScore") = vxR.second;
    vxR.first->auxdecor<int>("vertexFailType") = m_photonVertexSelectionTool->getFail();
    vxR.first->auxdecor<int>("vertexCase") = m_photonVertexSelectionTool->getCase();
    
    // Make a deep copy of the first vertex (the selected one) and add to the container
    if (not vxContainer->size())
    {  
      vxContainer->push_back( new xAOD::Vertex() );
      *( vxContainer->back() ) = *(vxR.first);
      // Make it a primary vertex for the MET tool
      vxContainer->back()->setVertexType( xAOD::VxType::PriVtx );
      
      // Decorate vertex with element link to the original one,
      // links to the photons and flag that tells if it used them
      typedef ElementLink<xAOD::VertexContainer> vxlink_t;
      vxContainer->back()->auxdecor<vxlink_t>("originalVertexLink") = vxlink_t(m_primaryVertexSGKey, vxR.first->index());
      
      typedef ElementLink<xAOD::PhotonContainer> phlink_t;
      vxContainer->back()->auxdecor<phlink_t>("leadingPhotonLink") =\
        (ph1 ? phlink_t(*photons, ph1->index()) : phlink_t() );
      vxContainer->back()->auxdecor<phlink_t>("subleadingPhotonLink") =\
        (ph2 ? phlink_t(*photons, ph2->index()) : phlink_t() );
      vxContainer->back()->auxdecor<int>("fromDiphoton") = fromDiphoton;
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

