/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TES Uncertainty
  ---------------

  Providing TES uncertainties mentioned on
  https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TauSystematicsWinterConf2012

  based on the uncertainty estimation using single particle uncertainties by
  - Matteo Volpi <mvolpi@ifae.es>
  - Nils Ruthmann <ruthmann@cern.ch>

  Code refactored by Noel Dawe <noel.dawe@cern.ch>
*/

#ifndef TESUNCERTAINTY_H
#define TESUNCERTAINTY_H

#include <vector>
#include <map>
#include <math.h>
#include <iostream>

#include "stdio.h"
#include "TFile.h"
#include "TH1D.h"
#include "TString.h"
#include "TMath.h"
#include "TObject.h"

namespace TauCorrUncert {

  class TESUncertainty : public TObject
  {

  public:

    TESUncertainty(TString datafile);

    ~TESUncertainty();

    /*
     * Please use for now only SINGLEPARTICLE or OTHERS
     */
    enum TESComponent {
      FINAL,       //0
      SHOWERMODEL,//1
      ////
      UE,//2
      DM,//3
      CLOSURE,//4
      PU,//5
      OTHERS,	//6        DOES NOT CONTAIN PU
      //Eiter use:
      SINGLEPARTICLE,//7
      //or split single particle uncertainty parameters
      EOP_RSP,//8
      EOP_SYST,//9
      CTB_SYST,//10
      CTB_RSP,//11
      EM,//12
      LCW,//13
      Bias,//14
      //Or use the following combined EOP parameter
      EOP,//15
      CTB,//16
      INSITUSTAT,
      INSITUSYST,
      SYSREST, // in-situ systematics + remaining systematics
      TOTAL,
      REMAININGSYS, // total - closure - pu
      MODELING,
      INSITU,
      INSITUINTERPOL,
      INSITUSTATINTERPOL,
      INSITUSYSTINTERPOL,
      SINGLEPARTICLEINTERPOL
    };

    /*
     * Get the TES uncertainty (%).
     * Provide pt in MeV!
     * Returns -1 in case of failure.
     */
    double GetTESUncertainty(double pt,
			     double eta,
			     int nProng,
			     TESComponent component = FINAL);
    double GetTESShift(double pt,
		       int nProng);

  private:

    double getInSituSys(const int& ntracks);
    double getInSituStat(const int& ntracks);
    double getSystRest(const double& pt,
		       const int& ntracks,
		       const int&, const unsigned int&);
    double getModeling(const double& pt,
		       const unsigned int& itracks,
		       const unsigned int& iE);
    double getClosure(const double& pt,
		      const unsigned int& itracks,
		      const unsigned int& iE);
    double getSingleParticleResponse(const double& pt,
				     const unsigned int& itracks,
				     const unsigned int& iE);
    double getSingleParticleResponseInterpolated(const double& pt,
						 const unsigned int& itracks,
						 const unsigned int& iE);

    double getInSitu(const unsigned int& itracks);
    double getInSitu(const double& pt,
		     const unsigned int& itracks);
    
    double interpolateLin(const double& pt);
    TH1D* GetHist(TFile* file, TString name);

    std::map<TESComponent, TString> map_components;
    std::map<TESComponent, double> map_uncertaintycomponents;
    std::map<TString, TH1D*> map_histo;

    int m_nEta;
    int m_nPt;

    const double GeV;
    //in-situ uncertainties
    const double m_dInSituStat1P;
    const double m_dInSituStat3P;
    const double m_dInSituSyst1P;
    const double m_dInSituSyst3P;
    ClassDef(TESUncertainty, 1)
  };
} // namespace TauCorrUncert

#endif
