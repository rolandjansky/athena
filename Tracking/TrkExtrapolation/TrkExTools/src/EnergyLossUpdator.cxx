/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include <cmath>

namespace{
// static particle masses
const Trk::ParticleMasses  s_particleMasses{};
// statics doubles
constexpr double s_ka_BetheBloch = 30.7075 * Gaudi::Units::MeV;

// from full with at half maximum to sigma for a gaussian
constexpr double s_fwhmToSigma = 0.424;  //   1./(2.*sqrt(2.*log(2.)));

// mpv to sigma fit values
constexpr double s_mpv_p0 = 4.57270e-02;
constexpr double s_mpv_p1 = 8.11761e-03;
constexpr double s_mpv_p2 = -4.85133e-01;
}

// constructor
Trk::EnergyLossUpdator::EnergyLossUpdator(const std::string &t, const std::string &n, const IInterface *p) :
  AthAlgTool(t, n, p),
  m_stragglingErrorScale(1.),
  m_mpvScale(0.98),
  m_useTrkUtils(true),
  m_gaussianVavilovTheory(false),
  m_useBetheBlochForElectrons(true),
  m_mpvSigmaParametric(false),
  m_detailedEloss(true),
  m_optimalRadiation(true) {
  declareInterface<Trk::IEnergyLossUpdator>(this);
  // scale from outside
  declareProperty("UseTrkUtils", m_useTrkUtils);
  // some additional setup
  declareProperty("UseGaussVavilovTheory", m_gaussianVavilovTheory);
  declareProperty("UseBetheBlochForElectrons", m_useBetheBlochForElectrons);
  // scalor for the straggling
  declareProperty("ScaleStragglingError", m_stragglingErrorScale);
  // scalor for the most probable value
  declareProperty("UseParametricMpvError", m_mpvSigmaParametric);
  declareProperty("ScaleMostProbableValue", m_mpvScale);
  declareProperty("DetailedEloss", m_detailedEloss);
  declareProperty("OptimalRadiation", m_optimalRadiation);
}


// public interface method
double
Trk::EnergyLossUpdator::dEdX(const MaterialProperties &mat,
                             double p,
                             ParticleHypothesis particle) const {
  if (particle == Trk::undefined || particle == Trk::nonInteracting) {
    return 0.;
  }

  // preparation of kinetic constants
  double m = s_particleMasses.mass[particle];
  double E = sqrt(p * p + m * m);
  double beta = p / E;
  double gamma = E / m;

  double transKaz = 0.;
  double transTmax = 0.;
  // add ionization and radiation
  double dEdX = dEdXBetheBloch(mat, transKaz, transTmax,beta, gamma, particle) + dEdXBetheHeitler(mat, E, particle);

  // add e+e- pair production and photonuclear effect for muons at energies above 8 GeV
  if ((particle == Trk::muon) && (E > 8000.)) {
    if (E < 1.e6) {
      dEdX += -0.5345 / mat.x0() + 6.803e-5 * E / mat.x0() + 2.278e-11 * E * E / mat.x0() - 9.899e-18 * E * E * E /
              mat.x0(); // E below 1 TeV
    } else {
      dEdX += -2.986 / mat.x0() + 9.253e-5 * E / mat.x0(); // E above 1 TeV
    }
  }

  return dEdX;
}

// public interface method
Trk::EnergyLoss *
Trk::EnergyLossUpdator::energyLoss(const MaterialProperties &mat,
                                   double p,
                                   double pathcorrection,
                                   PropDirection dir,
                                   ParticleHypothesis particle,
                                   bool mpvSwitch,
                                   bool usePDGformula) const {
  if (particle == Trk::undefined) {
    ATH_MSG_WARNING("undefined ParticleHypothesis, energy loss calculation cancelled");
    return nullptr;
  }

  if (usePDGformula) {
    return ionizationEnergyLoss(mat, p, pathcorrection, dir, particle);
  }

  double deltaE = 0.;
  double sigmaDeltaE = 0.;
   // preparation
  double sign = (dir == Trk::oppositeMomentum) ? -1. : 1.;

  double pathLength = pathcorrection * mat.thicknessInX0() * mat.x0();

  double sigIoni = 0.;
  double sigRad = 0.;
  double kazL = 0.;
  double meanIoni = m_matInt.dEdl_ionization(p, &(mat.material()), particle, sigIoni, kazL);
  double meanRad = m_matInt.dEdl_radiation(p, &(mat.material()), particle, sigRad);

  meanIoni = sign * pathLength * meanIoni;
  meanRad = sign * pathLength * meanRad;
  sigIoni = pathLength * sigIoni;
  sigRad = pathLength * sigRad;
  kazL = pathLength * kazL;

  //
  // include pathlength dependence of Landau ionization
  //
  sigIoni = sigIoni - kazL * log(pathLength);

  deltaE = meanIoni + meanRad;
  sigmaDeltaE = sqrt(sigIoni * sigIoni + sigRad * sigRad);
  ATH_MSG_DEBUG(
    " Energy loss updator deltaE " << deltaE << " meanIoni " << meanIoni << " meanRad " << meanRad << " sigIoni " << sigIoni << " sigRad " << sigRad << " sign " << sign << " pathLength " <<
  pathLength);

  std::unique_ptr<Trk::EnergyLoss> eloss = !m_detailedEloss ? 
    std::make_unique<Trk::EnergyLoss>(deltaE, sigmaDeltaE) :
    std::make_unique<Trk::EnergyLoss>(deltaE, sigmaDeltaE, sigmaDeltaE, sigmaDeltaE, meanIoni, sigIoni,meanRad, sigRad, pathLength);
  
  if (m_useTrkUtils) {
    return eloss.release();
  }

// Code below will not be used if the parameterization of TrkUtils is used

  double m = s_particleMasses.mass[particle];
  double mfrac = s_particleMasses.mass[Trk::electron] / m;
  double mfrac2 = mfrac * mfrac;
  double E = sqrt(p * p + m * m);
  // relativistic properties
  double beta = p / E;
  double gamma = E / m;

  // mean radiation loss + sigma
  // Follow CompPhys Comm 79 (1994) P157-164
  // Brem parameterization in terms of path-length in number of radiation lengths
  double dInX0 = pathcorrection * mat.thicknessInX0();
  double meanZ = exp(-mfrac2 * dInX0);

  double deltaE_rad = (dir == Trk::alongMomentum) ? sign * E * (1. - meanZ) : sign * E * (1. / meanZ - 1.);

  // add e+e- pair production and photonuclear effect for muons at energies above 8 GeV
  if ((particle == Trk::muon) && (E > 8000.)) {
    double deltaEpair = 0.;
    if (E < 1.e6) {
      deltaEpair = (-0.5345 + 6.803e-5 * E + 2.278e-11 * E * E - 9.899e-18 * E * E * E) * dInX0; // E below 1 TeV
    } else {
      deltaEpair = (-2.986 + 9.253e-5 * E) * dInX0; // E above 1 TeV
    }
    deltaE_rad += sign * deltaEpair;
  }

//  The sigma radiation loss of CompPhys Comm 79 (1994) P157-164 has PROBLEMS at low Eloss values

//  double varZ  = exp( -1. * dInX0 * log(3.) / log(2.) ) - exp(-2. * dInX0);
//  double varianceQoverP =  ( dir == Trk::alongMomentum )? 1. / (meanZ * meanZ * p * p) * varZ : varZ / (p * p);
//  double sigmaDeltaE_rad = beta*beta*p*p*sqrt( varianceQoverP );

// Use a simple scale factor applied to the Eloss from Radiation obtained from G4 simulation in stead

  double Radiation_FWHM = m_stragglingErrorScale * 4 * 0.85 * fabs(deltaE_rad) / 3.59524;
  double sigmaDeltaE_rad = s_fwhmToSigma * Radiation_FWHM;

  // ionization
  double deltaE_ioni = 0.;
  double sigmaDeltaE_ioni = 0.;
  if (particle != Trk::electron || m_useBetheBlochForElectrons) {
    if (!mpvSwitch) {
  
      // K A/Z and T max will be returned by the dEdXBetheBloch
      double transKaz{0};
      double transTmax{0};
      // use the dEdX function
      deltaE_ioni = sign * pathLength * dEdXBetheBloch(mat, transKaz,transTmax,beta, gamma, particle);
      // the different straggling functions
      if (m_gaussianVavilovTheory) {
        // use the Gaussian approximation for the Vavilov Theory
        double sigmaE2 = transKaz * pathLength * transTmax * (1. - beta * beta / 2.);
        sigmaDeltaE_ioni = sqrt(sigmaE2);
      } else {
        //      Take FWHM maximum of Landau and convert to Gaussian
        //      For the FWHM of the Landau Bichsel/PDG is used: FWHM = 4 xi = 4 m_transKaz * pathLength
        sigmaDeltaE_ioni = 4. * s_fwhmToSigma * transKaz * pathLength;
      }
    } else {
      double eta2 = beta * gamma;
      eta2 *= eta2;
      // most probable
      double kaz = 0.5 * s_ka_BetheBloch * mat.zOverAtimesRho();
      double xi = kaz * pathLength;
      // get the ionisation potential
      double iPot = 16.e-6 * std::pow(mat.averageZ(), 0.9);
      // density effect, only valid for high energies (gamma > 10 -> p > 1GeV for muons)
      double delta = 0.;
      /* ST replace with STEP-like coding
         if (eta2 > 2.) {
         // high energy density effect
         double eplasma = 28.816e-6 * sqrt(1000.*0.5);
         delta = 2.*log(eplasma/iPot) + log(eta2) - 0.5;
         }
       */
      if (gamma > 10.) {
        double eplasma = 28.816e-6 * sqrt(1000. * mat.zOverAtimesRho());
        delta = 2. * log(eplasma / iPot) + log(eta2) - 1.;
      }
      // calculate the most probable value of the Landau distribution
      double mpv = m_mpvScale * xi / (beta * beta) * (log(m * eta2 * kaz / (iPot * iPot)) + 0.2 - beta * beta - delta);//
                                                                                                                       // 12.325);

      // sigma
      // (1) - obtained from fitted function  [  return par0 + par1*std::pow(x,par2); ]
      // (2) - following Bichsel: fwhm (full with at half maximum) = 4 *xi, w = 2.35 fwhm (for gaussian)
      // use the crude approximation for the width 4*chi
      sigmaDeltaE_ioni = !m_mpvSigmaParametric ? s_fwhmToSigma * 4. * xi : mpv *
                         (s_mpv_p0 + s_mpv_p1 * std::pow(mpv, s_mpv_p2));

      // get the fitted sigma
      deltaE_ioni = sign * mpv;
    }
    // scale the error
    sigmaDeltaE_ioni *= m_stragglingErrorScale;
  }

  deltaE = deltaE_ioni + deltaE_rad;

  sigmaDeltaE = sqrt(sigmaDeltaE_rad * sigmaDeltaE_rad + sigmaDeltaE_ioni * sigmaDeltaE_ioni);

  return(m_detailedEloss ? new EnergyLoss(deltaE, sigmaDeltaE, sigmaDeltaE, sigmaDeltaE,
                                          (mpvSwitch ? deltaE_ioni : 0.9 * deltaE_ioni), sigmaDeltaE_ioni,
                                          deltaE_rad, sigmaDeltaE_rad, pathLength) :
         new EnergyLoss(deltaE, sigmaDeltaE));
}

double
Trk::EnergyLossUpdator::dEdXBetheBloch(const MaterialProperties &mat,
                                       double& transKaz,
                                       double& transTmax,
                                       double beta,
                                       double gamma,
                                       ParticleHypothesis particle) const {
  if (particle == Trk::undefined || particle == Trk::nonInteracting) {
    return 0.;
  }

  if (mat.averageZ() == 0. || mat.zOverAtimesRho() == 0.) {
    ATH_MSG_ERROR(
      "empty material properties pass to the EnergyLossUpdator:Z,zOAtr:" << mat.averageZ() << "," <<
    mat.zOverAtimesRho());
    return 0.;
  }

  // 16 eV * Z**0.9 - bring to MeV
  double iPot = 16.e-6 * std::pow(mat.averageZ(), 0.9);
  // and the electron mass in MeV
  double me = s_particleMasses.mass[Trk::electron];
  double m = s_particleMasses.mass[particle];
  double eta2 = beta * gamma;

  // K/A*Z = 0.5 * 30.7075MeV/(g/mm2) * Z/A * rho[g/mm3]  / scale to mm by this
  double kaz = 0.5 * s_ka_BetheBloch * mat.zOverAtimesRho();

  if (particle != Trk::electron) {
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

    if (gamma > 10.) {
      double eplasma = 28.816e-6 * sqrt(1000. * mat.zOverAtimesRho());
      delta = 2. * log(eplasma / iPot) + log(eta2) - 1.;
    }

    // mass fraction
    double mfrac = me / m;
    // tmax - cut off energy
    double tMax = 2. * eta2 * me / (1. + 2. * gamma * mfrac + mfrac * mfrac);
    // divide by beta^2 for non-electrons
    kaz /= beta * beta;
    // store the transport variables
    transKaz = kaz;
    transTmax = tMax;
    // return
    return kaz * (log(2. * me * eta2 * tMax / (iPot * iPot)) - 2. * (beta * beta) - delta);
  }
  transKaz = kaz;
  // for electrons use slightly different BetheBloch adaption
  // see Stampfer, et al, "Track Fitting With Energy Loss", Comp. Pyhs. Comm. 79 (1994), 157-164
  return kaz * (2. * log(2. * me / iPot) + 3. * log(gamma) - 1.95);
}

double
Trk::EnergyLossUpdator::dEdXBetheHeitler(const MaterialProperties &mat,
                                         double initialE,
                                         ParticleHypothesis particle) const {
  if (particle == Trk::undefined || particle == Trk::nonInteracting) {
    return 0.;
  }

  double mfrac = (s_particleMasses.mass[Trk::electron] / s_particleMasses.mass[particle]);
  mfrac *= mfrac;

  return initialE / mat.x0() * mfrac;
}

// public interface method

Trk::EnergyLoss *
Trk::EnergyLossUpdator::updateEnergyLoss(Trk::EnergyLoss *eLoss, double caloEnergy, double caloEnergyError,
                                         double pCaloEntry, double momentumError, int &elossFlag) const {
//
// Input: the detailed EnergyLoss object in the Calorimeter that contains the different Eloss terms
// and their uncertainties; caloEnergy and error; and the muon momentumError (all in MeV)
//
// For use in the MuonSystem
// Input: caloEnergy = 0. caloEnergyError = 0. and pCaloEntry = pMuonEntry momentum at MuonEntry
//
// Output: an updated Energy loss values deltaE()
//         that can be used in the track fit and corresponds to the Most Probable EnergyLoss value
//         taking into account the ionization, radiation and
//         smearing due to the errors including the momentumError (in MeV)
//
//         elossFlag = false if Calorimeter Energy is NOT stored (and later fitted) on the Eloss object
//                   = true  Calorimeter Energy is stored and will be fitted
//
//  deltaE is used in the final fit
//

  elossFlag = 0;

  int isign = 1;
  if (eLoss->deltaE() < 0) {
    isign = -1;
  }

  double deltaE = eLoss->deltaE();
  double sigmaDeltaE = eLoss->sigmaDeltaE();
// Detailed Eloss
  double deltaE_ioni = eLoss->meanIoni();
  double sigmaDeltaE_ioni = 0.45 * eLoss->sigmaIoni(); // sigma Landau
  double deltaE_rad = eLoss->meanRad();
  double sigmaDeltaE_rad = eLoss->sigmaRad(); // rms and mean of steep exponential
  double depth = eLoss->length();

// Eloss radiative protection

  if (eLoss->meanRad() > 100000.) {
    deltaE_rad = 100000.;
    sigmaDeltaE_rad = eLoss->sigmaRad() * 100000. / eLoss->meanRad();
  }

  double sigmaPlusDeltaE = eLoss->sigmaPlusDeltaE();
  double sigmaMinusDeltaE = eLoss->sigmaMinusDeltaE();

  double MOP = deltaE_ioni - isign * 3.59524 * sigmaDeltaE_ioni;

//  std::cout << " update Energyloss pCaloEntry " << pCaloEntry << " momentumError " << momentumError << " caloEnergy "
// << caloEnergy << std::endl;

//  std::cout << " update Energyloss old deltaE " << deltaE << " sigmaPlusDeltaE " << sigmaPlusDeltaE << "
// sigmaMinusDeltaE " << sigmaMinusDeltaE << std::endl;
//
// MOP shift due to ionization and radiation
//
  double MOPshift = isign * 50 * 10000. / pCaloEntry + isign * 0.75 * sqrt(sigmaDeltaE_ioni * sigmaDeltaE_rad);
  double MOPshiftNoRad = isign * 50 * 10000. / pCaloEntry;
//
// define sigmas for Landau convoluted with exponential
//
  double fracErad = sigmaDeltaE_rad + fabs(deltaE_rad) * pCaloEntry / (800000. + pCaloEntry);
  double sigmaL = sigmaDeltaE_ioni + 0.8*fracErad / 3.59524;
//  double sigmaLNoRad = sigmaDeltaE_ioni;
  double sigmaMinus = 1.02 * sigmaDeltaE_ioni + 0.08 * sigmaDeltaE_rad;
  double sigmaPlus = 4.65 * sigmaDeltaE_ioni + 1.16 * sigmaDeltaE_rad;
//  double sigmaMinusNoRad = 1.02 * sigmaDeltaE_ioni;
//  double sigmaPlusNoRad = 4.65 * sigmaDeltaE_ioni;
  double xc = momentumError/sigmaL;
  double correction = (0.3849*xc*xc + 7.76672e-03*xc*xc*xc)/(1+2.8631*xc+0.3849*xc*xc + 7.76672e-03*xc*xc*xc);
//  std::cout << " Eloss error plus incl radiation " << sigmaPlus << " no radiation " << sigmaPlusNoRad << std::endl;
//
// Case where the measured Calorimeter energy is not available (e.g. low pT or not isolated)
//

  if (caloEnergyError <= 0) {
//
// Shift of MOP due to momentum resolution
//
    double MOPreso = isign * 3.59524 * sigmaL * correction;

    deltaE = MOP + MOPshift + MOPreso;
    sigmaMinusDeltaE = sigmaMinus;
    sigmaPlusDeltaE = sigmaPlus;
    sigmaDeltaE = sqrt(0.5 * sigmaMinusDeltaE * sigmaMinusDeltaE + 0.5 * sigmaPlusDeltaE * sigmaPlusDeltaE);
//
    if (m_optimalRadiation && fabs(deltaE) < caloEnergy && pCaloEntry > 100000) {
//
// Calorimeter measurement can be used as veto to say there was no significant radiation
//
// In that case the Eloss is taken as the ionization Eloss
// Use MOP after correction for landau tail (MOPshiftNoRad) and momentum resolution smearing (MOPreso)
//
      sigmaL = sigmaDeltaE_ioni + 0.3*fracErad / 3.59524;
      xc = momentumError/sigmaL;
      correction = (0.3849*xc*xc + 7.76672e-03*xc*xc*xc)/(1+2.8631*xc+0.3849*xc*xc + 7.76672e-03*xc*xc*xc);

      MOPreso = isign * 3.59524 * sigmaL * correction;
      deltaE = MOP + MOPshift + MOPreso;
      sigmaMinusDeltaE = sigmaMinus;
      sigmaPlusDeltaE = sigmaPlus;
      sigmaDeltaE = sqrt(0.5 * sigmaMinusDeltaE * sigmaMinusDeltaE + 0.5 * sigmaPlusDeltaE * sigmaPlusDeltaE);
    }
//     std::cout << " updateEnergyLoss CaloEnergyError = 0 delta E " << deltaE << " sigmaMinusDeltaE " <<
// sigmaMinusDeltaE << " sigmaPlusDeltaE " << sigmaPlusDeltaE << std::endl;
  } else {
    double sigmaPlusTot = sqrt(sigmaPlus * sigmaPlus + caloEnergyError * caloEnergyError);
    if (m_optimalRadiation) {
      sigmaPlusTot = sqrt(4.65 * sigmaDeltaE_ioni * 4.65 * sigmaDeltaE_ioni + caloEnergyError * caloEnergyError);
    }
    double MOPtot = fabs(MOP + MOPshift);
    if (m_optimalRadiation) {
      MOPtot = fabs(MOP + MOPshiftNoRad);
    }

    if (caloEnergy > MOPtot + 2 * sigmaPlusTot) {
//
// Use measured Calorimeter energy
//
//
// take into account the tail in the Measured Eloss
//
      double MOPreso = isign * 3.59524 * sigmaL * correction;
      deltaE = isign * caloEnergy + MOPreso;
      sigmaMinusDeltaE = caloEnergyError + 0.08 * sigmaDeltaE_rad;
      sigmaPlusDeltaE = caloEnergyError + 1.16 * sigmaDeltaE_rad;
      sigmaDeltaE = sqrt(0.5 * sigmaMinusDeltaE * sigmaMinusDeltaE + 0.5 * sigmaPlusDeltaE * sigmaPlusDeltaE);
      elossFlag = 1;
//      std::cout << " updateEnergyLoss caloEnergy " <<  isign*caloEnergy << " shift " << MOPreso << " deltaE " <<
// deltaE << " sigmaMinusDeltaE " << sigmaMinusDeltaE << " sigmaPlusDeltaE " << sigmaPlusDeltaE << std::endl;
//      std::cout << " updateEnergyLoss CaloEnergy used delta E " << deltaE << " sigmaMinusDeltaE " << sigmaMinusDeltaE
// << " sigmaPlusDeltaE " << sigmaPlusDeltaE << std::endl;
    } else {
// Use MOP after corrections

//
// Shift of MOP due to momentum resolution smearing
//
      sigmaL = sigmaDeltaE_ioni + 0.3*fracErad / 3.59524;
      xc = momentumError/sigmaL;
      correction = (0.3849*xc*xc + 7.76672e-03*xc*xc*xc)/(1+2.8631*xc+0.3849*xc*xc + 7.76672e-03*xc*xc*xc);
      double MOPreso = isign * 3.59524 * sigmaL * correction;
//
// Use MOP after correction for landau tail (MOPshiftNoRad) and radiation (MOPshift) and momentum resolution smearing
// (MOPreso)
//
      deltaE = MOP + MOPshift + MOPreso;
      sigmaMinusDeltaE = sigmaMinus;
      sigmaPlusDeltaE = sigmaPlus;
      sigmaDeltaE = sqrt(0.5 * sigmaMinusDeltaE * sigmaMinusDeltaE + 0.5 * sigmaPlusDeltaE * sigmaPlusDeltaE);
//      std::cout << " updateEnergyLoss CaloEnergy NOT used delta E " << deltaE << " sigmaMinusDeltaE " <<
// sigmaMinusDeltaE << " sigmaPlusDeltaE " << sigmaPlusDeltaE << std::endl;
    }
  }

//  std::cout << " update Energyloss new deltaE " << deltaE << " sigmaPlusDeltaE " << sigmaPlusDeltaE << "
// sigmaMinusDeltaE " << sigmaMinusDeltaE << std::endl;

  return new EnergyLoss(deltaE, sigmaDeltaE, sigmaMinusDeltaE, sigmaPlusDeltaE, deltaE_ioni, sigmaDeltaE_ioni,
                        deltaE_rad, sigmaDeltaE_rad, depth);
}

// public interface method
void
Trk::EnergyLossUpdator::getX0ElossScales(int icalo, double eta, double phi, double &X0Scale, double &ElossScale) const {
  //
  // for Calorimeter icalo = 1
  //     Muon System icalo = 0
  // convention eta, phi is at Calorimeter Exit (or Muon Entry)
  //            eta and phi are from the position (not direction)
  //
  // input   X0 and ElossScale = 1
  // output  updated X0Scale and ElossScale
  //

  double X0CaloGirder[4] = {
    -1.02877e-01, -2.74322e-02, 8.12989e-02, 9.73551e-01
  };

// GEO-20-00-01 used for rel 19 and 20
//  double X0CaloScale[60] = {1.03178 ,1.03178 ,1.03178 ,1.03178 ,1.01512 ,1.01456 ,1.01266 ,1.02299 ,1.01437 ,1.01561
// ,1.01731 ,1.02502 ,1.01171 ,0.987469 ,0.979134 ,1.01018 ,1.01813 ,0.986147 ,0.973686 ,0.983771 ,0.960334 ,0.967629
// ,0.982935 ,0.993162 ,1.00241 ,1.00012 ,0.9814 ,0.937632 ,0.927918 ,0.93491 ,0.934184 ,0.921438 ,0.933788 ,0.954208
// ,0.973684 ,0.977765 ,1.00697 ,1.01779 ,1.02020 ,1.01604 ,1.02121 ,1.02344 ,1.02888 ,1.02278 ,1.03096 ,1.02409
// ,1.03149 ,1.03797 ,1.04254 ,1.04943 ,1.02433 ,1.02379 ,1.02177 ,1.02271 ,1.02011 ,1.02414 ,1.03649 ,1.03739 ,1.03739
// ,1.03739};

// R2012 to R2015 determined on ATLAS-R2-2015-03-01-00 to be used in rel 21
  double X0CaloScale[60] = {
    1.01685, 1.02092, 1.01875, 1.01812, 1.01791, 1.01345, 1.01354, 1.02145, 1.01645, 1.01585, 1.0172, 1.02262, 1.01464,
    0.990931, 0.971953, 0.99845, 1.01433, 0.982143, 0.974015, 0.978742, 0.960029, 0.966766, 0.980199, 0.989586,
    0.997144, 1.00169, 0.994166, 0.966332, 0.93671, 0.935656, 0.921994, 0.901489, 0.897799, 0.89638, 0.905629, 0.903374,
    0.925922, 0.941203, 0.956273, 0.968618, 0.976883, 0.988349, 0.99855, 1.00212, 1.01456, 1.01541, 1.02532, 1.03238,
    1.03688, 1.03783, 1.02078, 1.01529, 1.0156, 1.02212, 1.02226, 1.02406, 1.02188, 1.00661, 1.00661, 1.00661
  };

// GEO-20-00-01 used for rel19 and 20
//  double ElossCaloScale[30] = { 1.03504 ,1.03504 ,1.03504 ,1.02631 ,1.04258 ,1.03506 ,1.05307 ,1.02399 ,1.04237
// ,1.04368 ,1.04043 ,1.05899 ,1.07933 ,1.08604 ,1.08984 ,1.03564 ,1.04158 ,1.05983 ,1.06291 ,1.06853 ,1.0674 ,1.05427
// ,1.06466 ,1.06274 ,1.06141 ,1.06314 ,1.06868 ,1.07242 ,1.07242 ,1.07242};

// R2012 to R2015 determined on ATLAS-R2-2015-03-01-00 to be used in rel 21
  double ElossCaloScale[30] = {
    1.06921, 1.06828, 1.06734, 1.06092, 1.06638, 1.06335, 1.07421, 1.05885, 1.07351, 1.07435, 1.06902, 1.07704, 1.08782,
    1.09844, 1.115, 1.07609, 1.08233, 1.08764, 1.08209, 1.08255, 1.08008, 1.07573, 1.077, 1.07271, 1.07343, 1.07769,
    1.07794, 1.08377, 1.08377, 1.08377
  };

// never used
  double X0MuonScale[60] = {
    -0.0320612, -0.0320612, -0.0320612, -0.0320612, -0.0693796, -0.0389677, -0.0860891, -0.124606, -0.0882329,
    -0.100014, -0.0790912, -0.0745538, -0.099088, -0.0933711, -0.0618782, -0.0619762, -0.0658361, -0.109704, -0.129547,
    -0.143364, -0.0774768, -0.0739859, -0.0417835, -0.022119, 0.00308797, 0.0197657, -0.0137871, -0.036848, -0.0643794,
    -0.0514949, -0.0317105, 0.016539, 0.0308435, -0.00056883, -0.00756813, -0.00760612, -0.0234571, -0.0980915,
    -0.101175, -0.102354, -0.0920337, -0.100337, -0.0887628, 0.0660931, 0.228999, 0.260675, 0.266301, 0.267907,
    0.281668, 0.194433, 0.132954, 0.20707, 0.220466, 0.20936, 0.191441, 0.191441, 0.191441, 0.191441, 0.191441, 0.191441
  };


  int i60 = fabs(eta) * 20.;

  if (i60 < 0) {
    i60 = 0;
  }
  if (i60 > 59) {
    i60 = 59;
  }

  if (icalo == 1) {
//
// Girder parametrization
//
    double x = phi + 3.1416 - 3.1416 / 32. * int((3.1416 + phi) / (3.1416 / 32.));
    double scale = 0.;
    double pi = acos(-1.);
    if (x > pi / 64.) {
      x = pi / 32. - x;
    }

    if (x < 0.005) {
      scale = X0CaloGirder[0] * (1 - x / 0.005) + X0CaloGirder[1] + X0CaloGirder[3];
    } else if (x < 0.017) {
      scale = X0CaloGirder[1] + X0CaloGirder[3];
    } else if (x < 0.028) {
      scale = X0CaloGirder[2] + X0CaloGirder[3];
    } else {
      scale = X0CaloGirder[3];
    }

    if (fabs(eta) > 1.3) {
      scale = 1.;
    }
//
// eta dependence of X0
//
    scale *= X0CaloScale[i60];
    X0Scale = scale;
//
// eta dependence of Eloss
//
    int i30 = fabs(eta) * 10.;
    if (i30 < 0) {
      i30 = 0;
    }
    if (i30 > 29) {
      i30 = 29;
    }

    double nfactor = 0.987363 / 1.05471;

    ElossScale = nfactor * ElossCaloScale[i30] * scale;
  } else {
//
// Muon system
//
// eta dependence of X0
//
    double scale = 1. + X0MuonScale[i60];
//
// Muon scale is now 1 with MuonTrackingGeometry and TrkDetDescrGeoModelCnv fixes
//
    scale = 1.0;
    X0Scale = scale;
    ElossScale = 0.93 * scale;
  }
}

Trk::EnergyLoss *
Trk::EnergyLossUpdator::ionizationEnergyLoss(const MaterialProperties &mat,
                                             double p,
                                             double pathcorrection,
                                             PropDirection dir,
                                             ParticleHypothesis particle) const {
  // preparation
  double sign = (dir == Trk::oppositeMomentum) ? -1. : 1.;
  double pathLength = pathcorrection * mat.thicknessInX0() * mat.x0();

  double sigIoni = 0.;
  double kazL = 0.;

  double meanIoni = sign *
                    m_matInt.PDG_energyLoss_ionization(p, &(mat.material()), particle, sigIoni, kazL, pathLength);

  return !m_detailedEloss ? new Trk::EnergyLoss(meanIoni, sigIoni) :
         new Trk::EnergyLoss(meanIoni, sigIoni, sigIoni, sigIoni, meanIoni, sigIoni, 0., 0., pathLength);
}
