/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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
   nNextToInnerMostLayer 		: next to the inner most 
   nNextToInnerMostLayerOutliers 	: next to the inner most 
   expectNextToInnerMostLayer 	: next to the inner most 
   convBit 		: el_isEM & (0x1 << egammaPID::ConversionMatch_Electron)
   ambiguityBit 	: cut on the ambiguity type
   ip 		: Count number of vertices in vxp_n with >= 2 tracks in vxp_trk_n

   Created:
   June 2011

*/
//-------------------------------------------------------------------------------------------------------


#ifndef TELECTRONLIKELIHOODTOOL_H
#define TELECTRONLIKELIHOODTOOL_H

#include <fstream>
#include <iostream>
// Include the return objects and the base class
#include "PATCore/TCalculatorToolBase.h"
#include "PATCore/TSelectorToolBase.h"
#include "AsgTools/AsgMessaging.h"

#include <string>                       // for string
#include <vector>                       // for vector
class TFile;
class TH1F;
namespace Root { class TAccept; }
namespace Root { class TResult; }
namespace{
  const unsigned int  IP_BINS=1;
}
namespace LikeEnum {

  struct LHAcceptVars_t{
    double likelihood;
    double eta;
    double eT;
    int nSiHitsPlusDeadSensors;
    int nPixHitsPlusDeadSensors;
    bool passBLayerRequirement;
    int convBit;
    uint8_t ambiguityBit;
    double d0;
    double deltaEta;
    double deltaphires;
    double wstot;
    double EoverP;
    double ip;         
  };

  struct LHCalcVars_t{
    double eta;
    double eT;
    double f3; 
    double rHad; 
    double rHad1;
    double Reta;
    double w2; 
    double f1; 
    double eratio;
    double deltaEta; 
    double d0; 
    double d0sigma;
    double rphi; 
    double deltaPoverP;
    double deltaphires;
    double TRT_PID;
    double ip;        
  };
}

namespace Root {
  class TElectronLikelihoodTool : public TCalculatorToolBase, public TSelectorToolBase, public asg::AsgMessaging
  {

  public: 
    /// Standard constructor

    TElectronLikelihoodTool(const char* name = "TElectronLikelihoodTool");
    
    /// Standard destructor
    ~TElectronLikelihoodTool();

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
    };

    // Main methods
  public:
    /// Initialize this class
    int initialize();

    /// Finalize this class; everything that should be done after the event loop should go here
    int finalize() {return 1;}

    /// The main accept method: the actual cuts are applied here
    const Root::TAccept& accept(LikeEnum::LHAcceptVars_t& vars_struct) const;
    const Root::TAccept& accept( double likelihood,
                                 double eta, double eT,
                                 int nSiHitsPlusDeadSensors, int nPixHitsPlusDeadSensors,
                                 bool passBLayerRequirement,
                                 int convBit, uint8_t ambiguityBit, double d0, double deltaEta, double deltaphires, 
                                 double wstot, double EoverP, double ip ) const;
    const Root::TResult& calculate(LikeEnum::LHCalcVars_t& vars_struct) const ;
    const Root::TResult& calculate( double eta, double eT,double f3, double rHad, double rHad1,
                                    double Reta, double w2, double f1, double eratio,
                                    double deltaEta, double d0, double d0sigma, double rphi,
                                    double deltaPoverP ,double deltaphires, double TRT_PID,
                                    double ip) const;
    

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
    double EvaluateLikelihood(std::vector<double> varVector,double et,double eta,double ip=0) const;

    /// Description???
    double EvaluateLikelihood(std::vector<float>  varVector,double et,double eta,double ip=0) const;


    // To concoct a bitmask on your own, use the 
    // variable names prescribed in fVariables.
    
    /// Description???
    unsigned int GetLikelihoodBitmask(std::string vars) const;
    
    double InterpolateCuts(const std::vector<double>& cuts,const std::vector<double>& cuts_4gev,double et,double eta) const;
    double InterpolatePdfs(unsigned int s_or_b,unsigned int ipbin,double et,double eta,int bin,unsigned int var) const;
    
  public:
    /** @brief cut min on b-layer hits*/
    std::vector<int> CutBL;
    /** @brief cut min on pixel hits*/
    std::vector<int> CutPi;
    /** @brief cut min on precision hits*/
    std::vector<int> CutSi;
    /** @brief cut max on track d0 bit*/
    std::vector<double> CutA0;
    /** @brief do cut on delta eta bit*/
    std::vector<double> CutDeltaEta;
    // /** @brief do cut on delta phi bit*/
    std::vector<double> CutDeltaPhiRes;
    /** @brief do cut on conversion bit*/
    bool doCutConversion;
    /** @brief do cut on ambiguity bit*/
    std::vector<int> CutAmbiguity;
    /** @brief do remove f3 variable from likelihood at high Et (>80 GeV)*/
    bool doRemoveF3AtHighEt;
    /** @brief do remove TRTPID variable from likelihood at high Et (>80 GeV)*/
    bool doRemoveTRTPIDAtHighEt;
    /** @brief do smooth interpolation between bins */
    bool doSmoothBinInterpolation;
    /** @brief use binning for high ET LH*/
    bool useHighETLHBinning;
    /** @brief use one extra bin for high ET LH*/
    bool useOneExtraHighETLHBin;
    /** @brief ET threshold for using high ET cuts and bin */
    double HighETBinThreshold;
    // /** @brief do cut on wstot above HighETBinThreshold bit*/
    std::vector<double> CutWstotAtHighET;
    // /** @brief do cut on EoverP above HighETBinThreshold bit*/
    std::vector<double> CutEoverPAtHighET;
    /** @brief do pileup-dependent transform on discriminant value*/
    bool doPileupTransform;
    /** @brief do centrality-dependent transform on discriminant value*/
    bool doCentralityTransform;
    /** @brief cut on likelihood output*/
    std::vector<double> CutLikelihood;
    /** @brief pileup correction factor for cut on likelihood output*/
    std::vector<double> CutLikelihoodPileupCorrection;
    /** @brief cut on likelihood output, 4 GeV bin*/
    std::vector<double> CutLikelihood4GeV;
    /** @brief pileup correction factor for cut on likelihood output, 4 GeV bin*/
    std::vector<double> CutLikelihoodPileupCorrection4GeV;
    /** @brief reference disc for very hard cut; used by pileup transform */
    std::vector<double> DiscHardCutForPileupTransform;
    /** @brief reference slope on disc for very hard cut; used by pileup transform */
    std::vector<double> DiscHardCutSlopeForPileupTransform;
    /** @brief reference quadratic apr on disc for very hard cut; used by centrality transform */
    std::vector<double> DiscHardCutQuadForPileupTransform;
    /** @brief reference disc for a pileup independent loose menu; used by pileup transform */
    std::vector<double> DiscLooseForPileupTransform;
    /** @brief reference disc for very hard cut; used by pileup transform - 4-7 GeV */
    std::vector<double> DiscHardCutForPileupTransform4GeV;
    /** @brief reference slope on disc for very hard cut; used by pileup transform - 4-7 GeV */
    std::vector<double> DiscHardCutSlopeForPileupTransform4GeV;
    /** @brief reference quadratic par on disc for very hard cut; used by centrality transform - 4-7 GeV */
    std::vector<double> DiscHardCutQuadForPileupTransform4GeV;
    /** @brief reference disc for a pileup independent loose menu; used by pileup transform - 4-7 GeV */
    std::vector<double> DiscLooseForPileupTransform4GeV;
    /** @brief max discriminant for which pileup transform is to be used */
    double DiscMaxForPileupTransform;
    /** @brief max nvtx or mu to be used in pileup transform  */
    double PileupMaxForPileupTransform;
    /** @brief variables to use in the LH*/
    std::string VariableNames;
    /** Name of the pdf file*/
    std::string PdfFileName;


    // Private methods
  private:
    /// Apply a transform to zoom into the LH output peaks. Optionally do pileup correction too
    double TransformLikelihoodOutput(double ps,double pb, double ip, double et, double eta) const;

    /// Eta binning for pdfs and discriminant cuts.
    unsigned int getLikelihoodEtaBin(double eta) const ;

    /// Coarse Et binning. Used for the likelihood pdfs.
    unsigned int getLikelihoodEtHistBin(double eT)const ;
    
    /// Fine Et binning. Used for the likelihood discriminant cuts.
    unsigned int getLikelihoodEtDiscBin(double eT , const bool isLHbinning) const;


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

    /// The position of the NSilicon cut bit in the TAccept return object
    int m_cutPosition_NSilicon;

    /// The position of the NPixel cut bit in the TAccept return object
    int m_cutPosition_NPixel;

    /// The position of the NBlayer cut bit in the TAccept return object
    int m_cutPosition_NBlayer;

    /// The position of the conversion cut bit in the TAccept return object
    int m_cutPosition_conversion;

    /// The position of the ambiguity cut bit in the TAccept return object
    int m_cutPosition_ambiguity;

    /// The position of the likelihood cut bit in the TAccept return object
    int m_cutPosition_LH;

    /// The position of the d0 cut bit in the TAccept return object
    int m_cutPositionTrackA0;

    /// The position of the deltaeta cut bit in the TAccept return object
    int m_cutPositionTrackMatchEta;

    // /// The position of the deltaphi cut bit in the TAccept return object
    int m_cutPositionTrackMatchPhiRes;

    // /// The position of the high ET wstot cut bit in the TAccept return object
    int m_cutPositionWstotAtHighET;

    // /// The position of the high ET EoverP cut bit in the TAccept return object
    int m_cutPositionEoverPAtHighET;

    /// The position of the likelihood value bit in the TResult return object
    int m_resultPosition_LH;

    static const double fIpBounds[IP_BINS+1];
    static const unsigned int  fnEtBinsHist     = 8;  // number of hists stored for LH with many high ET bins (useHighETLHBinning), including 4GeV bin
    static const unsigned int  fnDiscEtBins     = 33; // number of discs stored for LH with many high ET bins (useHighETLHBinning), excluding 4GeV bin
    static const unsigned int  fnEtBinsHistOrig = 7;  // number of hists stored for original LH, including 4GeV bin (for backwards compatibility)
    static const unsigned int  fnDiscEtBinsOrig = 9;  // number of discs stored for original LH, excluding 4GeV bin (for backwards compatibility)
    static const unsigned int  fnDiscEtBinsOneExtra = 10; // number of discs stored for original LH plus one for HighETBinThreshold (useOneExtraHighETLHBin), excluding 4GeV bin
    static const unsigned int  fnEtaBins        = 10;
    static const unsigned int  fnVariables      = 13;
    TElectronLikelihoodTool::SafeTH1*      fPDFbins     [2][IP_BINS][fnEtBinsHist][fnEtaBins][fnVariables]; // [sig(0)/bkg(1)][ip][et][eta][variable]
    static const std::string  fVariables                [fnVariables];

    unsigned int getIpBin(double ip) const;
    void getBinName(char* buffer, int etbin,int etabin, int ipbin, std::string iptype) const;
  };

} // End: namespace Root

//----------------------------------------------------------------------------------------
#endif
