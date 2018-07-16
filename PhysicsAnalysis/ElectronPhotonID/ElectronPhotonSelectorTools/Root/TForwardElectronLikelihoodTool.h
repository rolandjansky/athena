// Dear emacs, this is -*-c++-*-
//----------------------------------------------------------------------------------------

/**
   Author: Kurt Brendlinger <kurb@sas.upenn.edu>
   
   Includes the following operating points:
   "Tight"      - Same background rejection as current (2012) tightpp  menu
   "VeryTight"  - Same signal efficiency    as current (2012) tightpp  menu
   "Medium"     - Same signal efficiency    as current (2012) mediumpp menu
   "VeryLoose"  - Same background rejection as current (2012) multilepton menu
   "Loose"      - Same signal efficiency    as current (2012) multilepton menu
   
   Usage:
   In order to compile this outside the Athena framework, you also need to get PATCore from svn.
   You need to include the header where you want to use it:
   #include "ElectronPhotonSelectorTools/TElectronLikelihoodTool.h"

   Then, before the event loop starts, you need to create an instance of this tool:
   Root::TElectronLikelihoodTool* myElLHTool = new TElectronLikelihoodTool();

   configure it:
   myElLHTool->setPDFFileName( "path/to/package/data/ElectronLikelihoodPdfs.root" );
   myElLHTool->setOperatingPoint( LikeEnum::VeryLoose ); // Just an example, see available enums below.

   and initialize it:
   myElLHTool->initialize();

   To get the likelihood value for this electron, do:
   double likelihood = double( myElLHTool->calculate(...) );

   To see if an electron passes this selection (in this example, the "VeryLoose" selection), do:
   bool passThisElectron = bool( myElLHTool->accept( likelihood, ... ) );

   See below which variables you have to use.


   In order to correctly apply the macro, you must use the following (d3pd) variables as inputs:
   eta 		: el_etas2
   eT (in MeV) 	: energy-rescaled (using egammaAnalysisUtils' EnergyRescalerUpgrade) pt, where pt is defined in 
   twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/ElectronsEnergyDirection
   so el_cl_pt if nSiHits < 4, or else 
   el_cl_E/cosh(el_etas2)   THIS IS PROBABLY NOT CORRECT!!! Don't want to use rescaled energies. To be verified... 
   f3               : el_f3
   rHad 		: el_Ethad / pt (see above for pt)
   rHad1 		: el_Ethad1/ pt
   Reta     	: el_reta
   w2 		: el_weta2
   f1       	: el_f1
   wstot    	: el_wstot
   eratio 		: (el_emaxs1+el_Emax2 == 0.) ? 0. : (el_emaxs1-el_Emax2)/(el_emaxs1+el_Emax2)
   (i.e. Eratio)
   deltaEta 	: el_deltaeta1
   d0 		: el_trackd0pvunbiased
   TRratio 		: el_TRTHighTHitsRatio
   eOverP 		: el_cl_E * el_trackqoverp (where el_cl_E is energy-rescaled)
   deltaPhi 	: el_deltaphi2
   d0sigma 		: el_tracksigd0pvunbiased
   rphi     	: el_rphi
   nTRT 		: el_nTRTHits
   nTRTOutliers 	: el_nTRTOutliers
   nSi 		: el_nSiHits
   nSiOutliers 	: el_nPixelOutliers + el_nSCTOutliers
   nPix 		: el_nPixHits
   nPixOutliers 	: el_nPixelOutliers
   nBlayer 		: el_nBLHits
   nBlayerOutliers 	: el_nBLayerOutliers
   expectBlayer 	: el_expectHitInBLayer
   convBit 		: el_isEM & (0x1 << egammaPID::ConversionMatch_Electron)
   ip 		: Count number of vertices in vxp_n with >= 2 tracks in vxp_trk_n

   Created:
   June 2011

*/
//-------------------------------------------------------------------------------------------------------


#ifndef TFORWARDELECTRONLIKELIHOODTOOL_H
#define TFORWARDELECTRONLIKELIHOODTOOL_H


#include <fstream>
#include <iostream>

// ROOT includes
#include "TFile.h"
#include "TH1.h"

// Include the return objects and the base class
#include "PATCore/TAccept.h"
#include "PATCore/TResult.h"
#include "PATCore/TCalculatorToolBase.h"
#include "PATCore/TSelectorToolBase.h"
#include "AsgTools/AsgMessaging.h"


//#define IP_FBINS 2
//#define IP_FBINS 1

namespace{

const unsigned int  IP_FBINS=1;
}

namespace LikeEnumForward {
  enum Menu {
    Loose,
    Medium,
    Tight,
    CustomOperatingPoint
  };

  struct LHAcceptVars_t{
    double likelihood;
    double eta;
    double eT;
    double ip;        
  };

  struct LHCalcVars_t{
    double eta;
    double eT;
    double secondLambda;
    double lateral; 
    double longitudinal; 
    double fracMax; 
    double centerLambda;
    double secondR;
    double secondDensity;
    double significance;
    double ip;        
    int set;};

   struct ROOT6_NamespaceAutoloadHook{};
}

namespace Root {
  class TForwardElectronLikelihoodTool : public TCalculatorToolBase, public TSelectorToolBase, public asg::AsgMessaging
  {

  public: 
    /// Standard constructor

    TForwardElectronLikelihoodTool(const char* name = "TForwardElectronLikelihoodTool");
    
    /// Standard destructor
    ~TForwardElectronLikelihoodTool();

  private:
    class SafeTH1{
    public :
      SafeTH1(TH1F* hist);
      ~SafeTH1();

      int GetNbinsX();
      int FindBin(double);
      double GetBinContent(int);
      double GetBinLowEdge(int);
      double Integral();

   private: 
      std::vector<float> m_binContent;
      double m_firstBinLowEdge;
      double m_lastBinLowEdge;
      double m_binWidth;
      double m_integral;
      char m_string[500];
    };

    // Main methods
  public:
    /// Initialize this class
    int initialize();

    /// Finalize this class; everything that should be done after the event loop should go here
    int finalize() {return 1;}

    /// The main accept method: the actual cuts are applied here
    const Root::TAccept& accept(LikeEnumForward::LHAcceptVars_t& vars_struct) const;
    const Root::TAccept& accept( double likelihood,
					  double eta, 
					  double eT,
					  double ip ) const;
    const Root::TResult& calculate(LikeEnumForward::LHCalcVars_t& vars_struct) const ;
    const Root::TResult& calculate(  
					  double eta,
					  double eT,
					  double secondLambda, 
					  double lateral, 
					  double longitudinal, 
					  double centerLambda,
					  double fracMax,
					  double secondR,
                                          double significance,
                                          double secondDensity,
					  double ip,
int set = 4) const;
    

    /// Add an input file that holds the PDFs
    inline void setPDFFileName ( const std::string& val ) { PdfFileName = val; }

    /// Define the variable names
    inline void setVariableNames ( const std::string& val ) { 
      VariableNames = val; 
      m_variableBitMask = GetLikelihoodBitmask(val);
    }

    /// Load the variable histograms from the pdf file.
    int LoadVarHistograms(std::string vstr, unsigned int varIndex);

    /// Define the binning 
    inline void setBinning ( const std::string& val ) { m_ipBinning = val; }

     /// Set the prefix of the result name
    inline void setResultPrefix ( const std::string& val ) { m_resultPrefix = val; }
   
    /// The string for the result
    inline void setResultName ( const std::string& val ) { m_resultName = val; }

    unsigned int getBitmask(void) const { return m_variableBitMask;} 
    inline void setBitmask(unsigned int val) { m_variableBitMask = val; };

    // Private methods
  private:
   
    // For every input "varVector", make sure elements of vector are
    // in the same order as prescribed in fVariables

    /// Description???
    double EvaluateLikelihood(std::vector<double> varVector,double et,double eta,double ip=0,int set = 4) const;

    /// Description???
    double EvaluateLikelihood(std::vector<float>  varVector,double et,double eta,double ip=0,int set = 4) const;


    // To concoct a bitmask on your own, use the 
    // variable names prescribed in fVariables.
    
    /// Description???
    unsigned int GetLikelihoodBitmask(std::string vars) const;
    unsigned int LikelihoodTightBitmask() const {
      std::string vars = "el_secLambda,el_secondR,el_lateral,el_lonngitudinal,el_centralLamba,el_fracMax,el_significance,el_secondDensity";
      return GetLikelihoodBitmask(vars);
    };
    
    /// Description???
    unsigned int LikelihoodMediumBitmask() const{
      return LikelihoodTightBitmask();
    };

    /// Description???
    unsigned int LikelihoodLooseBitmask() const{
      std::string vars = "el_secLambda,el_secondR,el_lateral,el_lonngitudinal,el_centralLamba,el_fracMax,el_significance,el_secondDensity";
      return GetLikelihoodBitmask(vars);
    };
    
    double InterpolateCuts(const std::vector<double>& cuts,const std::vector<double>& cuts_4gev,double et,double eta) const;
    double InterpolatePdfs(unsigned int s_or_b,unsigned int ipbin,double et,double eta,int bin,unsigned int var) const;
    
  public:
     /** @brief range of eta bins for e-ID*/
	  std::vector<float> CutBinEta_ForwardElectron;
	  /** @brief cut on secondlambda*/
	  std::vector<float> CutSECONDLAMBDA_ForwardElectron;
	  /** @brief cut on lateral*/
	  std::vector<float> CutLATERAL_ForwardElectron;
	  /** @brief cut on longitudinal*/
	  std::vector<float> CutLONGITUDINAL_ForwardElectron;
	  /** @brief cut on maxFrac*/
	  std::vector<float> CutCELLMAXFRAC_ForwardElectron;
	  /** @brief cut values for cut on secondR */
	  std::vector<float> CutSECONDR_ForwardElectron;
	  /** @brief cut on centerlambda*/
	  std::vector<float> CutCENTERLAMBDA_ForwardElectron;
      std::vector<float> CutSECONDDENSITY_ForwardElectron;

    bool useHighETLHBinning;
    /** @brief use one extra bin for high ET LH*/
    bool useOneExtraHighETLHBin;
    /** @brief do pileup-dependent transform on discriminant value*/
   
    /** @brief do pileup-dependent transform on discriminant value*/
    bool doPileupTransform;
    /** @brief cut on likelihood output*/
        std::vector<double> CutLikelihood;
        std::vector<double> CutLikelihoodA;
    /** @brief pileup correction factor for cut on likelihood output*/
        std::vector<double> CutLikelihoodPileupCorrection;
    /** @brief pileup correction factor for cut on likelihood output, 4 GeV bin*/
	std::vector<double> CutLikelihoodPileupCorrectionA;
	std::vector<double> CutLikelihoodPileupCorrectionB;


    /** @brief variables to use in the LH*/
    std::string VariableNames;
    /** The operating point for the final cuts*/
    LikeEnumForward::Menu OperatingPoint;
    /** Name of the pdf file*/
    std::string PdfFileName;

    // Private methods
  private:
    /// Apply a transform to zoom into the LH output peaks. Optionally do pileup correction too
    double TransformLikelihoodOutput(double ps,double pb, double ip, double et, double eta) const;

    /// Eta binning for pdfs and discriminant cuts.
    unsigned int getLikelihoodEtaBin(double eta) const ;

    /// Coarse Et binning. Used for the likelihood pdfs.
    unsigned int getLikelihoodEtHistBin(double et)const ;

    // Private member variables
  private:

    /// The bitmask corresponding to the variables in the likelihood. For internal use.
    unsigned int        m_variableBitMask;

    /// Deprecated.
    std::string         m_ipBinning;

    /// Pointer to the opened TFile that holds the PDFs
    TFile*              m_pdfFile;

    /// The prefix string for the result
    std::string m_resultPrefix;

    /// The string for the result
    std::string m_resultName;

    /// The position of the kinematic cut bit in the TAccept return object
    int m_cutPosition_kinematic;
    /// The position of the likelihood cut bit in the TAccept return object
    int m_cutPosition_LH;
    /// The position of the likelihood value bit in the TResult return object
   
    int m_resultPosition_LH;
	static const double fIpBounds[IP_FBINS+1];
    static const unsigned int  fnEtBinsHist     = 6;  // number of hists stored for nominal LH (useHighETLHBinning), including 4GeV bin
    static const unsigned int  fnEtBinsHistOrig = 5;  // number of hists stored for original LH, including 4GeV bin (for backwards compatibility)
    static const unsigned int  fnDiscEtBinsOrig = 7;  // number of discs stored for original LH, excluding 4GeV bin (for backwards compatibility)
    static const unsigned int  fnEtaBins        = 11;
    static const unsigned int  fnVariables      = 8;
    TForwardElectronLikelihoodTool::SafeTH1*      fPDFbins     [2][IP_FBINS][fnEtBinsHist][fnEtaBins][fnVariables]; // [sig(0)/bkg(1)][ip][et][eta][variable]
    static const char*  fVariables                      [fnVariables]; 
    static const double Disc_Loose[40];
    static const double Disc_Medium[40];
    static const double Disc_Tight[40];
    static const double Disc_Medium_b[40];
    static const double Disc_Tight_b[40];
	    static const double Disc_Medium_a[40];
    static const double Disc_Tight_a[40];
	static const double Disc_Loose_a[40];
	static const double Disc_Loose_b[40];
    unsigned int getIpBin(double ip) const;
    void getBinName(char* buffer, int etbin,int etabin, int ipbin, std::string iptype) const;
  };

} // End: namespace Root

//----------------------------------------------------------------------------------------
#endif
