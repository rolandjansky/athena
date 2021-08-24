/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// DiphotonVertexDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// To add the diphoton vertex to the evtStore

#include "DerivationFrameworkHiggs/DiphotonVertexDecorator.h"
#include <vector>
#include <string>
#include "TString.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTracking/VertexContainer.h"
#include "PhotonVertexSelection/IPhotonVertexSelectionTool.h"
#include "AthContainers/ConstDataVector.h"
// For DeltaR
#include "FourMomUtils/xAODP4Helpers.h"

typedef ElementLink<xAOD::PhotonContainer> phlink_t;

// Constructor
DerivationFramework::DiphotonVertexDecorator::DiphotonVertexDecorator(const std::string& t,
							    const std::string& n,
							    const IInterface* p) : 
  AthAlgTool(t, n, p)
{

  declareInterface<DerivationFramework::IAugmentationTool>(this);
 
  declareProperty("PhotonVertexSelectionTool",  m_photonVertexSelectionTool);
  declareProperty("RemoveCrack",           m_removeCrack    = true);
  declareProperty("MaxEta",                m_maxEta         = 2.37);
  declareProperty("MinimumPhotonPt",       m_minPhotonPt    = 20*CLHEP::GeV);
  declareProperty("IgnoreConvPointing",    m_ignoreConv     = false);
  declareProperty("TCMatchMaxRat",         m_tcMatch_maxRat = 1.5    );
  declareProperty("TCMatchDeltaR",         m_tcMatch_dR     = 0.1    );

}
   
// Destructor
DerivationFramework::DiphotonVertexDecorator::~DiphotonVertexDecorator() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::DiphotonVertexDecorator::initialize()
{
  ATH_CHECK( m_photonVertexSelectionTool.retrieve() );
  ATH_CHECK( m_primaryVertexKey.initialize() );
  ATH_CHECK( m_photonKey.initialize() );
  ATH_CHECK( m_diphotonVertexKey.initialize() );
  ATH_CHECK( m_FEContainerHandleKey.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::DiphotonVertexDecorator::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::DiphotonVertexDecorator::addBranches() const
{

  SG::ReadHandle<xAOD::VertexContainer> PV (m_primaryVertexKey);

  if (PV->size() && PV->at(0)) {
    ATH_MSG_DEBUG( "Default PV " << PV->at(0) << ", type = " << PV->at(0)->vertexType() << " , z = " << PV->at(0)->z()  );
  } 
  
  // Select the two highest pt photons that pass a preselection

  SG::ReadHandle<xAOD::PhotonContainer> photons (m_photonKey);
  const xAOD::Photon *ph1 = nullptr, *ph2 = nullptr;

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

  CP::IPhotonVertexSelectionTool::FailType vertexFailType;
  CP::IPhotonVertexSelectionTool::yyVtxType yyvertexVtxType;

  ATH_CHECK( m_photonVertexSelectionTool->decorateInputs(*(vertexPhotons.asDataVector()), &vertexFailType) );

  // Get the photon vertex if possible
  std::vector<std::pair<const xAOD::Vertex*, float> > vxResult;
  const xAOD::Vertex *newPV = nullptr;

  SG::ReadHandle<xAOD::FlowElementContainer> FEHandle(m_FEContainerHandleKey);
  for(const auto& fe : *FEHandle) fe->auxdecor<char>("passOR") = true;
  
  if (ph1 and ph2)
  {
    vxResult = m_photonVertexSelectionTool->getVertex( *( vertexPhotons.asDataVector()) , m_ignoreConv, &yyvertexVtxType, &vertexFailType );
    if(vxResult.size()) {
      newPV = vxResult[0].first; //output of photon vertex selection tool must be sorted according to score
    }
    ATH_CHECK(matchFlowElement(ph1,&*FEHandle));
    ATH_CHECK(matchFlowElement(ph2,&*FEHandle));
  }

  // Decorate the vertices with the NN score
  ATH_MSG_DEBUG("PhotonVertexSelection returns vertex " << newPV << " " << (newPV? Form(" with z = %g", newPV->z()) : "") );
  // Create shallow copy of the PrimaryVertices container
  std::pair< xAOD::VertexContainer*, xAOD::ShallowAuxContainer* > HggPV = xAOD::shallowCopyContainer( *PV );
  HggPV.second->setShallowIO(false);

  SG::WriteHandle<xAOD::VertexContainer> vertexContainer(m_diphotonVertexKey);
  ATH_CHECK(vertexContainer.recordNonConst(std::unique_ptr< xAOD::VertexContainer >(HggPV.first),
                                           std::unique_ptr< xAOD::ShallowAuxContainer >(HggPV.second)));


  if (newPV) {
    //loop over vertex container; shallow copy has the same order
    for (unsigned int iPV=0; iPV<PV->size(); iPV++) {
      auto vx = PV->at(iPV);
      auto yyvx = (HggPV.first)->at(iPV);
      //reset vertex type
      if (vx == newPV) { 
        //is this the diphoton primary vertex returned from the tool?
        yyvx->setVertexType( xAOD::VxType::PriVtx );
      } else if ( vx->vertexType()==xAOD::VxType::PriVtx || vx->vertexType()==xAOD::VxType::PileUp ) {
        //not overriding the type of dummy vertices of type 0 (NoVtx)
        yyvx->setVertexType( xAOD::VxType::PileUp );
      }
      //decorate score
      for (const auto& vxR: vxResult) {
        //find vertex in output from photonVertexSelectionTool
        if ( vx == vxR.first ) {
          yyvx->auxdata<float>("vertexScore") = vxR.second;
          yyvx->auxdata<int>("vertexFailType") = vertexFailType;
          yyvx->auxdata<int>("vertexCase") = yyvertexVtxType;
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
        (*yyvx_itr)->auxdata<int>("vertexFailType") = vertexFailType;
        (*yyvx_itr)->auxdata<int>("vertexCase") = yyvertexVtxType;
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
  double eta = std::abs(caloCluster->etaBE(2));

  if (eta > m_maxEta) return false;
  if (m_removeCrack && 1.37 <= eta && eta <= 1.52) return false;

  if (ph->pt() < m_minPhotonPt) return false;
  
  return true;

}

StatusCode DerivationFramework::DiphotonVertexDecorator::matchFlowElement(const xAOD::Photon* eg,const xAOD::FlowElementContainer *feCont) const {
  const xAOD::IParticle* swclus = eg->caloCluster();

  // Preselect FEs based on proximity: dR<0.4
  std::vector<const xAOD::FlowElement*> nearbyFE;
  nearbyFE.reserve(20);
  for(const auto& fe : *feCont) {
    if(xAOD::P4Helpers::isInDeltaR(*fe, *swclus, 0.4, true)) {
      if( ( !fe->isCharged() && fe->e() > FLT_MIN )) nearbyFE.push_back(fe);
    } // DeltaR check
  } // FE loop

  double eg_cl_e = swclus->e();
  bool doSum = true;
  double sumE_fe = 0.;
  const xAOD::IParticle* bestbadmatch = 0;
  std::sort(nearbyFE.begin(),nearbyFE.end(),greaterPtFlowElement);
  for(const auto& fe : nearbyFE) {
    if(!xAOD::P4Helpers::isInDeltaR(*fe, *swclus, m_tcMatch_dR, true)) {continue;}
    // Handle neutral FEs like topoclusters
    double fe_e = fe->e();
    // skip cluster if it's above our bad match threshold or outside the matching radius
    if(fe_e>m_tcMatch_maxRat*eg_cl_e) {
      ATH_MSG_VERBOSE("Reject topocluster in sum. Ratio vs eg cluster: " << (fe_e/eg_cl_e));
      if( !bestbadmatch || (std::abs(fe_e/eg_cl_e-1.) < std::abs(bestbadmatch->e()/eg_cl_e-1.)) ) bestbadmatch = fe;
      continue;
    }

    ATH_MSG_VERBOSE("E match with new nFE: " << std::abs(sumE_fe+fe_e - eg_cl_e) / eg_cl_e);
    if( (doSum = std::abs(sumE_fe+fe_e-eg_cl_e) < std::abs(sumE_fe - eg_cl_e)) ) {
      fe->auxdecor<char>("passOR") = false;
      sumE_fe += fe_e;
    } // if we will retain the topocluster
    else {break;}
  } // loop over nearby clusters
  if(sumE_fe<FLT_MIN && bestbadmatch) {
    bestbadmatch->auxdecor<char>("passOR") = false;
  }

  return StatusCode::SUCCESS;
}
