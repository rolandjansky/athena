/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_PAI_effectiveGas_h
#define TRT_PAI_effectiveGas_h

#include "AthenaKernel/MsgStreamMember.h"

#include <vector>

class TRT_PAI_gasMixture;

/**
 * Effective gas: All quantities necessary to calculate PAI ionisation for
 * gas mixture.
 */
class TRT_PAI_effectiveGas {
public:
  /**
   * \param gm:    gas mixture
   * \param Emin:  minimum energy transfer considered in calculations [eV]
   * \param Emax:  maximum energy transfer considered in calculations [ev]
   * \param tempK: gas temperature [Kelvin]
   * \param eps:   epsilon for numeric integration
   * \param mlog:  Message stream
   */
  TRT_PAI_effectiveGas(TRT_PAI_gasMixture * gm,
		       double Emin,
		       double Emax,
		       double tempK,
		       double eps);
  /**
   * Tabulate double differential distribution
   * \param gamvec:  vector of gamma values for which to tabulate
   * \param Earray:  vector of energy transfer values
   * \param fnArray: double differential distribution
   * \param dndx:    This array effectively contains the inverse of the mean
   *                 free path as function of binned energy variable
   */
  void GasTab(std::vector<float> gamvec,
	      std::vector<float>& EArray,
	      std::vector< std::vector<float> >& fnArray,
	      std::vector<float>& dndx);
  
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  bool msgLevel (MSG::Level lvl)    { return m_msg.get().level() <= lvl; }

private:
  double XSigma(double lnE, double dummy);
  double XFReal(double lnD, double lnE);
  double XGInt(double (TRT_PAI_effectiveGas::*pt2Func)(double,double),
	       double  lnLo,
	       double  lnHi,
	       const double  eps,
	       double  extraParameter);
  double dndedx(double lgE, double gamma);
  std::vector<float> m_lnELvls;
  std::vector<float> m_lnFosc;
  std::vector<float> m_lnIntegratedSigmas;
  std::vector<float> m_lnEpsR;
  std::vector<float> m_lnEpsI;
  const double m_lnEmin;
  const double m_lnEmax;
  const double m_eps;        // epsilon for numerical integration.
  double m_S1 ;              // x-section to F.osc
  double m_Wp2;              // plasma freq**2 {ev}
  double m_S2 ;              // dN/dx scale
  double m_ne ;              // Electron density

  mutable Athena::MsgStreamMember m_msg;

};

#endif
