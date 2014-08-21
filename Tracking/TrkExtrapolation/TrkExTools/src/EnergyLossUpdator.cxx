/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyLossUpdator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk include
#include "TrkExTools/EnergyLossUpdator.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <math.h>

// static particle masses
Trk::ParticleMasses Trk::EnergyLossUpdator::s_particleMasses;
// statics doubles 
double Trk::EnergyLossUpdator::s_ka_BetheBloch = 30.7075 * Gaudi::Units::MeV;
double Trk::EnergyLossUpdator::s_eulerConstant = 0.577215665; // as given by google.com ;-)

// from full with at half maximum to sigma for a gaussian
double Trk::EnergyLossUpdator::s_fwhmToSigma   = 0.424;  //   1./(2.*sqrt(2.*log(2.)));

// mpv to sigma fit values
double Trk::EnergyLossUpdator::s_mpv_p0        = 4.57270e-02;
double Trk::EnergyLossUpdator::s_mpv_p1        = 8.11761e-03;
double Trk::EnergyLossUpdator::s_mpv_p2        = -4.85133e-01;

// constructor
Trk::EnergyLossUpdator::EnergyLossUpdator(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_useTrkUtils(true),
  m_gaussianVavilovTheory(false),
  m_useBetheBlochForElectrons(true),
  m_transKaz(0.),
  m_transTmax(0.),
  m_stragglingErrorScale(1.),
  m_mpvScale(0.98),
  m_mpvSigmaParametric(false),
  m_detailedEloss(false)
{
   declareInterface<Trk::IEnergyLossUpdator>(this);
   // scale from outside
   declareProperty("UseTrkUtils"                , m_useTrkUtils);           
   declareProperty("BetheBlochKAFactor"         , s_ka_BetheBloch);
   // some additional setup
   declareProperty("UseGaussVavilovTheory"      , m_gaussianVavilovTheory);
   declareProperty("UseBetheBlochForElectrons"  , m_useBetheBlochForElectrons);
   // scalor for the straggling
   declareProperty("ScaleStragglingError"       , m_stragglingErrorScale);
   // scalor for the most probable value
   declareProperty("UseParametricMpvError"      , m_mpvSigmaParametric);
   declareProperty("ScaleMostProbableValue"     , m_mpvScale);
   declareProperty("DetailedEloss"              , m_detailedEloss);
}

// destructor
Trk::EnergyLossUpdator::~EnergyLossUpdator()
{}

// Athena standard methods
// initialize
StatusCode Trk::EnergyLossUpdator::initialize()
{
    ATH_MSG_INFO( "initialize()" );    
    return StatusCode::SUCCESS;
}

// finalize
StatusCode Trk::EnergyLossUpdator::finalize()
{
    ATH_MSG_INFO( "finalize() successful" );
    return StatusCode::SUCCESS;
}

// public interface method
double Trk::EnergyLossUpdator::dEdX(const MaterialProperties& mat,
                                    double p,
                                    ParticleHypothesis particle) const
{
  if (particle == Trk::undefined || particle == Trk::nonInteracting) return 0.; 
   
  // preparation of kinetic constants
  double m     = s_particleMasses.mass[particle];
  double E     = sqrt(p*p+m*m);
  double beta  = p/E;
  double gamma = E/m;

  m_transKaz  = 0.;
  m_transTmax = 0.;
  // add ionization and radiation
  double dEdX = dEdXBetheBloch(mat, beta, gamma, particle) + dEdXBetheHeitler(mat, E, particle);

  // add e+e- pair production and photonuclear effect for muons at energies above 8 GeV
  if ((particle == Trk::muon) && (E > 8000.)) {
    if (E < 1.e6) {
      dEdX += - 0.5345/mat.x0() + 6.803e-5*E/mat.x0() + 2.278e-11*E*E/mat.x0() - 9.899e-18*E*E*E/mat.x0(); //E below 1 TeV
    } else {
      dEdX += - 2.986/mat.x0() + 9.253e-5*E/mat.x0(); //E above 1 TeV
    }
  }

  return dEdX;
}

   
// public interface method
Trk::EnergyLoss* Trk::EnergyLossUpdator::energyLoss(const MaterialProperties& mat,
                                                    double p,
                                                    double pathcorrection,
                                                    PropDirection dir,
                                                    ParticleHypothesis particle,
                                                    bool mpvSwitch) const
{ 
 
  if (particle == Trk::undefined ) {
    ATH_MSG_WARNING( "undefined ParticleHypothesis, energy loss calculation cancelled" );
    return 0;
  }

  double deltaE       = 0.;
  double sigmaDeltaE  = 0.;
  m_transKaz  = 0.;
  m_transTmax = 0.;

  // preparation 
  double sign = (dir==Trk::oppositeMomentum) ? 1. : -1.;

  double pathLength = pathcorrection * mat.thicknessInX0()*mat.x0();  

  double sigIoni = 0.;
  double sigRad = 0.;
  double kazL = 0.;
  double meanIoni  = m_matInt.dEdl_ionization(p, &(mat.material()), particle, sigIoni, kazL);
  double meanRad  = m_matInt.dEdl_radiation(p, &(mat.material()), particle, sigRad);

  meanIoni = pathLength*meanIoni;  
  meanRad = pathLength*meanRad;  
  sigIoni = pathLength*sigIoni;  
  sigRad  = pathLength*sigRad;  
  kazL    = pathLength*kazL;
//
// include pathlength dependence of Landau ionization 
//
  sigIoni = sigIoni - kazL*log(pathLength);  

  deltaE = meanIoni + meanRad;
  sigmaDeltaE = sqrt(sigIoni*sigIoni+sigRad*sigRad);
  ATH_MSG_DEBUG( " Energy loss updator deltaE " << deltaE << " meanIoni " << meanIoni << " meanRad " << meanRad << " sigIoni " << sigIoni << " sigRad " << sigRad );   

  Trk::EnergyLoss* eloss = !m_detailedEloss ? new Trk::EnergyLoss(deltaE,sigmaDeltaE):
     new Trk::EnergyLoss(deltaE, sigmaDeltaE, meanIoni, sigIoni, meanRad, sigRad );

  if(m_useTrkUtils) return eloss;

// Code below will not be used if the parameterization of TrkUtils is used
  
  double m     = s_particleMasses.mass[particle]; 
  double mfrac = s_particleMasses.mass[Trk::electron]/m;
  double mfrac2 = mfrac*mfrac;
  double E     = sqrt(p*p+m*m);
  // relativistic properties
  double beta  = p/E;
  double gamma = E/m;
  
  // mean radiation loss + sigma
  // Follow CompPhys Comm 79 (1994) P157-164
  // Brem parameterization in terms of path-length in number of radiation lengths
  double dInX0  = pathcorrection*mat.thicknessInX0();
  double meanZ = exp(-mfrac2*dInX0);

  double deltaE_rad =  (dir == Trk::alongMomentum )? sign * E * (1. - meanZ) : sign * E * (1. / meanZ - 1.);

  // add e+e- pair production and photonuclear effect for muons at energies above 8 GeV
  if ((particle == Trk::muon) && (E > 8000.)) {
    double deltaEpair = 0.;
    if (E < 1.e6) {
      deltaEpair = (- 0.5345 + 6.803e-5*E + 2.278e-11*E*E - 9.899e-18*E*E*E)*dInX0; //E below 1 TeV
   } else {
      deltaEpair = (- 2.986 + 9.253e-5*E)*dInX0; //E above 1 TeV
    }
    deltaE_rad += sign*deltaEpair;
  }

//  The sigma radiation loss of CompPhys Comm 79 (1994) P157-164 has PROBLEMS at low Eloss values 

//  double varZ  = exp( -1. * dInX0 * log(3.) / log(2.) ) - exp(-2. * dInX0);
//  double varianceQoverP =  ( dir == Trk::alongMomentum )? 1. / (meanZ * meanZ * p * p) * varZ : varZ / (p * p);
//  double sigmaDeltaE_rad = beta*beta*p*p*sqrt( varianceQoverP );

// Use a simple scale factor applied to the Eloss from Radiation obtained from G4 simulation in stead

  double Radiation_FWHM = m_stragglingErrorScale*4*0.85*fabs(deltaE_rad)/3.59524;
  double sigmaDeltaE_rad = s_fwhmToSigma*Radiation_FWHM;

  // ionization 
  double deltaE_ioni = 0.;
  double sigmaDeltaE_ioni = 0.;
  if (particle != Trk::electron || m_useBetheBlochForElectrons) {

    if (!mpvSwitch) {
      // use the dEdX function 
      deltaE_ioni = sign*pathLength*dEdXBetheBloch(mat, beta, gamma, particle);

      // the different straggling functions
      if (m_gaussianVavilovTheory)     {
	// use the Gaussian approximation for the Vavilov Theory
	double sigmaE2 =  m_transKaz * pathLength * m_transTmax*(1.-beta*beta/2.);
	sigmaDeltaE_ioni = sqrt(sigmaE2);
      } else 

//      Take FWHM maximum of Landau and convert to Gaussian  
//      For the FWHM of the Landau Bichsel/PDG is used: FWHM = 4 xi = 4 m_transKaz * pathLength
	sigmaDeltaE_ioni = 4.*s_fwhmToSigma* m_transKaz * pathLength; 
      
    } else {

      double eta2   = beta*gamma;   
      eta2 *= eta2;   
      // most probable  
      double kaz = 0.5*s_ka_BetheBloch*mat.zOverAtimesRho();
      double xi = kaz * pathLength;
      // get the ionisation potential
      double iPot = 16.e-6 * std::pow( mat.averageZ(),0.9);
      // density effect, only valid for high energies (gamma > 10 -> p > 1GeV for muons)
      double delta = 0.;
      /* ST replace with STEP-like coding
	 if (eta2 > 2.) {
	 // high energy density effect
	 double eplasma = 28.816e-6 * sqrt(1000.*0.5);
	 delta = 2.*log(eplasma/iPot) + log(eta2) - 0.5;
	 }
      */
      if (gamma>10.) {
	double eplasma = 28.816e-6 * sqrt(1000.*mat.zOverAtimesRho());
	delta = 2.*log(eplasma/iPot) + log(eta2) - 1.;
      }	 
      // calculate the most probable value of the Landau distribution
      double mpv = m_mpvScale * xi/(beta*beta) * (log(m*eta2*kaz/(iPot*iPot)) + 0.2-beta*beta-delta);//12.325);

      // sigma 
      // (1) - obtained from fitted function  [  return par0 + par1*std::pow(x,par2); ]
      // (2) - following Bichsel: fwhm (full with at half maximum) = 4 *xi, w = 2.35 fwhm (for gaussian)  
      // use the crude approximation for the width 4*chi
      sigmaDeltaE_ioni = !m_mpvSigmaParametric ? s_fwhmToSigma*4.*xi : mpv*(s_mpv_p0 + s_mpv_p1*std::pow(mpv,s_mpv_p2));
  
      // get the fitted sigma        
      deltaE_ioni = sign* mpv;

    }
    // scale the error
    sigmaDeltaE_ioni *= m_stragglingErrorScale; 
  }

  deltaE = deltaE_ioni + deltaE_rad;

  sigmaDeltaE = sqrt( sigmaDeltaE_rad*sigmaDeltaE_rad + sigmaDeltaE_ioni*sigmaDeltaE_ioni);

  return ( m_detailedEloss ? new EnergyLoss(deltaE, sigmaDeltaE, 
					    (mpvSwitch? deltaE_ioni :0.9*deltaE_ioni), sigmaDeltaE_ioni,
					    deltaE_rad, sigmaDeltaE_rad ):
	                     new EnergyLoss(deltaE, sigmaDeltaE) );

}

double Trk::EnergyLossUpdator::dEdXBetheBloch(const MaterialProperties& mat,
                                              double beta,
                                              double gamma,
                                              ParticleHypothesis particle) const
{

  if (particle == Trk::undefined || particle == Trk::nonInteracting ) return 0.;

  if (mat.averageZ()==0. || mat.zOverAtimesRho()==0. ) {
    ATH_MSG_ERROR("empty material properties pass to the EnergyLossUpdator:Z,zOAtr:"<<mat.averageZ()<<","<<mat.zOverAtimesRho());
    return 0.;
  }

  // 16 eV * Z**0.9 - bring to MeV
  double iPot = 16.e-6 * std::pow(mat.averageZ(),0.9);
  // and the electron mass in MeV
  double me    = s_particleMasses.mass[Trk::electron];
  double m     = s_particleMasses.mass[particle];
  double eta2  = beta*gamma;
 
  // K/A*Z = 0.5 * 30.7075MeV/(g/mm2) * Z/A * rho[g/mm3]  / scale to mm by this
  double kaz = 0.5*s_ka_BetheBloch*mat.zOverAtimesRho();
     
  if (particle != Trk::electron){

    // density effect, only valid for high energies (gamma > 10 -> p > 1GeV for muons)
    double delta = 0.;

    /* ST replace with STEP-like coding  
    // high energy density effect --- will be ramped up linearly
    double eplasma = 28.816e-6 * sqrt(1000.*0.5);
    delta = 2.*log(eplasma/iPot) + log(eta2) - 0.5;
    if (eta2 < 100.){
    delta *= (eta2-3.)/97.; 
    }
    */
      
    eta2 *= eta2; 

    if (gamma>10.) {
      double eplasma = 28.816e-6 * sqrt(1000.*mat.zOverAtimesRho());
      delta = 2.*log(eplasma/iPot) + log(eta2) - 1.;
    }
    
    // mass fraction
    double mfrac = me/m;
    // tmax - cut off energy
    double tMax = 2.*eta2*me/(1.+2.*gamma*mfrac+mfrac*mfrac);
    // divide by beta^2 for non-electrons
    kaz /= beta*beta;
    // store the transport variables
    m_transKaz  = kaz;
    m_transTmax = tMax;
    // return
    return  kaz*(log(2.*me*eta2*tMax/(iPot*iPot)) - 2.*(beta*beta) - delta);
  
  }
  m_transKaz = kaz;
  // for electrons use slightly different BetheBloch adaption
  // see Stampfer, et al, "Track Fitting With Energy Loss", Comp. Pyhs. Comm. 79 (1994), 157-164
  return kaz*(2.*log(2.*me/iPot)+3.*log(gamma) - 1.95);
  
}

double Trk::EnergyLossUpdator::dEdXBetheHeitler(const MaterialProperties& mat,
                                                double initialE, 
                                                ParticleHypothesis particle) const
{

  if (particle == Trk::undefined || particle == Trk::nonInteracting ) return 0.;

  double mfrac = (s_particleMasses.mass[Trk::electron]/s_particleMasses.mass[particle]); mfrac *= mfrac;
  
  return initialE/mat.x0()*mfrac;
}

Trk::EnergyLoss* Trk::EnergyLossUpdator::updateEnergyLoss(Trk::EnergyLoss* eLoss, double caloEnergy, double caloEnergyError, double momentumError) const {

//
// Input the detailed EnergyLoss object that contains the different Eloss terms and their uncertainties
//        and the momentumError (all in MeV)
//
// Output: an updated Energy loss values deltaE() 
//  that can be used in the track fit and corresponds to the Most Probable EnergyLoss value
//  taking into account the ionization, radiation and 
//  smearing due to the errors including the momentumError (in MeV)
//
//  deltaE is used in the final fit
//

  int isign = 1;
  if(eLoss->deltaE()<0) isign = -1;
 
  double deltaE = eLoss->deltaE();
  double sigmaDeltaE = eLoss->sigmaDeltaE();
// Detailed Eloss
  double deltaE_ioni = eLoss->meanIoni(); 
  double sigmaDeltaE_ioni = eLoss->sigmaIoni(); // sigma Landau
//  double deltaE_rad = eLoss->meanRad();  
  double sigmaDeltaE_rad = eLoss->sigmaRad(); // rms and mean of steep exponential

  double sigmaPlusDeltaE = eLoss->sigmaPlusDeltaE();
  double sigmaMinusDeltaE = eLoss->sigmaMinusDeltaE();

  double MOP = deltaE_ioni - isign*3.59524*sigmaDeltaE_ioni;
//
// MOP shift due to ionization and radiation
//   
  double MOPshift = isign*0.05*sqrt(sigmaDeltaE_ioni*sigmaDeltaE_rad);
//
// define sigmas for Landau convoluted with exponential
//
  double sigmaL = sigmaDeltaE_ioni + sigmaDeltaE_rad/3.59524;
  double sigmaMinus = 1.02*sigmaDeltaE_ioni + 0.08*sigmaDeltaE_rad; 
  double sigmaPlus = 4.65*sigmaDeltaE_ioni + 1.16*sigmaDeltaE_rad;

//
// Case where the measured Calorimeter energy is not available
//
  
  if(caloEnergyError<=0) {
//
// Shift of MOP due to momentum resolution 
//
    double xc = 0.87388*momentumError/(3.59524*sigmaL);
    double correction = (1.747*xc*xc + 0.97*0.938*xc*xc*xc)/(1+4.346*xc+5.371*xc*xc+0.938*xc*xc*xc); // correction ranges from 0 to 0.97
    double MOPreso = isign*3.59524*sigmaL*correction;

    deltaE = MOP + MOPshift + MOPreso;
    sigmaMinusDeltaE = sigmaMinus;
    sigmaPlusDeltaE  = sigmaPlus;
    sigmaDeltaE = sqrt(0.5*sigmaMinusDeltaE*sigmaMinusDeltaE+0.5*sigmaPlusDeltaE*sigmaPlusDeltaE);

  } else {

    double sigmaPlusTot = sqrt(sigmaPlus*sigmaPlus+caloEnergyError*caloEnergyError);

    if(caloEnergy > fabs(MOP + MOPshift) + 2*sigmaPlusTot) {
//
// Use measured Calorimeter energy
//
      deltaE = isign*caloEnergy;
      sigmaMinusDeltaE = caloEnergyError;  
      sigmaPlusDeltaE = caloEnergyError;
      sigmaDeltaE = caloEnergyError;

    } else {

// Use MOp after corrections

//
// MOPCalo is correction to MOP for Calorimeter energy cut
//
      double xe = caloEnergyError/sigmaL;
      double MOPCalo = -isign*sigmaL*0.1*(1+xe)/(1+0.077*xe+0.077*xe*xe); // ranges from -0.1 sigmaL to 0.
//
// Shift of MOP due to momentum resolution smearing
//
      double errorScale = sqrt(1./4.65/4.65 + caloEnergyError*caloEnergyError/(caloEnergyError*caloEnergyError+sigmaPlus*sigmaPlus)); // ranges from 1/4.65 to 1 
      double xc = 0.87388*momentumError/(3.59524*sigmaL*errorScale);
      double correction = (1.747*xc*xc + 0.97*0.938*xc*xc*xc)/(1+4.346*xc+5.371*xc*xc+0.938*xc*xc*xc); // correction ranges from 0 to 0.97
      double MOPreso = isign*3.59524*sigmaL*errorScale*correction;
// 
// Use MOP after correction for radiation (MOPshift) and Calo cut (MOPCalo) and momentum resolution smearing (MOPreso) 
//
      deltaE = MOP + MOPshift + MOPCalo + MOPreso;
      sigmaMinusDeltaE = sigmaMinus;
      sigmaPlusDeltaE = sigmaPlus*errorScale; 
      sigmaDeltaE = sqrt(0.5*sigmaMinusDeltaE*sigmaMinusDeltaE+0.5*sigmaPlusDeltaE*sigmaPlusDeltaE);

   }

  }


  return new EnergyLoss(deltaE, sigmaDeltaE, sigmaPlusDeltaE, sigmaMinusDeltaE);

  
  }
