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
#include "TVector.h"
#include "TFile.h"
#include <string>
#include <vector>
#include <map>

// include xAODBtaggingEfficiency classes
#include "FTagAnalysisInterfaces/IBTaggingEfficiencyTool.h"
#include "FTagAnalysisInterfaces/IBTaggingSelectionTool.h"
#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"
#include "AsgTools/AnaToolHandle.h"

// calibration data variable
#include "CalibrationDataInterface/CalibrationDataVariables.h"
#include "xAODBTaggingEfficiency/TruthTagResults.h"

// xAOD jet
#include "xAODJet/JetContainer.h"

class BTaggingTruthTaggingTool: public asg::AsgTool,
        public virtual IBTaggingTruthTaggingTool {
  //typedef float (xAOD::BTagging::* tagWeight_member_t)() const;

  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS3( BTaggingTruthTaggingTool , IBTaggingTruthTaggingTool, ISystematicsTool, CP::IReentrantSystematicsTool )

    private:
      struct jetVariable{
      Analysis::CalibrationDataVariables vars;
      int flav;
    };

    // all the results about a single event are stored in this object
    struct TRFinfo {

      std::vector<jetVariable> jets;
        
      // features that will be used by the onnx tool
      std::vector<std::vector<float> > node_feat;
        
      unsigned int njets;

      TRandom3 rand;

      std::vector<std::vector<bool> > perm_ex; // for each tag mult, vector of bool: is the i-th jet tagged or not?
      std::vector<std::vector<bool> > perm_in;
      std::vector<std::vector<float> > trfwsys_ex; // map between syst and vecot with truth-tag weights (pos = # of b-tags)
      std::vector<std::vector<float> > trfwsys_in;
      std::vector<std::vector<int> > tbins_ex; //for each tag mult, vector of int: quantile of each jet
      std::vector<std::vector<int> > tbins_in;

      std::vector<float> effMC; // map between syst and vector of eff*SF for each jet
      std::vector<float> eff; // map between syst and vector of eff*SF for each jet

      std::map<std::string,std::vector<float>> eff_allOP; // same but map for each OP
      std::map<std::string,std::vector<float>> effMC_allOP; // same but map for each OP
      std::vector<float> permprob_ex; // probablity of chosen perm with nominal SF
      std::vector<float> permprob_in;
      std::vector<float> binsprob_ex; // probability of chosen quantile with nominal SF
      std::vector<float> binsprob_in;

      std::map<int,std::vector<std::vector<std::vector<bool> > > > perms;
      std::vector<std::vector<float> > permsWeight;
      std::vector<std::vector<float> > permsSumWeight;

    };

  public:
  /// Create a constructor for standalone usage
  BTaggingTruthTaggingTool( const std::string& name );

  private:
  StatusCode CalculateResults(TRFinfo &trfinf, Analysis::TruthTagResults& results,int rand_seed = -1);
            
  public:
  StatusCode CalculateResults( std::vector<float>& pt, std::vector<float>& eta, std::vector<int>& flav, std::vector<float>& tagw, Analysis::TruthTagResults& results,int rand_seed = -1);
  StatusCode CalculateResults( const xAOD::JetContainer& jets, Analysis::TruthTagResults& results,int rand_seed = -1);
        
  // will use onnxtool
  StatusCode CalculateResultsONNX( const std::vector<std::vector<float>>& node_feat, std::vector<float>& tagw,  Analysis::TruthTagResults& results, int rand_seed=-1);
  StatusCode CalculateResultsONNX( const xAOD::JetContainer& jets, const std::vector<std::vector<float>>& node_feat, Analysis::TruthTagResults& results, int rand_seed = -1);

  StatusCode setEffMapIndex(const std::string& flavour, unsigned int index);
  void setUseSystematics(bool useSystematics);

  virtual  ~BTaggingTruthTaggingTool();

  StatusCode initialize();

  CP::SystematicSet affectingSystematics() const;
  CP::SystematicCode applySystematicVariation( const CP::SystematicSet & systConfig);
  CP::SystematicSet recommendedSystematics() const;
  bool isAffectedBySystematic( const CP::SystematicVariation & systematic ) const;


  private:

  // set the jets in the event (pass same jets that satisfy kinematic criteria for b-tagging in pT and eta)
  StatusCode setJets(TRFinfo &trfinf,std::vector<float>& pt, std::vector<float>& eta, std::vector<int>& flav, std::vector<float>& tagw);
  StatusCode setJets(TRFinfo &trfinf,const xAOD::JetContainer& jets);
  StatusCode setJets(TRFinfo &trfinf,std::vector<int>& flav, const std::vector<Analysis::CalibrationDataVariables>& vars);

  // overloaded with node_feat that is used by onnx tool
  StatusCode setJets(TRFinfo &trfinf, const std::vector<std::vector<float>>& node_feat, std::vector<float>& tagw);
  StatusCode setJets(TRFinfo &trfinf, const xAOD::JetContainer& jets, const std::vector<std::vector<float>>& node_feat);
  StatusCode setJets(TRFinfo &trfinf,std::vector<int>& flav, const std::vector<Analysis::CalibrationDataVariables>& vars, const std::vector<std::vector<float>>& node_feat);

            
  // get truth tagging weights
  // for one single systematic (including "Nominal")
  StatusCode GetTruthTagWeights(TRFinfo &trfinf, std::vector<float> &trf_weight_ex, std::vector<float> &trf_weight_in, int sys=0);

  // tag permutation: trf_chosen_perm_ex.at(ntag).at(i) tells if the i-th jet is tagged in a selection requiring == ntag tags
  StatusCode getTagPermutation(TRFinfo &trfinf, std::vector<std::vector<bool> > &trf_chosen_perm_ex, std::vector<std::vector<bool> > &trf_chosen_perm_in);

  // chosen quantile: trf_bin_ex.at(ntag).at(i) tells the quantile in which the i-th jet falls in a selection requiring == ntag tags
  // returns 5 if between 60% and 0%
  // returns 4 if between 70% and 60%
  // returns 3 if between 77% and 70%
  // returns 2 if between 85% and 77%
  // returns 1 if between 100% and 85%
  // returns 0 if smaller than -1e4-> should never happen --> not currently implemented
  // return -1 if bigger than 1e4 or not in b-tagging acceptance --> not currently implemented
  StatusCode getQuantiles(TRFinfo &trfinf,std::vector<std::vector<int> > &trf_bin_ex, std::vector<std::vector<int> > &trf_bin_in);

  // functions to make comparison with direct-tagging easier
  float getEvtSF(TRFinfo &trfinf,int syst=0);
  StatusCode getDirectTaggedJets(TRFinfo &trfinf,std::vector<bool> &is_tagged);

  //These WP must be listed in ascending order of cut value, meaning 85 to 60
  std::vector<std::string> m_availableOP_fixCut= {"FixedCutBEff_85", "FixedCutBEff_77","FixedCutBEff_70","FixedCutBEff_60"};
  std::vector<std::string> m_availableOP_fixEff= {"FlatBEff_85", "FlatBEff_77", "FlatBEff_70", "FlatBEff_60"};
  std::vector<std::string> m_availableOP_HybEff= {"HybBEff_85", "HybBEff_77", "HybBEff_70", "HybBEff_60"};
  std::vector<std::string> m_availableOP;

  //this vector gets filled automatically when you initialize the tool, and given to the user.
  std::vector<float> m_binEdges = {};
  TFile *m_inf; //file for reading the cut values from the CDI.

  bool m_initialised;

  StatusCode getTRFweight(TRFinfo &trfinf,unsigned int nbtag, bool isInclusive, int sys);

  StatusCode getAllEffMC(TRFinfo &trfinf);
  StatusCode getAllEffMCCDI(TRFinfo &trfinf);
  StatusCode getAllEffMCGNN(TRFinfo &trfinf);
            
  StatusCode getAllEffSF(TRFinfo &trfinf,int =0);
  std::vector<CP::SystematicSet> m_eff_syst;
  std::vector<std::string> m_sys_name;

  // flav labelling
  int jetFlavourLabel (const xAOD::Jet& jet);

  int GAFinalHadronFlavourLabel(const xAOD::Jet& jet);
  int ConeFinalPartonFlavourLabel (const xAOD::Jet& jet);
  int ExclusiveConeHadronFlavourLabel (const xAOD::Jet& jet);
  std::vector<std::string> split(const std::string& str, char token);
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
  /// operating point when running in Continuous
  std::string m_cutBenchmark;
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
  /// in case of continuous WP you can choose to ignore some of the eigenvectors
  std::string m_excludeEV;
  ///possibility to compute the direct tagging SFs map directly from the TruthTaggingTool
  bool m_doDirectTag;
  /// if this string is empty, the onnx tool won't be used
  std::string m_pathToONNX;
  /// tagging strategy is required to do TT with GNN, when we don't want to truth tag all the jets (eg. 'leading2SignalJets')          
  std::string m_taggingStrategy;            
  /// will be set according to m_taggingStrategy
  enum NjetsTagStrategy {AllJets=-1, Leading2SignalJets=2, Leading3SignalJets=3};
  NjetsTagStrategy m_njetsTagStrategy;

  //*********************************//
  // Prop. of BTaggingSelectionTool  //
  //*********************************//

  float m_maxEta;
  float m_minPt;
  float m_maxRangePt;
  //  std::string m_CutFileName;

  // properties of truth tagging
  bool m_doOnlyUpVariations;
  bool m_ignoreSF;
  bool m_usePerm;
  bool m_useQuntile;
  bool m_continuous;
  bool m_useSys;
  int m_nbtag;

  unsigned int m_OperatingPoint_index;
  unsigned int m_OP_index_for_GNN;

  std::map<std::string, asg::AnaToolHandle<IBTaggingEfficiencyTool> > m_effTool_allOP;


  asg::AnaToolHandle<IBTaggingEfficiencyTool> m_effTool;
  asg::AnaToolHandle<IBTaggingSelectionTool> m_selTool; //!

  StatusCode check_syst_range(unsigned int sys);

  std::vector<std::vector<bool> > generatePermutations(int njets, int tags, int start=0);

  float trfWeight(TRFinfo &trfinf,const std::vector<bool> &tags);

  StatusCode chooseAllTagPermutation(TRFinfo &trfinf,unsigned int nbtag);
  StatusCode chooseTagPermutation(TRFinfo &trfinf,unsigned int nbtag, bool isIncl);
  float getPermutationRW(TRFinfo &trfinf,bool isIncl,unsigned int nbtag, int sys);


  StatusCode chooseAllTagBins(TRFinfo &trfinf);
  StatusCode chooseTagBins_cum(TRFinfo &trfinf,std::vector<bool> &tagconf, bool isIncl, unsigned int nbtag);
  StatusCode generateRandomTaggerScores(std::vector< std::vector<int> > &quantiles, std::vector< std::vector<float> > &scores);
  float getTagBinsConfProb(TRFinfo &trfinf,std::vector<int> &tagws);
  float getTagBinsRW(TRFinfo &trfinf,bool isIncl, unsigned int nbtag);


  bool fillVariables(const xAOD::Jet& jet, Analysis::CalibrationDataVariables& x);
  bool fillVariables(const float jetPt, const float jetEta, const float jetTagWeight, Analysis::CalibrationDataVariables& x);


};

#endif // CPBTAGGINGTRUTHTAGGINGTOOL_H

