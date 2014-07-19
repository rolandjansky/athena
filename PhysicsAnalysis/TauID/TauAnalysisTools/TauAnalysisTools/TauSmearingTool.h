// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUSMEARINGTOOL_H
#define TAUSMEARINGTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Local include(s):
#include "TauAnalysisTools/ITauSmearingTool.h"

#include "TH1D.h"
#include "TFile.h"

namespace TauAnalysisTools {

  class TauSmearingTool : public virtual ITauSmearingTool,
			  public asg::AsgTool {

    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS( TauSmearingTool,
		    TauAnalysisTools::ITauSmearingTool )

    public:
    /// Create a constructor for standalone usage
    TauSmearingTool( const std::string& name );

    ~TauSmearingTool( );

    /// @name Function(s) implementing the asg::IAsgTool interface

    /// Function initialising the tool
    virtual StatusCode initialize();

    /// @name Function(s) implementing the ITauCorrectionTool interface

    /// Apply the correction on a modifyable object
    virtual TauAnalysisTools::CorrectionCode applyCorrection( xAOD::TauJet& xTau );
    /// Create a corrected copy from a constant tau
    virtual TauAnalysisTools::CorrectionCode correctedCopy( const xAOD::TauJet& input,
							    xAOD::TauJet*& output);

    enum TESComponent {
      FINAL                  = 0,
      SHOWERMODEL            = 1,
      ////
      UE                     = 2,
      DM                     = 3,
      CLOSURE                = 4,
      PU                     = 5,
      OTHERS                 = 6,		// DOES NOT CONTAIN PU
      //Eiter use:
      SINGLEPARTICLE         = 7,
      //or split single particle uncertainty parameters
      EOP_RSP                = 8,
      EOP_SYST               = 9,
      CTB_SYST               = 10,
      CTB_RSP                = 11,
      EM                     = 12,
      LCW                    = 13,
      Bias                   = 14,
      //Or use the following combined EOP parameter
      EOP                    = 15,
      CTB                    = 16,
      INSITUSTAT             = 17,
      INSITUSYST             = 18,
      SYSREST                = 19, // in-situ systematics + remaining systematics
      TOTAL                  = 20,
      REMAININGSYS           = 21, // total - closure - pu
      MODELING               = 22,
      INSITU                 = 23,
      INSITUINTERPOL         = 24,
      INSITUSTATINTERPOL     = 25,
      INSITUSYSTINTERPOL     = 26,
      SINGLEPARTICLEINTERPOL = 27
    };

  private:
    bool m_bIsData ;
    std::string m_sDataFile;
    int m_iDirection;
    const double GeV;
    const unsigned int m_iEtaBins;
    //in-situ uncertainties
    const double m_dInSituStat1P;
    const double m_dInSituStat3P;
    const double m_dInSituSyst1P;
    const double m_dInSituSyst3P;
    int m_iTESComponent;
         
    double interpolateLin(const double& pt);
    void   loadInputHistograms();
    void   getHist(TFile* fFile, std::string sName, std::string sMapName = "");
    double getTESShift(double pt, int ntracks);
    double getTESUncertainty(double pt, // MeV
			     double eta,
			     int ntracks,
			     int iComponent);

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
     
  }; // class TauSmearingTool

} // namespace TauAnalysisTools

#endif // TAUSMEARINGTOOL_H
