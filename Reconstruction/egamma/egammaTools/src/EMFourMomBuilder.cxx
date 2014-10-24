/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EMFourMomBuilder.cxx
PACKAGE:  offline/Reconstruction/egamma/egammaTools

AUTHORS:  D. Zerwas, DDV , JM , TK, FD
CREATED:  August, 2005
CHANGES:  March 2014, xAOD migration (David di Valentino)
          May 2014, Bruno Lenzi. Simplify definitions: 
            - cluster energy
            - track eta, phi for electrons
            - vertex eta, phi for Si+Si conversions
            - cluster eta, phi for other photons
          Combination will be done with a dual-use tool later
          May 2014 Add some covariance matrix functionality
          TODO: Maybe just fill the cluster with a parametrization
          Use a different tool in the future
PURPOSE:  subAlgorithm which sets the 4-mom of the egamma object



********************************************************************/

// INCLUDE HEADER FILES:
#include "EMFourMomBuilder.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkTrackLink/ITrackLink.h"
#include "FourMom/ErrorMatrixEEtaPhiM.h"
#include "egammaInterfaces/IFourMomCombiner.h"

//NEW xAOD INCLUDES.
#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h" 
#include "xAODTracking/TrackParticleContainer.h" 
#include "xAODTracking/VertexContainer.h" 
#include "xAODTracking/TrackingPrimitives.h" 
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "EventPrimitives/EventPrimitives.h"
#include "FourMom/EigenP5Jacobiand0z0PhiThetaqOverP2d0z0PhiEtaP.h"

//  END OF HEADER FILES INCLUDE
using CLHEP::GeV;

/////////////////////////////////////////////////////////////////

const double DEFAULT_QOVERP_ERROR = 1; // this is a huge error
// this needs to be the same orsmaller than in EMErrorDetail
//const double DEFAULT_MOMENTUM = 1e10;

//  CONSTRUCTOR:
   
EMFourMomBuilder::EMFourMomBuilder(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent)
  : egammaBaseTool(type, name, parent),
    //m_tmp_dataType(ParticleDataType::Full),
    m_FourMomCombiner("FourMomCombiner")
{
  // declare interface
  declareInterface<IEMFourMomBuilder>(this);

  // The following properties are specified at run-time
  // (declared in jobOptions file)
  
  // Use E-p combination.
  declareProperty("UseCombination",
		  m_useCombination = false,
		  "Use the E-p combination");
 
  declareProperty("m_FourMomCombiner",
		  m_FourMomCombiner,
		  "Tool for performing E-p combination");		  
  
}

// =============================================================
// DESTRUCTOR:
EMFourMomBuilder::~EMFourMomBuilder()
{ 
}

// =============================================================
// INITIALIZE METHOD:  
StatusCode EMFourMomBuilder::initialize()
{
  ATH_MSG_DEBUG(" Initializing EMFourMomBuilder");
 
 return StatusCode::SUCCESS;
}


// =============================================================
StatusCode EMFourMomBuilder::execute(xAOD::Egamma* eg)
{
  return  execute(eg, 0);
}

// =============================================================
// EXECUTE METHOD :  (modified - xAOD::Egamma* is an input) 

StatusCode EMFourMomBuilder::execute(xAOD::Egamma* eg, unsigned int index)
{

  // Protection against bad pointers.
  if (!eg) return StatusCode::SUCCESS;
  
  const xAOD::CaloCluster *cluster = eg->caloCluster();
  if (!cluster) {
    ATH_MSG_WARNING("Null pointer to cluster");
    return StatusCode::SUCCESS;
  }
    
  xAOD::Electron *electron = dynamic_cast< xAOD::Electron* >(eg);
  xAOD::Photon   *photon   = electron ? 0 : dynamic_cast< xAOD::Photon* >(eg);
  bool doubleSiConversion  = ( xAOD::EgammaHelpers::conversionType(photon) == xAOD::EgammaParameters::doubleSi );

  //If we have a good TrackParticle, use its eta and phi.
  bool goodTrack(false);
  if (electron) {
    const xAOD::TrackParticle *trackParticle = electron->trackParticle();
    if (trackParticle &&  xAOD::EgammaHelpers::numberOfSiHits(trackParticle)>=4) 
      goodTrack=true;
  }
  
  StatusCode sc = StatusCode::SUCCESS;

  if (electron && goodTrack){
    sc = setFromTrkCluster(electron, index);
  }
  else if (photon && doubleSiConversion ){
    sc = setFromTrkCluster(photon);
  }
  else{
    sc = setFromCluster(eg);
  }
  return sc;
}

// ======================================================================
StatusCode EMFourMomBuilder::hltExecute(xAOD::Egamma* eg, unsigned int index)
{
  // This is for the HLT. It might change, which is why we don't just use combinationexecute directly
  return execute(eg, index);
}

// ===================================================================
StatusCode EMFourMomBuilder::setFromTrkCluster(xAOD::Electron* el, 
					       unsigned int index){
  if (m_useCombination) {
    
    //Get # of Si hits.
    uint8_t nPixel(0), nSCT(0);
    if(!el->trackParticle()->summaryValue(nPixel,xAOD::numberOfPixelHits)){
    ATH_MSG_INFO("No Pixel Hits available in xAOD");
    }

    if(!el->trackParticle()->summaryValue(nSCT,xAOD::numberOfSCTHits)){
      ATH_MSG_INFO("No SCT Hits available in xAOD");
    }

    bool hasSi (nPixel + nSCT > 4);

    //Perform E-p combination.
    StatusCode sc = m_FourMomCombiner->TrackClusterExecute(el, index, hasSi);
    
    //Put combined error matrix in EMErrorDetail.
    saveParameters(el);
  }
  else { //===========No combination  
    const xAOD::CaloCluster *cluster = el->caloCluster();
    if (!cluster) {
      ATH_MSG_WARNING("Null pointer to cluster");
      return StatusCode::SUCCESS;
    }

    const xAOD::TrackParticle *trackParticle = el->trackParticle();
    if (!trackParticle) {
      ATH_MSG_WARNING("Null pointer to Track Particle");
      return StatusCode::SUCCESS;
    }
    
    float E = cluster->e();
    float eta = 0., phi = 0. , mass=0.510998;        
    AmgMatrix(4,4) matrix;
    matrix.setZero();
    
    eta = trackParticle->eta();
    phi = trackParticle->phi();    
    
    //Trying to Fill a covariance for the case of good Track
    xAOD::ParametersCovMatrix_t covmat=trackParticle->definingParametersCovMatrix();
    EigenP5Jacobiand0z0PhiThetaqOverP2d0z0PhiEtaP J(trackParticle->phi(),
						    trackParticle->charge(),
						    trackParticle->p4().P()); 
    AmgMatrix(5,5) m;
    m.setZero();
    m = J * (covmat * J.transpose());
    
    //Rearrange the elements of the d0, z0, phi, eta, P representation to make the (E, eta, phi, M)     
    matrix(0,0) = m(4,4); //This should come from the cluster E 
    matrix(1,1) = m(3,3);
    matrix(2,2) = m(2,2);     
    matrix.fillSymmetric(0,1,m(4,3));
    matrix.fillSymmetric(0,2,m(4,2));
    matrix.fillSymmetric(1,2,m(3,2)); 
    
    //Set the four momentum.
    ATH_MSG_DEBUG("Setting P4 using E=" << E << " eta=" << eta << " phi=" << phi <<" mass" << mass);
    const double pt = sqrt(E*E - mass*mass)/cosh(eta);
    el->setP4( pt, eta, phi, mass);
    
    //Set the covariance matrix.
    el->setCovMatrix(matrix.cast<float>());
  }
  
  return StatusCode::SUCCESS;
}

StatusCode EMFourMomBuilder::setFromTrkCluster(xAOD::Photon* ph){
  
  const xAOD::CaloCluster *cluster = ph->caloCluster();
  if (!cluster) {
    ATH_MSG_WARNING("Null pointer to cluster");
    return StatusCode::SUCCESS;
  }
    
  float E = cluster->e();
  float eta = cluster->eta(), phi = cluster->phi() , mass =0.;
  
  AmgMatrix(4,4) matrix;
  matrix.setZero();
  
  Amg::Vector3D momentumAtVertex = xAOD::EgammaHelpers::momentumAtVertex(ph);
  if (momentumAtVertex.mag() > 1e-5) { // protection against p = 0
    eta = momentumAtVertex.eta();
    phi = momentumAtVertex.phi();
  }
  
  //Set the four momentum.
  ATH_MSG_DEBUG("Setting P4 using E=" << E << " eta=" << eta << " phi=" << phi <<" mass" << mass);
  ph->setP4( E/cosh(eta), eta, phi, mass);
  //Set the covariance matrix.
  ph->setCovMatrix(matrix.cast<float>()); 

  return StatusCode::SUCCESS;
}

// ===================================================================
StatusCode EMFourMomBuilder::setFromCluster(xAOD::Egamma* eg)
{
  // Protection against bad pointers.
  if (!eg) return StatusCode::SUCCESS;
  
  const xAOD::CaloCluster *cluster = eg->caloCluster();
  if (!cluster) {
    ATH_MSG_WARNING("Null pointer to cluster");
    return StatusCode::SUCCESS;
  }

  float eta  = cluster->eta(), phi = cluster->phi();
  float E    = cluster->e();
  float mass = 0.;

  AmgMatrix(4,4) matrix;
  matrix.setZero();
  
  xAOD::Electron *electron = dynamic_cast< xAOD::Electron* >(eg);
  if (electron) mass = 0.510998;

  //Set the four momentum and covariance matrix.
  eg->setP4( E/cosh(eta), eta, phi, mass);
  eg->setCovMatrix(matrix.cast<float>());

  return StatusCode::SUCCESS;
}

// =================================================================
// Fill egamma with the combined parameters + matrix.
void EMFourMomBuilder::saveParameters(xAOD::Egamma *eg) {
  saveCombinedParams(eg);
}

void EMFourMomBuilder::saveCombinedParams(xAOD::Egamma *eg) {
  double mass(0.);
  if (eg->type()==xAOD::Type::Electron){
    mass = 0.510998;
  }
  //Fill in combined parameters + matrix.
  Amg::VectorX m_combVector = m_FourMomCombiner->getCombinedVector();
  eg->setPhi(m_combVector[2]);
  eg->setEta(m_combVector[3]);
  eg->setPt(m_combVector[4] / cosh(m_combVector[3]));
  eg->setM(mass);

  //Set error matrices in egamma object.
  eg->setCovMatrix((m_FourMomCombiner->get4x4CombinedErrorMatrix()).cast<float>()); 
}
