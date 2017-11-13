/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// McMaterialEffectsUpdator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_FatrasTools/McMaterialEffectsUpdator.h"

// Gaudi Kernel
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
// ISF includes
#include "ISF_Interfaces/IParticleBroker.h"
#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ISFTruthIncident.h"
#include "ISF_Event/ParticleClipboard.h"
#include "ISF_Event/ParticleUserInformation.h"
#include "ISF_FatrasInterfaces/IParticleDecayHelper.h"
// iFatras
#include "ISF_FatrasInterfaces/IHadronicInteractionProcessor.h"
#include "ISF_FatrasInterfaces/IProcessSamplingTool.h"
#include "ISF_FatrasInterfaces/IPhysicsValidationTool.h"
#include "ISF_FatrasInterfaces/IPhotonConversionTool.h"
// FastSimulation
#include "FastSimulationEvent/GenParticleEnergyDepositMap.h"
// Trk inlcude
#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "TrkExInterfaces/IMultipleScatteringUpdator.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkSurfaces/Surface.h" 
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandLandau.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
// Validation mode - TTree includes
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h" 
// STD
#include <math.h>

// temporary
#include "TrkGeometry/TrackingVolume.h"

// constructor
iFatras::McMaterialEffectsUpdator::McMaterialEffectsUpdator(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_eLoss(true),
  m_eLossUpdator("iFatras::McEnergyLossUpdator/FatrasEnergyLossUpdator"),
  m_ms(true),
  m_msUpdator("Trk::MultipleScatteringUpdator/AtlasMultipleScatteringUpdator"),
  m_conversionTool("iFatras::McConversionCreator/FatrasConversionCreator"),
  m_processCode(3),
  m_hadInt(true),
  //m_hadIntProcessor("iFatras::HadIntProcessorParametric/FatrasHadIntProcessorParametric"),
  m_hadIntProcessor("iFatras::G4HadIntProcessor/FatrasG4HadIntProcessor"),
  m_particleDecayer(),
  m_minimumMomentum(50.*CLHEP::MeV),
  m_minimumBremPhotonMomentum(50.*CLHEP::MeV),
  m_parametricScattering(false),
  m_use_msUpdator(false),
  m_uniformHertzDipoleAngle(false),
  m_referenceMaterial(true),
  m_bendingCorrection(false),
  m_rndGenSvc("AtDSFMTGenSvc", n),
  m_randomEngine(0),
  m_randomEngineName("FatrasRnd"),
  m_recordedBremPhotons(0),
  m_currentSample(-1),
  m_recordEnergyDeposition(false),
  m_layerIndexCaloSampleMapName("LayerIndexCaloSampleMap"),
  m_layerIndexCaloSampleMap(0),
  m_trackingGeometry(0),
  m_trackingGeometrySvc("ISF_FatrasTrackingGeometrySvc", n),
  m_trackingGeometryName("ISF_FatrasTrackingGeometry"),
  m_projectionFactor(sqrt(2.)),
  m_validationMode(false),
  m_validationTool(""),
  m_validationTreeName("FatrasMaterialEffects"),
  m_validationTreeDescription("Validation output from the McMaterialEffectsUpdator"),
  m_validationTreeFolder("/val/FatrasSimulationMaterial"),
  m_validationTree(0),
  m_layerIndex(0),
  m_tInX0(0.),
  m_thetaMSproj(0.),
  m_thetaMSphi(0.),
  m_thetaMStheta(0.),
  m_deltaP(0.),
  m_deltaPsigma(0.),
  m_bremValidation(false),
  m_bremValidationTreeName("FatrasBremPhotons"),
  m_bremValidationTreeDescription("Validation output from the McMaterialEffectsUpdator"),
  m_bremValidationTreeFolder("/val/FatrasBremPhotons"),
  m_bremValidationTree(0),
  m_bremPointX(0.),
  m_bremPointY(0.),
  m_bremPointR(0.),
  m_bremPointZ(0.),
  m_bremMotherEnergy(0.),
  m_bremPhotonEnergy(0.),
  m_bremPhotonAngle(0.),
  m_edValidation(false),
  m_edValidationTreeName("FatrasEnergyInCaloDeposit"),      
  m_edValidationTreeDescription("Validation output from the McMaterialEffectUpdator"),
  m_edValidationTreeFolder("/val/FatrasEnergyInCaloDeposit"),    
  m_edValidationTree(0),                    
  m_edLayerIntersectX(0.),         
  m_edLayerIntersectY(0.),         
  m_edLayerIntersectZ(0.),         
  m_edLayerIntersectR(0.),         
  m_edLayerEnergyDeposit(0.),     
  m_edLayerSample(0),
  m_oneOverThree(1./3.),
  m_particleBroker("ISF_ParticleParticleBroker", n),
  m_truthRecordSvc("ISF_TruthRecordSvc", n),
  m_layer(0)
{
      declareInterface<ITimedMatEffUpdator>(this);
      // the tool parameters -----------------------------------------------------
      declareProperty("EnergyLoss"                          , m_eLoss);
      declareProperty("EnergyLossUpdator"                   , m_eLossUpdator);
      declareProperty("MultipleScattering"                  , m_ms);
      declareProperty("MultipleScatteringUpdator"           , m_msUpdator);
      declareProperty("ProcessSamplingTool"                 , m_samplingTool);
      declareProperty("PhotonConversionTool"                , m_conversionTool);
      // tool handle for the particle decayer
      declareProperty( "ParticleDecayHelper"                , m_particleDecayer      );
      // MC Truth Properties
      declareProperty("BremProcessCode"                     , m_processCode, "MCTruth Physics Process Code");
      // the steering --------------------------------------------------------------
      declareProperty("HadronicInteraction"                 , m_hadInt);
      declareProperty("HadronicInteractionProcessor"        , m_hadIntProcessor);
      // the steering --------------------------------------------------------------
      declareProperty("ParametericScattering"               , m_parametricScattering);
      declareProperty("UseMultipleScatteringUpdator"        , m_use_msUpdator);
      declareProperty("MomentumCut"                         , m_minimumMomentum);
      declareProperty("MinimumBremPhotonMomentum"           , m_minimumBremPhotonMomentum);
      // the steering --------------------------------------------------------------
      declareProperty("ReferenceMaterial"                   , m_referenceMaterial);
      declareProperty("BendingCorrection"                   , m_bendingCorrection);
      // validation mode ------------------------------------------------------------
      declareProperty("RecordEnergyDeposition"              , m_recordEnergyDeposition);
      declareProperty("LayerIndexCaloSampleMap"             , m_layerIndexCaloSampleMapName);      
      // validation mode ------------------------------------------------------------
      declareProperty("ValidationMode"                      , m_validationMode);
      declareProperty("PhysicsValidationTool"               , m_validationTool);
      declareProperty("BremPhotonValidation"                , m_bremValidation);
      declareProperty("EnergyDepositValidation"             , m_edValidation);      
      // TrackingGeometry Service      
      declareProperty("TrackingGeometrySvc",                  m_trackingGeometrySvc);     
      declareProperty("RandomNumberService"                 , m_rndGenSvc               , "Random number generator");
      declareProperty("RandomStreamName"                    , m_randomEngineName        , "Name of the random number stream");

      // ISF Services and Tools
      declareProperty("ParticleBroker"                      , m_particleBroker        , "ISF Particle Broker Svc");
      declareProperty("TruthRecordSvc"                      , m_truthRecordSvc        , "ISF Particle Truth Svc");      
}

// destructor
iFatras::McMaterialEffectsUpdator::~McMaterialEffectsUpdator()
{}

// Athena standard methods
// initialize
StatusCode iFatras::McMaterialEffectsUpdator::initialize()
{

    ATH_MSG_INFO( "initialize()" );

    // retrieve the process sampling tool
    if (m_samplingTool.retrieve().isFailure()){
      ATH_MSG_FATAL( "Could not retrieve " << m_samplingTool );
      return StatusCode::FAILURE;
    } else
      ATH_MSG_VERBOSE( "Successfully retrieved " << m_samplingTool );
           
    // retrieve the energy loss updator
    if (m_eLoss){
      if (m_eLossUpdator.retrieve().isFailure()){
          ATH_MSG_FATAL( "Could not retrieve " << m_eLossUpdator );
          return StatusCode::FAILURE;
      } else
         ATH_MSG_VERBOSE( "Successfully retrieved " << m_eLossUpdator );
    }
    
    // retrieve the multiple scattering updator tool
    if (m_ms && m_use_msUpdator){
      if (m_msUpdator.retrieve().isFailure()){
          ATH_MSG_FATAL( "Could not retrieve " << m_msUpdator );
          return StatusCode::FAILURE;
      } else
          ATH_MSG_VERBOSE( "Successfully retrieved " << m_msUpdator );
    }

    // retrieve the photon conversion tool
    if (m_conversionTool.retrieve().isFailure()){
      ATH_MSG_FATAL( "Could not retrieve " << m_conversionTool );
      return StatusCode::FAILURE;
    } else
      ATH_MSG_VERBOSE( "Successfully retrieved " << m_conversionTool );

    // retrieve the hadronic interaction tool
    if (m_hadInt){
      if (m_hadIntProcessor.retrieve().isFailure()){
          ATH_MSG_FATAL( "Could not retrieve " << m_hadIntProcessor );
          return StatusCode::FAILURE;
      } else
          ATH_MSG_VERBOSE( "Successfully retrieved " << m_hadIntProcessor );                
    }
    
    // get the random generator serice
     if (m_rndGenSvc.retrieve().isFailure()){
          ATH_MSG_FATAL( "Could not retrieve " << m_rndGenSvc );
          return StatusCode::FAILURE;
     } else
          ATH_MSG_VERBOSE( "Successfully retrieved " << m_rndGenSvc );
    
    //Get own engine with own seeds:
    m_randomEngine = m_rndGenSvc->GetEngine(m_randomEngineName);
    if (!m_randomEngine) {
        ATH_MSG_FATAL( "Could not get random engine '" << m_randomEngineName << "'" );
        return StatusCode::FAILURE;
    }    
         
    // get the tracking geometry for layer lookup     
    // get the TrackingGeometrySvc
    if (m_trackingGeometrySvc.retrieve().isSuccess()){
        ATH_MSG_INFO( "Successfully retrieved " << m_trackingGeometrySvc );
        m_trackingGeometryName = m_trackingGeometrySvc->trackingGeometryName();
    } else {
        ATH_MSG_WARNING( "Couldn't retrieve " << m_trackingGeometrySvc << ". " );
        ATH_MSG_WARNING( " -> Trying to retrieve default '" << m_trackingGeometryName << "' from DetectorStore." );
    }         

    // Particle decayer
    if (m_particleDecayer.retrieve().isFailure()){
      ATH_MSG_FATAL( "Could not retrieve " << m_particleDecayer );
      return StatusCode::FAILURE;
    } else
      ATH_MSG_VERBOSE( "Successfully retrieved " << m_particleDecayer );                
    
    ITHistSvc* tHistSvc = 0;
    if (m_validationMode || m_bremValidation || m_edValidation){
       // now register the Tree
        if (service("THistSvc",tHistSvc).isFailure()) 
           ATH_MSG_ERROR( "initialize() Could not find Hist Service -> Switching ValidationMode Off !" ); 
     }

    // ISF Services
    if (m_particleBroker.retrieve().isFailure()){
        ATH_MSG_FATAL( "Could not retrieve " << m_particleBroker );
        return StatusCode::FAILURE;
    }
    if (m_truthRecordSvc.retrieve().isFailure()){
        ATH_MSG_FATAL( "Could not retrieve " << m_truthRecordSvc );
        return StatusCode::FAILURE;
    }

    // the validation setup -------------------------------- PART 1: General ----------------------------------

    if (m_validationMode){

      // retrieve the physics validation tool
      if (m_validationTool.retrieve().isFailure()){
	ATH_MSG_FATAL( "Could not retrieve " << m_validationTool );
	return StatusCode::FAILURE;
      } else
	ATH_MSG_VERBOSE( "Successfully retrieved " << m_validationTool );
      
      ATH_MSG_VERBOSE( "Booking material validation TTree ... " );
      
      // create the new Tree
      m_validationTree = new TTree(m_validationTreeName.c_str(), m_validationTreeDescription.c_str());

      // counter for material effects
      m_validationTree->Branch("LayerIndex"      ,  &m_layerIndex,   "layerIdx/I");
      m_validationTree->Branch("PathInX0"        ,  &m_tInX0,        "tInX0/F");
      m_validationTree->Branch("ThetaMS"         ,  &m_thetaMSproj,  "thetaMS/F");
      m_validationTree->Branch("ThetaMSphi"      ,  &m_thetaMSphi,   "thetaMSphi/F");
      m_validationTree->Branch("ThetaMStheta"    ,  &m_thetaMStheta, "thetaMStheta/F");
      m_validationTree->Branch("DeltaP"          ,  &m_deltaP,       "deltaP/F");
      m_validationTree->Branch("DeltaPsigma"     ,  &m_deltaPsigma,  "deltaPsigma/F");
      

      if ((tHistSvc->regTree(m_validationTreeFolder, m_validationTree)).isFailure()) {
	ATH_MSG_ERROR( "initialize() Could not register the validation Tree -> Switching ValidationMode Off !" );
	delete m_validationTree; m_validationTree = 0;
      } else
	ATH_MSG_INFO( "TTree for MaterialEffects validation booked." );

    } 
    // the validation setup -------------------------------- PART 2: Brem Photons -----------------------------
    if (m_bremValidation){

        ATH_MSG_VERBOSE( "Booking bremstrahlung validation TTree ... " );

        // create the new Tree
        m_bremValidationTree = new TTree(m_bremValidationTreeName.c_str(), m_bremValidationTreeDescription.c_str());

           // counter for bremstrahlung
           m_bremValidationTree->Branch("BremPositionX"    ,  &m_bremPointX, "bremX/F");
           m_bremValidationTree->Branch("BremPositionY"    ,  &m_bremPointY, "bremY/F");
           m_bremValidationTree->Branch("BremPositionR"    ,  &m_bremPointR, "bremR/F");
           m_bremValidationTree->Branch("BremPositionZ"    ,  &m_bremPointZ, "bremZ/F");
           m_bremValidationTree->Branch("BremMotherEnergy" ,  &m_bremMotherEnergy,   "bremMotherE/F");
           m_bremValidationTree->Branch("BremPhotonEnergy" ,  &m_bremPhotonEnergy,   "bremPhotonE/F");
           m_bremValidationTree->Branch("BremPhotonAngle"  ,  &m_bremPhotonAngle,    "bremPhotondA/F");
    
        if ((tHistSvc->regTree(m_bremValidationTreeFolder, m_bremValidationTree)).isFailure()) {
           ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !" );
           delete m_bremValidationTree; m_bremValidationTree = 0;
        } else
           ATH_MSG_INFO( "TTree for Bremsstrahlung validation booked." );

    } // ------------- end of validation mode -----------------------------------------------------------------
   
    // the validation setup -------------------------------- PART 3: Brem Photons -----------------------------
    if (m_edValidation){

        ATH_MSG_VERBOSE( "Booking Energy deposition validation TTree ... " );

        // create the new Tree
        m_edValidationTree = new TTree(m_edValidationTreeName.c_str(), m_edValidationTreeDescription.c_str());

           // counter for boundary surfaces
           m_edValidationTree->Branch("EdepositPositionX"    ,  &m_edLayerIntersectX,    "edX/F");
           m_edValidationTree->Branch("EdepositPositionY"    ,  &m_edLayerIntersectY,    "edY/F");
           m_edValidationTree->Branch("EdepositPositionR"    ,  &m_edLayerIntersectR,    "edR/F");
           m_edValidationTree->Branch("EdepositPositionZ"    ,  &m_edLayerIntersectZ,    "edZ/F");
           m_edValidationTree->Branch("Edeposit"             ,  &m_edLayerEnergyDeposit, "ed/F");
           m_edValidationTree->Branch("EdepositLayerSample"  ,  &m_edLayerSample,        "edLayerSample/F");
    
        if ((tHistSvc->regTree(m_edValidationTreeFolder, m_edValidationTree)).isFailure()) {
           ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !" );
           delete m_edValidationTree; m_edValidationTree = 0;
        } else
           ATH_MSG_INFO( "TTree for Energy deposition validation booked." );



    } // ------------- end of validation mode -----------------------------------------------------------------

    return StatusCode::SUCCESS;
}

// finalize
StatusCode iFatras::McMaterialEffectsUpdator::finalize()
{

    ATH_MSG_INFO( " ---------- Statistics output -------------------------- " );
    ATH_MSG_INFO( "                     Minimum energy cut for brem photons : " <<   m_minimumBremPhotonMomentum  );
    ATH_MSG_INFO( "                     Brem photons (above cut, recorded)  : " <<   m_recordedBremPhotons        );

    ATH_MSG_INFO( "finalize() successful" );
    return StatusCode::SUCCESS;
}



const Trk::TrackParameters* iFatras::McMaterialEffectsUpdator::update(const Trk::TrackParameters* parm,
								      const Trk::Layer& lay,
								      Trk::TimeLimit& timeLim, Trk::PathLimit& pathLim,
								      Trk::GeometrySignature /*geoID*/,
								      Trk::PropDirection dir,
								      Trk::ParticleHypothesis particle) const
{   

  // the call should work for particles created on the layer, too
  // + additional parameter for material scaling

  double traversedMatFraction = 0.;

  m_layer = &lay;

  // get parent particle
  const ISF::ISFParticle *parent = ISF::ParticleClipboard::getInstance().getParticle();
  // something is seriously wrong if there is no parent particle
  assert(parent);

  m_isp = parent;

  // get the material properties 
  m_matProp =  0;

  if (m_referenceMaterial){
    // very first go : get the Surface
    const Trk::Surface &parmSurface = parm->associatedSurface();
    // only go on if the parmSurface has an associatedDetectorElement
    if (parmSurface.associatedDetectorElement()){     
      // first get the LayerMaterialProperties
      const Trk::LayerMaterialProperties* layerMaterial = m_layer->layerMaterialProperties();
      // assign the material properties 
      m_matProp = layerMaterial ? layerMaterial->fullMaterial( parm->position() ) : 0;
    }
  }


  return updateInLay(parent,parm,traversedMatFraction,timeLim, pathLim,dir, particle);

} 

const Trk::TrackParameters* iFatras::McMaterialEffectsUpdator::updateInLay(const ISF::ISFParticle* isp,
									const Trk::TrackParameters* parm,
									double& matFraction,
									Trk::TimeLimit& timeLim, Trk::PathLimit& pathLim,
									Trk::PropDirection dir,
									Trk::ParticleHypothesis particle) const
{   
  //double pathCorrection                =  0.; 

  //std::cout << "update on layer: processing isp:position within layer: "<<isp <<","<< isp->generation() <<","<< particle <<std::endl;
  //std::cout << "position: global, within layer:"<<parm->position()<<","<< matFraction<< std::endl;
  
  //if (m_referenceMaterial && m_matProp) pathCorrection = 1./fabs(parm->associatedSurface()->normal().dot(parm->momentum().unit()));

  // recalculate if missing
  m_matProp = m_matProp ? m_matProp : m_layer->fullUpdateMaterialProperties(*parm);
  double pathCorrection = m_layer->surfaceRepresentation().normal().dot(parm->momentum()) !=0 ?
    fabs(m_layer->surfaceRepresentation().pathCorrection(parm->position(),dir*(parm->momentum()))) : 1.;
    
  // check if a bending correction has to be applied
  if (m_bendingCorrection) {
    ATH_MSG_WARNING("BendingCorrection not implemented!! (McMaterialEffectsUpdator.cxx)");
    //const Trk::TrackingVolume* enclosingVolume = m_bendingCorrection ? m_layer->enclosingTrackingVolume() : 0;
    //if (enclosingVolume && enclosingVolume->bendingCorrector() ) { 
    //  double bendingCorrection = enclosingVolume->bendingCorrector()->bendingCorrection(*parm);
    //  pathCorrection *= bendingCorrection; 
    //}
  }

  //--------------------------------------------------------------------------------------------------  
  if (msgLvl(MSG::VERBOSE) && int(dir)){
    const Trk::TrackingVolume* enclosingVolume = m_layer->enclosingTrackingVolume();
    std::string volumeName = enclosingVolume ? enclosingVolume->volumeName() : "Unknown";
    double layerR = m_layer->surfaceRepresentation().bounds().r();
    double layerZ = m_layer->surfaceRepresentation().center().z();   
    ATH_MSG_VERBOSE( "  [M] Material effects update() on layer with index "<< m_layer->layerIndex() );
    ATH_MSG_VERBOSE( "                        -> path/X0 on layer [r/z] = [ " << layerR << " / " << layerZ << " ] :"     
        << pathCorrection*m_matProp->thicknessInX0() );
    ATH_MSG_VERBOSE( "                        -> path correctin factor  =   " << pathCorrection );
  }
  //--------------------------------------------------------------------------------------------------

  // validation mode ---------------------------------------------------------------
  if (m_validationMode) {
    m_tInX0           = (1-matFraction)*pathCorrection * m_matProp->thicknessInX0();
    m_layerIndex      = m_layer->layerIndex().value();
    m_validationTree->Fill();
  }
  // -------------------------------------------------------------------------------
  
  // prepare material collection
  const Trk::MaterialProperties* m_extMatProp = 0;
  double dInL0 = 0.;
  if (m_matProp) {
    m_extMatProp = dynamic_cast<const Trk::MaterialProperties*>(m_matProp);
    dInL0 = m_extMatProp ? (1-matFraction)*pathCorrection*m_extMatProp->thicknessInL0() : 
      (1-matFraction)*pathCorrection*m_matProp->thicknessInX0()/0.37/m_matProp->averageZ();
  }
  
  // figure out if particle stopped in the layer and recalculate path limit
  int iStatus = 0;
  double dX0 = (1.-matFraction)*pathCorrection*m_matProp->thicknessInX0();

  if (!m_matProp || particle==Trk::geantino || particle==Trk::nonInteracting || dX0==0.) {   
    // non-interacting - pass them back
    pathLim.updateMat(dX0,m_matProp->averageZ(),dInL0);

    // register particle if not in the stack already
    if (parm && isp!=m_isp ) {
      ISF::ISFParticle* regisp=new ISF::ISFParticle(isp->position(),parm->momentum(),isp->mass(),isp->charge(),
						  isp->pdgCode(),isp->timeStamp(),*m_isp,isp->barcode());
      // add presampled process info 
      if (isp->getUserInformation() && isp->getUserInformation()->materialLimit()) {
	ISF::MaterialPathInfo* matLim = isp->getUserInformation()->materialLimit();
	ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
	validInfo->setMaterialLimit(matLim->process,matLim->dMax,matLim->process==121 ? pathLim.l0Collected : pathLim.x0Collected);
	regisp->setUserInformation(validInfo);
      }
      // in the validation mode, add process info
      if (m_validationMode) {
	ISF::ParticleUserInformation* validInfo = regisp->getUserInformation();
        if (!validInfo) {
	  validInfo = new ISF::ParticleUserInformation();
	  regisp->setUserInformation(validInfo);
	}
	if (isp->getUserInformation()) validInfo->setProcess(isp->getUserInformation()->process());
	else validInfo->setProcess(-2);        // signal problem in the validation chain
	if (isp->getUserInformation()) validInfo->setGeneration(isp->getUserInformation()->generation());
	else validInfo->setGeneration(-1);        // signal problem in the validation chain
	regisp->setUserInformation(validInfo);
      }
      // register TruthIncident
      ISF::ISFParticleVector children(1, regisp);
      ISF::ISFTruthIncident truth( const_cast<ISF::ISFParticle&>(*m_isp),
                                   children,
                                   (isp->getUserInformation()) ? isp->getUserInformation()->process() : -2 /*!< @TODO fix non-static */,
                                   m_isp->nextGeoID(),
                                   ISF::fKillsPrimary );
      m_truthRecordSvc->registerTruthIncident( truth);
      //Making sure we get some correct truth info from parent if needed before pushing into the particle broker
      if (!regisp->getTruthBinding()) {
	regisp->setTruthBinding(new ISF::TruthBinding(*isp->getTruthBinding()));
      }
      m_particleBroker->push(regisp, m_isp);
    }
    if (isp!=m_isp) { delete isp; delete parm; }
    return parm; 
  } else {
    if ( pathLim.x0Max>0 && pathLim.process<100 && pathLim.x0Collected+dX0>= pathLim.x0Max) {      // elmg. interaction
      float x0rem = pathLim.x0Max - pathLim.x0Collected;
      dInL0 *= x0rem>0 ? x0rem/dX0 : 1.; 
      if ( x0rem>0 ) dX0 = x0rem;
      iStatus = 1; 
    } else if ( pathLim.x0Max>0 && m_extMatProp && pathLim.process>100 && pathLim.l0Collected+dInL0 >= pathLim.x0Max )   {     // hadronic interaction
      float l0rem = pathLim.x0Max - pathLim.l0Collected;
      dX0 *= l0rem>0 ? l0rem/dInL0 : 1.;
      if ( l0rem>0 ) dInL0 = l0rem;
      iStatus = 2; 
    }
    pathLim.updateMat(dX0,m_matProp->averageZ(),dInL0);
  }
 
  //std::cout <<"particle status on layer:"<<iStatus<<std::endl; 
 
  // get the kinematics
  double p    = parm->momentum().mag();
  //double m    = m_particleMasses.mass[particle];
  //double E    = sqrt(p*p+m*m);

  // radiation and ionization preceed the presampled interaction (if any) 

  // the updatedParameters - first a deep copy
  AmgVector(5) updatedParameters(parm->parameters());  
  const Trk::TrackParameters* updated = 0;

  if ( m_eLoss && particle==Trk::electron && dX0>0.) {

    double matTot =  (1-matFraction)*pathCorrection*m_matProp->thicknessInX0();
    Amg::Vector3D edir = parm->momentum().unit(); 
    radiate(isp,updatedParameters,parm->position(),edir,timeLim, dX0, matFraction, matTot, dir, particle); 

    if (1./fabs(updatedParameters[4]) < m_minimumMomentum ) {
      if (isp!=m_isp) delete isp;
      return 0;
    }
  } else {
    matFraction += dX0/pathCorrection/m_matProp->thicknessInX0();  
  }

  if ( isp->charge()!=0 && dX0>0.) {
    if ( m_eLoss ) ionize(*parm, updatedParameters, dX0, dir, particle );

    if ( m_ms  && m_matProp->thicknessInX0()>0.) {

      double sigmaMSproj = (m_use_msUpdator && m_msUpdator ) ?
	sqrt(m_msUpdator->sigmaSquare(*m_matProp, p, dX0/m_matProp->thicknessInX0(), particle)) : msSigma(dX0,p,particle);
 
      multipleScatteringUpdate(*parm,updatedParameters,sigmaMSproj);
    }
    
    // use the manipulator to update the track parameters -------> get rid of 0!
    updated = parm->associatedSurface().createTrackParameters(updatedParameters[0],
							      updatedParameters[1],
							      updatedParameters[2],
							      updatedParameters[3],
							      updatedParameters[4],
							      0);

    if (isp!=m_isp) delete parm;
    parm = updated;
       
    if (parm->momentum().mag() < m_minimumMomentum ) {
      if (isp!=m_isp) { delete isp; delete parm; }
      else delete updated;
      return 0;
    }
  }

  if ( iStatus==1 || iStatus==2 ) {   // interaction
    
    ISF::ISFParticleVector childs = iStatus==1 ? interactLay(isp,timeLim.time,*parm,particle,pathLim.process) :
      m_hadIntProcessor->doHadIntOnLayer(isp, timeLim.time, parm->position(), parm->momentum(),
					 ( m_extMatProp ? &m_extMatProp->material() : 0), particle);

    // save info for locally created particles
    if (m_validationMode && childs.size()>0 && isp!=m_isp) {
      ATH_MSG_VERBOSE( "  saving interaction info for locally produced particle " << isp->pdgCode() );
      m_validationTool->saveISFParticleInfo(*isp,pathLim.process,parm,timeLim.time,pathLim.x0Max); 
    }

    // loop over childrens and decide about their fate

    for (unsigned ic=0; ic<childs.size(); ic++) {
      double mom = childs[ic]->momentum().mag();

      if (mom<m_minimumMomentum) continue;
      Trk::ParticleHypothesis pHypothesis = m_pdgToParticleHypothesis.convert(childs[ic]->pdgCode(),childs[ic]->charge());
      const Trk::CurvilinearParameters*  cparm=new Trk::CurvilinearParameters(childs[ic]->position(),childs[ic]->momentum(),childs[ic]->charge());
      Trk::PathLimit pLim = m_samplingTool->sampleProcess(mom,childs[ic]->charge(),pHypothesis);

      // TODO sample decays and save the material collection & path limits at the exit from the layer (ISFFatrasParticle ?)

      // material fraction : flip if direction of propagation changed
      double ci = m_layer->surfaceRepresentation().normal().dot( parm->momentum().unit() );
      double co = m_layer->surfaceRepresentation().normal().dot( childs[ic]->momentum().unit() );
      double remMat =  ci*co <0 ? (1-matFraction)    : matFraction;

      // loop back      
      const Trk::TrackParameters* uPar = updateInLay(childs[ic],cparm,remMat,timeLim, pLim, dir, pHypothesis);
      if (uPar) ATH_MSG_VERBOSE( "Check this: parameters should be dummy here " << isp->pdgCode()<<","<<uPar->position() );

    }
    
    if (childs.size()>0) { // assume that interaction processing failed if no children
      if (isp!=m_isp) { delete isp; delete parm; }
      else delete updated;   // non-updated parameters registered with the extrapolator
      return 0;
    }
  }

  // register particle if not in the stack already
  if (parm && isp!=m_isp ) {
    ISF::ISFParticle* regisp=new ISF::ISFParticle(isp->position(),parm->momentum(),isp->mass(),isp->charge(),
						  isp->pdgCode(),isp->timeStamp(),*m_isp,isp->barcode());
    // add presampled process info 
    if (isp->getUserInformation() && isp->getUserInformation()->materialLimit()) {
      ISF::MaterialPathInfo* matLim = isp->getUserInformation()->materialLimit();
      ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
      validInfo->setMaterialLimit(matLim->process,matLim->dMax,matLim->process==121 ? pathLim.l0Collected : pathLim.x0Collected);
      regisp->setUserInformation(validInfo);
    }
    // in the validation mode, add process info
    if (m_validationMode) {
      ISF::ParticleUserInformation* validInfo = regisp->getUserInformation();
      if (!validInfo){
	validInfo = new ISF::ParticleUserInformation();
	regisp->setUserInformation(validInfo);
      }
      if (isp->getUserInformation()) validInfo->setProcess(isp->getUserInformation()->process());
      else validInfo->setProcess(-2);        // signal problem in the validation chain
      if (isp->getUserInformation()) validInfo->setGeneration(isp->getUserInformation()->generation());
      else validInfo->setGeneration(-1);        // signal problem in the validation chain
    }
    //Making sure we get some correct truth info from parent if needed before pushing into the particle broker
    if (!regisp->getTruthBinding()) {
	regisp->setTruthBinding(new ISF::TruthBinding(*isp->getTruthBinding()));
    }
    m_particleBroker->push(regisp, m_isp);
  }

  if (isp!=m_isp) { delete isp; delete parm; }

  return parm; 
  
}

void iFatras::McMaterialEffectsUpdator::ionize(const Trk::TrackParameters& parm, AmgVector(5)& updatedPar,double dInX0,Trk::PropDirection /*dir*/,Trk::ParticleHypothesis particle ) const {

  double p = parm.momentum().mag();                                                      
  double m    = m_particleMasses.mass[particle];
  double E    = sqrt(p*p+m*m);

  // the following formulas are imported from STEP
  // preparation of kinetic constants

  double me    = m_particleMasses.mass[Trk::electron];
  double mfrac = me/m;
  double beta  = p/E;
  double gamma = E/m;
  double Ionization = 0.;

  //Ionization - Bethe-Bloch
  double I = 16.e-6 * std::pow(m_matProp->averageZ(),0.9); //16 eV * Z**0.9 - bring to MeV

  //K/A*Z = 0.5 * 30.7075MeV/(g/mm2) * Z/A * rho[g/mm3]  / scale to mm by this
  double kaz = 0.5*30.7075*m_matProp->zOverAtimesRho();

  if (particle == Trk::electron){
    // for electrons use slightly different BetheBloch adaption
    // see Stampfer, et al, "Track Fitting With Energy Loss", Comp. Pyhs. Comm. 79 (1994), 157-164
    Ionization = -kaz*(2.*log(2.*me/I)+3.*log(gamma) - 1.95);
  }
  else {
    double eta2 = beta*gamma; eta2 *= eta2;
    // density effect, only valid for high energies (gamma > 10 -> p > 1GeV for muons)
    double delta = 0.;
    if (gamma > 10.) {
      double eplasma = 28.816e-6 * sqrt(1000.*m_matProp->zOverAtimesRho());
      delta = 2.*log(eplasma/I) + log(eta2) - 1.;
    }
    // tmax - cut off energy
    double tMax = 2.*eta2*me/(1.+2.*gamma*mfrac+mfrac*mfrac);
    // divide by beta^2 for non-electrons
    kaz /= beta*beta;
    Ionization = -kaz*(log(2.*me*eta2*tMax/(I*I)) - 2.*(beta*beta) - delta);
  }
 
  double energyLoss = Ionization*dInX0*m_matProp->x0();   
  double newP = sqrt(fmax(0.,(E+energyLoss)*(E+energyLoss)-m*m));       

  newP = newP <= 0.5*m_minimumMomentum ? 0.5*m_minimumMomentum : newP;      // arbitrary regularization;
  
  (updatedPar)[Trk::qOverP] = parm.charge()/newP; 

  /*
  // get the momentum change plus the according sigma
  Trk::EnergyLoss* sampledEnergyLoss = m_eLossUpdator->energyLoss(*m_matProp, p, dInX0/m_matProp->thicknessInX0(), dir, particle); 

  if (sampledEnergyLoss ) {

    double energyLoss = sampledEnergyLoss->deltaE();   
    double newP = sqrt(fmax(0.,(E+energyLoss)*(E+energyLoss)-m*m));       
    // the deltaP and recording
    //double deltaP = newP - p;
    //recordEnergyDeposit(deltaP, p, particle) return ;

    delete sampledEnergyLoss;

    newP = newP <= 10. ? 10. : newP;      // arbitrary regularization;

    (updatedPar)[Trk::qOverP] = parm.charge()/newP; 
  }
  */
}


// radiative effects
void iFatras::McMaterialEffectsUpdator::radiate(const ISF::ISFParticle* parent, AmgVector(5)& parm ,
						const Amg::Vector3D& pos, Amg::Vector3D& dir,
						Trk::TimeLimit timeLim, double pathLim, double& matFraction, double matTot, 
						Trk::PropDirection pdir, Trk::ParticleHypothesis particle) const {
 
  // sample energy loss and free path independently
  double path = 0.;
  double p = 1./ fabs(parm[Trk::qOverP]);

  //std::cout <<"radiating from gen,barcode" <<parent->generation()<<","<<parent->barcode()<<", p="<<p <<",at position "<< pos << std::endl;

  while ( path < pathLim && p>m_minimumMomentum ) {

    double rndx = CLHEP::RandFlat::shoot(m_randomEngine);

    // sample visible fraction of the mother momentum taken according to 1/f  
 
    double eps = fmin(10.,m_minimumMomentum)/p;

    double z = pow(eps,pow(rndx,exp(1.1)));          // adjustment here ? 

    // convert into scaling factor for mother momentum
    z = (1.- z); 

    // turn into path   
    double dx = -log(z)*0.65;     // adjust for mean of exp(-x)  
 
    // resolve the case when there is not enough material left in the layer
    if ( path+dx > pathLim ) {
      double rndp = CLHEP::RandFlat::shoot(m_randomEngine);
      if (rndp > (pathLim-path)/dx){        
	(parm)[Trk::qOverP] = (parm)[Trk::qOverP]>0 ? 1/p : -1/p; 
        path = pathLim;
        matFraction = 1.;
	return;                   // radiation loop ended         
      } 
      path += dx*rndp;
      matFraction += dx*rndp/matTot;
      
    } else { 
      path+=dx;
      matFraction += dx/matTot; 
    }

    /*
    // sample free path
    double dx   = -log(rndx)/2.;               // can be adjusted here
    //double dx   = log(1./rndx)/2.;               // can be adjusted here
    // there may not be enough material to proceed - sample
    //std::cout <<"brem in layer:"<< path<<"," << dx <<","<< pathLim<< std::endl;
    if ( path+dx > pathLim ) {
      double rndp = CLHEP::RandFlat::shoot(m_randomEngine);
      if (rndp > (pathLim-path)/dx){
        (parm)[Trk::qOverP] = (parm)[Trk::qOverP]>0 ? 1/p : -1/p; 
        path = pathLim;
        matFraction = 1.;
	return;            // radiation loop ended
      } 
      path += dx*rndp;
      matFraction += dx*rndp/matTot; 
    } else { 
      path+=dx;
      matFraction += dx/matTot; 
    }
    
    // radiate
    double z = exp(-dx);
    // resample according to 1/f
    //double rr = CLHEP::RandFlat::shoot(m_randomEngine);
    //if (rr*fxx > 1/z ) z=1./rr/fxx ;

    */

    // additional adjustement
    //double rndp = CLHEP::RandFlat::shoot(m_randomEngine);
    //if ( p*(1-z) > m_minimumBremPhotonMomentum && rndp>0.5 ) {
    if ( p*(1-z) > m_minimumBremPhotonMomentum ) {

      double deltaP = (1-z)*p;
      recordBremPhotonLay(parent,timeLim,p,deltaP, pos,dir,matFraction,pdir,particle);
      // recordEnergyDeposit(-deltaP,p,particle); 
      p *=z ;
    }    

  }

  (parm)[Trk::qOverP] = (parm)[Trk::qOverP] > 0 ? 1/p : -1./p; 
  (parm)[Trk::theta]  = dir.theta(); 
  (parm)[Trk::phi]    = dir.phi(); 
  return; 
}




/*

  // Hadronic Interaction section ----------------------------------------------------------------------------------------------
  // ST add hadronic interaction for secondaries, cut for momentum! 
  // if ( particle == Trk::pion && m_hadInt && m_hadIntProcessor->hadronicInteraction(*parm, p, E,*m_matProp, pathCorrection,particle) ) {
  //   ATH_MSG_VERBOSE( "  [+] Hadronic interaction killed initial hadron ... stop simulation, tackle childs." );
  //   return 0;
  // }
 
  // process energyloss
  if (m_eLoss){
    // get the momentum change plus the according sigma
    Trk::EnergyLoss* sampledEnergyLoss = m_eLossUpdator->energyLoss(*m_matProp, p, pathCorrection, dir, particle);     
    if (sampledEnergyLoss && !handleEnergyLoss(timeLim.time,*parm,updatedParameters, sampledEnergyLoss, lay, p, E, m, particle)) 
         return 0;
      
  }
  // process multiple scattering
  if (m_ms){
    // both the highland and the double gaussian approach describe the projected scattering angle
    double sigmaMSproj   = (m_msUpdator) ? sqrt(m_msUpdator->sigmaSquare(*m_matProp, p, pathCorrection, particle)) : 0.;
    // do the update
    multipleScatteringUpdate(*parm,*updatedParameters,sigmaMSproj);
  }

  // validation mode ---------------------------------------------------------------
  if (m_validationMode) {
    m_tInX0           = pathCorrection * m_matProp->thicknessInX0();
    m_layerIndex      = lay.layerIndex().value();
    m_validationTree->Fill();
  }
  // -------------------------------------------------------------------------------

  // use the manipulator to update the track parameters -------> get rid of 0!
  parm = Trk::TrkParametersManipulator::manipulateParameters(parm,updatedParameters,0);
  return parm;
}

*/

// actual update mehtod
const Trk::TrackParameters* iFatras::McMaterialEffectsUpdator::update(double time,
								      const Trk::TrackParameters& parm,
                                                               const Trk::MaterialProperties& matprop,
                                                               double pathCorrection,
                                                               Trk::PropDirection dir,
                                                               Trk::ParticleHypothesis particle,
                                                               Trk::MaterialUpdateMode) const
{    
  // no material properties - pass them back
  if (particle==Trk::geantino || (!m_ms && !m_eLoss) ) return 0;
  
  // get the kinematics
  double p    = parm.momentum().mag();
  double m    = m_particleMasses.mass[particle];
  double E    = sqrt(p*p+m*m);

  // Hadronic Interaction section ----------------------------------------------------------------------------------------------
  // ST add hadronic interaction for secondaries 
  if ( particle == Trk::pion &&  m_hadInt && m_hadIntProcessor->hadronicInteraction(parm.position(), parm.momentum(), p, E, parm.charge(), matprop, pathCorrection, particle)){
      ATH_MSG_VERBOSE( "  [+] Hadronic interaction killed initial hadron ... stop simulation, tackle childs." );
      return 0;    
  } 
    
  if ( particle == Trk::neutron || particle == Trk::pi0 || particle == Trk::k0) return parm.clone();

  // the updatedParameters - first a copy
  AmgVector(5) updatedParameters(parm.parameters());

  double newP = p;
  if (m_eLoss){
    // get the momentum change plus the according sigma
    Trk::EnergyLoss* sampledEnergyLoss = m_eLossUpdator->energyLoss(matprop, p, pathCorrection, dir, particle); 

    
    if (sampledEnergyLoss){
       double energyLoss = sampledEnergyLoss->deltaE();
       // protection against NaN
       if ( E+energyLoss < m) {
           ATH_MSG_VERBOSE( "  [+] particle momentum fell under momentum cut - stop simulation" );
           delete sampledEnergyLoss;
           return 0;
       }
      // smear the mometnum change with the sigma 
       newP = sqrt((E+energyLoss)*(E+energyLoss)-m*m);
    }
    
    if (m_recordEnergyDeposition && m_currentSample > 0){
        /* TODO: update to ISF Services
        ATH_MSG_VERBOSE( "  [+] try to record deposited energy (if mapped with a calo sample) " );        
        // record the energy loss           
        Trk::MaterialEffectsOnTrack* meot = new Trk::MaterialEffectsOnTrack(0.,0,
                                                                            sampledEnergyLoss,
                                                                            *parm.associatedSurface());
        // create a iFatras::EnergyDeposit
        FSim::EnergyDeposit edeposit(m_currentSample, meot, parm.position());
        iFatras::ICollectionManager::fillEnergyDeposit(m_currentGenParticle->barcode(),edeposit);
        ATH_MSG_VERBOSE( "  [+] sucessfully recorded deposited energy of " << sampledEnergyLoss->deltaE() << " CLHEP::MeV / sample "
                          <<  m_currentSample );  
        */
        sampledEnergyLoss = 0; // avoid double deletion --- EnergyDeposit takes ownership
        if (m_edValidationTree){
                // fill the variables
                m_edLayerIntersectX = parm.position().x();
                m_edLayerIntersectY = parm.position().y();
                m_edLayerIntersectZ = parm.position().z();
                m_edLayerIntersectR = parm.position().perp();
                // the layer and the deposited energy
                m_edLayerSample     = m_currentSample;
                m_edLayerEnergyDeposit = -1*sampledEnergyLoss->deltaE();
                // and fill it
                m_edValidationTree->Fill();            
            }
    }

    // the deltaP
    double deltaP = newP - p;
    delete sampledEnergyLoss;

    // validation
    if (m_bremValidation && particle==Trk::electron) m_bremMotherEnergy = p; 
    // record the brem
    if (particle == Trk::electron && deltaP < -1.*m_minimumBremPhotonMomentum) {
      Amg::Vector3D dir = parm.momentum().unit();     // to be used for recoil
      recordBremPhoton(time,p,-deltaP,
		       parm.position(),
		       dir,
		       Trk::electron);
    }
    // bail out if the update drops below the momentum cut
    if ( newP < m_minimumMomentum) {
      ATH_MSG_VERBOSE( "  [+] particle momentum fell under momentum cut - stop simulation" );
      return 0;
    }
    ATH_MSG_VERBOSE( "  [+] delta(P)   =  " << deltaP );
    // continue
    // TOM - The random number generator for energy loss is now controlled by MC versions of the energy loss updator
    //double sigmaDeltaP = (p+deltaP)*deltaPsigmaQoP.second;
    (updatedParameters)[Trk::qOverP] = parm.charge()/(p+deltaP); 
    // for the validation
    m_deltaP = deltaP;
  }
  
  if (m_ms && matprop.x0()>0 ){
    // get the projected scattering angle
    double sigmaMSproj   = (m_use_msUpdator && m_msUpdator) ? sqrt(m_msUpdator->sigmaSquare(matprop, p, pathCorrection, particle)) : 
                                                              msSigma(pathCorrection*matprop.thicknessInX0(),p,particle); 
    // do the update
    multipleScatteringUpdate(parm,updatedParameters,sigmaMSproj);
  }
  
   // validation mode ---------------------------------------------------------------
   if (m_validationMode) {
     m_tInX0           = pathCorrection * matprop.thicknessInX0();
     m_validationTree->Fill();
  }
  // -------------------------------------------------------------------------------

  AmgSymMatrix(5) *errorMatrix = ( parm.covariance() ) ? new AmgSymMatrix(5)(*parm.covariance()) : 0;

  return parm.associatedSurface().createTrackParameters(updatedParameters[0],
                                                        updatedParameters[1],
                                                        updatedParameters[2],
                                                        updatedParameters[3],
                                                        updatedParameters[4],
                                                        errorMatrix);
}



const Trk::TrackParameters*  iFatras::McMaterialEffectsUpdator::update( double /*time*/,
                                      const Trk::TrackParameters* parm,
                                      const Trk::MaterialEffectsOnTrack& meff,
                                      Trk::ParticleHypothesis particle,
                                      Trk::MaterialUpdateMode) const

{

   if (particle != Trk::muon) {
      ATH_MSG_WARNING( "  [ ---- ] Only implemented for muons yet. No parameterisation for other particles." );
      return parm;
   }
   
   // the updatedParameters - first a copy
   AmgVector(5) updatedParameters(parm->parameters());
     
   // get the path correciton --- is 1. for dynamic layers
   double pathCorrection = 1.;

   // get the kinematics
   double p    = parm->momentum().mag();
   double m    = m_particleMasses.mass[particle];
   double E    = sqrt(p*p+m*m);
   // the updatedParameters - first a d   
   
   
   // MaterialEffectsOnTrack object should contain
   // a) multiple scattering part
   // b) energy loss part for Landau simulation
   
   // (a)
   if (m_ms){       
      //If the meff has scattering angles use those, otherwise use MEffUpdator
      if(!meff.scatteringAngles()){
        //Trick to keep using existing MultipleScatteringUpdator interface
        //and create a dummy materialProperties with the properties we are interested in
        Trk::MaterialProperties mprop(meff.thicknessInX0(),1.,0.,0.,0.,0.);
       
        // get the projected scattering angle
       double sigmaMSproj   = (m_use_msUpdator && m_msUpdator) ? sqrt(m_msUpdator->sigmaSquare(mprop, p, pathCorrection, particle)) : 
                                                                 msSigma(pathCorrection*mprop.thicknessInX0(),p,particle); 
        // do the update
        multipleScatteringUpdate(*parm,updatedParameters,sigmaMSproj);       
     }  
   
   
   }
      
  // (b) 
  if (m_eLoss && meff.energyLoss()){
    // energy loss update        
    double energyLossMPV = meff.energyLoss()->deltaE();
    double energyLossSigma = meff.energyLoss()->sigmaDeltaE();   
    
    double simulatedEnergyLoss = -1.*(energyLossMPV+energyLossSigma*CLHEP::RandLandau::shoot(m_randomEngine));    
  
    if (m_recordEnergyDeposition){ 

       ATH_MSG_VERBOSE( "  [+] try to record deposited energy (if mapped with a calo sample) " );
               
       // need to identify the layer first
       if (!m_trackingGeometry && updateTrackingGeometry().isFailure()){
          ATH_MSG_WARNING( "  [ ----] Could not retrieve TrackingGeometry." );
       } else {
       
        const Trk::LayerIndexSampleMap* lism     = layerIndexSampleMap();

        if (lism){  
          // get the Volume and then get the layer
          const Trk::TrackingVolume* currentVolume = m_trackingGeometry->lowestTrackingVolume(parm->position()); 
          const Trk::Layer* associatedLayer = currentVolume ? currentVolume->associatedLayer(parm->position()) : 0;
      
           // only go on if you have found an associated Layer && the guy has an index
           if (associatedLayer && associatedLayer->layerIndex().value()){         
              // now try to find it
              Trk::LayerIndexSampleMap::const_iterator layerIndexCaloSample = lism->find(associatedLayer->layerIndex());
              // layerindex could be found
              if (layerIndexCaloSample != lism->end()){
                /* TODO: update to ISF Services
                ATH_MSG_VERBOSE( "  [+] CaloSample identified" );

                // create the energyloss object
                Trk::EnergyLoss* sampledEnergyLoss = new Trk::EnergyLoss(simulatedEnergyLoss,0.);
               
                // record the energy loss           
                Trk::MaterialEffectsOnTrack* meot = new Trk::MaterialEffectsOnTrack(0.,0,
                                                                                    sampledEnergyLoss,
                                                                                    associatedLayer->surfaceRepresentation());
               // create a iFatras::EnergyDeposit
               FSim::EnergyDeposit edeposit(layerIndexCaloSample->second, meot, parm->position());
               iFatras::ICollectionManager::fillEnergyDeposit(m_currentGenParticle->barcode(),edeposit);                                        
               ATH_MSG_VERBOSE( "  [+] sucessfully recorded deposited energy of " << sampledEnergyLoss->deltaE() << " CLHEP::MeV / sample "
                       <<  layerIndexCaloSample->second );
               */
               // the validation section
               if (m_edValidationTree){
                 // fill the variables
                 const Amg::Vector3D& edeposition = parm->position(); 
                 m_edLayerIntersectX = edeposition.x();
                 m_edLayerIntersectY = edeposition.y();
                 m_edLayerIntersectZ = edeposition.z();
                 m_edLayerIntersectR = edeposition.perp();
                 // the layer and the deposited energy
                 m_edLayerSample     = layerIndexCaloSample->second;
                 m_edLayerEnergyDeposit = -1*simulatedEnergyLoss;
                 // and fill it
                 m_edValidationTree->Fill();            
             } // end of validation section                           
            } // end of  calo sample found         
          } // end of associatedLayer 
         } // end of lism
       } // else -> trackingGeometry exists
    } // end of recordEnergyDeposition
    
    double newP = sqrt((E+simulatedEnergyLoss)*(E+simulatedEnergyLoss)-m*m);
    
    // set the new momentum
    updatedParameters[Trk::qOverP] = parm->charge()/newP;
    
  } 
   
  // use the manipulator to update the track parameters -------> get ridd of 0!
  return parm->associatedSurface().createTrackParameters(updatedParameters[0],
                                                         updatedParameters[1],
                                                         updatedParameters[2],
                                                         updatedParameters[3],
                                                         updatedParameters[4],
                                                         0);
   //TODO: here's probably a memory leak: need to delete the variable 'parm'
}

/*                                      
bool  iFatras::McMaterialEffectsUpdator::handleEnergyLoss( Trk::TrackParameters& parm, 
                 Amg::Vector3D* updatedParameters,
							   Trk::EnergyLoss* sampledEnergyLoss, 
							   Trk::ParticleHypothesis particle) const
{
  double p = parm.momentum().mag();                                                      
  double m    = m_particleMasses.mass[particle];
  double E    = sqrt(p*p+m*m);
  double energyLoss = sampledEnergyLoss->deltaE();   
  
  // record the energy deposition in the given layers ---------------------------------------------------
  if (m_recordEnergyDeposition){
    ATH_MSG_VERBOSE( "  [+] try to record deposited energy (if mapped with a calo sample) " );
    
    const Trk::LayerIndexSampleMap* lism     = layerIndexSampleMap();

    if (lism){
      
      ATH_MSG_VERBOSE( "  [+] CaloSample found try to find the layer index " << m_layer->layerIndex() << " in a map of size " << lism->size() );
      
      Trk::LayerIndexSampleMap::const_iterator layerIndexCaloSample = lism->find(m_layer->layerIndex());
      
      if (layerIndexCaloSample != lism->end()){
	// TODO: update to ISF Services
	//   ATH_MSG_VERBOSE( "  [+] CaloSample identified" );
	   
	   // record the energy loss           
	//   Trk::MaterialEffectsOnTrack* meot = new Trk::MaterialEffectsOnTrack(0.,0,
	//   sampledEnergyLoss,
	//   lay.surfaceRepresentation());
	//   // create a iFatras::EnergyDeposit
	//   FSim::EnergyDeposit edeposit(layerIndexCaloSample->second, meot, parm.position());
	//   iFatras::ICollectionManager::fillEnergyDeposit(m_currentGenParticle->barcode(),edeposit);                                        
	//   ATH_MSG_VERBOSE( "  [+] sucessfully recorded deposited energy of " << sampledEnergyLoss->deltaE() << " CLHEP::MeV / sample "
	//   <<  layerIndexCaloSample->second );
	//
	// the validation section
	if (m_edValidationTree){
	  // fill the variables
	  m_edLayerIntersectX = parm.position().x();
	  m_edLayerIntersectY = parm.position().y();
	  m_edLayerIntersectZ = parm.position().z();
	  m_edLayerIntersectR = parm.position().perp();
	  // the layer and the deposited energy
	  m_edLayerSample     = layerIndexCaloSample->second;
	  m_edLayerEnergyDeposit = -1*energyLoss;
	  // and fill it
	  m_edValidationTree->Fill();            
	}                                 
	// avoid double deletion
	sampledEnergyLoss = 0;                                                                                                        
      } else  
	ATH_MSG_VERBOSE( "  [-] Sample layer could not been identified" );
    } 
    
    delete sampledEnergyLoss;
    
    // protection against NaN && application of minimum momentum cut
    if ( E+energyLoss < m) {
      ATH_MSG_VERBOSE( "  [+] particle momentum fell under momentum cut - stop simulation" );
      delete updatedParameters;
      return false;
    }              
    
    newP = sqrt((E+energyLoss)*(E+energyLoss)-m*m);
    
    // the deltaP
    double deltaP = newP - p;
    
   // bail out if the update drops below the momentum cut - don't delete, since that's the extrapolators business
    if ( newP < m_minimumMomentum ) {
      ATH_MSG_VERBOSE( "  [+] particle momentum fell under momentum cut - stop simulation" );
      delete updatedParameters;
      return false ;
    }
    // continue else
    // TOM - The random number generator for energy loss is now controlled by MC versions of the energy loss updator
    //double sigmaDeltaP = (p+deltaP)*deltaPsigmaQoP.second;
    (*updatedParameters)[Trk::qOverP] = parm.charge()/(newP); 
    
    // for the validation
    m_deltaP = deltaP;                                                         
    
    return true;

}
*/     

StatusCode iFatras::McMaterialEffectsUpdator::updateTrackingGeometry() const
{

  // -------------------- public TrackingGeometry (from DetectorStore) ----------------------------
  // get the DetectorStore

  StatusCode s = detStore()->retrieve(m_trackingGeometry, m_trackingGeometryName);
  if (s.isFailure())
  {
       ATH_MSG_FATAL( "Could not retrieve TrackingGeometry '" << m_trackingGeometryName << "' from DetectorStore." );
       ATH_MSG_FATAL( "  - probably the chosen layout is not supported / no cool tag exists. "                     );
       return s;
   }

  return s;
}                                                    

double iFatras::McMaterialEffectsUpdator::msSigma(double dInX0,double p,Trk::ParticleHypothesis particle) const {
   
  double m    = m_particleMasses.mass[particle];
  double E    = sqrt(p*p+m*m);
  double beta = p/E; 

  // PDG Particle Review, Phys.Rev.D86,010001(2012), chapter 30.3, page 328 

  double sigmaMSproj = 13.6/beta/p*sqrt(dInX0)*(1.+0.038*log(dInX0));

  return sigmaMSproj; 
} 
                                                         
void iFatras::McMaterialEffectsUpdator::multipleScatteringUpdate(const Trk::TrackParameters& pars,
                                                                AmgVector(5)& parameters,
                                                                double sigmaMSproj) const
{

  // parametric scattering - independent in x/y 
  if (m_parametricScattering){ 
    // the initial values
    double theta =  parameters[Trk::theta];
    double phi   =  parameters[Trk::phi];
    double sinTheta   = (theta*theta > 10e-10) ? sin(theta) : 1.; 

    // sample them in an independent way
    double deltaTheta = m_projectionFactor*sigmaMSproj*CLHEP::RandGaussZiggurat::shoot(m_randomEngine);
    double deltaPhi   = m_projectionFactor*sigmaMSproj*CLHEP::RandGaussZiggurat::shoot(m_randomEngine)/sinTheta;

    phi += deltaPhi;
    if (phi >= M_PI) phi -= M_PI;
    else if (phi < -M_PI) phi += M_PI;
    if (theta > M_PI) theta -= M_PI;
    
    ATH_MSG_VERBOSE( "  [+] deltaPhi / deltaTheta = " << deltaPhi << " / " << deltaTheta );

    // assign the new values
    parameters[Trk::phi]   = phi;   
    parameters[Trk::theta] = fabs(theta + deltaTheta);

    // for the validation
    m_thetaMStheta = deltaTheta;
    m_thetaMSphi = deltaPhi;

  } else {
    // scale the projected out of the plane
    double thetaMs = m_projectionFactor*sigmaMSproj*CLHEP::RandGaussZiggurat::shoot(m_randomEngine);
    double psi     = 2.*M_PI*CLHEP::RandFlat::shoot(m_randomEngine);
    // more complex but "more true" - 
    CLHEP::Hep3Vector parsMomHep( pars.momentum().x(), pars.momentum().y(), pars.momentum().z() );
    CLHEP::Hep3Vector newDirectionHep( parsMomHep.unit().x(), parsMomHep.unit().y(), parsMomHep.unit().z());
    double x = -newDirectionHep.y();
    double y = newDirectionHep.x();
    double z = 0.;
    // if it runs along the z axis - no good ==> take the x axis
    if (newDirectionHep.z()*newDirectionHep.z() > 0.999999) {
        x = 1.; y=0.;
    }
    // deflector direction
    CLHEP::Hep3Vector deflector(x,y,z);
    // rotate the new direction for scattering
    newDirectionHep.rotate(thetaMs, deflector);
    // and arbitrarily in psi             
    newDirectionHep.rotate(psi, parsMomHep);
    // assign the new values
    parameters[Trk::phi]   = newDirectionHep.phi();   
    parameters[Trk::theta] = newDirectionHep.theta();

    if (m_validationMode){
      m_thetaMStheta = pars.parameters()[Trk::theta] - parameters[Trk::theta];
      m_thetaMSphi = pars.parameters()[Trk::phi] - parameters[Trk::phi];
   }
  }

}


void iFatras::McMaterialEffectsUpdator::recordBremPhoton(double time,
							 double pElectron,
                                                         double gammaE,
                                                         const Amg::Vector3D& vertex,
                                                         Amg::Vector3D& particleDir,
                                                         Trk::ParticleHypothesis particle ) const
{
    // get parent particle
    // @TODO: replace by Fatras internal bookkeeping
    const ISF::ISFParticle *parent = ISF::ParticleClipboard::getInstance().getParticle();
    // something is seriously wrong if there is no parent particle
    assert(parent);

    // statistics
    ++m_recordedBremPhotons;
    // ------------------------------------------------------
    // simple approach
    // (a) simulate theta uniform within the opening angle of the relativistic Hertz dipole
    //      theta_max = 1/gamma
    // (b)Following the Geant4 approximation from L. Urban -> encapsulate that later
    //      the azimutal angle
    
    double psi    =  2.*M_PI*CLHEP::RandFlat::shoot(m_randomEngine);
    
    // the start of the equation
    double theta = 0.;
    double m = m_particleMasses.mass[particle];
    double E = sqrt(pElectron*pElectron + m*m);

    if (m_uniformHertzDipoleAngle) {
       // the simplest simulation
       theta = m/E * CLHEP::RandFlat::shoot(m_randomEngine);  
    } else {
      // -----> 
      theta = m/E;
      // follow 
      double a = 0.625; // 5/8

      double r1 = CLHEP::RandFlat::shoot(m_randomEngine);
      double r2 = CLHEP::RandFlat::shoot(m_randomEngine);
      double r3 = CLHEP::RandFlat::shoot(m_randomEngine);

      double u =  -log(r2*r3)/a;
    
      theta *= (r1 < 0.25 ) ? u : u*m_oneOverThree; // 9./(9.+27) = 0.25
    }
     
    ATH_MSG_VERBOSE( "  [ brem ] Simulated angle to electron    = " << theta << "." );

    /*
    // more complex but "more true"
    Amg::Vector3D newDirection(particleDir);
    double x = -newDirection.y();
    double y = newDirection.x();
    double z = 0.;
    // if it runs along the z axis - no good ==> take the x axis
    if (newDirection.z()*newDirection.z() > 0.999999)       
         x = 1.;
    // deflector direction
    Amg::Vector3D deflector(x,y,z);
    // rotate the new direction for scattering
    newDirection.rotate(theta, deflector);
    // and arbitrarily in psi             
    newDirection.rotate(psi,particleDir);
    */
    // resample
    //double rand = CLHEP::RandFlat::shoot(m_randomEngine);
    //double eps = 0.001;
    //theta = eps/(1.-rand*(1-eps)) - eps;

    double th = particleDir.theta()-theta;
    double ph = particleDir.phi();
    if ( th<0.) { th *=-1; ph += M_PI; }
    CLHEP::Hep3Vector newDirectionHep( sin(th)*cos(ph),sin(th)*sin(ph),cos(th) );
    CLHEP::Hep3Vector particleDirHep( particleDir.x(), particleDir.y(), particleDir.z() );
    newDirectionHep.rotate(psi,particleDirHep);
    Amg::Vector3D newDirection( newDirectionHep.x(), newDirectionHep.y(), newDirectionHep.z() );
    
    // recoil / save input momentum for validation
    Amg::Vector3D inEl(pElectron*particleDir);    
    particleDir = (particleDir*pElectron- gammaE*newDirection).unit();

    //std::cout <<"brem opening angle:in:out:"<< cos(theta) <<","<<newDirection*particleDir<< std::endl; 
   
    // -------> create the brem photon <--------------------
    ISF::ISFParticle *bremPhoton = new ISF::ISFParticle( vertex,
                                                         gammaE*newDirection,
                                                         0,   //!< mass
                                                         0,   //!< charge
                                                         22,  //!< pdg code
                                                         time,  //!< time
                                                         *parent );

    // in the validation mode, add process info
    if (m_validationMode) {
      ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
      validInfo->setProcess(m_processCode);
      if (parent->getUserInformation()) validInfo->setGeneration(parent->getUserInformation()->generation()+1);
      else validInfo->setGeneration(1);     // assume parent is a primary
      bremPhoton->setUserInformation(validInfo);
    }

    // register TruthIncident
    ISF::ISFParticleVector children(1, bremPhoton);
    ISF::ISFTruthIncident truth( const_cast<ISF::ISFParticle&>(*parent),
                                 children,
                                 m_processCode /*!< @todo fix non-static */,
                                 parent->nextGeoID(),
                                 ISF::fPrimarySurvives );
    m_truthRecordSvc->registerTruthIncident( truth);
    //Making sure we get some correct truth info from parent if needed before pushing into the particle broker
    if (!bremPhoton->getTruthBinding()) {
	bremPhoton->setTruthBinding(new ISF::TruthBinding(*parent->getTruthBinding()));
    }
    m_particleBroker->push( bremPhoton, parent);


    // save info for validation
    if (m_validationMode && m_validationTool) {
      Amg::Vector3D* nMom = new Amg::Vector3D((pElectron-gammaE)*particleDir);
      m_validationTool->saveISFVertexInfo(3,vertex,*parent,inEl,nMom,children);
      delete nMom;
    }


    // if validation is turned on - go for it 
    if (m_bremValidationTree){
      // 
      ATH_MSG_VERBOSE( "  [ brem ] Filling bremsstrahlung validation TTree ..." );
      // spatical information
      m_bremPointX = float(vertex.x());     
      m_bremPointY = float(vertex.y());     
      m_bremPointR = float(vertex.perp());     
      m_bremPointZ = float(vertex.z());     
      // photon energyEnergy
      m_bremPhotonEnergy = float(gammaE);
      m_bremPhotonAngle  = float(theta);
      // fill the tree
      m_bremValidationTree->Fill();
   } else {
      ATH_MSG_VERBOSE( "  [ brem ] No TTree booked ! " );
   }

} 

void iFatras::McMaterialEffectsUpdator::recordBremPhotonLay(const ISF::ISFParticle* parent,
							    Trk::TimeLimit timeLim,
							    double pElectron,
							    double gammaE,
							    const Amg::Vector3D& vertex,
							    Amg::Vector3D& particleDir,
							    double matFraction,
							    Trk::PropDirection dir,
							    Trk::ParticleHypothesis particle ) const
{
    // statistics
    ++m_recordedBremPhotons;
    // ------------------------------------------------------
    // simple approach
    // (a) simulate theta uniform within the opening angle of the relativistic Hertz dipole
    //      theta_max = 1/gamma
    // (b)Following the Geant4 approximation from L. Urban -> encapsulate that later
    //      the azimutal angle
    
    double psi    =  2.*M_PI*CLHEP::RandFlat::shoot(m_randomEngine);
    
    // the start of the equation
    double theta = 0.;
    double m = m_particleMasses.mass[particle];
    double E = sqrt(pElectron*pElectron + m*m);

    if (m_uniformHertzDipoleAngle) {
       // the simplest simulation
       theta = m/E * CLHEP::RandFlat::shoot(m_randomEngine);  
    } else {
      // -----> 
      theta = m/E;
      // follow 
      double a = 0.625; // 5/8

      double r1 = CLHEP::RandFlat::shoot(m_randomEngine);
      double r2 = CLHEP::RandFlat::shoot(m_randomEngine);
      double r3 = CLHEP::RandFlat::shoot(m_randomEngine);

      double u =  -log(r2*r3)/a;
    
      theta *= (r1 < 0.25 ) ? u : u*m_oneOverThree; // 9./(9.+27) = 0.25
    }
     
    ATH_MSG_VERBOSE( "  [ brem ] Simulated angle to electron    = " << theta << "." );

    // more complex but "more true"
    CLHEP::Hep3Vector particleDirHep( particleDir.x(), particleDir.y(), particleDir.z() );
    CLHEP::Hep3Vector newDirectionHep( particleDirHep );
    double x = -newDirectionHep.y();
    double y = newDirectionHep.x();
    double z = 0.;
    // if it runs along the z axis - no good ==> take the x axis
    if (newDirectionHep.z()*newDirectionHep.z() > 0.999999)       
         x = 1.;
    // deflector direction
    CLHEP::Hep3Vector deflectorHep(x,y,z);
    // rotate the new direction for scattering
    newDirectionHep.rotate(theta, deflectorHep);
    // and arbitrarily in psi             
    newDirectionHep.rotate(psi,particleDirHep);

    // recoil   
    Amg::Vector3D newDirection( newDirectionHep.x(), newDirectionHep.y(), newDirectionHep.z() );
    particleDir = (particleDir*pElectron- gammaE*newDirection).unit();

    // -------> create the brem photon <--------------------
    ISF::ISFParticle *bremPhoton = new ISF::ISFParticle( vertex,
                                                         gammaE*newDirection,
                                                         0,   //!< mass
                                                         0,   //!< charge
                                                         22,  //!< pdg code
                                                         timeLim.time,  //!< time
                                                         *parent );


    // in the validation mode, add process info
    if (m_validationMode) {
      ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
      validInfo->setProcess(m_processCode);
      if (parent->getUserInformation()) validInfo->setGeneration(parent->getUserInformation()->generation()+1);
      else validInfo->setGeneration(1);     // assume parent is a primary track
      bremPhoton->setUserInformation(validInfo);
    }

    // register TruthIncident
    ISF::ISFParticleVector children(1, bremPhoton);
    ISF::ISFTruthIncident truth( const_cast<ISF::ISFParticle&>(*parent),
                                 children,
                                 m_processCode /*!< @TODO fix non-static */,
                                 parent->nextGeoID(),
                                 ISF::fPrimarySurvives );
    m_truthRecordSvc->registerTruthIncident( truth);

    //Making sure we get some correct truth info from parent if needed before pushing into the particle broker
    if (!bremPhoton->getTruthBinding()) {
	bremPhoton->setTruthBinding(new ISF::TruthBinding(*parent->getTruthBinding()));
    }

    // save info for validation
    if (m_validationMode && m_validationTool) {
      Amg::Vector3D* nMom = new Amg::Vector3D((pElectron-gammaE)*particleDir);
      m_validationTool->saveISFVertexInfo(3,vertex,*parent,pElectron*particleDir,nMom,children);
      delete nMom;
    }

    //std::cout <<"brem photon created:position,mom,gen,barcode:"<< vertex<<","<<gammaE<<","<<bremPhoton->generation()<<","<<bremPhoton->barcode()<< std::endl;

    // layer update : don't push into particle stack untill destiny resolved
    Trk::PathLimit pLim = m_samplingTool->sampleProcess(bremPhoton->momentum().mag(),0.,Trk::photon);

    // material fraction : flip if direction of propagation changed
    double ci = m_layer->surfaceRepresentation().normal().dot(particleDir);
    double co = m_layer->surfaceRepresentation().normal().dot(newDirection);

    double remMat =  ci*co <0 ? (1-matFraction)    : matFraction;

    // decide if photon leaves the layer
    // amount of material to traverse      
    double pathCorrection = m_layer->surfaceRepresentation().normal().dot(bremPhoton->momentum()) !=0 ?
      fabs(m_layer->surfaceRepresentation().pathCorrection(bremPhoton->position(),bremPhoton->momentum())) : 1.;

    double mTot = m_matProp->thicknessInX0()*pathCorrection*(1.-remMat); 

    if (mTot < pLim.x0Max) {  // release 

      m_particleBroker->push( bremPhoton, parent);
      
      //std::cout <<"brem photon:"<<bremPhoton<<" registered for extrapolation from "<< vertex << "; presampled pathLim, layer dx:"<< pLim.x0Max<<","<< mTot <<std::endl;

    } else {  //  interaction within the layer
      const Trk::CurvilinearParameters*  cparm = new Trk::CurvilinearParameters(bremPhoton->position(),bremPhoton->momentum(),bremPhoton->charge());
      const Trk::TrackParameters* uPar = updateInLay(bremPhoton,cparm,remMat,timeLim, pLim, dir, Trk::photon);
      if (uPar) ATH_MSG_VERBOSE( "Check this: parameters should be dummy here (brem.photon) " <<","<<uPar->position() );
    }

    // if validation is turned on - go for it 
    if (m_bremValidationTree){
      // 
      ATH_MSG_VERBOSE( "  [ brem ] Filling bremsstrahlung validation TTree ..." );
      // spatical information
      m_bremPointX = float(vertex.x());     
      m_bremPointY = float(vertex.y());     
      m_bremPointR = float(vertex.perp());     
      m_bremPointZ = float(vertex.z());     
      // photon energyEnergy
      m_bremPhotonEnergy = float(gammaE);
      m_bremPhotonAngle  = float(theta);
      // fill the tree
      m_bremValidationTree->Fill();
   } else {
      ATH_MSG_VERBOSE( "  [ brem ] No TTree booked ! " );
   }

} 


const Trk::LayerIndexSampleMap*  iFatras::McMaterialEffectsUpdator::layerIndexSampleMap() const 
{
  // no layer index map --- retrieve it
  if (!m_layerIndexCaloSampleMap){ 
      // try to retrieve it from detector store            
      if ((detStore()->retrieve(m_layerIndexCaloSampleMap, m_layerIndexCaloSampleMapName).isFailure())){
            ATH_MSG_WARNING( "Could not retrieve '" << m_layerIndexCaloSampleMapName << "' from DetectorStore." );
            ATH_MSG_WARNING( " -> switching recording of energy deposit off." );       
      }

  if (m_layerIndexCaloSampleMap)
        ATH_MSG_VERBOSE( "Successfully retrieved '" << m_layerIndexCaloSampleMapName << "' with entries:"
              << m_layerIndexCaloSampleMap->size() );
  }  
  return m_layerIndexCaloSampleMap;  
}

const Trk::TrackParameters*  iFatras::McMaterialEffectsUpdator::interact(double time,
						  const Amg::Vector3D& position,
						  const Amg::Vector3D& momentum,
						  Trk::ParticleHypothesis particle,
						  int process,
						  const Trk::Material* m_extMatProp) const {
  if ( process==0 ) return 0;

  // get parent particle
  // @TODO: replace by Fatras internal bookkeeping
  const ISF::ISFParticle *parent = ISF::ParticleClipboard::getInstance().getParticle();
  // something is seriously wrong if there is no parent particle
  assert(parent);

  double mass = m_particleMasses.mass[particle];
  double p = momentum.mag();
  /*double E = sqrt( p*p + mass*mass);*/

  if ( process == 201 ) {    // decay  
    // update parent before decay
    ISF::ISFParticleVector childVector = m_particleDecayer->decayParticle(*parent,position,momentum,time);

    for (unsigned int i=0; i<childVector.size(); i++) {
      // in the validation mode, add process info
      if (m_validationMode) {
	ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
	validInfo->setProcess(process);
	if (parent->getUserInformation()) validInfo->setGeneration(parent->getUserInformation()->generation()+1);
	else validInfo->setGeneration(1);     // assume parent is a primary track
        childVector[i]->setUserInformation(validInfo);
      }
      // register next geo (is current), next flavor can be defined by filter
      childVector[i]->setNextGeoID( parent->nextGeoID() );
      // feed it the particle broker with parent information
      m_particleBroker->push(childVector[i], parent);
    }

    // register TruthIncident
    ISF::ISFTruthIncident truth( const_cast<ISF::ISFParticle&>(*parent),
                                 childVector,
                                 process,
                                 parent->nextGeoID(),  // inherits from the parent
                                 ISF::fKillsPrimary );
    m_truthRecordSvc->registerTruthIncident( truth);

    // save info for validation
    if (m_validationMode && m_validationTool) {
      Amg::Vector3D* nMom = 0;
      m_validationTool->saveISFVertexInfo(process,position,*parent,momentum,nMom,childVector);
      delete nMom;
    }

    return 0;
  }

  if ( process == 5 ) {     // positron annihilation

    static ISF::ISFParticleVector children(2);

    double fmin = mass/p; 
     
    double fr = 1.-pow(fmin,CLHEP::RandFlat::shoot(m_randomEngine));

    // double cTh = 1.-fmin/(1.-fr)/fr;

    // first implementation: ctH=1 

    children[0] = new ISF::ISFParticle( position,
                                        fr*momentum,
                                        0.,
                                        0.,
                                        22,
                                        time,
                                        *parent );

    children[1] = new ISF::ISFParticle( position,
                                        (1-fr)*momentum,
                                        0.,
                                        0.,
                                        22,
                                        time,
                                        *parent );
    
    // in the validation mode, add process info
    if (m_validationMode) {
      ISF::ParticleUserInformation* validInfo1 = new ISF::ParticleUserInformation();
      validInfo1->setProcess(process);
      if (parent->getUserInformation()) validInfo1->setGeneration(parent->getUserInformation()->generation()+1);
      else validInfo1->setGeneration(1);     // assume parent is a primary track
      children[0]->setUserInformation(validInfo1);
      ISF::ParticleUserInformation* validInfo2 = new ISF::ParticleUserInformation();
      validInfo2->setProcess(process);
      if (parent->getUserInformation()) validInfo2->setGeneration(parent->getUserInformation()->generation()+1);
      else validInfo2->setGeneration(1);     // assume parent is a primary track
      children[1]->setUserInformation(validInfo2);
    }

    // register TruthIncident
    ISF::ISFTruthIncident truth( const_cast<ISF::ISFParticle&>(*parent),
                                 children,
                                 process,
                                 parent->nextGeoID(),  // inherits from the parent
                                 ISF::fPrimarySurvives );
    m_truthRecordSvc->registerTruthIncident( truth);

    // push child particles onto stack
    m_particleBroker->push( children[0], parent);
    m_particleBroker->push( children[1], parent);
 
    // save info for validation
    if (m_validationMode && m_validationTool) {
      Amg::Vector3D* nMom = 0;
      m_validationTool->saveISFVertexInfo(process,position,*parent,momentum,nMom,children);
      delete nMom;
    }

    // kill the track -----------------------------
    return 0;
  }

  if (process==14) {  // photon conversion
   
    const Trk::NeutralParameters* parm = new Trk::NeutralCurvilinearParameters(position,momentum,parent->charge());

    bool cStat = m_conversionTool->doConversion(time, *parm); 
   
    if (!cStat) ATH_MSG_WARNING( "Conversion failed, killing photon anyway ");

    // kill the mother particle
    delete parm; return 0;
  }

  if (process==121) {    // hadronic interaction

    //const Amg::Vector3D pDir = momentum.unit();

    const Trk::TrackParameters* parm = new Trk::CurvilinearParameters(position,momentum,parent->charge());

    bool recHad = m_hadIntProcessor->doHadronicInteraction(time, position, momentum, m_extMatProp, particle, true);
    // eventually : bool recHad =  m_hadIntProcessor->recordHadState( time, p, position, pDir, particle);
 
    // kill the track if interaction recorded --------------------------
    if ( recHad ) {delete parm; return 0;}
    else return parm;

    //delete parm;
    //return recHad;

  }
    
  return 0;
}


ISF::ISFParticleVector  iFatras::McMaterialEffectsUpdator::interactLay(const ISF::ISFParticle* parent,
								       double time,
								       const Trk::TrackParameters& parm,
								       Trk::ParticleHypothesis particle,
								       int process,
								       const Trk::MaterialProperties* m_extMatProp) const {
  ISF::ISFParticleVector childVector(0);

  if ( process==0 ) return childVector;

  double mass = m_particleMasses.mass[particle];
  double p = parm.momentum().mag();
  /*double E = sqrt( p*p + mass*mass);*/
  Amg::Vector3D position=parm.position();
  Amg::Vector3D momentum=parm.momentum();

  if ( process == 5 ) {     // positron annihilation

    static ISF::ISFParticleVector children(2);

    double fmin = mass/p; 
     
    double fr = 1.-pow(fmin,CLHEP::RandFlat::shoot(m_randomEngine));

    // double cTh = 1.-fmin/(1.-fr)/fr;

    // first implementation: ctH=1 

    children[0] = new ISF::ISFParticle( position,
                                        fr*momentum,
                                        0.,
                                        0.,
                                        22,
                                        time,
                                        *parent );

    children[1] = new ISF::ISFParticle( position,
                                        (1-fr)*momentum,
                                        0.,
                                        0.,
                                        22,
                                        time,
                                        *parent );
    
    // in the validation mode, add process info
    if (m_validationMode) {
      ISF::ParticleUserInformation* validInfo1 = new ISF::ParticleUserInformation();
      validInfo1->setProcess(process);
      if (parent->getUserInformation()) validInfo1->setGeneration(parent->getUserInformation()->generation()+1);
      else validInfo1->setGeneration(1);     // assume parent is a primary track
      children[0]->setUserInformation(validInfo1);
      ISF::ParticleUserInformation* validInfo2 = new ISF::ParticleUserInformation();
      validInfo2->setProcess(process);
      if (parent->getUserInformation()) validInfo2->setGeneration(parent->getUserInformation()->generation()+1);
      else validInfo2->setGeneration(-1);     // assume parent is a primary track
      children[1]->setUserInformation(validInfo2);
    }

    // register TruthIncident
    ISF::ISFTruthIncident truth( const_cast<ISF::ISFParticle&>(*parent),
                                 children,
                                 process,
                                 parent->nextGeoID(),  // inherits from the parent
                                 ISF::fPrimarySurvives );
    m_truthRecordSvc->registerTruthIncident( truth);

    // save info for validation
    if (m_validationMode && m_validationTool) {
      Amg::Vector3D* nMom = 0;
      m_validationTool->saveISFVertexInfo(process,position,*parent,momentum,nMom,children);
      delete nMom;
    }

    //Making sure we get some correct truth info from parent if needed before pushing into the particle broker
    if (!children[0]->getTruthBinding()) {
	children[0]->setTruthBinding(new ISF::TruthBinding(*parent->getTruthBinding()));
    }
    if (!children[1]->getTruthBinding()) {
	children[1]->setTruthBinding(new ISF::TruthBinding(*parent->getTruthBinding()));
    }

    return children;
  }

  if (process==14) {  // photon conversion
   
    Trk::NeutralCurvilinearParameters neu(position,momentum,parent->charge());
    childVector=m_conversionTool->doConversionOnLayer(parent, time, neu); 

    // validation mode 
    if (m_validationMode && m_validationTool) {

      // add process info for children
      for (unsigned int i=0; i<childVector.size(); i++) {
	ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
	validInfo->setProcess(process);
	if (parent->getUserInformation()) validInfo->setGeneration(parent->getUserInformation()->generation()+1);
	else validInfo->setGeneration(1);     // assume parent is a primary track
	childVector[i]->setUserInformation(validInfo);
      }
      // save interaction info 
      if ( m_validationTool ) {
	Amg::Vector3D* nMom = 0;
	m_validationTool->saveISFVertexInfo(process, position,*parent,momentum,nMom,childVector);
	delete nMom;
      }
    }
 
    //Making sure we get some correct truth info from parent if needed before pushing into the particle broker
    for (unsigned int i=0; i<childVector.size(); i++) {
	if (!childVector[i]->getTruthBinding()) {
		childVector[i]->setTruthBinding(new ISF::TruthBinding(*parent->getTruthBinding()));
	}
    }
 
    return childVector;
  }

  if (process==121) {    // hadronic interaction

    //const Amg::Vector3D pDir = momentum.unit();

    const Trk::CurvilinearParameters parm(position,momentum,parent->charge());

    return ( m_hadIntProcessor->doHadIntOnLayer(parent, time, position, momentum,
						m_extMatProp? &m_extMatProp->material() : 0, particle) );

  }

  if ( process == 201 ) {    // decay  
    childVector = m_particleDecayer->decayParticle(*parent,parm.position(),parm.momentum(),time);
    // in the validation mode, add process info
    if (m_validationMode) {
      for (unsigned int i=0; i<childVector.size(); i++) {
	ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
	validInfo->setProcess(process);
	if (parent->getUserInformation()) validInfo->setGeneration(parent->getUserInformation()->generation()+1);
	else validInfo->setGeneration(1);     // assume parent is a primary track
        childVector[i]->setUserInformation(validInfo);
      }
    }

    // register TruthIncident
    ISF::ISFTruthIncident truth( const_cast<ISF::ISFParticle&>(*parent),
                                 childVector,
                                 process,
                                 parent->nextGeoID(),  // inherits from the parent
                                 ISF::fKillsPrimary );
    m_truthRecordSvc->registerTruthIncident( truth);

    // save info for validation
    if (m_validationMode && m_validationTool) {
      Amg::Vector3D* nMom = 0;
      m_validationTool->saveISFVertexInfo(process,parm.position(),*parent,parm.momentum(),nMom,childVector);
      delete nMom;
    }
  }

  //Making sure we get some correct truth info from parent if needed before pushing into the particle broker
  for (unsigned int i=0; i<childVector.size(); i++) {
	if (!childVector[i]->getTruthBinding()) {
		childVector[i]->setTruthBinding(new ISF::TruthBinding(*parent->getTruthBinding()));
	}
  }
    
  return childVector;
}

