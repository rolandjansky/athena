// Dear emacs, this is -*- c++ -*-
///////////////////////////////////////////////////////////////////
// BTaggingTruthTaggingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
/**
  @class BTaggingTruthTaggingTool
  Tool to apply flavour-tagging requirements on jets
  @author C. Rizzi, M. Ughetto
  @contact chiara.rizzi@cern.ch, mughetto@cern.ch
**/

#ifndef CPBTAGGINGTRUTHTAGGINGTOOL_H
#define CPBTAGGINGTRUTHTAGGINGTOOL_H

#include "FTagAnalysisInterfaces/IBTaggingTruthTaggingTool.h"
#include "xAODBTagging/BTagging.h"

#include "AsgTools/AsgTool.h"
#include <AsgTools/MessageCheck.h>

#include "TFile.h"
#include "TRandom3.h"
#include <string>
#include <vector>
#include <map>

// include xAODBtaggingEfficiency classes
#include "FTagAnalysisInterfaces/IBTaggingEfficiencyTool.h"
#include "FTagAnalysisInterfaces/IBTaggingSelectionTool.h"
#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"
#include <AsgTools/AnaToolHandle.h>

// calibration data variable
#include "CalibrationDataInterface/CalibrationDataVariables.h"

// xAOD jet
#include "xAODJet/JetContainer.h"

class BTaggingTruthTaggingTool: public asg::AsgTool,
				public virtual IBTaggingTruthTaggingTool {
  typedef double (xAOD::BTagging::* tagWeight_member_t)() const;

  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( BTaggingTruthTaggingTool , IBTaggingTruthTaggingTool)

  public:
  /// Create a constructor for standalone usage
  BTaggingTruthTaggingTool( const std::string& name );


  virtual  ~BTaggingTruthTaggingTool();

  // initialization of the tool
  StatusCode initialize();

  // set the seed of the raddom generator, needed only if choosing permutation or quantile
  StatusCode setSeed(unsigned int seed) ;

  // set the jets in the event (pass same jets that satisfy kinematic criteria for b-tagging in pT and eta)
  StatusCode setJets(std::vector<double>* pt, std::vector<double>* eta, std::vector<int>* flav, std::vector<double>* tagw) ;
  StatusCode setJets(const xAOD::JetContainer*& jets) ;
  StatusCode setJets(std::vector<int>* flav, std::vector<Analysis::CalibrationDataVariables>* vars) ;

  // get truth tagging weights
  // for one single systematic (including "Nominal")
  StatusCode getTruthTagWei(unsigned int limit, std::vector<double> &trf_weight_ex, std::vector<double> &trf_weight_in, int sys=0);
  // for all of the systematics
  StatusCode getTruthTagWei(unsigned int limit, std::vector<std::vector<double> > &trf_weight_ex, std::vector<std::vector<double> > &trf_weight_in);
  StatusCode getTruthTagWei(unsigned int limit, std::map<std::string,std::vector<double> > &trf_weight_ex, std::map<std::string,std::vector<double> > &trf_weight_in);

  // tag permutation: trf_chosen_perm_ex.at(ntag).at(i) tells if the i-th jet is tagged in a selection requiring == ntag tags
  StatusCode getTagPermutation(unsigned int nbtag, std::vector<std::vector<bool> > &trf_chosen_perm_ex, std::vector<std::vector<bool> > &trf_chosen_perm_in);

  // chosen quantile: trf_bin_ex.at(ntag).at(i) tells the quantile in which the i-th jet falls in a selection requiring == ntag tags
  // returns 5 if between 60% and 0%
  // returns 4 if between 70% and 60%
  // returns 3 if between 77% and 70%
  // returns 2 if between 85% and 77%
  // returns 1 if between 100% and 85%
  // returns 0 if smaller than -1e4-> should never happen --> not currently implemented
  // return -1 if bigger than 1e4 or not in b-tagging acceptance --> not currently implemented
  StatusCode getQuantiles(std::vector<std::vector<int> > &trf_bin_ex, std::vector<std::vector<int> > &trf_bin_in);

  // functions to make comparison with direct-tagging easier
  double getEvtSF(int syst=0) ;
  StatusCode getDirectTaggedJets(std::vector<bool> &is_tagged);

  std::vector<std::string> affectingSystematics(){return m_sys_name;};

  StatusCode setEffMapIndex(const std::string& flavour, unsigned int index);

private:

  std::vector<std::string> m_availableOP_fixCut= {"FixedCutBEff_85", "FixedCutBEff_77","FixedCutBEff_70","FixedCutBEff_60"};
  std::vector<std::string> m_availableOP_fixEff= {"FlatBEff_85", "FlatBEff_77", "FlatBEff_70", "FlatBEff_60"};
  std::vector<std::string> m_availableOP_HybEff= {"HybBEff_85", "HybBEff_77", "HybBEff_70", "HybBEff_60"};
  std::vector<std::string> m_availableOP;

  /// Helper function that decides whether a jet belongs to the correct jet selection for b-tagging
  //   bool checkRange( double /* jet pt */, double /* jet eta */ ) ;

  bool m_initialised;

  StatusCode getTRFweight(unsigned int nbtag, bool isInclusive, int sys);
  StatusCode getAllEffMC();
  StatusCode getAllEffSF(int =0);
  std::vector<CP::SystematicSet> m_eff_syst;
  std::vector<std::string> m_sys_name;

  // flav labelling
  int jetFlavourLabel (const xAOD::Jet& jet);

  int GAFinalHadronFlavourLabel(const xAOD::Jet& jet);
  int ConeFinalPartonFlavourLabel (const xAOD::Jet& jet) ;
  int ExclusiveConeHadronFlavourLabel (const xAOD::Jet& jet) ;

  //*********************************//
  // Prop. of BTaggingEfficiencyTool //
  //*********************************//

  /// name of the data/MC efficiency scale factor calibration file (may be changed by the @c PathResolver)
  std::string m_SFFile;
  /// name of the optional MC efficiency file (may be changed by the @c PathResolver)
  std::string m_EffFile;
  /// name of the data/MC scale factor calibration for b jets
  std::string m_SFBName;
  /// name of the data/MC scale factor calibration for charm jets
  std::string m_SFCName;
  /// name of the data/MC scale factor calibration for tau jets
  std::string m_SFTName;
  /// name of the data/MC scale factor calibration for light-flavour jets
  std::string m_SFLightName;
  /// specification of the eigenvector reduction strategy for b jets (if eigenvectors are used)
  std::string m_EVReductionB;
  /// specification of the eigenvector reduction strategy for c jets (if eigenvectors are used)
  std::string m_EVReductionC;
  /// specification of the eigenvector reduction strategy for light-flavour jets (if eigenvectors are used)
  std::string m_EVReductionLight;
  /// semicolon-separated list of MC efficiency parametrisation names for b jets
  std::string m_EffBName;
  /// semicolon-separated list of MC efficiency parametrisation names for charm jets
  std::string m_EffCName;
  /// semicolon-separated list of MC efficiency parametrisation names for tau jets
  std::string m_EffTName;
  /// semicolon-separated list of MC efficiency parametrisation names for light-flavour jets
  std::string m_EffLightName;
  /// semicolon-separated list of uncertainties to be excluded from the eigenvector variation procedure
  std::string m_excludeFromEV;
  /// tagger name
  std::string m_taggerName;
  /// operating point
  std::string m_OP;
  ///  jet collection name
  std::string m_jetAuthor;
  /// systematics model to be used (current choices are "SFEigen" and "Envelope")
  std::string m_systStrategy;
  /// if true, attempt to retrieve the data/MC efficiency scale factor calibration files from the @PathResolver development area
  bool m_useDevFile;
  /// if true, use cone-based labelling (as opposed to ghost association)
  bool m_coneFlavourLabel;
  /// when using cone-based labelling (see above), if true, use the "traditional" (parton-based) labelling instead of the current default (hadron-based, exclusive)
  bool m_oldConeFlavourLabel;
  // bool m_excludeJESFromEV; // commented out in the trunk of BTaggingEfficiencyTool


  //*********************************//
  // Prop. of BTaggingSelectionTool  //
  //*********************************//

  double m_maxEta;
  double m_minPt;
  double m_maxRangePt;
  //  std::string m_CutFileName;

  // properties of truth tagging
  bool m_ignoreSF;
  bool m_usePerm;
  bool m_useQuntile;

  unsigned int m_OperatingPoint_index;

  // xAODBTaggingEfficiency classes
  // BTaggingEfficiencyTool* m_effTool;
  //std::map<std::string,BTaggingEfficiencyTool*> m_effTool_allOP; // tagbin
  // BTaggingSelectionTool* m_selTool;

  std::map<std::string, asg::AnaToolHandle<IBTaggingEfficiencyTool> > m_effTool_allOP;

  asg::AnaToolHandle<IBTaggingEfficiencyTool> m_effTool; //!
  asg::AnaToolHandle<IBTaggingSelectionTool> m_selTool; //!

  //  other members
  unsigned int m_njets;
  TRandom3 m_rand;

  // functions
  StatusCode check_syst_range(unsigned int sys);

  std::vector<std::vector<bool> > generatePermutations(int njets, int tags, int start=0);
  // Permutations: njets, ntags, permutations
  std::map<int,std::vector<std::vector<std::vector<bool> > > > m_perms; // int: N jets, i tagged. All the possible permutations of N jets with i tags. m_perms[njets].at(i) --> vector of vector of bools with the position of the i tgged jets
  std::vector<std::vector<double> > m_permsWeight; // First vector = N b-tags required, second vector = vector of permutation -> 1 trf weight per permutation
  std::vector<std::vector<double> > m_permsSumWeight; // First vector = N b-tags required, second vector = vector of permutation -> 1 trf sum weight per permutation
  double trfWeight(const std::vector<bool> &tags);

  StatusCode chooseAllTagPermutation(unsigned int nbtag);
  StatusCode chooseTagPermutation(unsigned int nbtag, bool isIncl);
  double getPermutationRW(bool isIncl, unsigned int nbtag, int sys);

  StatusCode chooseAllTagBins();
  StatusCode chooseTagBins_cum(std::vector<bool> &tagconf, bool isIncl, unsigned int nbtag);
  double getTagBinsConfProb(std::vector<int> &tagws);
  double getTagBinsRW(bool isIncl, unsigned int nbtag);

  // all the TRF info
  struct TRFres {
    // resutls
    std::vector<std::vector<bool> > perm_ex; // for each tag mult, vector of bool: is the i-th jet tagged or not?
    std::vector<std::vector<bool> > perm_in;
    std::vector<std::vector<double> > trfwsys_ex; // map between syst and vecot with truth-tag weights (pos = # of b-tags)
    std::vector<std::vector<double> > trfwsys_in;
    std::vector<std::vector<int> > tbins_ex; //for each tag mult, vector of int: quantile of each jet
    std::vector<std::vector<int> > tbins_in;
    // changed
    std::vector<double> effMC; // map between syst and vector of eff*SF for each jet
    std::vector<double> eff; // map between syst and vector of eff*SF for each jet
    // changed
    std::map<std::string,std::vector<double>> eff_allOP; // same but map for each OP
    std::map<std::string,std::vector<double>> effMC_allOP; // same but map for each OP
    std::vector<double> permprob_ex; // probablity of chosen perm with nominal SF
    std::vector<double> permprob_in;
    std::vector<double> binsprob_ex; // probability of chosen quantile with nominal SF
    std::vector<double> binsprob_in;
  };

  TRFres m_trfRes;

  struct jetVariable{
    Analysis::CalibrationDataVariables vars;
    int flav;
    void print(){
      std::cout << "pt " << vars.jetPt << "  eta " << vars.jetEta << "  tag-wei " << vars.jetTagWeight << "  flav " << flav << std::endl;
      return;
    }
  };
  std::vector<jetVariable> * m_jets;

  bool fillVariables(const xAOD::Jet& jet, Analysis::CalibrationDataVariables& x) ;
  bool fillVariables(const double jetPt, const double jetEta, const double jetTagWeight, Analysis::CalibrationDataVariables& x) ;

};

#endif // CPBTAGGINGTRUTHTAGGINGTOOL_H

