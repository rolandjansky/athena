/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
#include <iostream>
#include <cmath>
#include "CxxUtils/make_unique.h"
#include <memory>
// ROOT includes
#include "TSystem.h"
#include "TROOT.h"
#include "TFile.h"
#include "TClass.h"
#include "TKey.h"
#include "TH1.h"
#include "TH2.h"
#include "TString.h"
#include "TObjString.h"
#include "TMD5.h"

namespace mapkey{
enum key{ sf =1,
  stat=2,
  eig=3,
  uncorr=4,	      
  sys=5,
  end=6
};
const char* keytostring (int input){
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


namespace{
const std::string LowPt_string("LowPt");
const std::vector<int> s_keys={mapkey::sf,mapkey::stat,mapkey::eig,mapkey::uncorr};
}


Root::TElectronEfficiencyCorrectionTool::TElectronEfficiencyCorrectionTool(const char *name) :
  asg::AsgMessaging(std::string(name)),
  m_doToyMC(false),
  m_doCombToyMC(false),
  m_nToyMC(0),
  m_seed(0),
  m_nSysMax(0),
  m_Rndm()
{
}

Root::TElectronEfficiencyCorrectionTool::~TElectronEfficiencyCorrectionTool() {

  /*
   * Make sure that all TObjArray 
   * elements are owned and deleted 
   */
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

int Root::TElectronEfficiencyCorrectionTool::initialize() {
  // use an int as a StatusCode
  int sc(1);

  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: "
                << __LINE__ << ")\n" << "Debug flag set. Printing verbose output!");

  //Check if files are present
  if (m_corrFileNameList.size() == 0) {
    ATH_MSG_ERROR(" No file added!");
    return 0;
  }
  ATH_MSG_DEBUG("Initializing tool with " << m_corrFileNameList.size() 
                << " configuration file(s)");

  
  if (m_doToyMC && m_doCombToyMC) {
    ATH_MSG_ERROR(" Both regular and combined toy MCs booked!" << " Only use one!");
    return 0;
  }
  /*
   * initialize the random number generator if toyMC propagation booked
   * Use the 1st 4 bytes of the CheckSum of the reccomendation file as seed
   */
  if (m_doToyMC || m_doCombToyMC) {
    if (m_seed == 0) {
    // Use the name of the correction  for auto-setting of the seed based on the md5-sum of the file
      const std::unique_ptr<char[]> fname(gSystem->ExpandPathName(m_corrFileNameList[0].c_str()));
      std::unique_ptr<TMD5> tmd=CxxUtils::make_unique<TMD5>();
      const char* tmd_as_string=tmd->FileChecksum(fname.get())->AsString();
      m_seed = *(reinterpret_cast<const unsigned long int*>(tmd_as_string));
      ATH_MSG_DEBUG("Seed (automatically) set to " << m_seed);
    }else {
      ATH_MSG_DEBUG("Seed set to " << m_seed);
    }
    m_Rndm= TRandom3(m_seed);
  }
  /*
   * Load the needed histograms
   */
  if (0 == getHistograms()) {
    ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ")\n" << "! Problem when calling getHistograms()");
    return 0;
  }
  const unsigned int nRunNumbersFull = m_begRunNumberList.size();
  const unsigned int nRunNumbersFast = m_begRunNumberListFastSim.size();

  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ")\n"
                << "Found " << nRunNumbersFast << " run number ranges for fast sim with a total of " <<
                m_fastHistList[mapkey::sf].size() << " scale factor histograms.");

  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ")\n"
                << "Found " << nRunNumbersFull << " run number ranges for full sim with a total of " <<
                m_histList[mapkey::sf].size() << " scale factor histograms.");

   ATH_MSG_DEBUG("Tool succesfully initialized!");

  return sc;
}

int 
Root::TElectronEfficiencyCorrectionTool::calculate(const PATCore::ParticleDataType::DataType dataType,
                                                   const unsigned int runnumber,
                                                   const double cluster_eta,
                                                   const double et, /* in MeV */
                                                   std::vector<double>& result,
                                                   size_t& index_of_corr,
                                                   size_t& index_of_toys) const {

  /* 
   * At this point, we know the size of the vector.
   * Position::End + m_nSysMax + m_nToyMC
   * The starting index of the sys is Position::End 
   * The starting point of the toys is Position::End+m_nSysMax
   */
  ATH_MSG_DEBUG("Max number of systematics seen : " << m_nSysMax << " number of toys " <<m_nToyMC);
  result.resize(static_cast<size_t> (Position::End)+m_nSysMax+m_nToyMC);
  const size_t position_corrSys= static_cast<size_t> (Position::End);
  const size_t position_uncorrToyMCSF=position_corrSys+m_nSysMax;
  //Set up the non-0 defaults
  result[static_cast<size_t> (Position::SF)]=-999;
  result[static_cast<size_t> (Position::Total)]=1; 
  if (m_nSysMax) {
    index_of_corr=position_corrSys;
  }
  if (m_nToyMC) {
    index_of_toys=position_uncorrToyMCSF;
  } 
  /* 
   * Determine Simulation flavour and find the run period
   */
  const bool isFastSim=(dataType == PATCore::ParticleDataType::Fast) ? true: false;
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
  if(runnumberIndex <0 ){  
    ATH_MSG_DEBUG("(file: " << __FILE__ << ", line: " << __LINE__ << ")\n"<<
                  "No valid run number period  found for the current run number: " 
                  << runnumber <<" for simulation type: " << dataType); 
    return 0;
  }
  /* What we have is a map :
   * Key can be SF,Stat,Eigen,UnCorr
   * Entry is a vector<TObArray> 
   * Each vector<TObjArray>  has as many entries as supported Run periods.
   * Each TObjjArray has 2D histos (could be standard, low-et, or forward electrons)
   * The 2D Histo then has the number we want.
   * What follows is the logic to get to this number.
   */
  const std::unordered_map<int, std::vector< TObjArray > >& currentmap = (isFastSim)? m_fastHistList : m_histList;
  std::unordered_map<int, std::vector< TObjArray > >::const_iterator currentVector_itr = currentmap.find(mapkey::sf); 
  /*
   * See if we can find a  vector for key SF in the map 
   * and then if we can get the  corresponding TObjArray 
   * for the run period.
   */
  if (currentVector_itr == currentmap.end()) {
    ATH_MSG_DEBUG("(file: " << __FILE__ << ", line: " << __LINE__ << ")\n"<<
                  "No valid vector of sf ObjArray found for the current run number " 
                  << runnumber<<" for simulation type: " << dataType);  
    return 0;
  } 
  const std::vector<TObjArray>& currentVector=currentVector_itr->second;
  if (currentVector.size()<=0 || runnumberIndex>= static_cast <int> (currentVector.size())) {
    ATH_MSG_DEBUG("(file: " << __FILE__ << ", line: " << __LINE__ << ")\n"<<
                  "No valid  sf ObjArray found for the current run number " 
                  << runnumber<<" for simulation type: " << dataType);  
    return 0;
  }
  /* 
   * At this stage we have found the relevant TObjArray
   * So we need to locate the right histogram.
   */
  const TObjArray& currentObjectArray = currentVector.at(runnumberIndex);  
  const int entries = currentObjectArray.GetEntries();
  /* 
   * Now the logic of finding the histogram
   * Some parts of the code can be perhaps improved ...
   */
  double xValue(et);
  double yValue(cluster_eta);
  int smallEt(0), etaCov(0), nSF(0);
  bool invalid = false;
  bool changedEt = false;
  int index = -1;
  TH2 *tmpHist(0);

  for (int i = 0; i < entries ; ++i) {
    invalid = false;

    tmpHist = (TH2 *) (currentObjectArray.At(i));
    //invalid if we are below minimum et 
    if (et < tmpHist->GetXaxis()->GetXmin()) {
      smallEt++;
      invalid = true;
    }
    //invalid if we are above max eta 
    if (std::abs(yValue) >= tmpHist->GetYaxis()->GetXmax()) {
      etaCov++;
      invalid=true;
    }
    // invalid if we are less than minimum eta (forward electrons)
    if (std::abs(yValue) < tmpHist->GetYaxis()->GetXmin()) {
      etaCov++;
      invalid = true;
    }
    /* 
     * Invalid if above max et and is a low Et histogram.
     * If not low Et histogram then change the xValue to the maximum
     * availabe Et of ths histogram. As we assume that the  SF stays the same 
     * for very high Et
     */
    if (et > tmpHist->GetXaxis()->GetXmax()) {
      if (TString(tmpHist->GetName()).Contains(LowPt_string)) {
        invalid = true;
      } else {
        xValue = tmpHist->GetXaxis()->GetBinCenter(tmpHist->GetNbinsX());
        changedEt = true;
      }
    }
    /*
     * Get the histogram index in the TObjArray
     * Also mark how many times we found something
     * as SF should be unique
     */
    if (!invalid) {
      index = i;
      if (!changedEt) {
        nSF++;
      }
    }
  }
  if (smallEt == entries) {
    ATH_MSG_DEBUG("(file: " << __FILE__ << ", line: " << __LINE__ << ")\n"<<
                  "No correction factor provided for et " << xValue);  
    return 0;
  }
  if (etaCov == entries) {
    ATH_MSG_DEBUG("(file: " << __FILE__ << ", line: " << __LINE__ << ")\n"<<
                  "No correction factor provided for eta " << yValue);  
    return 0;
  }
  if (nSF>1) {
    ATH_MSG_WARNING("More than 1 SF found for eta=" << yValue << " , et = " 
                    << et << " , run number = " << runnumber << ". Please check your input files!");
  }
  /*
   * Now we have the index of the histogram 
   * for this region in the TObjectArray
   */
  TH2* currentHist(0);
  if (index >= 0) {
    currentHist = static_cast<TH2*> (currentObjectArray.At(index));
  }
  else {
    ATH_MSG_DEBUG("(file: " << __FILE__ << ", line: " << __LINE__ << ")\n"<<
                  "No correction factor provided because of an invalid index" << yValue);
    return 0;
  }
  /*
   * If SF is only given in Abs(eta) convert eta input to std::abs()
   */
  constexpr double epsilon = 1e-6;
  if (currentHist->GetYaxis()->GetBinLowEdge(1) >= 0 - epsilon) {
    if (yValue < -1) {
      ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ")\n"
                    << "Scale factor only measured in Abs(eta) changing eta from " 
                    << yValue << " to " << std::abs(yValue));
    }
    yValue = std::abs(yValue);
  }
  const int globalBinNumber = currentHist->FindFixBin(xValue, yValue);
  const double scaleFactor = currentHist->GetBinContent(globalBinNumber);
  const double scaleFactorErr = currentHist->GetBinError(globalBinNumber);
  /* 
   * Write the retrieved values to the output
   * */
  result[static_cast<size_t> (Position::SF)]=scaleFactor;
  result[static_cast<size_t> (Position::Total)]=scaleFactorErr;
  /*
   * Do the stat error using the available info from the above (SF)
   */
  double statErr = -999;
  currentVector_itr = currentmap.find(mapkey::stat); 
  if (currentVector_itr != currentmap.end()) {
    const TH1 *stat = static_cast<TH1*>(currentVector_itr->second.at(runnumberIndex).At(index));
    statErr = stat->GetBinContent(globalBinNumber);
    result[static_cast<size_t> (Position::Stat)]=statErr;
  }
  /*
   * Do the Uncorr  uncertainty
   */
  double val = statErr;
  currentVector_itr = currentmap.find(mapkey::uncorr); 
  if (currentVector_itr != currentmap.end()) {
    if (currentVector_itr->second.at(runnumberIndex).GetEntriesFast()>0) {
     TH1 *uncorr = static_cast<TH1*>(currentVector_itr->second.at(runnumberIndex).At(index));
      const double valAdd = uncorr->GetBinContent(globalBinNumber);
      val = sqrt(val * val + valAdd * valAdd);
    }
  }
  result[static_cast<size_t> (Position::UnCorr)]=val; 
  /* 
   * The previous setup is becoming cumbersome 
   * for the N~16 systematic variations.
   * So we keep them in a vector of vector of TObjectArray
   * The first vector index being the runnumber
   * The second the systematic
   * And them the TObjArray for high low etc.
   * We invert the order in the output
   */
  const std::vector< std::vector< TObjArray > > &sysList = (isFastSim) ? m_fastSysList : m_sysList;
  std::vector<double> corrSys; 
  corrSys.reserve(16); 
  corrSys.clear();
  if (sysList.size() > static_cast<unsigned int> (index)) {
    if (sysList.at(index).size() > static_cast<unsigned int> (runnumberIndex)) {
      const int sys_entries = sysList.at(index).at( runnumberIndex).GetEntries();
      for (int sys = 0; sys < sys_entries; ++sys) {
        tmpHist = (TH2 *) sysList.at(index).at(runnumberIndex).At(sys_entries - 1 - sys);
        corrSys.push_back(tmpHist->GetBinContent(globalBinNumber));
        result[position_corrSys + sys_entries - 1 - sys] =corrSys[sys];
        }
      if (m_nSysMax > 0 && sys_entries<=1) {
        if (result[position_corrSys] == 0) {
            result[position_corrSys]=scaleFactorErr;
        }
      }
    }
  }
  /* 
   * Do the toys
   */
  if (m_doToyMC || m_doCombToyMC) {
    const std::vector<std::vector<TObjArray > >& toyMCList = ((isFastSim) ? m_uncorrToyMCSystFast : m_uncorrToyMCSystFull);
    if (toyMCList.size() > (unsigned int) runnumberIndex) {
      for (int toy = 0; toy < m_nToyMC; toy++) {
        if (toyMCList.at(runnumberIndex).at(toy).GetLast() >= index) {
          result[position_uncorrToyMCSF+toy]=
            ((TH2 *) toyMCList.at(runnumberIndex).at(toy).At(index))->GetBinContent(globalBinNumber);
        }
      }
    }
  }
  result[static_cast<size_t> (Position::GlobalBinNumber)]=globalBinNumber;
  return 1;
}
/*
 * Build the toyMC tables from inputs
 * Ownership should be tranfered to the map of the tables
 * and the proper delete happens in the dtor
 */
std::vector<TH2 *>
Root::TElectronEfficiencyCorrectionTool::buildSingleToyMC(const TH2 *sf, 
                                                          const TH2 *stat, 
                                                          const TH2 *uncorr, 
                                                          const TObjArray& corr,
                                                          int& randomCounter) {

  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ")! " 
                << "Entering function buildSingleToyMC");
  std::vector<TH2*> tmpHists;
  int nBins = (stat->GetNbinsX() + 2) * (stat->GetNbinsY() + 2);
  for (int toy = 0; toy < m_nToyMC; toy++) {
    tmpHists.push_back((TH2 *) corr.At(0)->Clone());
  }
  // Loop over all bins
  for (int bin = 0; bin < nBins; bin++) {
    double val = stat->GetBinContent(bin);

    // Add uncorrelated systematics
    if (uncorr != 0) {
      double valAdd = uncorr->GetBinContent(bin);
      val = sqrt(val * val + valAdd * valAdd);
    }
    for (int toy = 0; toy < m_nToyMC; toy++) {
      tmpHists.at(toy)->SetBinContent(bin, (val * m_Rndm.Gaus(0, 1)) + sf->GetBinContent(bin));
      randomCounter++;
      tmpHists.at(toy)->SetDirectory(0);
    }
  }
  return tmpHists;
}
/*
 * Build the combined toyMC tables from inputs
 * Ownership should be tranfered to the map of the tables
 * and the proper delete happens in the dtor
 */
TH2*
Root::TElectronEfficiencyCorrectionTool::buildSingleCombToyMC(const TH2 *sf, 
                                                              const TH2 *stat, 
                                                              const TH2 *uncorr, 
                                                              const TObjArray& corr,
                                                              const int nSys,
                                                              int& randomCounter){

  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ")\n" 
                << "Entering function buildSingleCombToyMC");

  TH2 *tmpHist;
  const int nBins = (stat->GetNbinsX() + 2) * (stat->GetNbinsY() + 2);
  tmpHist = (TH2 *) corr.At(0)->Clone();
  // Create random numbers for the corr. uncertainties
  std::vector<double> rnd (nSys,0);
  for (int s = 0; s < nSys; ++s) {
    rnd[s] = m_Rndm.Gaus(0, 1);
    randomCounter++;
  }
  // Loop over all bins
  for (int bin = 0; bin < nBins; ++bin) {
    double val = stat->GetBinContent(bin);

    // Add uncorrelated systematics
    if (uncorr != 0) {
      double valAdd = uncorr->GetBinContent(bin);
      val = sqrt(val * val + valAdd * valAdd);
    }
    val = val * m_Rndm.Gaus(0,1);
    randomCounter++;
    // Add larger correlated systematics
    for (int s = 0; s < nSys; ++s) {
      if (corr.At(s) != 0) {
        val += ((TH2 *) corr.At(s))->GetBinContent(bin) * rnd[s];
      }
    }
    tmpHist->SetBinContent(bin, val + sf->GetBinContent(bin));
  }
  tmpHist->SetDirectory(0);
  return tmpHist;
}
/*
 * Build the toyMC tables from inputs
 */
std::vector<TObjArray>
Root::TElectronEfficiencyCorrectionTool::buildToyMCTable(const TObjArray& sf, 
                                                         const TObjArray& eig, 
                                                         const TObjArray& stat,
                                                         const TObjArray& uncorr, 
                                                         const std::vector<TObjArray>& corr) {

  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ")\n" 
                << "Entering function buildToyMCTable");

  int nSys{};
  int randomCounter(0);
  std::vector<TObjArray> tmpVec;
  const int stat_entries = stat.GetEntries();
  if (m_doCombToyMC) {
    for (int toyMC = 0; toyMC < m_nToyMC; toyMC++) {
      TObjArray tmpArray;
      for (int i = 0; i < stat_entries; ++i) {
        if (eig.GetEntriesFast()>0 && uncorr.GetEntriesFast()>0) { 
          nSys = ((TH1*)eig.At(i))->GetNbinsX()-1;  
          tmpArray.Add(buildSingleCombToyMC((TH2 *) sf.At(i), 
                                            (TH2 *) stat.At(i), 
                                            (TH2 *) uncorr.At(i), 
                                            corr.at(i),
                                            nSys,
                                            randomCounter));
        }else {
          tmpArray.Add(buildSingleCombToyMC((TH2*) sf.At(i), 
                                            (TH2*) stat.At(i), 
                                            0, 
                                            corr.at(i) ,
                                            nSys,
                                            randomCounter));
        }
      }
      tmpVec.push_back(tmpArray);
    }
  }else {
    std::vector< std::vector<TH2*> > tmpVec2 ;
    for (int i = 0; i < stat_entries; ++i) {
      nSys = ((TH1*)eig.At(i))->GetNbinsX()-1;
      tmpVec2.push_back(buildSingleToyMC((TH2*) sf.At(i), 
                                         (TH2*) stat.At(i), 
                                         (TH2*) uncorr.At(i),
                                         corr.at(i),
                                         randomCounter));
    }
    for (int toy = 0; toy < m_nToyMC; toy++) {
      TObjArray tmpArray;
      for (unsigned int i = 0; i < tmpVec2.size(); ++i) {
        tmpArray.Add(tmpVec2.at(i).at(toy));
      }
      tmpVec.push_back(tmpArray);
    }
  }
  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ")\n" 
                << "m_Rndm->Uniform(0, 1) after throwing " << randomCounter
                << " random numbers: " << m_Rndm.Uniform(0,1));

  return tmpVec;
}
/*
 * Helper function to retrieve number of uncorrelated bins
 */
int
Root::TElectronEfficiencyCorrectionTool::getNbins(std::map<float, std::vector<float> > &pt_eta1) const {
  //Get sf histograms 
  const std::vector<TObjArray >& tmpVec = m_histList.at(mapkey::sf);
  int nbinsTotal = 0;
  pt_eta1.clear();
  std::vector<float>eta1;
  eta1.clear();

  //Loop over the different Run range (one TObjeArray for each)
  for (unsigned int ikey = 0; ikey < tmpVec.size(); ++ikey) {
    //Loop over the histograms for a given run numbers
    for (int entries = 0; entries < (tmpVec.at(ikey)).GetEntries(); ++entries) {
      eta1.clear();
      //Get number of bins
      TH2 *h_tmp = ((TH2 * ) (tmpVec.at(ikey)).At(entries));
      int nbinsX = h_tmp->GetNbinsX();
      int nbinsY = h_tmp->GetNbinsY();
      //fill in the eta pushing back
      for (int biny = 1; biny <= nbinsY; ++biny) {
        eta1.push_back(h_tmp->GetYaxis()->GetBinLowEdge(biny));
      }
      //associate each pt (bin) with the corresponding/available eta ones
      for (int binx = 1; binx <=nbinsX; ++binx) {
        pt_eta1[h_tmp->GetXaxis()->GetBinLowEdge(binx)] = eta1;
      }
    }
  }
  for (auto &i : pt_eta1) {
    nbinsTotal += i.second.size();
  }
  return nbinsTotal;
}
/*
 * Get the  histograms from the input files
 */
int Root::TElectronEfficiencyCorrectionTool::getHistograms() {

  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ")\n" 
                << "Entering function getHistograms");
  // Cache the current directory in the root file
  TDirectory *origDir = gDirectory;
  /*
   * Get the name of the first input ROOT file and
   * interpret from that what we have:
   * efficiency vs. efficiencySF; offline vs. trigger; medium, loose,...
   */
  if (!m_corrFileNameList.empty()) {
    TString firstFileNameAndPath = m_corrFileNameList[0].c_str();
    std::unique_ptr<TObjArray> myStringList(firstFileNameAndPath.Tokenize("/"));
    int lastIdx = myStringList->GetLast();
    TString fileName = ((TObjString *) myStringList->At(lastIdx))->GetString();
    std::unique_ptr<TObjArray> myFileNameTokensList(fileName.Tokenize("."));

    if (myFileNameTokensList->GetLast() < 3) {
      ATH_MSG_ERROR("input file name has wrong format!");
      return 0;
    }
  }
  /*
   * Get all ROOT files and histograms
   */
  for (unsigned int i = 0; i < m_corrFileNameList.size(); ++i) {
    // Load the ROOT file
    const std::unique_ptr<char[]> fname (gSystem->ExpandPathName(m_corrFileNameList[i].c_str()));
    std::unique_ptr<TFile> rootFile( TFile::Open(fname.get(), "READ") );
    if (!rootFile) {
      ATH_MSG_ERROR( "No ROOT file found here: " <<m_corrFileNameList[i]);
      return 0;
    }
    // Loop over all directories inside the root file (correspond to the run number ranges
    TIter nextdir(rootFile->GetListOfKeys());
    TKey *dir=nullptr;
    TObject *obj=nullptr;
    while ((dir = (TKey *) nextdir())) {
      obj = dir->ReadObj();
      if (obj->IsA()->InheritsFrom("TDirectory")) {
        // splits string by delimiter --> e.g RunNumber1_RunNumber2
        std::unique_ptr<TObjArray> dirNameArray(TString(obj->GetName()).Tokenize("_"));
        // returns index of last string --> if one, the directory name does not contain any run numbers
        int lastIdx = dirNameArray->GetLast();
        if (lastIdx != 1) {
          ATH_MSG_ERROR("The folder name seems to have the wrong format! Directory name:"<< obj->GetName());
          return 0;
        }
        rootFile->cd(obj->GetName());
        if (0 == this->setupHistogramsInFolder(*dirNameArray, lastIdx)) {
          ATH_MSG_ERROR("Unable to setup the histograms in directory " << dir->GetName()
                        << "in file " << m_corrFileNameList[i]);
          return 0;
        }
      }else {
        ATH_MSG_ERROR( "Wrong file content! Expected only Directories " <<
                      gDirectory->cd());
        return 0;
      }
      // Return to the original ROOT directory
      gDirectory = origDir;
    } // End: directory loop
  } // End: file loop
  return 1;
}
/*
 * Get the input histograms from a given folder/run number range
 */
int Root::TElectronEfficiencyCorrectionTool::setupHistogramsInFolder(const TObjArray& dirNameArray, int lastIdx) {

  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ")\n" 
                << "Entering funtion setupHistogramsInFolder");

  int runNumBegin(-1);
  TString myBegRunNumString = ( (TObjString*) dirNameArray.At(lastIdx - 1) )->GetString();
  if (myBegRunNumString.IsDigit()) {
    runNumBegin = myBegRunNumString.Atoi();
  }
  int runNumEnd(-1);
  TString myEndRunNumString = ( (TObjString *) dirNameArray.At(lastIdx) )->GetString();
  if (myEndRunNumString.IsDigit()) {
    runNumEnd = myEndRunNumString.Atoi();
  }
  if (runNumBegin < 0 || runNumEnd < 0 || runNumEnd < runNumBegin) {
    ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ")\n" 
                  << "Could NOT interpret the run number range: " << runNumBegin
                  << " - " << runNumEnd);
    return 0;
  }
  /// setup pairs of obj arrays and keys --> e.g. "sf", new Array to take all SF Histos
  std::unordered_map<int, TObjArray> objsFull;
  std::unordered_map<int, TObjArray > objsFast;
  for (unsigned int ikey = 0; ikey < s_keys.size(); ++ikey) {
    TObjArray dummyFull;
    objsFull.insert(std::make_pair(s_keys.at(ikey),dummyFull));
    TObjArray dummyFast;
    objsFast.insert(std::make_pair(s_keys.at(ikey),dummyFast));
  }  
  TObjArray dummyFull;    
  objsFull.insert(std::make_pair(mapkey::sys, dummyFull));
  TObjArray dummyFast;
  objsFast.insert(std::make_pair(mapkey::sys, dummyFast));

  std::vector<TObjArray > sysObjsFull;
  std::vector<TObjArray > sysObjsFast;

  TIter nextkey(gDirectory->GetListOfKeys());
  TKey *key=nullptr;
  TObject *obj=nullptr;
  int seenSystematics= 0;

  //Loop of the keys 
  while ((key = (TKey *) nextkey())) {
    obj = key->ReadObj();
    if (obj->IsA()->InheritsFrom("TH1")) {
      // The histogram containing the scale factors need to end with _sf and need to contain either the string "FullSim"
      // or "AtlFast2"!
      if (TString(obj->GetName()).Contains("FullSim")) {
        setupTempMapsHelper( obj,objsFull,sysObjsFull, seenSystematics); 
      }
      else if (TString(obj->GetName()).Contains("AtlFast2")) {
        setupTempMapsHelper( obj,objsFast,sysObjsFast, seenSystematics); 
      } else {
        ATH_MSG_ERROR( "Could NOT interpret if the histogram: " << obj->GetName()
                      << " is full or fast simulation!");
        return 0;
      }
    }
  }
  ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ")\n" 
                << "Setting up histograms for Run range  " <<
                runNumEnd);
  /*
   * Copy from the temporaries to the actual member variables
   * via the setup function
   */
  for (unsigned int ikey = 0; ikey < s_keys.size(); ++ikey) {
    if (objsFull.find(s_keys.at(ikey))->second.GetEntries() != 0) {
      if (0 == setup(objsFull.find(s_keys.at(ikey))->second, m_histList[s_keys.at(ikey)], 
                     m_begRunNumberList,m_endRunNumberList,runNumBegin,runNumEnd)) {
        ATH_MSG_ERROR("! Could NOT setup histogram " 
                      << s_keys.at(ikey)<< " for full sim!");
        return 0;
      }
    }
    if (objsFast.find(s_keys.at(ikey))->second.GetEntries() != 0) {
      if (0 == setup(objsFast.find(s_keys.at(ikey))->second, m_fastHistList[s_keys.at(ikey)],
                     m_begRunNumberListFastSim, m_endRunNumberListFastSim,runNumBegin,runNumEnd)) {
        ATH_MSG_ERROR("! Could NOT setup histogram " << s_keys.at(ikey)
                      << " for fast sim");
        return 0;
      }
    }
  }
  for (unsigned int sys = 0; sys < sysObjsFast.size(); sys++) {
    m_fastSysList.resize(sysObjsFast.size());
    if (0 == setup(sysObjsFast.at(sys), m_fastSysList[sys], m_begRunNumberListFastSim, 
                   m_endRunNumberListFastSim,runNumBegin,runNumEnd)) {
      ATH_MSG_ERROR("! Could NOT setup systematic histograms for fast sim");
      return 0;
    }
  }
  for (unsigned int sys = 0; sys < sysObjsFull.size(); sys++) {
    m_sysList.resize(sysObjsFull.size());
    if (0 == setup(sysObjsFull.at(sys), m_sysList[sys], m_begRunNumberList, 
                   m_endRunNumberList,runNumBegin,runNumEnd)) {
      ATH_MSG_ERROR("! Could NOT setup systematic histograms for fast sim");
      return 0;
    }
  }
  //Toys
  if (m_doToyMC || m_doCombToyMC) {
    bool fullToysBooked = setupUncorrToySyst(objsFull,sysObjsFull,m_uncorrToyMCSystFull);
    bool fastToysBooked = setupUncorrToySyst(objsFast,sysObjsFast,m_uncorrToyMCSystFast); 
    if (fullToysBooked || fastToysBooked) {
      if (m_doToyMC) {
        ATH_MSG_DEBUG("Created tables for " << m_nToyMC << " ToyMC systematics ");
      }
      if (m_doCombToyMC) {
        ATH_MSG_DEBUG("Created tables for " << m_nToyMC << " combined ToyMC systematics ");
      }
    }
  }
  return 1;
}
/*
 * Helper for Setting up the temporary/intermediate maps to Key -> TObjecArray from the histos
 */
void Root::TElectronEfficiencyCorrectionTool::setupTempMapsHelper(TObject* obj, 
                                                                  std::unordered_map<int, TObjArray>& objs,
                                                                  std::vector<TObjArray >& sysObjs, 
                                                                  int& seenSystematics)  {
  //Add all except the correlated 
  for (unsigned int ikey = 0; ikey < s_keys.size(); ++ikey) {
    if (TString(obj->GetName()).EndsWith("_" +  TString(mapkey::keytostring(s_keys.at(ikey))))) {
      objs.find(s_keys.at(ikey))->second.Add(obj);
    }
  }

  const TString tmpName(obj->GetName());
  //Special treatment , this is only for photons 
  if (tmpName.EndsWith("_sys")) {
    objs.find(mapkey::sys)->second.Add(obj);
    TObjArray tmpArray;
    tmpArray.Add(obj);
    sysObjs.push_back(tmpArray);
    seenSystematics++;
  }

  //See if we are dealing with correlated
  if (tmpName.Contains("_corr")) {
    /*
     * This is the worse part ...
     * corr0 triggers a few things
     * We assume that 0 is the 1st
     * histogram in a series of corr(i) that 
     * we see for each of the vector entries that 
     * can be one for LowPt,Standard,Forward etc
     */
    if (tmpName.EndsWith("corr0")) {
      /*
       * 1st create a TObjectArray
       */
      TObjArray tmpArray;
      /* 
       * Register it to the vector
       */
      sysObjs.push_back(tmpArray);
      /*
       * Reset the counter here
       */
      seenSystematics=0;
    }
    /*
     * Now we can add to the TObjeArray
     * This can be Low Pt or high Pt
     */
    sysObjs.back().Add(obj);
    /*Increase the counter*/
    seenSystematics++;
  }

  if (seenSystematics > m_nSysMax) {
    m_nSysMax = seenSystematics;
  }
}
/*
 * Helper for Setting up the uncorrelated syst for the toys
 */
bool Root::TElectronEfficiencyCorrectionTool::setupUncorrToySyst(std::unordered_map<int, TObjArray>& objs,
                                                                 std::vector<TObjArray>& sysObjs,
                                                                 std::vector< std::vector<TObjArray>>& uncorrToyMCSyst){
  bool toysBooked = false;
  if (m_histList[mapkey::sf].size() > 0) {
    if (objs.find(mapkey::eig)->second.GetEntries() < 1 || objs.find(mapkey::stat)->second.GetEntries() < 1 ||
        objs.find(mapkey::uncorr)->second.GetEntries() < 1) {

      if (objs.find(mapkey::stat)->second.GetEntries() > 1 || objs.find(mapkey::sys)->second.GetEntries() > 1) {

        TObjArray dummy;
        uncorrToyMCSyst.push_back(buildToyMCTable(objs.find(mapkey::sf)->second, 
                                                  dummy,
                                                  objs.find(mapkey::stat)->second,
                                                  dummy, 
                                                  sysObjs));
        toysBooked = true;
      }else {
        ATH_MSG_DEBUG("! Toy MC error propagation booked, but not all needed" 
                      <<"Histograms found in the output (For full sim). Skipping toy creation!");
      }
    }else {
      uncorrToyMCSyst.push_back(buildToyMCTable(objs.find(mapkey::sf)->second, 
                                                objs.find(mapkey::eig)->second,
                                                objs.find(mapkey::stat)->second,
                                                objs.find(mapkey::uncorr)->second, 
                                                sysObjs));
      toysBooked = true;
    }
  }

  return toysBooked;
}

/*
 * Fill and interpret the setup, depending 
 * on which histograms are found in the input file(s)
 */
int
Root::TElectronEfficiencyCorrectionTool::setup(const TObjArray& hists,
                                               std::vector< TObjArray> &histList,
                                               std::vector< unsigned int > &beginRunNumberList,
                                               std::vector< unsigned int > &endRunNumberList,
                                               const int runNumBegin,
                                               const int runNumEnd) const {
  if (hists.GetEntriesFast()==0) {
    ATH_MSG_ERROR( "! Could NOT find histogram with name *_sf in folder");
    return 0;
  }
  TH1 *tmpHist(0);
  for (int i = 0; i < hists.GetEntries(); ++i) {
    tmpHist = (TH1 *) hists.At(i);
    tmpHist->SetDirectory(0);
  }
  // Now, we have all the needed info. Fill the vectors accordingly
  histList.push_back(hists);
  if (beginRunNumberList.size() > 0) {
    if (runNumBegin != (int) beginRunNumberList.back()) {
      beginRunNumberList.push_back(runNumBegin);
    }
  }else {
    beginRunNumberList.push_back(runNumBegin);
  }
  if (endRunNumberList.size() > 0) {
    if (runNumEnd != (int) endRunNumberList.back()) {
      endRunNumberList.push_back(runNumEnd);
    }
  }else {
    endRunNumberList.push_back(runNumEnd);
  }
  return 1;
}
