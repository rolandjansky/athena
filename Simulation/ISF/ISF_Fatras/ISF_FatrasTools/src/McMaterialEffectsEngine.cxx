/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// McMaterialEffectsEngine.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_FatrasTools/McMaterialEffectsEngine.h"

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
// iFatras
// #include "ISF_FatrasInterfaces/IHadronicInteractionProcessor.h"
// #include "ISF_FatrasInterfaces/IProcessSamplingTool.h"
// #include "ISF_FatrasInterfaces/IPhysicsValidationTool.h"
// #include "ISF_FatrasInterfaces/IPhotonConversionTool.h"
// #include "ISF_FatrasInterfaces/IParticleDecayer.h"
// FastSimulation
// #include "FastSimulationEvent/GenParticleEnergyDepositMap.h"
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
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Random/RandLandau.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
// Validation mode - TTree includes
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h" 
// STD
#include <math.h>

// temporary
#include "TrkGeometry/TrackingVolume.h"

// constructor
iFatras::McMaterialEffectsEngine::McMaterialEffectsEngine(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_eLoss(true),
  m_eLossUpdator("iFatras::McEnergyLossUpdator/FatrasEnergyLossUpdator"),
  m_ms(true),
  m_msUpdator("Trk::MultipleScatteringUpdator/AtlasMultipleScatteringUpdator"),  
  m_minimumMomentum(50.*CLHEP::MeV),
  m_use_msUpdator(false),
  m_referenceMaterial(true),
  m_bendingCorrection(false),
  m_rndGenSvc("AtDSFMTGenSvc", n),
  m_randomEngine(0),
  m_randomEngineName("FatrasRnd"),
  m_trackingGeometry(0),
  m_trackingGeometrySvc("AtlasTrackingGeometrySvc", n),
  m_trackingGeometryName("AtlasTrackingGeometry"),
  m_particleBroker("ISF_ParticleParticleBroker", n),
  m_truthRecordSvc("ISF_TruthRecordSvc", n),
  m_layer(0)
{
  declareInterface<Trk::IMaterialEffectsEngine>(this);
  // the tool parameters -----------------------------------------------------
  declareProperty("EnergyLoss"                          , m_eLoss);
  declareProperty("EnergyLossUpdator"                   , m_eLossUpdator);
  declareProperty("MultipleScattering"                  , m_ms);
  declareProperty("MultipleScatteringUpdator"           , m_msUpdator);
  // the steering --------------------------------------------------------------
  declareProperty("UseMultipleScatteringUpdator"        , m_use_msUpdator);
  declareProperty("MomentumCut"                         , m_minimumMomentum);
  // TrackingGeometry Service      
  declareProperty("TrackingGeometrySvc",                  m_trackingGeometrySvc);     
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

  ATH_MSG_INFO( "initialize()" );
    
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
    
  // get the random generator service
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
   
  // ISF Services
  if (m_particleBroker.retrieve().isFailure()){
    ATH_MSG_FATAL( "Could not retrieve " << m_particleBroker );
    return StatusCode::FAILURE;
  }
  if (m_truthRecordSvc.retrieve().isFailure()){
    ATH_MSG_FATAL( "Could not retrieve " << m_truthRecordSvc );
    return StatusCode::FAILURE;
  }

   return StatusCode::SUCCESS;
}

// finalize
StatusCode iFatras::McMaterialEffectsEngine::finalize()
{
  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;
}

Trk::ExtrapolationCode iFatras::McMaterialEffectsEngine::handleMaterial(Trk::ExCellCharged& eCell,
									Trk::PropDirection dir,
									Trk::MaterialUpdateStage matupstage) const
{
  m_layer = eCell.leadLayer;

  // get the material properties 
  m_matProp =  0;
  
  if (m_referenceMaterial){
    if (m_layer && m_layer->layerMaterialProperties()) {
      // get the LayerMaterialProperties
      const Trk::LayerMaterialProperties* layerMaterial = m_layer->layerMaterialProperties();
      m_matProp = layerMaterial ? layerMaterial->fullMaterial( eCell.leadParameters->position() ) : 0;
    }
  }

  // the Extrapolator made sure that the layer is the lead layer && the parameters are the lead parameters
  if (eCell.leadLayer && eCell.leadLayer->layerMaterialProperties()){
    ATH_MSG_DEBUG( "handleMaterial for TrackParameters.");
    // update the track parameters   
    
    eCell.leadParameters = updateTrackParameters(*eCell.leadParameters,eCell,dir,matupstage);
  }

  

  return Trk::ExtrapolationCode::InProgress;
}


const Trk::TrackParameters* iFatras::McMaterialEffectsEngine::updateTrackParameters(  const Trk::TrackParameters& parm,
										      Trk::ExCellCharged& eCell,
										      // const Trk::Layer& lay,
										      // Trk::TimeLimit& timeLim, Trk::PathLimit& pathLim,
										      // Trk::GeometrySignature /*geoID*/,
										      Trk::PropDirection dir,
										      Trk::MaterialUpdateStage matupstage) const
                                                                                    // Trk::ParticleHypothesis particle) const
{   

  double matFraction = 0.;

  const Trk::TrackParameters* tParameters = &(parm);

  // get parent particle
  const ISF::ISFParticle *isp = ISF::ParticleClipboard::getInstance().getParticle();
  // something is seriously wrong if there is no parent particle
  assert(isp);

  m_isp = isp;
  
  //std::cout << "updateTrackParameters : processing isp:position within layer: "<<isp <<","<< isp->generation() <<","<< particle <<std::endl;
  //std::cout << "position: global, within layer:"<<parm->position()<<","<< matFraction<< std::endl;
  
  // recalculate if missing
  m_matProp = m_matProp ? m_matProp : m_layer->fullUpdateMaterialProperties(*tParameters);
  double pathCorrection = m_layer->surfaceRepresentation().pathCorrection(tParameters->position(),dir*(tParameters->momentum()));
  //pathCorrection = pathCorrection > 0. ? pathCorrection : m_layer->pathCorrection(parm); 
  
  // check if a bending correction has to be applied
  if (m_bendingCorrection) {
    ATH_MSG_WARNING("BendingCorrection not implemented!! (McMaterialEffectsEngine.cxx)");
    // const Trk::TrackingVolume* enclosingVolume = m_bendingCorrection ? m_layer->enclosingTrackingVolume() : 0;
    // if (enclosingVolume && enclosingVolume->bendingCorrector() ) { 
    //   double bendingCorrection = enclosingVolume->bendingCorrector()->bendingCorrection(*parm);
    //   pathCorrection *= bendingCorrection; 
    // }
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
  
  // figure out if particle stopped in the layer and recalculate path limit
  int iStatus = 0;
  double dX0 = (1.-matFraction)*pathCorrection*m_matProp->thicknessInX0();
  
  // get the kinematics
  double p    = (tParameters->momentum()).mag();
  
  matFraction += dX0/pathCorrection/m_matProp->thicknessInX0();  
  
  if ( isp->charge()!=0 ) {
    AmgVector(5) updatedParameters(tParameters->parameters());  
    if ( m_eLoss ) ionize(*tParameters, updatedParameters, dX0, dir, eCell.pHypothesis );

    if ( m_ms ) {

      double sigmaMSproj = (m_use_msUpdator && m_msUpdator ) ?
	sqrt(m_msUpdator->sigmaSquare(*m_matProp, p, dX0/m_matProp->thicknessInX0(), eCell.pHypothesis)) : msSigma(dX0,p,eCell.pHypothesis);
 
      multipleScatteringUpdate(*tParameters,updatedParameters,sigmaMSproj, pathCorrection);
    }
    
    // use the manipulator to update the track parameters -------> get rid of 0!
    tParameters = tParameters->associatedSurface().createTrackParameters(updatedParameters[0],
									 updatedParameters[1],
									 updatedParameters[2],
									 updatedParameters[3],
									 updatedParameters[4],
									 0);
    
    if ((tParameters->momentum()).mag() < m_minimumMomentum ) {
      if (isp!=m_isp) { delete isp; delete tParameters; }
      return 0;
    }
    
  }
  
  // register particle if not in the stack already
  if (tParameters && isp!=m_isp ) {
    ISF::ISFParticle* regisp=new ISF::ISFParticle(isp->position(),tParameters->momentum(),isp->mass(),isp->charge(),
						  isp->pdgCode(),isp->timeStamp(),*m_isp,isp->barcode());
    m_particleBroker->push(regisp, m_isp);
  }
  
  if (isp!=m_isp) delete isp;
  
  if (tParameters) std::cout <<"layer "<< tParameters->position()<< " returns :"<< isp <<" with momentum  "<< tParameters->momentum().mag()  << std::endl;
  else std::cout <<"layer returns :"<<  0  << std::endl;
  
  return (&parm); 
  
}

void iFatras::McMaterialEffectsEngine::ionize(const Trk::TrackParameters& parm, 
					      AmgVector(5)& updatedPar,
					      double dInX0,
					      Trk::PropDirection /*dir*/,
					      Trk::ParticleHypothesis particle ) const 
{
  double p    = parm.momentum().mag();                                                      
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

}

double iFatras::McMaterialEffectsEngine::msSigma(double dInX0,double p,Trk::ParticleHypothesis particle) const {
   
  double m    = m_particleMasses.mass[particle];
  double E    = sqrt(p*p+m*m);
  double beta = p/E; 

  // PDG Particle Review, Phys.Rev.D86,010001(2012), chapter 30.3, page 328 

  double sigmaMSproj = 13.6/beta/p*sqrt(dInX0)*(1.+0.038*log(dInX0));

  return sigmaMSproj; 
} 
                                                         
void iFatras::McMaterialEffectsEngine::multipleScatteringUpdate(const Trk::TrackParameters& pars,
								AmgVector(5)& parameters,
                                                                double sigmaMSproj, double pathCorrection) const
{
  //block of initial values
  //------------------------------------//
  double d = 0.1*m_matProp->thickness(); //layer thickness, cm
  d*=pathCorrection;
  double X0 = 0.1*m_matProp->x0(); //layer rad len, cm
  double Z = m_matProp->averageZ(); //charge layer material
  //double AN = 2*Z; //mass number of the material (an approximation, bad for heavy nuclei)
  double p = m_isp->momentum().mag()*0.001; //particle momentum, GeV
  double m = m_isp->mass()*0.001; //particle mass, GeV
  double dOverX0 = d/X0;
  double E=sqrt(p*p+m*m);
  double beta=p/E;
  double scale = 0.860176; ///magic
  
  std::cout<<"McMaterialEffectsEngine::multipleScatteringUpdate: Material pars (d,beta,p,dOverX0,Z): "<<std::endl;
  std::cout<<d<<' '<<beta<<' '<<p<<' '<<dOverX0<<' '<<Z<<std::endl;
  //-----------------------------------//
  
  double theta = simulate_theta_space(beta, p, dOverX0, Z, scale);
  //theta*=m_projectionFactor*sigmaMSproj;
  
  double psi = 2*M_PI*CLHEP::RandFlat::shoot(m_randomEngine);
  
  CLHEP::Hep3Vector parsMomHep( pars.momentum().x(), pars.momentum().y(), pars.momentum().z() );
  CLHEP::Hep3Vector newDirection( parsMomHep.unit().x(), parsMomHep.unit().y(), parsMomHep.unit().z());
  double x = -newDirection.y();
  double y = newDirection.x();
  double z = 0.;
  // if it runs along the z axis - no good ==> take the x axis
  if (newDirection.z()*newDirection.z() > 0.999999) {
    x=1.; y=0.;
  }
  // deflector direction
  CLHEP::Hep3Vector deflector(x,y,z);
  // rotate the new direction for scattering
  newDirection.rotate(theta, deflector);
  // and arbitrarily in psi             
  newDirection.rotate(psi, parsMomHep);
  // assign the new values
  parameters[Trk::phi]   = newDirection.phi();   
  parameters[Trk::theta] = newDirection.theta();
  
}

double iFatras::McMaterialEffectsEngine::simulate_theta_space(double beta, double p,double dOverX0,double Z, double scale) const{
  double * scattering_params;
    // Decide which mixture is best
  if (dOverX0/(beta*beta)>0.6/pow(Z,0.6)){ //Gaussian
    // Gaussian mixture or pure Gaussian
    if (dOverX0/(beta*beta)>10){
        scattering_params=get_gaussian(beta,p,dOverX0,scale); // Get parameters
        std::cout<<"McMaterialEffectsEngine::multipleScatteringUpdate: using pure_gaussian"<<std::endl;
    }
    else{
        scattering_params=get_gaussmix(beta,p,dOverX0,Z,scale); // Get parameters
        std::cout<<"McMaterialEffectsEngine::multipleScatteringUpdate: using gaussian_mixture"<<std::endl;
    }
    return sim_gaussmix(scattering_params); // Simulate
  }
  //Semigaussian mixture
  scattering_params = get_semigauss(beta,p,dOverX0,Z,scale); // Get parameters
  std::cout<<"McMaterialEffectsEngine::multipleScatteringUpdate: using semi_gaussian mixture"<<std::endl;
  return sim_semigauss(scattering_params); // Simulate
}

double * iFatras::McMaterialEffectsEngine::get_gaussian(double beta, double p,double dOverX0, double scale) const{
  double * scattering_params = new double[4];
  scattering_params[0]=0.015/beta/p*sqrt(dOverX0)*scale; //Total standard deviation of mixture
 // scattering_params[0]=1.0;
  scattering_params[1]=1.0; //Variance of core
  scattering_params[2]=1.0; //Variance of tails
  scattering_params[3]=0.5; //Mixture weight of tail component
  return scattering_params;
}

double * iFatras::McMaterialEffectsEngine::get_gaussmix(double beta, double p,double dOverX0,double Z, double scale) const{  
  double * scattering_params = new double[4];
  scattering_params[0]=0.015/beta/p*sqrt(dOverX0)*scale; //Total standard deviation of mixture
//   scattering_params[0]=1.0;
  double d1=log(dOverX0/(beta*beta));
  double d2=log(pow(Z,2.0/3.0)*dOverX0/(beta*beta));
  double epsi;
  double var1=(-1.843e-3*d1+3.347e-2)*d1+8.471e-1; //Variance of core
  if (d2<0.5)
    epsi=(6.096e-4*d2+6.348e-3)*d2+4.841e-2;
  else
    epsi=(-5.729e-3*d2+1.106e-1)*d2-1.908e-2;
  scattering_params[1]=var1; //Variance of core
  scattering_params[2]=(1-(1-epsi)*var1)/epsi; //Variance of tails
  scattering_params[3]=epsi; //Mixture weight of tail component
  return scattering_params;
}

double * iFatras::McMaterialEffectsEngine::get_semigauss(double beta,double p,double dOverX0,double Z, double scale) const{
  double * scattering_params = new double[6];
  double N=dOverX0*1.587E7*pow(Z,1.0/3.0)/(beta*beta)/(Z+1)/log(287/sqrt(Z));
  scattering_params[4]=0.015/beta/p*sqrt(dOverX0)*scale; //Total standard deviation of mixture
//  double eps = 0.125*log10(10*dOverX0);
//  scattering_params[4]=0.0175/beta/p*sqrt(dOverX0)*(1+eps);
  //scattering_params[4]= 0.0192/beta/p*sqrt(dOverX0)*(1.+0.038*log(dOverX0));
  //scattering_params[4]=1.0;
  double rho=41000/pow(Z,2.0/3.0);
  double b=rho/sqrt(N*(log(rho)-0.5));
  double n=pow(Z,0.1)*log(N);
  double var1=(5.783E-4*n+3.803E-2)*n+1.827E-1;
  double a=(((-4.590E-5*n+1.330E-3)*n-1.355E-2)*n+9.828E-2)*n+2.822E-1;
  double epsi = (1-var1)/(a*a*(log(b/a)-0.5)-var1);
  scattering_params[3]=(epsi>0) ? epsi : 0.0;//Mixture weight of tail component
  scattering_params[0]=a; //Parameter 1 of tails
  scattering_params[1]=b; //Parameter 2 of tails
  scattering_params[2]=var1; //Variance of core
  scattering_params[5]=N; //Average number of scattering processes
  return scattering_params;
}

double iFatras::McMaterialEffectsEngine::sim_gaussmix(double * scattering_params) const{
  double sigma_tot = scattering_params[0];
  double var1 = scattering_params[1];
  double var2 = scattering_params[2];
  double epsi = scattering_params[3];  
  bool ind = (double)rand()/RAND_MAX>epsi;
  double u=(double)rand()/RAND_MAX;
  if(ind)
  return sqrt(var1)*sqrt(-2*log(u))*sigma_tot;
  else
  return sqrt(var2)*sqrt(-2*log(u))*sigma_tot;
}

double iFatras::McMaterialEffectsEngine::sim_semigauss(double * scattering_params) const{
  double a = scattering_params[0];
  double b = scattering_params[1];
  double var1 = scattering_params[2];
  double epsi = scattering_params[3];
  double sigma_tot = scattering_params[4];
  bool ind=(double)rand()/RAND_MAX>epsi;
  double u=(double)rand()/RAND_MAX;
  if(ind)
  return sqrt(var1)*sqrt(-2*log(u))*sigma_tot;
  else
  return a*b*sqrt((1-u)/(u*b*b+a*a))*sigma_tot;
}
