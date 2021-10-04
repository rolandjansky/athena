/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_PAI_Process.h"
#include "TRT_PAI_effectiveGas.h"
#include "TRT_PAI_gasMixture.h"
#include "TRT_PAI_gasComponent.h"
#include "TRT_PAI_element.h"
#include "TRT_PAI_utils.h"
#include "TRT_PAI_physicsConstants.h"

#include <vector>
#include <iostream>
#include <complex>
#include <cmath>
#include <algorithm> //for std::max, std::clamp

#include "CLHEP/Units/SystemOfUnits.h"

//____________________________________________________________________________

TRT_PAI_effectiveGas::TRT_PAI_effectiveGas(TRT_PAI_gasMixture * gm,
					   double Emin,
					   double Emax,
					   double tempK,
					   double eps)
  : m_lnEmin(std::log(Emin)),
    m_lnEmax(std::log(Emax)),
    m_eps(eps),
    m_msg("TRT_PAI_effectiveGas")
{
  using namespace TRT_PAI_physicsConstants;

  TRT_PAI_element* pe;
  double rho  = 0.;
  double Zeff = 0.;
  double Aeff = 0.;
  double w    = 0.;
  double wtot = 0.;
  int Nelem   = gm->getNElements();

  for ( int k=0; k<Nelem; k++ ) {
    pe    = gm->getElement(k);
    w     = gm->getElemWeight(k);
    wtot += w;
    rho  += w * pe->getDensity(tempK);
    Aeff += w * pe->getAtomicA();
    Zeff += w * pe->getAtomicZ();
  }

  Aeff /= wtot;
  Zeff /= wtot;

  m_ne  = Nav*rho*Zeff/Aeff;         // Electron density
  m_Wp2 = 4.*M_PI*r0*m_ne*he*he;       // plasma freq**2 {ev}
  m_S1  = mb/(2.*M_PI*M_PI*r0*he*Zeff);  // x-section to F.osc
  m_S2  = 2.*M_PI*r0*m_ne*Qe*Qe/erg;   // dN/dx scale

  ATH_MSG_DEBUG ( "effectiveGas: Electron density     " << m_ne );
  ATH_MSG_DEBUG ( "effectiveGas: Plasma freq**2 {eV}  " << m_Wp2 );
  ATH_MSG_DEBUG ( "effectiveGas: x-section to F.osc   " << m_S1 );
  ATH_MSG_DEBUG ( "effectiveGas: dN/dx scale          " << m_S2 );

  // Merge all energy levels into ELvls
  std::vector<float> tempv;
  for ( int k=0; k<Nelem; k++ ) {
    pe    = gm->getElement(k);
    tempv = pe->getLnELvls();
    m_lnELvls.insert( m_lnELvls.end(), tempv.begin(), tempv.end() );
  }

  // Insert also Emin and Emax into ELvls, then sort
  m_lnELvls.push_back(m_lnEmin);
  m_lnELvls.push_back(m_lnEmax);
  sort(m_lnELvls.begin(),m_lnELvls.end());

  std::vector<float>::iterator vit;

  // Remove duplicate energies
  vit = unique(m_lnELvls.begin(), m_lnELvls.end());
  m_lnELvls.erase(vit, m_lnELvls.end());

  // Remove everything below Emin
  vit = lower_bound(m_lnELvls.begin(), m_lnELvls.end(), m_lnEmin);
  m_lnELvls.erase(m_lnELvls.begin(), vit);

  // Remove everything above Emax
  vit = upper_bound(m_lnELvls.begin(), m_lnELvls.end(), m_lnEmax)-1;
  m_lnELvls.erase(vit, m_lnELvls.end());

  int NLvls = m_lnELvls.size();


  // Expand vectors to correct dimension
  m_lnFosc.resize(NLvls);
  m_lnIntegratedSigmas.resize(NLvls);
  m_lnEpsI.resize(NLvls);
  m_lnEpsR.resize(NLvls);

  // create array of effective cross sections (Fosc).

  for ( int i=0; i<NLvls; i++ ) {
    double fosc = 0.;
    // all atoms with an absorbtion energylevel low enough contribute.
    for ( int k=0; k<Nelem; k++ ) {
      pe = gm->getElement(k);
      if ( m_lnELvls[i] >= pe->getLnELvls()[0] ) {
	double lnSig = TRT_PAI_utils::Interpolate( m_lnELvls[i],
						   pe->getLnELvls(),
						   pe->getLnSigmas() );
	fosc += std::exp(lnSig) * gm->getElemWeight(k);
      }
    }
    //clamp, just in case
    fosc = std::clamp(fosc, 1e-300,1e+300);
    m_lnFosc[i] = std::log(fosc);
  }

  // Create array of integrated cross-sections
  double sigma = 0.;
  double lnSigma = 0.;
  double lnEHigh;
  double lnELow = m_lnEmax;
  double dsigma;

  for ( int i=NLvls-1; i>=0; --i) {
    lnEHigh = lnELow;
    lnELow  = m_lnELvls[i];
    dsigma  = XGInt( &TRT_PAI_effectiveGas::XSigma,
		     lnELow,
		     lnEHigh,
		     m_eps,
		     0.);
    sigma  += dsigma;
    lnSigma = std::log(sigma);
    m_lnIntegratedSigmas[i] = lnSigma;
  }

  // Normalize
  for ( int i=0; i<NLvls; i++) {
    m_lnFosc[i]             -= lnSigma;
    m_lnIntegratedSigmas[i] -= lnSigma;
  }

  double cnst = std::log(M_PI_2*m_Wp2);
  for (int i=0; i<NLvls; i++) {
    m_lnEpsI[i]  =  cnst - m_lnELvls[i] + m_lnFosc[i];
    float xint = XGInt(&TRT_PAI_effectiveGas::XFReal,
		       0.,
		       m_lnEmax,
		       m_eps,
		       m_lnELvls[i] );
    m_lnEpsR[i]  = m_Wp2 * std::exp(m_lnELvls[i]) * xint;
  }
  return;
}

//____________________________________________________________________________

double TRT_PAI_effectiveGas::XSigma (double lnE, double dummy) {
  double xsig = dummy;
  xsig = TRT_PAI_utils::Interpolate(std::max(lnE,m_lnEmin),
				    m_lnELvls,
				    m_lnFosc);
  xsig = std::max(xsig,-99.);
  xsig = std::exp( xsig + lnE );
  return xsig;
}

//____________________________________________________________________________

double TRT_PAI_effectiveGas::XFReal (double lnD, double lnE) {

  double fp = 0.;
  if ( lnE+lnD > m_lnEmin ) {
    fp = TRT_PAI_utils::Interpolate(lnE+lnD, m_lnELvls, m_lnFosc);
    fp = std::exp(std::max(fp, -99.0));
  }

  double fm = 0.;
  if (lnE-lnD > m_lnEmin ) {
    fm = TRT_PAI_utils::Interpolate(lnE-lnD, m_lnELvls, m_lnFosc);
    fm = std::exp(std::max(fm, -99.0));
  }

  double x = std::exp(lnD);
  return x/(x*x-1.)*(fp-fm);
}

//____________________________________________________________________________

double TRT_PAI_effectiveGas::XGInt( double (TRT_PAI_effectiveGas::*pt2Func)(double,double),
				    double  lnLo,
				    double  lnHi,
				    const double eps,
				    double  extraParameter ) {
  //
  // Pavel Nevskis "famous integration procedure"
  //
  // Applies the Gauss-Legendre Quadrature method with n=4

  // These constants belong to the Gauss-Legendre quadrature method:
  const int M = 4;                                              // 4 points
  const double U[M]={-.8611363,-.3399810, .3399810 ,.8611363};  // abscissas
  const double W[M]={ .3478548, .6521452, .6521452, .3478548};  // weights

  int N=10;
  double OTB=0,Y,D,result;

  double Dt = 0.5*(lnHi-lnLo);

  do {
    Y   = OTB;
    OTB = 0.;
    D   = Dt/N;
    for (int i=1; i<=N; i++) {
      for (int k=0; k<M; k++) {
	double arg = lnLo + D*(2*i-1+U[k]);
	OTB += W[k] * (this->*pt2Func)(arg, extraParameter );
      }
    }
    OTB *= D;
    result = OTB;
    N *= 2;
    if ( N>100000 ) {
      ATH_MSG_WARNING( "effectiveGas::XGInt: integrate Divergence! " << std::abs(OTB-Y) << " " << std::abs(eps*OTB) );
      break;
    }
  } while ( (std::abs(OTB-Y) > std::abs(eps*OTB)) && (eps>0) );

  return result;
}

//____________________________________________________________________________

double TRT_PAI_effectiveGas::dndedx(double lnE, double gamma) {

  using namespace TRT_PAI_physicsConstants;
  using namespace TRT_PAI_utils;

  double E       = std::exp(lnE);
  double gammaSq = gamma*gamma;
  double betaSq  = 1.-1./gammaSq;

  double er = Interpolate(lnE, m_lnELvls, m_lnEpsR);
  double ei = Interpolate(lnE, m_lnELvls, m_lnEpsI);

  std::complex<double> Ceps1(er/(E*E), std::exp(ei));
  std::complex<double> C1 = 1./gammaSq - Ceps1*betaSq;
  std::complex<double> C2 = C1/(1.+Ceps1) * std::log(2.*betaSq*MeeV/(E*C1));

  double x;
  x = Interpolate(lnE, m_lnELvls, m_lnIntegratedSigmas);
  x = m_S2/betaSq * E * ( -2.*imag(C2)/(m_Wp2*M_PI) + (1.-std::exp(x))/(E*E) );

  return x;
}

//____________________________________________________________________________

void TRT_PAI_effectiveGas::GasTab(std::vector<float> gamvec,
				  std::vector<float>& lnEarray,
				  std::vector< std::vector<float> >& fnArray,
				  std::vector<float>& dndx)
{
  // For a given gamma factor, return dn/dx
  //     (by integrating dn^2/dEdx with respect to E)
  // and tabulated values of dn^2/dEdx(?)....

  double Rener = 0.05;

  int NLvls    = m_lnELvls.size();
  int nGamVals = gamvec.size();

  double lnEi = m_lnEmax;
  double lnEo;
  double lnEs = -99999999.;

  fnArray.resize(nGamVals);
  int nEVals = 0;

  // Initialize
  for ( int ig=0; ig<nGamVals; ++ig ) {
    fnArray[ig].push_back(0.);
  }

  // Calculate integral from above and store the cumulative values in fnArray

  for ( int ie=NLvls-1; ie>=0; --ie ) {
    lnEo = lnEi;
    lnEi = m_lnELvls[ie];
    for ( int ig=0; ig<nGamVals; ++ig ) {
      double ds =
	XGInt( &TRT_PAI_effectiveGas::dndedx, lnEi, lnEo, m_eps, gamvec[ig]);
      fnArray[ig][nEVals] += ds;
    }
    if ( std::abs(lnEs-lnEi) > Rener || ie==0 ) {
      lnEs = lnEi;
      lnEarray.push_back( lnEs );
      if ( ie>0 ) {
	for ( int ig=0; ig<nGamVals; ++ig ) {
	  fnArray[ig].push_back( fnArray[ig][nEVals] );
	}
      }
      nEVals++;
    }
  }

  // Copy the total integral into auxillary vector
  dndx.resize(nGamVals);
  for ( int ig = 0; ig < nGamVals; ++ig ) {
    dndx[ig] = fnArray[ig][nEVals-1];
  }

  return;
}

