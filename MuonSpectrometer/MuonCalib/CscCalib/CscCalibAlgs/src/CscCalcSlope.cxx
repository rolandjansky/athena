/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CscCalcSlope.h"

#include "MuonRDO/CscRawData.h"
#include "MuonRDO/CscRawDataCollection.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "CscCalibTools/ICscCalibTool.h"
#include "GaudiKernel/Chrono.h"
#include "MuonCondInterface/CscICoolStrSvc.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"
#include "CscCalibData/CscCalibResultContainer.h"
#include "CscCalibData/CscCalibReportContainer.h"
#include "CscCalibData/CscCalibReportSlope.h"
#include "BipolarFit.h"

#include "TH1I.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TFile.h"
#include "TProfile.h"

#include <iostream>
#include <fstream>
#include <bitset>
#include <inttypes.h>

namespace MuonCalib {

  CscCalcSlope::CscCalcSlope(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name,pSvcLocator),
    m_storeGate(nullptr),
    m_cscCalibTool(nullptr),
    m_chronoSvc(nullptr),
    m_outputFileName("output.cal"),
    m_dumpAllHists(false),
    m_maxStripHash(0),
    m_lastPulserLevel(-999),
    m_fracProfs(nullptr),
    m_fracGraphs(nullptr),
    m_bitHists(nullptr),
    m_fitReturns(nullptr),
    m_resGraph(nullptr),
    m_calGraphs(nullptr),
    m_currentAmpProf(nullptr),
    m_ampProfs(nullptr),
    m_pulsedChambers(nullptr),
    m_eventCnt(0),
    m_slopes(nullptr),
    m_intercepts(nullptr),
    m_peds(nullptr),
    m_noises(nullptr),
    m_peakTimeProf(nullptr),
    m_peakTimes(nullptr),
    m_numBits(12)
  {
    declareProperty("OutputFile", m_outputFileName = "");
    declareProperty("IgnoreDatabaseError",m_ignoreDatabaseError = false); //Set to true to ignore database errors
    declareProperty("TitlePrefix",m_titlePrefix = "");		//Prefix appended to title of histograms and graphs
    declareProperty("TitlePostfix",m_titlePostfix = ""); 	//Postfix appended to title of histograms and graphs

    //test parameters
    declareProperty("DoBipolarFit", m_doBipolarFit = true);
    declareProperty("DoCrossTalkFix",m_doCrossTalkFix = true);

    declareProperty("GetPedFromFile",m_pedFile = false);
    declareProperty("PedFileName",m_pedFileName = "");

    declareProperty("ExpectedChamberLayer", m_expectedChamberLayer = 2);

    declareProperty("DoLinPlot" , m_doLinPlot = false);
    declareProperty("CalibFitFunction" , m_calFitFunc = "[0] + [1]*10^(x/-20)");
    declareProperty("MinDeltaAdc",m_minDeltaAdc = 10, "Minimum change in ADC a calgraph needs to drop for a fit lower bound to be set");

    /*
       ADC  = mC(db) = m*C_max*10^(db/20) + intercept // C_max = maximum charge

       we divide by c_max later to get m (the function as in m_calFitFunc has
       [1] = max ADC, so we divide max charge to get 
       
       since attenuation is negative

       db = -20 log(ADC+intercept/mC_max)
       
       db = -20 log(ADC - intercept / mC_max)

       (-20 since V^2 gives power disipation)

       */

    declareProperty("FindPeakTime", m_findPeakTime = true);
    declareProperty("DoBitHists", m_doBitHists = true);
    
    declareProperty("CalOutputVersion", m_calOutputVersion="03-00");

    m_crossTalkFix = new double[24];
    m_crossTalkFix[0] = 1.0322840930;
    m_crossTalkFix[1] = 1.0422690324;
    m_crossTalkFix[2] = 1.0235384586;
    m_crossTalkFix[3] = 1.0183445962;
    m_crossTalkFix[4] = 1.0151409212;
    m_crossTalkFix[5] = 1.0152511102;
    m_crossTalkFix[6] = 1.0103618910;
    m_crossTalkFix[7] = 1.0113985580;
    m_crossTalkFix[8] = 1.0040464232;
    m_crossTalkFix[9] = 1.0049431193;
    m_crossTalkFix[10] = 0.9997829589;
    m_crossTalkFix[11] = 1.0003994005;
    m_crossTalkFix[12] = 0.9826108255;
    m_crossTalkFix[13] = 0.9850002836;
    m_crossTalkFix[14] = 0.9831852065;
    m_crossTalkFix[15] = 0.9826508145;
    m_crossTalkFix[16] = 0.9804885017;
    m_crossTalkFix[17] = 0.9811262196;
    m_crossTalkFix[18] = 0.9784119832;
    m_crossTalkFix[19] = 0.9777689757;
    m_crossTalkFix[20] = 0.9704773978;
    m_crossTalkFix[21] = 0.9738781078;
    m_crossTalkFix[22] = 0.9710430303;
    m_crossTalkFix[23] = 0.9743144079;
  }

  StatusCode CscCalcSlope::initialize()
  {
    ATH_MSG_INFO("CscCalcSlope::initialize() called");

    //*******Register services and tools *********/ 	
    // Store Gate active store
    ATH_CHECK(serviceLocator()->service("StoreGateSvc", m_storeGate));

    ATH_CHECK(m_idHelperSvc.retrieve());
 
    ATH_CHECK(service("ChronoStatSvc",m_chronoSvc));

    IToolSvc* toolSvc=nullptr;
    ATH_CHECK(service("ToolSvc",toolSvc));

    ATH_CHECK(toolSvc->retrieveTool("CscCalibTool",m_cscCalibTool));

    ATH_CHECK(m_cscRdoDecoderTool.retrieve());

    ATH_MSG_INFO("Finished initializing services. "); 
    //*****Initialize internal variables and histograms*******/	
    m_ampProfs = new std::map<int, TProfile* >();
    //Setup lookup table for pulser levels
    m_dbLevels.resize(64);
    for(unsigned int pulserLevel=0; pulserLevel < 64; pulserLevel++)
      m_dbLevels[pulserLevel] = pulserLevel*.5;

    IdContext channelContext = m_idHelperSvc->cscIdHelper().channel_context();

    if(m_doBitHists) m_bitHists = new DataVector<TH1I>(SG::VIEW_ELEMENTS);
    //Loop through ids to find out what hash range we're working on, and to 
    //initialize histograms.
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

        if(m_maxStripHash < (unsigned int)stripHash)
          m_maxStripHash = (unsigned int)stripHash; 

        if(m_bitHists)
        {
          Identifier id;
          m_idHelperSvc->cscIdHelper().get_id((IdentifierHash)stripHash,id,&channelContext);
          int wireLayer = m_idHelperSvc->cscIdHelper().wireLayer(id);
          char orientation = (m_idHelperSvc->cscIdHelper().measuresPhi(id) ? 'Y':'X');

          int stationName = m_idHelperSvc->cscIdHelper().stationName(id);
          int stationPhi = m_idHelperSvc->cscIdHelper().stationPhi(id);
          int stationEta = m_idHelperSvc->cscIdHelper().stationEta(id);

          int stripNumber = m_idHelperSvc->cscIdHelper().strip(id);

          char bitName[200], titleSeed[500];
          //Bit histogram (for looking for stuck-bits)
          sprintf(bitName, "bitHist%d",(int)stripHash);
          sprintf(titleSeed, "Bit histogram for eta %d, sector %d, layer %d%c strip %d", 
              stationEta,(2*stationPhi+50 - stationName),wireLayer,orientation,stripNumber);
          std::string title = m_titlePrefix + titleSeed + m_titlePostfix;
          TH1I* hist = new TH1I(bitName, title.c_str(), m_numBits, 0, m_numBits); //12 bits
          hist->GetXaxis()->SetTitle("Bit");
          hist->GetYaxis()->SetTitle("Counts");
          m_bitHists->push_back(hist);
        }
      }
    }//end chamber loop

    m_fitReturns = new std::vector<float>;
    m_fitReturns->resize(m_maxStripHash+1,0);

    m_calGraphs = new DataVector<TGraphErrors>(SG::VIEW_ELEMENTS);
    for(unsigned int chanItr =0; chanItr <= m_maxStripHash; chanItr++)
    {
      m_calGraphs->push_back(nullptr);
    }


    if(m_pedFile)
    {
      ATH_MSG_INFO("Opening pedestal file");
      std::ifstream in(m_pedFileName.c_str());
      int stripHash;
      double ped,noise;//,pedError,noiseError;
      std::string buff;
      in >> buff >> buff >> buff >> buff >> buff ;// skip header 
      m_peds = new float[m_maxStripHash+1];
      m_noises = new float[m_maxStripHash+1];

      while(!in.eof())
      {
        in >> stripHash >> buff >> buff >> ped >> noise;
        ATH_MSG_INFO(stripHash << "\t" << ped << "\t" << noise);
        if( stripHash < 0 || (unsigned int) stripHash > m_maxStripHash ) {
          ATH_MSG_FATAL("The hash "<< (int) stripHash << " is out of range for the Ped-Vector - Crashing!");
          return StatusCode::FAILURE;
        }
        m_peds[stripHash] = ped;
        m_noises[stripHash] = noise;
      }
    }
    else
    {
      ATH_CHECK(m_readKey.initialize());
    }

    ATH_MSG_INFO("Counted " << m_maxStripHash +1 << " strips.");

    m_slopes = new CscCalibResultCollection("pslope");
    m_intercepts = new CscCalibResultCollection("pinter");

    if(m_findPeakTime)
    {
      m_peakTimeProf = new TProfile("PeakTimes","Peaking Time for each channel",m_maxStripHash+1,
          0,m_maxStripHash+1);
      m_peakTimes = new CscCalibResultCollection("peakt");
    }

    m_pulsedChambers = new std::set<int>;


    ATH_MSG_DEBUG("End initialize");
    return StatusCode::SUCCESS;
  }//end initialize

  //Execute loops through all strips and fills histograms
  StatusCode CscCalcSlope::execute()
  {
    ATH_MSG_INFO("Begin execute");	
    //collectEventInfo collects infomation about each event by filling ampHistCollection and peaktHist.
    StatusCode sc = collectEventInfo();

    if(!sc.isSuccess())
    {
      ATH_MSG_WARNING("There was an error collecting information from the RDO this event.");
      return sc;
    }
    ATH_MSG_INFO("End execute");	
    return StatusCode::SUCCESS;
  } //end execute()

  StatusCode CscCalcSlope::finalize()
  {
    ATH_MSG_INFO("In finalize()");

    StatusCode sc;

    bool thereIsAFatal=false;

    //calculateParameters() finds means and fits gain curves from the data in 
    ///m_adcValues and/or m_allPeaktsHist 
    sc =calculateParameters();
    if(sc.isFailure())
    {
      ATH_MSG_WARNING("Calculation of parameters failed!");
    }
    ATH_MSG_DEBUG("Finished calculating parameters");

    //writeCalibrationFile() writes the calculated parameters into a calibration fie.
    sc = writeCalibrationFile();
    if(!sc.isSuccess())
    {
      ATH_MSG_FATAL("Failed to write parameters to disk!");
      thereIsAFatal = true; //Not quiting yet to ensure memory is properly deleted
    }

    sc = storeGateRecord();
    if(sc.isFailure())
    {
      ATH_MSG_FATAL("Failed to record parameters in StoreGate ");
      thereIsAFatal = true;
    }

    delete m_peakTimeProf;

    delete [] m_crossTalkFix;
    ATH_MSG_DEBUG("Finished finalize()");

    if(thereIsAFatal)
      return StatusCode::FAILURE; 

    return StatusCode::SUCCESS;	
  }//end finalize()


  //Collect event info is run every event, gathering amplitutdes and peakting times.
  StatusCode CscCalcSlope::collectEventInfo()
  {
    MsgStream mLog( msgSvc(), name() );

    // apparently not used since code is exited automatically if there is an error
    // bool thereIsAnError = false;

    Chrono chrono(m_chronoSvc,"collectEventInfo");
    ATH_MSG_DEBUG("Collecting event info for event " << m_eventCnt);
    //Below might need to be changed depending on how we get data
    const CscRawDataContainer* fullRDO;
    StatusCode sc_read = m_storeGate->retrieve(fullRDO, "CSCRDO"); 
    if (sc_read != StatusCode::SUCCESS)
    {
      ATH_MSG_FATAL("Could not find event");
      return StatusCode::FAILURE;
    }  

    SG::ReadCondHandle<CscCondDbData> readHandle{m_readKey}; 
    const CscCondDbData* readCdo{*readHandle};
       
    //Loop over RODs (data from 2 chambers), each of which is in
    //a single CscRawaData collection
    CscRawDataContainer::const_iterator rodItr = fullRDO->begin();
    CscRawDataContainer::const_iterator rodEnd = fullRDO->end();
    for(;rodItr != rodEnd; rodItr++)
    {
      const CscRawDataCollection * rod = (*rodItr); 	//Removing another "pointer layer" to make
      if(rod->size() >0) 
      {

        uint16_t pulsedWireLayer = rod->calLayer();

        int pulserLevel = rod->calAmplitude(); 
        ATH_MSG_VERBOSE("Pulser level is " << pulserLevel);
        if( pulserLevel != m_lastPulserLevel)
        {
          ATH_MSG_INFO("New pulser level found. (" << pulserLevel <<").");

          std::map<int,TProfile*>::iterator alreadyExistingProfile = m_ampProfs->find(pulserLevel);

          if(alreadyExistingProfile == m_ampProfs->end())
          {//No previous profile for this amplitude exists

            ATH_MSG_DEBUG(" creating new amplitude profile");
            std::stringstream name, title;
            name << "ampProf_" << pulserLevel;
            title << m_titlePrefix << "Amplitudes For Pulser Level " << pulserLevel << m_titlePostfix;
            m_currentAmpProf = new TProfile(name.str().c_str(), title.str().c_str(), 
                m_maxStripHash+1, 0, m_maxStripHash);
            m_currentAmpProf->GetXaxis()->SetTitle("Channel (Hash Id)");
            m_currentAmpProf->GetYaxis()->SetTitle("Amplitude (ADC value)");
            ATH_MSG_DEBUG("Adding new amplitude profile");
            m_ampProfs->insert(std::pair<int, TProfile*>( pulserLevel, m_currentAmpProf));
          }
          else
          {
            ATH_MSG_DEBUG(" using existing amplitude profile");
            m_currentAmpProf = alreadyExistingProfile->second;
          }

          m_lastPulserLevel = pulserLevel;
        }

        unsigned int samplingPhase = rod->samplingPhase();		
        uint8_t samplingPeriod = rod->rate(); //sampling period in ns


        //Loop over strips in rod
        CscRawDataCollection::const_iterator clusItr = rod->begin();
        CscRawDataCollection::const_iterator clusEnd = rod->end();
        for(; clusItr!=clusEnd ; clusItr++)
        {
          const CscRawData * cluster = (*clusItr); //Note: For a pulser or ped run, the "cluster" 
          //is the size of an entire layer
          int numStrips = cluster->width();
          int samplesPerStrip = (cluster->samples()).size()/numStrips;

          IdContext channelContext = m_idHelperSvc->cscIdHelper().channel_context();	

          for(int stripItr = 0; stripItr <numStrips; stripItr++)
          {
            Identifier stripId =m_cscRdoDecoderTool->channelIdentifier(cluster, stripItr);
            IdentifierHash cscChannelHashId;
            m_idHelperSvc->cscIdHelper().get_channel_hash(stripId, cscChannelHashId);
            int stripHash = cscChannelHashId;
            ATH_MSG_VERBOSE("The eta of this strip is: " << m_idHelperSvc->cscIdHelper().stationEta(stripId));

            int chamberLayer = m_idHelperSvc->cscIdHelper().chamberLayer(stripId);
            if(chamberLayer != m_expectedChamberLayer)
            {
              ATH_MSG_FATAL("Cluster includes strip in chamber layer "
                << chamberLayer << ". Only " << m_expectedChamberLayer 
                << " is valid.");
              return StatusCode::FAILURE;
            }

            int currentWireLayer = m_idHelperSvc->cscIdHelper().wireLayer(stripId) - 1;
            if( currentWireLayer < 0 || currentWireLayer > 3)
            {
              ATH_MSG_FATAL("Problem in getting wire layer! - Current value is " 
                   << m_idHelperSvc->cscIdHelper().wireLayer(stripId) << " while only values between 1-4 are allowed.");
              return StatusCode::FAILURE;
            }
            bool isThisLayerPulsed = (pulsedWireLayer >> currentWireLayer)&0x1;
            if(isThisLayerPulsed)
            {

              std::vector<uint16_t> samples;
              cluster->samples(stripItr,samplesPerStrip,samples); //Retrieve samples for a single strip

              float ped;
              float noise;
              if(m_pedFile)
              {
                ped = m_peds[stripHash];
                noise = m_noises[stripHash];
              }
              else{

                if(!readCdo->readChannelPed(stripHash, ped).isSuccess()){
                  ped = 2054;
                  if (m_ignoreDatabaseError) ATH_MSG_WARNING("Failed at getting pedestal from COOL for hash " << stripHash);
                  else ATH_MSG_ERROR("Failed at getting pedestal from COOL for hash " << stripHash);
                  if(!m_ignoreDatabaseError)
                    return StatusCode::RECOVERABLE;
                  ATH_MSG_WARNING("Setting to " << ped);
                }
                else
                  ATH_MSG_VERBOSE("Got pedestal of " << ped);
                if(!readCdo->readChannelNoise(stripHash, noise).isSuccess())
                {
                  noise = .001;
                  if (m_ignoreDatabaseError) ATH_MSG_WARNING("Failed at getting noise from COOL for hash " << stripHash);
                  else ATH_MSG_ERROR("Failed at getting noise from COOL for hash " << stripHash);
                  if(!m_ignoreDatabaseError)
                    return StatusCode::FAILURE;
                  ATH_MSG_WARNING("Setting to " << noise);
                }

              }

              double peakAmp, peakTime;
              int success;
              if(!m_doBipolarFit)
              {
                //Need to convert vector from ints to floats to pass to findCharge 
                std::vector<float> floatSamples; 
                std::vector<uint16_t>::const_iterator sampItr = samples.begin();
                std::vector<uint16_t>::const_iterator sampEnd = samples.end();
                for(;sampItr != sampEnd; sampItr++){

                  floatSamples.push_back((*sampItr)-ped);
                  if(m_bitHists){
                    if(!fillBitHist((*m_bitHists)[stripHash],*sampItr)){
                      ATH_MSG_WARNING("Failed recording bits for strip " << stripHash);
                    }

                  }
                }

                success = m_cscCalibTool->findCharge((float)samplingPeriod, samplingPhase,floatSamples,peakAmp,peakTime);

              } 
              else
              {
                //Need to convert vector from ints to doubles to pass to bipolar fit
                double adcSamples[4];
                for(int i = 0; i < 4; i++) adcSamples[i] = samples[i] -ped;
                double fitResult[3],fitErrors[3], chi2;
                double width = samplingPeriod == 50 ? 7.2:14.4; //check if 50 or 25ns period

                m_bipolarFit.Fit(adcSamples,noise,ped,width,fitResult,fitErrors,&chi2);
                success = true;
                peakAmp = fitResult[0];
                peakTime = fitResult[1] - (samplingPhase ? 25 : 0);
              }//end if m_doBipolarFit


              if(success)
              {
                m_currentAmpProf->Fill(stripHash,peakAmp); 
                //((*m_ampHists)[stripHash])->Fill(peakAmp);

                if(m_findPeakTime)
                  m_peakTimeProf->Fill(stripHash,peakTime);
              }	
              else
              {
                ATH_MSG_WARNING("Failed at fitting pulse shape. Debug info: ");
                ATH_MSG_WARNING("stripHash "   << stripHash);
                ATH_MSG_WARNING("strip in chamber " << stripItr);
                ATH_MSG_WARNING(" and detailed id " <<  m_idHelperSvc->cscIdHelper().show_to_string(stripId,&channelContext));
                ATH_MSG_WARNING("Pulsed layer " << pulsedWireLayer<< ", Samples: "  << samples[0] <<", " << samples[1] << ", " << samples[2] << ", " << samples[3]);
              }
            }//end if (islayerPulsedand and is precision layer)
          }//end strip loop

        }//end cluster loop
      }//end if rod >1
    }//end rod loop


    ATH_MSG_DEBUG("end collectEventInfo()");
    m_eventCnt++;

    // at this part of the code thereIsAnError is always false - if true it would exit earlier
    // if(thereIsAnError)
    //   return StatusCode::RECOVERABLE;

    return StatusCode::SUCCESS;
  }// end collectEventInfo()


  //Calculate parameters is called during finalize, and calculates the parameter values from the
  //data taken by collectEventData()
  StatusCode CscCalcSlope::calculateParameters()
  {
    Chrono chrono(m_chronoSvc,"calculateParameters");
    StatusCode sc; 
    ATH_MSG_INFO("Calculating calibration constants.");

    if(!m_ampProfs){
      ATH_MSG_FATAL("m_ampProfs empty!");
      return StatusCode::FAILURE;
    }
    unsigned int numCalibPoints = m_ampProfs->size();	
    ATH_MSG_INFO("There are " << numCalibPoints << " pulser levels to evaluate.");

    IdContext channelContext = m_idHelperSvc->cscIdHelper().channel_context();	

    float chargeMax = 530.88; //in fC

    int crossTalkCnt = 0;

    for(unsigned int stripHash = 0 ;stripHash <= m_maxStripHash; stripHash++)
    {

      if(true)//stripHash < 50 || stripHash%1000 == 0)
      {
        ATH_MSG_INFO("Analyzing strip with hash " << stripHash << " out of " << m_maxStripHash); 
      }

      //**Now tackle slope calculation


      Identifier id;
      m_idHelperSvc->cscIdHelper().get_id((IdentifierHash)stripHash,id,&channelContext);
      int chamberLayer = m_idHelperSvc->cscIdHelper().chamberLayer(id);
      char orientation = (m_idHelperSvc->cscIdHelper().measuresPhi(id) ? 'Y':'X');

      int wireLayer = m_idHelperSvc->cscIdHelper().wireLayer(id);


      int stationName = m_idHelperSvc->cscIdHelper().stationName(id);
      int stationPhi = m_idHelperSvc->cscIdHelper().stationPhi(id);
      int stationEta = m_idHelperSvc->cscIdHelper().stationEta(id);
      int stripNumber = m_idHelperSvc->cscIdHelper().strip(id);

      IdentifierHash chamHash;
      m_idHelperSvc->cscIdHelper().get_module_hash(id,chamHash);

      if(chamberLayer != m_expectedChamberLayer)
        continue;

      if(m_findPeakTime)
      {
        if(m_peakTimeProf->GetBinEntries(stripHash+1)) //See if any peaking times were recorded for strip
        {
          float peakt = m_peakTimeProf->GetBinContent(stripHash+1);
          float peaktError = m_peakTimeProf->GetBinError(stripHash+1);
          CscCalibResult * peaktResult = new CscCalibResult(stripHash,peakt, peaktError);
          m_peakTimes->push_back(peaktResult);
        }
      }//end if(m_findPeakTime)

      //Don't find slope for this strip if it is a transverse strip
      if(orientation != 'X')
        continue;

      //For removing plateau's from fit
      bool foundMin(false);
      double fitMinX = 0;
      double fitMaxX = 0;
      double lastVal = -1;
      double lastDrop=0;
      double thisDrop=0;


      TGraphErrors * calGraph = new TGraphErrors(numCalibPoints);      //calGraph will be what the gain will be found on
      char calName[20],titleSeed[500];
      sprintf(calName, "calGraph%u",stripHash);
      sprintf(titleSeed, "Calgraph for eta %d, sector %d, layer %d%c, strip %d",stationEta,(2*stationPhi+50 - stationName),wireLayer,orientation, stripNumber);
      calGraph->SetName(calName);
      std::string title = m_titlePrefix + titleSeed + m_titlePostfix;
      calGraph->SetTitle(title.c_str());
      calGraph->GetYaxis()->SetTitle("ADC counts");
      calGraph->GetXaxis()->SetTitle("Attenuation (-db)");

      ATH_MSG_DEBUG(" Generating " << title);   

      bool isGoodStrip = false;

      //Loop over all attenuation levels, filling the calGraph with the amplitudes
      //for this strip 
      ATH_MSG_DEBUG("Number of ampProfs " << m_ampProfs->size());
      int calPointItr = 0;
      std::map<int, TProfile*>::const_iterator ampProfItr = m_ampProfs->begin();
      std::map<int, TProfile*>::const_iterator ampProfEnd = m_ampProfs->end();
      for(; ampProfItr != ampProfEnd; ampProfItr++)
      {
        if(!ampProfItr->second){
          ATH_MSG_FATAL("Failed at accessing ampProf!");
          return StatusCode::FAILURE;
        }
        ATH_MSG_DEBUG("\tLooking for data for pulser level "
           << ampProfItr->first);

        if(ampProfItr->second->GetBinEntries(stripHash+1))
        {

          ATH_MSG_VERBOSE("\nHave data for strip " << stripHash);

          isGoodStrip = true;

          int pulserLevel = ampProfItr->first; 
          float adcValue = ampProfItr->second->GetBinContent(stripHash+1); 
          float adcError = ampProfItr->second->GetBinError(stripHash+1); 
          if(m_doCrossTalkFix)
          {
            ATH_MSG_VERBOSE("\tCrosstalk fix " << m_crossTalkFix[crossTalkCnt]);
            adcValue /= m_crossTalkFix[crossTalkCnt];
            adcError /= m_crossTalkFix[crossTalkCnt];
          }
          if(adcError != adcError)
            adcError = 0.01;

          float db = m_dbLevels[pulserLevel];
          

          float attenValue =0;
          if(m_doLinPlot)
            attenValue = 300*std::pow(10,db/20);
          else 
            attenValue = db;

          ATH_MSG_DEBUG("\tStoring at db of " << db << " with attenValue " << attenValue << " from pulser level of " << pulserLevel << " and adcValue " << adcValue);


          
          //See if the last two drops were far down enough
          if(!foundMin){
            thisDrop = lastVal - adcValue;
            ATH_MSG_DEBUG("\tFinding fit min:" 
             << "\tlastVal = " << lastVal
             << ";lastDrop " << lastDrop << "; thisDrop " << thisDrop);
            if(thisDrop > m_minDeltaAdc && lastDrop > m_minDeltaAdc){
              ATH_MSG_DEBUG("Found fitMin!");
              foundMin = true;
              fitMinX = attenValue;
            }
            else{
              //Not enough deltaADC, store this iterations values for the next loop
              lastDrop = thisDrop;
              lastVal = adcValue;
            }
          }

          //Use highest attenuation level as fitMaxX
          if(attenValue > fitMaxX)
            fitMaxX = attenValue;

          calGraph->SetPoint(calPointItr,attenValue,adcValue);
          calGraph->SetPointError(calPointItr,0.01,adcError);
          calPointItr++;
        }//done if(entries >0)

      }//Done ampProfItr loop

      if(!foundMin && isGoodStrip){
        ATH_MSG_WARNING("Failed to find minium for " << title); 
      }

      //***Do a simple fit to calGraph***
      //Here we only fit the linear part of the plot. m_fitCutoff can be set by user.			
      if(isGoodStrip)
      {
        ATH_MSG_INFO("we have a good stripHash at " << stripHash); 

        m_pulsedChambers->insert(chamHash); //Programer note: Only gets filled on x-axis. Probably OK.

        float slope, slopeError, intercept, interceptError, chiSquared;
        int ndf;
        int fitRet=0;

        //Setup our gain fit function
        TF1  myFunc("myFunction", m_calFitFunc.c_str(), fitMinX, fitMaxX);
        myFunc.SetLineColor(kRed);
        if(m_doLinPlot)
        {
          myFunc.SetParameters(0,5);
          slope = myFunc.GetParameter(1);
          slopeError = myFunc.GetParError(1);
          intercept = myFunc.GetParameter(0);
          interceptError = myFunc.GetParError(0);
          chiSquared = myFunc.GetChisquare();
          ndf = myFunc.GetNDF();
        }
        else
        {
          myFunc.SetParameters(0.1,2000);

          fitRet = calGraph->Fit(&myFunc,"RV");

          slope = myFunc.GetParameter(1)/chargeMax;
          slopeError = myFunc.GetParError(1);
          intercept = myFunc.GetParameter(0);
          interceptError = myFunc.GetParError(0);
          chiSquared = myFunc.GetChisquare(); 
          ndf = myFunc.GetNDF();
        }

        float invertedSlope;
        if(std::abs(slope) < 0.00001 || slope == -999) //watch out for slope==0 
        {
          ATH_MSG_WARNING("Slope invalid ");
          continue;
        }

        invertedSlope = 1/slope;

        ATH_MSG_ERROR("Inserting calgraph in for hash " << stripHash);
        (*m_calGraphs)[stripHash] = calGraph;

        ATH_MSG_DEBUG("StripHash: " << stripHash << "; slope: " <<slope  
          << "; intercept: " << intercept
          << "; chi^2/ndf: " << chiSquared << "/" << ndf);
        CscCalibResult * slopeResult = new CscCalibResult(stripHash,invertedSlope,slopeError,chiSquared,ndf);
        CscCalibResult * interceptResult = new CscCalibResult(stripHash, intercept, interceptError, chiSquared, ndf);                

        m_slopes->push_back(slopeResult);
        m_intercepts->push_back(interceptResult);
        (*m_fitReturns)[stripHash] = fitRet;

      }//end if(isGoodStrip)


      if(crossTalkCnt == 23)
        crossTalkCnt = 0;
      else
        crossTalkCnt++;
      ATH_MSG_DEBUG("Looping over next strip...");
    }//end loop over strips
    ATH_MSG_INFO("Completed calculating parameters for each strip");
    return StatusCode::SUCCESS;
  }//End calculateParameters()

  //writeCalibrationFile() dumps the parameters to disk
  StatusCode CscCalcSlope::writeCalibrationFile()
  {
    Chrono chrono(m_chronoSvc,"writeCalibrationFile");
    if(m_calOutputVersion == "00-00"){
      ATH_MSG_INFO("Printing output file version 00-00");
      return calOutput0();
    }
    else if(m_calOutputVersion == "03-00") {
      ATH_MSG_INFO("Printing output file version 03-00");
      return calOutput3();
    }
    else{
      ATH_MSG_INFO("Don't know how to write calibration file version " << m_calOutputVersion);
      return StatusCode::RECOVERABLE;
    }
  }

  StatusCode CscCalcSlope::calOutput0(){
    //***Take conditions data held in summary histograms and  print to the calibration file***//
    ATH_MSG_INFO("Parameters calculated, preparing to outputing to file: " << m_outputFileName);
    std::ofstream out;
    out.open(m_outputFileName.c_str());
    if(!out.is_open())
    {
      ATH_MSG_FATAL("Can't open file " << m_outputFileName.c_str() << "for writing");
      return StatusCode::FAILURE;
    }
    //Start by writing file version number (mainly for COOL program to read)
    out << "00-00 ";
    //Number of strips we have info for:
    out << m_slopes->size() << " ";
    //print out header
    out << "pslope ";
    if(m_findPeakTime) out << "peakt ";
    out << "END_HEADER\n";			
    //Now we loop over each strip's parameters and print them out
    ATH_MSG_DEBUG("Begining loop over all " << m_maxStripHash  << " hash ids.");

    //form is:
    //hashID chamber LayerOrientationStrip  parametervalue parametervalue 
    CscCalibResultCollection::iterator slopeItr = m_slopes->begin();
    CscCalibResultCollection::iterator slopeEnd = m_slopes->end(); 
    CscCalibResultCollection::iterator peaktItr;
    CscCalibResultCollection::iterator peaktEnd;
    if(m_findPeakTime)
    {
      peaktItr = m_peakTimes->begin();
      peaktEnd = m_peakTimes->end();
    }        
    for(; slopeItr != slopeEnd; slopeItr++)
    {
      if(m_findPeakTime && (peaktItr == peaktEnd) )
      {
        ATH_MSG_FATAL("Peaktimes out of sync with slopes. Quiting write.");

        return StatusCode::FAILURE;
      }

      //Output hashId
      out << (*slopeItr)->hashId();

      //get id for next few outputs
      Identifier id;
      IdContext channelContext = m_idHelperSvc->cscIdHelper().channel_context();	
      m_idHelperSvc->cscIdHelper().get_id((*slopeItr)->hashId(),id, &channelContext);

      //output chamber #
      IdentifierHash chamberHash;
      Identifier chamberId = m_idHelperSvc->cscIdHelper().elementID(id);
      if(!m_idHelperSvc->cscIdHelper().valid(chamberId))
      {
        ATH_MSG_FATAL(chamberId.getString() << " is not a valid id!");
        ATH_MSG_FATAL("identifier is: " << m_idHelperSvc->cscIdHelper().show_to_string(chamberId));
        return StatusCode::FAILURE;
      }

      m_idHelperSvc->cscIdHelper().get_module_hash(id,chamberHash);
      out <<" " << chamberHash;

      //output strip details
      out << " " << m_idHelperSvc->cscIdHelper().show_to_string(id) << " ";

      //output parameter values	
      out << " " << (*slopeItr)->value();
      if(m_findPeakTime) out << " " << (*peaktItr)->value();
      out << "\n" ; //to improve readability
    } //end loop over hash Ids

    out.close(); //done writing		

    return StatusCode::SUCCESS;	
  }//end calOutput0

  StatusCode CscCalcSlope::storeGateRecord()
  {
    ATH_MSG_INFO("Recording csc calibration report.");

    StatusCode sc = StatusCode::SUCCESS;

    bool thereIsAnError = false;

    std::string histKey = "cscSlopeCalibReport";
    ATH_MSG_DEBUG("Recording calibration graphs to TDS with key " << histKey);

    CscCalibReportSlope * report = new CscCalibReportSlope("calGraphs");

    report->setCalGraphs(m_calGraphs);
    report->setAmpProfs(m_ampProfs);
    report->setPulsedChambers(m_pulsedChambers);
    report->setBitHists(m_bitHists);
    report->setFitResults(m_fitReturns);

    CscCalibReportContainer * repCont = new CscCalibReportContainer(histKey);
    repCont->push_back(report);

    sc = m_storeGate->record(repCont, histKey);
    if(sc.isFailure())
    {
      ATH_MSG_ERROR("Failed to record CscCalibReportSlope to storegate");
      thereIsAnError = true;
      //Since storegate isn't taking ownership, we'll delete it:
      delete repCont; 
    }

    CscCalibResultContainer * calibResults 
      = new CscCalibResultContainer("CscCalibResultSlope");
    calibResults->push_back(m_slopes);
    calibResults->push_back(m_intercepts);
    if(m_findPeakTime)
      calibResults->push_back(m_peakTimes);
    sc = m_storeGate->record(calibResults,"CscCalibResultSlope");
    if(sc.isFailure())
    {
      ATH_MSG_ERROR("Failed to record results to storegate");
      thereIsAnError = true;
      //Since storegate isn't taking ownership, we'll delete it
      delete calibResults;
    }

    if(thereIsAnError)
      return StatusCode::RECOVERABLE;

    return StatusCode::SUCCESS;
  }



  StatusCode CscCalcSlope::calOutput3() {
    std::ofstream out;
    out.open(m_outputFileName.c_str());
    if(!out.is_open())
    {
      ATH_MSG_ERROR("Can't open file " << m_outputFileName.c_str());
      return StatusCode::RECOVERABLE;
    }
    out << "03-00 <END_HEADER>";

    outputParameter3(*m_slopes, out);
    out << "\n<END_FILE>";
    out.close();

    ATH_MSG_INFO("Successfully opened file " << m_outputFileName);

    return StatusCode::SUCCESS;
  }


  void CscCalcSlope::outputParameter3(const CscCalibResultCollection & results, std::ofstream & out){
    ATH_MSG_INFO("Printing out parameter " << results.parName());

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
}//end namespace MuonCalib
