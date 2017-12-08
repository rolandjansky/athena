/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MultipleScatteringSamplerGeneralMixture.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk include
#include "TrkGeometry/MaterialProperties.h"

#include "ISF_FatrasTools/MultipleScatteringSamplerGeneralMixture.h"
#include "CLHEP/Random/RandGaussZiggurat.h"

// static particle masses
Trk::ParticleMasses iFatras::MultipleScatteringSamplerGeneralMixture::s_particleMasses;
// static doubles

double iFatras::MultipleScatteringSamplerGeneralMixture::s_main_RossiGreisen = 17.5*Gaudi::Units::MeV;
double iFatras::MultipleScatteringSamplerGeneralMixture::s_log_RossiGreisen  =  0.125;

double iFatras::MultipleScatteringSamplerGeneralMixture::s_projectionFactor  =  sqrt(2.);

// ============================= General mixture model =============
double iFatras::MultipleScatteringSamplerGeneralMixture::s_genMixScale = 0.608236; //numerically derived scaling factor

// constructor
iFatras::MultipleScatteringSamplerGeneralMixture::MultipleScatteringSamplerGeneralMixture(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_log_include(true),
  m_rndGenSvc("AtRndmGenSvc", n),
  m_randomEngine(0),
  m_randomEngineName("TrkExRnd")
{
  declareInterface<IMultipleScatteringSampler>(this);
  // multiple scattering parameters
  declareProperty("MultipleScatteringLogarithmicTermOn", m_log_include);
  // random service for Gaussian mixture model
  declareProperty("RandomNumberService",                 m_rndGenSvc,            "Name of the random number service");
  declareProperty("RandomStreamName",                    m_randomEngineName,     "Name of the random number stream");
}

// destructor
iFatras::MultipleScatteringSamplerGeneralMixture::~MultipleScatteringSamplerGeneralMixture()
{}

// Athena standard methods
// initialize
StatusCode iFatras::MultipleScatteringSamplerGeneralMixture::initialize()
{
  // get the random generator service
  if (m_rndGenSvc.retrieve().isFailure()) {
    ATH_MSG_WARNING( "Could not retrieve " << m_rndGenSvc );
  } else {
    ATH_MSG_VERBOSE( "Successfully retrieved " << m_rndGenSvc );
  }

  //Get own engine with own seeds:
  m_randomEngine = m_rndGenSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine) {
    ATH_MSG_FATAL( "Could not get random engine '" << m_randomEngineName << "'" );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_VERBOSE ("Successfully got random engine '" << m_randomEngineName << "'");
  

  ATH_MSG_INFO( "initialize() successful" );
  return StatusCode::SUCCESS;
}

// finalize
StatusCode iFatras::MultipleScatteringSamplerGeneralMixture::finalize()
{
  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;
}


double iFatras::MultipleScatteringSamplerGeneralMixture::simTheta(const Trk::MaterialProperties& mat,
								      double p,
								      double pathcorrection,
								      Trk::ParticleHypothesis particle,
								      double ) const
{
  if (mat.thicknessInX0()<=0. || particle==Trk::geantino) return 0.;
  
  // make sure the path correction is positive to avoid a floating point exception
  pathcorrection *= pathcorrection < 0. ? (-1.) : (1) ;
  
  // scale the path length to the radiation length
  double dOverX0 = pathcorrection * mat.thicknessInX0();

  // kinematics (relativistic)
  double m    = s_particleMasses.mass[particle];
  double E    = sqrt(p*p + m*m);
  double beta = p/E;
  
  //material properties
  double Z = mat.averageZ(); //charge layer material

  double sigma2(0.);
  double theta(0.);
  
  
  if (particle != Trk::electron) {
    //----------------------------------------------------------------------------------------------//
    //see Mixture models of multiple scattering: computation and simulation. - R.Frühwirth, M. Liendl. -
    //Computer Physics Communications 141 (2001) 230–246
    //----------------------------------------------------------------------------------------------//
    double * scattering_params;
    // Decide which mixture is best
    if (dOverX0/(beta*beta)>0.6/pow(Z,0.6)){ //Gaussian
      // Gaussian mixture or pure Gaussian
      if (dOverX0/(beta*beta)>10){
	  scattering_params=getGaussian(beta,p,dOverX0,s_genMixScale); // Get parameters
	  //std::cout<<"MultipleScatteringSamplerGeneralMixture::multipleScatteringUpdate: using pure_gaussian"<<std::endl;
      }
      else{
	  scattering_params=getGaussmix(beta,p,dOverX0,Z,s_genMixScale); // Get parameters
	  //std::cout<<"MultipleScatteringSamplerGeneralMixture::multipleScatteringUpdate: using gaussian_mixture"<<std::endl;
      }
      theta = simGaussmix(scattering_params); // Simulate
    }
    else{
      //Semigaussian mixture
      scattering_params = getSemigauss(beta,p,dOverX0,Z,s_genMixScale); // Get parameters
      //std::cout<<"MultipleScatteringSamplerGeneralMixture::multipleScatteringUpdate: using semi_gaussian mixture"<<std::endl;
      theta = simSemigauss(scattering_params); // Simulate
    }
  }
  
  else {
    // Electron multiple scattering effects - see Highland NIM 129 (1975) p497-499
    // (Highland extension to the Rossi-Greisen formulation)
    // NOTE: The formula can be extended by replacing the momentum^2 term with pi * pf
    sigma2 = s_main_RossiGreisen / ( beta * p );
    sigma2 *= (sigma2*dOverX0);
    
    if ( m_log_include ) {
      double factor = 1. + s_log_RossiGreisen * log10( 10. * dOverX0 );
      factor *= factor;
      sigma2 *= factor;
    }
    
    theta=sqrt(sigma2)*CLHEP::RandGaussZiggurat::shoot(m_randomEngine);
  }
  
  return theta*s_projectionFactor;
}

double * iFatras::MultipleScatteringSamplerGeneralMixture::getGaussian(double beta, double p,double dOverX0, double scale) const{
  double * scattering_params = new double[4];
  scattering_params[0]=15./beta/p*sqrt(dOverX0)*scale; //Total standard deviation of mixture
  scattering_params[1]=1.0; //Variance of core
  scattering_params[2]=1.0; //Variance of tails
  scattering_params[3]=0.5; //Mixture weight of tail component
  return scattering_params;
}

double * iFatras::MultipleScatteringSamplerGeneralMixture::getGaussmix(double beta, double p,double dOverX0,double Z, double scale) const{  
  double * scattering_params = new double[4];
  scattering_params[0]=15./beta/p*sqrt(dOverX0)*scale; //Total standard deviation of mixture
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

double * iFatras::MultipleScatteringSamplerGeneralMixture::getSemigauss(double beta,double p,double dOverX0,double Z, double scale) const{
  double * scattering_params = new double[6];
  double N=dOverX0*1.587E7*pow(Z,1.0/3.0)/(beta*beta)/(Z+1)/log(287/sqrt(Z));
  scattering_params[4]=15./beta/p*sqrt(dOverX0)*scale; //Total standard deviation of mixture
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

double iFatras::MultipleScatteringSamplerGeneralMixture::simGaussmix(double * scattering_params) const{
  double sigma_tot = scattering_params[0];
  double var1 = scattering_params[1];
  double var2 = scattering_params[2];
  double epsi = scattering_params[3];  
  bool ind = CLHEP::RandFlat::shoot(m_randomEngine)>epsi;
  double u=CLHEP::RandFlat::shoot(m_randomEngine);
  if(ind)
  return sqrt(var1)*sqrt(-2*log(u))*sigma_tot;
  else
  return sqrt(var2)*sqrt(-2*log(u))*sigma_tot;
}

double iFatras::MultipleScatteringSamplerGeneralMixture::simSemigauss(double * scattering_params) const{
  double a = scattering_params[0];
  double b = scattering_params[1];
  double var1 = scattering_params[2];
  double epsi = scattering_params[3];
  double sigma_tot = scattering_params[4];
  bool ind=CLHEP::RandFlat::shoot(m_randomEngine)>epsi;
  double u=CLHEP::RandFlat::shoot(m_randomEngine);
  if(ind)
  return sqrt(var1)*sqrt(-2*log(u))*sigma_tot;
  else
  return a*b*sqrt((1-u)/(u*b*b+a*a))*sigma_tot;
}




