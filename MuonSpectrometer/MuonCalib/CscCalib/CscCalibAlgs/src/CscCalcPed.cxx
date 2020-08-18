/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CscCalcPed.h"

#include "GaudiKernel/Chrono.h"
#include "MuonRDO/CscRawData.h"
#include "MuonRDO/CscRawDataCollection.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "CscCalibData/CscCalibResultContainer.h"
#include "CscCalibData/CscCalibReportContainer.h"
#include "CscCalibData/CscCalibReportPed.h"

#include "TF1.h"

#include <iostream>
#include <fstream>
#include <bitset>
#include <inttypes.h>

namespace MuonCalib {

  CscCalcPed::CscCalcPed(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name,pSvcLocator),
    m_chronoSvc(0),
    m_maxStripHash(0),
    m_numBits(12),
    m_ampHists(nullptr),
    m_sampHists(nullptr),
    m_bitHists(nullptr),
    m_bitProds(nullptr),
    m_bitCorrelation(nullptr),
    m_peds(nullptr),
    m_noises(nullptr),
    m_rmses(nullptr),
    m_f001s(nullptr),
    m_onlineTHoldBreaches(nullptr),
    m_eventCnt(0),
    m_crossTalkFix(nullptr)
  {
    declareProperty("OutputFile", m_outputFileName = "output.cal");
    declareProperty("TitlePrefix",m_titlePrefix = "");	//Prefix appended to title of histograms and graphs
    declareProperty("TitlePostfix",m_titlePostfix = "");	//Prefix appended to title of histograms and graphs
    declareProperty("PedAmpHighBound", m_ampHistHighBound = 2600);
    declareProperty("PedAmpLowBound", m_ampHistLowBound = 1800); 
    declareProperty("PedAmpNumBins", m_ampHistNumBins = 800);
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
    }
  }

  StatusCode CscCalcPed::initialize()
  {
    ATH_MSG_INFO("CscCalcPed::initialize() called");

    if(m_doOnlineDbFile && m_onlineDbFile ==""){
      ATH_MSG_FATAL("Either specify an OnlineCalibFile or set CompareOnlineCalibFile to false");
      return StatusCode::FAILURE;
    }

    //*******Register services and tools *********/ 	
    /// Store Gate active store

    ATH_CHECK(m_idHelperSvc.retrieve());

    ATH_CHECK(m_readKey.initialize());

    ATH_CHECK(service("ChronoStatSvc",m_chronoSvc));    

    ATH_CHECK(m_cscRdoDecoderTool.retrieve());

    IToolSvc* toolSvc=nullptr;
    ATH_CHECK(service("ToolSvc",toolSvc));

    //Set to SG::VIEW_ELEMENTS, since we want root to do its own memory
    //management.
    m_ampHists = new DataVector<TH1I>(SG::VIEW_ELEMENTS);
    if(m_doSampleHists) m_sampHists = new DataVector< DataVector<TH1I> >;
    if(m_doBitHists) m_bitHists = new DataVector<TH1I>(SG::VIEW_ELEMENTS);

    //Loop through ids to find out what hash range we're working on (in case we're using some 
    //unusual geometry)
    std::vector<Identifier> ids = m_idHelperSvc->cscIdHelper().idVector();
    std::vector<Identifier>::const_iterator chamItr = ids.begin();
    std::vector<Identifier>::const_iterator chamEnd = ids.end();
    m_maxStripHash = 0;

    for(; chamItr != chamEnd; chamItr++)
    {
      std::vector<Identifier> stripVect;
      m_idHelperSvc->cscIdHelper().idChannels(*chamItr,stripVect);

      std::vector<Identifier>::const_iterator stripItr = stripVect.begin();
      std::vector<Identifier>::const_iterator stripEnd = stripVect.end();
      for(;stripItr != stripEnd; stripItr++)
      {
        IdentifierHash stripHash;
        m_idHelperSvc->cscIdHelper().get_channel_hash(*stripItr,stripHash);
        if((unsigned int)stripHash > m_maxStripHash)
          m_maxStripHash = (int)stripHash;
      }//End strip loop
    }//end chamber loop		


    //Now creating ampHists. It wasn't done in last loop since there 
    //is no gaurantee that it will come out in strip order, and we assume
    //later that m_ampHists's index = stripHash
    ATH_MSG_DEBUG("Preparing ampHists. Only allowing those for " << " chamberLayer " << m_expectedChamberLayer);
    for(unsigned int stripItr = 0 ; stripItr <=m_maxStripHash; stripItr++)
    {        

      IdentifierHash stripHash =stripItr;
      Identifier stripId;
      IdContext channelContext = m_idHelperSvc->cscIdHelper().channel_context();	
      m_idHelperSvc->cscIdHelper().get_id(stripHash, stripId, &channelContext);

      int chamLayer = m_idHelperSvc->cscIdHelper().chamberLayer(stripId);
      if(chamLayer == m_expectedChamberLayer) //Only second chamber layer exists
      {
        int stationEta = m_idHelperSvc->cscIdHelper().stationEta(stripId);
        int stationPhi = m_idHelperSvc->cscIdHelper().stationPhi(stripId);
        int stripNumber = m_idHelperSvc->cscIdHelper().strip(stripId);
        int wireLayer = m_idHelperSvc->cscIdHelper().wireLayer(stripId);
        char orientation = m_idHelperSvc->cscIdHelper().measuresPhi(stripId) ? 'Y':'X';


        char name[30],titleSeed[600];
        TH1I* hist = nullptr;

        int stationName = m_idHelperSvc->cscIdHelper().stationName(stripId);		
        //Amplitude histogram
        sprintf(name, "ampHist%u",stripItr);
        sprintf(titleSeed, "Amplitude Histogram for eta %d, sector %d, layer %d%c, strip %d",
            stationEta,(2*stationPhi+50 - stationName),wireLayer,orientation,stripNumber);
        std::string title = m_titlePrefix + titleSeed + m_titlePostfix;

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
        m_ampHists->push_back(nullptr);
        if(m_bitHists) m_bitHists->push_back(nullptr);
        if(m_doSampleHists) m_sampHists->push_back(nullptr);
      }
    }//end strip loop

    //Setup result collections. These will be passed to storegate for any monitoring later
    m_peds = new CscCalibResultCollection("ped");
    m_noises = new CscCalibResultCollection("noise");
    m_rmses = new CscCalibResultCollection("rms");
    if(m_doF001){
      ATH_MSG_DEBUG("Doing f001");
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
        std::ifstream ifile; ifile.open(m_onlineDbFile.c_str());
        if(!ifile.is_open()){
          ATH_MSG_FATAL("Failed to open online database file " << m_onlineDbFile);
          return StatusCode::FAILURE;
        }
        std::string buf;
        ifile >> buf; // skip 32 at start of file
        unsigned int onlineId;
        unsigned int hashId;
        double rms;
        double f001;

        if(!ifile){
          ATH_MSG_FATAL("Problem with file after one word read in.");
          return StatusCode::FAILURE;
        }


        ATH_MSG_DEBUG("Reading in online thresholds from file " <<  m_onlineDbFile);
        ATH_MSG_DEBUG("First (junk) word: " << buf);
        int chanCnt = 0;
        while(ifile >> std::hex >> onlineId >> std::dec) { 
          chanCnt++;
          onlineToOfflineHashId(onlineId,hashId);

          ifile >> buf >> buf >> buf >> buf >>  rms >> buf >> f001;
          double thold = f001 + 2*rms;
          ATH_MSG_VERBOSE("onlid: " <<  std::hex << onlineId << std::dec << " hash: " << hashId << " rms: " << rms << " f001: " << f001 << " thold: " << thold);
          m_onlineThresholds.at(hashId) = thold;
            if(!ifile)
              ATH_MSG_VERBOSE("input file is done, ready to close!");
          else 
            ATH_MSG_VERBOSE("Input file still good!");


        }
        if(chanCnt != 30720){
          ATH_MSG_FATAL("Did not retrieve expected 30720 channels from online database! Retrieved: " << chanCnt);
          ATH_MSG_FATAL("Last onlineId read: " << std::hex << onlineId << std::dec);
          return StatusCode::FAILURE;
        }

      }//if m_doOnlineDBFile
    }//db file

    ATH_MSG_INFO("highest strip hash id is " << m_maxStripHash);

    //If we're doing correlation plots, set up the product histogram array
    if(m_doCorrelation){
      m_bitProds = new DataVector<TH2F>(SG::VIEW_ELEMENTS);
      m_bitProds->resize(m_maxStripHash+1);
    }





    ATH_MSG_INFO("m_prods value: " << m_bitProds <<  "\ndoCorrelation" << m_doCorrelation);
    return StatusCode::SUCCESS;
  }

  //Execute loops through all strips and fills histograms
  StatusCode CscCalcPed::execute()
  {
    ATH_MSG_DEBUG("Begin execute");	
    //collectEventInfo collects infomation about each event by filling ampHistCollection and peaktHist.
    StatusCode sc = collectEventInfo();

    if(!sc.isSuccess())
    {
      ATH_MSG_ERROR("There was an error collecting information from the RDO this event.");
      return StatusCode::RECOVERABLE;
    }
    ATH_MSG_DEBUG("End execute");	
    return StatusCode::SUCCESS;
  } //end execute()

  StatusCode CscCalcPed::finalize() {
    if(m_eventCnt ==0)
    {
      ATH_MSG_FATAL("No events processed!");
      return StatusCode::FAILURE;
    }
    else
      ATH_MSG_INFO("In finalize() after analyzing " << m_eventCnt << " events ");

    StatusCode sc;

    ATH_MSG_INFO("not dump all hists!");

    //calculateParameters() finds means and fits gain curves from the data in 
    //m_ampHistCollection and/or m_peaktHist 
    sc =calculateParameters();
    if(!sc.isSuccess())
    {
      ATH_MSG_FATAL("Calculation of parameters failed!");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Finished calculating parameters");


    //writeCalibrationFile() writes the calculated parameters into a calibration fie.
    sc = writeCalibrationFile();
    if(!sc.isSuccess())
    {
      ATH_MSG_FATAL("Calculation of parameters failed!");
      return StatusCode::FAILURE;
    }

    //Record report (ampHists) and results to storegate for future algorithms
    //like COOL database storage or monitoring to collect
    sc =storeGateRecord(); 
    if (!sc.isSuccess())
    {
      ATH_MSG_FATAL("Failed recording data in storegate");
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("Finished finalize");
    return StatusCode::SUCCESS;	
  }//end finalize()


  //Collect event info is run every event, gathering amplitutdes and peakting times.
  StatusCode CscCalcPed::collectEventInfo()
  {
    //start the timer
    Chrono chrono(m_chronoSvc,"collectEventInfo");


    m_eventCnt++;
    ATH_MSG_DEBUG("Collecting event info for event " << m_eventCnt);
    //Below might need to be changed depending on how we get data
    const CscRawDataContainer* rawDataContainter;
    StatusCode sc_read = evtStore()->retrieve(rawDataContainter, "CSCRDO"); 
    if (sc_read != StatusCode::SUCCESS)
    {
      ATH_MSG_FATAL("Could not find event");
      return StatusCode::FAILURE;
    }

    ATH_MSG_VERBOSE("Retrieved RDO from storegate ");

    if(rawDataContainter->size() == 0)
    {
      ATH_MSG_FATAL("no rods in RDO!");
      return StatusCode::FAILURE;
    }

    ATH_MSG_VERBOSE("There are " << rawDataContainter->size() << " rods in the RDO");

    IdContext channelContext = m_idHelperSvc->cscIdHelper().channel_context();	

    //Loop over RODs (data from 2 chambers), each of which is in
    //a single CscRawaData collection
    CscRawDataContainer::const_iterator rodItr = rawDataContainter->begin();
    CscRawDataContainer::const_iterator rodEnd = rawDataContainter->end();
    ATH_MSG_VERBOSE("does rodItr == rodEnd? " << (rodItr == rodEnd));
    for(;rodItr != rodEnd; rodItr++)
    {
      Chrono chronoRod(m_chronoSvc,"RodItr");
      ATH_MSG_VERBOSE("Examining a ROD");
      const CscRawDataCollection * rod = (*rodItr); 	//Removing another "pointer layer" to make
      //                                                syntax simpler

      ATH_MSG_VERBOSE("There are " << rod->size() << " clusters in the ROD");
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

          ATH_MSG_VERBOSE("About to collect info from " << numStrips << " strips");
          for(int stripItr = 0; stripItr <numStrips; stripItr++)
          {

            Chrono chronoStrip(m_chronoSvc,"stripItr");
            // WP Added
            Identifier channelId =m_cscRdoDecoderTool->channelIdentifier(cluster, stripItr);
            IdentifierHash cscChannelHashId;
            m_idHelperSvc->cscIdHelper().get_channel_hash(channelId, cscChannelHashId);

            ///////////
            int stripHash = cscChannelHashId;

            Identifier stripId;
            m_idHelperSvc->cscIdHelper().get_id(stripHash, stripId, &channelContext);


            Chrono chronoAfterId(m_chronoSvc,"afterID1");

            if( m_idHelperSvc->cscIdHelper().chamberLayer(channelId) != m_expectedChamberLayer)
            {
              ATH_MSG_WARNING("Wrong chamber layer  a hash ("
                << stripHash << ")  from the wrong multilayer has appeared in the data. Its string id is " << m_idHelperSvc->cscIdHelper().show_to_string(stripId)
                <<  "  " << m_idHelperSvc->cscIdHelper().show_to_string(channelId));

              ATH_MSG_INFO("WP added (1) "
                << m_idHelperSvc->cscIdHelper().stationEta(stripId) << " " << m_idHelperSvc->cscIdHelper().measuresPhi(stripId) << " "
                << stripHash << " " << cscChannelHashId);

              ATH_MSG_INFO("WP added (2) "
                << m_idHelperSvc->cscIdHelper().stationEta(stripId) << " " << m_idHelperSvc->cscIdHelper().measuresPhi(stripId) << " "
                << stripId << " " << channelId);

              stripId = m_idHelperSvc->cscIdHelper().channelID(
                  m_idHelperSvc->cscIdHelper().stationName(stripId),
                  m_idHelperSvc->cscIdHelper().stationEta(stripId),
                  m_idHelperSvc->cscIdHelper().stationPhi(stripId),
                  2,
                  m_idHelperSvc->cscIdHelper().wireLayer(stripId),
                  m_idHelperSvc->cscIdHelper().measuresPhi(stripId),
                  m_idHelperSvc->cscIdHelper().strip(stripId)
                  ); 
              IdentifierHash newHash;
              m_idHelperSvc->cscIdHelper().get_channel_hash(stripId, newHash );
              stripHash = newHash;
              ATH_MSG_DEBUG("New hash " << stripHash);
            }
            else{
              if(m_idHelperSvc->cscIdHelper().measuresPhi(stripId))
                ATH_MSG_VERBOSE(" good id Measures Phi");
              else
                ATH_MSG_VERBOSE(" good id is eta");
            }

            Chrono chronoAfterId2(m_chronoSvc,"afterID2");

            //Get samples. Each shows amplitude of pulse at different
            //time slice.
            std::vector<uint16_t> samples;
            cluster->samples(stripItr,samplesPerStrip,samples);

            //Test for threshold breach... 
            size_t sampCnt = 0;
            std::vector<uint16_t>::const_iterator sampItr = samples.begin();
            std::vector<uint16_t>::const_iterator sampEnd = samples.end();
            for(;sampItr != sampEnd; sampItr++)
            {
              (*m_ampHists)[stripHash]->Fill(*sampItr);
              if(m_doSampleHists)
                (*((*m_sampHists)[stripHash]))[sampCnt]->Fill(*sampItr);
              if(m_bitHists && sampCnt==1)
              {
                TH2F* prodHist = nullptr;
                if(m_bitProds)
                  prodHist = (*m_bitProds)[stripHash];
                if(!fillBitHist((*m_bitHists)[stripHash],*sampItr, prodHist).isSuccess())
                  ATH_MSG_WARNING("Failed recording bits for strip " << stripHash);
              }//end if(m_bitHists)

              if(m_doOnlineDbFile){//m_doF001){
                //test if any samples are obvoe the online threshold
                if (*sampItr > m_onlineThresholds[stripHash] ){
                  m_onlineThresholdFailureCount[stripHash]++;
                  ATH_MSG_VERBOSE("StripHash: " << stripHash  << 
                    " has online threshold breach. Sample: " << *sampItr << " Thold: " 
                      << m_onlineThresholds[stripHash]);
                }
              }
              sampCnt++;
              }//end Sample loop
          }//end strip loop
          }//end cluster loop
        }
        else
          ATH_MSG_DEBUG("There is an empty rod (CscRawDataContainer).");
      }//end rod loop
      ATH_MSG_DEBUG("end collectEventInfo()");
      return StatusCode::SUCCESS;
    }// end collectEventInfo()


    //Calculate parameters is called during finalize,
    //calculates the parameter values.
    StatusCode CscCalcPed::calculateParameters()
    {
      Chrono chrono(m_chronoSvc,"calculateParameters");


      //**********Calculate all the parameters from data collected during execute*****************//

      for(unsigned int stripHash = 0 ;stripHash <= m_maxStripHash; stripHash++)
      {
        if(stripHash < 50 || stripHash%1000 == 0)
        {
          ATH_MSG_INFO("Analyzing strip with hash " << stripHash << " out of " << m_maxStripHash); 
          ATH_MSG_VERBOSE((float)clock()/((float)CLOCKS_PER_SEC) << " is the time");
        }

        TH1I * ampHist = (*m_ampHists)[stripHash];
        if(ampHist)
        {
          ATH_MSG_VERBOSE("Have data for strip hash " << stripHash);
          if(ampHist->GetEntries() >0) //If strip wasn't tested, it won't have entries
          {
            //Following Schernau's work
            float histMean = ampHist->GetMean();
            float histRMS = ampHist->GetRMS();
            float histRMSError = ampHist->GetRMSError();

            float lowbound = histMean - 3*histRMS;
            float highbound = histMean + 3*histRMS;
            ATH_MSG_VERBOSE("About to fit...");

            int result = ampHist->Fit("gaus","QL","",lowbound,highbound);
            ATH_MSG_VERBOSE("Result is " << result);
            TF1 * fittedFunction = ampHist->GetFunction("gaus");
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
          }
        }
        else
          ATH_MSG_VERBOSE("Don't have data for strip hash " << stripHash);
      }//end loop over strips


      //don't need it anymore, clear ram taken by m_failure tests
      ATH_MSG_DEBUG("Clearing m_onlineThresholdFailureCount");
      m_onlineThresholdFailureCount.resize(0);


      ATH_MSG_INFO("Completed calculating parameters.");

      return StatusCode::SUCCESS;
    }//End calculateParameters()

    //writeCalibrationFile() dumps the parameters to disk
    StatusCode CscCalcPed::writeCalibrationFile()
    {
      Chrono chrono(m_chronoSvc,"writeFile");
      //***Take conditions data held in summary histograms and  print to the calibration file***//
      ATH_MSG_INFO("Parameters calculated, preparing to output to file: " << m_outputFileName << " Types 1 and " << m_calOutputVersion);

      ATH_CHECK( calOutput1() );

      if(m_calOutputVersion == "00-00"){
        return calOutput0();
      }
      else if(m_calOutputVersion == "03-00") {
        return calOutput3();
      }
      else{
        ATH_MSG_WARNING("Don't know how to write calibration file version " << m_calOutputVersion);
        return StatusCode::RECOVERABLE;
      }
      // this part of the code cannot be reached since one of the if statements before already exits the code
      // return StatusCode::SUCCESS;
    }

    StatusCode CscCalcPed::calOutput0() {

      std::ofstream out;
      out.open(m_outputFileName.c_str());
      if(!out.is_open())
      {
        ATH_MSG_ERROR("Can't open file " << m_outputFileName.c_str());
        return StatusCode::RECOVERABLE;
      }

      out << "00-00 ";
      out << m_peds->size() << " ";
      out << "ped ";
      out << "noise ";
      out << "rms ";
      out << "END_HEADER\n";			

      ATH_MSG_DEBUG("Begining loop over all " << m_peds->size() << " channels data was collected for.");

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

        ATH_MSG_DEBUG("we're on hash " << hashId << " with pedestal " << ped << "and noise " << noise);
        Identifier id;
        IdContext channelContext = m_idHelperSvc->cscIdHelper().channel_context();	
        m_idHelperSvc->cscIdHelper().get_id(hashId,id, &channelContext);

        Identifier chamberId = m_idHelperSvc->cscIdHelper().elementID(id);
        if(!m_idHelperSvc->cscIdHelper().valid(chamberId))
        {
          ATH_MSG_WARNING(chamberId.getString() << " is not a valid id!");
          ATH_MSG_WARNING("identifier is: " << m_idHelperSvc->cscIdHelper().show_to_string(chamberId));
        }

        IdentifierHash chamberHash;
        m_idHelperSvc->cscIdHelper().get_module_hash(id,chamberHash);

        //print out values.
        out << hashId;
        out <<" " << chamberHash;
        out << " " << m_idHelperSvc->cscIdHelper().show_to_string(id) << " ";
        out << " " << ped;
        out << " " << noise;
        out << " " << rms;
        out << "\n" ; 
      } //end loop over hash Ids

      out.close(); //done writing		
      ATH_MSG_INFO("File written");
      return StatusCode::SUCCESS;	
    }//end calOutput0

    //calOutput1 prints out calibration output file in format Michael Schernau's 
    //online software likes to read
    StatusCode CscCalcPed::calOutput1() {

      std::ofstream out;
      std::string onlineFileName = m_outputFileName + "_online";

      out.open(onlineFileName.c_str());
      if(!out.is_open())
      {
        ATH_MSG_ERROR("Can't open online file " << m_outputFileName.c_str());
        return StatusCode::RECOVERABLE;
      }

      out << "32\n";

      ATH_MSG_DEBUG("Begining loop over all " << m_peds->size() << " channels data was collected for.");

      SG::ReadCondHandle<CscCondDbData> readHandle{m_readKey}; 
      const CscCondDbData* readCdo{*readHandle};

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
        std::string onlineHexId;

        //Online ids are same as "string ids" used internally in COOL db.
        readCdo->indexToStringId(&m_idHelperSvc->cscIdHelper(), hashId, "CHANNEL", onlineHexId).ignore();

        ATH_MSG_DEBUG("we're on hash " << hashId << " with pedestal " << ped << "and noise " << noise);
        Identifier id;
        IdContext channelContext = m_idHelperSvc->cscIdHelper().channel_context();	
        m_idHelperSvc->cscIdHelper().get_id(hashId,id, &channelContext);

        Identifier chamberId = m_idHelperSvc->cscIdHelper().elementID(id);
        if(!m_idHelperSvc->cscIdHelper().valid(chamberId))
        {
          ATH_MSG_WARNING(chamberId.getString() << " is not a valid id!");
          ATH_MSG_WARNING("identifier is: " << m_idHelperSvc->cscIdHelper().show_to_string(chamberId));
        }

        char orientationChar = (m_idHelperSvc->cscIdHelper().measuresPhi(id) ? 'Y':'X');


        IdentifierHash chamberHash;
        m_idHelperSvc->cscIdHelper().get_module_hash(id,chamberHash);

        //print out values.
        out.setf(std::ios::right);//right aligned columns
        out << std::setfill('0') << std::setw(8) << onlineHexId;
        out <<" " 
          << std::setw(2) << chamberHash << orientationChar << (m_idHelperSvc->cscIdHelper().wireLayer(id)-1)
          <<" "
          << std::setw(3) << m_idHelperSvc->cscIdHelper().strip(id) -1 << " " ;
        out.setf(std::ios::fixed);


        out << " " << std::setprecision(3) << std::setw(8) << ped << " 0000.00";
        out << " " << std::setprecision(3) << std::setw(8) << noise << " 0000.000";
        out << " " << f001;
        out << "\n" ; 
      } //end loop over hash Ids

      out.close(); //done writing		
      ATH_MSG_INFO("File written");
      return StatusCode::SUCCESS;	
    }//end calOutput1

    //calOutput3 outputs version 03-00 calibration files, which are what the most recent version
    //of CscCoolReadWrite takes for input
    StatusCode CscCalcPed::calOutput3() {
      std::ofstream out;
      out.open(m_outputFileName.c_str());
      if(!out.is_open())
      {
        ATH_MSG_ERROR("Can't open output 3 type file " << m_outputFileName.c_str() <<  " for writing ");
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
    void CscCalcPed::outputParameter3(const CscCalibResultCollection & results, std::ofstream & out){

      SG::ReadCondHandle<CscCondDbData> readHandle{m_readKey}; 
      const CscCondDbData* readCdo{*readHandle};

      out << "\n";
      out << "<NEW_PAR> " << results.parName() << "\n";
      std::string idString;

      CscCalibResultCollection::const_iterator resItr = results.begin();
      CscCalibResultCollection::const_iterator resEnd = results.end();
      for(; resItr != resEnd; resItr++){
        unsigned int hashId = (*resItr)->hashId();
        double value = (*resItr)->value();
        std::string idString;

        readCdo->indexToStringId(&m_idHelperSvc->cscIdHelper(), hashId, "CHANNEL", idString).ignore();

        out << idString << " " << value << "\n";
      }

      out << "<END_PAR>" ;
    }



    //Record ampHist and calib results to storegate for monitoring and maybe other 
    //programs
    StatusCode CscCalcPed::storeGateRecord()
    {
      StatusCode sc = StatusCode::SUCCESS;

      bool thereIsAnError = false;

      std::string histKey = "cscPedCalibReport";
      ATH_MSG_DEBUG("Recording pedestal amplitude histograms to TDS with key " << histKey);

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

      sc = evtStore()->record(repCont, histKey);
      if(!sc.isSuccess())
      {
        ATH_MSG_ERROR("Failed to record CscCalibReportPed to storegate");
        thereIsAnError = true;
        delete repCont;
      }


      //CscCalibResult contains the actual parameters that we recorded, mostly things that should be entered
      //into cool
      std::string key = "CscCalibResultPed";
      ATH_MSG_DEBUG("Recording calibration results to TDS with key " << key);

      CscCalibResultContainer * calibResults 
        = new CscCalibResultContainer("CscCalibResultPed");
      calibResults->push_back(m_peds);
      calibResults->push_back(m_noises);
      calibResults->push_back(m_rmses);
      calibResults->push_back(m_f001s);
      if(m_doOnlineDbFile)
        calibResults->push_back(m_onlineTHoldBreaches);

      sc = evtStore()->record(calibResults,key);
      if(!sc.isSuccess())
      {
        ATH_MSG_ERROR("Failed to record data to storegate");
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
        return nullptr;

      DataVector<TH2F> * correlations = new DataVector<TH2F>(SG::VIEW_ELEMENTS);
      correlations->resize(m_maxStripHash +1);

      for(unsigned int hashItr =0; hashItr <= m_maxStripHash; hashItr++) {
        IdentifierHash stripHash =hashItr;
        Identifier stripId;
        IdContext channelContext = m_idHelperSvc->cscIdHelper().channel_context();	
        m_idHelperSvc->cscIdHelper().get_id(stripHash, stripId, &channelContext);

        int chamLayer = m_idHelperSvc->cscIdHelper().chamberLayer(stripId);
        if(chamLayer == m_expectedChamberLayer) //Only second chamber layer exists
        {
          int stationName = m_idHelperSvc->cscIdHelper().stationName(stripId);
          //int stationEta = m_idHelperSvc->cscIdHelper().stationEta(stripId);
          int stationPhi = m_idHelperSvc->cscIdHelper().stationPhi(stripId);
          int stripNumber = m_idHelperSvc->cscIdHelper().strip(stripId);
          int wireLayer = m_idHelperSvc->cscIdHelper().wireLayer(stripId);
          char orientation = m_idHelperSvc->cscIdHelper().measuresPhi(stripId) ? 'Y':'X';

          int sector = 2*stationPhi + 50 - stationName;

          std::stringstream name; 
          name << "h_bitCorr_sector_" << std::setfill('0') << std::setw(2) << sector << 
            "_lay_" << wireLayer  << orientation << "_strip_" << std::setw(3) << stripNumber; 
          std::stringstream title; 
          title << "h_bitCorr_sector_" << std::setfill('0') << std::setw(2) << sector << 
            "_lay_" << wireLayer  << orientation << "_strip_" << std::setw(3) << stripNumber; 

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

              float xy = bitProds->GetBinContent(bit1,bit2);
              float x = bitHist->GetBinContent(bit1);
              float y = bitHist->GetBinContent(bit2);

              float r;
              float denom = (n*x-x*x)*(n*y-y*y);
              if(denom <= 0 )
                r= 0;
              else 
                r = (n*xy - x*y)/std::sqrt(denom);

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


    void CscCalcPed::onlineToOfflineHashId(const unsigned int & onlineId, unsigned int &hashId) const
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

      Identifier chanId = m_idHelperSvc->cscIdHelper().channelID(stationName,eta,phi,chamLay,wireLay,measuresPhi,strip);

      IdentifierHash chanHash;
      m_idHelperSvc->cscIdHelper().get_channel_hash(chanId, chanHash);

      hashId = (unsigned int)chanHash;

    }

  }//end namespace MuonCalib
