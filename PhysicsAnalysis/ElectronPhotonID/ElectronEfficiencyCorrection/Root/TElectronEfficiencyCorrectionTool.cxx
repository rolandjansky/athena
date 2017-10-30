/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class TElectronEfficiencyCorrectionTool
   @brief Calculate the egamma scale factors in pure ROOT
   @author Karsten Koeneke, Felix Buehrer, Kristin Lohwasser
   @date   July 2012
*/

// This class header
#include "ElectronEfficiencyCorrection/TElectronEfficiencyCorrectionTool.h"
// STL includes
#include <iomanip>
#include <iostream>
#include <cfloat>
#include <math.h>
#include <limits.h>
#include "CxxUtils/make_unique.h"
#include <memory>
// ROOT includes
#include "TString.h"
#include "TSystem.h"
#include "TObjString.h"
#include "TROOT.h"
#include "TFile.h"
#include "TClass.h"
#include "TMD5.h"

namespace{

  template <class T>
    inline std::string toString(const T& in){
    std::stringstream stream;
    stream << in;
    return stream.str();
  }
}

namespace mapkey{
  enum key{ sf =1,
	    stat=2,
	    eig=3,
	    uncorr=4,	      
	    sys=5

  };
  std::string keytostring (int input){
    switch(input){
    case(sf) : 
      return "sf";
    case(stat) : 
      return "stat";
    case(eig) : 
      return "eig";
    case(uncorr) : 
      return "uncorr";
    case(sys) : 
      return "sys";
    }
    return "";
  }
}

// =============================================================================
// Constructor
// =============================================================================
Root::TElectronEfficiencyCorrectionTool::TElectronEfficiencyCorrectionTool(const char *name) :
  Root::TCalculatorToolBase(name),
  asg::AsgMessaging(std::string(name)),
  m_Rndm(),
  m_randomCounter(0),
  m_isInitialized(false),
  m_detailLevel(2),
  m_toyMCSF(-1),
  m_seed(0),
  m_doToyMC(false),
  m_doCombToyMC(false),
  m_nToyMC(0),
  m_nSys(0),
  m_nSysMax(0),
  m_runNumBegin(0),
  m_runNumEnd(0),
  m_resultPrefix("efficiency_SF"),
  m_resultName(""),
  m_position_eff(0),
  m_position_err(0),
  m_position_statErr(0),
  m_position_nSys(0),
  m_position_uncorrSys(0),
  m_nbins(0),
  m_nSimpleUncorrSyst(0),
  m_position_globalBinNumber(0)
{
  //Setup the keys
  m_keys.push_back(mapkey::sf);
  m_keys.push_back(mapkey::stat);
  m_keys.push_back(mapkey::eig);
  m_keys.push_back(mapkey::uncorr);
  //
}

// =============================================================================
// Destructor
// =============================================================================

Root::TElectronEfficiencyCorrectionTool::~TElectronEfficiencyCorrectionTool() {

  //Need some gymnastic to make sure that the TObjArray elements are owned
  //and deleted ... 
  for (auto  &tempit : m_histList) {
    for (unsigned int i = 0; i < tempit.second.size(); ++i) {
        tempit.second.at(i).SetOwner(kTRUE);
    }
  }
  for (auto  &tempit : m_fastHistList) {
    for (unsigned int i = 0; i < tempit.second.size(); ++i) {
        tempit.second.at(i).SetOwner(kTRUE);
    }
  }
  for (auto  &tempit : m_sysList) {
    for (auto  &i : tempit) {
      i.SetOwner(kTRUE);
    }
  }
  for (auto  &tempit :  m_fastSysList) {
    for (auto  &i : tempit) {
      i.SetOwner(kTRUE);
    }
  }
  for (auto  &tempit : m_uncorrToyMCSystFull) {
    for (auto & i : tempit) {
      i.SetOwner(kTRUE);
    }
  }  
  for (auto  &tempit : m_uncorrToyMCSystFast) {
    for (auto & i : tempit) {
      i.SetOwner(kTRUE);
    }
  }
}

// =============================================================================
// Initialize this correction tool
// =============================================================================
int Root::TElectronEfficiencyCorrectionTool::initialize() {
  // use an int as a StatusCode
  int sc(1);
  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: "
		<< __LINE__ << ") " << "Debug flag set. Printing verbose output!");

  //Avoid double init
  if (m_isInitialized) {
    ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "tool initialized twice!");
    return 0;
  }
  //Check if files are present
  if (m_corrFileNameList.size() == 0) {
    ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << " No file added!");
    return 0;
  }
  ATH_MSG_INFO("Initializing tool with " << m_corrFileNameList.size() << " configuration file(s)");
  //
  // Check if the first file can be opened (needed for auto-setting of the seed based on the md5-sum of the file)
  const std::unique_ptr<char> fname(gSystem->ExpandPathName(m_corrFileNameList[0].c_str()));
  std::unique_ptr<TFile> rootFile_tmp = CxxUtils::make_unique<TFile> (fname.get(), "READ");
  if (!rootFile_tmp) {
    ATH_MSG_ERROR(
		  " (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "No ROOT file found here: " << m_corrFileNameList[0]);
    return 0;
  }
  rootFile_tmp->Close();
  // close it back again
  //
  //invalid input requested
  if (m_doToyMC && m_doCombToyMC) {
    ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << " Both regular and combined toy MCs booked!"
		  << " Only use one!");
    return 0;
  }

  // initialize the random number generator if toyMC propagation booked
  if (m_doToyMC || m_doCombToyMC) {
    if (m_seed == 0) {
      std::unique_ptr<TMD5> tmd=CxxUtils::make_unique<TMD5>();
      const char* tmd_as_string=tmd->FileChecksum(fname.get())->AsString();
      m_seed = *reinterpret_cast<const int*>(tmd_as_string);
      ATH_MSG_INFO("Seed (automatically) set to " << m_seed);
    }else {
      ATH_MSG_INFO("Seed set to " << m_seed);
    }
    m_Rndm= TRandom3(m_seed);
  }
  //
  // Load the needed histograms
  if (0 == getHistograms()) {
    ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "! Problem when calling getHistograms()");
    return 0;
  }
  unsigned int nRunNumbersFull = m_begRunNumberList.size();
  unsigned int nRunNumbersFast = m_begRunNumberListFastSim.size();

  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		<< "Found " << nRunNumbersFast << " run number ranges for fast sim with a total of " <<
                m_fastHistList[mapkey::sf].size() << " scale factor histograms.");

  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		<< "Found " << nRunNumbersFull << " run number ranges for full sim with a total of " <<
                m_histList[mapkey::sf].size() << " scale factor histograms.");
  //
  // --------------------------------------------------------------------------
  // Register the output / results
  // NOTE: THE ORDER IS IMPORTANT!!! 
  m_position_eff = m_result.addResult((m_resultPrefix + m_resultName).c_str(), "efficiency scale factor");
  if (m_position_eff < 0) {
    sc = 0; // Exceeded the number of allowed results
  }
  m_position_err = m_result.addResult(
				      (m_resultPrefix + m_resultName + "_err").c_str(), "efficiency scale factor uncertainty");
  if (m_position_err < 0) {
    sc = 0; // Exceeded the number of allowed results
  }
  m_position_statErr = m_result.addResult(
					  (m_resultPrefix + m_resultName + "_statErr").c_str(), "efficiency scale factor statistical uncertainty");
  if (m_position_statErr < 0) {
    sc = 0; // Exceeded the number of allowed results
  }
  m_position_nSys = m_result.addResult(
				       (m_resultPrefix + m_resultName + "_nSys").c_str(),
				       "number of corrrelated systematic uncertainties (dependent of detailLevel)");
  if (m_position_nSys < 0) {
    sc = 0; // Exceeded the number of allowed results
  }
  m_position_uncorrSys = m_result.addResult(
					    (m_resultPrefix + m_resultName + "_uncorrSys").c_str(), "total uncorrelated uncertainty");
  if (m_position_uncorrSys < 0) {
    sc = 0; // Exceeded the number of allowed results
  }
  for (int sys = 0; sys < m_nSysMax; sys++) {
    m_position_corrSys.push_back(m_result.addResult((m_resultPrefix + m_resultName + "_corrSys_" +
                                                     toString(sys)).c_str(),
                                                    "correlated uncertainty number " + toString(sys + 1)));
    if (m_position_corrSys.at(sys) < 0) {
      sc = 0; // Exceeded the number of allowed results
    }
    m_result.setResult(m_position_corrSys.at(sys), 0);
  }

  for (int toy = 0; toy < m_nToyMC; toy++) {
    m_position_uncorrToyMCSF.push_back(m_result.addResult((m_resultPrefix + m_resultName + "_uncorrToyMCSF_" +
                                                           toString(toy)).c_str(),
                                                          "Toy MC scale factor number " + toString(toy)));
    if (m_position_uncorrToyMCSF.at(toy) < 0) {
      sc = 0; // Exceeded the number of allowed results
    }
    m_result.setResult(m_position_uncorrToyMCSF.at(toy), 0);
  }

  m_position_globalBinNumber = m_result.addResult(
						  (m_resultPrefix + m_resultName + "_globalBinNumber").c_str(), "bin position in histograms");
  if (m_position_globalBinNumber < 0) {
    sc = 0; // Exceeded the number of allowed results
  }
  // Set the results to default values
  m_result.setResult(m_position_eff, -999.0);
  m_result.setResult(m_position_err, 1.0);
  m_result.setResult(m_position_statErr, 0.0);
  m_result.setResult(m_position_nSys, 0);
  m_result.setResult(m_position_uncorrSys, 0);
  m_isInitialized = kTRUE;
  // --------------------------------------------------------------------------
  ATH_MSG_INFO("Tool succesfully initialized!");
  return sc;
}

// =============================================================================
// Calculate the actual accept of each cut individually.
// =============================================================================
const Root::TResult &
Root::TElectronEfficiencyCorrectionTool::calculate(const PATCore::ParticleDataType::DataType dataType,
                                                   const unsigned int runnumber,
                                                   const double cluster_eta,
                                                   const double et /* in MeV */
                                                   ) {
  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << this->getName()
		<< "  entering function calculate");

  // A few helpful strings
  static const std::string LowPt_string("LowPt");
  //
  //
  //
  // check if a file is given and tool correctly initialized
  if (!m_isInitialized) {
    ATH_MSG_ERROR(
		  " (file: " << __FILE__ << ", line: " << __LINE__ << ") " <<
		  " Tool not correctly initialized or no scale factor file given. ");
    return m_result;
  }
  ATH_MSG_DEBUG("(file: " << __FILE__ << ", line: " << __LINE__ << ") " << this->getName() << " n Systematics: " << m_nSysMax);
  // Reset the results to default values
  m_result.setResult(m_position_eff, -999.0);
  m_result.setResult(m_position_err, 1.0);
  m_result.setResult(m_position_statErr, 0.0);
  m_result.setResult(m_position_uncorrSys, 0);
  //
  for (int sys = 0; sys < m_nSysMax; sys++) {
    m_result.setResult(m_position_corrSys.at(sys), 0);
  }
  for (int toy = 0; toy < m_nToyMC; toy++) {
    m_result.setResult(m_position_uncorrToyMCSF.at(toy), 0);
  }
  m_result.setResult(m_position_globalBinNumber, 0);
  //
  //
  //See if fastsim
  bool isFastSim=(dataType == PATCore::ParticleDataType::Fast) ? true: false;
  //
  //Find the corresponding run index for this period
  int runnumberIndex = -1;
  if (isFastSim) {
    for (unsigned int i = 0; i < m_begRunNumberListFastSim.size(); ++i) {
      if (m_begRunNumberListFastSim[i] <= runnumber && runnumber <= m_endRunNumberListFastSim[i]) {
	runnumberIndex=i;
	break;
      }
    }  
  }else {
    for (unsigned int i = 0; i < m_begRunNumberList.size(); ++i) {
      if (m_begRunNumberList[i] <= runnumber && runnumber <= m_endRunNumberList[i]) {
	runnumberIndex=i;
	break;
      }
    }
  }
  //
  if(runnumberIndex <0 ){
    if (this->msgLvl(MSG::DEBUG)){
      printDefaultReturnMessage(TString::Format(
						"No valid run number period  found for the current run number: %i for simulation type: %i",
						runnumber, dataType),
				__LINE__);
    }    
    return m_result;
  }
  //
  //
  //What we have is a map 
  //key is sf,stat,eigen, uncorr
  //The vector has as many entries as supported run period
  //The TobjArray has 2D histos for  high, low et, or forward
  //The 2D Histo has the number we want.
  const std::map<int, std::vector< TObjArray > >& currentmap = (isFastSim)? m_fastHistList : m_histList;
  std::map<int, std::vector< TObjArray > >::const_iterator currentVector_itr = currentmap.find(mapkey::sf); //find the vector
  //See if we can find a SF vector in the map and the corresponding TobjArray for this run period 
  if (currentVector_itr == currentmap.end()) {
    if (this->msgLvl(MSG::DEBUG)){	
      printDefaultReturnMessage(TString::Format(
						"No valid vector of sf ObjArray found for the current run number: %i for simulation type: %i",
						runnumber, dataType),
				__LINE__);
    }
    return m_result;
  }
  //Get a reference (synonym) to this vector 
  const std::vector<TObjArray>& currentVector=currentVector_itr->second;
  if (currentVector.size()<=0 || runnumberIndex>= static_cast <signed> (currentVector.size())) {
    if (this->msgLvl(MSG::DEBUG)){
      printDefaultReturnMessage(TString::Format(
						"No valid sf ObjArrays found for the current run number: %i for simulation type: %i",
						runnumber, dataType),
				__LINE__);
    }
    return m_result;
  }
  //
  //
  //Now we can get the corresponding Object array 
  const TObjArray& currentObjectArray = currentVector.at(runnumberIndex);
  //
  //This is the number of entries in the TObjArray  
  const int entries = currentObjectArray.GetEntries();
  //Find the correct histogram in the TObjArray (Low, high Et or forward)
  double xValue, yValue;
  xValue = et;
  yValue = cluster_eta;
  int smallEt(0), etaCov(0), nSF(0);
  bool block = kFALSE;
  bool changed = kFALSE;
  int index = -1;
  TH2 *tmpHist(0);
  for (int i = 0; i < entries ; ++i) {
    block = kFALSE;
    tmpHist = (TH2 *) (currentObjectArray.At(i));

    //block if we are below minimum et 
    if (et < tmpHist->GetXaxis()->GetXmin()) {
      smallEt++;
      block = kTRUE;
    }

    //block if we are above max eta 
    if (TMath::Abs(yValue) >= tmpHist->GetYaxis()->GetXmax()) {
      etaCov++;
      block = kTRUE;
    }
    // Blocj if we are less than minimum (fwd electrons)
    if (TMath::Abs(yValue) < tmpHist->GetYaxis()->GetXmin()) {
      etaCov++;
      block = kTRUE;
    }
    //Block if above max et and is the low Et histo
    if (et > tmpHist->GetXaxis()->GetXmax()) {
      if (TString(tmpHist->GetName()).Contains(LowPt_string)) {
        block = kTRUE;
      } else {
        xValue = tmpHist->GetXaxis()->GetBinCenter(tmpHist->GetNbinsX());
        changed = kTRUE;
      }
    }
    if (!block) {
      index = i;
      if (!changed) {
        nSF++;
      }
    }
  }
  //We are out of bounds 
  //
  if (smallEt == entries) {
    if (this->msgLvl(MSG::DEBUG)){
      printDefaultReturnMessage(TString::Format("No correction factor provided for et=%f", xValue), __LINE__);
    }
    return m_result;
  }
  if (etaCov == entries) {
    if (this->msgLvl(MSG::DEBUG)){
      printDefaultReturnMessage(TString::Format("No correction factor provided for eta=%f", yValue), __LINE__);
    }
    return m_result;
  }
  if (nSF > 1) {
    ATH_MSG_WARNING(
		    "More than 1 SF found for eta=" << yValue << " , et = " << et << " , run number = " << runnumber <<
		    " . Please check your input files!");
  }
  //
  //
  //Now we have the index of the histogram for this region in the TObjectarray 
  TH2* currentHist(0);
  if (index >= 0) {
    currentHist = static_cast<TH2*> (currentObjectArray.At(index));
  }
  else {
    if (this->msgLvl(MSG::DEBUG)){
      printDefaultReturnMessage(TString::Format(
						"No correction factor provided because there was an index problem"), __LINE__);
    }
    return m_result;
  }
  ///
  // If SF is only given in Abs(eta) convert eta input to TMath::Abs()
  float epsilon = 1e-6;
  if (currentHist->GetYaxis()->GetBinLowEdge(1) >= 0 - epsilon) {
    if (yValue < 0) {
      ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		    << "Scale factor only measured in Abs(eta) changing eta from " << yValue << " to " << TMath::Abs(
														     yValue));
    }
    yValue = TMath::Abs(yValue);
  }
  //
  int globalBinNumber = currentHist->FindFixBin(xValue, yValue);
  double scaleFactor = currentHist->GetBinContent(globalBinNumber);
  double scaleFactorErr = currentHist->GetBinError(globalBinNumber);
  //
  // Write the retrieved values into the return object
  m_result.setResult(m_position_eff, scaleFactor);
  m_result.setResult(m_position_err, scaleFactorErr);
  //
  //DONE WITH THE SF
  //
  //DO the stat error using the available info from above i.e index etc
  double statErr = -999;
  currentVector_itr = currentmap.find(mapkey::stat); //find the vector
  if (currentVector_itr != currentmap.end()) {
    //itr at the location of the vector, .second get the vector, at(runnumberIndex is the TObjectArray 
    // for the period , finaly get the hist at index (from above).
    TH1 *stat = static_cast<TH1*>(currentVector_itr->second.at(runnumberIndex).At(index));
    statErr = stat->GetBinContent(globalBinNumber);
    m_result.setResult(m_position_statErr, statErr);
  }
  //
  //
  //Do the eigen
  currentVector_itr = currentmap.find(mapkey::eig); //find the vector
  if (currentVector_itr != currentmap.end()) {
    //Check on the ObjArray
    if (currentVector_itr->second.at(runnumberIndex).GetEntriesFast()> 0) {
      //itr at the location of the vector, .second get the vector, at(runnumberIndex is the TObjectArray 
      // for the period , finaly get the hist at index (from above).
      TH1 *eig = static_cast<TH1*>(currentVector_itr->second.at(runnumberIndex).At(index));
      m_sLevel[Root::TElectronEfficiencyCorrectionTool::simple] = 0;
      m_sLevel[Root::TElectronEfficiencyCorrectionTool::medium] = 0;
      m_sLevel[Root::TElectronEfficiencyCorrectionTool::detailed] = 0;
      int nSys = eig->GetNbinsX() - 1;
      double sign = 0;
      // Calculate detail level
      for (int i = nSys + 1; i >= 2; i--) {
        sign += eig->GetBinContent(i);
        if (sign > 0.8 && m_sLevel[Root::TElectronEfficiencyCorrectionTool::simple] == 0) {
          m_sLevel[Root::TElectronEfficiencyCorrectionTool::simple] = i - 2;
        }
        if (sign > 0.95 && m_sLevel[Root::TElectronEfficiencyCorrectionTool::medium] == 0) {
          m_sLevel[Root::TElectronEfficiencyCorrectionTool::medium] = i - 2;
        }
      }
      nSys -= m_sLevel[m_detailLevel];
      m_result.setResult(m_position_nSys, nSys);
    }
  }
  //
  //
  //The issue now is that the previous setup is becoming cumbersome for the 10 systematic
  //So we keep them in a vector of vector of TObjectArray
  //The first vector index being the runnumber
  //The second the systematic
  //And the obj array for high low etc
  //We need to see if we can do something better here ...
  //
  std::vector< std::vector< TObjArray > > &sysList = (isFastSim) ? m_fastSysList : m_sysList;
  static std::vector<double> corrSys(10); // Avoid some re-allocations of  memory
  corrSys.clear();
  if (sysList.size() > (unsigned int) index) {
    if (sysList.at(index).size() > (unsigned int) runnumberIndex) {
      const int sys_entries = sysList.at(index).at( runnumberIndex).GetEntries();
    
      for (int sys = 0; sys < sys_entries; ++sys) {
	tmpHist = (TH2 *) sysList.at(index).at(runnumberIndex).At(sys_entries - 1 - sys);
	corrSys.push_back(tmpHist->GetBinContent(globalBinNumber));
	m_result.setResult(m_position_corrSys[(sys_entries - 1 - sys)], corrSys[sys]);
      }
      if (m_position_corrSys.size() > 0 && sys_entries<=1) {
	if (m_result.getResult(m_position_corrSys[0]) == 0) {
	  m_result.setResult(m_position_corrSys[0], scaleFactorErr);
	}
      }
    }
  }
  //
  //Do the uncorr error using the available info from above i.e index etc
  //////////////////////////////////////////////////////////////////
  //Get the statErr from above
  double val = statErr;
  currentVector_itr = currentmap.find(mapkey::uncorr); //find the vector
  if (currentVector_itr != currentmap.end()) {
    //Check on the ObjArray
    if (currentVector_itr->second.at(runnumberIndex).GetEntriesFast()>0) {
      TH1 *uncorr = static_cast<TH1*>(currentVector_itr->second.at(runnumberIndex).At(index));
      double valAdd = uncorr->GetBinContent(globalBinNumber);
      val = sqrt(val * val + valAdd * valAdd);
      for (int i = 0; i < m_sLevel[m_detailLevel]; ++i) {
        double valAdd = corrSys.at(corrSys.size() - 1 - i);
        val = sqrt(val * val + valAdd * valAdd);
      }
    }
  }
  if (val == -999) {
    val = 0;
  }
  m_result.setResult(m_position_uncorrSys, val);
  ///
  //Here are the toys 
  ////////////////////////// 
  if (m_doToyMC || m_doCombToyMC) {
    std::vector<std::vector<TObjArray > >& toyMCList = ((isFastSim) ? m_uncorrToyMCSystFast : m_uncorrToyMCSystFull);
      if (toyMCList.size() > (unsigned int) runnumberIndex) {
        for (int toy = 0; toy < m_nToyMC; toy++) {
          if (toyMCList.at(runnumberIndex).at(toy).GetLast() >= index) {
            m_result.setResult(m_position_uncorrToyMCSF.at(toy),
                               ((TH2 *) toyMCList.at(runnumberIndex).at(toy).At(index))->GetBinContent(globalBinNumber));
          }
        }
      }
  }
  m_result.setResult(m_position_globalBinNumber, globalBinNumber);
  return m_result;
}

// =============================================================================
// Calculate the detail levels for a given eigenvector histogram
// =============================================================================
void
Root::TElectronEfficiencyCorrectionTool::calcDetailLevels(TH1D *eig) {
 
  m_sLevel[Root::TElectronEfficiencyCorrectionTool::simple] = 0;
  m_sLevel[Root::TElectronEfficiencyCorrectionTool::medium] = 0;
  m_sLevel[Root::TElectronEfficiencyCorrectionTool::detailed] = 0;
  
  int nSys = eig->GetNbinsX() - 1;
  double sign = 0;
  // Calculate detail level
  for (int i = nSys + 1; i >= 2; i--) {
    sign += eig->GetBinContent(i);
    if (sign > 0.8 && m_sLevel[Root::TElectronEfficiencyCorrectionTool::simple] == 0) {
      m_sLevel[Root::TElectronEfficiencyCorrectionTool::simple] = i - 2;
    }
    if (sign > 0.95 && m_sLevel[Root::TElectronEfficiencyCorrectionTool::medium] == 0) {
      m_sLevel[Root::TElectronEfficiencyCorrectionTool::medium] = i - 2;
    }
  }
  m_nSys = nSys;
}

// =============================================================================
// Build the toyMC tables from inputs
// =============================================================================
std::vector<TH2D *>
Root::TElectronEfficiencyCorrectionTool::buildSingleToyMC(TH2D *sf, TH2D *stat, TH2D *uncorr, const TObjArray& corr) {
  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ")! " << "entering function buildSingleToyMC");

  std::vector<TH2D*> tmpHists;
  int nBins = (stat->GetNbinsX() + 2) * (stat->GetNbinsY() + 2);
  for (int toy = 0; toy < m_nToyMC; toy++) {
    tmpHists.push_back((TH2D *) corr.At(0)->Clone());
  }
  // Loop over all bins
  for (int bin = 0; bin < nBins; bin++) {
    double val = stat->GetBinContent(bin);

    // Add uncorrelated systematics
    if (uncorr != 0) {
      double valAdd = uncorr->GetBinContent(bin);
      val = sqrt(val * val + valAdd * valAdd);
    }

    // Add smaller correlated systematics as uncorrelated
    for (int i = 0; i < m_sLevel[m_detailLevel]; ++i) {
      if (corr.At(i) != 0) {
        double valAdd = ((TH2D *) corr.At(i))->GetBinContent(bin);
        val = sqrt(val * val + valAdd * valAdd);
      }
    }
    for (int toy = 0; toy < m_nToyMC; toy++) {
      tmpHists.at(toy)->SetBinContent(bin, (val * m_Rndm.Gaus(0, 1)) + sf->GetBinContent(bin));
      m_randomCounter++;
      tmpHists.at(toy)->SetDirectory(0);
    }
  }
  return tmpHists;
}

// =============================================================================
// Build the combined toyMC tables from inputs
// =============================================================================
TH2D*
Root::TElectronEfficiencyCorrectionTool::buildSingleCombToyMC(TH2D *sf, TH2D *stat, TH2D *uncorr, const TObjArray& corr) {
  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "entering function buildSingleCombToyMC");

  TH2D *tmpHist;
  int nBins = (stat->GetNbinsX() + 2) * (stat->GetNbinsY() + 2);
  tmpHist = (TH2D *) corr.At(0)->Clone();

  // Create random numbers for the corr. uncertainties
  std::vector<double> rnd (m_nSys,0);
  for (int s = 0; s < m_nSys; s++) {
    rnd[s] = m_Rndm.Gaus(0, 1);
    m_randomCounter++;
  }

  // Loop over all bins
  for (int bin = 0; bin < nBins; bin++) {
    double val = stat->GetBinContent(bin);

    // Add uncorrelated systematics
    if (uncorr != 0) {
      double valAdd = uncorr->GetBinContent(bin);
      val = sqrt(val * val + valAdd * valAdd);
    }
    // Add smaller correlated systematics as uncorrelated

    for (int s = 0; s < m_sLevel[m_detailLevel]; s++) {
      if (corr.At(s) != 0) {
        double valAdd = ((TH2D *) corr.At(s))->GetBinContent(bin);
        val = sqrt(val * val + valAdd * valAdd);
      }
    }

    val = val * m_Rndm.Gaus(0, 1);
    m_randomCounter++;

    // Add larger correlated systematics
    for (int s = m_sLevel[m_detailLevel]; s < m_nSys; s++) {
      if (corr.At(s) != 0) {
        val += ((TH2D *) corr.At(s))->GetBinContent(bin) * rnd[s];
      }
    }
    tmpHist->SetBinContent(bin, val + sf->GetBinContent(bin));
  }

  tmpHist->SetDirectory(0);
  return tmpHist;
}

// =============================================================================
// Build the toyMC tables from inputs
// =============================================================================
std::vector<TObjArray>
Root::TElectronEfficiencyCorrectionTool::buildToyMCTable(const TObjArray& sf, const TObjArray& eig, const TObjArray& stat,
                                                         const TObjArray& uncorr, const std::vector<TObjArray>& corr) {

  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "entering function buildToyMCTable");
  std::vector<TObjArray> tmpVec;
  const int stat_entries = stat.GetEntries();

  if (m_doCombToyMC) {
    for (int toyMC = 0; toyMC < m_nToyMC; toyMC++) {
      TObjArray tmpArray;
      for (int i = 0; i < stat_entries; ++i) {
        if (eig.GetEntriesFast()>0 && uncorr.GetEntriesFast()>0) {
          calcDetailLevels((TH1D *) eig.At(i));
          tmpArray.Add(buildSingleCombToyMC((TH2D *) sf.At(i), (TH2D *) stat.At(i), (TH2D *) uncorr.At(i), corr.at(i)));
        }else {
          tmpArray.Add(buildSingleCombToyMC((TH2D *) sf.At(i), (TH2D *) stat.At(i), 0, corr.at(i)));
        }
      }
      tmpVec.push_back(tmpArray);
    }
  }else {
    std::vector< std::vector<TH2D*> > tmpVec2 ;
    for (int i = 0; i < stat_entries; ++i) {
      calcDetailLevels((TH1D *) eig.At(i));
      tmpVec2.push_back(buildSingleToyMC((TH2D *) sf.At(i), (TH2D *) stat.At(i), 
					 (TH2D *) uncorr.At(i),corr.at(i)));
    }
    for (int toy = 0; toy < m_nToyMC; toy++) {
      TObjArray tmpArray;
      for (unsigned int i = 0; i < tmpVec2.size(); ++i) {
        tmpArray.Add(tmpVec2.at(i).at(toy));
      }
      tmpVec.push_back(tmpArray);
    }
  }
  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "m_Rndm->Uniform(0, 1) after throwing " << m_randomCounter
		<< " random numbers: " << m_Rndm.Uniform(0,1));
  return tmpVec;
}

// =============================================================================
// Helper function to retrieve number of uncorrelated bins
// =============================================================================
int
Root::TElectronEfficiencyCorrectionTool::getNbins(std::map<float, std::vector<float> > &pt_eta1) const {
  const std::vector<TObjArray >& tmpVec = m_histList.at(mapkey::sf);
  int nbinsTotal = 0;
  pt_eta1.clear();
  std::vector<float>eta1;
  eta1.clear();

  for (unsigned int ikey = 0; ikey < tmpVec.size(); ++ikey) {
    for (int entries = 0; entries < (tmpVec.at(ikey)).GetEntries(); entries++) {
      eta1.clear();

      TH2D *h_tmp = ((TH2D * ) (tmpVec.at(ikey)).At(entries));
      int nbinsX = h_tmp->GetNbinsX();
      int nbinsY = h_tmp->GetNbinsY();

      for (int biny = 1; biny <= nbinsY; biny++) {
        eta1.push_back(h_tmp->GetYaxis()->GetBinLowEdge(biny));
        if (entries == (tmpVec.at(ikey)).GetEntries() - 1) {
          eta1.push_back(h_tmp->GetYaxis()->GetBinLowEdge(biny + 1));
        }
      }
      for (int binx = 1; binx < nbinsX; binx++) {
        pt_eta1[h_tmp->GetXaxis()->GetBinLowEdge(binx)] = eta1;
        if (entries == (tmpVec.at(ikey)).GetEntries() - 1) {
          pt_eta1[h_tmp->GetXaxis()->GetBinLowEdge(binx + 1)] = eta1;
        }
      }
    }
  }
  for (auto &i : pt_eta1) {
    nbinsTotal += i.second.size();
  }
  return nbinsTotal;
}

// =============================================================================
// Helper function to retrieve the position of the first toy MC scale factor
// =============================================================================
int Root::TElectronEfficiencyCorrectionTool::getFirstToyMCPosition() const {
  if (!m_isInitialized) {
    ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "Tool not initialized.");
    return -1;
  }
  if (m_nToyMC > 0) {
    return m_position_uncorrToyMCSF.at(0);
  }else {
    ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		  << " No toy scale factors booked, returning the position of the scale factor! This is most likely not what you want!");
    return 0;
  }
}

int Root::TElectronEfficiencyCorrectionTool::getLastToyMCPosition() const {
  if (!m_isInitialized) {
    ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "Tool not initialized.");
    return -1;
  }
  if (m_nToyMC > 0) {
    return m_position_uncorrToyMCSF.at(m_nToyMC - 1);
  } else {
    ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		  << "No toy scale factors booked, returning the position of the scale factor! This is most likely not what you want!");
    return 0;
  }
}
//================================================================================
/// Helpers
int Root::TElectronEfficiencyCorrectionTool::getFirstCorrSysPosition() const {
  if (!m_isInitialized) {
    ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "Tool not initialized.");
    return -1;
  }
  if (m_position_corrSys.size() > 0) {
    return m_position_corrSys.at(0);
  }else {
    ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		  << "No correlated uncertainties found, returning the position of the scale factor! This is most likely not what you want!");
    return 0;
  }
}

int Root::TElectronEfficiencyCorrectionTool::getLastCorrSysPosition() const {
  if (!m_isInitialized) {
    ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "Tool not initialized.");
    return -1;
  }

  if (m_position_corrSys.size() > 0) {
    if (m_result.getResult(3) > 0) {
      return m_position_corrSys.at(m_result.getResult(3) - 1);
    }else {
      return m_position_corrSys.at(0);
    }
  }else {
    ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		  << "No correlated uncertainties found, returning the position of the scale factor! This is most likely not what you want!");
    return 0;
  }
}
int Root::TElectronEfficiencyCorrectionTool::getGlobalBinNumberPosition() const {
  if (!m_isInitialized) {
    ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "Tool not initialized.");
    return -1;
  }
  return m_position_globalBinNumber;
}

void Root::TElectronEfficiencyCorrectionTool::printResultMap() const {
  if (!m_isInitialized) {
    ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "Tool not initialized.");
    return;
  }
  ATH_MSG_INFO("!" << std::endl << "############" << std::endl << "Position  \t -\t Description ");
  for (unsigned int pos = 0; pos < m_result.getNResults(); pos++) {
    ATH_MSG_INFO(pos << " \t \t - \t " << m_result.getResultDescription(pos));
  }
}
//================================================================================

// =============================================================================
// Get the input histograms from the input files
// =============================================================================
int Root::TElectronEfficiencyCorrectionTool::getHistograms() {

  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "entering function getHistograms");
  // Cache the current directory in the root file
  TDirectory *origDir = gDirectory;
  // -------------------------------------------------------
  // Get the name of the first input ROOT file and
  // interpret from that what we have:
  // efficiency vs. efficiencySF; offline vs. trigger; medium, loose,...
  // -------------------------------------------------------
  if (!m_corrFileNameList.empty()) {
    TString firstFileNameAndPath = m_corrFileNameList[0].c_str();
    std::unique_ptr<TObjArray> myStringList(firstFileNameAndPath.Tokenize("/"));
    int lastIdx = myStringList->GetLast();
    TString fileName = ((TObjString *) myStringList->At(lastIdx))->GetString();
    std::unique_ptr<TObjArray> myFileNameTokensList(fileName.Tokenize("."));
  
    if (myFileNameTokensList->GetLast() < 3) {
      ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "input file name has wrong format!");
      return 0;
    }

    if (m_resultPrefix.empty()) {// Only overwrite it if the user didn't explicitly set it
      m_resultPrefix = ((TObjString *) myFileNameTokensList->At(0))->GetString();
      m_resultPrefix += "_";
    }
    if (m_resultName.empty()) {// Only overwrite it if the user didn't explicitly set it
      m_resultName = ((TObjString *) myFileNameTokensList->At(1))->GetString()
	+ "_" + ((TObjString *) myFileNameTokensList->At(2))->GetString();
    }
    ATH_MSG_INFO("Using resultPrefix: " << m_resultPrefix
		 << " and resultName: " << m_resultName);

  }
  // -------------------------------------------------------
  // Get all ROOT files and histograms
  // -------------------------------------------------------
  for (unsigned int i = 0; i < m_corrFileNameList.size(); ++i) {
    // Load the ROOT file
    const std::unique_ptr<char> fname (gSystem->ExpandPathName(m_corrFileNameList[i].c_str()));
    std::unique_ptr<TFile> rootFile = CxxUtils::make_unique<TFile> (fname.get(), "READ");
    if (!rootFile) {
      ATH_MSG_ERROR(
		    " (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "No ROOT file found here: " <<
		    m_corrFileNameList[i]);
      return 0;
    }
    // Loop over all directories inside the root file (correspond to the run number ranges
    TIter nextdir(rootFile->GetListOfKeys());
    TKey *dir;
    TObject *obj;
    while ((dir = (TKey *) nextdir())) {
      obj = dir->ReadObj();
      if (obj->IsA()->InheritsFrom("TDirectory")) {
        // splits string by delimiter --> e.g RunNumber1_RunNumber2
	//std::unique_ptr<TObjArray> dirNameArray(TString(obj->GetName()).Tokenize("_"));
        TObjArray dirNameArray = *(TString(obj->GetName()).Tokenize("_"));
        //// returns index of last string --> if one, the directory name does not contain any run numbers
        int lastIdx = dirNameArray.GetLast();
        if (lastIdx != 1) {
          ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "The folder name seems to have the wrong format! Directory name:"
			<< obj->GetName());
          return 0;
        }
        rootFile->cd(obj->GetName());
        if (0 == this->setupHistogramsInFolder(dirNameArray, lastIdx)) {
          ATH_MSG_ERROR(
			" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "unable to setup the histograms in directory " << dir->GetName()
			<< "in file " << m_corrFileNameList[i]);
          return 0;
        }
      }else {
        ATH_MSG_ERROR(
		      " (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "Wrong file content! Expected only Directories " <<
		      gDirectory->cd());
        return 0;
      }
      // Return to the original ROOT directory
      gDirectory = origDir;
    } // End: directory loop
  } // End: file loop
  return 1;
}

// =============================================================================
// Get the input histograms from a given folder/run number range
// =============================================================================
int Root::TElectronEfficiencyCorrectionTool::setupHistogramsInFolder(const TObjArray& dirNameArray, int lastIdx) {

  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "entering funtion setupHistogramsInFolder");

  m_runNumBegin = -1;
  TString myBegRunNumString = ((TObjString *) dirNameArray.At(lastIdx - 1))->GetString();
  if (myBegRunNumString.IsDigit()) {
    m_runNumBegin = myBegRunNumString.Atoi();
  }
  m_runNumEnd = -1;
  TString myEndRunNumString = ((TObjString *) dirNameArray.At(lastIdx))->GetString();
  if (myEndRunNumString.IsDigit()) {
    m_runNumEnd = myEndRunNumString.Atoi();
  }
  if (m_runNumBegin < 0 || m_runNumEnd < 0 || m_runNumEnd < m_runNumBegin) {
    ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "Could NOT interpret the run number range: " << m_runNumBegin
		  << " - " << m_runNumEnd);
    return 0;
  }
  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << m_runNumBegin << "  " << m_runNumEnd);
  //
  /// setup pairs of obj arrays and keys --> e.g. "sf", new Array to take all SF Histos
  std::map<int, TObjArray> objsFull;
  std::map<int, TObjArray > objsFast;
  for (unsigned int ikey = 0; ikey < m_keys.size(); ++ikey) {
    TObjArray dummyFull;
    objsFull.insert(std::make_pair(m_keys.at(ikey),dummyFull));
    TObjArray dummyFast;
    objsFast.insert(std::make_pair(m_keys.at(ikey),dummyFast));
  }  
  TObjArray dummyFull;    
  objsFull.insert(std::make_pair(mapkey::sys, dummyFull));
  TObjArray dummyFast;
  objsFast.insert(std::make_pair(mapkey::sys, dummyFast));
  //---------------------------------------------------------
  //
  std::vector<TObjArray > sysObjsFull;
  std::vector<TObjArray > sysObjsFast;
  //
  TIter nextkey(gDirectory->GetListOfKeys());
  TKey *key;
  TObject *obj(0);
  TString tmpName = "";
  int tmpCounter = 0;

  //
  //Loop of the keys 
  while ((key = (TKey *) nextkey())) {
    obj = key->ReadObj();
    if (obj->IsA()->InheritsFrom("TH1")) {
      // The histogram containing the scale factors need to end with _sf and need to contain either the string "FullSim"
      // or "AtlFast2"!
      if (TString(obj->GetName()).Contains("FullSim")) {
	//Add all except the correlated 
	for (unsigned int ikey = 0; ikey < m_keys.size(); ++ikey) {
	  if (TString(obj->GetName()).EndsWith("_" +  TString(mapkey::keytostring(m_keys.at(ikey))))) {
	    objsFull.find(m_keys.at(ikey))->second.Add(obj);
	  }
	}
	//
	tmpName = TString(obj->GetName());
	//Special treatment , this is only for photons 
	if (tmpName.EndsWith("_sys")) {
	  objsFull.find(mapkey::sys)->second.Add(obj);
	  TObjArray tmpArrayFull;
	  tmpArrayFull.Add(obj);
	  sysObjsFull.push_back(tmpArrayFull);
	  tmpCounter++;
	}
	//
	//See if we are dealing with correlated
	if (tmpName.Contains("_corr")) {
 	  if (tmpName.EndsWith("corr0")) {
	    //This is the worse part ...
	    //corr0 triggers a few thing
	    //
	    //1st create a TObjectArray
	    //For high or low Pt (one for each ...)
	    TObjArray tmpArrayFull;
	    //Resgister it to the vector (so the vector has size 1 or 2)
	    sysObjsFull.push_back(tmpArrayFull);
            //Reset the counter
	    tmpCounter=0;
	  }
	  //Add to the current last element of the sysObject full
	  //This will be Low Pt once and high Pt the other time
	  sysObjsFull.back().Add(obj);
	  //Now increase the counter
	  tmpCounter++;
	}
	if (tmpCounter > m_nSysMax) {
	  m_nSysMax = tmpCounter;
	}
      }
      else if (TString(obj->GetName()).Contains("AtlFast2")) {
	for (unsigned int ikey = 0; ikey < m_keys.size(); ++ikey) {
	  if (TString(obj->GetName()).EndsWith("_" +  TString(mapkey::keytostring(m_keys.at(ikey))))) {
	    objsFast.find(m_keys.at(ikey))->second.Add(obj);
	  }
	}
	//See if we are dealing with correlated
	tmpName = TString(obj->GetName());
	//Special treatment , this is only for photons 
	if (tmpName.EndsWith("_sys")) {
	  objsFast.find(mapkey::sys)->second.Add(obj);
	  TObjArray tmpArrayFast;
	  tmpArrayFast.Add(obj);
	  sysObjsFast.push_back(tmpArrayFast);
	  tmpCounter++;
	}
	//
	//See if we are dealing with correlated
	if (tmpName.Contains("_corr")) {
 	  if (tmpName.EndsWith("corr0")) {
	    //This is the worse part ...
	    //corr0 triggers a few thing
	    //1st create a TObjectArray
	    TObjArray tmpArrayFast;
	    //Resgister it
	    sysObjsFast.push_back(tmpArrayFast);
            //Reset the counter
	    tmpCounter=0;
	  }
	  //Add to the current last element of the sysObject full
	  //This will be Low Pt once and high Pt the other time
	  sysObjsFast.back().Add(obj);
	  //Now increase the counter
	  tmpCounter++;
	}
	if (tmpCounter > m_nSysMax) {
	  m_nSysMax = tmpCounter;
	}
      } else {
	ATH_MSG_ERROR(
		      " (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "Could NOT interpret if the histogram: " << obj->GetName()
		      << " is full or fast simulation!");
	return 0;
      }
    }
  }

  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "setting up histograms for run ranges  " <<
		m_runNumEnd);
  //
  //The setup here copies from the temporaties created in this function , 
  //to the actual class member variables.
  for (unsigned int ikey = 0; ikey < m_keys.size(); ++ikey) {
    if (objsFull.find(m_keys.at(ikey))->second.GetEntries() != 0) {
      if (0 ==
	  setup(objsFull.find(m_keys.at(ikey))->second, m_histList[m_keys.at(ikey)], 
		m_begRunNumberList,m_endRunNumberList)) {
	ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "! Could NOT setup histogram " 
		      << m_keys.at(ikey)<< " for full sim!");
	return 0;
      }
    }
    if (objsFast.find(m_keys.at(ikey))->second.GetEntries() != 0) {
      if (0 ==
	  setup(objsFast.find(m_keys.at(ikey))->second, m_fastHistList[m_keys.at(ikey)],
		m_begRunNumberListFastSim, m_endRunNumberListFastSim)) {
	ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "! Could NOT setup histogram " << m_keys.at(ikey)
		      << " for fast sim");
	return 0;
      }
    }
  }
  for (unsigned int sys = 0; sys < sysObjsFast.size(); sys++) {
    m_fastSysList.resize(sysObjsFast.size());
    if (0 == setup(sysObjsFast.at(sys), m_fastSysList[sys], m_begRunNumberListFastSim, m_endRunNumberListFastSim)) {
      ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " <<
		    "! Could NOT setup systematic histograms for fast sim");
      return 0;
    }
  }
  for (unsigned int sys = 0; sys < sysObjsFull.size(); sys++) {
    m_sysList.resize(sysObjsFull.size());
    if (0 == setup(sysObjsFull.at(sys), m_sysList[sys], m_begRunNumberList, m_endRunNumberList)) {
      ATH_MSG_ERROR(
		    " (file: " << __FILE__ << ", line: " << __LINE__ << ") " <<
		    "! Could NOT setup systematic histograms for fast sim");
      return 0;
    }
  }
  //
  //Toys
  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << this->getName()
		<< "Checking for (m_doToyMC || m_doCombToyMC)");

  if (m_doToyMC || m_doCombToyMC) {
    bool fullToysBooked = kFALSE;
    bool fastToysBooked = kFALSE;
    TObjArray dummy;

    if (m_histList[mapkey::sf].size() > 0) {
      if (objsFull.find(mapkey::eig)->second.GetEntries() < 1 || objsFull.find(mapkey::stat)->second.GetEntries() < 1 ||
	  objsFull.find(mapkey::uncorr)->second.GetEntries() < 1) {
	if (objsFull.find(mapkey::stat)->second.GetEntries() > 1 || objsFull.find(mapkey::sys)->second.GetEntries() > 1) {
	  m_uncorrToyMCSystFull.push_back(buildToyMCTable(objsFull.find(mapkey::sf)->second, dummy,
							   objsFull.find(mapkey::stat)->second,
							   dummy, sysObjsFull));
	  fullToysBooked = kTRUE;
	}else {
	  ATH_MSG_INFO(
		       "! Toy MC error propagation booked, but not all needed histograms found in the output (For full sim). Skipping toy creation!");
	}
      }else {
	m_uncorrToyMCSystFull.push_back(buildToyMCTable(objsFull.find(mapkey::sf)->second, objsFull.find(mapkey::eig)->second,
							 objsFull.find(mapkey::stat)->second,
							 objsFull.find(mapkey::uncorr)->second, sysObjsFull));
	fullToysBooked = kTRUE;
      }
    }
    ///// AF2
    if (m_fastHistList[mapkey::sf].size() > 0) {
      if (objsFast.find(mapkey::eig)->second.GetEntries() < 1 || objsFast.find(mapkey::stat)->second.GetEntries() < 1 ||
	  objsFast.find(mapkey::uncorr)->second.GetEntries() < 1) {
	if (objsFast.find(mapkey::stat)->second.GetEntries() > 1 || objsFast.find(mapkey::sys)->second.GetEntries() > 1) {
	  m_uncorrToyMCSystFast.push_back(buildToyMCTable(objsFast.find(mapkey::sf)->second, dummy,
							   objsFast.find(mapkey::stat)->second,
							   dummy, sysObjsFast));
	  fastToysBooked = kTRUE;
	}else {
	  ATH_MSG_INFO(
		       "! Toy MC error propagation booked, but not all needed histograms found in the output (For fast sim). Skipping toy creation!");
	}
      }else {
	m_uncorrToyMCSystFast.push_back(buildToyMCTable(objsFast.find(mapkey::sf)->second, objsFast.find(mapkey::eig)->second,
							 objsFast.find(mapkey::stat)->second,
							 objsFast.find(mapkey::uncorr)->second, sysObjsFast));
	fastToysBooked = kTRUE;
      }
    }

    if (fullToysBooked || fastToysBooked) {
      if (m_doToyMC) {
	ATH_MSG_INFO("Created tables for " << m_nToyMC << " ToyMC systematics ");
      }
      if (m_doCombToyMC) {
	ATH_MSG_INFO("Created tables for " << m_nToyMC << " combined ToyMC systematics ");
      }
    }
  }
  return 1;
}

// =============================================================================
// Fill and interpret the setup, depending on which histograms are found in the input file(s)
// =============================================================================
int
Root::TElectronEfficiencyCorrectionTool::setup(const TObjArray& hists,
                                               std::vector< TObjArray> &histList,
                                               std::vector< unsigned int > &beginRunNumberList,
                                               std::vector< unsigned int > &endRunNumberList) {
  if (hists.GetEntriesFast()==0) {
    ATH_MSG_ERROR(
		  "(file: " << __FILE__ << ", line: " << __LINE__ << ") " << "! Could NOT find histogram with name *_sf in folder");
    return 0;
  }
  TH2 *tmpHist(0);
  for (int i = 0; i < hists.GetEntries(); ++i) {
    tmpHist = (TH2 *) hists.At(i);
    tmpHist->SetDirectory(0);
  }
  // Now, we have all the needed info. Fill the vectors accordingly
  histList.push_back(hists);
  if (beginRunNumberList.size() > 0) {
    if (m_runNumBegin != (int) beginRunNumberList.back()) {
      beginRunNumberList.push_back(m_runNumBegin);
    }
  }else {
    beginRunNumberList.push_back(m_runNumBegin);
  }
  if (endRunNumberList.size() > 0) {
    if (m_runNumEnd != (int) endRunNumberList.back()) {
      endRunNumberList.push_back(m_runNumEnd);
    }
  }else {
    endRunNumberList.push_back(m_runNumEnd);
  }
  return 1;
}
// =============================================================================
// print a message that the default scale factor is returned
// =============================================================================
void
Root::TElectronEfficiencyCorrectionTool::printDefaultReturnMessage(const TString& reason, int line) const{
  ATH_MSG_DEBUG(
		this->getName() << " (file: " << __FILE__ << ", line: " << line << ")  " << reason << "\n" <<
		"Returning scale factor -999 ");
}
