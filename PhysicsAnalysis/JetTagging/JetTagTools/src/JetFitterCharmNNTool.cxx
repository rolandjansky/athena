/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
/// Name    : JetFitterCharmNNTool.h
/// Package : BTagTools 
/// Author  : Giacinto Piacquadio, Christian Weiser (University of Freiburg)
/// Created : January 2008
///
/// Adopted for JetFitterCharm by Dan Guest in 2012
///
/// DESCRIPTION:
///
/// This class computes the Neural Network probability for being b-jet, 
/// c-jet or uds-jet for 
/// a single event. JetNet 3.5 (TM) is used for training, interfaced from 
/// C++ code compiled with ROOT.
///
/////////////////////////////////////////////////////////////////////////////

#include <string> 
#include <map>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <boost/format.hpp>

#include "TH1D.h"
#include "TH2D.h"

#include "JetTagTools/JetFitterCharmNNTool.h"

#include "JetTagCalibration/CalibrationBroker.h"
#include "TrkNeuralNetworkUtils/NeuralNetworkToHistoTool.h"
#include "TrkNeuralNetworkUtils/TTrainedNetwork.h"
#include "JetTagInfo/IJetFitterTagInfo.h"
#include "JetTagInfo/JetFitterGenericTagInfo.h"



namespace Analysis {


  // strings used to look up hists
  const std::string JetFitterCharmNNTool::TOPOLOGY_HIST = "LayersInfo"; 
  const std::string JetFitterCharmNNTool::WEIGHTS_HISTS = "Layer%i_weights"; 
  const std::string JetFitterCharmNNTool::THRESHOLD_HISTS = "Layer%i_thresholds"; 
  const std::string JetFitterCharmNNTool::INPUTS_HIST = "InputsInfo"; 
  const std::string JetFitterCharmNNTool::ETA_HIST = "eta_cat"; 
  const std::string JetFitterCharmNNTool::PT_HIST = "pt_cat"; 

  // names of the catigory information expected by the nn
  // (these must be defined in the nn calibration)
  const std::string JetFitterCharmNNTool::PT_CAT_NAME = "cat_pT"; 
  const std::string JetFitterCharmNNTool::ETA_CAT_NAME = "cat_eta"; 

  // variable names used internally to pass jet pt / eta around 
  const std::string JetFitterCharmNNTool::JET_PT = "JetPt"; 
  const std::string JetFitterCharmNNTool::JET_ETA = "JetEta"; 


  JetFitterCharmNNTool::JetFitterCharmNNTool(const std::string& name,
					     const std::string& n, 
					     const IInterface* p):
    AthAlgTool(name, n,p),
    m_calibrationDirectory("JetFitterCharm"),
    m_calibrationSubDirectory("NeuralNetwork"),
    m_calibrationTool("BTagCalibrationBroker"), 
    m_maximumRegisteredLayers(4), 
    m_networkToHistoTool("Trk::NeuralNetworkToHistoTool")
  {
    declareProperty("calibration_directory",m_calibrationDirectory);
    declareProperty("calibration_subdirectory",m_calibrationSubDirectory);
    declareProperty("calibration_tool",	m_calibrationTool);

    declareProperty("maximum_registered_layers",m_maximumRegisteredLayers);
    declareProperty("neural_network_to_histo_tool",m_networkToHistoTool);

    m_variable_remapping["IP3DNeg"] = "IP3D"; 
    m_variable_remapping["SV1Flip"] = "SV1"; 
    declareProperty("variable_remapping", m_variable_remapping); 

    // declareProperty("network_file",m_network_file_name); 
    // declareProperty("nn_name", m_nn_name); 
    declareInterface<IJetFitterClassifierTool>(this);

  } 

  ///////////////////////////////////////////////////////////////////
  /// Destructor - check up memory allocation
  /// delete any memory allocation on the heap

  JetFitterCharmNNTool::~JetFitterCharmNNTool() {

    for (std::map<std::string,RobustNN*>::iterator itr = m_networks.begin();
    	 itr != m_networks.end();
    	 itr++){ 
      delete itr->second; 
      itr->second = 0; 
    } 
  
  }

  StatusCode JetFitterCharmNNTool::initialize() {

    StatusCode sc = m_calibrationTool.retrieve();
    if (sc.isFailure())
      {
	ATH_MSG_FATAL( "Could not retrieve " << m_calibrationTool  
		      << ". Aborting...");
	return sc;
      } else ATH_MSG_INFO(" Retrieved: " << m_calibrationTool);

    sc = m_networkToHistoTool.retrieve();
    if (sc.isFailure()){
      ATH_MSG_FATAL(" Could not retrieve " << m_networkToHistoTool << 
		    ". Aborting...");
      return sc;
    } else ATH_MSG_INFO(" Retrieved: " << m_networkToHistoTool << ". ");


    initializeCalibrationFile();

    ATH_MSG_INFO(" Initialization of JetFitterNNTool succesfull");
    return StatusCode::SUCCESS;
  }

  void JetFitterCharmNNTool::initializeCalibrationFile()
  {

    registerHist(TOPOLOGY_HIST); 
    // TODO: see if I can remove this variable 
    // (and use the LayersInfo hist for this)
    
    for (Int_t i = 0; i < m_maximumRegisteredLayers - 1; ++i) {
      std::string wt_name = (boost::format(WEIGHTS_HISTS) % i).str(); 
      std::string th_name = (boost::format(THRESHOLD_HISTS) % i).str(); 

      registerHist(wt_name);
      registerHist(th_name); 
      
    }

    registerHist(INPUTS_HIST); 
    registerHist(ETA_HIST); 
    registerHist(PT_HIST); 

    ATH_MSG_DEBUG(" Registered NN histograms with directory: " << 
		  m_calibrationDirectory << " and subdirectory " << 
		  m_calibrationSubDirectory);

    m_calibrationTool->printStatus();
    
  }
  
  void JetFitterCharmNNTool::registerHist(std::string name) 
  { 
    std::string full_name = m_calibrationSubDirectory + "/" + name; 
    m_calibrationTool->registerHistogram(m_calibrationDirectory, full_name); 
  }

  TH1* JetFitterCharmNNTool::getHist(std::string jetauthor, std::string name)
  {
    std::string full_name = m_calibrationSubDirectory + "/" + name; 
    
    std::pair<TH1*, bool> histpair = m_calibrationTool->retrieveHistogram
	(m_calibrationDirectory, jetauthor, full_name); 

    if (histpair.second == true) {
      m_calibrationTool->updateHistogramStatus
	(m_calibrationDirectory, jetauthor, name, false);
    }
            
    TH1* hist = dynamic_cast<TH1*>(histpair.first);
      
    if (hist == 0){
      std::string full_name = m_calibrationDirectory + '/' + 
        jetauthor + '/' + m_calibrationSubDirectory + '/' + name; 
      throw std::runtime_error("Cannot retrieve histogram: " + full_name);
    } 
    else {
      ATH_MSG_VERBOSE(" Retrieved histo: " << name << 
		      " for channel: " << jetauthor << 
		      " the first bin content of the weight histo is: " << 
		      hist->GetBinContent(1,1));
    }
    
    return hist; 
  }
  
  StatusCode JetFitterCharmNNTool::finalize() {
  
    //here you should probably delete something :-)
  
    ATH_MSG_INFO(" Finalization of JetFitterCharmNNTool succesfull");
    return StatusCode::SUCCESS;
  }


  void 
  JetFitterCharmNNTool::fillLikelihoodValues(IJetFitterTagInfo& in_tag_info,
					     const std::string & jetauthor,
					     double jetpT,
					     double jeteta,
					     double /*IP3dlike*/) {
 
    if (jetauthor=="") {
      ATH_MSG_WARNING(" Hypothesis or jetauthor is empty."
		      " No likelihood value given back. ");
    }
  

    //at each fillLikelihood call check if the calibration is updated
    //(or load it for the first time)
    loadCalibration(jetauthor);


    // TODO: pass casted JetFitterGenericTagInfo to overloaded fillLikelihoodValues
    //       so that this function is simply a caster. 
    try { 
      dynamic_cast<JetFitterGenericTagInfo&>(in_tag_info); 
    }
    catch (const std::bad_cast& c) { 
      std::string error = "JetFitter is configured wrong: " __FILE__ 
	" requires JetFitterGenericTagInfo"
	" make sure you're using JetFitterCharmVariablesFactory";
      throw std::runtime_error(error); 
    }
  
    JetFitterGenericTagInfo& myTagInfo = dynamic_cast<JetFitterGenericTagInfo&>(in_tag_info); 

    myTagInfo.setTemporaryDouble(JET_PT, jetpT); 
    myTagInfo.setTemporaryDouble(JET_ETA, jeteta); 

    if (!m_networks.count(jetauthor)) { 
      throw std::runtime_error(" JetFitter NN instance " + jetauthor + 
			       " not found: cannot do any calculation...");
    }

    std::vector<double> weights = myTagInfo.tagLikelihood();
  
    if (weights.size()!=0) {
      weights.clear();
      ATH_MSG_WARNING(" The probability values for the tagging were"
		      " already filled. Inconsistency found...");
    }

    

    std::map<std::string, double> input_doubles(myTagInfo.getDoublesBegin(),
						myTagInfo.getDoublesEnd()); 

    for (std::map<std::string, int>::const_iterator 
	   itr = myTagInfo.getIntsBegin(); 
	 itr != myTagInfo.getIntsEnd(); 
	 itr++){ 

      input_doubles[itr->first] = double(itr->second); 
    }
    
    for (std::map<std::string, std::string>::const_iterator 
	     itr = m_variable_remapping.begin(); 
	 itr != m_variable_remapping.end(); 
	 itr++) { 
      if (input_doubles.find(itr->first) != input_doubles.end() ) { 
	input_doubles[itr->second] = input_doubles[itr->first]; 
	input_doubles.erase(itr->first); 
      }
    }

    // may throw a runtime error if the inputs aren't complete
    std::vector<double> outputValues 
	= m_networks.find(jetauthor)->second->calculate(input_doubles);

    ATH_MSG_DEBUG(" NN Discriminator b: " << outputValues[0] << 
		  " c: " << outputValues[1] << " u: " <<  outputValues[2]);

    // inversion with respect to NN output
    weights.push_back(outputValues[0]);//b 
    weights.push_back(outputValues[2]);//l
    weights.push_back(outputValues[1]);//c

    myTagInfo.setTagLikelihood(weights);
    myTagInfo.makeValid(); 
  }

  void JetFitterCharmNNTool::loadCalibration(const std::string & jetauthor) {
    
    std::map<std::string,TH1*> retrievedHistos;
    
    //it is assumed that in the NN:
    // 0 is bottom, 1 is charm, and 2 is light
    std::string layers_name = m_calibrationSubDirectory + "/" 
      + TOPOLOGY_HIST ; 

    std::pair<TH1*, bool> histoLayers = m_calibrationTool->retrieveHistogram
      (m_calibrationDirectory, jetauthor, layers_name);
    
    
    if (histoLayers.second==false && m_networks[jetauthor]!=0) {
      return;
    } 

    TH1* myHistoLayers = getHist(jetauthor, TOPOLOGY_HIST); 
    
    retrievedHistos[myHistoLayers->GetName()] = myHistoLayers;

    int nHidden = myHistoLayers->GetNbinsX() - 2;

    ATH_MSG_DEBUG(" Retrieving calibration for NN with: " << 
		  nHidden << " hidden layers.");

    for (Int_t i = 0; i < m_maximumRegisteredLayers - 1; ++i) {

      std::string wt_name = (boost::format(WEIGHTS_HISTS) % i).str(); 
      std::string th_name = (boost::format(THRESHOLD_HISTS) % i).str(); 

      TH1* myWeightHisto = getHist(jetauthor, wt_name); 

      retrievedHistos[myWeightHisto->GetName()] = myWeightHisto; 

      TH1* myThresholdHisto = getHist(jetauthor, th_name); 

      retrievedHistos[myThresholdHisto->GetName()] = myThresholdHisto;
    
    }

    TH1* norm_hist = getHist(jetauthor, INPUTS_HIST); 
    retrievedHistos[norm_hist->GetName()] = norm_hist; 
    
    TH1* eta_hist = getHist(jetauthor, ETA_HIST); 
    retrievedHistos[eta_hist->GetName()] = eta_hist; 
    TH1* pt_hist = getHist(jetauthor, PT_HIST); 
    retrievedHistos[pt_hist->GetName()] = pt_hist; 
    
    RobustNN* network = m_networks[jetauthor];
    if (network)
    {
      delete network;
      network = 0;
      ATH_MSG_DEBUG(" Istantiating TTrainedNetwork for jet author: " << 
		    jetauthor);
    }
    
    m_networks[jetauthor] = new RobustNN(retrievedHistos, this); 

  }


  // --- private helpers 

  JetFitterCharmNNTool::RobustNN::RobustNN(std::map<std::string,TH1*>& 
					   input_hists, 
					   const JetFitterCharmNNTool* parent)
  {
    TH1* pt_hist = input_hists[PT_HIST]; 
    for (int bin = 0; bin < pt_hist->GetNbinsX(); bin++) { 
      // pt is stored in GeV in histogram, convert here 
      double pt_this_bin = pt_hist->GetBinContent(bin + 1) * 1000.0; 
      m_pt_cat_map[pt_this_bin] = bin; 
    }
    TH1* eta_hist = input_hists[ETA_HIST]; 
    for (int bin = 0; bin < eta_hist->GetNbinsX(); bin++) { 
      m_abseta_cat_map[eta_hist->GetBinContent(bin + 1)] = bin; 
    }
    TH1* in_hist = input_hists[INPUTS_HIST]; 
    for (int bin = 1; bin <= in_hist->GetNbinsX(); bin++) { 
      m_allowed_inputs.insert(in_hist->GetXaxis()->GetBinLabel(bin)); 
    }
    
    m_flavnet = parent->m_networkToHistoTool->networkFromHists(input_hists); 
  }

  JetFitterCharmNNTool::RobustNN::~RobustNN(){ 
    delete m_flavnet; 
  }

  std::vector<double> JetFitterCharmNNTool
  ::RobustNN::calculate(const std::map<std::string,double>& in) const { 
    std::map<std::string, double> safe_values; 
    for (std::map<std::string, double>::const_iterator itr = in.begin(); 
	 itr != in.end(); 
	 itr++){ 
      if (m_allowed_inputs.count(itr->first) ) { 
	safe_values.insert(*itr); 
      }
    }
    assert(in.count(JET_PT)); 
    if (m_pt_cat_map.size() > 0 ) { 
      safe_values[PT_CAT_NAME] = get_bin
	(m_pt_cat_map, in.find(JET_PT)->second); 
    }
    assert(in.count(JET_ETA)); 
    if (m_abseta_cat_map.size() > 0 ) { 
      safe_values[ETA_CAT_NAME] = get_bin
	(m_abseta_cat_map, fabs(in.find(JET_ETA)->second) ); 
    }
    return m_flavnet->calculateNormalized(safe_values); 
  }
  JetFitterCharmNNTool::RobustNN::RobustNN(const RobustNN& old)
  {
    m_flavnet = dynamic_cast<TTrainedNetwork*>(old.m_flavnet->Clone()); 
    m_allowed_inputs = old.m_allowed_inputs; 
  }
  JetFitterCharmNNTool::RobustNN& 
  JetFitterCharmNNTool::RobustNN::operator=(const RobustNN& old) 
  { 
    delete m_flavnet; 
    m_flavnet = dynamic_cast<TTrainedNetwork*>(old.m_flavnet->Clone()); 
    m_allowed_inputs = old.m_allowed_inputs; 
    return *this; 
  }


  
  int JetFitterCharmNNTool
  ::RobustNN::get_bin(const std::map<double,int>& bins,
		      double value) const { 
    std::map<double, int>::const_iterator the_bin = bins.upper_bound(value); 
    if (the_bin == bins.end()) { 
      return bins.size(); 
    }
    return the_bin->second; 
  }




}//end Analysis namespace
