/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TELECTRONLIKELIHOODTOOL_H
#define TELECTRONLIKELIHOODTOOL_H


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



//#define IP_BINS 2
//#define IP_BINS 1

namespace{

const unsigned int  IP_BINS=1;
}

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


   Additional output will be givedn if debug is set, (false by default).

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

namespace LikeEnum {
  enum Menu {
    VeryLoose,
    Loose,
    Medium,
    Tight,
    VeryTight,
    LooseRelaxed,
    CustomOperatingPoint
  };

  struct LHAcceptVars_t{
    double likelihood;
    double eta;
    double eT;
    int nSi;
    int nSiDeadSensors;
    int nPix;
    int nPixDeadSensors;
    int nBlayer;
    int nBlayerOutliers;
    bool expectBlayer;
    int convBit;
    double d0;
    double deltaEta;
    double deltaphires;
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
    double TRratio;
    double d0sigma;
    //double ptcone20; 
    double rphi; 
    //double ws3;
    double deltaPoverP;
    double deltaphires;
    double TRT_PID;
    double ip;        
  };

   struct ROOT6_NamespaceAutoloadHook{};
}

//This is an *ugly* solution due to what appears to be Root dict issue for interactive use

namespace Root {

  class ITElectronLikelihoodTool :   public TCalculatorToolBase, public TSelectorToolBase{

  public :
    ITElectronLikelihoodTool(const char* name="ITElectronLikelihoodTool") : 
      TCalculatorToolBase(name),
      TSelectorToolBase(name){}
      
    virtual int initialize()=0;

    /// Finalize this class; everything that should be done after the event loop should go here
    virtual int finalize() =0;
  };
}

namespace Root {
  class TElectronLikelihoodTool : public ITElectronLikelihoodTool 
  {

  public: 
    /// Standard constructor

    TElectronLikelihoodTool(const char* name = "TElectronLikelihoodTool");
    
    /// Standard destructor
    ~TElectronLikelihoodTool();

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
                                 int nSi,int nSiDeadSensors, int nPix, int nPixDeadSensors,
                                 int nBlayer, int nBlayerOutliers, bool expectBlayer,
                                 int convBit, double d0, double deltaEta, double deltaphires, double ip ) const;
    const Root::TResult& calculate(LikeEnum::LHCalcVars_t& vars_struct) const ;
    const Root::TResult& calculate( double eta, double eT,double f3, double rHad, double rHad1,
                                    double Reta, double w2, double f1, /*double wstot,*/ double eratio,
                                    double deltaEta, double d0, double TRratio, /*double eOverP,*/
                                    /*double deltaPhi,*/ double d0sigma, /*double fside,*/
                                    /*double ptcone20,*/ double rphi, /*double ws3,*/
                                    double deltaPoverP ,double deltaphires,
				    double TRT_PID,
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

    /// Set the cut value to be used
    inline void setOperatingPoint( const LikeEnum::Menu op_point ){ 
      OperatingPoint = op_point; 
      // Tight cut requirements
      if (OperatingPoint == LikeEnum::VeryTight){
	m_variableBitMask = LikelihoodTightBitmask();
	CutBL.assign(TightCutBL,TightCutBL+sizeof(TightCutBL)/sizeof(int));
	CutPi.assign(TightCutPi,TightCutPi+sizeof(TightCutPi)/sizeof(int));
	CutSi.assign(TightCutSi,TightCutSi+sizeof(TightCutSi)/sizeof(int));
	doCutConversion = true;
	doRemoveF3AtHighEt = false;
	doPileupTransform = false;
	CutLikelihood.assign(Disc_VeryTight,Disc_VeryTight+sizeof(Disc_VeryTight)/sizeof(double));
	CutLikelihoodPileupCorrection.assign(Disc_VeryTight_b,Disc_VeryTight_b+sizeof(Disc_VeryTight_b)/sizeof(double));
      }
      else if (OperatingPoint == LikeEnum::Tight){
	m_variableBitMask = LikelihoodTightBitmask();
	CutBL.assign(TightCutBL,TightCutBL+sizeof(TightCutBL)/sizeof(int));
	CutPi.assign(TightCutPi,TightCutPi+sizeof(TightCutPi)/sizeof(int));
	CutSi.assign(TightCutSi,TightCutSi+sizeof(TightCutSi)/sizeof(int));
	doCutConversion = true;
	doRemoveF3AtHighEt = false;
	doPileupTransform = false;
	CutLikelihood.assign(Disc_Tight,Disc_Tight+sizeof(Disc_Tight)/sizeof(double));
	CutLikelihoodPileupCorrection.assign(Disc_Tight_b,Disc_Tight_b+sizeof(Disc_Tight_b)/sizeof(double));
      }	
      // Medium cut requirements
      else if (OperatingPoint == LikeEnum::Medium){
	m_variableBitMask = LikelihoodMediumBitmask();
	CutBL.assign(TightCutBL,TightCutBL+sizeof(TightCutBL)/sizeof(int));
	CutPi.assign(TightCutPi,TightCutPi+sizeof(TightCutPi)/sizeof(int));
	CutSi.assign(TightCutSi,TightCutSi+sizeof(TightCutSi)/sizeof(int));
	doCutConversion = false;
	doRemoveF3AtHighEt = false;
	doPileupTransform = false;
	CutLikelihood.assign(Disc_Medium,Disc_Medium+sizeof(Disc_Medium)/sizeof(double));
	CutLikelihoodPileupCorrection.assign(Disc_Medium_b,Disc_Medium_b+sizeof(Disc_Medium_b)/sizeof(double));
      }
      // Loose / Loose Relaxed / Very Loose
      else if (OperatingPoint == LikeEnum::Loose){
	m_variableBitMask = LikelihoodLooseBitmask();
	CutBL.assign(TightCutBL,TightCutBL+sizeof(TightCutBL)/sizeof(int));
	CutPi.assign(TightCutPi,TightCutPi+sizeof(TightCutPi)/sizeof(int));
	CutSi.assign(TightCutSi,TightCutSi+sizeof(TightCutSi)/sizeof(int));
	doCutConversion = false;
	doRemoveF3AtHighEt = false;
	doPileupTransform = false;
	CutLikelihood.assign(Disc_Loose,Disc_Loose+sizeof(Disc_Loose)/sizeof(double));
	CutLikelihoodPileupCorrection.clear();
      }
      else if (OperatingPoint == LikeEnum::LooseRelaxed){
	m_variableBitMask = LikelihoodLooseBitmask();
	CutBL.assign(TightCutBL,TightCutBL+sizeof(TightCutBL)/sizeof(int));
	CutPi.assign(TightCutPi,TightCutPi+sizeof(TightCutPi)/sizeof(int));
	CutSi.assign(TightCutSi,TightCutSi+sizeof(TightCutSi)/sizeof(int));
	doCutConversion = false;
	doRemoveF3AtHighEt = false;
	doPileupTransform = false;
	CutLikelihood.assign(Disc_LooseRelaxed,Disc_LooseRelaxed+sizeof(Disc_LooseRelaxed)/sizeof(double));
	CutLikelihoodPileupCorrection.clear();
      }
      else if (OperatingPoint == LikeEnum::VeryLoose){
	m_variableBitMask = LikelihoodLooseBitmask();
	CutBL.clear();
	CutPi.assign(VeryLooseCutPi,VeryLooseCutPi+sizeof(VeryLooseCutPi)/sizeof(int));
	CutSi.assign(TightCutSi,TightCutSi+sizeof(TightCutSi)/sizeof(int));
	doCutConversion = false;
	doRemoveF3AtHighEt = false;
	doPileupTransform = false;
	CutLikelihood.assign(Disc_VeryLoose,Disc_VeryLoose+sizeof(Disc_VeryLoose)/sizeof(double));
	CutLikelihoodPileupCorrection.clear();
      }
      // Custom operating point (user-defined stuff)
      else if (OperatingPoint == LikeEnum::CustomOperatingPoint) {
	m_variableBitMask = GetLikelihoodBitmask(VariableNames,m_debug);
      }
    }

    /// Set the prefix of the result name
    inline void setResultPrefix ( const std::string& val ) { m_resultPrefix = val; }
   
    /// The string for the result
    inline void setResultName ( const std::string& val ) { m_resultName = val; }


    /// Set printout to debug level
    inline void setDebug( bool val=true ){ m_debug = val; };
    

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
    unsigned int GetLikelihoodBitmask(std::string vars,bool debug=false) const;
    unsigned int LikelihoodTightBitmask(bool debug=false) const {
      std::string vars = "el_deltaeta1,el_weta2,el_TRTHighTOutliersRatio,el_f1,el_f3,el_eratio,el_rphi,el_rhad,el_reta,el_DeltaPoverP,el_deltaphiRescaled,el_trackd0pvunbiased,el_d0significance";
      return GetLikelihoodBitmask(vars,debug);
    };
    
    /// Description???
    unsigned int LikelihoodMediumBitmask(bool debug=false) const{
      return LikelihoodTightBitmask(debug);
    };

    /// Description???
    unsigned int LikelihoodLooseBitmask(bool debug=false) const{
      std::string vars = "el_rhad,el_reta,el_deltaeta1,el_weta2,el_TRTHighTOutliersRatio,el_f1,el_f3,el_eratio,el_rphi,el_DeltaPoverP,el_deltaphiRescaled";
      return GetLikelihoodBitmask(vars,debug);
    };
    
    
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
    // /** @brief do cut on delta phi (not res) bit*/
    std::vector<double> CutDeltaPhiRes;
    /** @brief do cut on conversion bit*/
    bool doCutConversion;
    /** @brief do remove f3 variable from likelihood at high Et (>100 GeV)*/
    bool doRemoveF3AtHighEt;
    /** @brief do pileup-dependent transform on discriminant value*/
    bool doPileupTransform;
    /** @brief cut on likelihood output*/
    std::vector<double> CutLikelihood;
    /** @brief pileup correction factor for cut on likelihood output*/
    std::vector<double> CutLikelihoodPileupCorrection;
    /** @brief reference disc for very hard cut; used by pileup transform */
    std::vector<double> DiscHardCutForPileupTransform;
    /** @brief reference slope on disc for very hard cut; used by pileup transform */
    std::vector<double> DiscHardCutSlopeForPileupTransform;
    /** @brief reference disc for a pileup independent loose menu; used by pileup transform */
    std::vector<double> DiscLooseForPileupTransform;
    /** @brief max discriminant for which pileup transform is to be used */
    double DiscMaxForPileupTransform;
    /** @brief max nvtx or mu to be used in pileup transform  */
    double PileupMaxForPileupTransform;
    /** @brief variables to use in the LH*/
    std::string VariableNames;
    /** The operating point for the final cuts*/
    LikeEnum::Menu OperatingPoint;
    /** Name of the pdf file*/
    std::string PdfFileName;




    // Private methods
  private:
    /// Apply a transform to zoom into the LH output peaks. Optionally do pileup correction too
    double TransformLikelihoodOutput(double ps,double pb, double ip, unsigned int ibin_combined) const;

    /// Eta binning for pdfs and discriminant cuts.
    unsigned int getLikelihoodEtaBin(double eta) const ;

    /// Coarse Et binning. Used for the likelihood pdfs.
    unsigned int getLikelihoodEtBin(double eT)const ;
    
    /// Fine Et binning. Used for the likelihood discriminant cuts.
    unsigned int getLikelihoodEtFineBin(double eT) const;


    // Private member variables
  private:
    /// Option to print out debug statements
    bool                m_debug;

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

    /// The position of the likelihood cut bit in the TAccept return object
    int m_cutPosition_LH;

    /// The position of the d0 cut bit in the TAccept return object
    int m_cutPositionTrackA0;

    /// The position of the deltaeta cut bit in the TAccept return object
    int m_cutPositionTrackMatchEta;

    // /// The position of the deltaphi cut bit in the TAccept return object
    int m_cutPositionTrackMatchPhiRes;

    /// The position of the likelihood value bit in the TResult return object
    int m_resultPosition_LH;



    //static const unsigned int  fnIpBins =2;
    static const double fIpBounds[IP_BINS+1];
    static const unsigned int  fnEtBins         = 6;
    static const unsigned int  fnFineEtBins     = 9;
    static const unsigned int  fnEtaBins        = 10;
    static const unsigned int  fnVariables      = 14; // 19
    TH1F*               fPDFbins     [2][IP_BINS][6][10][14]; // [sig(0)/bkg(1)][ip][et][eta][variable]
    double              fPDFIntegrals[2][IP_BINS][6][10][14]; // [sig(0)/bkg(1)][ip][et][eta][variable]
    static const char*  fVariables                      [14]; // 
    //static const double cutDiscriminant [1][6][10];     // [ip][et][eta]

    static const double Disc_VeryLoose[90];
    static const double Disc_LooseRelaxed[90];
    static const double Disc_Loose[90];
    static const double Disc_Medium[90];
    static const double Disc_Tight[90];
    static const double Disc_VeryTight[90];

    static const double Disc_Medium_b[90];
    static const double Disc_Tight_b[90];
    static const double Disc_VeryTight_b[90];

    static const int TightCutBL[10];
    static const int TightCutPi[10];
    static const int VeryLooseCutPi[10];
    static const int TightCutSi[10];

    unsigned int getIpBin(double ip) const;
    void getBinName(char* buffer, int etbin,int etabin, int ipbin, std::string iptype) const;
  };


} // End: namespace Root

//----------------------------------------------------------------------------------------
#endif
