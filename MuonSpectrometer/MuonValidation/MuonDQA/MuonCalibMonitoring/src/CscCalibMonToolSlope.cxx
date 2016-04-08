/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//********************************************************************
//
// NAME:     CscCalibMonToolSlope.cxx
// PACKAGE:  MuonCalibMonitoring  
//
// AUTHORS: Caleb Parnell-Lampen <lampen@physics.arizona.edu> 
//
// ********************************************************************

#include "CscCalibMonToolSlope.h"

#include <cassert>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ToolHandle.h"

#include "CscCalibData/CscCalibReportContainer.h"
#include "CscCalibData/CscCalibReportSlope.h"

#include "TProfile.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TF1.h"

using namespace std;



CscCalibMonToolSlope::CscCalibMonToolSlope(const std::string & type, const std::string & name, 
    const IInterface* parent) : 
  CscCalibMonToolBase(type, name, parent),
  m_slopeBadBin(1),
  m_interceptBadBin(2),
  m_chi2BadBin(3),
  m_peaktBadBin(4),
  m_fracBadBin(5),
  m_deadBadBin(6),
  m_missingBadBin(7),
  m_totalLiveBin(1),
  m_totalDeadBin(2),
  m_newLiveBin(3),
  m_newDeadBin(4),
  m_h_slopeMissingBadBin(0),
  m_h_numBad(NULL),
  m_h_slopeCompareOverview(NULL),
  m_h_interceptOverview(NULL),
  m_h_chi2Overview(NULL),
  m_h_peaktCompareOverview(NULL),
  m_h_slopeMissingChans(NULL),
  m_h_deadOverview(NULL),
  m_slopeNewColl(NULL),
  m_slopeOldColl(NULL),
  m_slopeDiffColl(NULL),
  m_peaktNewColl(NULL),
  m_peaktOldColl(NULL),
  m_peaktDiffColl(NULL),
  m_interceptColl(NULL),
  m_chi2Coll(NULL),
  m_deadNewColl(NULL),
  m_deadDiffColl(NULL),
  m_slopeRatioColl(NULL),
  m_fitResColl(NULL),
  m_muon_mgr(NULL),
  m_expectedChamberLayer(0)
{
  declareProperty("MaxSlopeDiff",m_slopeMaxDiff=0.5);
  declareProperty("MaxIntercept",m_interceptMax = 5.0);
  declareProperty("MaxChi2_NDF",m_chi2Max = 100);
  declareProperty("MaxFracDev",m_fracDevMax = .5);
  declareProperty("MaxPeaktDiff",m_peaktMaxDiff= 10);
  m_calibResultKey = "CscCalibResultSlope";
  declareProperty("calGraphKey", m_histKey = "cscSlopeCalibReport");
  declareProperty("subDir", m_subDir ="");
  declareProperty("DeadPulserLevelCutoff", m_deadPulserLevelCutoff = 50);   
  declareProperty("DeadADCCutoff", m_deadADCCutoff =100); //Cutoff after ped subtraction
  declareProperty("DoNeighborRatio", m_doNeighborRatios = true);
  declareProperty("HistAttenLevels", m_histAttenLevels = false , "Make histograms for all attenuation levels. Potentially requires a LOT of RAM.");


  /*##From CscCalibMonToolBase.cxx, for reference. Do not uncomment!
    declareProperty("MakeHashValueHist",m_makeHashHist=true);
    declareProperty("MakeLayerValueHists",m_makeLayerHists=false);
    declareProperty("DetailedResultHashIds",m_detailedHashIds);
   */

}


CscCalibMonToolSlope::~CscCalibMonToolSlope()
{
  m_log << MSG::INFO << "CscCalibMonToolSlope :  deleting CscCalibMonToolSlope " << endreq;
  cerr << "Deleting CscCalibMonToolSlope" << endl;
}


StatusCode CscCalibMonToolSlope::finalize()
{
  m_log << MSG::DEBUG << "Slope Finalizing "<< endreq;
  delete m_slopeNewColl;
  delete m_slopeOldColl;
  delete m_slopeDiffColl;
  delete m_interceptColl;
  delete m_chi2Coll;
  delete m_deadNewColl;
  delete m_deadDiffColl;
  delete m_peaktNewColl;
  delete m_peaktOldColl;
  delete m_peaktDiffColl;


  return CscCalibMonToolBase::finalize();
}

StatusCode CscCalibMonToolSlope::initialize() 
{
  m_onlyExpectPrecisionHashIds = true; 
  StatusCode sc = CscCalibMonToolBase::initialize();

  IdContext channelContext = m_cscIdHelper->channel_context();

  m_log << MSG::DEBUG << "Expected chamber layer is " << m_expectedChamberLayer<< endreq;

  for(unsigned int hash = 0 ; hash <= m_maxHashId; hash++)
  {
    m_fracProfs.push_back(NULL);

    Identifier id;



  }
  m_generic_path_csccalibmonitoring = "MUON_CSC_PULSER";

  return sc;
}


StatusCode CscCalibMonToolSlope::bookHistograms()
{
  CscCalibMonToolBase::bookHistograms();
  if (m_debuglevel) m_log << MSG::DEBUG << "CscCalibMonToolSlope : in bookHistograms()" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  if (newLumiBlock){}
  if (newRun)
  {
    string name,title,xaxis,yaxis;
    int highbound,lowbound,nbins;

    string geoPath = getGeoPath();
    string path = getFullPath(geoPath, "Misc", "");
    MonGroup monGroup(this, path, run, ATTRIB_MANAGED );

    //num bad histograms
    name = "h_csc_calib_numSignificant";
    title = "Number of significant results.";
    xaxis = "Catagory";
    yaxis = "Num channels with bad value.";
    lowbound = 1;
    highbound = 8;
    m_h_numBad = new TH1I(name.c_str(),title.c_str(),highbound-lowbound,lowbound,highbound);
    m_h_numBad->GetYaxis()->SetTitle(yaxis.c_str());     
    m_h_numBad->GetXaxis()->SetTitle(xaxis.c_str());
    m_h_numBad->GetXaxis()->SetBinLabel(m_slopeBadBin,"slope diff");
    m_h_numBad->GetXaxis()->SetBinLabel(m_interceptBadBin,"intercept diff");
    m_h_numBad->GetXaxis()->SetBinLabel(m_chi2BadBin,"chi^2/ndf ");
    m_h_numBad->GetXaxis()->SetBinLabel(m_peaktBadBin,"Peaking Time Diff");
    m_h_numBad->GetXaxis()->SetBinLabel(m_fracBadBin,"Fractional Deviation");
    m_h_numBad->GetXaxis()->SetBinLabel(m_deadBadBin,"New (or fixed) Dead Channel");
    m_h_numBad->GetXaxis()->SetBinLabel(m_missingBadBin,"Num chans w/ no data");
    m_h_numBad->SetFillColor(m_histCol);
    sc = monGroup.regHist(m_h_numBad);

    //--overview histograms-----------------------------------------------------------------
    /*
       name = "h_csc_calib_peaktCompareOverview";


       title = "Differences between measured peaking time and values stored in condtions database";
       xaxis = "Difference (ns)";
       yaxis = "";
       lowbound = -10;
       highbound = 10;
       nbins = 100;
       m_h_peaktCompareOverview = new TH1F(name.c_str(),title.c_str(),nbins,lowbound,highbound);
       m_h_peaktCompareOverview->GetXaxis()->SetTitle(xaxis.c_str());
       m_h_peaktCompareOverview->GetYaxis()->SetTitle(yaxis.c_str());     
       m_h_peaktCompareOverview->SetFillColor(m_histCol);
       sc = monGroup.regHist(m_h_peaktCompareOverview);

       name = "h_csc_calib_slopeCompareOverview";
       title = "Differences between measured slope  and values stored in condtions database";
       xaxis = "Difference (fC/ADC)";
       yaxis = "";
       lowbound = -10;
       highbound = 10;
       nbins = 100;
       m_h_slopeCompareOverview = new TH1F(name.c_str(),title.c_str(),nbins,lowbound,highbound);
       m_h_slopeCompareOverview->GetXaxis()->SetTitle(xaxis.c_str());
       m_h_slopeCompareOverview->GetYaxis()->SetTitle(yaxis.c_str());     
       m_h_slopeCompareOverview->SetFillColor(m_histCol);
       sc = monGroup.regHist(m_h_slopeCompareOverview);

       name = "h_csc_calib_interceptOverview";
       title = "Intercepts";
       xaxis = "Intercept (ADC count)";
       yaxis = "";
       lowbound = 0;
       highbound = 10;
       nbins = 100;
       m_h_interceptOverview = new TH1F(name.c_str(),title.c_str(),nbins,lowbound,highbound);
       m_h_interceptOverview->GetXaxis()->SetTitle(xaxis.c_str());
       m_h_interceptOverview->GetYaxis()->SetTitle(yaxis.c_str());     
       m_h_interceptOverview->SetFillColor(m_histCol);
       sc = monGroup.regHist(m_h_interceptOverview);

       name = "h_csc_calib_chi2Overview";
       title = "chi^2/ndf for slope fits";
       xaxis = "chi^2/ndf";
       yaxis = "";
       lowbound = 0;
       highbound = 10;
       nbins = 100;
       m_h_chi2Overview = new TH1F(name.c_str(),title.c_str(),nbins,lowbound,highbound);
       m_h_chi2Overview->GetXaxis()->SetTitle(xaxis.c_str());
       m_h_chi2Overview->GetYaxis()->SetTitle(yaxis.c_str());     
       m_h_chi2Overview->SetFillColor(m_histCol);
       sc = monGroup.regHist(m_h_chi2Overview);
     */
    name = "h_csc_calib_deadOverview";
    title = "Number of dead channels";
    lowbound = 1;
    highbound = 5;
    nbins = 4;
    m_h_deadOverview = new TH1F(name.c_str(),title.c_str(),nbins,lowbound,highbound);
    m_h_deadOverview->GetXaxis()->SetBinLabel(m_totalLiveBin,"Total Live channels");
    m_h_deadOverview->GetXaxis()->SetBinLabel(m_totalDeadBin,"Total Dead channels");
    m_h_deadOverview->GetXaxis()->SetBinLabel(m_newLiveBin,"New Live Channels");
    m_h_deadOverview->GetXaxis()->SetBinLabel(m_newDeadBin,"New Dead Channels");
    m_h_deadOverview->SetFillColor(m_histColAlert);
    sc = monGroup.regHist(m_h_deadOverview);

    name = "h_csc_calib_slopeMissingChans";
    title = "Number of dead channels";
    xaxis = "Channel (Hash ID)";
    yaxis = "";
    lowbound = 0;
    highbound = m_maxHashId+1;
    nbins = m_maxHashId+1;
    m_h_slopeMissingChans = new TH1F(name.c_str(),title.c_str(),nbins,lowbound,highbound);
    m_h_slopeMissingChans->GetXaxis()->SetTitle(xaxis.c_str());
    m_h_slopeMissingChans->GetYaxis()->SetTitle(yaxis.c_str());     
    m_h_slopeMissingChans->SetFillColor(m_histColAlert);
    sc = monGroup.regHist(m_h_slopeMissingChans);


    string peaktDataName        = "peakt";
    string peaktDataTitle       = "Peaking Time";
    string peaktSubDir          = "Peakt";

    string slopeDataName        = "slope";
    string slopeDataTitle       = "Pulser Gain Slope";
    string slopeSubDir          = "Slope";
    
    string slopeRatioDataName        = "slopeRatio";
    string slopeRatioDataTitle       = "Ratio of N : N+1 Channel Slopes";
    string slopeRatioSubDir          = "SlopeRatio";

    string interceptDataName    = "intercept";  
    string interceptDataTitle    = "Intercept";
    string interceptSubDir      = "Intercept";

    string chi2DataName         = "chi2";
    string chi2DataTitle        = "Chi^2/ndf for gain slope fit";
    string chi2SubDir           = "Chi2";

    string deadDataName         = "dead";
    string deadDataTitle        = "Dead";
    string deadSubDir           = "Dead";

    string fitResDataName         = "fitRes";
    string fitResDataTitle        = "Fit Return Value";
    string fitResSubDir           = "FitResult";

    //Set naming parameters for histogram category names
    string newCatName       = "new";
    string newCatTitle      = "New";

    string oldCatName       = "old";
    string oldCatTitle      = "COOL";

    string diffCatName      = "diff";
    string diffCatTitle     = "Difference Between New and COOL";

    //axis info
    string peaktAxisLabel = "Peaking Time (ns)";
    string peaktDiffAxisLabel = "Peaking Time Difference (ns)";
    int peaktNumBins = 100;
    float peaktLowBound = 0;
    float peaktHighBound = 100;

    string slopeAxisLabel = "Gain (fC/ADC)";
    string slopeDiffAxisLabel = "Gain Difference (fC/ADC)";
    int slopeNumBins =300;
    float slopeLowBound = 0;
    float slopeHighBound = 5;
    
    string slopeRatioAxisLabel = "Ratio of N/(N+1) channel";
    int slopeRatioNumBins = 500;
    float slopeRatioLowBound = 0;
    float slopeRatioHighBound = 5;

    string interceptAxisLabel = "Intercept (ADC counts)";
    int interceptNumBins = 200;
    float interceptLowBound = -100;
    float interceptHighBound = 100;

    string chi2AxisLabel = "Chi^2/ndf";
    int chi2NumBins = 1000;
    float chi2LowBound = 0;
    float chi2HighBound = 3000; 

    string deadAxisLabel = "Is Dead";
    int deadNumBins = 3;
    float deadLowBound = -1.5;
    float deadHighBound = 1.5;

    string fitResAxisLabel = "Fit Return Value";
    int fitResNumBins =10000;
    float fitResLowBound = 0;
    float fitResHighBound = 10000;

    //Initialize histogram collections
    m_peaktNewColl = new HistCollection(m_maxHashId +1, m_maxHashId +1);
    m_peaktNewColl->ignoreY = false;
    m_peaktOldColl = new HistCollection(m_maxHashId +1);
    m_peaktOldColl->ignoreY = false;
    m_peaktDiffColl = new HistCollection(m_maxHashId +1);
    m_peaktDiffColl->ignoreY = false;


    if(m_doNeighborRatios){
      m_slopeRatioColl = new HistCollection(m_maxHashId +1);
      m_slopeRatioColl->ignoreY = true;
    }

    m_slopeNewColl = new HistCollection(m_maxHashId +1, m_maxHashId +1);
    m_slopeNewColl->ignoreY = true;
    m_slopeOldColl = new HistCollection(m_maxHashId +1);
    m_slopeOldColl->ignoreY = true;
    m_slopeDiffColl = new HistCollection(m_maxHashId +1);
    m_slopeDiffColl->ignoreY = true;

    m_interceptColl = new HistCollection(m_maxHashId +1, m_maxHashId +1);
    m_interceptColl->ignoreY = true;

    m_chi2Coll = new HistCollection(m_maxHashId +1);
    m_chi2Coll->ignoreY = true;


    m_deadNewColl = new HistCollection(m_maxHashId +1, m_maxHashId +1);
    m_deadNewColl->ignoreY = false;
    m_deadDiffColl = new HistCollection(m_maxHashId +1,m_maxHashId +1);
    m_deadDiffColl->ignoreY = false;

    m_fitResColl = new HistCollection(m_maxHashId +1);
    m_fitResColl->ignoreY = true;

    //Set subdirectory in monitoring root file

    //initialize, name, and book histograms in histogram collections using the bookHistCollection()
    //function

    sc = bookHistCollection(m_peaktNewColl, peaktDataName, peaktDataTitle, newCatName, 
        newCatTitle, peaktAxisLabel, peaktNumBins, peaktLowBound, peaktHighBound, peaktSubDir);
    if(!sc.isSuccess())
    {
      m_log << MSG::FATAL << "Failed to book hist"<< endreq;
      return StatusCode::FAILURE;
    }

    if(m_debuglevel) m_log << MSG::DEBUG << "Registering peaktOldColl" << endreq;
    sc = bookHistCollection(m_peaktOldColl, peaktDataName, peaktDataTitle, oldCatName, oldCatTitle,
        peaktAxisLabel, peaktNumBins, peaktLowBound, peaktHighBound, peaktSubDir);
    if(!sc.isSuccess())
    {
      m_log << MSG::FATAL << "Failed to book hist"<< endreq;
      return StatusCode::FAILURE;
    }

    if(m_debuglevel) m_log << MSG::DEBUG << "Registering peaktDiffColl" << endreq;
    sc = bookHistCollection(m_peaktDiffColl, peaktDataName, peaktDataTitle, diffCatName, diffCatTitle,
        peaktDiffAxisLabel, peaktNumBins, peaktLowBound, peaktHighBound, peaktSubDir);
    if(!sc.isSuccess())
    {
      m_log << MSG::FATAL << "Failed to book hist"<< endreq;
      return StatusCode::FAILURE;
    }

    if(m_debuglevel) m_log << MSG::DEBUG << "Registering slopeRatioColl" << endreq;
    sc = bookHistCollection(m_slopeNewColl, slopeDataName, slopeDataTitle, newCatName, 
        newCatTitle, slopeAxisLabel, slopeNumBins, slopeLowBound, slopeHighBound, slopeSubDir);
    if(!sc.isSuccess())
    {
      m_log << MSG::FATAL << "Failed to book hist"<< endreq;
      return StatusCode::FAILURE;
    }

    if(m_debuglevel) m_log << MSG::DEBUG << "Registering slopeNewColl" << endreq;
    sc = bookHistCollection(m_slopeRatioColl, slopeRatioDataName, slopeRatioDataTitle, "", 
        "", slopeRatioAxisLabel, slopeRatioNumBins, slopeRatioLowBound, slopeRatioHighBound, slopeRatioSubDir);
    if(!sc.isSuccess())
    {
      m_log << MSG::FATAL << "Failed to book hist"<< endreq;
      return StatusCode::FAILURE;
    }

    if(m_debuglevel) m_log << MSG::DEBUG << "Registering slopeOldColl" << endreq;
    sc = bookHistCollection(m_slopeOldColl, slopeDataName, slopeDataTitle, oldCatName, oldCatTitle,
        slopeAxisLabel, slopeNumBins, slopeLowBound, slopeHighBound, slopeSubDir);
    if(!sc.isSuccess())
    {
      m_log << MSG::FATAL << "Failed to book hist"<< endreq;
      return StatusCode::FAILURE;
    }

    if(m_debuglevel) m_log << MSG::DEBUG << "Registering slopeDiffColl" << endreq;
    sc = bookHistCollection(m_slopeDiffColl, slopeDataName, slopeDataTitle, diffCatName, diffCatTitle,
        slopeDiffAxisLabel, slopeNumBins, slopeLowBound, slopeHighBound, slopeSubDir);
    if(!sc.isSuccess())
    {
      m_log << MSG::FATAL << "Failed to book hist"<< endreq;
      return StatusCode::FAILURE;
    }

    if(m_debuglevel) m_log << MSG::DEBUG << "Registering " << interceptDataTitle << endreq;
    sc = bookHistCollection(m_interceptColl, interceptDataName, interceptDataTitle, "",
        "", interceptAxisLabel, interceptNumBins, interceptLowBound, interceptHighBound, 
        interceptSubDir);
    if(!sc.isSuccess())
    {
      m_log << MSG::FATAL << "Failed to book hist"<< endreq;
      return StatusCode::FAILURE;
    }

    sc = bookHistCollection(m_chi2Coll, chi2DataName, chi2DataTitle, "", "",
        chi2AxisLabel, chi2NumBins, chi2LowBound, chi2HighBound, chi2SubDir);
    if(!sc.isSuccess())
    {
      m_log << MSG::FATAL << "Failed to book hist"<< endreq;
      return StatusCode::FAILURE;
    }

    sc = bookHistCollection(m_deadNewColl, deadDataName, deadDataTitle, newCatName, newCatTitle,
        deadAxisLabel, deadNumBins, deadLowBound, deadHighBound, deadSubDir);
    if(!sc.isSuccess())
    {
      m_log << MSG::FATAL << "Failed to book hist"<< endreq;
      return StatusCode::FAILURE;
    }
    
    sc = bookHistCollection(m_fitResColl, fitResDataName, fitResDataTitle, "", "",
        fitResAxisLabel, fitResNumBins, fitResLowBound, fitResHighBound, fitResSubDir);
    if(!sc.isSuccess())
    {
      m_log << MSG::FATAL << "Failed to book hist"<< endreq;
      return StatusCode::FAILURE;
    }

  }
  return sc;

}

/*StatusCode CscCalibMonToolSlope::fillHistograms()
  {
  StatusCode sc = StatusCode::SUCCESS;

  if (m_debuglevel) 
  m_log << MSG::DEBUG << "CscCalibMonToolSlope :: in fillHistograms()" << endreq;

  return sc;
  }*/

//--handleParameter: Processes a vector of parameter values by filling the appropriate histograms
StatusCode CscCalibMonToolSlope::handleParameter(const CscCalibResultCollection* parVals)
{
  if (m_debuglevel) m_log << MSG::DEBUG << "CscCalibMonToolSlope : in handleParameter()" << endreq;

  //The whole point of this funciton is to pass the correct histograms and setup info 
  //to CsccalibMonToolBase::procParameter. To organize this, we store the setup info into
  //these structs: 
  ProcSetupInfo simpleSet;

  //--setup for this parameter
  string parName = parVals->parName();
  /*if(parName == "peakt")
    {
    m_log << MSG::INFO << "Evaluating peaking times" << endreq;
    simpleSet.dbName = parVals->parName();
    simpleSet.badHist = m_h_numBad;
    simpleSet.badBin = m_peaktBadBin;
    simpleSet.maxDiff = m_peaktMaxDiff;
    simpleSet.overDiff = m_h_peaktCompareOverview;
    simpleSet.overChi2 = m_h_chi2Overview;
    simpleSet.chi2BadBin = m_chi2BadBin;
    simpleSet.doChi2 = false;
    simpleSet.vals = &m_peaktNewColl->data;
    simpleSet.errors = &m_peaktNewColl->errors;
    simpleSet.oldVals = &m_peaktOldColl->data;
    simpleSet.diffs = &m_peaktDiffColl->data;
    simpleSet.missingBadBin = m_missingBadBin;
    simpleSet.missingChans = m_h_slopeMissingChans;
    simpleSet.expectedChannels = m_expectedHashIdsPrec;
    }
   */
  if(parName == "pslope")
  {
    m_log << MSG::INFO << "Evaluating slopes" << endreq;
    simpleSet.dbName = parVals->parName();
    simpleSet.badHist = m_h_numBad;
    simpleSet.badBin = m_slopeBadBin;
    simpleSet.maxDiff = m_slopeMaxDiff;
    simpleSet.overDiff = m_h_slopeCompareOverview;
    simpleSet.overChi2 = m_h_chi2Overview;
    simpleSet.chi2BadBin = m_chi2BadBin;
    simpleSet.chi2Max = m_chi2Max;
    simpleSet.doChi2 = true;
    simpleSet.vals = &m_slopeNewColl->data;
    simpleSet.errors = &m_slopeNewColl->errors;
    simpleSet.oldVals = &m_slopeOldColl->data;
    simpleSet.diffs = &m_slopeDiffColl->data;
    simpleSet.chi2s = &m_chi2Coll->data;
    simpleSet.missingBadBin = m_missingBadBin;
    simpleSet.missingChans = m_h_slopeMissingChans;
    simpleSet.expectedChannels = m_expectedHashIdsPrec;
  }
  else if (parName == "pinter")
  {
    m_log << MSG::INFO << "Evaluating intercepts" << endreq;
    simpleSet.expectedVal = 0;
    simpleSet.badHist = m_h_numBad;
    simpleSet.badBin = m_interceptBadBin;
    simpleSet.maxDiff = m_interceptMax;
    simpleSet.overDiff = m_h_interceptOverview;
    simpleSet.overChi2 = m_h_chi2Overview;
    simpleSet.doChi2 = false;
    simpleSet.vals = &m_interceptColl->data;
    simpleSet.errors = &m_interceptColl->errors;
    simpleSet.oldVals = NULL ;
    simpleSet.diffs = NULL ;
    simpleSet.missingBadBin = m_missingBadBin;
    simpleSet.missingChans = m_h_slopeMissingChans;
    simpleSet.expectedChannels = m_expectedHashIdsPrec;
  }
  else
  {
    m_log << MSG::INFO << "CscCalibMonToolSlope : Did not recognize parameter name " 
      << parName << ". This is usually ok." << endreq;
    return StatusCode::FAILURE;
  }

  //Process parameter by filling histograms in simpleSet and allIdsSet structures
  StatusCode sc = procParameter(parVals,&simpleSet);
  if(!sc.isSuccess())
  {
    m_log << MSG::FATAL << "CscCalibMonToolSlope : Failed to process parameter " << parName 
      << endreq;
    return StatusCode::FAILURE;
  }


  if(parName == "peakt")
  {
    m_log << MSG::INFO << "Generating peaking time histograms" << endreq;

    copyDataToHists(m_peaktNewColl);
    copyDataToHists(m_peaktOldColl);
    copyDataToHists(m_peaktDiffColl);
  }
  if(parName == "pslope") 
  {

    m_log << MSG::INFO << "Generating slope histograms" << endreq;
    
    if(m_doNeighborRatios){
      genNeighborRatios(m_slopeNewColl->data, m_slopeRatioColl->data);
      copyDataToHists(m_slopeRatioColl);
    }



    copyDataToHists(m_slopeNewColl);
    copyDataToHists(m_slopeOldColl);
    copyDataToHists(m_slopeDiffColl);
    copyDataToHists(m_chi2Coll);
  }
  if(parName == "pinter")
  {
    m_log << MSG::INFO << "Generating intercept histograms" << endreq;
    copyDataToHists(m_interceptColl);
  }
  return StatusCode::SUCCESS;
}

//--retrieveHistos() will retrieve the slope amplitude histograms for the channels
//requested by the user in m_detailedHashIds. 
StatusCode CscCalibMonToolSlope::postProc()
{
  if (m_debuglevel) m_log << MSG::DEBUG << "CscCalibMonToolSlope : in retrieveHistos()" << endreq;

  if (m_debuglevel) m_log << MSG::DEBUG << "Do all detailed is: " << m_doAllDetailed << endreq;
  StatusCode sc = StatusCode::SUCCESS;



  IdContext chanContext = m_cscIdHelper->channel_context();

  //Get the slopeReport, checking for pointer errors along the way
  const DataHandle<CscCalibReportContainer> repCont;
  sc = m_storeGate->retrieve(repCont, m_histKey);
  if( !sc.isSuccess())
  {
    m_log << MSG::ERROR << " Cannot retrieve object from storegate with key "
      << m_histKey <<  " aborting retrieving hists " << endreq;
    return StatusCode::RECOVERABLE;
  }
  if(repCont->size() != 1)
  {
    m_log << MSG::ERROR << "Container with key " << m_histKey
      << " does not have a size of one. Do not know how to proceed, so aborting"
      << " retrieving calibration histograms." << endreq;
    return StatusCode::RECOVERABLE;
  }

  const CscCalibReportSlope * slopeReport =
    dynamic_cast<const CscCalibReportSlope *>(repCont->front());
  if(!slopeReport)
  {
    m_log << MSG::ERROR << "No report stored in the container with key " << m_histKey
      << ". Aborting retrieving histograms." << endreq;
    return StatusCode::RECOVERABLE;
  }
  if(slopeReport->getLabel() != "calGraphs")
  {
    m_log << MSG::ERROR << "Incorect object retrieved from container with key " << m_histKey
      << ". Aborting hist retrieval" << endreq;
    return StatusCode::RECOVERABLE;
  }
  
 //ampProfs 
  const map<int,TProfile*> * ampProfs = slopeReport->getAmpProfs();
  if(!ampProfs)
  {
    m_log <<MSG::ERROR << "There are no amplitude profiles in the slope report! Can't find dead chans."
      <<endreq;
    return StatusCode::RECOVERABLE;
  }

  if(m_histAttenLevels){
    map<int,TProfile*>::const_iterator profItr = ampProfs->begin();
    map<int,TProfile*>::const_iterator profEnd = ampProfs->end();
    for(; profItr != profEnd; profItr++){

      float atten = profItr->first/2.0;
      stringstream attenSS; attenSS << atten;
      string attenStr = attenSS.str();

      HistCollection * ampColl = new HistCollection(m_maxHashId +1, m_maxHashId +1);
      ampColl->ignoreY = true;

      //put into ampColls to make for easy deleting later
      m_ampColls.push_back(ampColl);


      string dataName = "amp_atten" + attenStr;
      string dataTitle = "ADC response at attenuation " + attenStr + " db" ; 
      string axisLabel = "ADC";
      unsigned int numBins = 300; 
      float lowBound = 0;
      float highBound = 3000;
      string subDir = "AmpAtten" + attenStr;

      sc = bookHistCollection(ampColl, dataName, dataTitle, "", "", axisLabel, numBins, lowBound, highBound, subDir);
      for(unsigned int stripHash = 0; stripHash < m_maxHashId; stripHash++){
        ampColl->data[stripHash] =  profItr->second->GetBinContent(stripHash +1);
      }

      copyDataToHists(ampColl);
    }
  }


  const std::vector<float> * fitResVec = slopeReport->getFitResults();
  m_fitResColl->data = *fitResVec; 
  copyDataToHists(m_fitResColl);

  //Generate fractional deviation histograms
  if(m_debuglevel) m_log << MSG::DEBUG << "About to generate fractional deviation graphs" 
    << endreq;
  if(!makeFracGraphs(*slopeReport).isSuccess())
    m_log << MSG::WARNING << "Failed to generate fractional deviation graphs. Continuing anyway.." 
      << endreq;


  if(m_debuglevel) m_log << MSG::DEBUG << "About to find dead channels" 
    << endreq;
  //Determine dead channels
  //sc = findDeadChannels(*slopeReport);
  //if(!sc.isSuccess())
  //  m_log << MSG::WARNING << "Failure while finding dead channels" << endreq;


  //Put extra info for those channels indicated in m_detailedHashIds
  if(m_debuglevel) m_log << MSG::DEBUG << "Picking detailed graphs to output to root file" 
    << endreq;
  if(m_numBad >0 || m_maxDetailedChannels < 0 || m_doAllDetailed)
  { 
    const DataVector<TGraphErrors> * calGraphs 
      = slopeReport->getCalGraphs();
    if(!calGraphs)
    {
      m_log << MSG::ERROR << "No calGraph stored inside object with key " << m_histKey
        << ". Aborting hist retrieval." << endreq;
      return StatusCode::RECOVERABLE;
    }
    else
      m_log <<MSG::INFO << "Got calGraphs" << endreq;

    const DataVector<TH1I> * bitHists = slopeReport->getBitHists();
    if(!bitHists)
      m_log << MSG::INFO << "No bit histogram vector found from calibration. "
        << " Won't be in monitoring output file. " << endreq;
    else
      m_log <<MSG::INFO << "Got bitHists" << endreq;


    //These are the channels we will get detailed forr.
    for(unsigned int idItr = 0; idItr < m_maxHashId; idItr++)
    {
      cerr << "Calgraph Address: " <<  (*calGraphs)[idItr] << endl;
      if(m_expectedHashIdsPrec.count(idItr) && (m_detailedHashIds[idItr] || (m_doAllDetailed) ) )
      {
        cerr << "Doing detailed plots of hash " << idItr << endl;

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
        string calGraphPath = getFullPath(geoPath, "CalGraphs","");
        string fracPath = getFullPath(geoPath,"FracProf","");
        string bitHistPath = getFullPath(geoPath, "BitHists", "");

        //Record calgraph
        TGraphErrors  * sourceGraph = 
          const_cast<TGraphErrors*>((*calGraphs)[idItr]);
        if(!sourceGraph)
        {
          m_log << MSG::ERROR << "The requested calgraph for hash "
            << idItr << " doesn't exist in CscCalibReport object!" << endreq;
        }
        else {
          stringstream name;
          name << "calfit" 
            << "_EC" << getEndCap(stationEta)
            << "_sector_" << sector 
            << "_layer_" << wireLayer
            << "_" << (measuresPhi ? "trans" : "prec")
            << "_strip_" 
            << setfill('0') <<  setw(measuresPhi ? 2 : 3) 
            << strip;

          sourceGraph->SetName(name.str().c_str());
          m_log << MSG::DEBUG << "CalGraph axis title: " << sourceGraph->GetXaxis()->GetTitle() << endreq;

          regGraph(sourceGraph, calGraphPath, run, ATTRIB_MANAGED);
        }

        //record fracDev graph
        TProfile * sourceProf = m_fracProfs[idItr];
        if(!sourceProf)
        {
          m_log << MSG::ERROR << "There is no fractional profile available for hash " 
            << idItr << ". Quitting retrieveHistos()." << endreq;
        }
        else{
          stringstream fracName;
          fracName << "frac_"
            << "_EC" << getEndCap(stationEta)
            << "_sector_" << sector 
            << "_layer_" << wireLayer
            << "_" << (measuresPhi ? "trans" : "prec")
            << "_strip_" 
            << setfill('0') <<  setw(measuresPhi ? 2 : 3) 
            << strip;

          sourceProf->SetName(fracName.str().c_str());

          regHist(sourceProf, fracPath, run, ATTRIB_MANAGED);
        }

        //Bit map histograms
        //copy source histogram into new histogram, and store
        if(bitHists)
        {
          TH1I *  bitHist = const_cast<TH1I*>((*bitHists)[idItr]);
          if(!bitHist)
          {
            m_log << MSG::ERROR << "There is no bit histogram with hashId "
              << idItr << " Quiting out of detailed histogram loop." <<  endreq;
          }
          else {

            stringstream name2;
            name2 << "h_bitMap"
              << "_EC" << getEndCap(stationEta)
              << "_sector_" << sector 
              << "_layer_" << wireLayer
              << "_" << (measuresPhi ? "trans" : "prec")
              << "_strip_" 
              << setfill('0') <<  setw(measuresPhi ? 2 : 3) 
              << strip;
            // TH1I * newHist2 = (TH1I*)bitHist->Clone(name2.str().c_str());
            bitHist->SetName(name2.str().c_str());
            bitHist->SetFillColor((m_detailedHashIds[idItr] ? m_histColAlert : m_histCol));

            //regHist(newHist2, chanPath, run, ATTRIB_MANAGED);
            regHist(bitHist, bitHistPath, run, ATTRIB_MANAGED);
          }
        }//end if bithists*/

      }//end if (m_detailedHashIds)
      else
        cerr << "Skipping hash " << idItr << " " <<  m_expectedHashIdsPrec.count(idItr) << " " << m_doAllDetailed << endl;
    }//end idItr loop
  }//if numBad > 0
  return StatusCode::SUCCESS;    
}// end retrieveHistos

//Generate fractional Deviation graphs
StatusCode CscCalibMonToolSlope::makeFracGraphs(const CscCalibReportSlope & slopeReport)
{
  if (m_debuglevel) m_log << MSG::DEBUG << "CscCalibMonToolSlope : in makeFracGraphs()" << endreq;
  const DataVector<TGraphErrors> * calGraphs = slopeReport.getCalGraphs();
  if(!calGraphs)
  {
    m_log << MSG::ERROR << "No calGraphs in slopeReport. Not going to make fractional deviation"
      << " plots." << endreq;
    return StatusCode::RECOVERABLE;
  }

  //Loop through all channels in geometry:
  vector<Identifier> ids = m_cscIdHelper->idVector();
  vector<Identifier>::const_iterator chamItr = ids.begin();
  vector<Identifier>::const_iterator chamEnd = ids.end();
  for(; chamItr != chamEnd; chamItr++)
  {
    if(m_verboselevel) m_log << MSG::VERBOSE << "in Chamber loop " << endreq;
    unsigned int stationSize = m_cscIdHelper->stationName(*chamItr); //51 = large, 50 = small
    unsigned int stationPhi = m_cscIdHelper->stationPhi(*chamItr);
    int stationEta = m_cscIdHelper->stationEta(*chamItr);
    unsigned int sector = getSector(stationPhi,stationSize);

    vector<Identifier> stripVect;
    m_cscIdHelper->idChannels(*chamItr,stripVect);
    vector<Identifier>::const_iterator stripItr = stripVect.begin();
    vector<Identifier>::const_iterator stripEnd = stripVect.end();
    for(;stripItr != stripEnd; stripItr++)
    {
      if(m_verboselevel) m_log << MSG::VERBOSE << "in strip loop " << endreq;
      IdentifierHash stripHash;
      m_cscIdHelper->get_channel_hash(*stripItr,stripHash);
      if(!m_expectedHashIdsPrec.count((int)stripHash)){
        m_log << MSG::VERBOSE << "Skipping hash"  << (int)stripHash << endreq;
        continue;
      }
      if(m_verboselevel) m_log << MSG::VERBOSE << "strip hash " << stripHash <<  endreq;

      const TGraphErrors * graph = (*calGraphs)[stripHash];
      if(!graph)
      {
        if(m_verboselevel) m_log << MSG::VERBOSE << "SKipping graph" << endreq;
        continue;
      }
      
      TF1 * func = graph->GetFunction("simpleFunc");
      if(!func){
        m_log << MSG::DEBUG << "Could not retrieve function, skipping this graph" << endreq;
        continue;
      }

      if(m_verboselevel)
      {

        m_log << MSG::VERBOSE << "Address is " << graph<< endreq;

        m_log << MSG::VERBOSE << "name is "<< graph->GetName() << endl;
        m_log << MSG::VERBOSE << "Getting n " << endl;
      }
      int nPoints = graph->GetN();
      if(m_verboselevel)m_log << MSG::VERBOSE << "Got n " << endl;
      //Get identification information
      //Note, we don't ask for measuresPhi because there should be no
      //TGraphs with Y anyways.
      if(m_verboselevel) m_log << MSG::VERBOSE << "getting id info " << endreq;
      unsigned int layer = m_cscIdHelper->wireLayer(*stripItr);
      unsigned int strip = m_cscIdHelper->strip(*stripItr);
      if(m_verboselevel) m_log << "Got strip and layer" << endl;
      //initialize fractional deviation profile
      if(m_verboselevel) m_log << MSG::VERBOSE << "initializing profile " << endreq;

      stringstream nameStream;
      nameStream.setf(ios::right,ios::adjustfield);
      nameStream << "dev_" 
        << "X"   //orientation
        << "_eta_" << stationEta
        << "_sector_" << setw(2) << setfill('0') << sector
        << "_layer_" << layer
        << "_strip_" << strip;
      stringstream titleStream;
      titleStream << "Fractional Deviation of Measured ADC From Fit ADC for Precision Direction"
        << ", Sector " << sector
        << ", Eta "     << stationEta
        << ", Layer " << layer
        << ", Strip " << strip;
      TProfile * fracProf 
        = new TProfile(nameStream.str().c_str(), titleStream.str().c_str(), nPoints, 
            1, nPoints +1);
      fracProf->GetXaxis()->SetTitle("Pulser Attenuator Settings in dB (not auscaled axis) ");
      fracProf->GetYaxis()->SetTitle("ADC response"); 

      if(m_verboselevel) m_log << MSG::VERBOSE << "getting parameters " << endreq;
      float intercept = func->GetParameter(0);
      float slope = func->GetParameter(1);
      if(m_verboselevel) m_log << MSG::VERBOSE << "got them " << endreq;

      //Loop through points in graph, and compare the adc value and charge
      //to that expected from the fit parameters from the TF1 object
      double adc, db, calcAdc;
      bool isBad = false;
      for(int itr = 0; itr < nPoints; itr++)
      {
        if(m_verboselevel) m_log << MSG::VERBOSE << "in point loop on point " << itr 
          << endreq;

        //Find Fractional Deviation
        graph->GetPoint(itr,db,adc);
        calcAdc = intercept + slope*std::pow(10,db/-20);
        double frac = (adc - calcAdc)/( calcAdc - intercept);

        //Test to see if this is a bad result
        if(m_verboselevel) m_log << MSG::VERBOSE << "bad result test " << endreq;
        if(frac > m_fracDevMax)
        {
          m_h_numBad->Fill(m_fracBadBin);
          isBad = true;
        }

        //test for nan, and put into fracProf 
        if (frac==frac)
        {
          if(m_verboselevel) m_log << MSG::VERBOSE << "filling fracProf " << endreq;
          fracProf->Fill(itr +1, frac);     
        }

        //Label bin with db amount
        if(m_verboselevel) m_log << MSG::VERBOSE << "labeling bin " << endreq;
        stringstream binLabel;
        binLabel << db;    
        fracProf->GetXaxis()->SetBinLabel(itr+1, binLabel.str().c_str());
      }
      if(m_verboselevel) m_log << MSG::VERBOSE << "storing fracProf " << endreq;
      //Store fraction graph for later
      if(stripHash > m_maxHashId)
      {
        m_log << MSG::ERROR << "Tried to assign fracProf with stripHash " << stripHash
          << " when max stripHash is " << m_maxHashId << endreq;
        return StatusCode::RECOVERABLE;
      }

      m_fracProfs[stripHash] = fracProf; 
      //Record if this was a bad channel
      if(isBad && ((int)m_numBad <= m_maxDetailedChannels) )
      {
        if(!m_detailedHashIds[stripHash])
        {
          m_numBad++;
          m_detailedHashIds[stripHash] = true;
        }
      }

    }//end loop over strips
  }//end loop over chambers
  m_log << MSG::DEBUG << "Finished generating frac graphs" << endreq;

  return StatusCode::SUCCESS;
}//end makeFracGraphs

StatusCode CscCalibMonToolSlope::findDeadChannels(const CscCalibReportSlope & slopeReport)
{//This function has grown a bit unwieldly, and duplicates info in sets and arrays. Can this be merged?

  //****Find Dead channels
  IdContext channelContext = m_cscIdHelper->channel_context(); 

  set<int> newDead, newUndead;

  const set<int> * pulsedChambers = slopeReport.getPulsedChambers();
  if(!pulsedChambers)
  {
    m_log <<MSG::ERROR << "No pulsed chambers stored in slopeReport! Skipping dead channel collecting!" << endreq;
    return StatusCode::RECOVERABLE;
  }

  const map<int,TProfile*> * ampProfs = slopeReport.getAmpProfs();
  if(!ampProfs)
  {
    m_log <<MSG::ERROR << "There are no amplitude profiles in the slope report! Can't find dead chans."
      <<endreq;
    return StatusCode::RECOVERABLE;
  }

  map<int,TProfile*>::const_iterator profItr = ampProfs->begin();

  int pulserLevel = profItr->first;
  m_log << MSG::INFO << "Looking for dead channels. Lowest attenuation level is " 
    << pulserLevel << endreq;
  m_log << MSG::INFO << "Dead channel cutoff is: "<< m_deadADCCutoff; 

  if(pulserLevel < m_deadPulserLevelCutoff)
  {//Pulser level is low enough (pulse voltage is high enough) to test for dead channels

    const TProfile * ampProf = profItr->second;
    if(!ampProf)
    {
      m_log << MSG::ERROR << "There is no profile for this attenuation level! Skipping dead channel finding!"
        << endreq;
      return StatusCode::RECOVERABLE;
    }

    if(m_debuglevel) m_log << MSG::DEBUG << "There were " << pulsedChambers->size()
      << " chambers pulsed." << endreq;

    //Prepare dead channel content
    std::vector<float> & currentDeadVals = m_deadNewColl->data;
    std::vector<float> & diffDeadVals = m_deadDiffColl->data;

    setArray(currentDeadVals,0,m_maxHashId+1);
    setArray(diffDeadVals,0,m_maxHashId+1);

    //Loop through each channel in TProfile (bin = hash + 1), and check value to see
    //if its dead
    float adc, ped;
    bool wasDead, isDead;
    uint8_t statusWord;
    for(unsigned int hashItr = 0; hashItr <= m_maxHashId ; hashItr++)
    {
      Identifier id;
      m_cscIdHelper->get_id(hashItr,id, &channelContext);
      int chamberLayer = m_cscIdHelper->chamberLayer(id);
      IdentifierHash chamberHash;
      m_cscIdHelper->get_module_hash(id, chamberHash);

      if(chamberLayer == 2 && pulsedChambers->count((int)chamberHash))
      {//This is a good chamber layer and it is a pulsed chamber

        m_cscCoolSvc->getStatus(statusWord, hashItr);
        m_cscCoolSvc->getParameter(ped,"ped", hashItr);
        wasDead = statusWord & 0x1;

        adc = ampProf->GetBinContent( hashItr + 1 );
        isDead = ( (adc-ped) < m_deadADCCutoff);

        if(isDead && !wasDead)
        {
          m_log << MSG::INFO << "NEW DEAD CHANNEL! Hash: " << hashItr << " ped: " << ped << " adc " << adc << " diff: " << adc-ped <<  endreq;
          newDead.insert(hashItr);
          m_h_deadOverview->Fill(m_newDeadBin);
          diffDeadVals[hashItr] = 1;
          m_detailedHashIds[hashItr] = true;
        }
        if(!isDead && wasDead)
        {
          m_log << MSG::INFO << "PREVIOUSLY DEAD CHANNEL NOW LIVE! Hash: " << hashItr << endreq;
          newUndead.insert(hashItr);
          m_h_deadOverview->Fill(m_newLiveBin);
          diffDeadVals[hashItr] = -1;
          m_detailedHashIds[hashItr] = true;
        }

        if(isDead)
        {
          currentDeadVals[hashItr] = 1;
          m_h_deadOverview->Fill(m_totalDeadBin);
        }
        else
          m_h_deadOverview->Fill(m_totalLiveBin);

      }//end if chamberLayer

    }//End for(hashItr)

    //***Generate histograms
    m_log << MSG::INFO << "Generating dead histograms" << endreq;
    copyDataToHists(m_deadNewColl);

    //copyDataToHists(m_deadDiffColl); 
    m_log << MSG::INFO << "Finished generating dead histograms" << endreq;

    //***Fill COOL input file if there are new dead channels******
    if(newDead.size() || newUndead.size())
    {
      m_log << MSG::INFO << "There are " << newDead.size() 
        << " newly dead channels and " << newUndead.size() 
        << " newly live channels" 
        << endreq;
      ofstream out("deadInfo.cal");
      out <<"00-00 " << newDead.size() + newUndead.size() << " dead_stat END_HEADER\n";

      set<int>::const_iterator deadItr = newDead.begin(); 
      set<int>::const_iterator deadEnd = newDead.end();
      for(; deadItr != deadEnd; deadItr++)
      {
        Identifier id;
        m_cscIdHelper->get_id(*deadItr,id, &channelContext);
        IdentifierHash chamHash;
        m_cscIdHelper->get_module_hash(id, chamHash);
        out << *deadItr << " " << (int)chamHash << " " 
          << m_cscIdHelper->show_to_string(id, &channelContext) << " 1\n";
      }

      set<int>::const_iterator undeadItr = newUndead.begin(); 
      set<int>::const_iterator undeadEnd = newUndead.end();
      for(; undeadItr != undeadEnd; undeadItr++)
      {
        Identifier id;
        m_cscIdHelper->get_id(*undeadItr,id, &channelContext);
        IdentifierHash chamHash;
        m_cscIdHelper->get_module_hash(id, chamHash);
        out << *undeadItr << " " << (int)chamHash << " " 
          << m_cscIdHelper->show_to_string(id, &channelContext)
          << "0\n";
      } 
      out.close();
    }//end if newDead.size()

  }//end if(pulserLevel < cutoff)
  else 
  {
    m_log << MSG::ERROR
      << "Lowest pulser level isn't low enough to count as a dead channel test. Skipping." 
      << endreq;
    return StatusCode::RECOVERABLE;
  }


  return StatusCode::SUCCESS;
}

//I changed the array to a vector. At some point I should replace code like this to make more sense for vector
std::vector<float> & CscCalibMonToolSlope::setArray(std::vector<float>  & array, const float &value, const int &numEntries)
{
  for(int i = 0 ; i < numEntries; i++)
    array[i] = value;
  return array;
}
       
void CscCalibMonToolSlope::genNeighborRatios(const std::vector<float> & source, std::vector<float> & ratios) const{
  size_t nEntries = source.size();
  if(nEntries != ratios.size()){
    m_log << MSG::ERROR << " in genNeighborRatios, source (" << nEntries << ") and ratio (" << ratios.size() 
      << ") vectors have different numbers of entries!" << endreq;
    return;
  }

  //loop through every channel, taking the ratio between it and the next
  size_t nloops = nEntries -1; //Can't do this to the last channel
  for(size_t cnt=0; cnt < nloops; cnt++){
    ratios[cnt] = source[cnt]/source[cnt+1];
  }
}

