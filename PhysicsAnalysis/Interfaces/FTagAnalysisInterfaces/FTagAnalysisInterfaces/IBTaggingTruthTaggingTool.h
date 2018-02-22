// Dear emacs, this is -*- c++ -*-
///////////////////////////////////////////////////////////////////
// IBTaggingTruthTaggingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef CPIBTAGGINGTRUTHTAGGINGTOOL_H
#define CPIBTAGGINGTRUTHTAGGINGTOOL_H

#include "AsgTools/IAsgTool.h"
#include <string>

#include <vector>
#include <map>
// calibration data variable     
#include "CalibrationDataInterface/CalibrationDataVariables.h"

// xAOD jet 
#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicSet.h"

class IBTaggingTruthTaggingTool : virtual public asg::IAsgTool {

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( IBTagTruthTaggingTool )

  public:

  // initializie the tool
  virtual StatusCode initialize()=0;

  // set the seed of the raddom generator, needed only if choosing permutation or quantile          
  virtual StatusCode setSeed(unsigned int seed) =0;

  // set the jets in the event (pass same jets that satisfy kinematic criteria for b-tagging in pT and eta)
  virtual StatusCode setJets(std::vector<double>* pt, std::vector<double>* eta, std::vector<int>* flav, std::vector<double>* tagw) =0;

  virtual StatusCode setJets(const xAOD::JetContainer*& jets) =0;
  virtual StatusCode setJets(std::vector<int>* flav, std::vector<Analysis::CalibrationDataVariables>* vars) =0;
   
  // for one systematic
  virtual  StatusCode getTruthTagWei(unsigned int limit, std::vector<double> &trf_weight_ex, std::vector<double> &trf_weight_in, int sys =0) =0;
  // for all of the systematics
  virtual  StatusCode getTruthTagWei(unsigned int limit, std::vector<std::vector<double> > &trf_weight_ex, std::vector<std::vector<double> > &trf_weight_in) =0;
  // for all systematics , in map
  virtual  StatusCode getTruthTagWei(unsigned int limit, std::map<std::string,std::vector<double> > &trf_weight_ex, std::map<std::string,std::vector<double> > &trf_weight_in)=0;
  
  // tag permutation: trf_chosen_perm_ex.at(ntag).at(i) tells if the i-th jet is tagged in a selection requiring == ntag tags
  virtual StatusCode getTagPermutation(unsigned int nbtag, std::vector<std::vector<bool> > &trf_chosen_perm_ex, std::vector<std::vector<bool> > &trf_chosen_perm_in) =0;
 
  // quantiles
  virtual StatusCode getQuantiles(std::vector<std::vector<int> > &trf_bin_ex, std::vector<std::vector<int> > &trf_bin_in) =0;
  
  // functions to make comparison with direct-tagging easier
  virtual double getEvtSF(int syst=0) =0;
  virtual StatusCode getDirectTaggedJets(std::vector<bool> &is_tagged) =0; 
 
  // affecting systematics (same entries as in the TT weight maps)
  virtual   std::vector<std::string> affectingSystematics()=0;
 
};
#endif // CPIBTAGGINGTRUTHTAGGINGTOOL_H
