/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#include "GaudiKernel/MsgStream.h"

// C++
// Stuff be here...
#include "egammaMVACalib/egammaMVATool.h"
#include "egammaMVACalib/egammaMVACalib.h"

// xAOD
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaDefs.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "PATInterfaces/CorrectionCode.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

// tracking
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackingPrimitives.h"


egammaMVATool::egammaMVATool( const std::string &name )
  : asg::AsgTool(name),
    m_mvaElectron(0),
    m_mvaPhoton(0),
    m_folder(""),
    m_egammaType(0),
    m_mvaCalibratedEnergy(0),
    m_rawcl_Es0(0),
    m_rawcl_Es1(0),
    m_rawcl_Es2(0),
    m_rawcl_Es3(0),
    m_cl_eta(0),
    m_cl_E(0),
    m_ptconv(0),
    m_pt1conv(0),
    m_pt2conv(0),
    m_cl_etaCalo(0),
    m_cl_phiCalo(0),
    m_convtrk1nPixHits(0),
    m_convtrk1nSCTHits(0),
    m_convtrk2nPixHits(0),
    m_convtrk2nSCTHits(0),
    m_rawcl_calibHitsShowerDepth(0),
    m_Rconv(0)
{

  // Configurable properties...
  declareProperty("folder", m_folder="egammaMVACalib/v1");//, "folder with weight files");
}

StatusCode egammaMVATool::initialize(){
  ATH_MSG_DEBUG("In initialize of " << name() << "..." );
  
  m_mvaElectron = new egammaMVACalib(egammaMVACalib::egELECTRON, // particle type
				     true, // use new BDT (not TMVA)
				     m_folder, // folder with weight files
				     "BDTG", // method
				     1, // Full Calib
				     false, // not debugging
				     "", // that
				     "",
				     "",
				     "", // file pattern of xml files
				     true // ignore spectators
				     );
  m_mvaElectron->InitTree(0);
	
  m_mvaPhoton = new egammaMVACalib(egammaMVACalib::egPHOTON, // particle type
				   true, // use new BDT (not TMVA)
				   m_folder, // folder with weight files
				   "BDTG", // method
				   1 , // Full Calib
				   false, // not debugging
				   "", // that
				   "",
				   "",
				   "", // file pattern of xml files
				   true // ignore spectators
				   );
  m_mvaPhoton->InitTree(0);  
  return StatusCode::SUCCESS;
}

StatusCode egammaMVATool::finalize(){
  ATH_MSG_DEBUG( "in finalize" );

  return StatusCode::SUCCESS;
}

StatusCode egammaMVATool::execute(xAOD::CaloCluster* cluster,const xAOD::Egamma* eg){
  ATH_MSG_DEBUG( "in finalize" );
  if(!eg || !cluster){
    ATH_MSG_ERROR("Invalid Pointer to egamma or cluster object");
    return StatusCode::FAILURE;
  }
  double mvaE = getEnergy(cluster, eg);
  ATH_MSG_DEBUG( "Calculated MVA calibrated energy = " << mvaE );
  if(mvaE>0){
      cluster->setCalE(mvaE);
    }
  else{
    cluster->setCalE(cluster->e());
  }
  return StatusCode::SUCCESS;
}

StatusCode egammaMVATool::hltexecute(xAOD::CaloCluster* cluster,std::string egType) {
    ATH_MSG_DEBUG( "in finalize" );
    if(!cluster){
        ATH_MSG_ERROR("Invalid Pointer to cluster object");
        return StatusCode::FAILURE;
    }
    double mvaE = getEnergy(cluster, egType);
    ATH_MSG_DEBUG( "Calculated MVA calibrated energy = " << mvaE );
    if(mvaE>0){
        cluster->setCalE(mvaE);
    }
    else{
        cluster->setCalE(cluster->e());
    }
  return StatusCode::SUCCESS;
}

float egammaMVATool::getEnergy(xAOD::CaloCluster* cluster, std::string egType){
  ATH_MSG_DEBUG("In execute...");

  // Check for errors...
  if ( !cluster ){
    ATH_MSG_WARNING("no xAOD::CaloCluster object provided");
    return 0;
  }
  
  getClusterVariables(cluster);

  if(egType == "Electron"){
      ATH_MSG_DEBUG("Processing for electron...");
      m_mvaCalibratedEnergy = m_mvaElectron->getMVAEnergyElectron(m_rawcl_Es0,
							      m_rawcl_Es1,
							      m_rawcl_Es2,
							      m_rawcl_Es3,
							      m_cl_eta,
							      m_cl_E,
							      m_cl_etaCalo,
							      m_cl_phiCalo);

      return m_mvaCalibratedEnergy;
  }
  else if(egType == "Photon"){
    ATH_MSG_DEBUG("Processing for photon...");
    m_ptconv = 0;
    m_pt1conv = 0;
    m_pt2conv = 0;
    m_convtrk1nPixHits = 0;
    m_convtrk1nSCTHits = 0;
    m_convtrk2nPixHits = 0;
    m_convtrk2nSCTHits = 0;
    m_Rconv = 0;
    m_mvaCalibratedEnergy = m_mvaPhoton->getMVAEnergyPhoton(m_rawcl_Es0,
            m_rawcl_Es1,
            m_rawcl_Es2,
            m_rawcl_Es3,
            m_cl_eta,
            m_cl_E,
            m_cl_etaCalo,
            m_cl_phiCalo,
            m_ptconv,
            m_pt1conv,
            m_pt2conv,
            m_convtrk1nPixHits,
            m_convtrk1nSCTHits,
            m_convtrk2nPixHits,
            m_convtrk2nSCTHits,
            m_Rconv
            );

    return m_mvaCalibratedEnergy;
  }
  else{
    ATH_MSG_INFO("Unknown Type");
  }

  return 0;
}

float egammaMVATool::getEnergy(xAOD::CaloCluster* cluster, const xAOD::Egamma* eg){
  ATH_MSG_DEBUG("In execute...");

  // Check for errors...
  if ( !eg ){
    ATH_MSG_WARNING("no xAOD::Egamma object provided");
    return 0;
  }
  
  if( eg->type() == xAOD::Type::Electron  ){
    ATH_MSG_DEBUG("Processing for electron...");

    return getEnergy(cluster, static_cast<const xAOD::Electron*>(eg));
  }
  else if (eg->type() == xAOD::Type::Photon ){
    ATH_MSG_DEBUG("Processing for photon...");

    return getEnergy(cluster, static_cast<const xAOD::Photon*>(eg));
  }
  else{
    ATH_MSG_INFO("Unknown Type");
  }
  
  return 0;
}

float egammaMVATool::getEnergy(xAOD::CaloCluster* cluster, const xAOD::Electron* el){
  if(!el){
    ATH_MSG_ERROR("No electron passed");
    return 0;
  }
  
  getClusterVariables(cluster);

  m_mvaCalibratedEnergy = m_mvaElectron->getMVAEnergyElectron(m_rawcl_Es0,
							      m_rawcl_Es1,
							      m_rawcl_Es2,
							      m_rawcl_Es3,
							      m_cl_eta,
							      m_cl_E,
							      m_cl_etaCalo,
							      m_cl_phiCalo);
  
  return m_mvaCalibratedEnergy;
}

float egammaMVATool::getEnergy(xAOD::CaloCluster* cluster, const xAOD::Photon* ph){
  if(!ph){
    ATH_MSG_ERROR("No photon passed");
    return 0;
  }

  
  if( !getClusterVariables(cluster)){
    ATH_MSG_ERROR("Could not retrieve cluster variables.");
    return 0;
  }

  const xAOD::Vertex* phVertex = ph->vertex();
  if(!phVertex){
    m_ptconv = 0;
    m_pt1conv = 0;
    m_pt2conv = 0;
    m_convtrk1nPixHits = 0;
    m_convtrk1nSCTHits = 0;
    m_convtrk2nPixHits = 0;
    m_convtrk2nSCTHits = 0;
    m_Rconv = 0;
  } else{
    getConversionVariables(phVertex);
  }
  
  m_mvaCalibratedEnergy = m_mvaPhoton->getMVAEnergyPhoton(m_rawcl_Es0,
							  m_rawcl_Es1,
							  m_rawcl_Es2,
							  m_rawcl_Es3,
							  m_cl_eta,
							  m_cl_E,
							  m_cl_etaCalo,
							  m_cl_phiCalo,
							  m_ptconv,
							  m_pt1conv,
							  m_pt2conv,
							  m_convtrk1nPixHits,
							  m_convtrk1nSCTHits,
							  m_convtrk2nPixHits,
							  m_convtrk2nSCTHits,
							  m_Rconv
							  );
  return m_mvaCalibratedEnergy;
}

bool egammaMVATool::getClusterVariables(const xAOD::CaloCluster* cluster){

  // The cluster energy in the sampling layers
  m_rawcl_Es0 = cluster->energyBE(0);
  m_rawcl_Es1 = cluster->energyBE(1);
  m_rawcl_Es2 = cluster->energyBE(2);
  m_rawcl_Es3 = cluster->energyBE(3);

  m_cl_eta = cluster->eta();//el_cl_eta;
  m_cl_E = cluster->e(); //el_cl_E;

  // Correct this with the right variables
  m_cl_etaCalo = 0.; 
  if(! (cluster->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,m_cl_etaCalo) ) ){
      ATH_MSG_ERROR("etaCalo not found in CaloCluster object");
      return false;
    }
  
    m_cl_phiCalo = 0.; 
    if(! (cluster->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME,m_cl_phiCalo) ) ){
      ATH_MSG_ERROR("phiCalo not found in CaloCluster object");
      return false;
    }
    
    return true;
}

bool egammaMVATool::getConversionVariables(const xAOD::Vertex *phVertex){
  const Amg::Vector3D pos = phVertex->position();
  m_Rconv = static_cast<float>(hypot (pos.x(), pos.y()));

  const xAOD::TrackParticle* tp0 = phVertex->trackParticle(0);
  const xAOD::TrackParticle* tp1 = phVertex->trackParticle(1);
  
  TLorentzVector sum;
  if(tp0){
    sum += tp0->p4();
    uint8_t hits;
    tp0->summaryValue(hits,xAOD::numberOfPixelHits);
    m_convtrk1nPixHits = hits;
    tp0->summaryValue(hits,xAOD::numberOfSCTHits);
    m_convtrk1nSCTHits = hits;
    m_pt1conv = static_cast<float>(tp0->pt());
  }

  if(tp1){
    sum += tp1->p4();
    uint8_t hits;
    tp1->summaryValue(hits,xAOD::numberOfPixelHits);
    m_convtrk2nPixHits = hits;
    tp1->summaryValue(hits,xAOD::numberOfSCTHits);
    m_convtrk2nSCTHits = hits;
    m_pt2conv = static_cast<float>(tp1->pt());
  }

  m_ptconv = sum.Perp();

  return true;
}

