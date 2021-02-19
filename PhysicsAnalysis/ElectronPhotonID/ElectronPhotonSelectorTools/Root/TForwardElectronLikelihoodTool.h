/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//----------------------------------------------------------------------------------------

/** 

    Tool to apply the ID criteria for forward electrons

    The methodology is largely based on the likelihood of central electrons, so please refer to ElectronPhotonSelectorTools/TElectronLikelihoodTool.h for details

    The forward likelihood ID relies on the following list of topo-cluster variables:
    el_secondLambda : longitudinal second moment
    el_lateral      : normalised lateral moment
    el_longitudinal : normalised longitudinal moment
    el_centerLambda : shower depth
    el_fracMax      : fraction of cluster energy in the most energetic cell
    el_secondR      : transverse second moment
    el_significance : cluster energy divided by cluster noise
    el_secondDensity: second moment of signal density
    
    Major revision when migrating to rel 21 in August 2018
*/

#ifndef TFORWARDELECTRONLIKELIHOODTOOL_H
#define TFORWARDELECTRONLIKELIHOODTOOL_H

#include <string> // for string
#include <vector> // for vector

// ROOT includes
#include "TFile.h"
#include "TH1.h"

// Include the return objects and the base class
//#include "PATCore/TAccept.h"
//#include "PATCore/TResult.h"
#include "PATCore/AcceptData.h"
#include "PATCore/AcceptInfo.h"
//#include "PATCore/TCalculatorToolBase.h"
//#include "PATCore/TSelectorToolBase.h"
#include "AsgMessaging/AsgMessaging.h"
#include "AsgTools/IAsgTool.h"
#include "SafeTH1.h"
#include "memory"

namespace
{
  const unsigned int IP_FBINS = 1;
}

namespace LikeEnumForward
{
  enum Menu
  {
    Loose,
    Medium,
    Tight,
    CustomOperatingPoint
  };

  struct LHAcceptVars_t
  {
    double likelihood;
    double eta;
    double eT;
    double ip;
  };

  struct LHCalcVars_t
  {
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
  };

  struct ROOT6_NamespaceAutoloadHook
  {
  };
} // namespace LikeEnumForward

namespace Root
{
  class TForwardElectronLikelihoodTool : public asg::AsgMessaging
  {

  public:
    /// Standard constructor

    TForwardElectronLikelihoodTool(const char *name = "TForwardElectronLikelihoodTool");

    /// Standard destructor
    ~TForwardElectronLikelihoodTool();

  public:
    /// Initialize this class
    StatusCode initialize();

    /// accesss to the accept info object
    const asg::AcceptInfo &getAcceptInfo() const { return m_acceptInfo; }

    /// The main accept method: the actual cuts are applied here
    asg::AcceptData accept(LikeEnumForward::LHAcceptVars_t &vars_struct) const;
    asg::AcceptData accept(double likelihood,
                           double eta,
                           double eT,
                           double ip) const;

    /** Return dummy accept with only info */
    asg::AcceptData accept() const { return asg::AcceptData(&m_acceptInfo); }

    double calculate(LikeEnumForward::LHCalcVars_t &vars_struct) const;
    double calculate(
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
        double ip) const;

    /// Add an input file that holds the PDFs
    inline void setPDFFileName(const std::string &val) { m_pdfFileName = val; }

    /// Define the variable names
    inline void setVariableNames(const std::string &val)
    {
      m_variableNames = val;
      m_variableBitMask = getLikelihoodBitmask(val);
    }

    /// Load the variable histograms from the pdf file.
    int loadVarHistograms(const std::string &vstr, unsigned int varIndex);

    /// Define the binning
    inline void setBinning(const std::string &val) { m_ipBinning = val; }

    unsigned int getBitmask(void) const { return m_variableBitMask; }
    inline void setBitmask(unsigned int val) { m_variableBitMask = val; };

    // Private methods
  private:
    // For every input "varVector", make sure elements of vector are
    // in the same order as prescribed in fVariables
    // Internal methods to calculate the LH discriminant from a set of variables
    double evaluateLikelihood(std::vector<double> &varVector, double et, double eta, double ip = 0) const;
    double evaluateLikelihood(std::vector<float> &varVector, double et, double eta, double ip = 0) const;

    /// Mask out the variables ,out of all possible ones,
    /// that are not employed in the current configuration
    /// as read from the input config file
    unsigned int getLikelihoodBitmask(const std::string &vars) const;

    // PHILIP interpolation is unused
    /* double InterpolateCuts(const std::vector<double>& cuts,const std::vector<double>& cuts_4gev,double et,double eta) const; */
    /* double InterpolatePdfs(unsigned int s_or_b,unsigned int ipbin,double et,double eta,int bin,unsigned int var) const; */

    /// Apply a transform to zoom into the LH output peaks.
    //double TransformLikelihoodOutput(double ps, double pb) const;
    /// Eta binning for pdfs and discriminant cuts.
    //unsigned int getLikelihoodEtaBin(double eta) const;
    /// Et binning for for the likelihood pdfs and discriminants.
    //unsigned int getLikelihoodEtBin(double et) const;
    //Pile-up binning
    //unsigned int getIpBin(double ip) const;
    //get the bin names as is in the input file
    //void getBinName(char *buffer, int etbin, int etabin) const;

  public:
    /** @brief do pileup-dependent correction on discriminant value*/
    bool m_doPileupCorrection;
    /** @brief cut on likelihood output*/
    std::vector<double> m_cutLikelihood;
    /** the cut on the PU discriminant is adjusted as a function of nVtx
	cut + nVtx*cutA + cutB
	this is different from the procedure for central electrons
    */
    /** @brief pileup slope factor for cut on likelihood output*/
    std::vector<double> m_cutLikelihoodPileupCorrectionA;
    /** @brief pileup constant factor for cut on likelihood output*/
    std::vector<double> m_cutLikelihoodPileupCorrectionB;
    /** @brief variables to use in the LH*/
    std::string m_variableNames;
    /** Name of the pdf file*/
    std::string m_pdfFileName;

    // Private methods
  private:
    /// Apply a transform to zoom into the LH output peaks. Optionally do pileup
    /// correction too
    double TransformLikelihoodOutput(double ps,
                                     double pb) const;

    /// Eta binning for pdfs and discriminant cuts.
    unsigned int getLikelihoodEtaBin(double eta) const;

    /// Coarse Et binning. Used for the likelihood and discriminant pdfs.
    unsigned int getLikelihoodEtHistBin(double eT) const;

    // Private member variables
  private:
    /// The bitmask corresponding to the variables in the likelihood. For internal use.
    unsigned int m_variableBitMask;

    /// Accept info
    asg::AcceptInfo m_acceptInfo;

    /// Deprecated.
    std::string m_ipBinning;

    /// Pointer to the opened TFile that holds the PDFs
    TFile *m_pdfFile;

    /// The position of the kinematic cuts bit in the AcceptInfo return object, separate for eta/Et
    int m_cutPosition_kinematicEta;
    int m_cutPosition_kinematicEt;
    /// The position of the likelihood cut bit in the AcceptInfo return object
    int m_cutPosition_LH;

    static const double fIpBounds[IP_FBINS + 1];
    static const unsigned int s_fnEtBinsHist = 4; // number of hists stored for nominal LH
    static const unsigned int s_fnDiscEtBins = 4; // number of discs stored for original LH
    static const unsigned int s_fnEtaBins = 10;
    static const unsigned int s_fnVariables = 8;
    static const std::string fVariables[s_fnVariables];
    // 5D array of ptr to SafeTH1  // [sig(0)/bkg(1)][ip][et][eta][variable]
    EGSelectors::SafeTH1 *fPDFbins[2][IP_FBINS][s_fnEtBinsHist][s_fnEtaBins][s_fnVariables];

    unsigned int getIpBin(double ip) const;
    void getBinName(char *buffer,
                    int etbin,
                    int etabin,
                    int ipbin,
                    const std::string &iptype) const;
  };

} // namespace Root

#endif
