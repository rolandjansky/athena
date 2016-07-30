/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Chrono.h"

#include "TH1.h"
#include "TF1.h"
#include "TFile.h"

#include "StoreGate/StoreGate.h"

#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"

#include "MuonRDO/CscRawData.h"
#include "MuonRDO/CscRawDataCollection.h"
#include "MuonRDO/CscRawDataContainer.h"

#include "CscCalibData/CscCalibResultContainer.h"
#include "CscCalibData/CscCalibReportContainer.h"
#include "CscCalibData/CscCalibReportPed.h"

#include <string>
#include <iostream>
#include <fstream>
#include <bitset>
#include <inttypes.h>
//#include <math>

#include "CscCalcPed.h"

using namespace std;

namespace MuonCalib {

  CscCalcPed::CscCalcPed(const string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name,pSvcLocator),
    m_storeGate(NULL),
    m_detStore(NULL),
    m_cscId(NULL),
    m_muon_mgr(NULL),
    m_chronoSvc(0),
    m_cscCoolStrSvc("MuonCalib::CscCoolStrSvc",name),
    m_cscRdoDecoderTool ("Muon::CscRDO_Decoder"),
    m_maxStripHash(0),
    m_numBits(12),
    m_ampHists(NULL),
    m_sampHists(NULL),
    m_bitHists(NULL),
    m_bitProds(NULL),
    m_bitCorrelation(NULL),
    m_peds(NULL),
    m_noises(NULL),
    m_rmses(NULL),
    m_f001s(NULL),
    m_onlineTHoldBreaches(NULL),
    m_eventCnt(0),
    m_crossTalkFix(NULL),
    m_debug(0),
    m_verbose(0)
  {
    declareProperty("OutputFile", m_outputFileName = "output.cal");
    declareProperty("TitlePrefix",m_titlePrefix = "");	//Prefix appended to title of histograms and graphs
    declareProperty("TitlePostfix",m_titlePostfix = "");	//Prefix appended to title of histograms and graphs
    declareProperty("CscRdoDecoderTool",   m_cscRdoDecoderTool );
    declareProperty("PedAmpHighBound", m_ampHistHighBound = 2600);
    declareProperty("PedAmpLowBound", m_ampHistLowBound = 1800); 
    declareProperty("PedAmpNumBins", m_ampHistNumBins = 800);
    //declareProperty("PedAmpHighBound", m_ampHistHighBound = 2300);
    //declareProperty("PedAmpLowBound", m_ampHistLowBound = 1900); 
    //declareProperty("PedAmpNumBins", m_ampHistNumBins = 400);
    declareProperty("ExpectedChamberLayer", m_expectedChamberLayer = 2);
    declareProperty("ThresholdMultiplier", m_thresholdMultiplier = 3.1);
    declareProperty("DoBitHists", m_doBitHists = false);
    declareProperty("CalOutputVersion", m_calOutputVersion="03-00");
    declareProperty("DoCorrelation", m_doCorrelation = false);

    declareProperty("DoSampHists", m_doSampleHists = false);
    declareProperty("NumSamplesExpected", m_numSamplesExpected = 4);
    declareProperty("DoF001", m_doF001 = true);

    declareProperty("OnlineCalibFile", m_onlineDbFile = "", "File with data currently stored in online configuration database");
    declareProperty("CompareOnlineCalibFile",m_doOnlineDbFile = true, "Compare new to online data?");


    //Can't do correlation without bitHists
    if(!m_doBitHists){ 
      m_doCorrelation = false;
      //MsgStream mLog( msgSvc(), name() );
      // mLog << MSG::WARNING 
      //   << "Tried to do correlation without bitHists. This isn't possible." << endmsg;
    }

  }

  CscCalcPed::~CscCalcPed() {}

  StatusCode CscCalcPed::initialize()
  {
    MsgStream mLog( msgSvc(), name() );

    //Set debug level    
    m_debug = (mLog.level() <= MSG::DEBUG);
    m_verbose = (mLog.level() <= MSG::VERBOSE);

    mLog << MSG::INFO << "CscCalcPed::initialize() called" << endmsg;

    if(m_doOnlineDbFile && m_onlineDbFile ==""){
      mLog << MSG::FATAL <<"Either specify an OnlineCalibFile or set CompareOnlineCalibFile to false"<< endmsg;
      return StatusCode::FAILURE;
    }

    //*******Register services and tools *********/ 	
    /// Store Gate active store
    StatusCode sc = serviceLocator()->service("StoreGateSvc", m_storeGate);
    if (sc != StatusCode::SUCCESS )
    {
      mLog << MSG::ERROR << " Cannot get StoreGateSvc " << endmsg;
      return sc ;
    }

    sc = serviceLocator()->service("DetectorStore", m_detStore);

    if(sc.isSuccess()) 
    {
      sc = m_detStore->retrieve(m_cscId,"CSCIDHELPER");
      if( sc.isFailure())
      {
        mLog << MSG::ERROR << " Cannot retrieve CscIdHelper " << endmsg;
        return sc;
      }
    }	
    else
    {
      mLog << MSG::ERROR << "couldn't retrieve CscIdHelper" << endmsg;

      return StatusCode::FAILURE;
    }

    if(m_cscCoolStrSvc.retrieve().isFailure())
    {
      mLog << MSG::FATAL << "Unable to retrieve CscCoolStrSvc" << endmsg;
      return StatusCode::FAILURE;
    }

    sc = service("ChronoStatSvc",m_chronoSvc);    
    if(sc.isFailure())
    {
      mLog << MSG::FATAL << "Cannot retrieve ChronoStatSvc!" << endmsg;
      return StatusCode::FAILURE;
    }

    if (m_cscRdoDecoderTool.retrieve().isFailure()){
      mLog << MSG::FATAL << "Cannot retrieve Csc_Rdo_Decoder Tool!" << endmsg;
      return StatusCode::FAILURE;
    }
    /*sc = service("THistSvc", m_thistSvc);
      if(sc.isFailure())
      {
      mLog << MSG::FATAL << "Cannot retrieve THistSvc!" << endmsg;
      return StatusCode::FAILURE;
      }*/

    IToolSvc* toolSvc;
    sc = service("ToolSvc",toolSvc);
    if(sc.isFailure())
    {
      mLog << MSG::ERROR << "Unable to retrieve ToolSvc" << endmsg;
      return StatusCode::FAILURE;
    }

    //Set to SG::VIEW_ELEMENTS, since we want root to do its own memory
    //management.
    m_ampHists = new DataVector<TH1I>(SG::VIEW_ELEMENTS);
    if(m_doSampleHists) m_sampHists = new DataVector< DataVector<TH1I> >;
    if(m_doBitHists) m_bitHists = new DataVector<TH1I>(SG::VIEW_ELEMENTS);

    //Loop through ids to find out what hash range we're working on (in case we're using some 
    //unusual geometry)
    vector<Identifier> ids = m_cscId->idVector();
    vector<Identifier>::const_iterator chamItr = ids.begin();
    vector<Identifier>::const_iterator chamEnd = ids.end();
    m_maxStripHash = 0;

    for(; chamItr != chamEnd; chamItr++)
    {
      vector<Identifier> stripVect;
      m_cscId->idChannels(*chamItr,stripVect);

      vector<Identifier>::const_iterator stripItr = stripVect.begin();
      vector<Identifier>::const_iterator stripEnd = stripVect.end();
      for(;stripItr != stripEnd; stripItr++)
      {
        IdentifierHash stripHash;
        m_cscId->get_channel_hash(*stripItr,stripHash);
        if((unsigned int)stripHash > m_maxStripHash)
          m_maxStripHash = (int)stripHash;
      }//End strip loop
    }//end chamber loop		


    //Now creating ampHists. It wasn't done in last loop since there 
    //is no gaurantee that it will come out in strip order, and we assume
    //later that m_ampHists's index = stripHash
    if(m_debug) mLog << MSG::DEBUG << "Preparing ampHists. Only allowing those for "
      << " chamberLayer " << m_expectedChamberLayer << endmsg;
    for(unsigned int stripItr = 0 ; stripItr <=m_maxStripHash; stripItr++)
    {        

      IdentifierHash stripHash =stripItr;
      Identifier stripId;
      IdContext channelContext = m_cscId->channel_context();	
      m_cscId->get_id(stripHash, stripId, &channelContext);

      int chamLayer = m_cscId->chamberLayer(stripId);
      if(chamLayer == m_expectedChamberLayer) //Only second chamber layer exists
      {
        int stationEta = m_cscId->stationEta(stripId);
        int stationPhi = m_cscId->stationPhi(stripId);
        int stripNumber = m_cscId->strip(stripId);
        int wireLayer = m_cscId->wireLayer(stripId);
        char orientation = m_cscId->measuresPhi(stripId) ? 'Y':'X';


        char name[30],titleSeed[600];
        TH1I* hist = NULL;

        int stationName = m_cscId->stationName(stripId);		
        //Amplitude histogram
        sprintf(name, "ampHist%u",stripItr);
        sprintf(titleSeed, "Amplitude Histogram for eta %d, sector %d, layer %d%c, strip %d",
            stationEta,(2*stationPhi+50 - stationName),wireLayer,orientation,stripNumber);
        string title = m_titlePrefix + titleSeed + m_titlePostfix;

        hist = new TH1I(name,title.c_str(),m_ampHistNumBins,m_ampHistLowBound,
            m_ampHistHighBound);
        hist->GetXaxis()->SetTitle("Amplitude (ADC value)");
        hist->GetYaxis()->SetTitle("Counts");
        m_ampHists->push_back(hist);

        if(m_doSampleHists) {
          DataVector<TH1I>* tempVect  = new DataVector<TH1I>(SG::VIEW_ELEMENTS);
          for(int cnt = 0; cnt < m_numSamplesExpected ; cnt++) {
            sprintf(name, "sampHist%u_%d",stripItr,cnt);
            sprintf(titleSeed, "Amplitude Histogram for eta %d, sector %d, layer %d%c, strip %d, sample %d",
                stationEta,(2*stationPhi+50 - stationName),wireLayer,orientation,stripNumber,cnt);

            hist = new TH1I(name,title.c_str(),m_ampHistNumBins,m_ampHistLowBound,
                m_ampHistHighBound);
            tempVect->push_back(hist);

          }
          m_sampHists->push_back(tempVect);
        }

        if(m_bitHists)
        {
          //Bit histogram (for looking for stuck-bits)
          sprintf(name, "bitHist%u",stripItr);
          sprintf(titleSeed, "Bit histogram for eta %d, sector %d, layer %d%c strip %d", 
              stationEta,(2*stationPhi+50 - stationName),wireLayer,orientation,stripNumber);
          title = m_titlePrefix + titleSeed + m_titlePostfix;
          hist = new TH1I(name, title.c_str(), m_numBits, 0, m_numBits); //12 bits
          hist->GetXaxis()->SetTitle("Bit");
          hist->GetYaxis()->SetTitle("Counts");
          m_bitHists->push_back(hist);
          if(m_doCorrelation) { 
            (*m_bitProds)[stripItr] = new TH2F(TString::Format("bitProds%d",stripItr),"Bit products", m_numBits,0,m_numBits, m_numBits, 0, m_numBits);

          }
        }
      }
      else
      {
        m_ampHists->push_back(NULL);
        if(m_bitHists) m_bitHists->push_back(NULL);
        if(m_doSampleHists) m_sampHists->push_back(NULL);
      }
    }//end strip loop

    //Setup result collections. These will be passed to storegate for any monitoring later
    m_peds = new CscCalibResultCollection("ped");
    m_noises = new CscCalibResultCollection("noise");
    m_rmses = new CscCalibResultCollection("rms");
    if(m_doF001){
      if(m_debug) mLog << MSG::DEBUG <<  "Doing f001" << endmsg;
      //For f001 values
      m_f001s = new CscCalibResultCollection("f001");

      //Initializing for comparing new values to an online database file
      if(m_doOnlineDbFile){

        //How many samples failed the online threshold test of f001 +2*RMS 
        //(f001 and RMS read from a file from online configuration db)
        m_onlineTHoldBreaches = new CscCalibResultCollection("OnlTHoldBreaches");



        //Vector we use to count the online thresholds
        m_onlineThresholdFailureCount.resize(m_maxStripHash+1,0);

        //Retrieve current online thresholds
        m_onlineThresholds.resize(m_maxStripHash+1);
        ifstream ifile; ifile.open(m_onlineDbFile.c_str());
        if(!ifile.is_open()){
          mLog << MSG::FATAL << "Failed to open online database file " << m_onlineDbFile << endmsg;
          return StatusCode::FAILURE;
        }
        std::string buf;
        ifile >> buf; // skip 32 at start of file
        unsigned int onlineId;
        unsigned int hashId;
        double rms;
        double f001;

        if(!ifile){
          mLog << MSG::FATAL << "Problem with file after one word read in." << endmsg;
          return StatusCode::FAILURE;
        }


        if(m_debug) mLog << MSG::DEBUG << "Reading in online thresholds from file " <<  m_onlineDbFile << endmsg;
        if(m_debug) mLog << MSG::DEBUG << "First (junk) word: " << buf << endmsg;
        int chanCnt = 0;
        while(ifile >> hex >> onlineId >> dec) { 
          chanCnt++;
          onlineToOfflineHashId(onlineId,hashId);

          ifile >> buf >> buf >> buf >> buf >>  rms >> buf >> f001;
          double thold = f001 + 2*rms;
          if(m_verbose) mLog << MSG::VERBOSE 
            << "onlid: " <<  hex << onlineId << dec << " hash: " << hashId << " rms: " << rms << " f001: " << f001 << " thold: " << thold 
              << endmsg;
          m_onlineThresholds.at(hashId) = thold;
          if(m_verbose){
            if(!ifile)
              mLog << MSG::VERBOSE << "input file is done, ready to close!"<< std::endl;
          }
          else 
            mLog << MSG::VERBOSE << "Input file still good!" <<std::endl;


        }
        if(chanCnt != 30720){
          mLog << MSG::FATAL << "Did not retrieve expected 30720 channels from online database! Retrieved: " << chanCnt << endmsg;
          mLog << MSG::FATAL << "Last onlineId read: " << hex << onlineId << dec << endmsg;
          return StatusCode::FAILURE;
        }

      }//if m_doOnlineDBFile
    }//db file

    mLog << MSG::INFO << "highest strip hash id is " << m_maxStripHash  << endmsg;

    //If we're doing correlation plots, set up the product histogram array
    if(m_doCorrelation){
      m_bitProds = new DataVector<TH2F>(SG::VIEW_ELEMENTS);
      m_bitProds->resize(m_maxStripHash+1);
    }





    cout << "m_prods value: " << m_bitProds <<  "\ndoCorrelation" << m_doCorrelation << endl;
    return StatusCode::SUCCESS;
  }

  //Execute loops through all strips and fills histograms
  StatusCode CscCalcPed::execute()
  {
    MsgStream mLog( msgSvc(), name() );
    if(m_debug) mLog << MSG::DEBUG << "Begin execute" << endmsg;	
    //collectEventInfo collects infomation about each event by filling ampHistCollection and peaktHist.
    StatusCode sc = collectEventInfo();

    if(!sc.isSuccess())
    {
      mLog << MSG::ERROR << "There was an error collecting information from the RDO this event." << endmsg;
      return StatusCode::RECOVERABLE;
    }
    mLog << MSG::INFO << "End execute" << endmsg;	
    return StatusCode::SUCCESS;
  } //end execute()

  StatusCode CscCalcPed::finalize() {

    MsgStream mLog( msgSvc(), name() );

    if(m_eventCnt ==0)
    {
      mLog <<MSG::FATAL << "No events processed!" << endmsg;
      return StatusCode::FAILURE;
    }
    else
      mLog << MSG::INFO << "In finalize() after analyzing " << m_eventCnt << " events " <<endmsg;

    StatusCode sc;

    mLog << MSG::INFO << "not dump all hists!" << endmsg;

    //calculateParameters() finds means and fits gain curves from the data in 
    //m_ampHistCollection and/or m_peaktHist 
    sc =calculateParameters();
    if(!sc.isSuccess())
    {
      mLog << MSG::FATAL << "Calculation of parameters failed!" << endmsg;
      return StatusCode::FAILURE;
    }
    if(m_debug) mLog << MSG::DEBUG << "Finished calculating parameters" << endmsg;


    //writeCalibrationFile() writes the calculated parameters into a calibration fie.
    sc = writeCalibrationFile();
    if(!sc.isSuccess())
    {
      mLog << MSG::FATAL << "Calculation of parameters failed!" << endmsg;
      return StatusCode::FAILURE;
    }

    //Record report (ampHists) and results to storegate for future algorithms
    //like COOL database storage or monitoring to collect
    sc =storeGateRecord(); 
    if (!sc.isSuccess())
    {
      mLog << MSG::FATAL << "Failed recording data in storegate" <<endmsg;
    }

    mLog << MSG::INFO << "Finished finalize" << endmsg;
    return StatusCode::SUCCESS;	
  }//end finalize()


  //Collect event info is run every event, gathering amplitutdes and peakting times.
  StatusCode CscCalcPed::collectEventInfo()
  {
    MsgStream mLog( msgSvc(), name() );


    //start the timer
    Chrono chrono(m_chronoSvc,"collectEventInfo");


    m_eventCnt++;
    if(m_debug) mLog << MSG::DEBUG <<"Collecting event info for event " << m_eventCnt << endmsg;
    //Below might need to be changed depending on how we get data
    const CscRawDataContainer* rawDataContainter;
    StatusCode sc_read = m_storeGate->retrieve(rawDataContainter, "CSCRDO"); 
    if (sc_read != StatusCode::SUCCESS)
    {
      mLog << MSG::FATAL << "Could not find event" << endmsg;
      return StatusCode::FAILURE;
    }

    if(m_verbose) mLog << MSG::VERBOSE <<"Retrieved RDO from storegate " <<  endmsg;

    if(rawDataContainter->size() == 0)
    {
      mLog << MSG::FATAL << "no rods in RDO!" << endmsg;
      return StatusCode::FAILURE;
    }

    if(m_verbose) mLog << MSG::VERBOSE <<"There are " << rawDataContainter->size() << " rods in the RDO" << endmsg;

    IdContext channelContext = m_cscId->channel_context();	

    //Loop over RODs (data from 2 chambers), each of which is in
    //a single CscRawaData collection
    CscRawDataContainer::const_iterator rodItr = rawDataContainter->begin();
    CscRawDataContainer::const_iterator rodEnd = rawDataContainter->end();
    if(m_verbose) mLog << MSG::VERBOSE <<"does rodItr == rodEnd? " << (rodItr == rodEnd) << endmsg;
    //	mLog << MSG::INFO<< " going to read all " << rawDataContainter->size() << " rod's" << endmsg;
    for(;rodItr != rodEnd; rodItr++)
    {
      Chrono chronoRod(m_chronoSvc,"RodItr");
      if(m_verbose) mLog << MSG::VERBOSE <<"Examining a ROD" << endmsg;
      const CscRawDataCollection * rod = (*rodItr); 	//Removing another "pointer layer" to make
      //                                                syntax simpler

      if(m_verbose) mLog << MSG::VERBOSE <<"There are " << rod->size() << " clusters in the ROD" << endmsg;
      if(rod->size() >0) 
      {
        //Loop over strips in rod
        CscRawDataCollection::const_iterator clusItr = rod->begin();
        CscRawDataCollection::const_iterator clusEnd = rod->end();
        for(; clusItr!=clusEnd ; clusItr++)
        {
          Chrono chronoClus(m_chronoSvc,"ClusterItr");
          const CscRawData * cluster = (*clusItr); //Note: For a ped run, the "cluster" 
          //                                               is the size of an entire layer. 
          int numStrips = cluster->width();
          int samplesPerStrip = (cluster->samples()).size()/numStrips;

          if(m_verbose) mLog << MSG::VERBOSE << "About to collect info from " << numStrips << " strips" << endl;
          for(int stripItr = 0; stripItr <numStrips; stripItr++)
          {

            Chrono chronoStrip(m_chronoSvc,"stripItr");
            // WP Added
            Identifier channelId =m_cscRdoDecoderTool->channelIdentifier(cluster, stripItr);
            IdentifierHash cscChannelHashId;
            m_cscId->get_channel_hash(channelId, cscChannelHashId);

            ///////////
            int stripHash = cscChannelHashId;

            Identifier stripId;
            m_cscId->get_id(stripHash, stripId, &channelContext);


            Chrono chronoAfterId(m_chronoSvc,"afterID1");

            if( m_cscId->chamberLayer(channelId) != m_expectedChamberLayer)
            {
              //              cout << "Wrong chamber layer" << endl;
              mLog << MSG::WARNING << "Wrong chamber layer  a hash ("
                << stripHash << ")  from the wrong multilayer has appeared in the data. Its string id is " << m_cscId->show_to_string(stripId)
                <<  "  " << m_cscId->show_to_string(channelId)
                << endmsg;

              mLog << MSG::INFO << "WP added (1) "
                << m_cscId->stationEta(stripId) << " " << m_cscId->measuresPhi(stripId) << " "
                << stripHash << " " << cscChannelHashId
                << endmsg;

              mLog << MSG::INFO << "WP added (2) "
                << m_cscId->stationEta(stripId) << " " << m_cscId->measuresPhi(stripId) << " "
                << stripId << " " << channelId
                << endmsg;



              //              if(m_cscId->measuresPhi(stripId))
              //                mLog << MSG::DEBUG <<" bad id Measures Phi" << endmsg;
              //              else
              //                mLog << MSG::DEBUG <<" bad id is eta" << endmsg;
              //continue; 
              stripId = m_cscId->channelID(
                  m_cscId->stationName(stripId),
                  m_cscId->stationEta(stripId),
                  m_cscId->stationPhi(stripId),
                  2,
                  m_cscId->wireLayer(stripId),
                  m_cscId->measuresPhi(stripId),
                  m_cscId->strip(stripId)
                  ); 
              IdentifierHash newHash;
              m_cscId->get_channel_hash(stripId, newHash );
              stripHash = newHash;
              if(m_debug) mLog << MSG::DEBUG << "New hash " << stripHash << endmsg;
            }
            else{
              if(m_cscId->measuresPhi(stripId))
                mLog << MSG::VERBOSE <<" good id Measures Phi" << endmsg;
              else
                mLog << MSG::VERBOSE <<" good id is eta" << endmsg;
            }

            Chrono chronoAfterId2(m_chronoSvc,"afterID2");

            //Get samples. Each shows amplitude of pulse at different
            //time slice.
            vector<uint16_t> samples;
            cluster->samples(stripItr,samplesPerStrip,samples);

            //Test for threshold breach... 

            //int minMax = GetMinMax(samples);
            //if(minMax < (m_cscId->measuresPhi(stripId) ?  50 :30)) {
            size_t sampCnt = 0;
            std::vector<uint16_t>::const_iterator sampItr = samples.begin();
            std::vector<uint16_t>::const_iterator sampEnd = samples.end();
            for(;sampItr != sampEnd; sampItr++)
            {
              //if(m_debug) mLog << MSG::DEBUG << "Filling amplitude histogram" << endmsg;
              (*m_ampHists)[stripHash]->Fill(*sampItr);
              //cerr << "Filling sample hist " << sampCnt << endl;
              if(m_doSampleHists)
                (*((*m_sampHists)[stripHash]))[sampCnt]->Fill(*sampItr);
              //cerr << "Filled" << sampCnt << endl;
              if(m_bitHists && sampCnt==1)
              {
                TH2F* prodHist = NULL;
                if(m_bitProds!=NULL)
                  prodHist = (*m_bitProds)[stripHash];
                if(!fillBitHist((*m_bitHists)[stripHash],*sampItr, prodHist).isSuccess())
                  mLog << MSG::WARNING << "Failed recording bits for strip " << stripHash << endmsg;
              }//end if(m_bitHists)

              if(m_doOnlineDbFile){//m_doF001){
                //test if any samples are obvoe the online threshold
                if (*sampItr > m_onlineThresholds[stripHash] ){
                  m_onlineThresholdFailureCount[stripHash]++;
                  if(m_verbose) mLog << MSG::VERBOSE << "StripHash: " << stripHash  << 
                    " has online threshold breach. Sample: " << *sampItr << " Thold: " 
                      << m_onlineThresholds[stripHash] << endmsg;
                }
              }
              sampCnt++;
              }//end Sample loop
              // }//end if GetMinMax
              // else{
              //   mLog << MSG::DEBUG << "Didn't pass GetMinMax() on hash " << stripHash << std::endl;
              // }
          }//end strip loop
          }//end cluster loop
        }
        else
          if(m_debug) mLog << MSG::DEBUG << "There is an empty rod (CscRawDataContainer)." <<endmsg;
      }//end rod loop
      if(m_debug) mLog << MSG::DEBUG << "end collectEventInfo()" << endmsg;
      return StatusCode::SUCCESS;
    }// end collectEventInfo()


    //Calculate parameters is called during finalize,
    //calculates the parameter values.
    StatusCode CscCalcPed::calculateParameters()
    {
      MsgStream mLog( msgSvc(), name() );
      Chrono chrono(m_chronoSvc,"calculateParameters");


      //**********Calculate all the parameters from data collected during execute*****************//

      for(unsigned int stripHash = 0 ;stripHash <= m_maxStripHash; stripHash++)
      {
        if(stripHash < 50 || stripHash%1000 == 0)
        {
          mLog << MSG::INFO << "Analyzing strip with hash " << stripHash << " out of " << m_maxStripHash << endmsg; 
          if(m_verbose) mLog <<MSG::VERBOSE << (float)clock()/((float)CLOCKS_PER_SEC) << " is the time" << endmsg;
        }

        TH1I * ampHist = (*m_ampHists)[stripHash];
        if(ampHist != NULL)
        {
          if(m_verbose) mLog << MSG::VERBOSE << "Have data for strip hash " << stripHash << endmsg;
          if(ampHist->GetEntries() >0) //If strip wasn't tested, it won't have entries
          {
            //Following Schernau's work
            float histMean = ampHist->GetMean();
            float histRMS = ampHist->GetRMS();
            float histRMSError = ampHist->GetRMSError();

            float lowbound = histMean - 3*histRMS;
            float highbound = histMean + 3*histRMS;
            if(m_verbose) mLog << MSG::VERBOSE << "About to fit..." << endmsg;

            int result = ampHist->Fit("gaus","QL","",lowbound,highbound);
            if(m_verbose) mLog << MSG::VERBOSE << "Result is " << result << endmsg;
            TF1 * fittedFunction = ampHist->GetFunction("gaus");
            //double mean = fittedFunction->GetParameter(1);
            double meanError = fittedFunction->GetParError(1);
            double sigma = fittedFunction->GetParameter(2);
            double sigmaError = fittedFunction->GetParError(2);
            double chi2 = fittedFunction->GetChisquare();
            int ndf = fittedFunction->GetNDF();

            m_peds->push_back(new CscCalibResult(stripHash,histMean,meanError,chi2,ndf));
            m_noises->push_back(new CscCalibResult(stripHash,sigma,sigmaError,chi2,ndf));
            m_rmses->push_back(new CscCalibResult(stripHash,histRMS,histRMSError,0,0));



            //Integrated threshold (f001) calculation
            if(m_doF001){
              int num = (int)ampHist->GetEntries();
              int thr = ampHist->GetNbinsX() + 1; // start at overflow bin
              double maxSum = 0.001*num;  // 99.90 of pedestals under thr
              //double maxSum = 0.0001*num; // 99.99 of pedestals under thr

              double sum = 0;
              do{
                sum += ampHist->GetBinContent(thr);
                thr--;
              } while ((thr>0)&&(sum<maxSum));

              //double threshold = ampHist->GetXaxis()->GetBinLowEdge(thr) +2; //For some reason +2 here matches Michael Schernau's +1
              double threshold = ampHist->GetXaxis()->GetBinLowEdge(thr) +1; //For some reason +2 here matches Michael Schernau's +1
              m_f001s->push_back(new CscCalibResult(stripHash,threshold,0,0,0));

              if(m_doOnlineDbFile){
                m_onlineTHoldBreaches->push_back(new CscCalibResult(stripHash,m_onlineThresholdFailureCount[stripHash],0));
              }
            }

            //Threshold calculation
            //double threshold = mean + m_thresholdMultiplier*sigma;
            //double thresholdError = sqrt((meanError*meanError +
            //      m_thresholdMultiplier*sigmaError*m_thresholdMultiplier*sigmaError)/2);

            //if(m_verbose) mLog << MSG::VERBOSE << "===> Ped: " << mean << "\tNoise: " << sigma 
            //  << "\tthreshold: " << threshold << endmsg; 

          }
        }
        else
          if(m_verbose) mLog << MSG::VERBOSE << "Don't have data for strip hash " << stripHash << endmsg;
      }//end loop over strips


      //don't need it anymore, clear ram taken by m_failure tests
      mLog << MSG::DEBUG << "Clearing m_onlineThresholdFailureCount" << endmsg;
      m_onlineThresholdFailureCount.resize(0);


      mLog << MSG::INFO << "Completed calculating parameters." << endmsg;

      return StatusCode::SUCCESS;
    }//End calculateParameters()

    //writeCalibrationFile() dumps the parameters to disk
    StatusCode CscCalcPed::writeCalibrationFile()
    {
      MsgStream mLog( msgSvc(), name() );
      Chrono chrono(m_chronoSvc,"writeFile");
      //***Take conditions data held in summary histograms and  print to the calibration file***//
      mLog << MSG::INFO << "Parameters calculated, preparing to output to file: " << m_outputFileName << " Types 1 and " << m_calOutputVersion << endmsg;

      calOutput1();

      if(m_calOutputVersion == "00-00"){
        return calOutput0();
      }
      else if(m_calOutputVersion == "03-00") {
        return calOutput3();
      }
      else{
        mLog << "Don't know how to write calibration file version " << m_calOutputVersion << endmsg;
        return StatusCode::RECOVERABLE;
      }
      // this part of the code cannot be reached since one of the if statements before already exits the code
      // return StatusCode::SUCCESS;
    }

    StatusCode CscCalcPed::calOutput0() {
      MsgStream mLog( msgSvc(), name() );

      ofstream out;
      out.open(m_outputFileName.c_str());
      if(!out.is_open())
      {
        mLog << MSG::ERROR << "Can't open file " << m_outputFileName.c_str() << endmsg;
        return StatusCode::RECOVERABLE;
      }

      out << "00-00 ";
      out << m_peds->size() << " ";
      out << "ped ";
      out << "noise ";
      cout << "rms ";
      //out << "thold ";
      out << "END_HEADER\n";			

      if(m_debug) mLog << MSG::DEBUG <<  "Begining loop over all " << m_peds->size()  
        << " channels data was collected for." << endmsg;

      //form is:hashID chamber LayerOrientationStrip  parametervalue parametervalue 
      CscCalibResultCollection::iterator pedItr = m_peds->begin();
      CscCalibResultCollection::iterator pedEnd = m_peds->end();
      CscCalibResultCollection::iterator noiseItr = m_noises->begin(); 
      CscCalibResultCollection::iterator rmsItr = m_rmses->begin(); 
      for(;pedItr!= pedEnd;pedItr++,noiseItr++, rmsItr++)//,tholdItr++)
      {
        int hashId = (*pedItr)->hashId();
        double ped = (*pedItr)->value();
        double noise = (*noiseItr)->value();
        double rms = (*rmsItr)->value();
        //double thold = (*tholdItr)->value();

        if(m_debug) mLog << MSG::DEBUG << "we're on hash " << hashId << " with pedestal " << ped 
          << "and noise " << noise << endmsg;//<< " and threshold " << thold << endmsg;
        Identifier id;
        IdContext channelContext = m_cscId->channel_context();	
        m_cscId->get_id(hashId,id, &channelContext);

        Identifier chamberId = m_cscId->elementID(id);
        if(!m_cscId->valid(chamberId))
        {
          mLog << MSG::WARNING << chamberId.getString() << " is not a valid id!" << endmsg;
          mLog << MSG::WARNING << "identifier is: " << m_cscId->show_to_string(chamberId) << endmsg;
          //id.show();
          //in.ignore(10000,'\n');
        }

        IdentifierHash chamberHash;
        m_cscId->get_module_hash(id,chamberHash);

        //print out values.
        out << hashId;
        out <<" " << chamberHash;
        out << " " << m_cscId->show_to_string(id) << " ";
        out << " " << ped;
        out << " " << noise;
        out << " " << rms;
        //out << " " << thold;
        out << "\n" ; 
      } //end loop over hash Ids

      out.close(); //done writing		
      mLog <<MSG::INFO <<"File written" << endmsg;
      return StatusCode::SUCCESS;	
    }//end calOutput0

    //calOutput1 prints out calibration output file in format Michael Schernau's 
    //online software likes to read
    StatusCode CscCalcPed::calOutput1() {
      MsgStream mLog( msgSvc(), name() );

      ofstream out;
      string onlineFileName = m_outputFileName + "_online";

      out.open(onlineFileName.c_str());
      if(!out.is_open())
      {
        mLog << MSG::ERROR << "Can't open online file " << m_outputFileName.c_str() << endmsg;
        return StatusCode::RECOVERABLE;
      }

      out << "32\n";

      if(m_debug) mLog << MSG::DEBUG <<  "Begining loop over all " << m_peds->size()  
        << " channels data was collected for." << endmsg;

      //form is:hashID chamber LayerOrientationStrip  parametervalue parametervalue 
      CscCalibResultCollection::iterator pedItr = m_peds->begin();
      CscCalibResultCollection::iterator pedEnd = m_peds->end();
      CscCalibResultCollection::iterator noiseItr = m_noises->begin(); 
      CscCalibResultCollection::iterator rmsItr = m_rmses->begin(); 
      CscCalibResultCollection::iterator f001Itr = m_f001s->begin();
      for(;pedItr!= pedEnd;pedItr++,noiseItr++, rmsItr++, f001Itr++)//,tholdItr++)
      {
        int hashId = (*pedItr)->hashId();
        double ped = (*pedItr)->value();
        double noise = (*noiseItr)->value();
        //double rms = (*rmsItr)->value();
        double f001 = (*f001Itr)->value();

        //double thold = (*tholdItr)->value();
        string onlineHexId;

        //Online ids are same as "string ids" used internally in COOL db.
        m_cscCoolStrSvc->indexToStringId(hashId, "CHANNEL", onlineHexId);

        if(m_debug) mLog << MSG::DEBUG << "we're on hash " << hashId << " with pedestal " << ped 
          << "and noise " << noise << endmsg;//<< " and threshold " << thold << endmsg;
        Identifier id;
        IdContext channelContext = m_cscId->channel_context();	
        m_cscId->get_id(hashId,id, &channelContext);

        Identifier chamberId = m_cscId->elementID(id);
        if(!m_cscId->valid(chamberId))
        {
          mLog << MSG::WARNING << chamberId.getString() << " is not a valid id!" << endmsg;
          mLog << MSG::WARNING << "identifier is: " << m_cscId->show_to_string(chamberId) << endmsg;
          //id.show();
          //in.ignore(10000,'\n');
        }

        char orientationChar = (m_cscId->measuresPhi(id) ? 'Y':'X');


        IdentifierHash chamberHash;
        m_cscId->get_module_hash(id,chamberHash);

        //print out values.
        out.setf(ios::right);//right aligned columns
        out << setfill('0') << setw(8) << onlineHexId;
        out <<" " 
          << setw(2) << chamberHash << orientationChar << (m_cscId->wireLayer(id)-1)
          <<" "
          << setw(3) << m_cscId->strip(id) -1 << " " ;
        out.setf(ios::fixed);


        out << " " << setprecision(3) << setw(8) << ped << " 0000.00";
        out << " " << setprecision(3) << setw(8) << noise << " 0000.000";
        out << " " << f001;
        out << "\n" ; 
      } //end loop over hash Ids

      out.close(); //done writing		
      mLog <<MSG::INFO <<"File written" << endmsg;
      return StatusCode::SUCCESS;	
    }//end calOutput1

    //calOutput3 outputs version 03-00 calibration files, which are what the most recent version
    //of CscCoolReadWrite takes for input
    StatusCode CscCalcPed::calOutput3() {
      MsgStream mLog( msgSvc(), name() );

      ofstream out;
      out.open(m_outputFileName.c_str());
      if(!out.is_open())
      {
        mLog << MSG::ERROR << "Can't open output 3 type file " << m_outputFileName.c_str() <<  " for writing " << endmsg;
        return StatusCode::RECOVERABLE;
      }
      out << "03-00 <END_HEADER>";

      outputParameter3(*m_peds, out);
      outputParameter3(*m_noises,out);
      outputParameter3(*m_rmses,out);
      if(m_doF001)
        outputParameter3(*m_f001s,out);
      out << "\n<END_FILE>";
      out.close();

      return StatusCode::SUCCESS;
    }


    //Outputs a single parameter in version 03-00
    void CscCalcPed::outputParameter3(const CscCalibResultCollection & results, ofstream & out){

      out << "\n";
      out << "<NEW_PAR> " << results.parName() << "\n";
      std::string idString;

      CscCalibResultCollection::const_iterator resItr = results.begin();
      CscCalibResultCollection::const_iterator resEnd = results.end();
      for(; resItr != resEnd; resItr++){
        unsigned int hashId = (*resItr)->hashId();
        double value = (*resItr)->value();
        std::string idString;

        m_cscCoolStrSvc->indexToStringId(hashId, "CHANNEL", idString);

        out << idString << " " << value << "\n";
      }

      out << "<END_PAR>" ;
    }



    //Record ampHist and calib results to storegate for monitoring and maybe other 
    //programs
    StatusCode CscCalcPed::storeGateRecord()
    {
      MsgStream mLog( msgSvc(), name() );

      StatusCode sc = StatusCode::SUCCESS;

      bool thereIsAnError = false;

      string histKey = "cscPedCalibReport";
      if(m_debug) mLog << MSG::DEBUG << "Recording pedestal amplitude histograms to TDS with key " 
        << histKey << endmsg;

      //CscCalibReport has extraraneous monitoring information
      CscCalibReportPed * report = new CscCalibReportPed("pedAmps");
      report->setPedAmpHists(m_ampHists); //report now has ownership of the DataVector
      report->setBitHists(m_bitHists); //report now has ownership of the DataVector
      if(m_doSampleHists){
        report->setSampHists(m_sampHists);
      }
      if(m_bitProds){
        report->setBitCorrelation( makeBitCorrelation());
      }

      CscCalibReportContainer * repCont = new CscCalibReportContainer(histKey);
      repCont->push_back(report);  

      sc = m_storeGate->record(repCont, histKey);
      if(!sc.isSuccess())
      {
        mLog << MSG::ERROR << "Failed to record CscCalibReportPed to storegate" << endmsg;
        thereIsAnError = true;
        delete repCont;
      }


      //CscCalibResult contains the actual parameters that we recorded, mostly things that should be entered
      //into cool
      string key = "CscCalibResultPed";
      if(m_debug) mLog << MSG::DEBUG << "Recording calibration results to TDS with key " << key << endmsg;

      CscCalibResultContainer * calibResults 
        = new CscCalibResultContainer("CscCalibResultPed");
      calibResults->push_back(m_peds);
      calibResults->push_back(m_noises);
      calibResults->push_back(m_rmses);
      calibResults->push_back(m_f001s);
      if(m_doOnlineDbFile)
        calibResults->push_back(m_onlineTHoldBreaches);

      sc = m_storeGate->record(calibResults,key);
      if(!sc.isSuccess())
      {
        mLog << MSG::ERROR << "Failed to record data to storegate" << endmsg;
        thereIsAnError = true;
        delete calibResults;
      }

      if(thereIsAnError)
        return StatusCode::RECOVERABLE;

      return StatusCode::SUCCESS;
    }

    //Does bit correlation study. Likely will be dropped
    DataVector<TH2F> * CscCalcPed::makeBitCorrelation() {

      if(!m_bitProds || !m_bitHists)
        return NULL;

      DataVector<TH2F> * correlations = new DataVector<TH2F>(SG::VIEW_ELEMENTS);
      correlations->resize(m_maxStripHash +1);

      for(unsigned int hashItr =0; hashItr <= m_maxStripHash; hashItr++) {
        IdentifierHash stripHash =hashItr;
        Identifier stripId;
        IdContext channelContext = m_cscId->channel_context();	
        m_cscId->get_id(stripHash, stripId, &channelContext);

        int chamLayer = m_cscId->chamberLayer(stripId);
        if(chamLayer == m_expectedChamberLayer) //Only second chamber layer exists
        {
          int stationName = m_cscId->stationName(stripId);
          //int stationEta = m_cscId->stationEta(stripId);
          int stationPhi = m_cscId->stationPhi(stripId);
          int stripNumber = m_cscId->strip(stripId);
          int wireLayer = m_cscId->wireLayer(stripId);
          char orientation = m_cscId->measuresPhi(stripId) ? 'Y':'X';

          int sector = 2*stationPhi + 50 - stationName;

          stringstream name; 
          name << "h_bitCorr_sector_" << setfill('0') << setw(2) << sector << 
            "_lay_" << wireLayer  << orientation << "_strip_" << setw(3) << stripNumber; 
          stringstream title; 
          title << "h_bitCorr_sector_" << setfill('0') << setw(2) << sector << 
            "_lay_" << wireLayer  << orientation << "_strip_" << setw(3) << stripNumber; 

          TH2F* correlationHist = new TH2F(
              name.str().c_str(), 
              title.str().c_str(),
              m_numBits, 0, m_numBits,
              m_numBits, 0, m_numBits
              );
          (*correlations)[hashItr] = correlationHist;

          //each amphis is filled exaclty the number of times the bits are sampled,
          //so its a good place to get n
          double n = (*m_ampHists)[hashItr]->GetEntries();
          TH1I * bitHist = (*m_bitHists)[hashItr];
          TH2F * bitProds = (*m_bitProds)[hashItr];
          for(unsigned int bit1 = 1; bit1 <=m_numBits; bit1++){
            for(unsigned int bit2 = 1; bit2 <=bit1; bit2++){

              /*
              //Prolay's method

              float xyn = bitProds->GetBinContent(bit1,bit2)/n; //Avg product
              float x = bitHist->GetBinContent(bit1);
              float xn = x/n;                                   //avg bit1
              float y = bitHist->GetBinContent(bit2);
              float yn = y/n;                                   //avg bit2
              float sigx = sqrt(x*(1-xn));                      //sigma 1
              float sigy = sqrt(y*(1-yn));                      //sigma 2

              float pxy = (xyn - xn*yn)/sigx/sigy;

              correlationHist->SetBinContent(bit1,bit2,pxy);
              if(bit1!=bit2)
              correlationHist->SetBinContent(bit2,bit1,pxy);
               */
              float xy = bitProds->GetBinContent(bit1,bit2);
              float x = bitHist->GetBinContent(bit1);
              float y = bitHist->GetBinContent(bit2);

              float r;
              float denom = (n*x-x*x)*(n*y-y*y);
              if(denom <= 0 )
                r= 0;
              else 
                r = (n*xy - x*y)/sqrt(denom);

              //Pearson r
              correlationHist->SetBinContent(bit1,bit2,r);
              if(bit1!=bit2)
                correlationHist->SetBinContent(bit2,bit1,r);


            }
          }
        }
      }
      return correlations;
    }//end makeBitCorrelations


    StatusCode CscCalcPed::onlineToOfflineHashId(const unsigned int & onlineId, unsigned int &hashId) const
    {
      int stationName =       ((onlineId >> 16)&0x1) + 50;
      int phi =               ((onlineId >> 13)&0x7)+1;
      int eta =               ((((onlineId >> 12)&0x1) == 1) ? 1:-1);
      int chamLay =           ((onlineId>>11)&0x1) +1;
      int wireLay =           ((onlineId>>9)&0x3) +1;
      int measuresPhi =       ((onlineId >> 8)&0x1);
      int strip;

      // Online and offline phi ids are flipped on A wheel
      if( measuresPhi && eta == 1){
        strip = 48 - ((onlineId)&0xff) ; //equivalent: 49 -( onlineId&0xff +1)
      }
      else {
        strip = ((onlineId)&0xff) +1;
      }

      Identifier chanId = m_cscId->channelID(stationName,eta,phi,chamLay,wireLay,measuresPhi,strip);

      IdentifierHash chanHash;
      m_cscId->get_channel_hash(chanId, chanHash);

      hashId = (unsigned int)chanHash;

      return StatusCode::SUCCESS;
    }

  }//end namespace MuonCalib
