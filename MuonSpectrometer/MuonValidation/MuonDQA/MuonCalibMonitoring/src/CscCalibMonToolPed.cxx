/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     CscCalibMonToolPed.cxx
// PACKAGE:  MuonCalibMonitoring  
//
// AUTHORS: Caleb Parnell-Lampen <lampen@physics.arizona.edu> 
//
// ********************************************************************

#include "CscCalibMonToolPed.h"

#include <cassert>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "CscCalibData/CscCalibReportContainer.h"
#include "CscCalibData/CscCalibReportPed.h"
#include "MuonCondInterface/CscICoolStrSvc.h"
using namespace std;


CscCalibMonToolPed::CscCalibMonToolPed(const std::string & type, const std::string & name, 
    const IInterface* parent) : 
  CscCalibMonToolBase(type, name, parent),
  m_muon_mgr(NULL),
  m_h_pedMissingChannels(NULL),
  m_pedBadBin(1),
  m_noiseBadBin(2),
  m_rmsBadBin(3),
  m_f001BadBin(4),
  m_nEntriesBadBin(5),
  m_chi2BadBin(6),
  m_missingBadBin(7),
  m_onlTHoldBreachBadBin(8),
  m_alwaysPrintErrorReport(true),
  m_h_numBad(NULL),
  m_pedNewColl(NULL),
  m_pedOldColl(NULL),
  m_pedDiffColl(NULL),        
  m_noiseNewColl(NULL),
  m_noiseOldColl(NULL),
  m_noiseDiffColl(NULL),
  m_chi2Coll(NULL),
  m_rmsNewColl(NULL),
  m_rmsOldColl(NULL),
  m_rmsDiffColl(NULL),
  m_f001NewColl(NULL),
  m_f001OldColl(NULL),
  m_f001DiffColl(NULL),
  m_onlTHoldBreachColl(NULL),
  m_nEntriesColl(NULL),
  m_tholdDiffColl(NULL),
  m_maxBitCorrColl(NULL),
  m_h2_rmsVnoiseEta(NULL),
  m_h2_rmsVnoisePhi(NULL)
{
  declareProperty("MaxPedDiff",m_pedMaxDiff=2.0);
  declareProperty("MaxNoiseDiff",m_noiseMaxDiff = 5.0);
  declareProperty("MaxChi2_NDF",m_chi2Max = 800);
  declareProperty("MaxRMSDiff",m_rmsMaxDiff = 5);
  declareProperty("MaxF001Diff", m_f001MaxDiff = 5);
  declareProperty("MaxOnlTHoldBreachesPerChannel", m_onlTHoldBreachMax = 400);
  declareProperty("PedAmpHistKey", m_histKey = "cscPedCalibReport"); 
  declareProperty("DoRmsVNoise", m_doRmsVNoise = false);
  declareProperty("MinAmpHistEntries", m_minAmpHistEntries = 4000);
  declareProperty("AlwaysPrintErrorReport",m_allGoodFileName = "AllCalibMonGood");
  declareProperty("StatusReportFileName", m_statusReportName = "PedReport.txt");
  declareProperty("StatusReportPrefix", m_statusReportPrefix = "");

  declareProperty("DoBitCorrelations", m_doBitCorrelations = false);


  //Although this can be declared in the base class, we give a default here
  if(m_calibResultKey == "")
    m_calibResultKey = "CscCalibResultPed"; //Determines what CscCalibMonToolBase will pull out of TDS



  /*##From CscCalibMonToolBase.cxx, for reference. 
    declareProperty("MakeHashValueHist",m_makeHashHist=true);
    declareProperty("MakeLayerValueHists",m_makeLayerHists=false);
    declareProperty("DetailedResultHashIds",m_detailedHashIds);
   */

}

StatusCode CscCalibMonToolPed::initialize() 
{
  StatusCode sc = CscCalibMonToolBase::initialize();


  m_generic_path_csccalibmonitoring = "MUON_CSC_PED";

  return sc;
}


StatusCode CscCalibMonToolPed::finalize()
{
  delete m_pedNewColl;    
  delete m_pedOldColl;
  delete m_pedDiffColl;
  delete m_noiseNewColl;
  delete m_noiseOldColl;
  delete m_noiseDiffColl;
  delete m_rmsNewColl;
  delete m_rmsOldColl;
  delete m_rmsDiffColl;
  delete m_f001NewColl;
  delete m_f001OldColl;
  delete m_f001DiffColl;
  delete m_chi2Coll;

  return CscCalibMonToolBase::finalize();
}


CscCalibMonToolPed::~CscCalibMonToolPed()
{

  ATH_MSG_INFO( "CscCalibMonToolPed :  deleting CscCalibMonToolPed "  );
}

StatusCode CscCalibMonToolPed::bookHistograms()
{ 
  if (!CscCalibMonToolBase::bookHistograms().isSuccess())
  {
    ATH_MSG_WARNING( "CscCalibMonToolPed : in bookHistograms()"  );
  }
  ATH_MSG_DEBUG( "CscCalibMonToolPed : in bookHistograms()"  );

  //declare a group of histograms

  if (newRunFlag())
  {
    m_monGroupVec = new DataVector<MonGroup>;  

    string geoPath = getGeoPath();
    string path = getFullPath(geoPath, "Misc", "");
    MonGroup monGroup( this, path, run, ATTRIB_MANAGED );

    string name,title,xaxis,yaxis;
    int highbound,lowbound,nbins;

    //num bad histograms
    name = "h_csc_calib_numSignificant";
    title = "Number of significant results.";
    xaxis = "Catagory";
    yaxis = "Num channels with bad value.";
    lowbound = 1;
    highbound = 8;
    m_h_numBad = new TH1I(name.c_str(),title.c_str(),highbound-lowbound+1,lowbound,highbound+1);
    m_h_numBad->GetYaxis()->SetTitle(yaxis.c_str());     
    m_h_numBad->GetXaxis()->SetTitle(xaxis.c_str());
    stringstream pedBinTitle; pedBinTitle << "#Delta ped > " << m_pedMaxDiff;
    m_h_numBad->GetXaxis()->SetBinLabel(m_pedBadBin, pedBinTitle.str().c_str());
    stringstream noiseBinTitle; noiseBinTitle << "#Delta noise > "<< m_noiseMaxDiff;
    m_h_numBad->GetXaxis()->SetBinLabel(m_noiseBadBin, noiseBinTitle.str().c_str());
    stringstream rmsBinTitle; rmsBinTitle << "#Delta RMS > " << m_rmsMaxDiff;
    m_h_numBad->GetXaxis()->SetBinLabel(m_rmsBadBin, rmsBinTitle.str().c_str());
    stringstream f001BinTitle; f001BinTitle << "#Delta F001 > " << m_f001MaxDiff;
    m_h_numBad->GetXaxis()->SetBinLabel(m_f001BadBin, f001BinTitle.str().c_str());
    stringstream statisticsBinTitle; statisticsBinTitle << "N Entries < " << m_minAmpHistEntries;
    m_h_numBad->GetXaxis()->SetBinLabel(m_nEntriesBadBin, statisticsBinTitle.str().c_str());
    stringstream chiBinTitle; chiBinTitle << "#frac{#chi^{2}}{ndf} > " << m_chi2Max;
    m_h_numBad->GetXaxis()->SetBinLabel(m_chi2BadBin,chiBinTitle.str().c_str());
    m_h_numBad->GetXaxis()->SetBinLabel(m_missingBadBin,"Missing channels ");
    m_h_numBad->GetXaxis()->SetBinLabel(m_onlTHoldBreachBadBin, "Onl THold Breaches");
    m_h_numBad->SetFillColor(m_histColAlert);
    monGroup.regHist(m_h_numBad).ignore();


    name = "h_csc_calib_pedMissingChannels";
    title = "Channels Missing Data From Pedestal Run.";
    xaxis = "Channel (Hash ID)";
    yaxis = "Number of Parameter Types Missing";
    lowbound = 0;
    highbound = m_maxHashId +1;
    nbins = m_maxHashId +1;
    m_h_pedMissingChannels = new TH1F(name.c_str(), title.c_str(), nbins,lowbound,highbound);
    m_h_pedMissingChannels->GetXaxis()->SetTitle(xaxis.c_str());
    m_h_pedMissingChannels->GetYaxis()->SetTitle(yaxis.c_str());     
    m_h_pedMissingChannels->SetFillColor(m_histColAlert);
    monGroup.regHist(m_h_pedMissingChannels).ignore();

    //Set naming parameters for datatypes
    string pedDataName      = "ped";
    string pedDataTitle     = "Pedestals";
    string pedSubDir        = "Ped";

    string noiseDataName    = "noise";
    string noiseDataTitle   = "Noise (ped sigma)";
    string noiseSubDir      = "Noise";

    string chi2DataName     = "chi2";
    string chi2DataTitle    = "Chi^2/ndf for Pedestal Gaussian Fit";
    string chi2SubDir       = "Chi2";

    string rmsDataName      = "rms";
    string rmsDataTitle     = "RMS from gaussian distribution";
    string rmsSubDir        = "RMS";

    string f001DataName      = "f001";
    string f001DataTitle     = "F001";
    string f001SubDir        = "F001";

    string onlTHoldBreachDataName = "onlTHoldBreach";
    string onlTHoldBreachDataTitle = "Online Threshold Breaches";
    string onlTHoldBreachSubDir = "OnlTHoldBreaches";

    string nEntriesDataName = "nEntries";
    string nEntriesDataTitle = "Number of Entries from Pedestal Amplitude Hist";
    string nEntriesSubDir    = "NumEntries";

    string maxBitCorrDataName = "maxBitCorr";
    string maxBitCorrDataTitle = "Maximimum Bit Correlation";
    string maxBitCorrSubDir = "MaxBitCorr";


    //Set naming parameters for histogram category names
    string newCatName       = "new";
    string newCatTitle      = "New";

    string oldCatName       = "old";
    string oldCatTitle      = "COOL";

    string diffCatName      = "diff";
    string diffCatTitle     = "Change of ";

    //axis info
    string pedAxisLabel = "Pedestal Mean (ADC counts)";
    string pedDiffAxisLabel = "Pedestal Difference (ADC counts)";
    int pedNumBins =300;
    float pedLowBound = 1900;
    float pedHighBound = 2200;

    string noiseAxisLabel = "Pedestal Noise (ADC counts)";
    string noiseDiffAxisLabel = "Noise Difference (ADC counts)";
    int noiseNumBins = 300;
    float noiseLowBound = 0;
    float noiseHighBound = 30;

    string chi2AxisLabel = "Chi^2/ndf";
    int chi2NumBins = 500;
    float chi2LowBound = 0;
    float chi2HighBound = 500;

    string rmsAxisLabel = "RMS (ADC Counts)";
    string rmsDiffAxisLabel = "RMS Difference (ADC Counts)";
    int rmsNumBins = 300;
    float rmsLowBound =0;
    float rmsHighBound = 30;

    string f001AxisLabel = "F001 (ADC)";
    string f001DiffAxisLabel = "F001 Difference (ADC)";
    int f001NumBins = 350;
    float f001LowBound =1950;
    float f001HighBound = 2300;

    string nEntriesAxisLabel = "Number of Entries";
    int nEntriesNumBins = 20;
    float nEntriesLowBound = 0; 
    float nEntriesHighBound = 10000;
    uint16_t nEntriesHistMask = 0x3F; //only do overall spectra and 2d view

    string maxBitCorrAxisLabel = "Correlation";
    int maxBitCorrNumBins = 300;
    float maxBitCorrLowBound = -3;
    float maxBitCorrHighBound = 3;
    //Subdirectory this will be stored in:

    //Initialize histogram collections
    //Each colleciton contains all of the "views" for one particular set of data
    m_pedNewColl = new HistCollection(m_maxHashId +1, m_maxHashId +1); //New Pedestals
    m_pedOldColl = new HistCollection(m_maxHashId +1);  //Last pedestals
    m_pedDiffColl = new HistCollection(m_maxHashId +1); //Difference

    m_noiseNewColl = new HistCollection(m_maxHashId +1, m_maxHashId +1);
    m_noiseOldColl = new HistCollection(m_maxHashId +1);
    m_noiseDiffColl = new HistCollection(m_maxHashId +1);

    m_chi2Coll = new HistCollection(m_maxHashId +1);

    m_rmsNewColl = new HistCollection(m_maxHashId +1, m_maxHashId +1);
    m_rmsOldColl = new HistCollection(m_maxHashId +1);
    m_rmsDiffColl = new HistCollection(m_maxHashId +1);

    m_f001NewColl = new HistCollection(m_maxHashId +1);
    m_f001OldColl = new HistCollection(m_maxHashId +1);
    m_f001DiffColl = new HistCollection(m_maxHashId +1);

    m_onlTHoldBreachColl = new HistCollection(m_maxHashId +1);

    m_nEntriesColl = new HistCollection(m_maxHashId +1);


    m_tholdDiffColl = new HistCollection(m_maxHashId +1);

    if(m_doBitCorrelations)
      m_maxBitCorrColl = new HistCollection(m_maxHashId +1);


    //initialize, name, and book histograms in histogram collections:

    ATH_MSG_DEBUG( "Registering pedNewColl"  );
    ATH_CHECK( bookHistCollection(m_pedNewColl, pedDataName, pedDataTitle, newCatName, newCatTitle,
                                  pedAxisLabel, pedNumBins, pedLowBound, pedHighBound, pedSubDir) );

    ATH_MSG_DEBUG( "Registering pedOldColl"  );
    ATH_CHECK( bookHistCollection(m_pedOldColl, pedDataName, pedDataTitle, oldCatName, oldCatTitle,
                                  pedAxisLabel, pedNumBins, pedLowBound, pedHighBound, pedSubDir) );

    ATH_MSG_DEBUG( "Registering pedDiffColl"  );
    ATH_CHECK( bookHistCollection(m_pedDiffColl, pedDataName, pedDataTitle, diffCatName, diffCatTitle,
                                  pedDiffAxisLabel, 100, -2, 2, pedSubDir) );

    ATH_MSG_DEBUG( "Registering noiseNewColl"  );
    ATH_CHECK( bookHistCollection(m_noiseNewColl, noiseDataName, noiseDataTitle, newCatName, 
                                  newCatTitle, noiseAxisLabel, noiseNumBins, noiseLowBound, noiseHighBound, noiseSubDir) );

    ATH_MSG_DEBUG( "Registering noiseOldColl"  );
    ATH_CHECK( bookHistCollection(m_noiseOldColl, noiseDataName, noiseDataTitle, oldCatName, 
                                  oldCatTitle, noiseAxisLabel, 100, -2, 2, noiseSubDir) );

    ATH_MSG_DEBUG( "Registering noiseDiffColl"  );
    ATH_CHECK( bookHistCollection(m_noiseDiffColl, noiseDataName, noiseDataTitle, diffCatName, 
                                  diffCatTitle, noiseDiffAxisLabel, noiseNumBins, -1*noiseHighBound, noiseHighBound,noiseSubDir) );

    ATH_MSG_DEBUG( "Registering rmsNewColl"  );
    ATH_CHECK( bookHistCollection(m_rmsNewColl, rmsDataName, rmsDataTitle, newCatName, 
                                  newCatTitle, rmsAxisLabel, rmsNumBins, rmsLowBound, rmsHighBound, rmsSubDir) );

    ATH_MSG_DEBUG( "Registering rmsOldColl"  );
    ATH_CHECK( bookHistCollection(m_rmsOldColl, rmsDataName, rmsDataTitle, oldCatName, 
                                  oldCatTitle, rmsAxisLabel, rmsNumBins, rmsLowBound, rmsHighBound, rmsSubDir) );

    ATH_MSG_DEBUG( "Registering rmsDiffColl"  );
    ATH_CHECK( bookHistCollection(m_rmsDiffColl, rmsDataName, rmsDataTitle, diffCatName, 
                                  diffCatTitle, rmsDiffAxisLabel, rmsNumBins, -1*rmsHighBound, rmsHighBound,rmsSubDir) );

    ATH_MSG_DEBUG( "Registering f001NewColl"  );
    ATH_CHECK( bookHistCollection(m_f001NewColl, f001DataName, f001DataTitle, newCatName, 
                                  newCatTitle, f001AxisLabel, f001NumBins, f001LowBound, f001HighBound, f001SubDir) );

    ATH_MSG_DEBUG( "Registering f001OldColl"  );
    ATH_CHECK( bookHistCollection(m_f001OldColl, f001DataName, f001DataTitle, oldCatName, 
                                  oldCatTitle, f001AxisLabel, 100, -2, 2, f001SubDir) );

    ATH_MSG_DEBUG( "Registering f001DiffColl"  );
    ATH_CHECK( bookHistCollection(m_f001DiffColl, f001DataName, f001DataTitle, diffCatName, 
                                  diffCatTitle, f001DiffAxisLabel, 60, -30, 30, f001SubDir) );
    
    ATH_MSG_DEBUG( "Registering onlTHoldBreachColl"  );
    ATH_CHECK( bookHistCollection(m_onlTHoldBreachColl, onlTHoldBreachDataName, onlTHoldBreachDataTitle, "",  
                                  "" , "Number of Online THold Breachs", 100, 0, 1000, onlTHoldBreachSubDir) );

    ATH_MSG_DEBUG( "Registering Num Entries"  );
    ATH_CHECK( bookHistCollection(m_nEntriesColl, nEntriesDataName, nEntriesDataTitle, "", 
                                  "", nEntriesAxisLabel, nEntriesNumBins, nEntriesLowBound, nEntriesHighBound, nEntriesSubDir, nEntriesHistMask) );

    ATH_CHECK( bookHistCollection(m_chi2Coll, chi2DataName, chi2DataTitle, "", "",
                                  chi2AxisLabel, chi2NumBins, chi2LowBound, chi2HighBound, chi2SubDir) );

    ATH_CHECK( bookHistCollection(m_tholdDiffColl, "thold", "Threshold", diffCatName, diffCatTitle, "#Delta{}Threshold", 500, -10, 10, "THold") );

    if(m_doBitCorrelations) {
      ATH_CHECK( bookHistCollection(m_maxBitCorrColl, maxBitCorrDataName, maxBitCorrDataTitle,
                                    "", "", maxBitCorrAxisLabel, maxBitCorrNumBins, maxBitCorrLowBound, 
                                    maxBitCorrHighBound, maxBitCorrSubDir) );
    }
  }//end if newRun

  return StatusCode::SUCCESS;
}//end bookHistograms


//--handleParameter: Processes a vector of parameter values by filling the appropriate histograms
StatusCode CscCalibMonToolPed::handleParameter(const CscCalibResultCollection* parVals)
{
  ATH_MSG_DEBUG( "CscCalibMonToolPed : in procParameter()"  );

  //The whole point of this funciton is to pass the correct histograms and setup info 
  //to CsccalibMonToolBase::procParameter. To organize this, we store the setup info into
  //these structs: 
  ProcSetupInfo ProcParameterInput;


  //--setup for this parameter
  //We will specify :
  //name of database parameter if we want to compare with datbase value
  //otherwise we provide expeted value to compare with
  //we also provide handles to all the arrays and values procParameter uses, such as the list
  //of expected hash ids.
  if(!parVals)
  {
    ATH_MSG_FATAL("Blank parval passed to handle parameter");
    return StatusCode::FAILURE;
  }
  string parName = parVals->parName();
  if(parName == "ped")
  {
    ProcParameterInput.dbName = parVals->parName();
    ProcParameterInput.badHist = m_h_numBad;
    ProcParameterInput.badBin = m_pedBadBin;
    ProcParameterInput.maxDiff = m_pedMaxDiff;
    ProcParameterInput.chi2BadBin = m_chi2BadBin;
    ProcParameterInput.chi2Max = m_chi2Max;
    ProcParameterInput.doChi2 = true;
    ProcParameterInput.vals = &(m_pedNewColl->data);
    ProcParameterInput.errors = &(m_pedNewColl->errors);
    ProcParameterInput.oldVals =& (m_pedOldColl->data);
    ProcParameterInput.diffs = &(m_pedDiffColl->data);
    ProcParameterInput.chi2s = &(m_chi2Coll->data);
    ProcParameterInput.expectedChannels = m_expectedHashIdsAll;
  }
  else if (parName == "noise")
  {
    ProcParameterInput.dbName = parVals->parName();
    ProcParameterInput.badHist = m_h_numBad;
    ProcParameterInput.badBin = m_noiseBadBin;
    ProcParameterInput.maxDiff = m_noiseMaxDiff;
    ProcParameterInput.missingBadBin = m_missingBadBin;
    ProcParameterInput.missingChans = m_h_pedMissingChannels;
    ProcParameterInput.doChi2 = false;
    ProcParameterInput.vals = &(m_noiseNewColl->data);
    ProcParameterInput.errors = &(m_noiseNewColl->errors);
    ProcParameterInput.oldVals = &(m_noiseOldColl->data);
    ProcParameterInput.diffs = &(m_noiseDiffColl->data);
    ProcParameterInput.expectedChannels = m_expectedHashIdsAll;
  }
  else if (parName == "rms")
  {
    ProcParameterInput.dbName = parVals->parName();
    ProcParameterInput.badHist = m_h_numBad;
    ProcParameterInput.badBin = m_rmsBadBin;
    ProcParameterInput.maxDiff = m_rmsMaxDiff;
    ProcParameterInput.missingBadBin = m_missingBadBin;
    ProcParameterInput.missingChans = m_h_pedMissingChannels;
    ProcParameterInput.doChi2 = false;
    ProcParameterInput.vals = &(m_rmsNewColl->data);
    ProcParameterInput.errors = &(m_rmsNewColl->errors);
    ProcParameterInput.oldVals = &(m_rmsOldColl->data);
    ProcParameterInput.diffs = &(m_rmsDiffColl->data);
    ProcParameterInput.expectedChannels = m_expectedHashIdsAll;
  }
  else if (parName == "f001")
  {
    ProcParameterInput.dbName = parVals->parName();
    ProcParameterInput.badHist = m_h_numBad;
    ProcParameterInput.badBin = m_f001BadBin;
    ProcParameterInput.maxDiff = m_f001MaxDiff;
    ProcParameterInput.missingBadBin = m_missingBadBin;
    ProcParameterInput.missingChans = m_h_pedMissingChannels;
    ProcParameterInput.doChi2 = false;
    ProcParameterInput.vals = &(m_f001NewColl->data);
    ProcParameterInput.errors = &(m_f001NewColl->errors);
    ProcParameterInput.oldVals = &(m_f001OldColl->data);
    ProcParameterInput.diffs = &(m_f001DiffColl->data);
    ProcParameterInput.expectedChannels = m_expectedHashIdsAll;
  }
  else if (parName == "OnlTHoldBreaches"){
    //How many samples failed the online threshold test of f001 +2*RMS 
    //(f001 and RMS read from a file from online configuration db)
    //ProcParameterInput.dbName = parVals->parName();
    ProcParameterInput.badHist = m_h_numBad;
    ProcParameterInput.badBin = m_onlTHoldBreachBadBin;
    ProcParameterInput.expectedVal = 0; //expect 0 f001
    ProcParameterInput.maxDiff = m_onlTHoldBreachMax;
    ProcParameterInput.missingBadBin = m_missingBadBin;
    ProcParameterInput.missingChans = m_h_pedMissingChannels;
    ProcParameterInput.doChi2 = false;
    ProcParameterInput.vals = &(m_onlTHoldBreachColl->data);
    ProcParameterInput.errors = &(m_onlTHoldBreachColl->errors);
    //ProcParameterInput.oldVals = &(m_onlTHoldBreachColl->data);
    //ProcParameterInput.diffs = &(m_onlTHoldBreachColl->data);
    ProcParameterInput.expectedChannels = m_expectedHashIdsAll;
  }
  else
  {
    ATH_MSG_INFO( "CscCalibMonToolPed : Did not recognize parameter name " 
                  << parName << ". This is usually ok."  );
    return StatusCode::SUCCESS;
  }

  //Process data in parVals according to setting in ProcParameterInput. This includes:
  // -Copying data from parVals to ProcParameterInput.vals (i.e. the HistCollection)
  // -Checking for missing channels
  // -Look for deviations from expected values for each channel
  if(!procParameter(parVals,&ProcParameterInput).isSuccess())
  {
    ATH_MSG_FATAL( "CscCalibMonToolPed : Failed to process parameter " 
                   << parName  );
    return StatusCode::FAILURE;
  }

  if(parName == "ped") 
  {
    copyDataToHists(m_pedNewColl);
    copyDataToHists(m_pedOldColl);
    copyDataToHists(m_pedDiffColl);
    copyDataToHists(m_chi2Coll);
  }
  if(parName == "noise")
  {
    copyDataToHists(m_noiseNewColl);
    copyDataToHists(m_noiseOldColl);
    copyDataToHists(m_noiseDiffColl);
  }       
  if(parName == "rms")
  {
    copyDataToHists(m_rmsNewColl);
    copyDataToHists(m_rmsOldColl);
    copyDataToHists(m_rmsDiffColl);
  }       
  if(parName == "f001")
  {
    copyDataToHists(m_f001NewColl);
    copyDataToHists(m_f001OldColl);
    copyDataToHists(m_f001DiffColl);
  }       
  if(parName == "OnlTHoldBreaches"){
    copyDataToHists(m_onlTHoldBreachColl);
  }
  return StatusCode::SUCCESS;
}

void CscCalibMonToolPed::genThreshold(HistCollection * pedColl, HistCollection * noiseColl, HistCollection * tholdColl, float multiplier) {
  for(unsigned int chanItr = 0 ; chanItr <= m_maxHashId; chanItr++) {
    tholdColl->data[chanItr] = pedColl->data[chanItr] + multiplier*noiseColl->data[chanItr];
  }
}
//--postProc() will retrieve the details for the channels
//requested by the user in m_detailedHashIds.  
StatusCode CscCalibMonToolPed::postProc()
{
  ATH_MSG_DEBUG( "CscCalibMonToolPed : in postProc()"  );

  IdContext chanContext = m_cscIdHelper->channel_context();

  genThreshold(m_pedDiffColl, m_noiseDiffColl, m_tholdDiffColl, 3.5);

  copyDataToHists(m_tholdDiffColl);

  if(m_doRmsVNoise) {
    string geoPath = getGeoPath();
    string path = getFullPath(geoPath, "Misc", "");

    m_h2_rmsVnoiseEta = new TH2I("rmsVsigma_eta", "RMS versus sigma for #eta strips", 100, 0, 30, 100, 0,30) ;
    m_h2_rmsVnoiseEta->GetXaxis()->SetTitle("Sigma");
    m_h2_rmsVnoiseEta->GetYaxis()->SetTitle("RMS");
    regHist(m_h2_rmsVnoiseEta,path, run, ATTRIB_MANAGED);

    m_h2_rmsVnoisePhi = new TH2I("rmsVsigma_phi", "RMS versus sigma for #phi strips", 100, 0, 30, 100, 0,30) ;
    m_h2_rmsVnoisePhi->GetXaxis()->SetTitle("Sigma");
    m_h2_rmsVnoisePhi->GetYaxis()->SetTitle("RMS");
    regHist(m_h2_rmsVnoisePhi,path, run, ATTRIB_MANAGED);

    std::vector<float> & rmsVec = m_rmsNewColl->data;
    std::vector<float> & noiseVec = m_noiseNewColl->data;
    size_t nEntries = rmsVec.size();
    if(nEntries != noiseVec.size()){
      ATH_MSG_ERROR( "Number of noises != number of rmses"  );
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Filling rmsVnoise "  );

    for(unsigned int hashId = 0; hashId < nEntries; hashId++){
      ATH_MSG_DEBUG( "Filling rmsVnoise for hash id " << hashId  );
      Identifier chanId;
      m_cscIdHelper->get_id(IdentifierHash(hashId), chanId, &chanContext);
      int measuresPhi = m_cscIdHelper->measuresPhi(chanId);

      if(m_expectedHashIdsAll.count(hashId)) {
        if(measuresPhi)
          m_h2_rmsVnoisePhi->Fill(noiseVec[hashId], rmsVec[hashId]);
        else
          m_h2_rmsVnoiseEta->Fill(noiseVec[hashId], rmsVec[hashId]);
      }
    }
    ATH_MSG_DEBUG( "filled rmsVnoise "  );

  }

  if(m_doAllDetailed || (m_doBadDetailed && m_numBad > 0))
  {
    //Retrieve calibration report container from transient data store
    const DataHandle<CscCalibReportContainer> repCont;
    if (!evtStore()->retrieve(repCont, m_histKey).isSuccess())
    {
      ATH_MSG_WARNING( " Cannot retrieve object from storegate with key " 
                       << m_histKey <<  " aborting retrieving hists "  );
      return StatusCode::RECOVERABLE;
    }

    if(repCont->size() != 1)
    {
      ATH_MSG_WARNING( "Container with key " << m_histKey 
                       << " does not have a size of one. Do not know how to proceed, so aborting"
                       << " retrieving calibration histograms."  );
      return StatusCode::RECOVERABLE;
    }

    //Retrieve DataVector of amplitude histograms
    const CscCalibReportPed * pedReport = dynamic_cast<const CscCalibReportPed *>(repCont->front());
    if(pedReport->getLabel() != "pedAmps")
    {
      ATH_MSG_WARNING( "Incorrect object retrieved from  container."
                       << " Aborting hist retrieval."  );
      return StatusCode::RECOVERABLE;
    }

    const DataVector<TH1I> * pedAmpHists = pedReport->getPedAmpHists();
    if(!pedAmpHists)
      ATH_MSG_WARNING( "No pedAmpHists vector found from calibration. "
                       << " Won't be in monitoring output file"  );

    const DataVector<TH1I> * bitHists = pedReport->getBitHists();
    if(!bitHists)
      ATH_MSG_INFO( "No bit histogram vector found from calibration. "
                    << " Won't be in monitoring output file. "  );

    const DataVector<TH2F> * bitCorrelations = NULL;

    if(m_doBitCorrelations)
      bitCorrelations = pedReport->getBitCorrelation();

    const DataVector< DataVector<TH1I> >* sampHists = pedReport->getSampHists();

    //These are the channels we will get detailed amplitude histograms for.
    for(unsigned int idItr = 0; idItr <= m_maxHashId; idItr++)
    {
      if(m_expectedHashIdsAll.count(idItr))
      {
        ATH_MSG_VERBOSE( "Debug info for hash " << idItr 
                         << " is being retrieved."  );
        TH1I * sourceHist;

        Identifier chanId;
        m_cscIdHelper->get_id(IdentifierHash(idItr), chanId, &chanContext);
        int stationSize = m_cscIdHelper->stationName(chanId);
        int stationEta = m_cscIdHelper->stationEta(chanId);
        int stationPhi = m_cscIdHelper->stationPhi(chanId);
        int wireLayer = m_cscIdHelper->wireLayer(chanId);
        int measuresPhi = m_cscIdHelper->measuresPhi(chanId);
        int strip = m_cscIdHelper->strip(chanId);
        int sector = getSector(stationPhi, stationSize);

        string geoPath = getGeoPath(stationEta, sector, wireLayer, measuresPhi);

        string pedAmpPath = getFullPath(geoPath, "PedAmpHists", "");
        string sampPath = getFullPath(geoPath, "SampHists","");
        string bitHistPath = getFullPath(geoPath, "BitHists", "");
        string bitCorrelationPath("");
        if(bitCorrelations)
          bitCorrelationPath = getFullPath(geoPath, "BitCorrelations", "");

        ATH_MSG_DEBUG( "Hash Id: " << idItr << ". Booking channel histograms in paths : " << pedAmpPath << " and " << bitHistPath   );
        //MonGroup chanMonGroup( this, path , run, ATTRIB_MANAGED);

        //Pedestal amplitude histograms
        //copy source histogram into new histogram, and store
        if(pedAmpHists)
        {
          sourceHist = const_cast<TH1I*>((*pedAmpHists)[idItr]);
          if(!sourceHist)
          {
            ATH_MSG_ERROR( "There is no pedestal amplitude histogram with hashId "
                           << idItr  );
            return StatusCode::RECOVERABLE;
          }

          double nEntries = sourceHist->GetEntries();

          if(idItr >= m_nEntriesColl->data.size() ){

            ATH_MSG_ERROR( "idItr == " << idItr 
                           << " but maximum m_nEntriesColl.data.size() == "
                           << m_nEntriesColl->data.size()   );
            return StatusCode::RECOVERABLE;
          }
          m_nEntriesColl->data[idItr] = nEntries;

          if(nEntries < m_minAmpHistEntries){
            m_detailedHashIds[idItr] = true;
            m_h_numBad->Fill(m_nEntriesBadBin);
          }

          if(m_detailedHashIds[idItr] || m_doAllDetailed){


            stringstream name;
            name << "h_pedAmp"
              << "_EC" << getEndCap(stationEta)
              << "_sector_" << sector 
              << "_layer_" << wireLayer
              << "_" << (measuresPhi ? "trans" : "prec")
              << "_strip_" 
              << setfill('0') <<  setw(measuresPhi ? 2 : 3) 
              << strip;

            //TH1I * newHist = (TH1I*)sourceHist->Clone(name.str().c_str());

            //Calibration is finished with histogram, so we can modify it a bit:

            sourceHist->SetName(name.str().c_str());
            sourceHist->SetFillColor((m_detailedHashIds[idItr] ? m_histColAlert : m_histCol));
            //regHist(newHist, chanPath, run, ATTRIB_MANAGED);
            regHist(sourceHist, pedAmpPath, run, ATTRIB_MANAGED);
          }
        }

        if(sampHists) {
          const DataVector<TH1I> * histVect = (*sampHists)[idItr];

          size_t hNum = histVect->size();
          for(size_t hCnt = 0; hCnt < hNum; hCnt++) {
            sourceHist = const_cast<TH1I*>((*histVect)[hCnt]);
            stringstream name;
            name << "h_samp"
              << "_EC" << getEndCap(stationEta)
              << "_sector_" << sector 
              << "_layer_" << wireLayer
              << "_" << (measuresPhi ? "trans" : "prec")
              << "_strip_" 
              << setfill('0') <<  setw(measuresPhi ? 2 : 3) 
              << strip 
              <<"_samp_"
              << hCnt;
            sourceHist->SetName(name.str().c_str());
            sourceHist->SetFillColor(m_histCol); 
            regHist(sourceHist,sampPath,run, ATTRIB_MANAGED);
          }
        }

        //Bit map histograms
        //copy source histogram into new histogram, and store
        if(bitHists)
        {
          sourceHist = const_cast<TH1I*>((*bitHists)[idItr]);
          if(!sourceHist)
          {
            ATH_MSG_ERROR( "There is no bit histogram with hashId "
                           << idItr << " Quiting out of detailed histogram loop."  );
            return StatusCode::RECOVERABLE;
          }

          stringstream name2;
          name2 << "h_bitMap"
            << "_EC" << getEndCap(stationEta)
            << "_sector_" << sector 
            << "_layer_" << wireLayer
            << "_" << (measuresPhi ? "trans" : "prec")
            << "_strip_" 
            << setfill('0') <<  setw(measuresPhi ? 2 : 3) 
            << strip;
          // TH1I * newHist2 = (TH1I*)sourceHist->Clone(name2.str().c_str());
          sourceHist->SetName(name2.str().c_str());
          sourceHist->SetFillColor((m_detailedHashIds[idItr] ? m_histColAlert : m_histCol));

          //regHist(newHist2, chanPath, run, ATTRIB_MANAGED);
          regHist(sourceHist, bitHistPath, run, ATTRIB_MANAGED);
        }//end if bithists*/

        if(bitCorrelations)
        {
          TH2F* hist = const_cast<TH2F*>((*bitCorrelations)[idItr]);
          regHist(hist,bitCorrelationPath,run, ATTRIB_MANAGED);

          float maxVal = -2;
          //Now find the maximum correlation
          size_t nx = hist->GetXaxis()->GetNbins();
          size_t ny = hist->GetYaxis()->GetNbins();
          size_t x_cut = nx -1; //skip last two bits, which can introduce fake 
          size_t y_cut = ny -1; //correlation
          for(size_t xi = 1; xi < x_cut; xi++){
            for(size_t yi = 1; yi < y_cut; yi++){
              if(yi == xi)  //Don't do diagonal
                continue;
              float val = hist->GetBinContent(xi,yi);
              if(val > maxVal)
                maxVal = val;
            }
          }//end x/y loops
          m_maxBitCorrColl->data[idItr] = maxVal;

        }
      }//if detailedHashInfo[stripHash] || do all hists.
    }//end hash loop
  }
  else
    ATH_MSG_DEBUG( "No channels flagged for debug info retrieval" );

  //Copy data from the num entries vector to all relevant histograms
  copyDataToHists(m_nEntriesColl);
  //Copy data from the bit correlation vector to all relevant histograms
  if(m_doBitCorrelations)
    copyDataToHists(m_maxBitCorrColl);

  makeErrorReport(); 

  return StatusCode::SUCCESS;    
}


void CscCalibMonToolPed::makeErrorReport(){
  stringstream report; 

  double numBadPed = m_h_numBad->GetBinContent(m_pedBadBin);
  report << "Num channels with Delta Ped > " << m_pedMaxDiff << " : " << numBadPed << endl;

  double numBadRms = m_h_numBad->GetBinContent(m_rmsBadBin);
  report << "Num channels with Delta RMS > " << m_rmsMaxDiff << " : " << numBadRms << endl;

  double numBadF001 = m_h_numBad->GetBinContent(m_f001BadBin);
  report << "Num channels with Delta f001 > " << m_f001MaxDiff << " : " << numBadF001 << endl;

  double numBadNoise = m_h_numBad->GetBinContent(m_noiseBadBin);
  report << "Num channels with Delta sigma > " << m_noiseMaxDiff << " : " << numBadNoise << endl;

  double numBadNEntries = m_h_numBad->GetBinContent(m_nEntriesBadBin);
  report << "Num channels with num pedestal amplitude entries < " << m_minAmpHistEntries 
    << ": " << numBadNEntries <<endl;

  double numBreachEntries = m_h_numBad->GetBinContent(m_onlTHoldBreachBadBin);
  report << "Num channels with too high an occupancy (> " << m_onlTHoldBreachMax << ")."
    << ": " << numBreachEntries <<endl;

  if(!(numBadPed >100 || numBadRms > 100 || numBadNEntries>0|| numBreachEntries > 0)){
    ofstream allGoodFile(m_allGoodFileName.c_str());
    allGoodFile << "All tests past.";
    allGoodFile.close();
  }

  ofstream reportFile(m_statusReportName.c_str());
  reportFile << m_statusReportPrefix;
  reportFile << "\n\n";
  reportFile << report.str();
  return;

}
