// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_TAUSMEARINGRUN1TOOL_H
#define TAUANALYSISTOOLS_TAUSMEARINGRUN1TOOL_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
                    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/README.rst
		    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/README.rst
*/

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Local include(s):
#include "TauAnalysisTools/CommonSmearingTool.h"

// ROOT include(s):
#include "TH1D.h"
#include "TFile.h"

namespace TauAnalysisTools
{

class TauSmearingRun1Tool
  : public CommonSmearingTool
{
  ASG_TOOL_CLASS( TauSmearingRun1Tool,
                  TauAnalysisTools::ITauSmearingTool )

public:
  /// Create a constructor for standalone usage
  TauSmearingRun1Tool( const std::string& sName );

  ~TauSmearingRun1Tool( );

  /// Function initialising the tool
  virtual StatusCode initialize();

  /// Apply the correction on a modifyable object
  virtual CP::CorrectionCode applyCorrection( xAOD::TauJet& xTau );

  /// returns: the list of all systematics this tool can be affected by
  virtual CP::SystematicSet affectingSystematics() const;

  /// returns: the list of all systematics this tool recommends to use
  virtual CP::SystematicSet recommendedSystematics() const;

  virtual CP::SystematicCode applySystematicVariation( const CP::SystematicSet& systConfig );

  enum TESComponent
  {
    // don't use 0, because signed value is needed for differntiate up/down systematics
    FINAL                  = 1,
    SHOWERMODEL            = 2,
    ////
    UE                     = 3,
    DM                     = 4,
    CLOSURE                = 5,
    PU                     = 6,
    OTHERS                 = 7,		// DOES NOT CONTAIN PU
    //Eiter use:
    SINGLEPARTICLE         = 8,
    //or split single particle uncertainty parameters
    EOP_RSP                = 9,
    EOP_SYST               = 10,
    CTB_SYST               = 11,
    CTB_RSP                = 12,
    EM                     = 13,
    LCW                    = 14,
    Bias                   = 15,
    //Or use the following combined EOP parameter
    EOP                    = 16,
    CTB                    = 17,
    INSITUSTAT             = 18,
    INSITUSYST             = 19,
    SYSREST                = 20, // in-situ systematics + remaining systematics
    TOTAL                  = 21,
    REMAININGSYS           = 22, // total - closure - pu
    MODELING               = 23,
    INSITU                 = 24,
    INSITUINTERPOL         = 25,
    INSITUSTATINTERPOL     = 26,
    INSITUSYSTINTERPOL     = 27,
    SINGLEPARTICLEINTERPOL = 28
  };

private:
  const double m_GeV;
  const unsigned int m_iEtaBins;
  //in-situ uncertainties
  const double m_dInSituStat1P;
  const double m_dInSituStat3P;
  const double m_dInSituSyst1P;
  const double m_dInSituSyst3P;

  double interpolateLin(const double& pt);
  StatusCode loadInputHistograms();
  void   getHist(TFile* fFile, std::string sName, std::string sMapName = "");
  double getTESShift(double pt, int ntracks);
  double getTESUncertainty(double pt, // MeV
                           double eta,
                           int ntracks,
                           int iComponent);

  double getTotal(const double& pt, const unsigned int& itrack, const unsigned int& iE);
  double getModeling(const double& pt, const unsigned int& itrack, const unsigned int& iE);
  double getClosure(const double& pt, const unsigned int& itrack, const unsigned int& iE);
  double getInSitu(const unsigned int& itrack);
  double getInSitu(const double& pt, const unsigned int& itrack);
  double getSingleParticleResponse(const double& pt, const unsigned int& itrack, const unsigned int& iE);
  double getSingleParticleResponseInterpolated(const double& pt, const unsigned int& itrack,const unsigned int& iE);
  double getInSituStat(const int& ntracks);
  double getInSituSys(const int& ntracks);
  double getSystRest(const double& pt, const int& ntracks, const int& itrack, const unsigned int& iE);

  std::string convertConfigToString(int iTESComponent, int iTrack, int iEta);



  std::map<int, std::string> m_mComponents;
  std::map<std::string, TH1D*> m_hHisto;

  std::vector<double> m_vEtaBins;
  std::vector<std::string> m_vPF_EtaBins;

}; // class TauSmearingRun1Tool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_TAUSMEARINGRUN1TOOL_H
