/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// McMaterialEffectsEngine.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_FatrasTools/McMaterialEffectsEngine.h"

// Gaudi Kernel
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// ISF includes
#include "ISF_Interfaces/IParticleBroker.h"
#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ISFTruthIncident.h"
#include "ISF_Event/ParticleUserInformation.h"
// iFatras
#include "ISF_FatrasInterfaces/IHadronicInteractionProcessor.h"
#include "ISF_FatrasInterfaces/IProcessSamplingTool.h"
#include "ISF_FatrasInterfaces/IPhysicsValidationTool.h"
#include "ISF_FatrasInterfaces/IParticleDecayHelper.h"
#include "ISF_FatrasInterfaces/IEnergyLossSampler.h"
#include "ISF_FatrasInterfaces/IMultipleScatteringSampler.h"
// Trk inlcude
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkSurfaces/Surface.h" 
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkExInterfaces/ExtrapolationMacros.h"
#include "TrkExInterfaces/ITimedExtrapolator.h"

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandLandau.h"

// Validation mode - TTree includes
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h" 
// STD
#include <math.h>

// constructor
iFatras::McMaterialEffectsEngine::McMaterialEffectsEngine(const std::string& t, const std::string& n, const IInterface* p) :
  base_class(t,n,p),
  m_samplingTool(""),
  m_eLoss(true),
  m_eLossSampler(""),
  m_dedicatedElectronSampler(true),
  m_elEnergyLossSampler("" ),
  m_ms(true),
  m_msSampler(""),
  m_particleDecayHelper(""),  
  m_parametricScattering(true),
  m_uniformHertzDipoleAngle(false),
  m_processCode(3),
  m_minimumMomentum(50.*CLHEP::MeV),
  m_minimumBremPhotonMomentum(50.*CLHEP::MeV),
  m_createBremPhoton(true),
  m_bendingCorrection(false),
  m_rndGenSvc("AtDSFMTGenSvc", n),
  m_randomEngine(0),
  m_randomEngineName("FatrasRnd"),
  m_trackingGeometry(0),
  m_trackingGeometrySvc("AtlasTrackingGeometrySvc", n),
  m_trackingGeometryName("AtlasTrackingGeometry"),
  m_validationMode(false),
  m_validationTool(""),
  m_particleBroker("ISF_ParticleParticleBroker", n),
  m_truthRecordSvc("ISF_TruthRecordSvc", n),
  m_oneOverThree(1./3.),
  m_projectionFactor(sqrt(2.)/2.)
{
  // steering of the screen outoput (SOP)
  declareProperty("OutputPrefix"                        , m_sopPrefix);
  declareProperty("OutputPostfix"                       , m_sopPostfix);

  // Tool Parameters 
  // Energy Loss Samplers
  declareProperty("EnergyLoss"                          , m_eLoss);
  declareProperty("EnergyLossSampler"                   , m_eLossSampler);
  // Dedicated electron Energy Loss Samplers
  declareProperty("UseElectronSampler"                  , m_dedicatedElectronSampler );
  declareProperty("ElectronEnergyLossSampler"           , m_elEnergyLossSampler);
  // Multiple Scattering Sampler
  declareProperty("MultipleScattering"                  , m_ms);
  declareProperty("MultipleScatteringSampler"           , m_msSampler);
  // tool handle for the particle decayer
  declareProperty( "ParticleDecayHelper"                , m_particleDecayHelper      );  
  // Process sampling
  declareProperty( "ProcessSamplingTool"                , m_samplingTool             );

  // MC Truth Properties
  declareProperty("BremProcessCode"                     , m_processCode, "MCTruth Physics Process Code");
  
  // The Steering
  declareProperty("ParametericScattering"               , m_parametricScattering);
  declareProperty("MomentumCut"                         , m_minimumMomentum);
  declareProperty("MinimumBremPhotonMomentum"           , m_minimumBremPhotonMomentum);
  declareProperty("CreateBremPhotons"                   , m_createBremPhoton);
  declareProperty("BendingCorrection"                   , m_bendingCorrection);

  // Validation mode
  declareProperty("ValidationMode"                      , m_validationMode);
  declareProperty("PhysicsValidationTool"               , m_validationTool);

  // TrackingGeometry Service      
  declareProperty("TrackingGeometrySvc"                 , m_trackingGeometrySvc);     
  declareProperty("RandomNumberService"                 , m_rndGenSvc               , "Random number generator");
  declareProperty("RandomStreamName"                    , m_randomEngineName        , "Name of the random number stream");
  
  // ISF Services and Tools
  declareProperty("ParticleBroker"                      , m_particleBroker        , "ISF Particle Broker Svc");
  declareProperty("TruthRecordSvc"                      , m_truthRecordSvc        , "ISF Particle Truth Svc");      
}

// destructor
iFatras::McMaterialEffectsEngine::~McMaterialEffectsEngine()
{}

// Athena standard methods
// initialize
StatusCode iFatras::McMaterialEffectsEngine::initialize()
{
  EX_MSG_INFO( "", "init", "", "starting initialize()" );
    
  // retrieve the energy loss sampler
  if (m_eLoss){
    EX_MSG_VERBOSE("", "init", "", "Running WITH EnergyLossSampler ");
    if (m_eLossSampler.retrieve().isFailure()) {
      ATH_MSG_FATAL("Could not retrieve " << m_eLossSampler );
      return StatusCode::FAILURE;
    } else EX_MSG_VERBOSE("", "init", "", "Successfully retrieved " << m_eLossSampler );
  } else EX_MSG_VERBOSE("", "init", "", "Running WITHOUT EnergyLossSampler ");
  
  // retrieve the dedicated electron energy loss 
  if (m_dedicatedElectronSampler){
    EX_MSG_VERBOSE("", "init", "", "Running WITH dedicated electorn EnergyLossSampler ");
    // Retrieve the energy loss sampler tool for electrons
    if ( m_elEnergyLossSampler.retrieve().isFailure() ){
      ATH_MSG_FATAL( "Could not retrieve " << m_elEnergyLossSampler );
      return StatusCode::FAILURE;
    } else EX_MSG_VERBOSE("", "init", "", "Successfully retrieved " << m_elEnergyLossSampler );
  } else EX_MSG_VERBOSE("", "init", "", "Running WITHOUT dedicated electorn EnergyLossSampler ");
  
  // retrieve the multiple scattering sampler tool
  if (m_ms){
    EX_MSG_VERBOSE("", "init", "", "Running WITH MultipleScatteringSamper ");
    if (m_msSampler.retrieve().isFailure()){
      ATH_MSG_FATAL( "Could not retrieve " << m_msSampler );
      return StatusCode::FAILURE;
    } else EX_MSG_VERBOSE("", "init", "", "Successfully retrieved " << m_msSampler );
  } else EX_MSG_VERBOSE("", "init", "", "Running WITHOUT MultipleScatteringSamper ");

   // retrieve the process sampling tool
  if (m_samplingTool.retrieve().isFailure()){
    ATH_MSG_FATAL( "Could not retrieve " << m_samplingTool );
    return StatusCode::FAILURE;
  } else EX_MSG_VERBOSE("", "init", "", "Successfully retrieved " << m_samplingTool );
  
   // retrieve the decayHelper
  if (m_particleDecayHelper.retrieve().isFailure()){
    ATH_MSG_FATAL( "Could not retrieve " << m_particleDecayHelper );
    return StatusCode::FAILURE;
  } else EX_MSG_VERBOSE("", "init", "", "Successfully retrieved " << m_particleDecayHelper );
  
  // get the random generator service
  if (m_rndGenSvc.retrieve().isFailure()){
    ATH_MSG_FATAL( "Could not retrieve " << m_rndGenSvc );
    return StatusCode::FAILURE;
  } else EX_MSG_VERBOSE("", "init", "", "Successfully retrieved " <<  m_rndGenSvc );
  
  // get own engine with own seeds:
  m_randomEngine = m_rndGenSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine) {
    ATH_MSG_FATAL( "Could not get random engine '" << m_randomEngineName <<  "'" );
    return StatusCode::FAILURE;
  } else EX_MSG_VERBOSE("", "init", "", "Successfully got random engine '" << m_randomEngineName << "'" );
  
  // get the tracking geometry for layer lookup     
  // get the TrackingGeometrySvc
  if (m_trackingGeometrySvc.retrieve().isSuccess()){
    EX_MSG_VERBOSE("", "init", "", "Successfully retrieved " << m_trackingGeometrySvc );
    m_trackingGeometryName = m_trackingGeometrySvc->trackingGeometryName();
  } else {
    ATH_MSG_WARNING( "Couldn't retrieve " << m_trackingGeometrySvc << ". " );
    ATH_MSG_WARNING( " -> Trying to retrieve default '" << m_trackingGeometryName << "' from DetectorStore." );
  }     
  
  // ISF Services
  if (m_particleBroker.retrieve().isFailure()){
    ATH_MSG_FATAL( "Could not retrieve " <<  m_particleBroker );
    return StatusCode::FAILURE;
  } else EX_MSG_VERBOSE("", "init", "", "Successfully retrieved " << m_particleBroker );
  
  if (m_truthRecordSvc.retrieve().isFailure()){
    ATH_MSG_FATAL( "Could not retrieve " << m_truthRecordSvc );
    return StatusCode::FAILURE;
  } else EX_MSG_VERBOSE("", "init", "", "Successfully retrieved " << m_truthRecordSvc );

  if (m_validationMode){
    // retrieve the physics validation tool
    if (m_validationTool.retrieve().isFailure()){
      ATH_MSG_FATAL( "Could not retrieve " <<  m_validationTool );
      return StatusCode::FAILURE;
    } else EX_MSG_VERBOSE("", "init", "", "Successfully retrieved " <<  m_validationTool );
  }

  return StatusCode::SUCCESS;
}

// finalize
StatusCode iFatras::McMaterialEffectsEngine::finalize()
{
  EX_MSG_INFO("", "fini ", "", "finalize() successful" );    
  return StatusCode::SUCCESS;
}

Trk::ExtrapolationCode iFatras::McMaterialEffectsEngine::handleMaterial(Trk::ExCellCharged& ecCharged,
									Trk::PropDirection dir,
									Trk::MaterialUpdateStage matupstage) const
{ 
  EX_MSG_DEBUG(++ecCharged.navigationStep, "handleMaterial", "char", "handleMaterial for charge particle called."); 
  return handleMaterialT<Trk::TrackParameters> (ecCharged, dir, matupstage);
  
}

Trk::ExtrapolationCode iFatras::McMaterialEffectsEngine::handleMaterial(Trk::ExCellNeutral& ecNeutral, 
									Trk::PropDirection dir,
									Trk::MaterialUpdateStage matupstage) const
{ 
  EX_MSG_DEBUG(++ecNeutral.navigationStep, "handleMaterial", "neut", "handleMaterial for neutral particle called."); 
  return handleMaterialT<Trk::NeutralParameters> (ecNeutral, dir, matupstage); 
}


void iFatras::McMaterialEffectsEngine::multipleScatteringUpdate(const Trk::TrackParameters& pars,
								AmgVector(5)& parameters,
								double simTheta, double num_deltaPhi) const
{
  
  // parametric scattering - independent in x/y 
  if (m_parametricScattering){ 
    EX_MSG_VERBOSE("[msupdate]", "MultipleScatteringUpdate", "", "Using parametric scattering." );
    // the initial values
    double theta =  parameters[Trk::theta];
    double phi   =  parameters[Trk::phi];
    double sinTheta   = (sin(theta)*sin(theta) > 10e-10) ? sin(theta) : 1.; 
    
    // sample them in an independent way
    double deltaTheta = m_projectionFactor*simTheta;
    double deltaPhi   = m_projectionFactor*num_deltaPhi/sinTheta; 
    
    phi += deltaPhi;
    if (phi >= M_PI) phi -= M_PI;
    else if (phi < -M_PI) phi += M_PI;
    if (theta > M_PI) theta -= M_PI;
    
    EX_MSG_VERBOSE("[msupdate]", "MultipleScatteringUpdate", "", "deltaPhi / deltaTheta = " << deltaPhi << " / " << deltaTheta );
    
    // assign the new values
    parameters[Trk::phi]   = phi;   
    parameters[Trk::theta] = fabs(theta + deltaTheta);

  } else {
    double thetaMs = simTheta;
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

    EX_MSG_VERBOSE("[msupdate]", "MultipleScatteringUpdate", "", "deltaPsi / deltaTheta = " << psi << " / " << thetaMs );

    // assign the new values
    parameters[Trk::phi]   = newDirectionHep.phi();   
    parameters[Trk::theta] = newDirectionHep.theta();
  }

}

void iFatras::McMaterialEffectsEngine::recordBremPhoton(const ISF::ISFParticle* parent,
							double time,
							double pElectron,
							double gammaE,
							const Amg::Vector3D& vertex,
							Amg::Vector3D& particleDir,
							Trk::ParticleHypothesis particle) const
{
  ISF::ISFParticle *bremPhot = bremPhoton(parent,time,pElectron,gammaE,vertex,particleDir,particle);
  m_particleBroker->push(bremPhot, parent);

}

void iFatras::McMaterialEffectsEngine::recordBremPhotonLay(const ISF::ISFParticle* parent,
							   double time,
							   double pElectron,
							   double gammaE,
							   const Amg::Vector3D& vertex,
							   Amg::Vector3D& particleDir,
							   Trk::ParticleHypothesis particle,
							   Trk::PropDirection dir,
							   float matFraction ) const {

  ISF::ISFParticle *bremPhot = bremPhoton(parent,time,pElectron,gammaE,vertex,particleDir,particle);

  // process the photon on layer 
  if (gammaE<m_minimumMomentum) return;
  Trk::PathLimit pLim = m_samplingTool->sampleProcess(gammaE,0,Trk::photon);

  // save presampled information
  ISF::ParticleUserInformation* childInfo =  bremPhot->getUserInformation();
  if (!childInfo) {
    childInfo = new ISF::ParticleUserInformation();
    bremPhot->setUserInformation(childInfo);
  }
  childInfo->setMaterialLimit(pLim.process,pLim.x0Max,0.); 
      
  // material fraction : flip if direction of propagation changed
  float ci = m_layer->surfaceRepresentation().normal().dot( particleDir );
  float co = m_layer->surfaceRepresentation().normal().dot( bremPhot->momentum().unit() );
  float remMat =  ci*co <0 ? (1-matFraction)    : matFraction;

  Trk::NeutralCurvilinearParameters  nParm(bremPhot->position(),bremPhot->momentum(),bremPhot->charge());
  Trk::ExtrapolationCell< Trk::NeutralParameters > enc(nParm);
  enc.addConfigurationMode(Trk::ExtrapolationMode::FATRAS);   

  enc.leadLayer = m_layer;
       
  if (pLim.x0Max>0) {
    enc.addConfigurationMode(Trk::ExtrapolationMode::StopWithMaterialLimitX0);
    enc.materialLimitX0 = pLim.x0Max;
  }
  enc.materialProcess = pLim.process;
  enc.materialX0 = 0.;
  
  // loop back      
  processMaterialOnLayer(bremPhot,enc,dir,remMat);	

}

ISF::ISFParticle* iFatras::McMaterialEffectsEngine::bremPhoton(const ISF::ISFParticle* parent,
							       double time,
							       double pElectron,
							       double gammaE,
							       const Amg::Vector3D& vertex,
							       Amg::Vector3D& particleDir,Trk::ParticleHypothesis particle) const
{
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
  
  EX_MSG_VERBOSE("[ brem ]", "BremPhoton", "", "Simulated angle to electron    = " << theta << "." );
  
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
     
  // -------> create the brem photon <--------------------
  ISF::ISFParticle *bremPhoton = new ISF::ISFParticle( vertex,
						       gammaE*newDirection,
						       0,   //!< mass
						       0,   //!< charge
						       22,  //!< pdg code
						       time,  //!< time
						       *parent );
  
  // register TruthIncident
  ISF::ISFParticleVector children(1, bremPhoton);
  ISF::ISFTruthIncident truth( const_cast<ISF::ISFParticle&>(*parent),
			       children,
			       m_processCode,
			       parent->nextGeoID(),
			       ISF::fPrimarySurvives );
  m_truthRecordSvc->registerTruthIncident(truth);

  if (m_validationMode) {
    ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
    validInfo->setProcess(m_processCode);
    if (parent->getUserInformation()) validInfo->setGeneration(parent->getUserInformation()->generation()+1);
    else validInfo->setGeneration(1);     // assume parent is a primary track
    bremPhoton->setUserInformation(validInfo);

    // save interaction info 
    if ( m_validationTool ) {
      Amg::Vector3D* nMom = new Amg::Vector3D(inEl);
      m_validationTool->saveISFVertexInfo(m_processCode, vertex,*parent,inEl+gammaE*newDirection,nMom,children);
      delete nMom; 
    }


  }

  //Making sure we get some correct truth info from parent if needed before pushing into the particle broker
  if (!bremPhoton->getTruthBinding()) {
         bremPhoton->setTruthBinding(new ISF::TruthBinding(*parent->getTruthBinding()));
  }

  return bremPhoton;
  
} 

Trk::ExtrapolationCode iFatras::McMaterialEffectsEngine::processMaterialOnLayer( const ISF::ISFParticle* isp,
							       Trk::ExCellCharged& eCell,
							       Trk::PropDirection dir,
							       float& mFraction) const
{   

  const Trk::TrackParameters* parm = eCell.leadParameters;
    
  // path correction
  double pathCorrection = m_layer->surfaceRepresentation().normal().dot(parm->momentum()) !=0 ?
    fabs(m_layer->surfaceRepresentation().pathCorrection(parm->position(),dir*(parm->momentum()))) : 1.;

  if (!pathCorrection) return Trk::ExtrapolationCode::InProgress;
  
  // figure out if particle stopped in the layer and recalculate path limit
  int iStatus = 0;
  float dX0 = (1.-mFraction)*pathCorrection*m_thicknessInX0;
  float dL0 = (1.-mFraction)*pathCorrection*m_thicknessInL0;
  
  if ( eCell.materialLimitX0>0 && eCell.materialProcess<100 && eCell.materialX0+dX0>= eCell.materialLimitX0) {      // elmg. interaction
    float x0rem = eCell.materialLimitX0 - eCell.materialX0;
    dL0 *= x0rem>0 ? x0rem/dX0 : 1.; 
    if ( x0rem>0 ) dX0 = x0rem;
    iStatus = 1; 
  } else if ( eCell.materialLimitL0>0 && eCell.materialProcess>100 && eCell.materialL0+dL0 >= eCell.materialLimitL0 )   {     // hadronic interaction
    float l0rem = eCell.materialLimitX0 - eCell.materialL0 ;
    dX0 *= l0rem>0 ? l0rem/dL0 : 1.;
    if ( l0rem>0 ) dL0 = l0rem;
    iStatus = 2; 
  }
  
  // material update
  // eCell.materialX0 += dX0;
  // eCell.materialL0 += dL0;
  // eCell.zOaTrX     += m_matProp->zOverAtimesRho()*dX0;
  // eCell.zX         += m_matProp->averageZ()*dX0;

  // check if material filling was requested
  if (eCell.checkConfigurationMode(Trk::ExtrapolationMode::CollectMaterial)) {
     EX_MSG_VERBOSE(eCell.navigationStep, "Update", "char", "collecting material at layer " << m_layer->layerIndex().value() << " of - t/X0 = " << m_matProp->thicknessInX0());
     eCell.stepMaterial(parm->associatedSurface(), m_layer, parm->position(), (1.-mFraction)*pathCorrection, m_matProp);
   } else {
     EX_MSG_VERBOSE(eCell.navigationStep, "Update", "char", "adding material of - t/X0 = " << m_matProp->thicknessInX0());
     eCell.addMaterial((1.-mFraction)*pathCorrection, m_matProp);
   }
  
  // get the kinematics
  double p    = parm->momentum().mag();
  double m    = m_particleMasses.mass[eCell.pHypothesis];
  double E    = sqrt(p*p+m*m);

  // radiation and ionization preceed the presampled interaction (if any) 

  if (m_eLoss || m_ms) {
    // the updatedParameters - first a deep copy
    AmgVector(5)      uParameters = parm->parameters();
    
    if ( m_eLoss ) {

      // smeared/presampled energy loss
      Trk::EnergyLoss* eloss = (eCell.pHypothesis==Trk::electron && m_dedicatedElectronSampler) ? m_elEnergyLossSampler->energyLoss(*m_matProp, p, dX0/m_matProp->thicknessInX0(), dir, eCell.pHypothesis) : m_eLossSampler->energyLoss(*m_matProp, p, dX0/m_matProp->thicknessInX0(), dir, eCell.pHypothesis);
      
      if (eCell.pHypothesis==Trk::electron && m_createBremPhoton) { 
	
	// ionization update
	
	double newP = E+eloss->meanIoni()>m ? sqrt((E+eloss->meanIoni())*(E+eloss->meanIoni())-m*m) : 0.5*m_minimumMomentum;
	uParameters[Trk::qOverP] = parm->charge()/newP;

	// radiation 
	if (newP>m_minimumMomentum)
	  radiate(isp,uParameters,eCell,dX0,mFraction,dir,pathCorrection*m_thicknessInX0);   // mFraction used to estimate material thickness for brem photons
	
	// save the actual radiation loss
	float nqOp = uParameters[Trk::qOverP];
	float radLoss = fabs(1./nqOp) - newP;
	eloss->update(0.,0.,radLoss-eloss->meanRad(),eloss->meanRad()-radLoss);
	
      } else {
	
	// calculate the new momentum
	double newP = E+eloss->deltaE()>m ? sqrt((E+eloss->deltaE())*(E+eloss->deltaE())-m*m) : 0.5*m_minimumMomentum;
	uParameters[Trk::qOverP] = parm->charge()/newP;
	
      }

      EX_MSG_VERBOSE(eCell.navigationStep, "eloss", "char ", "E,deltaE:" <<E<<","<< eloss->deltaE() );
      
      // TODO straggling 
      
      if (m_validationMode) {
	if (eCell.eLoss) { 
	  eCell.eLoss->update(*eloss);
	  delete eloss;
	} else eCell.eLoss=eloss;
      } else delete eloss;
      
      if ( 1./fabs(uParameters[Trk::qOverP]) < m_minimumMomentum ) {
	
	// save info for locally created particle
	if (m_validationMode && isp!=m_isp) {
	  EX_MSG_VERBOSE( "[validation]", "processMaterialOnLayer", "char", "saving interaction info for locally produced particle " << isp->pdgCode() );
	  m_validationTool->saveISFParticleInfo(*isp,eCell,Trk::ExtrapolationCode::SuccessMaterialLimit); 
	}
	
	if (isp!=m_isp) { delete isp; }      
	return Trk::ExtrapolationCode::SuccessMaterialLimit;
      }
      
    }
    
    
    if ( m_ms && m_thicknessInX0>0 ) {

      double simTheta = m_msSampler->simTheta(*m_matProp, p, dX0/m_thicknessInX0, eCell.pHypothesis);
      //do the update -> You need 2 evaluation of simTheta. The second one is used to calculate deltaphi in multipleScatteringUpdate
      multipleScatteringUpdate(*(eCell.leadParameters), uParameters, simTheta, 
			       m_msSampler->simTheta(*m_matProp, p, dX0/m_thicknessInX0, eCell.pHypothesis));
      
    }    
    
    // use the manipulator to update the track parameters -------> get rid of 0!
    const Trk::TrackParameters* upd = eCell.leadParameters->associatedSurface().createTrackParameters(uParameters[0],
												      uParameters[1],
												      uParameters[2],
												      uParameters[3],
												      uParameters[4]);
    
    // memory handling for locally created particles ( TODO : add hits for them by collecting active, too)
    eCell.leadParameters=upd; 
    // ST fixing memory leak
    // if (isp!=m_isp) eCell.stepParameters(eCell.leadParameters,Trk::ExtrapolationMode::CollectPassive); 
    eCell.stepParameters(eCell.leadParameters,Trk::ExtrapolationMode::CollectPassive); 

    if (upd->momentum().mag() < m_minimumMomentum ) {
      if (isp!=m_isp) { delete isp;}
      return Trk::ExtrapolationCode::SuccessMaterialLimit;
    }
  }

  mFraction += dX0/pathCorrection/m_thicknessInX0;  
  
  if ( iStatus==1 || iStatus==2 ) {   // interaction with particle stopping
    
    ISF::ISFParticleVector childs = m_samplingTool->interact(isp,eCell);    // TODO pass material when needed
          
    // save info for locally created particles
    if (m_validationMode && isp!=m_isp) {
      EX_MSG_VERBOSE( "[validation]", "processMaterialOnLayer", "char", "saving interaction info for locally produced particle " << isp->pdgCode() );
      m_validationTool->saveISFParticleInfo(*isp,eCell,Trk::ExtrapolationCode::SuccessMaterialLimit); 
    }
    
    // loop over childrens and decide about their fate
    
    for (unsigned ic=0; ic<childs.size(); ic++) {
      double mom = childs[ic]->momentum().mag();
      
      if (mom<m_minimumMomentum) continue;
      Trk::ParticleHypothesis pHypothesis = m_pdgToParticleHypothesis.convert(childs[ic]->pdgCode(),childs[ic]->charge());
      Trk::PathLimit pLim = m_samplingTool->sampleProcess(mom,childs[ic]->charge(),pHypothesis);
      double pathLimit = ( !m_particleDecayHelper.empty()) ? m_particleDecayHelper->freePath(*childs[ic]) : -1.; 
      
      // TODO sample decays and save the material collection & path limits at the exit from the layer (ISFFatrasParticle ?)
      
      // material fraction : flip if direction of propagation changed
      float ci = m_layer->surfaceRepresentation().normal().dot( parm->momentum().unit() );
      float co = m_layer->surfaceRepresentation().normal().dot( childs[ic]->momentum().unit() );
      float remMat =  ci*co <0 ? (1-mFraction)    : mFraction;
      
      // save presampled information
      ISF::ParticleUserInformation* childInfo =  childs[ic]->getUserInformation();
      if (!childInfo) {
	childInfo = new ISF::ParticleUserInformation();
	childs[ic]->setUserInformation(childInfo);
      }
      childInfo->setMaterialLimit(pLim.process,pLim.x0Max,0.); 
      //std::cout <<"child info saved on isp:"<<childs[ic]<<","<<childs[ic]->getUserInformation()->materialLimit()<< std::endl;

      // configure child cell and loop back
      if (childs[ic]->charge()!=0) {
	Trk::CurvilinearParameters  cParm(childs[ic]->position(),childs[ic]->momentum(),childs[ic]->charge());
	Trk::ExtrapolationCell< Trk::TrackParameters > ecc(cParm);
	ecc.addConfigurationMode(Trk::ExtrapolationMode::FATRAS);   
	
	if (pathLimit>0) {
	  ecc.addConfigurationMode(Trk::ExtrapolationMode::StopWithPathLimit);
	  ecc.pathLimit = pathLimit;
	}
	
	if (pLim.x0Max>0) {
	  if (pLim.process==121) {
	    ecc.addConfigurationMode(Trk::ExtrapolationMode::StopWithMaterialLimitL0);
	    ecc.materialLimitL0 = pLim.x0Max;
	  } else {
	    ecc.addConfigurationMode(Trk::ExtrapolationMode::StopWithMaterialLimitX0);
	    ecc.materialLimitX0 = pLim.x0Max;
	  }
	  ecc.materialProcess = pLim.process;
	}
        ecc.leadLayer = m_layer;
	
	// loop back      
	Trk::ExtrapolationCode exitCode=processMaterialOnLayer(childs[ic],ecc,dir,remMat);
  
        if (exitCode == Trk::ExtrapolationCode::SuccessMaterialLimit )
	  EX_MSG_VERBOSE( "", "processMaterialOnLayer", "", "child particle:"<< ic << "stopped in layer" );
        else 
	  EX_MSG_VERBOSE( "", "processMaterialOnLayer", "", "child particle:"<< ic << "leaving layer" );

      } else {
	Trk::NeutralCurvilinearParameters  nParm(childs[ic]->position(),childs[ic]->momentum(),childs[ic]->charge());
	Trk::ExtrapolationCell< Trk::NeutralParameters > enc(nParm);
	enc.addConfigurationMode(Trk::ExtrapolationMode::FATRAS);   
	
	if (pathLimit>0) {
	  enc.addConfigurationMode(Trk::ExtrapolationMode::StopWithPathLimit);
	  enc.pathLimit = pathLimit;
	}
       
	if (pLim.x0Max>0) {
	  if (pLim.process==121) {
	    enc.addConfigurationMode(Trk::ExtrapolationMode::StopWithMaterialLimitL0);
	    enc.materialLimitL0 = pLim.x0Max;
	  } else {
	    enc.addConfigurationMode(Trk::ExtrapolationMode::StopWithMaterialLimitX0);
	    enc.materialLimitX0 = pLim.x0Max;
	  }
	 enc.materialProcess = pLim.process;
	}
        enc.leadLayer = m_layer;

	// loop back      
	Trk::ExtrapolationCode exitCode=processMaterialOnLayer(childs[ic],enc,dir,remMat);	
  
        if (exitCode == Trk::ExtrapolationCode::SuccessMaterialLimit )
	  EX_MSG_VERBOSE( "", "processMaterialOnLayer", "", "child particle:"<< ic << "stopped in layer" );
        else 
	  EX_MSG_VERBOSE( "", "processMaterialOnLayer", "", "child particle:"<< ic << "leaving layer" ); 
      }
    }
    
    if (childs.size()>0 || true ) { // TODO: pass the interaction status
      if (isp!=m_isp) { delete isp; }
      return Trk::ExtrapolationCode::SuccessMaterialLimit;
    }

   
  
  }
  
  // register particle if not in the stack already 
  if (isp!=m_isp ) {
    ISF::ISFParticle* regisp=new ISF::ISFParticle(isp->position(),parm->momentum(),isp->mass(),isp->charge(),
						  isp->pdgCode(),isp->timeStamp(),*m_isp,isp->barcode());
    // add presampled process info 
    if (isp->getUserInformation() && isp->getUserInformation()->materialLimit()) {
      ISF::MaterialPathInfo* matLim = isp->getUserInformation()->materialLimit();
      ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
      validInfo->setMaterialLimit(matLim->process,matLim->dMax,matLim->process==121 ? eCell.materialL0 : eCell.materialX0);
      regisp->setUserInformation(validInfo);
      //std::cout <<"collected material at layer exit:"<<regisp->getUserInformation()->materialLimit()->dCollected<<std::endl;
    }
    // validation mode
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

  if (isp!=m_isp) delete isp;

  return Trk::ExtrapolationCode::InProgress;   
}

Trk::ExtrapolationCode iFatras::McMaterialEffectsEngine::processMaterialOnLayer( const ISF::ISFParticle* isp,
							       Trk::ExCellNeutral& eCell,
							       Trk::PropDirection dir,
							       float& mFraction) const
{   

  const Trk::NeutralParameters* parm = eCell.leadParameters;
    
  // path correction
  double pathCorrection = m_layer->surfaceRepresentation().normal().dot(parm->momentum()) !=0 ?
    fabs(m_layer->surfaceRepresentation().pathCorrection(parm->position(),dir*(parm->momentum()))) : 1.;

  if (!pathCorrection) return Trk::ExtrapolationCode::InProgress;
  
  // figure out if particle stopped in the layer and recalculate path limit
  int iStatus = 0;
  float dX0 = (1.-mFraction)*pathCorrection*m_thicknessInX0;
  float dL0 = (1.-mFraction)*pathCorrection*m_thicknessInL0;
  
  if ( eCell.materialLimitX0>0 && eCell.materialProcess<100 && eCell.materialX0+dX0>= eCell.materialLimitX0) {      // elmg. interaction
    float x0rem = eCell.materialLimitX0 - eCell.materialX0;
    dL0 *= x0rem>0 ? x0rem/dX0 : 1.; 
    if ( x0rem>0 ) dX0 = x0rem;
    iStatus = 1; 
  } else if ( eCell.materialLimitL0>0 && eCell.materialProcess>100 && eCell.materialL0+dL0 >= eCell.materialLimitL0 )   {     // hadronic interaction
    float l0rem = eCell.materialLimitX0 - eCell.materialL0 ;
    dX0 *= l0rem>0 ? l0rem/dL0 : 1.;
    if ( l0rem>0 ) dL0 = l0rem;
    iStatus = 2; 
  }
  
  // material update
  // eCell.materialX0 += dX0;
  // eCell.materialL0 += dL0;
  // eCell.zOaTrX     += m_matProp->zOverAtimesRho()*dX0;
  // eCell.zX         += m_matProp->averageZ()*dX0;

  // check if material filling was requested
  if (eCell.checkConfigurationMode(Trk::ExtrapolationMode::CollectMaterial)) {
     EX_MSG_VERBOSE(eCell.navigationStep, "Update", "neut", "collecting material at layer " << m_layer->layerIndex().value() << " of - t/X0 = " << m_matProp->thicknessInX0());
     eCell.stepMaterial(parm->associatedSurface(), m_layer, parm->position(), (1.-mFraction)*pathCorrection, m_matProp);
  } else {
  EX_MSG_VERBOSE(eCell.navigationStep, "Update", "neut", "adding material of - t/X0 = " << m_matProp->thicknessInX0());
  eCell.addMaterial((1.-mFraction)*pathCorrection, m_matProp);
  }
  
  // get the kinematics
  //double p    = parm->momentum().mag();
  //double m    = m_particleMasses.mass[eCell.pHypothesis];
  //double E    = sqrt(p*p+m*m);
    
  if ( iStatus==1 || iStatus==2 ) {   // interaction with particle stopping
    
    ISF::ISFParticleVector childs = m_samplingTool->interact(isp,eCell);     // TODO pass material when needed
    
    // save info for locally created particles
    if (m_validationMode && isp!=m_isp) {
      EX_MSG_VERBOSE( "[validation]", "processMaterialOnLayer", "neut", "saving interaction info for locally produced particle " << isp->pdgCode() );
      m_validationTool->saveISFParticleInfo(*isp,eCell,Trk::ExtrapolationCode::SuccessMaterialLimit); 
    }
    
    // loop over childrens and decide about their fate
    
    for (unsigned ic=0; ic<childs.size(); ic++) {
      double mom = childs[ic]->momentum().mag();
      
      if (mom<m_minimumMomentum) continue;
      Trk::ParticleHypothesis pHypothesis = m_pdgToParticleHypothesis.convert(childs[ic]->pdgCode(),childs[ic]->charge());
      Trk::PathLimit pLim = m_samplingTool->sampleProcess(mom,childs[ic]->charge(),pHypothesis);
      double pathLimit = ( !m_particleDecayHelper.empty()) ? m_particleDecayHelper->freePath(*childs[ic]) : -1.; 
      
      // TODO sample decays and save the material collection & path limits at the exit from the layer (ISFFatrasParticle ?)
      
      // material fraction : flip if direction of propagation changed
      float ci = m_layer->surfaceRepresentation().normal().dot( parm->momentum().unit() );
      float co = m_layer->surfaceRepresentation().normal().dot( childs[ic]->momentum().unit() );
      float remMat =  ci*co <0 ? (1-mFraction)    : mFraction;

      // save presampled information
      ISF::ParticleUserInformation* childInfo =  childs[ic]->getUserInformation();
      if (!childInfo) {
	childInfo = new ISF::ParticleUserInformation();
	childs[ic]->setUserInformation(childInfo);
      }
      childInfo->setMaterialLimit(pLim.process,pLim.x0Max,0.); 
      
      // configure child cell and loop back
      if (childs[ic]->charge()!=0) {
	Trk::CurvilinearParameters  cParm(childs[ic]->position(),childs[ic]->momentum(),childs[ic]->charge());
	Trk::ExtrapolationCell< Trk::TrackParameters > ecc(cParm);
	ecc.addConfigurationMode(Trk::ExtrapolationMode::FATRAS);   
	
	if (pathLimit>0) {
	  ecc.addConfigurationMode(Trk::ExtrapolationMode::StopWithPathLimit);
	  ecc.pathLimit = pathLimit;
	}
	
	if (pLim.x0Max>0) {
	  if (pLim.process==121) {
	    ecc.addConfigurationMode(Trk::ExtrapolationMode::StopWithMaterialLimitL0);
	    ecc.materialLimitL0 = pLim.x0Max;
	  } else {
	    ecc.addConfigurationMode(Trk::ExtrapolationMode::StopWithMaterialLimitX0);
	    ecc.materialLimitX0 = pLim.x0Max;
	  }
	  ecc.materialProcess = pLim.process;
	}
	
	// loop back      
	Trk::ExtrapolationCode exitCode = processMaterialOnLayer(childs[ic],ecc,dir,remMat);
  
        if (exitCode == Trk::ExtrapolationCode::SuccessMaterialLimit )
	  EX_MSG_VERBOSE( "", "processMaterialOnLayer", "", "child particle:"<< ic << "stopped in layer" );
        else 
	  EX_MSG_VERBOSE( "", "processMaterialOnLayer", "", "child particle:"<< ic << "leaving layer" ); 

        // cleanup
        ecc.finalize(exitCode);

      } else {
	Trk::NeutralCurvilinearParameters  nParm(childs[ic]->position(),childs[ic]->momentum(),childs[ic]->charge());
	Trk::ExtrapolationCell< Trk::NeutralParameters > enc(nParm);
	enc.addConfigurationMode(Trk::ExtrapolationMode::FATRAS);   
	
	if (pathLimit>0) {
	  enc.addConfigurationMode(Trk::ExtrapolationMode::StopWithPathLimit);
	  enc.pathLimit = pathLimit;
	}
       
	if (pLim.x0Max>0) {
	  if (pLim.process==121) {
	    enc.addConfigurationMode(Trk::ExtrapolationMode::StopWithMaterialLimitL0);
	    enc.materialLimitL0 = pLim.x0Max;
	  } else {
	    enc.addConfigurationMode(Trk::ExtrapolationMode::StopWithMaterialLimitX0);
	    enc.materialLimitX0 = pLim.x0Max;
	  }
	 enc.materialProcess = pLim.process;
	}
	
	// loop back      
	Trk::ExtrapolationCode exitCode = processMaterialOnLayer(childs[ic],enc,dir,remMat);	

        if (exitCode == Trk::ExtrapolationCode::SuccessMaterialLimit )
	  EX_MSG_VERBOSE( "", "processMaterialOnLayer", "", "child particle:"<< ic << "stopped in layer" );
        else 
	  EX_MSG_VERBOSE( "", "processMaterialOnLayer", "", "child particle:"<< ic << "leaving layer" ); 

        // cleanup
        enc.finalize(exitCode);

      }
    }
    
    if (childs.size()>0 || true) { // TODO : pass the interaction status 
      if (isp!=m_isp) { delete isp; }
      return Trk::ExtrapolationCode::SuccessMaterialLimit;
    }
  }
  
  // register particle if not in the stack already
  if (isp!=m_isp ) {
    ISF::ISFParticle* regisp=new ISF::ISFParticle(isp->position(),parm->momentum(),isp->mass(),isp->charge(),
						  isp->pdgCode(),isp->timeStamp(),*m_isp,isp->barcode());
    // add presampled process info 
    if (isp->getUserInformation() && isp->getUserInformation()->materialLimit()) {
      ISF::MaterialPathInfo* matLim = isp->getUserInformation()->materialLimit();
      ISF::ParticleUserInformation* validInfo = new ISF::ParticleUserInformation();
      validInfo->setMaterialLimit(matLim->process,matLim->dMax,matLim->process==121 ? eCell.materialL0 : eCell.materialX0);
      regisp->setUserInformation(validInfo);
    }
    // validation mode
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
  if (isp!=m_isp) delete isp;
  
  return Trk::ExtrapolationCode::InProgress;   
}


// radiative effects
void iFatras::McMaterialEffectsEngine::radiate(const ISF::ISFParticle* parent, AmgVector(5)& parm ,
					       Trk::ExCellCharged& eCell, float pathLim, float mFr,
					       Trk::PropDirection dir, float refX) const {
 
  // sample energy loss and free path independently
  double path = 0.;
  double p = 1./ fabs(parm[Trk::qOverP]);

  Amg::Vector3D eDir = eCell.leadParameters->momentum().unit();
  Amg::Vector3D ePos = eCell.leadParameters->position();

  while ( path < pathLim && p>m_minimumMomentum ) {

    double rndx = CLHEP::RandFlat::shoot(m_randomEngine);

    // sample visible fraction of the mother momentum taken according to 1/f  
 
    double eps = fmin(10.,m_minimumMomentum)/p;

    double z = pow(eps,pow(rndx,exp(1.1)));          // adjustment here ? 

    // convert into scaling factor for mother momentum
    z = (1.- z); 

    // turn into path   
    //double dx = -log(z)*0.65;     // adjust for mean of exp(-x)  
    double dx = -0.7*log(z);     // adjust for mean of exp(-x)  
 
    // sample free path (in X0)
    //double rnd = CLHEP::RandFlat::shoot(m_randomEngine);
    //double dx = -log(rnd)/13.;

    // resolve the case when there is not enough material left in the layer
    if ( path+dx > pathLim ) {
      double rndp = CLHEP::RandFlat::shoot(m_randomEngine);
      if (rndp > (pathLim-path)/dx){        
	(parm)[Trk::qOverP] = (parm)[Trk::qOverP]>0 ? 1/p : -1/p; 
	mFr += (pathLim-path)/refX; 
        path = pathLim;
	break;                   // radiation loop ended         
      } 
      path += dx*rndp;
      mFr += dx*rndp/refX; 
      
    } else { 
      path+=dx;
      mFr += dx/refX; 
    }
    if ( p*(1-z) > m_minimumBremPhotonMomentum ) {

      double deltaP = (1-z)*p;
      recordBremPhotonLay(parent,eCell.time,p,deltaP,ePos,eDir,eCell.pHypothesis,dir,mFr);
      p *=z ;

      EX_MSG_VERBOSE("", "radiate", "", "brem photon emitted " << deltaP<<":updated e momentum:"<< p   );
      // std::cout <<"brem photon emitted, momentum update:"<< p<<","<<path<<","<<pathLim<<",mat.fraction:"<<mFr<<std::endl; 
    }    
  }

  (parm)[Trk::qOverP] = (parm)[Trk::qOverP] > 0 ? 1/p : -1./p; 
  (parm)[Trk::theta]  = eDir.theta(); 
  (parm)[Trk::phi]    = eDir.phi(); 
  return; 
}
