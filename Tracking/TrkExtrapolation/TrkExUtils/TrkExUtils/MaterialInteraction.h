/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialInteraction.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_MATERIALINTERACTION_H
#define TRKGEOMETRY_MATERIALINTERACTION_H

#include "TrkGeometry/Material.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "EventPrimitives/EventPrimitives.h"

namespace Trk {

   /** @class MaterialInteraction 
   
    Collection of parametrizations used in the Tracking realm
    
    @author sarka.todorova@cern.ch 
  */    

  class MaterialInteraction {
  public :  

    /** Default Constructor needed for POOL */
     MaterialInteraction(){}    

    /** Descructor */
    ~MaterialInteraction() {}
    
    /** dE/dl ionization energy loss per path unit */
    double dEdl_ionization(double p, const Material* mat, ParticleHypothesis particle, double& sigma, double& kazL) const;    

    /** dE/dl radiation energy loss per path unit */
    double dEdl_radiation(double p, const Material* mat, ParticleHypothesis particle, double& sigma) const;    

    /** multiple scattering as function of dInX0 */
    double sigmaMS(double dInX0, double p, double beta) const;    
    
  private : 

    static  ParticleMasses                 s_particleMasses;    //!< Struct of Particle masses
    
  };
 
  /** dE/dl ionization energy loss per path unit */
  inline double Trk::MaterialInteraction::dEdl_ionization(double p, const Trk::Material* mat, Trk::ParticleHypothesis particle, double& sigma, double& kazL) const{
//
// calculate mean ionization that is pathlentgh INDEPENDENT 
//
// sigma = sigmaL = landau sigma is pathlentgh DEPENDENT (here calculated for 1 mm)
// sigma(length) =  sigma - kazL*log(pathlength)
// kazL is calculated in this function for later use.
//
// For a Landau: MOP value = Mean + 3.59524*sigmaL 
//

    sigma = 0.;
    if ( mat->averageZ()<1 ) return 0.;    

    double Ionization = 0.;
    
    // kinetic variables
    // and the electron mass in MeV
    double me    = s_particleMasses.mass[Trk::electron];
    double m     = s_particleMasses.mass[particle];
    double mfrac = me/m;
    double E     = sqrt(p*p+m*m);
    double beta  = p/E;
    double gamma = E/m;
    
    //Ionization - Bethe-Bloch
    double I = 16.e-6 * std::pow(mat->averageZ(),0.9); //16 eV * Z**0.9 - bring to MeV
    
    //K/A*Z = 0.5 * 30.7075MeV/(g/mm2) * Z/A * rho[g/mm3]  / scale to mm by this
    double kaz = 0.5*30.7075*mat->zOverAtimesRho();

//  sigmaL of Landau 
    sigma = 4*kaz*beta/beta;       // dsigma/dl
    kazL = 0.;
  
    double MOP = 0.;
    if (particle == Trk::electron){
      // for electrons use slightly different BetheBloch adaption
      // see Stampfer, et al, "Track Fitting With Energy Loss", Comp. Pyhs. Comm. 79 (1994), 157-164
      Ionization = -kaz*(2.*log(2.*me/I)+3.*log(gamma) - 1.95);
    }  else {

      double eta2 = beta*gamma; eta2 *= eta2;
      // density effect, only valid for high energies (gamma > 10 -> p > 1GeV for muons)
      double delta = 0.;
      if (gamma > 10.) {
	double eplasma = 28.816e-6 * sqrt(1000.*mat->zOverAtimesRho());
	delta = 2.*log(eplasma/I) + log(eta2) - 1.;
      }
      // tmax - cut off energy
      double tMax = 2.*eta2*me/(1.+2.*gamma*mfrac+mfrac*mfrac);
      // divide by beta^2 for non-electrons
      kaz /= beta*beta;
      Ionization = -kaz*(log(2.*me*eta2*tMax/(I*I)) - 2.*(beta*beta) - delta);
      //
      // the landau sigmaL is path length dependent assume with 1 mm path 
      //
      double path = 1.;
      //
      //    PDG formula 27.11 for MOP value from http://pdg.lbl.gov/2011/reviews/rpp2011-rev-passage-particles-matter.pdf 
      //
      MOP =  -kaz*(log(2.*me*eta2/I) + log(path*kaz/I) + 0.2 - (beta*beta) - delta);
      sigma = -(Ionization - MOP)/3.59524; 
      kazL = kaz/3.59524;
    }
 
    return Ionization;
  }

  /** dE/dl radiation energy loss per path unit */
  inline double Trk::MaterialInteraction::dEdl_radiation(double p, const Trk::Material* mat, Trk::ParticleHypothesis particle, double& sigma) const{
    sigma = 0.;
    if ( mat->x0()==0. ) return 0.;    

    // preparation of kinetic constants
    double m     = s_particleMasses.mass[particle];
    double me    = s_particleMasses.mass[Trk::electron];
    double mfrac = me/m;
    double E     = sqrt(p*p+m*m);

    //Bremsstrahlung - Bethe-Heitler
    double Radiation = -E*mfrac*mfrac;
    // sigma is rms of steep exponential part of radiation 
    sigma = -Radiation;

    //Add e+e- pair production and photonuclear effect for muons at energies above 8 GeV
    //    Radiation gives mean Eloss including the long tail from 'catastrophic' Eloss 
    //    sigma the rms of steep exponential
    if ((particle == Trk::muon) && (E > 8000.)) {
      if (E < 1.e6) {
	Radiation += 0.5345 - 6.803e-5*E - 2.278e-11*E*E + 9.899e-18*E*E*E; //E below 1 TeV
        sigma     += (0.1828 -3.966e-3*sqrt(E) + 2.151e-5*E ); // idem
      } else {
	Radiation += 2.986 - 9.253e-5*E; //E above 1 TeV
        sigma     += 17.73 + 2.409e-5*(E-1000000.); // idem
      }
    }

    sigma = sigma/mat->x0();  
    
    return Radiation/mat->x0();  
  }

  /** multiple scattering as function of dInX0 */
  inline  double Trk::MaterialInteraction::sigmaMS(double dInX0, double p, double beta) const{

    if ( dInX0==0. || p==0. || beta==0.) return 0.;
    
    // Highland formula
    double sig_ms= 13.6*sqrt(dInX0)/(beta*p) * (1.+0.038*log(dInX0/(beta*beta))); 

    return sig_ms; 
  }    

}

#endif
