/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"
// xAOD
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODEventInfo/EventInfo.h"
#include "PATInterfaces/SystematicRegistry.h" 

// Header for the ROOT tool of this package 
#include "ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionTool.h"

typedef AtlasRoot::egammaEnergyCorrectionTool::ParticleInformation ParticleInformation;

namespace CP { 

EgammaCalibrationAndSmearingTool::EgammaCalibrationAndSmearingTool( const std::string &name )
  : asg::AsgTool(name),
    m_TESModel(egEnergyCorr::UNDEFINED),
    m_seed(0),
    m_TResolutionType(egEnergyCorr::Resolution::SigmaEff90),
    m_CalibratedEnergy(-1), // why do we need this?
    m_CorrectedMomentum(-1), // why do we need this?
    m_rootTool(0),
    m_currentScaleVariation(egEnergyCorr::Scale::Nominal), 
    m_currentResolutionVariation(egEnergyCorr::Resolution::Nominal)
{
  declareProperty("ESModel", m_ESModel = "es2012c");
  declareProperty("ResolutionType", m_ResolutionType = "SigmaEff90");
  declareProperty("varSF", m_varSF = 1.0);
  declareProperty("debug", m_debug = false);
  declareProperty("doScaleCorrection", m_doScaleCorrection = true);
  declareProperty("doSmearing", m_doSmearing = true);
  declareProperty("useLayerCorrection", m_useLayerCorrection = false);
  declareProperty("forceLayerCorrection", m_forceLayerCorrection = false);
  declareProperty("usePSCorrection", m_usePSCorrection = false);  
  declareProperty("forcePSCorrection", m_forcePSCorrection = false);  
  declareProperty("useS12Correction", m_useS12Correction = false);
  declareProperty("forceS12Correction", m_forceS12Correction = false);
  declareProperty("useLayer2Recalibration", m_useLayer2Recalibration = false);  
  declareProperty("forceLayer2Recalibration", m_forceLayer2Recalibration = false);  
  declareProperty("useIntermoduleCorrection", m_useIntermoduleCorrection = false);  
  declareProperty("forceIntermoduleCorrection", m_forceIntermoduleCorrection = false);  
  declareProperty("usePhiUniformCorrection", m_usePhiUniformCorrection = false);  
  declareProperty("forcePhiUniformCorrection", m_forcePhiUniformCorrection = false);  
  declareProperty("useGainCorrection", m_useGainCorrection = false);
  declareProperty("forceGainCorrection", m_forceGainCorrection = false);
}

StatusCode EgammaCalibrationAndSmearingTool::initialize(){
  ATH_MSG_INFO("In initialize of " << name() << "..." );
  
  if (m_ESModel == "es2010") { m_TESModel = egEnergyCorr::es2010; }           // legacy
  else if (m_ESModel == "es2011c") { m_TESModel = egEnergyCorr::es2011c; }    // mc11c : faulty G4; old geometry
  else if (m_ESModel == "es2011d") { m_TESModel = egEnergyCorr::es2011d; }    // mc11d : corrected G4; new geometry == final Run1 scheme
  else if (m_ESModel == "es2012a") { m_TESModel = egEnergyCorr::es2012a; }    // mc12a : "crude" G4 fix; old geometry
  else if (m_ESModel == "es2012c") { m_TESModel = egEnergyCorr::es2012c; }    // mc12c : corrected G4; new geometry == final Run1 scheme
  else {
    ATH_MSG_ERROR("Cannot understand model " << m_ESModel);
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("ESModel: " << m_ESModel);
  
  if (m_ResolutionType == "Gaussian") { m_TResolutionType = egEnergyCorr::Resolution::Gaussian; }
  else if (m_ResolutionType == "SigmaEff80") { m_TResolutionType = egEnergyCorr::Resolution::SigmaEff80; }
  else if (m_ResolutionType == "SigmaEff90") { m_TResolutionType = egEnergyCorr::Resolution::SigmaEff90; }
  else {
    ATH_MSG_ERROR("Cannot understand resolution " << m_ResolutionType);
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_INFO("ResolutionType: " << m_ResolutionType);

  if (m_debug) { ATH_MSG_INFO("using debug"); }

  //apply or not scale correction
  //WARNING: this may be overwritten if you call systematics
  if(!m_doScaleCorrection){ 
    m_currentScaleVariation = egEnergyCorr::Scale::None; 
    ATH_MSG_INFO("Do not apply SCALE CORRECTION");
  }
  //apply or not smearing
  //WARNING: this may be overwritten if you call systematics
  if(!m_doSmearing){
    m_currentResolutionVariation = egEnergyCorr::Resolution::None;
    ATH_MSG_INFO("Do not use SMEARING");
  }

  //create the root underlying tool
  m_rootTool = new AtlasRoot::egammaEnergyCorrectionTool();
  if (!m_rootTool) {
    ATH_MSG_ERROR("Cannot initialize underlying tool");
    return StatusCode::FAILURE;
  }

  // configure the underlying tool
  m_rootTool->setESModel(m_TESModel);
  m_rootTool->setDebug(m_debug);
  
  m_rootTool->initialize();
  
  //force settings
  if(m_forceLayerCorrection || m_forcePSCorrection || m_forceS12Correction || m_forceLayer2Recalibration || m_forceIntermoduleCorrection || m_forcePhiUniformCorrection || m_forceGainCorrection){
    ATH_MSG_INFO("Specific corrections will be applied (not the default ones for the given configuration!)");
  }
  if(m_forceLayerCorrection) m_rootTool->useLayerCorrection(m_useLayerCorrection);
  if(m_forcePSCorrection) m_rootTool->applyPSCorrection(m_usePSCorrection); 
  if(m_forceS12Correction) m_rootTool->applyS12Correction(m_useS12Correction); 
  if(m_forceLayer2Recalibration) m_rootTool->useLayer2Recalibration(m_useLayer2Recalibration); 
  if(m_forceIntermoduleCorrection) m_rootTool->useIntermoduleCorrection(m_useIntermoduleCorrection); 
  if(m_forcePhiUniformCorrection) m_rootTool->usePhiUniformCorrection(m_usePhiUniformCorrection); 
  if(m_forceGainCorrection) m_rootTool->useGainCorrection(m_useGainCorrection);
  
  //systematics
  applySystematicVariation( CP::SystematicSet () ).ignore();
  CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  if( registry.registerSystematics( *this ) != CP::SystematicCode::Ok ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

StatusCode EgammaCalibrationAndSmearingTool::finalize(){
  ATH_MSG_INFO( "in finalize" );

  return StatusCode::SUCCESS;
}

void EgammaCalibrationAndSmearingTool::setRandomSeed ( unsigned seed )
{
  m_rootTool->setRandomSeed(seed);
}

void EgammaCalibrationAndSmearingTool::setDefaultConfiguration(const xAOD::EventInfo* event_info)
{
  PATCore::ParticleDataType::DataType dataType = (event_info->eventType(xAOD::EventInfo::IS_SIMULATION)) ? PATCore::ParticleDataType::Full : PATCore::ParticleDataType::Data;
  if(dataType==PATCore::ParticleDataType::Full){//MC
    m_currentScaleVariation = egEnergyCorr::Scale::None;  
    m_currentResolutionVariation = egEnergyCorr::Resolution::Nominal;
  } 
  else{//DATA
    m_currentScaleVariation = egEnergyCorr::Scale::Nominal;   
    m_currentResolutionVariation = egEnergyCorr::Resolution::None;
  }
}

void EgammaCalibrationAndSmearingTool::forceSmearing(bool force)
{
  if(force) m_currentResolutionVariation = egEnergyCorr::Resolution::Nominal;
  else m_currentResolutionVariation = egEnergyCorr::Resolution::None; 
}

void EgammaCalibrationAndSmearingTool::forceScaleCorrection(bool force) 
{ 
  if(force) m_currentScaleVariation = egEnergyCorr::Scale::Nominal;
  else m_currentScaleVariation = egEnergyCorr::Scale::None;
}

CP::CorrectionCode EgammaCalibrationAndSmearingTool::applyCorrection(xAOD::Egamma & input, const xAOD::EventInfo* event_info) 
{ 
  const double new_energy = getEnergy(&input, event_info);
  const double cosh_eta = input.e() / input.pt(); 
  input.setPt(new_energy / cosh_eta); 
  return CP::CorrectionCode::Ok; 
}

CP::CorrectionCode EgammaCalibrationAndSmearingTool::correctedCopy( const xAOD::Egamma& input, xAOD::Egamma*& output, const xAOD::EventInfo* event_info)
{
  // A sanity check:
  if( output ) ATH_MSG_WARNING( "Non-null pointer received. " "There's a possible memory leak!" );

  if ( input.type() == xAOD::Type::Electron ) {
    output = new xAOD::Electron();
    output->makePrivateStore( static_cast<const xAOD::Electron&>(input) );
    return applyCorrection( *output, event_info );
  }
  else if ( input.type() == xAOD::Type::Photon ) {
    output = new xAOD::Photon();
    output->makePrivateStore( static_cast<const xAOD::Photon&>(input) );
    return applyCorrection( *output, event_info );
  }
  else {
    ATH_MSG_WARNING("Didn't get an electron or photon");
    return CP::CorrectionCode::Error;
  }
  
}

double EgammaCalibrationAndSmearingTool::getEnergy(const xAOD::Egamma* p, const xAOD::EventInfo* event_info)
{
  const xAOD::Electron *el = dynamic_cast<const xAOD::Electron*>(p);
  if (el) { return getElectronEnergy(el, event_info); }//runnumber, dataType, m_resType, m_varSF); } 
    
  const xAOD::Photon *ph = dynamic_cast<const xAOD::Photon*>(p);
  if (ph) { return getPhotonEnergy(ph, event_info); }//runnumber, dataType, resType, varSF); }
    
  // TODO: find a better solution: return status code
  return -999;
}

double EgammaCalibrationAndSmearingTool::getElectronEnergy(const xAOD::Electron *el, const xAOD::EventInfo* event_info)
{
  const unsigned int runnumber = event_info->runNumber(); 
  PATCore::ParticleDataType::DataType dataType = (event_info->eventType(xAOD::EventInfo::IS_SIMULATION)) ? PATCore::ParticleDataType::Full : PATCore::ParticleDataType::Data;
  
  //cluster
  const xAOD::CaloCluster* eCluster = el->caloCluster();
  if (!eCluster) {
    ATH_MSG_ERROR("electron without CaloCluster");
    return -1;
  }

  float m_el_rawcl_Es0 = -999.; 
  float m_el_rawcl_Es1 = -999.; 
  float m_el_rawcl_Es2 = -999.; 
  float m_el_rawcl_Es3 = -999.; 
  float m_el_cl_eta = -999.; 
  float m_el_cl_phi = -999.; 
  float m_el_cl_E = -999.; 
  float m_el_cl_etaCalo = -999.; 
  float m_el_cl_phiCalo = -999.; 
  //float m_el_rawcl_calibHitsShowerDepth = -999.; 
  float m_el_tracketa = -999.; 
  
  m_el_rawcl_Es0 = eCluster->energyBE(0);
  m_el_rawcl_Es1 = eCluster->energyBE(1);
  m_el_rawcl_Es2 = eCluster->energyBE(2);
  m_el_rawcl_Es3 = eCluster->energyBE(3);
  m_el_cl_eta = eCluster->eta();   
  m_el_cl_phi = eCluster->phi();
  m_el_cl_E = eCluster->e();
  
  //cluster moments
  //the following does not work for now: use the code below, copied from egammaMvaTool
  //=> need to validate that these are the correct quantities
  /*
  double el_cl_etaCalo_tmp = 999.;
  if(eCluster->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,el_cl_etaCalo_tmp)) m_el_cl_etaCalo=el_cl_etaCalo_tmp;
  //m_el_cl_etaCalo=eCluster()->isAvailable<float>("etaCalo");
  //std::cout << m_el_cl_etaCalo << std::endl;
  double el_cl_phiCalo_tmp = 999.;
  if(eCluster->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME,el_cl_phiCalo_tmp)) m_el_cl_phiCalo=el_cl_phiCalo_tmp;
  //!!!temporary hack!!! => gets 0 for both moments
  m_el_cl_etaCalo = m_el_cl_eta;
  m_el_cl_phiCalo = m_el_cl_phi;
  */
  
  if(eCluster->isAvailable<float>("etaCalo")) 
    m_el_cl_etaCalo = eCluster->auxdata<float>("etaCalo"); 
  else{
    std::cout << "WARNING: etaCalo not available as auxilliary variable => setting it to 0" << std::endl; 
    m_el_cl_etaCalo = 0.; 
  }
  if(eCluster->isAvailable<float>("phiCalo")) 
    m_el_cl_phiCalo = eCluster->auxdata<float>("phiCalo"); 
  else{
    std::cout << "WARNING: phiCalo not available as auxilliary variable => setting it to 0" << std::endl;
    m_el_cl_phiCalo = 0.; 
  }
  
  //track eta 
  const xAOD::TrackParticle* eTrack = el->trackParticle(); 
  m_el_tracketa = eTrack->eta();

   //decide if we are doing nominal case (will have both 'variations' set to nominal)
   egEnergyCorr::Scale::Variation thisScaleVariation = m_currentScaleVariation;
   egEnergyCorr::Resolution::Variation thisResolutionVariation = m_currentResolutionVariation;

   if(m_currentScaleVariation==egEnergyCorr::Scale::Nominal && m_currentResolutionVariation == egEnergyCorr::Resolution::Nominal) {
	//assume we are actually doing the 'default configuration'
	if(dataType==PATCore::ParticleDataType::Full) thisScaleVariation=egEnergyCorr::Scale::None; //MC
	else thisResolutionVariation=egEnergyCorr::Resolution::None; //Data
   }

  m_CalibratedEnergy = m_rootTool->getCorrectedEnergy(runnumber,  
						      dataType,
						      ParticleInformation(m_el_rawcl_Es0,  
									  m_el_rawcl_Es1,  
									  m_el_rawcl_Es2,  
									  m_el_rawcl_Es3,  
									  m_el_cl_eta,  
									  m_el_cl_phi,  
									  m_el_tracketa,  
									  m_el_cl_E,  
									  m_el_cl_etaCalo,  
									  m_el_cl_phiCalo),  
						      thisScaleVariation,
						      thisResolutionVariation,
						      m_TResolutionType,
						      m_varSF);

  return m_CalibratedEnergy;
  
}

double EgammaCalibrationAndSmearingTool::getPhotonEnergy(const xAOD::Photon *ph,const xAOD::EventInfo* event_info)
{
  const unsigned int runnumber = event_info->runNumber();  
  PATCore::ParticleDataType::DataType dataType = (event_info->eventType(xAOD::EventInfo::IS_SIMULATION)) ? PATCore::ParticleDataType::Full : PATCore::ParticleDataType::Data;
  
  //cluster
  const xAOD::CaloCluster* eCluster = ph->caloCluster();
  if (!eCluster) {
    ATH_MSG_ERROR("photon without CaloCluster");
    return -1;
  }
  
  float m_ph_rawcl_Es0 = -999.; 
  float m_ph_rawcl_Es1 = -999.; 
  float m_ph_rawcl_Es2 = -999.; 
  float m_ph_rawcl_Es3 = -999.; 
  float m_ph_cl_eta = -999.; 
  float m_ph_cl_phi = -999.; 
  float m_ph_cl_E = -999.; 
  float m_ph_ptconv = -999.; 
  float m_ph_pt1conv = -999.; 
  float m_ph_pt2conv = -999.; 
  float m_ph_cl_etaCalo = -999.; 
  float m_ph_cl_phiCalo = -999.; 
  int m_ph_convtrk1nPixHits = -999; 
  int m_ph_convtrk1nSCTHits = -999; 
  int m_ph_convtrk2nPixHits = -999; 
  int m_ph_convtrk2nSCTHits = -999; 
  //float m_ph_rawcl_calibHitsShowerDepth = -999.; 
  float m_ph_Rconv = -999.;


  m_ph_rawcl_Es0 = eCluster->energyBE(0);
  m_ph_rawcl_Es1 = eCluster->energyBE(1);
  m_ph_rawcl_Es2 = eCluster->energyBE(2);
  m_ph_rawcl_Es3 = eCluster->energyBE(3);
  m_ph_cl_eta = eCluster->eta();   
  m_ph_cl_phi = eCluster->phi();
  m_ph_cl_E = eCluster->e();
  
  //cluster moments
  //the following does not work for now: use the code below, copied from egammaMvaTool 
  //=> need to validate that these are the correct quantities
  /*
  double ph_cl_etaCalo_tmp = 999.;
  if(eCluster->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,ph_cl_etaCalo_tmp)) m_ph_cl_etaCalo=ph_cl_etaCalo_tmp;
  double ph_cl_phiCalo_tmp = 999.;
  if(eCluster->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME,ph_cl_phiCalo_tmp)) m_ph_cl_phiCalo=ph_cl_phiCalo_tmp;
  //!!!temporary hack!!! => gets 0 for both moments
  m_ph_cl_etaCalo = m_ph_cl_eta;
  m_ph_cl_phiCalo = m_ph_cl_phi;
  */

  if(eCluster->isAvailable<float>("etaCalo"))  
    m_ph_cl_etaCalo = eCluster->auxdata<float>("etaCalo");  
  else{ 
    std::cout << "WARNING: etaCalo not available as auxilliary variable => setting it to 0" << std::endl;  
    m_ph_cl_etaCalo = 0.;  
  } 
  if(eCluster->isAvailable<float>("phiCalo"))  
    m_ph_cl_phiCalo = eCluster->auxdata<float>("phiCalo");  
  else{ 
    std::cout << "WARNING: phiCalo not available as auxilliary variable => setting it to 0" << std::endl; 
    m_ph_cl_phiCalo = 0.;  
  }
  
  //conversion
  const xAOD::Vertex* phVertex = ph->vertex();
  if(!phVertex){
    m_ph_ptconv = 0;
    m_ph_pt1conv = 0;
    m_ph_pt2conv = 0;
    m_ph_convtrk1nPixHits = 0;
    m_ph_convtrk1nSCTHits = 0;
    m_ph_convtrk2nPixHits = 0;
    m_ph_convtrk2nSCTHits = 0;
    m_ph_Rconv = 0;
  }
  else{
    const Amg::Vector3D pos = phVertex->position();
    m_ph_Rconv = static_cast<float>(hypot (pos.x(), pos.y()));

    const xAOD::TrackParticle* tp0 = phVertex->trackParticle(0);
    const xAOD::TrackParticle* tp1 = phVertex->trackParticle(1);

    TLorentzVector sum;
    if(tp0){
      sum += tp0->p4();
      uint8_t hits;
      tp0->summaryValue(hits,xAOD::numberOfPixelHits);
      m_ph_convtrk1nPixHits = hits;
      tp0->summaryValue(hits,xAOD::numberOfSCTHits);
      m_ph_convtrk1nSCTHits = hits;

      m_ph_pt1conv = static_cast<float>(tp0->pt());
    }

    if(tp1){
      sum += tp1->p4();
      uint8_t hits;
      tp1->summaryValue(hits,xAOD::numberOfPixelHits);
      m_ph_convtrk2nPixHits = hits;
      tp1->summaryValue(hits,xAOD::numberOfSCTHits);
      m_ph_convtrk2nSCTHits = hits;

      m_ph_pt2conv = static_cast<float>(tp1->pt());
    }

    m_ph_ptconv = sum.Perp();
  }
  
   //decide if we are doing nominal case (will have both 'variations' set to nominal)
   egEnergyCorr::Scale::Variation thisScaleVariation = m_currentScaleVariation;
   egEnergyCorr::Resolution::Variation thisResolutionVariation = m_currentResolutionVariation;
   if(m_currentScaleVariation==egEnergyCorr::Scale::Nominal && m_currentResolutionVariation == egEnergyCorr::Resolution::Nominal) {
	//assume we are actually doing the 'default configuration'
	if(dataType==PATCore::ParticleDataType::Full) thisScaleVariation=egEnergyCorr::Scale::None; //MC
	else thisResolutionVariation=egEnergyCorr::Resolution::None; //Data
   }

  m_CalibratedEnergy = m_rootTool->getCorrectedEnergy(runnumber,  
						      dataType,
						      ParticleInformation(m_ph_rawcl_Es0,  
									  m_ph_rawcl_Es1,  
									  m_ph_rawcl_Es2,  
									  m_ph_rawcl_Es3,  
									  m_ph_cl_eta,  
									  m_ph_cl_phi,  
									  m_ph_cl_E,  
									  m_ph_cl_etaCalo,  
									  m_ph_cl_phiCalo,  
									  m_ph_ptconv,
									  m_ph_pt1conv,
									  m_ph_pt2conv,
									  m_ph_convtrk1nPixHits,
									  m_ph_convtrk1nSCTHits,
									  m_ph_convtrk2nPixHits,
									  m_ph_convtrk2nSCTHits,
									  m_ph_Rconv),
						      thisScaleVariation,
						      thisResolutionVariation,
						      m_TResolutionType, 
                                                      m_varSF);
  
  return m_CalibratedEnergy;
  
}

double EgammaCalibrationAndSmearingTool::getElectronMomentum(const xAOD::Electron *el, const xAOD::EventInfo* event_info) 
{ 
  PATCore::ParticleDataType::DataType dataType = (event_info->eventType(xAOD::EventInfo::IS_SIMULATION)) ? PATCore::ParticleDataType::Full : PATCore::ParticleDataType::Data;
  
  //adjust by hand the ParticleType
  PATCore::ParticleType::Type ptype = PATCore::ParticleType::Electron;
  
  //track momentum and eta
  float m_el_tracketa = -999.;  
  float m_el_trackmomentum = -999.;
  
  const xAOD::TrackParticle* eTrack = el->trackParticle();  
  m_el_tracketa = eTrack->eta(); 
  m_el_trackmomentum = eTrack->pt()*cosh(el->eta());
  
  m_CorrectedMomentum = m_rootTool->getCorrectedMomentum(dataType,
							 ptype,
							 m_el_trackmomentum,
							 m_el_tracketa,
							 m_currentScaleVariation,
							 m_varSF);
  
  return m_CorrectedMomentum;
}

//Systematics
bool EgammaCalibrationAndSmearingTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const { 
  CP::SystematicSet sys = affectingSystematics(); 
  return sys.find( systematic ) != sys.end(); 
} 

CP::SystematicSet EgammaCalibrationAndSmearingTool::affectingSystematics() const { 
  CP::SystematicSet result;

  //scale electrons only
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::MomentumUp] = CP::SystematicVariation("EL_SCALE_MOMENTUM", 1) );  
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::MomentumDown] = CP::SystematicVariation("EL_SCALE_MOMENTUM", -1) );
  
  //scale (electrons and photons)
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::ZeeStatUp] =CP::SystematicVariation( "EG_SCALE_ZEESTAT", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::ZeeStatDown] =CP::SystematicVariation( "EG_SCALE_ZEESTAT", -1 ) ); 
 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::ZeeSystUp] =CP::SystematicVariation( "EG_SCALE_ZEESYST", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::ZeeSystDown] =CP::SystematicVariation( "EG_SCALE_ZEESYST", -1 ) ); 
 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::LArCalibUp] =CP::SystematicVariation( "EG_SCALE_LARCALIB", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::LArCalibDown] =CP::SystematicVariation( "EG_SCALE_LARCALIB", -1 ) ); 
 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::LArUnconvCalibUp] =CP::SystematicVariation( "EG_SCALE_LARUNCONVCALIB", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::LArUnconvCalibDown] =CP::SystematicVariation( "EG_SCALE_LARUNCONVCALIB", -1 ) ); 
 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::LArElecCalibUp] =CP::SystematicVariation( "EG_SCALE_LARELECCALIB", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::LArElecCalibDown] =CP::SystematicVariation( "EG_SCALE_LARELECCALIB", -1 ) ); 
 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::LArElecUnconvUp] =CP::SystematicVariation( "EG_SCALE_LARELECUNCONV", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::LArElecUnconvDown] =CP::SystematicVariation( "EG_SCALE_LARELECUNCONV", -1 ) );

  result.insert( m_scaleSystMap[egEnergyCorr::Scale::G4Up] =CP::SystematicVariation( "EG_SCALE_G4", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::G4Down] =CP::SystematicVariation( "EG_SCALE_G4", -1 ) ); 
    
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::PSUp] =CP::SystematicVariation( "EG_SCALE_PS", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::PSDown] =CP::SystematicVariation( "EG_SCALE_PS", -1 ) ); 
 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::S12Up] =CP::SystematicVariation( "EG_SCALE_S12", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::S12Down] =CP::SystematicVariation( "EG_SCALE_S12", -1 ) ); 
 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::MatIDUp] =CP::SystematicVariation( "EG_SCALE_MATID", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::MatIDDown] =CP::SystematicVariation( "EG_SCALE_MATID", -1 ) ); 
 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::MatCryoUp] =CP::SystematicVariation( "EG_SCALE_MATCRYO", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::MatCryoDown] =CP::SystematicVariation( "EG_SCALE_MATCRYO", -1 ) ); 
 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::MatCaloUp] =CP::SystematicVariation( "EG_SCALE_MATCALO", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::MatCaloDown] =CP::SystematicVariation( "EG_SCALE_MATCALO", -1 ) ); 
 
  result.insert(m_scaleSystMap[egEnergyCorr::Scale::L1GainUp] = CP::SystematicVariation( "EG_SCALE_L1GAIN", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::L1GainDown] =CP::SystematicVariation( "EG_SCALE_L1GAIN", -1 ) ); 
 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::L2GainUp] =CP::SystematicVariation( "EG_SCALE_L2GAIN", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::L2GainDown] =CP::SystematicVariation( "EG_SCALE_L2GAIN", -1 ) ); 
 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::PedestalUp] =CP::SystematicVariation( "EG_SCALE_PEDESTAL", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::PedestalDown] =CP::SystematicVariation( "EG_SCALE_PEDESTAL", -1 ) );
  
  //scale photons only
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::LeakageUnconvUp] =CP::SystematicVariation( "PH_SCALE_LEAKAGEUNCONV", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::LeakageUnconvDown] =CP::SystematicVariation( "PH_SCALE_LEAKAGEUNCONV", -1 ) ); 
 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::LeakageConvUp] =CP::SystematicVariation( "PH_SCALE_LEAKAGECONV", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::LeakageConvDown] =CP::SystematicVariation( "PH_SCALE_LEAKAGECONV", -1 ) ); 
 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::ConvEfficiencyUp] =CP::SystematicVariation( "PH_SCALE_CONVEFFICIENCY", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::ConvEfficiencyDown] =CP::SystematicVariation( "PH_SCALE_CONVEFFICIENCY", -1 ) ); 
 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::ConvFakeRateUp] =CP::SystematicVariation( "PH_SCALE_CONVFAKERATE", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::ConvFakeRateDown] =CP::SystematicVariation( "PH_SCALE_CONVFAKERATE", -1 ) ); 
 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::ConvRadiusUp] =CP::SystematicVariation( "PH_SCALE_CONVRADIUS", 1 ) ); 
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::ConvRadiusDown] =CP::SystematicVariation( "PH_SCALE_CONVRADIUS", -1 ) );
  
  //scale technical
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::AllUp] =CP::SystematicVariation( "EG_SCALE_ALL", 1 ) );  
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::AllDown] =CP::SystematicVariation( "EG_SCALE_ALL", -1 ) );
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::None] =CP::SystematicVariation( "EG_SCALE_NONE") );
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::Nominal] =CP::SystematicVariation( "EG_SCALE_NOMINAL") );
  result.insert( m_scaleSystMap[egEnergyCorr::Scale::LastScaleVariation] = CP::SystematicVariation( "EG_SCALE_LASTSCALEVARIATION" ) );
  
  //smearing (electrons and photons) 
  result.insert( m_resSystMap[egEnergyCorr::Resolution::ZSmearingUp] = CP::SystematicVariation( "EG_RESOLUTION_ZSMEARING", 1 ) ) ; 
  result.insert( m_resSystMap[egEnergyCorr::Resolution::ZSmearingDown] = CP::SystematicVariation( "EG_RESOLUTION_ZSMEARING", -1 ) ); 
 
  result.insert( m_resSystMap[egEnergyCorr::Resolution::SamplingTermUp] = CP::SystematicVariation( "EG_RESOLUTION_SAMPLINGTERM", 1 ) ); 
  result.insert( m_resSystMap[egEnergyCorr::Resolution::SamplingTermUp] = CP::SystematicVariation( "EG_RESOLUTION_SAMPLINGTERM", -1 ) ); 
 
  result.insert( m_resSystMap[egEnergyCorr::Resolution::MaterialIDUp] = CP::SystematicVariation( "EG_RESOLUTION_MATERIALID", 1 ) );  
  result.insert( m_resSystMap[egEnergyCorr::Resolution::MaterialIDDown] = CP::SystematicVariation( "EG_RESOLUTION_MATERIALID", -1 ) ); 
    
  result.insert( m_resSystMap[egEnergyCorr::Resolution::MaterialCaloUp] = CP::SystematicVariation( "EG_RESOLUTION_MATERIALCALO", 1 ) ); 
  result.insert( m_resSystMap[egEnergyCorr::Resolution::MaterialCaloDown] = CP::SystematicVariation( "EG_RESOLUTION_MATERIALCALO", -1 ) ); 
    
  result.insert( m_resSystMap[egEnergyCorr::Resolution::MaterialGapUp] = CP::SystematicVariation( "EG_RESOLUTION_MATERIALGAP", 1 ) ); 
  result.insert( m_resSystMap[egEnergyCorr::Resolution::MaterialGapDown] = CP::SystematicVariation( "EG_RESOLUTION_MATERIALGAP", -1 ) ); 
 
  result.insert( m_resSystMap[egEnergyCorr::Resolution::MaterialCryoUp] = CP::SystematicVariation( "EG_RESOLUTION_MATERIALCRYO", 1 ) ); 
  result.insert( m_resSystMap[egEnergyCorr::Resolution::MaterialCryoDown] = CP::SystematicVariation( "EG_RESOLUTION_MATERIALCRYO", -1 ) ); 
 
  result.insert( m_resSystMap[egEnergyCorr::Resolution::PileUpUp] = CP::SystematicVariation( "EG_RESOLUTION_PILEUP", 1 ) ); 
  result.insert( m_resSystMap[egEnergyCorr::Resolution::PileUpDown] = CP::SystematicVariation( "EG_RESOLUTION_PILEUP", -1 ) ); 
    
  //smearing technical 
  result.insert( m_resSystMap[egEnergyCorr::Resolution::AllUp] = CP::SystematicVariation( "EG_RESOLUTION_ALL", 1 ) );  
  result.insert( m_resSystMap[egEnergyCorr::Resolution::AllDown] = CP::SystematicVariation( "EG_RESOLUTION_ALL", -1 ) ); 
  result.insert( m_resSystMap[egEnergyCorr::Resolution::None] = CP::SystematicVariation( "EG_RESOLUTION_NONE" ) ); 
  result.insert( m_resSystMap[egEnergyCorr::Resolution::Nominal] = CP::SystematicVariation( "EG_RESOLUTION_NOMINAL" ) ); 
  result.insert( m_resSystMap[egEnergyCorr::Resolution::LastResolutionVariation] = CP::SystematicVariation( "EG_RESOLUTION_LASTRESOLUTIONVARIATION" ) );
  
  return result;
}

CP::SystematicSet EgammaCalibrationAndSmearingTool::recommendedSystematics() const { 
  return affectingSystematics(); 
} 

CP::SystematicCode EgammaCalibrationAndSmearingTool::applySystematicVariation( const CP::SystematicSet& systConfig ) { 

   //switch to 'nominal' which we flag by both scale and resolution variations being equal to nominal
   //in the actual calibration function (above) it will use the data/mc flag to decide which of these to switch to 'none'
   m_currentScaleVariation = egEnergyCorr::Scale::Nominal;
   m_currentResolutionVariation = egEnergyCorr::Resolution::Nominal;

   //the following code allows only ONE systematic variation at a time. If you try and do two, only the last one in the map will be applied

  std::map<egEnergyCorr::Scale::Variation, CP::SystematicVariation>::iterator scaleIter;
  for(scaleIter=m_scaleSystMap.begin();scaleIter!=m_scaleSystMap.end();scaleIter++){
    if(systConfig.find( scaleIter->second )!=systConfig.end()) {
      m_currentScaleVariation = scaleIter->first;
      m_currentResolutionVariation = egEnergyCorr::Resolution::Nominal;
    }
    }
  
  std::map<egEnergyCorr::Resolution::Variation, CP::SystematicVariation>::iterator resIter;
  for(resIter=m_resSystMap.begin();resIter!=m_resSystMap.end();resIter++){ 
    if(systConfig.find( resIter->second )!=systConfig.end()) {
      m_currentResolutionVariation = resIter->first; 
      m_currentScaleVariation = egEnergyCorr::Scale::None;
    }
  }
  
  return CP::SystematicCode::Ok; 
}

}
