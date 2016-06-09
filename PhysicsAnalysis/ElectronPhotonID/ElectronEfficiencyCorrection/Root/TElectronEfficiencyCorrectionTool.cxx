/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class TElectronEfficiencyCorrectionTool
   @brief Calculate the egamma scale factors in pure ROOT

   @author Karsten Koeneke, Felix Buehrer
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


//=============================================================================
// Constructor
//=============================================================================
Root::TElectronEfficiencyCorrectionTool::TElectronEfficiencyCorrectionTool(const char* name) :
  Root::TCalculatorToolBase(name),
  asg::AsgMessaging(std::string(name)),
  m_Rndm(0),
  m_randomCounter(0),
  m_isInitialized(false),
  m_detailLevel(0),
  m_toyMCSF(-1),
  m_seed(0),
  m_doToyMC(false),
  m_doCombToyMC(false),
  m_nToyMC(0),
  m_uncorrToyMCSystFull(0),
  m_uncorrToyMCSystFast(0),
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
  m_position_globalBinNumber(0),
  objsFull(0),
  objsFast(0),
  sysObjsFull(0),
  sysObjsFast(0),
  m_runnumberIndex(-1),
  m_last_runnumber(0),
  m_last_dataType(PATCore::ParticleDataType::Data),
  m_last_hist(0),
  m_last_hists(0)
{
}

//=============================================================================
// Destructor
//=============================================================================

Root::TElectronEfficiencyCorrectionTool::~TElectronEfficiencyCorrectionTool()
{
  //C.A delete all pointers to maps and the things contained
  if (objsFull){
    delete objsFull;
  }

  if (objsFast){
    delete objsFast;
  }
    
  if (sysObjsFull) {
    delete sysObjsFull;
  }
  if (sysObjsFast) {
    delete sysObjsFast;
  }
  //These point to same things as the ones above, prb is OK not to touch them here
  if (m_last_hist)  {
    delete m_last_hist;
  }
  if (m_last_hists) {
    delete m_last_hists;
  }
  if (m_Rndm) {
    delete m_Rndm;
  }
  
  for (unsigned int ikey=0; ikey<m_keys.size(); ++ikey){
   
    std::map<TString, std::vector< TObjArray* > >::iterator tempit;
  
    tempit = m_histList.find(TString(m_keys.at(ikey)));

    if ( tempit != m_histList.end()) {
      for (unsigned int i=0; i< tempit->second.size();++i){
	tempit->second.at(i)->SetOwner(kTRUE);
	delete tempit->second.at(i);
      }
    }
   
    tempit = m_fastHistList.find(TString(m_keys.at(ikey)));
    if ( tempit != m_fastHistList.end()) {
      for (unsigned int i=0; i< tempit->second.size();++i){
	tempit->second.at(i)->SetOwner(kTRUE);
	delete tempit->second.at(i);
      }
    }
  }

}

//=============================================================================
// Calculate the detail levels for a given eigenvector histogram
//=============================================================================
void Root::TElectronEfficiencyCorrectionTool::calcDetailLevels(TH1D *eig)
{
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


//=============================================================================
// Build the toyMC tables from inputs
//=============================================================================
std::vector<TH2D*> *Root::TElectronEfficiencyCorrectionTool::buildSingleToyMC(TH2D* sf, TH2D* stat, TH2D* uncorr, TObjArray *corr)
{
  ATH_MSG_DEBUG(this->getName() << "entering function buildSingleToyMC");

  std::vector<TH2D*> *tmpHists = new std::vector<TH2D*>;
  int nBins = (stat->GetNbinsX() + 2) * (stat->GetNbinsY() + 2);
  for (int toy = 0; toy < m_nToyMC; toy++) {
    tmpHists->push_back((TH2D*)corr->At(0)->Clone());
  }
  // Loop over all bins
  for (int bin = 0; bin < nBins; bin++) {

    double val = stat->GetBinContent(bin);

    // Add uncorrelated systematics
    if (uncorr != NULL) {
      double valAdd = uncorr->GetBinContent(bin);
      val = sqrt(val * val + valAdd * valAdd);
    }
    
    // Add smaller correlated systematics as uncorrelated
    for (int i = 0; i < m_sLevel[m_detailLevel]; i++) {
      if (corr->At(i) != NULL) {
	double valAdd = ((TH2D*)corr->At(i))->GetBinContent(bin);
	val = sqrt(val * val + valAdd * valAdd);
      }
    }
    for (int toy = 0; toy < m_nToyMC; toy++) {
      tmpHists->at(toy)->SetBinContent(bin, (val * m_Rndm->Gaus(0, 1)) + sf->GetBinContent(bin));
      m_randomCounter++;
      tmpHists->at(toy)->SetDirectory(0);
    }
  }

  return tmpHists;
}


//=============================================================================
// Build the combined toyMC tables from inputs
//=============================================================================
TH2D *Root::TElectronEfficiencyCorrectionTool::buildSingleCombToyMC(TH2D* sf, TH2D* stat, TH2D* uncorr, TObjArray *corr)
{
  ATH_MSG_DEBUG(this->getName() << "entering function buildSingleCombToyMC");

  TH2D* tmpHist;
  int nBins = (stat->GetNbinsX() + 2) * (stat->GetNbinsY() + 2);
  tmpHist = (TH2D*)corr->At(0)->Clone();


  // Create random numbers for the corr. uncertainties
  double *rnd = new double[m_nSys];
  for (int s = 0; s < m_nSys; s++) {
    rnd[s] = m_Rndm->Gaus(0, 1);
    m_randomCounter++;
  }

  // Loop over all bins
  for (int bin = 0; bin < nBins; bin++) {

    double val = stat->GetBinContent(bin);


    // Add uncorrelated systematics
    if (uncorr != NULL) {
      double valAdd = uncorr->GetBinContent(bin);
      val = sqrt(val * val + valAdd * valAdd);
    }
    // Add smaller correlated systematics as uncorrelated

    for (int s = 0; s < m_sLevel[m_detailLevel]; s++) {

      if (corr->At(s) != NULL) {
	double valAdd = ((TH2D*)corr->At(s))->GetBinContent(bin);
	val = sqrt(val * val + valAdd * valAdd);
      }
    }

    val = val * m_Rndm->Gaus(0, 1);
    m_randomCounter++;

    // Add larger correlated systematics
    for (int s = m_sLevel[m_detailLevel]; s < m_nSys; s++) {

      if (corr->At(s) != NULL) {
	val += ((TH2D*)corr->At(s))->GetBinContent(bin) * rnd[s];
      }
    }

    tmpHist->SetBinContent(bin, val + sf->GetBinContent(bin));
  }

  tmpHist->SetDirectory(0);

  delete[] rnd;
  return tmpHist;
}


//=============================================================================
// Build the toyMC tables from inputs
//=============================================================================
std::vector<TObjArray*> *Root::TElectronEfficiencyCorrectionTool::buildToyMCTable(TObjArray *sf, TObjArray* eig, TObjArray* stat, TObjArray* uncorr, std::vector<TObjArray*> *corr)
{
  ATH_MSG_DEBUG(this->getName() << "entering function buildToyMCTable");

  std::vector<TObjArray*> *tmpVec = new std::vector<TObjArray*>;
  TObjArray *tmpArray;
  if (m_doCombToyMC)
    {
      for (int toyMC=0; toyMC<m_nToyMC; toyMC++)
	{
	  tmpArray = new TObjArray();
	  for (int i=0; i<stat->GetEntries(); i++)
	    {
	      if (eig!=NULL && uncorr!=NULL) {
		calcDetailLevels((TH1D*)eig->At(i));
		tmpArray->Add(buildSingleCombToyMC((TH2D*)sf->At(i), (TH2D*)stat->At(i),(TH2D*)uncorr->At(i),corr->at(i)));
	      }
	      else {
		tmpArray->Add(buildSingleCombToyMC((TH2D*)sf->At(i), (TH2D*)stat->At(i),NULL,corr->at(i)));
	      }
	    }
	  tmpVec->push_back(tmpArray);
	}
    }
  else
    {
      std::vector< std::vector<TH2D*> *> *tmpVec2 = new  std::vector< std::vector<TH2D*> *>;
      for (int i=0; i<stat->GetEntries(); i++)
	{
	  calcDetailLevels((TH1D*)eig->At(i));
	  tmpVec2->push_back(buildSingleToyMC((TH2D*)sf->At(i), (TH2D*)stat->At(i),(TH2D*)uncorr->At(i),corr->at(i)));
	}
      for (int toy=0; toy<m_nToyMC; toy++)
	{
	  tmpArray = new TObjArray();
	  for (unsigned int i=0; i<tmpVec2->size(); i++)
	    {
	      tmpArray->Add(tmpVec2->at(i)->at(toy));
	    }
	  tmpVec->push_back(tmpArray);
	}
      delete tmpVec2;
    }

  ATH_MSG_DEBUG(this->getName() 
		<< " (file: " << __FILE__ << ", line: " << __LINE__ << ")! "
		<< "m_Rndm->Uniform(0, 1) after throwing "<< m_randomCounter << " random numbers: " << m_Rndm->Uniform(0, 1));
  return tmpVec;

}

//=============================================================================
// Initialize this correction tool
//=============================================================================
int Root::TElectronEfficiencyCorrectionTool::initialize()
{
  // use an int as a StatusCode
  int sc(1);

  ATH_MSG_DEBUG( this->getName() 
		 << " (file: " << __FILE__ << ", line: " << __LINE__ << ")! "
		 <<"Debug flag set. Printing verbose output!");
  
  if (m_isInitialized) {
    ATH_MSG_ERROR( this->getName() 
		   << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		   << "! tool initialized twice!");
    return 0;
  }

  if (m_corrFileNameList.size() == 0) {
    ATH_MSG_ERROR( this->getName() 
		   << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		   << "! No file added!" );
    return 0;
  }

  ATH_MSG_INFO (this->getName() 
		<< " (file: " << __FILE__ << ", line: " << __LINE__ << ")! "
		<< "Initializing tool with " << m_corrFileNameList.size() << " configuration file(s)");

  //Check if the first file can be opened (needed for auto-setting of the seed based on the md5-sum of the file)
  const char* fname;
  fname = gSystem->ExpandPathName( m_corrFileNameList[0].c_str() );
  TFile* rootFile = TFile::Open( fname, "READ" );
  if ( !rootFile ){
    ATH_MSG_ERROR( this->getName() 
		   << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		   << "! No ROOT file found here: " << m_corrFileNameList[0] );
    return 0;
  }
  rootFile->Close();
  delete rootFile;

  if (m_doToyMC && m_doCombToyMC) {
    ATH_MSG_ERROR(this->getName() 
		  << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		  << "! Both regular and combined toy MCs booked!"
		  << " Only use one!");
    return 0;
  }

  m_keys.push_back("sf");
  m_keys.push_back("stat");
  m_keys.push_back("eig");
  m_keys.push_back("uncorr");
  
  m_last_runnumber = 0;
  m_last_dataType = PATCore::ParticleDataType::Data;
  m_last_hist = 0;
  m_last_hists = 0;


  //initialize the random number generated if toyMC propagation booked
  if (m_doToyMC || m_doCombToyMC) {
    if (m_seed==0) {
      TMD5 *tmd = new TMD5();
      m_seed = *reinterpret_cast<const int*>(tmd->FileChecksum(fname)->AsString());
      ATH_MSG_INFO (this->getName() << " (file: " << __FILE__ << ", line: " << __LINE__ << ")! "
		    << "Seed (automatically) set to "<< m_seed);
    }
    else { 
      ATH_MSG_INFO (this->getName() << " (file: " << __FILE__ << ", line: " << __LINE__ << ")! "
		    <<"Seed set to "<< m_seed);
    }
    m_Rndm = new TRandom3(m_seed);
  }

  // Load the needed histograms
  if ( 0 == this->getHistograms() )
    {
      ATH_MSG_ERROR(this->getName() 
		    << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		    << "! Problem when calling getHistograms()!");
      return 0;
    }
  unsigned int nRunNumbersFull = m_begRunNumberList.size();
  unsigned int nRunNumbersFast = m_begRunNumberListFastSim.size();
  

  ATH_MSG_DEBUG(this->getName() 
		<< " (file: " << __FILE__ << ", line: " << __LINE__ << ")! "
		"Found "<< nRunNumbersFast << " run number ranges for fast sim with a total of " <<m_fastHistList["sf"].size()<<" scale factor histograms.");
  
  ATH_MSG_DEBUG(this->getName() 
		<< " (file: " << __FILE__ << ", line: " << __LINE__ << ")! "
		<< "Found "<< nRunNumbersFull << " run number ranges for full sim with a total of " <<m_histList["sf"].size()<<" scale factor histograms.");

  // --------------------------------------------------------------------------
  // Register the cuts and check that the registration worked:
  // NOTE: THE ORDER IS IMPORTANT!!! Cut0 corresponds to bit 0, Cut1 to bit 1,...
  m_position_eff = m_result.addResult( (m_resultPrefix+m_resultName).c_str(), "efficiency scale factor" );
  if ( m_position_eff < 0 ) sc = 0; // Exceeded the number of allowed results

  m_position_err = m_result.addResult( (m_resultPrefix+m_resultName+"_err").c_str(), "efficiency scale factor uncertainty" );
  if ( m_position_err < 0 ) sc = 0; // Exceeded the number of allowed results

  m_position_statErr = m_result.addResult( (m_resultPrefix+m_resultName+"_statErr").c_str(), "efficiency scale factor statistical uncertainty" );
  if ( m_position_statErr < 0 ) sc = 0; // Exceeded the number of allowed results

  m_position_nSys = m_result.addResult( (m_resultPrefix+m_resultName+"_nSys").c_str(), "number of corrrelated systematic uncertainties (dependent of detailLevel)" );
  if ( m_position_nSys < 0 ) sc = 0; // Exceeded the number of allowed results

  m_position_uncorrSys = m_result.addResult( (m_resultPrefix+m_resultName+"_uncorrSys").c_str(), "total uncorrelated uncertainty" );
  if ( m_position_uncorrSys < 0 ) sc = 0; // Exceeded the number of allowed results

  for (int sys=0; sys<m_nSysMax; sys++){
    m_position_corrSys.push_back(m_result.addResult( (m_resultPrefix+m_resultName+"_corrSys_"+toString(sys)).c_str(), "correlated uncertainty number "+toString(sys + 1) ));
    if ( m_position_corrSys.at(sys) < 0 ) sc = 0; // Exceeded the number of allowed results
    m_result.setResult( m_position_corrSys.at(sys), 0  );
  }

  for (int toy=0; toy<m_nToyMC; toy++){
    m_position_uncorrToyMCSF.push_back(m_result.addResult( (m_resultPrefix+m_resultName+"_uncorrToyMCSF_"+toString(toy)).c_str(), "Toy MC scale factor number "+toString(toy) ) );
    if ( m_position_uncorrToyMCSF.at(toy) < 0 ) sc = 0; // Exceeded the number of allowed results
    m_result.setResult( m_position_uncorrToyMCSF.at(toy), 0  );
  }

  m_position_globalBinNumber = m_result.addResult( (m_resultPrefix+m_resultName+"_globalBinNumber").c_str(), "bin position in histograms" );
  if ( m_position_globalBinNumber < 0 ) sc = 0; // Exceeded the number of allowed results


  // Set the results to default values
  m_result.setResult( m_position_eff, 1.0  );
  m_result.setResult( m_position_err, 1.0  );
  m_result.setResult( m_position_statErr, 0.0  );
  m_result.setResult( m_position_nSys, 0  );
  m_result.setResult( m_position_uncorrSys, 0  );
  m_isInitialized = kTRUE;
  
  ATH_MSG_INFO(this->getName() 
	       << " (file: " << __FILE__ << ", line: " << __LINE__ << ")! "
	       << "Tool succesfully initialized!");
  return sc;
}

//=============================================================================
// Calculate the actual accept of each cut individually.
//=============================================================================
const Root::TResult& Root::TElectronEfficiencyCorrectionTool::calculate( const PATCore::ParticleDataType::DataType dataType,
                                                                         const unsigned int runnumber,
                                                                         const double cluster_eta,
                                                                         const double et /* in MeV */
                                                                         )
{
  ATH_MSG_DEBUG(this->getName() 
		<< "  entering function calculate" );

  //check if a file is given and tool correctly initialized
  if ( !m_isInitialized ) {
    ATH_MSG_ERROR( this->getName() 
		   << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		   << " Tool not correctly initialized or no scale factor file given. "
		   << " Returning 1 +- 1!");
    return m_result;
  }
   
  ATH_MSG_DEBUG( this->getName() << " n Systematics: " << m_nSysMax );
  // Reset the results to default values
  m_result.setResult( m_position_eff, 1.0  );
  m_result.setResult( m_position_err, 1.0  );
  m_result.setResult( m_position_statErr, 0.0  );
  m_result.setResult( m_position_uncorrSys, 0  );
  for (int sys=0; sys<m_nSysMax; sys++){
    m_result.setResult( m_position_corrSys.at(sys), 0  );
  }
  for (int toy=0; toy<m_nToyMC; toy++){
    m_result.setResult( m_position_uncorrToyMCSF.at(toy), 0  );
  }
  m_result.setResult( m_position_globalBinNumber, 0  );
  // If we have real data, we don't need to scale anything and return sf=1.0+-0.0
  if ( dataType == PATCore::ParticleDataType::Data ){
    m_result.setResult( m_position_err, 0.0  );
    return m_result;
  }

  // Check if fast shower wanted and abort (not available yet)
  if ( dataType == PATCore::ParticleDataType::FastShower )
    {
      printDefaultReturnMessage("Fast shower scale factors not available. Please check your data type", __LINE__);
      return m_result;
    }

  bool isFastSim(false);
  if ( dataType == PATCore::ParticleDataType::Fast )
    {
      isFastSim = true;
    }
  bool sameHistos = kFALSE;
  if (m_last_runnumber == runnumber && m_last_dataType == dataType) sameHistos = kTRUE;
  m_last_runnumber = runnumber;
  m_last_dataType = dataType;


  std::map<std::string, const TObjArray*> *thisHists;
  std::map<std::string, const TH1*> *thisHist;

  //Only find histogram if not the same as last call
  if (sameHistos){
    thisHist = m_last_hist;
    thisHists = m_last_hists;
  }

  else
    {

      // Find the right run number range and the corresponding histogram
      thisHists = new std::map<std::string, const TObjArray*>;
      thisHist = new std::map<std::string,const TH1*>;
      std::vector<TObjArray*> tmpVec;
      if ( isFastSim )
	{
	  for ( unsigned int i=0; i<m_begRunNumberListFastSim.size(); ++i )
	    {
	      if ( m_begRunNumberListFastSim[i] <= runnumber && runnumber <= m_endRunNumberListFastSim[i] )
		{
		  for (unsigned int ikey=0; ikey<m_keys.size(); ++ikey){
		    tmpVec = m_fastHistList[m_keys.at(ikey)];
		    if (tmpVec.size() > 0) thisHists->insert(std::make_pair(m_keys.at(ikey), tmpVec.at(i)));
		  }
		  m_runnumberIndex = i;
		  break;
		}
	    }
	}
      else
	{
	  for ( unsigned int i=0; i<m_begRunNumberList.size(); ++i )
	    {
	      if ( m_begRunNumberList[i] <= runnumber && runnumber <= m_endRunNumberList[i] )
		{
		  for (unsigned int ikey=0; ikey<m_keys.size(); ++ikey){
		    tmpVec = m_histList[m_keys.at(ikey)];
		    if ( tmpVec.size() > 0 && i < tmpVec.size() ) thisHists->insert(std::make_pair(m_keys.at(ikey), tmpVec.at(i)));
		  }
		  m_runnumberIndex = i;
		  break;
		}
	    }
	}

      if (m_last_hist)  delete m_last_hist;
      if (m_last_hists) delete m_last_hists;
      m_last_hist = thisHist;
      m_last_hists = thisHists;
      // If no valid hist, then, run number was out of range
      if ( thisHists->find("sf") == thisHists->end() ){
	printDefaultReturnMessage(TString::Format("No valid histogram found for the current run number: %i for simulation type: %i",runnumber, dataType), __LINE__);
	return m_result;
      }
    }

  // Get the actual scale factor and its uncertainty from the current histogram
  double xValue,yValue;
  xValue = et;
  yValue = cluster_eta;
  int smallEt(0), etaCov(0), nSF(0);

  bool block = kFALSE;
  bool changed = kFALSE;
  bool isLowPt;
  int index = -1;
  TH2* tmpHist(NULL);
  if (thisHists->find("sf") == thisHists->end())
    {
      printDefaultReturnMessage("Could not find a histogram that ends with _sf, please check your input file!",__LINE__);
      return m_result;
    }
  for ( int i=0; i<(thisHists->find("sf"))->second->GetEntries(); i++){
    isLowPt = kFALSE;
    block = kFALSE;
    tmpHist = (TH2*)(thisHists->find("sf")->second->At(i));
    if (TString(tmpHist->GetName()).Contains("LowPt"))
      isLowPt = kTRUE;
    block = kFALSE;
    if (et < tmpHist->GetXaxis()->GetXmin()){
      smallEt++;
      block = kTRUE;
    }

    if (TMath::Abs(yValue) >= tmpHist->GetYaxis()->GetXmax()){
      etaCov++;
      block = kTRUE;
    }

    // this is for forward electrons (eta-histos start at eta=2.47
    if (TMath::Abs(yValue) < tmpHist->GetYaxis()->GetXmin()){
      etaCov++;
      block = kTRUE;
    }

    if (et > tmpHist->GetXaxis()->GetXmax() )
      {
	if (isLowPt)
	  block = kTRUE;
	else
	  {
	    xValue = tmpHist->GetXaxis()->GetBinCenter(tmpHist->GetNbinsX());
	    changed = kTRUE;
	  }
      }


    if (!block) {
      index = i;
      if (!changed) nSF++;
    }
  }

  if(index >= 0){
    if(thisHist->find("sf") == thisHist->end()) thisHist->insert(std::make_pair("sf", (TH2*)thisHists->find("sf")->second->At(index)));
    else thisHist->find("sf")->second = (TH2*)thisHists->find("sf")->second->At(index);
  }
  else {
    printDefaultReturnMessage(TString::Format("No correction factor provided because there was an index problem"),__LINE__);
    return m_result;
  }
  
  if (smallEt == thisHists->find("sf")->second->GetEntries())
    {
      printDefaultReturnMessage(TString::Format("No correction factor provided for et=%f",xValue),__LINE__);
      return m_result;
    }

  if (etaCov == thisHists->find("sf")->second->GetEntries())
    {
      printDefaultReturnMessage(TString::Format("No correction factor provided for eta=%f",yValue),__LINE__);
      return m_result;
    }

  if (nSF > 1){
    ATH_MSG_WARNING("More than 1 SF found for eta="<<yValue<<" , et = "<<et<<" , run number = "<<runnumber<<" . Please check your input files!");
  }
  
  // If SF is only given in Abs(eta) convert eta input to TMath::Abs()
  float epsilon = 1e-6;
  if (thisHist->find("sf")->second->GetYaxis()->GetBinLowEdge(1) >= 0-epsilon ){
    if (yValue < 0) {   
      ATH_MSG_DEBUG(this->getName() 
		    << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		    <<"Scale factor only measured in Abs(eta) changing eta from "<<yValue<<" to "<<TMath::Abs(yValue)); 
    }
    yValue = TMath::Abs(yValue);
  }

  int globalBinNumber = thisHist->find("sf")->second->FindFixBin( xValue, yValue );
  double scaleFactor = thisHist->find("sf")->second->GetBinContent( globalBinNumber );
  double scaleFactorErr = thisHist->find("sf")->second->GetBinError( globalBinNumber );
  
  // Write the retrieved values into the return object
  m_result.setResult( m_position_eff, scaleFactor );
  m_result.setResult( m_position_err, scaleFactorErr );

  std::map<std::string, const TObjArray*>::iterator tempit;
  double statErr = -999;

  //////////////////////////////

  tempit = thisHists->find("stat");
  if (tempit != thisHists->end()) {
    if (tempit->second->Last() != NULL){
      TH1* stat = (TH1*)tempit->second->At(index);
      statErr = stat->GetBinContent(globalBinNumber);
      m_result.setResult( m_position_statErr, statErr );
    }
  }

  tempit = thisHists->find("eig");
  if (tempit != thisHists->end()) {
    if (tempit->second->Last() != NULL){
      TH1* eig = (TH1*)tempit->second->At(index);
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
      m_result.setResult( m_position_nSys, nSys );
    }
  }

  std::vector< std::vector< TObjArray* > > *sysList;
  if (isFastSim) sysList = &m_fastSysList;
  else sysList = &m_sysList;
  std::vector<double> corrSys;
  if (sysList != 0){
    if( sysList->size()>(unsigned int)index){
      if(sysList->at(index).size()>(unsigned int)m_runnumberIndex){
        for (int sys = 0; sys<sysList->at(index).at(m_runnumberIndex)->GetEntries(); sys++){

	  tmpHist = (TH2*)sysList->at(index).at(m_runnumberIndex)->At(sysList->at(index).at(m_runnumberIndex)->GetEntries() -1 - sys);
	 
	  corrSys.push_back(tmpHist->GetBinContent(globalBinNumber));
	  m_result.setResult ( m_position_corrSys[sys], corrSys[sys] );

        }
      }
    }
  }
  if (m_position_corrSys.size()>0) {
    if (m_result.getResult ( m_position_corrSys[0] ) == 0  ) {
      m_result.setResult( m_position_corrSys[0], scaleFactorErr );

    }
  }
  statErr = -999;
  tempit = thisHists->find("stat");
  if (tempit != thisHists->end()) {
    if (tempit->second->Last() != NULL){
      TH1* stat = (TH1*)tempit->second->At(index);
      statErr = stat->GetBinContent(globalBinNumber);
    }
  }
  double val = statErr;
  tempit = thisHists->find("uncorr");
  if (tempit != thisHists->end()) {
    if (tempit->second->Last() != 0){
      TH1* uncorr = (TH1*)tempit->second->At(index);
      double valAdd = uncorr->GetBinContent(globalBinNumber);
      val = sqrt(val*val + valAdd*valAdd);
      for(int i=0; i<m_sLevel[m_detailLevel]; i++){
        double valAdd = corrSys.at(corrSys.size() - 1 - i);
        val = sqrt(val*val + valAdd*valAdd);
      }
    }
  }
  if (val == -999)
    val = 0;
  m_result.setResult( m_position_uncorrSys, val );
  
  if (m_doToyMC||m_doCombToyMC){
    std::vector< std::vector<TObjArray*> *> * toyMCList;
    if (isFastSim) toyMCList = m_uncorrToyMCSystFast;
    else toyMCList = m_uncorrToyMCSystFull;
    if (toyMCList != 0){
      if( toyMCList->size()>(unsigned int)m_runnumberIndex){
        for (int toy=0; toy<m_nToyMC; toy++){
	  if(toyMCList->at(m_runnumberIndex)->at(toy)->GetLast()>=index){
	    m_result.setResult(m_position_uncorrToyMCSF.at(toy), ((TH2*)toyMCList->at(m_runnumberIndex)->at(toy)->At(index))->GetBinContent(globalBinNumber));
	  }
	}
      }
    }
  }
  m_result.setResult( m_position_globalBinNumber, globalBinNumber );
  return m_result;
}
//=============================================================================
// Helper function to retrieve number of uncorrelated bins
//=============================================================================
int Root::TElectronEfficiencyCorrectionTool::getNbins(std::map<float, std::vector<float> > &pt_eta1)
{
  std::vector<TObjArray*> tmpVec;
  tmpVec = m_histList["sf"];
  int nbinsTotal=0;
  pt_eta1.clear();
  std::vector<float>eta1;
  eta1.clear();
 
  for (unsigned int ikey=0; ikey<tmpVec.size(); ++ikey){
    for (int entries=0; entries<(tmpVec.at(ikey))->GetEntries(); entries++) {
      eta1.clear();
 
      TH2D* h_tmp = ((TH2D* )(tmpVec.at(ikey))->At(entries));  
      int nbinsX=h_tmp->GetNbinsX();
      int nbinsY=h_tmp->GetNbinsY();

      for (int biny=1;biny<=nbinsY+1;biny++){
     	eta1.push_back(h_tmp->GetYaxis()->GetBinLowEdge(biny));
     	if (entries==(tmpVec.at(ikey))->GetEntries()-1) {eta1.push_back(h_tmp->GetYaxis()->GetBinLowEdge(biny+1));}
      }

      for (int binx=1;binx<=nbinsX;binx++){
	//  if (pt_eta1[(h_tmp->GetXaxis()->GetBinLowEdge(binx))]!=0) 
	pt_eta1[h_tmp->GetXaxis()->GetBinLowEdge(binx)] = eta1;

     	if (entries==(tmpVec.at(ikey))->GetEntries()-1) {
	  //  if (pt_eta1[(h_tmp->GetXaxis()->GetBinLowEdge(binx+1))]!=0) 
	pt_eta1[h_tmp->GetXaxis()->GetBinLowEdge(binx+1)] = eta1;
	}   	
      }
    }
  }
  
  for(auto& i : pt_eta1){
    nbinsTotal+=i.second.size();
  }
 
  return nbinsTotal;
}

//=============================================================================
// Helper function to retrieve the position of the first toy MC scale factor
//=============================================================================
int Root::TElectronEfficiencyCorrectionTool::getFirstToyMCPosition()
{
  if (!m_isInitialized){
    ATH_MSG_ERROR(this->getName() 
		  << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "   
		  << "! Tool not initialized.");
    return -1;
  }
  
  if (m_nToyMC>0){
    return m_position_uncorrToyMCSF.at(0);
  }
  else{
    ATH_MSG_ERROR(this->getName() 
		  << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "   
		  << "! No toy scale factors booked, returning the position of the scale factor! This is most likely not what you want!");
    return 0;
  }
}

int Root::TElectronEfficiencyCorrectionTool::getLastToyMCPosition(){
  if (!m_isInitialized){
    ATH_MSG_ERROR(this->getName() 
		  << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		  << "! Tool not initialized.");
    return -1;
  }

  if (m_nToyMC>0)
    return m_position_uncorrToyMCSF.at(m_nToyMC-1);
  else{
    ATH_MSG_ERROR(this->getName() 
		  << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		  << "! No toy scale factors booked, returning the position of the scale factor! This is most likely not what you want!");
    return 0;
  }
}

int Root::TElectronEfficiencyCorrectionTool::getFirstCorrSysPosition(){
  if (!m_isInitialized){
    ATH_MSG_ERROR(this->getName() 
		  << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		  << "! Tool not initialized.");
    return -1;
  }
  
  if (m_position_corrSys.size()>0){
    return m_position_corrSys.at(0);
  }
  else{
    ATH_MSG_ERROR(this->getName() 
		  << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		  << "! No correlated uncertainties found, returning the position of the scale factor! This is most likely not what you want!");
    return 0;
  }
}


int Root::TElectronEfficiencyCorrectionTool::getLastCorrSysPosition(){
  if (!m_isInitialized){
    ATH_MSG_ERROR(this->getName() 
		  << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		  << "! Tool not initialized.");
    return -1;
  }
  
  if (m_position_corrSys.size()>0){
    if (m_result.getResult(3) > 0){
      return m_position_corrSys.at(m_result.getResult(3)-1);
    }
    else{
      return m_position_corrSys.at(0);
    }
  }
  else{
    ATH_MSG_ERROR(this->getName() 
		  << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		  << "! No correlated uncertainties found, returning the position of the scale factor! This is most likely not what you want!");
    return 0;
  }
}

int Root::TElectronEfficiencyCorrectionTool::getGlobalBinNumberPosition()
{
  if (!m_isInitialized){
    ATH_MSG_ERROR(this->getName() 
		  << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		  << "! Tool not initialized.");
    return -1;
  }
  return m_position_globalBinNumber;
}

void Root::TElectronEfficiencyCorrectionTool::printResultMap()
{
  if (!m_isInitialized){
    ATH_MSG_ERROR(this->getName() 
		  << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		  << "! Tool not initialized.");
    return ;
  }
  
  ATH_MSG_INFO(this->getName() 
	       << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
	       << "!" <<std::endl <<"############"<<std::endl <<"Position  \t -\t Description ");
  
  for (unsigned int pos=0; pos<m_result.getNResults(); pos++){
    ATH_MSG_INFO(pos<<" \t \t - \t "<<m_result.getResultDescription(pos));
  }
}


//=============================================================================
// Get the input histograms from the input files
//=============================================================================
int Root::TElectronEfficiencyCorrectionTool::getHistograms()
{
  ATH_MSG_DEBUG(this->getName() << "entering function getHistograms");

  // Cache the current directory in the root file
  TDirectory* origDir = gDirectory;

  //-------------------------------------------------------
  // Get the name of the first input ROOT file and 
  // interpret from that what we have:
  // efficiency vs. efficiencySF; offline vs. trigger; medium, loose,...
  //-------------------------------------------------------
  if ( !m_corrFileNameList.empty() ){
    TString firstFileNameAndPath = m_corrFileNameList[0].c_str();
    TObjArray* myStringList = firstFileNameAndPath.Tokenize("/");
    int lastIdx      = myStringList->GetLast();
    TString fileName = ((TObjString*)myStringList->At(lastIdx))->GetString();
    TObjArray* myFileNameTokensList = fileName.Tokenize(".");
    if (myFileNameTokensList->GetLast() < 3){
      ATH_MSG_ERROR(this->getName() 
		    << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "	      
		    << "! input file name has wrong format!");
      return 0;
    }
      
    if ( m_resultPrefix.empty() ){ // Only overwrite it if the user didn't explicitly set it
      m_resultPrefix = ((TObjString*)myFileNameTokensList->At(0))->GetString();
      m_resultPrefix += "_";
    }
    if ( m_resultName.empty() ){ // Only overwrite it if the user didn't explicitly set it
      m_resultName = ((TObjString*)myFileNameTokensList->At(1))->GetString() 
	+ "_" + ((TObjString*)myFileNameTokensList->At(2))->GetString();
    }
    ATH_MSG_INFO(this->getName() 
		 << "Using resultPrefix: " << m_resultPrefix 
		 << " and resultName: " << m_resultName);


    delete myStringList;
    delete myFileNameTokensList;

  }

  //-------------------------------------------------------
  // Get all ROOT files and histograms
  //-------------------------------------------------------

  for ( unsigned int i=0; i<m_corrFileNameList.size(); ++i ){
    // Load the ROOT file
    const char* fname;
    fname = gSystem->ExpandPathName( m_corrFileNameList[i].c_str() );
    TFile* rootFile = TFile::Open( fname, "READ" );
    if ( !rootFile ){
      ATH_MSG_ERROR(this->getName() 
		    << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "	      
		    << "! No ROOT file found here: " << m_corrFileNameList[i]);;
      return 0;
    }
    
    if (m_doToyMC || m_doCombToyMC){
      m_uncorrToyMCSystFull = new std::vector< std::vector<TObjArray*>* >;
      m_uncorrToyMCSystFast = new std::vector< std::vector<TObjArray*> *>;
    }
    // Loop over all directories inside the root file (correspond to the run number ranges
    TIter nextdir(rootFile->GetListOfKeys());
    TKey *dir;
    TObject *obj;
    TObjArray* dirNameArray = 0;
    while ( (dir = (TKey*)nextdir()) ){          
      obj = dir->ReadObj();
      if ( obj->IsA()->InheritsFrom( "TDirectory" ) ){
	// splits string by delimiter --> e.g RunNumber1_RunNumber2
	dirNameArray = TString(obj->GetName()).Tokenize("_");
	//// returns index of last string --> if one, the directory name does not contain any run numbers
	int lastIdx = dirNameArray->GetLast();
	if ( lastIdx != 1 ){
	  ATH_MSG_ERROR(this->getName() 
			<< " (file: " << __FILE__ << ", line: " << __LINE__ << ") "	      
			<< "! The folder name seems to have the wrong format! Directory name:"
			<< obj->GetName());
	  return 0;
	}
	rootFile->cd(obj->GetName());
	if ( 0 == this->setupHistogramsInFolder(dirNameArray,lastIdx) ){
	  ATH_MSG_ERROR(this->getName() 
			<< " (file: " << __FILE__ << ", line: " << __LINE__ << ") "	      
			<< "unable to setup the histograms in directory " << dir->GetName()
			<< "in file " << m_corrFileNameList[i]);
	  return 0;
	}
      }
      else{
	ATH_MSG_ERROR(this->getName() 
		      << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "	      
		      << "Wrong file content! Expected only Directories " << gDirectory->cd());
	return 0;
      }
      // Return to the original ROOT directory
      gDirectory = origDir;
    } // End: directory loop
    delete dirNameArray;
    delete rootFile;
  } //End: file loop
   
  return 1;
}

//=============================================================================
// Get the input histograms from a given folder/run number range
//=============================================================================
int Root::TElectronEfficiencyCorrectionTool::setupHistogramsInFolder(TObjArray* dirNameArray, int lastIdx)
{
  ATH_MSG_DEBUG(this->getName() << "entering funtion setupHistogramsInFolder");

  if (objsFull){
    delete objsFull;
  }
  if (objsFast){
    delete objsFast;
  }
  ///////////////////////////////////////////////////////////////

  objsFull = new std::map<std::string, TObjArray*>;
  objsFast = new std::map<std::string, TObjArray*>;

  /// setup pairs of obj arrays and keys --> e.g. "sf", new Array to take all SF Histos
  for (unsigned int ikey=0; ikey<m_keys.size(); ++ikey){
    objsFull->insert(std::make_pair(m_keys.at(ikey), new TObjArray()));
    objsFast->insert(std::make_pair(m_keys.at(ikey), new TObjArray()));
  }

  objsFull->insert(std::make_pair("sys", new TObjArray()));
  objsFast->insert(std::make_pair("sys", new TObjArray()));
  //C.A if we really overwrite memory here, we need to delete what is in, but do we reach this point , really?
  if (sysObjsFull) {
    delete sysObjsFull;
  }
  if (sysObjsFast) {
    delete sysObjsFast;
  }

  sysObjsFull = new std::vector<TObjArray*>;
  sysObjsFast = new std::vector<TObjArray*>;

  m_runNumBegin = -1;
  TString myBegRunNumString = ((TObjString*)dirNameArray->At(lastIdx-1))->GetString();
  if ( myBegRunNumString.IsDigit() )
    {
      m_runNumBegin = myBegRunNumString.Atoi();
    }
  m_runNumEnd = -1;
  TString myEndRunNumString = ((TObjString*)dirNameArray->At(lastIdx))->GetString();
  if ( myEndRunNumString.IsDigit() ){
    m_runNumEnd = myEndRunNumString.Atoi();
  }
  if ( m_runNumBegin < 0 || m_runNumEnd < 0 || m_runNumEnd < m_runNumBegin ){

    ATH_MSG_ERROR(this->getName() 
		  << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "      
		  << "! Could NOT interpret the run number range: " << m_runNumBegin
		  << " - " << m_runNumEnd);
    return 0;
  }
  
  ATH_MSG_DEBUG(this->getName() << m_runNumBegin << "  " << m_runNumEnd);

  TIter nextkey(gDirectory->GetListOfKeys());
  TKey *key;
  TObject* obj(0);
  TString tmpName="";
  TObjArray *tmpArrayFull(NULL);
  TObjArray *tmpArrayFast(NULL);
  int tmpCounter = 0;
  int loop=0;
  while ( (key = (TKey*)nextkey()) ){
    obj=key->ReadObj();
    if ( obj->IsA()->InheritsFrom( "TH1" ) )
      {
	//The histogram containing the scale factors need to end with _sf and need to contain either the string "FullSim" or "AtlFast2"!
	if (TString(obj->GetName()).Contains("FullSim"))
	  {
	    for (unsigned int ikey=0; ikey<m_keys.size(); ++ikey)
	      {
		if ( TString(obj->GetName()).EndsWith("_"+(TString)m_keys.at(ikey))) {
		  objsFull->find(m_keys.at(ikey))->second->Add(obj);
		}
		if (!loop && TString(obj->GetName()).EndsWith("_sys")) {
		  objsFull->find("sys")->second->Add(obj);
		  tmpCounter++;loop++;
		  if (tmpArrayFull == 0 && (objsFull->find("sys")->second))  {
		    tmpArrayFull = new TObjArray();
		    tmpArrayFull->Add(obj);
		  }
		  else if (tmpArrayFull != 0)  tmpArrayFull->Add(obj);
		}
	      }

	    if (!loop && TString(obj->GetName()).EndsWith("_sys")) {
	      objsFull->find("sys")->second->Add(obj);
	      tmpCounter++;
	      loop++;
	      if (tmpArrayFull == 0 && (objsFull->find("sys")->second))  {
		tmpArrayFull = new TObjArray();
		tmpArrayFull->Add(obj);
	      }
	      else   tmpArrayFull->Add(obj);
	    }

	    tmpName = TString(obj->GetName());
	    if (tmpName.Contains("_corr"))  
	      {
		if (tmpName.EndsWith("corr0")){
		  if (tmpCounter > m_nSysMax) m_nSysMax = tmpCounter;
		  tmpCounter = 0;
		  if (tmpArrayFull != 0) {
		    sysObjsFull->push_back(tmpArrayFull);
		  }
		  tmpArrayFull = new TObjArray();
		}
		if (tmpArrayFull != 0)  tmpArrayFull->Add(obj);
		else
		  {
		    ATH_MSG_ERROR(this->getName() 
				  << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "	      
				  << "! Tried to read systematic uncertainties for full sim scale factor," 
				  <<" but could NOT find a histogram with name ending in \"corr0\". Please check input file! ");
		    return 0;
		  }
		tmpCounter++;
	      }
	    if (tmpCounter > m_nSysMax) m_nSysMax = tmpCounter;
	  }
	else if (TString(obj->GetName()).Contains("AtlFast2"))
	  {
	    for (unsigned int ikey=0; ikey<m_keys.size(); ++ikey)
	      {
		if ( TString(obj->GetName()).EndsWith("_"+(TString)m_keys.at(ikey))) objsFast->find(m_keys.at(ikey))->second->Add(obj);
	      }
	    tmpName = TString(obj->GetName());
	    if (tmpName.Contains("_corr"))  
	      {
		if (tmpName.EndsWith("corr0")){
		  if (tmpCounter > m_nSysMax) m_nSysMax = tmpCounter;
		  tmpCounter = 0;
		  if (tmpArrayFast != 0) {
		    sysObjsFast->push_back(tmpArrayFast);
		  }
		  tmpArrayFast = new TObjArray();
		}
		if (tmpArrayFast != 0)  tmpArrayFast->Add(obj);
		else
		  {
		    ATH_MSG_ERROR(this->getName() 
				  << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
				  << "! Tried to read systematic uncertainties for fast sim scale factor, " 
				  <<"but could NOT find a histogram with name ending in \"corr0\". Please check input file! ");
		    return 0;
		  }
		tmpCounter++;
	      }
	    if (tmpCounter > m_nSysMax) m_nSysMax = tmpCounter;
	  }
	else{
	  ATH_MSG_ERROR(this->getName() 
			<< " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
			<< "! Could NOT interpret if the histogram: " << obj->GetName()
			<< " is full or fast simulation!");
	  return 0;
	}
      }
  }
  //  if (tmpArrayFull == 0 && (objsFull->find("sys")->second))  {
  //    tmpArrayFull = new TObjArray();
  //    tmpArrayFull->Add(objsFull->find("sys")->second);
  //}
  // if (tmpArrayFast == 0 && (objsFast->find("sys")->second))  {
  //   tmpArrayFast = new TObjArray();
  //  tmpArrayFast->Add(objsFast->find("sys")->second);
  //}
  if (tmpArrayFull != 0){
    sysObjsFull->push_back(tmpArrayFull);
  }
  if (tmpArrayFast != 0){
    sysObjsFast->push_back(tmpArrayFast);
  }

  ATH_MSG_DEBUG(this->getName() << "setting up histograms for run ranges  " << m_runNumEnd);

  for (unsigned int ikey=0; ikey<m_keys.size(); ++ikey){
 
    if (objsFull->find(m_keys.at(ikey))->second->GetEntries() != 0) { 
 
      if ( 0 == this->setup( objsFull->find(m_keys.at(ikey))->second, m_histList[m_keys.at(ikey)], m_begRunNumberList, m_endRunNumberList) ){
	
	ATH_MSG_ERROR(this->getName() 
		      << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "			  
		      << "! Could NOT setup histogram " << m_keys.at(ikey)
		      << " for full sim!");
	return 0;
      }
    }
    
    if (objsFast->find(m_keys.at(ikey))->second->GetEntries() != 0) { 
      if ( 0 == this->setup( objsFast->find(std::string(m_keys.at(ikey)))->second, m_fastHistList[m_keys.at(ikey)], m_begRunNumberListFastSim, m_endRunNumberListFastSim) ){	    
	ATH_MSG_ERROR(this->getName() 
		      << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "			  
		      << "! Could NOT setup histogram " << m_keys.at(ikey)
		      << " for fast sim");
	return 0;
      }
    }
  }

  for (unsigned int sys=0; sys<sysObjsFast->size(); sys++){
    m_fastSysList.resize(sysObjsFast->size());
    if ( 0 == this->setup( sysObjsFast->at(sys), m_fastSysList[sys], m_begRunNumberListFastSim, m_endRunNumberListFastSim) ){
      ATH_MSG_ERROR(this->getName() 
		    << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "			  
		    << "! Could NOT setup systematic histograms for fast sim");
      return 0;
    }
  }
  
  for (unsigned int sys=0; sys<sysObjsFull->size(); sys++){
    m_sysList.resize(sysObjsFull->size());
    if ( 0 == this->setup( sysObjsFull->at(sys), m_sysList[sys], m_begRunNumberList, m_endRunNumberList) ){
      ATH_MSG_ERROR(this->getName() 
		    << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "			  
		    << "! Could NOT setup systematic histograms for fast sim");
      return 0;
    }
  }
  
  ATH_MSG_DEBUG(this->getName()  
		<< TString::Format(" (file: %s, line: %i!)\n", __FILE__, __LINE__)<<"Checking for (m_doToyMC || m_doCombToyMC)");

  if (m_doToyMC || m_doCombToyMC){
    bool fullToysBooked = kFALSE;
    bool fastToysBooked = kFALSE;
   
    if ( m_histList["sf"].size() > 0 ){

      if ( objsFull->find("eig")->second->GetEntries()<1 ||objsFull->find("stat")->second->GetEntries() < 1 || objsFull->find("uncorr")->second->GetEntries() < 1 ) {

	if ( objsFull->find("stat")->second->GetEntries() > 1 || objsFull->find("sys")->second->GetEntries() > 1 ) {

	  m_uncorrToyMCSystFull->push_back(buildToyMCTable(objsFull->find("sf")->second, NULL, objsFull->find("stat")->second, 
							   NULL, sysObjsFull));
	  fullToysBooked = kTRUE;
	}
	else {
	
	  ATH_MSG_INFO(this->getName() 
		       << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		       << "! Toy MC error propagation booked, but not all needed histograms found in the output (For full sim). Skipping toy creation!");
	}
      }
      else {
	m_uncorrToyMCSystFull->push_back(buildToyMCTable(objsFull->find("sf")->second, objsFull->find("eig")->second, objsFull->find("stat")->second, 
							 objsFull->find("uncorr")->second, sysObjsFull));
	fullToysBooked = kTRUE;
      }
    }
    
    ///// AF2
    if ( m_fastHistList["sf"].size() > 0 ){
      if ( objsFast->find("eig")->second->GetEntries()<1 ||objsFast->find("stat")->second->GetEntries() < 1 || objsFast->find("uncorr")->second->GetEntries() < 1 ){

	if ( objsFast->find("stat")->second->GetEntries() > 1 || objsFast->find("sys")->second->GetEntries() > 1 ) {

	  m_uncorrToyMCSystFast->push_back(buildToyMCTable(objsFast->find("sf")->second, NULL, objsFast->find("stat")->second, 
							   NULL, sysObjsFast));
	  fastToysBooked = kTRUE;
	}
	else {

	  ATH_MSG_INFO(this->getName() 
		       << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		       << "! Toy MC error propagation booked, but not all needed histograms found in the output (For fast sim). Skipping toy creation!");
	}
      }  
      else {
	m_uncorrToyMCSystFast->push_back(buildToyMCTable(objsFast->find("sf")->second, objsFast->find("eig")->second, objsFast->find("stat")->second, 
							 objsFast->find("uncorr")->second, sysObjsFast));
	fastToysBooked = kTRUE;
      }
    }
    
    if (fullToysBooked || fastToysBooked){
      if (m_doToyMC){
	ATH_MSG_INFO(this->getName() 
		     << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		     << "Created tables for "<<m_nToyMC<<" ToyMC systematics ");
      }
      if (m_doCombToyMC){
	ATH_MSG_INFO(this->getName() 
		     << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		     << "Created tables for "<<m_nToyMC<<" combined ToyMC systematics ");
      }
    }
  }

  return 1;
}


//=============================================================================
// Fill and interpret the setup, depending on which histograms are found in the input file(s)
//=============================================================================
int Root::TElectronEfficiencyCorrectionTool::setup( TObjArray* hists,
                                                    std::vector< TObjArray* >& histList,
                                                    std::vector< unsigned int >& beginRunNumberList,
                                                    std::vector< unsigned int >& endRunNumberList )
{
 
  if ( !hists ){
    ATH_MSG_ERROR(this->getName() 
		  << " (file: " << __FILE__ << ", line: " << __LINE__ << ") "
		  << "! Could NOT find histogram with name *_sf in folder");
    return 0;
  }
  TH2 *tmpHist(0);
  for (int i=0; i<hists->GetEntries(); i++)  {
    tmpHist = (TH2*)hists->At(i);
    tmpHist->SetDirectory(0);
  }
  // Now, we have all the needed info. Fill the vectors accordingly
  histList.push_back( hists );
  if (beginRunNumberList.size()>0){
    if (m_runNumBegin != (int)beginRunNumberList.back()) beginRunNumberList.push_back( m_runNumBegin );
  }
  else beginRunNumberList.push_back( m_runNumBegin );
  if (endRunNumberList.size()>0 ){
    if ( m_runNumEnd != (int)endRunNumberList.back()) endRunNumberList.push_back( m_runNumEnd );
  }
  else endRunNumberList.push_back( m_runNumEnd );

  return 1;
}

//=============================================================================
// print a message that the default scale factor is returned
//=============================================================================
void Root::TElectronEfficiencyCorrectionTool::printDefaultReturnMessage(TString reason, int line){

  ATH_MSG_DEBUG(this->getName()  
		<< TString::Format(" (file: %s, line: %i!)\n", __FILE__, line)<< 
		reason <<"\n"<<
		"Returning scale factor 1.0 with uncertainty 1.0");

}
