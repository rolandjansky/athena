/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Chrono.h"

#include "TF1.h"
#include "TGraphErrors.h"
#include "TFile.h"
#include "TProfile.h"

#include "StoreGate/StoreGate.h"

#include "MuonIdHelpers/CscIdHelper.h"

#include "MuonRDO/CscRawData.h"
#include "MuonRDO/CscRawDataCollection.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "CscCalibTools/ICscCalibTool.h"

#include "MuonCondInterface/CscICoolStrSvc.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"
//#include "MuonCondSvc/CscCoolStrSvc.h"

#include <string>
#include <iostream>
#include <fstream>
#include <bitset>
#include <inttypes.h>
//#include <math>

#include "CscCalcSlope.h"
#include "BipolarFit.h"

#include "CscCalibData/CscCalibResultContainer.h"
#include "CscCalibData/CscCalibReportContainer.h"
#include "CscCalibData/CscCalibReportSlope.h"


using namespace std;

namespace MuonCalib {

  CscCalcSlope::CscCalcSlope(const string& name, ISvcLocator* pSvcLocator) :
    Algorithm(name,pSvcLocator),
    m_cscCoolStrSvc("MuonCalib::CscCoolStrSvc",name),
    m_cscRdoDecoderTool ("Muon::CscRDO_Decoder"),
    m_outputFileName("output.cal"),
    m_lastPulserLevel(-999),
    m_currentAmpProf(NULL),
    m_ampProfs(NULL),
    m_pulsedChambers(NULL),
    m_eventCnt(0),
    m_peakTimeProf(NULL),
    m_numBits(12)
  {
    declareProperty("OutputFile", m_outputFileName = "");
    //declareProperty("FitCutoff",m_fitCutoff=0);
    declareProperty("IgnoreDatabaseError",m_ignoreDatabaseError = false); //Set to true to ignore database errors
    declareProperty("TitlePrefix",m_titlePrefix = "");		//Prefix appended to title of histograms and graphs
    declareProperty("TitlePostfix",m_titlePostfix = ""); 	//Postfix appended to title of histograms and graphs

    //Don't put the min at 0! At least some offset!
    //declareProperty("FitMin",m_calFuncMin=2);
    //declareProperty("EdgeChanFitMin", m_calFuncEdgeChanMin = 6);
    //declareProperty("FitMax",m_calFuncMax=31.5);

    //test parameters
    declareProperty("DoBipolarFit", m_doBipolarFit = true);
    declareProperty("DoCrossTalkFix",m_doCrossTalkFix = true);

    //declareProperty("DoPulserLinearityCorrection",m_doPulserFactor = true);
    //declareProperty("PulserLinearityFile", m_pulserFactorFile = "pulserFactors.txt");

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
    //declareProperty("CalibFitFunction", m_calFitFunc = "-20*TMath::Log10((x-[0])/[1])");
    //declareProperty("CalibFitFunction", m_calFitFunc = "-20*TMath::Log10((x-[0])/[1])");

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

  CscCalcSlope::~CscCalcSlope() {}

  StatusCode CscCalcSlope::initialize()
  {
    MsgStream mLog( msgSvc(), name() );

    mLog << MSG::INFO << "CscCalcSlope::initialize() called" << endreq;

    //*******Register services and tools *********/ 	
    // Store Gate active store
    StatusCode sc = serviceLocator()->service("StoreGateSvc", m_storeGate);
    if (sc != StatusCode::SUCCESS )
    {
      mLog << MSG::FATAL << " Cannot get StoreGateSvc " << endreq;
      return StatusCode::FAILURE ;
    }

cerr << "detstore" << endl;
    StoreGateSvc* detStore = 0;
    sc = serviceLocator()->service("DetectorStore", detStore);

    if(!sc.isSuccess()) 
    {
      mLog << MSG::FATAL << " DetectorStore not found" << endreq;
      return StatusCode::FAILURE;
    }
    
cerr << "idhelper" << endl;
    sc = detStore->retrieve(m_cscId,"CSCIDHELPER");
    if( sc.isFailure())
    {
      mLog << MSG::ERROR << " Cannot retrieve CscIdHelper " << endreq;
      return sc;
    }

cerr << "chorno" << endl;
    sc = service("ChronoStatSvc",m_chronoSvc);    
    if(sc.isFailure())
    {
      mLog << MSG::FATAL << "Cannot retrieve ChronoStatSvc!" << endreq;
      return StatusCode::FAILURE;
    }

    /*sc = service("THistSvc", m_thistSvc);
      if(sc.isFailure())
      {
      mLog << MSG::FATAL << "Cannot retrieve THistSvc!" << endreq;
      return StatusCode::FAILURE;
      }*/

cerr << "toolsvc" << endl;
    IToolSvc* toolSvc;
    sc = service("ToolSvc",toolSvc);
    if(sc.isFailure())
    {
      mLog << MSG::FATAL << "Unable to retrieve ToolSvc" << endreq;
      return StatusCode::FAILURE;
    }

cerr << "calibtool" << endl;
    sc = toolSvc->retrieveTool("CscCalibTool",m_cscCalibTool);
    if(sc.isFailure())
    {
      mLog << MSG::FATAL << "Unable to retrieve CscCalibTool" << endreq;
      return StatusCode::FAILURE;
    }



cerr << "decoder tool" << endl;
    if (m_cscRdoDecoderTool.retrieve().isFailure()){
      mLog << MSG::FATAL << "Cannot retrieve Csc_Rdo_Decoder Tool!" << endreq;
      return StatusCode::FAILURE;
    }

    mLog << MSG::INFO <<"Finished initializing services. " << endreq; 
cerr << "done init services" << endl;
    //*****Initialize internal variables and histograms*******/	
    m_ampProfs = new std::map<int, TProfile* >();
    //m_ampHists = new DataVector<TH1F>();
    //m_pulserLevels = new DataVector< std::vector<int> >;
    //m_adcValues = new DataVector< std::vector<double> >; 
    //m_adcErrors = new DataVector< std::vector<double> >;

    //Setup lookup table for pulser levels
    m_dbLevels.resize(64);
    for(unsigned int pulserLevel=0; pulserLevel < 64; pulserLevel++)
      m_dbLevels[pulserLevel] = pulserLevel*.5;

    IdContext channelContext = m_cscId->channel_context();

    if(m_doBitHists) m_bitHists = new DataVector<TH1I>(SG::VIEW_ELEMENTS);
    //Loop through ids to find out what hash range we're working on, and to 
    //initialize histograms.
    vector<Identifier> ids = m_cscId->idVector();
    vector<Identifier>::const_iterator chamItr = ids.begin();
    vector<Identifier>::const_iterator chamEnd = ids.end();
    m_maxStripHash = 0;
    for(; chamItr != chamEnd; chamItr++)
    {
      std::vector<Identifier> stripVect;
      m_cscId->idChannels(*chamItr,stripVect);

      /*
         vector<vector<float> >  chamberDbs;
         for(unsigned int layItr = 0; layItr <4; layItr++)
         {
         vector<float> layerDbs;
         for(unsigned int pulserLevel=0;pulserLevel< 64;pulserLevel++)
         {
         layerDbs.push_back(pulserLevel*.5);
         }
         chamberDbs.push_back(layerDbs);
         }
         m_dbLevels.push_back(chamberDbs);*/

      vector<Identifier>::const_iterator stripItr = stripVect.begin();
      vector<Identifier>::const_iterator stripEnd = stripVect.end();
      for(;stripItr != stripEnd; stripItr++)
      {
        IdentifierHash stripHash;
        m_cscId->get_channel_hash(*stripItr,stripHash);

        if(m_maxStripHash < (unsigned int)stripHash)
          m_maxStripHash = (unsigned int)stripHash; 

        if(m_bitHists)
        {
          Identifier id;
          m_cscId->get_id((IdentifierHash)stripHash,id,&channelContext);
          int wireLayer = m_cscId->wireLayer(id);
          char orientation = (m_cscId->measuresPhi(id) ? 'Y':'X');

          int stationName = m_cscId->stationName(id);
          int stationPhi = m_cscId->stationPhi(id);
          int stationEta = m_cscId->stationEta(id);

          int stripNumber = m_cscId->strip(id);

          char bitName[200], titleSeed[500];
          //Bit histogram (for looking for stuck-bits)
          sprintf(bitName, "bitHist%d",(int)stripHash);
          sprintf(titleSeed, "Bit histogram for eta %d, sector %d, layer %d%c strip %d", 
              stationEta,(2*stationPhi+50 - stationName),wireLayer,orientation,stripNumber);
          string title = m_titlePrefix + titleSeed + m_titlePostfix;
          TH1I* hist = new TH1I(bitName, title.c_str(), m_numBits, 0, m_numBits); //12 bits
          hist->GetXaxis()->SetTitle("Bit");
          hist->GetYaxis()->SetTitle("Counts");
          m_bitHists->push_back(hist);
        }
      }
    }//end chamber loop

    m_fitReturns = new std::vector<float>;
    m_fitReturns->resize(m_maxStripHash+1,0);
    /*m_allowedStrips = new bool[m_maxStripHash+1]*/;

    m_calGraphs = new DataVector<TGraphErrors>(SG::VIEW_ELEMENTS);
    for(unsigned int chanItr =0; chanItr <= m_maxStripHash; chanItr++)
    {
      m_calGraphs->push_back(NULL);
      /*IdentifierHash stripHash =chanItr;
        Identifier stripId;
        IdContext channelContext = m_cscId->channel_context();
        m_cscId->get_id(stripHash, stripId, &channelContext);
        m_allowedStrips[chanItr] = (m_cscId->chamberLayer(stripId) == m_expectedChamberLayer);*/
    }


    if(m_pedFile)
    {
      mLog << MSG::INFO << "Opening pedestal file" << endreq;
      ifstream in(m_pedFileName.c_str());
      int stripHash;
      double ped,noise;//,pedError,noiseError;
      string buff;
      in >> buff >> buff >> buff >> buff >> buff ;// skip header 
      m_peds = new float[m_maxStripHash+1];
      m_noises = new float[m_maxStripHash+1];

      while(!in.eof())
      {
        in >> stripHash >> buff >> buff >> ped >> noise;
        mLog << MSG::INFO << stripHash << "\t" << ped << "\t" << noise << endreq;
        m_peds[stripHash] = ped;
        m_noises[stripHash] = noise;
      }
    }
    else
    {
      if(m_cscCoolStrSvc.retrieve().isFailure())
      {
        mLog << MSG::FATAL << "Unable to retrieve CscCoolStrSvc" << endreq;
        return StatusCode::FAILURE;
      }
    }

    /*for(unsigned int stripItr = 0 ; stripItr < m_maxStripHash+1; stripItr++)
      {
      m_pulserLevels->push_back(new std::vector<int> );
      m_adcValues->push_back(new std::vector<double> );
      m_adcErrors->push_back(new std::vector<double> );
      }//End strip loop*/
    mLog << MSG::INFO << "Counted " << m_maxStripHash +1 << " strips." << endreq;

    //m_fracProfs = new DataVector<DataVector<TProfile> >();
    //m_fracGraphs = new DataVector<DataVector<TGraph> >();
    m_slopes = new CscCalibResultCollection("pslope");
    m_intercepts = new CscCalibResultCollection("pinter");

    if(m_findPeakTime)
    {
      m_peakTimeProf = new TProfile("PeakTimes","Peaking Time for each channel",m_maxStripHash+1,
          0,m_maxStripHash+1);
      m_peakTimes = new CscCalibResultCollection("peakt");
    }

    m_pulsedChambers = new std::set<int>;


    mLog <<MSG::DEBUG << "End initialize" << endreq;
    return StatusCode::SUCCESS;
  }//end initialize

  //Execute loops through all strips and fills histograms
  StatusCode CscCalcSlope::execute()
  {
    MsgStream mLog( msgSvc(), name() );
    mLog << MSG::INFO << "Begin execute" << endreq;	
    //collectEventInfo collects infomation about each event by filling ampHistCollection and peaktHist.
    StatusCode sc = collectEventInfo();

    if(!sc.isSuccess())
    {
      mLog << MSG::WARNING << "There was an error collecting information from the RDO this event." << endreq;
      return sc;
    }
    mLog << MSG::INFO << "End execute" << endreq;	
    return StatusCode::SUCCESS;
  } //end execute()

  StatusCode CscCalcSlope::finalize()
  {
    MsgStream mLog( msgSvc(), name() );
    mLog << MSG::INFO << "In finalize()" << endreq;

    StatusCode sc;

    bool thereIsAFatal=false;

    //makeCalibPoints(); 

    //calculateParameters() finds means and fits gain curves from the data in 
    ///m_adcValues and/or m_allPeaktsHist 
    sc =calculateParameters();
    if(sc.isFailure())
    {
      mLog << MSG::WARNING << "Calculation of parameters failed!" << endreq;
    }
    mLog << MSG::DEBUG << "Finished calculating parameters" << endreq;

    //writeCalibrationFile() writes the calculated parameters into a calibration fie.
    sc = writeCalibrationFile();
    if(!sc.isSuccess())
    {
      mLog << MSG::FATAL << "Failed to write parameters to disk!" << endreq;
      thereIsAFatal = true; //Not quiting yet to ensure memory is properly deleted
    }

    sc = storeGateRecord();
    if(sc.isFailure())
    {
      mLog <<MSG::FATAL << "Failed to record parameters in StoreGate " << endreq;
      thereIsAFatal = true;
    }

    //delete m_ampHists;
    //delete m_pulserLevels;
    //delete m_adcValues;
    //delete m_adcErrors;

    delete m_peakTimeProf;

    delete [] m_crossTalkFix;
    mLog <<MSG::DEBUG  << "Finished finalize()" << endl;

    if(thereIsAFatal)
      return StatusCode::FAILURE; 

    return StatusCode::SUCCESS;	
  }//end finalize()


  //Collect event info is run every event, gathering amplitutdes and peakting times.
  StatusCode CscCalcSlope::collectEventInfo()
  {
    MsgStream mLog( msgSvc(), name() );

    bool thereIsAnError = false;

    Chrono chrono(m_chronoSvc,"collectEventInfo");
    mLog << MSG::DEBUG <<"Collecting event info for event " << m_eventCnt << endreq;
    //Below might need to be changed depending on how we get data
    const CscRawDataContainer* fullRDO;
    StatusCode sc_read = m_storeGate->retrieve(fullRDO, "CSCRDO"); 
    if (sc_read != StatusCode::SUCCESS)
    {
      mLog << MSG::FATAL << "Could not find event" << endreq;
      return StatusCode::FAILURE;
    }

    //	mLog << MSG::INFO <<"Got raw data " << endreq;
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
        mLog << MSG::VERBOSE << "Pulser level is " << pulserLevel << endreq;
        if( pulserLevel != m_lastPulserLevel)
        {
          mLog <<MSG::INFO << "New pulser level found. (" << pulserLevel <<")." << endreq;

          //m_ampHists->clear();
          map<int,TProfile*>::iterator alreadyExistingProfile = m_ampProfs->find(pulserLevel);

          if(alreadyExistingProfile == m_ampProfs->end())
          {//No previous profile for this amplitude exists

            mLog << MSG::DEBUG << " creating new amplitude profile" << endreq;
            stringstream name, title;
            name << "ampProf_" << pulserLevel;
            title << m_titlePrefix << "Amplitudes For Pulser Level " << pulserLevel << m_titlePostfix;
            m_currentAmpProf = new TProfile(name.str().c_str(), title.str().c_str(), 
                m_maxStripHash+1, 0, m_maxStripHash);
            m_currentAmpProf->GetXaxis()->SetTitle("Channel (Hash Id)");
            m_currentAmpProf->GetYaxis()->SetTitle("Amplitude (ADC value)");
            mLog << MSG::DEBUG << "Adding new amplitude profile" << endreq;
            m_ampProfs->insert(pair<int, TProfile*>( pulserLevel, m_currentAmpProf));
          }
          else
          {
            mLog << MSG::DEBUG << " using existing amplitude profile" << endreq;
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
          //mLog << MSG::INFO<< " converting cluster" << endreq;
          const CscRawData * cluster = (*clusItr); //Note: For a pulser or ped run, the "cluster" 
          //is the size of an entire layer
          int numStrips = cluster->width();
          int samplesPerStrip = (cluster->samples()).size()/numStrips;

          IdContext channelContext = m_cscId->channel_context();	
          /*
             Identifier startId;
             m_cscId->get_id(hashStart, startId, &channelContex);

          //check the chamber layer.
          int chamberLayer = m_cscId->chamberLayer(startId) << endreq;
          if(chamberLayer != m_expectedChamberLayer)
          {
          m_log << MSG::ERROR << "A chamber with a chamber layer of " << chamberLayer << " was found. "
          << m_expectedChamberLayer << " was expected. Something is probably wrong." << endreq;
          continue;
          } */

          for(int stripItr = 0; stripItr <numStrips; stripItr++)
          {
            Identifier stripId =m_cscRdoDecoderTool->channelIdentifier(cluster, stripItr);
            IdentifierHash cscChannelHashId;
            m_cscId->get_channel_hash(stripId, cscChannelHashId);
            int stripHash = cscChannelHashId;
            mLog << MSG::VERBOSE << "The eta of this strip is: " << m_cscId->stationEta(stripId) << endreq;

            int chamberLayer = m_cscId->chamberLayer(stripId);
            if(chamberLayer != m_expectedChamberLayer)
            {
              mLog << MSG::FATAL << "Cluster includes strip in chamber layer "
                << chamberLayer << ". Only " << m_expectedChamberLayer 
                << " is valid." << endreq;
              thereIsAnError = true;
              return StatusCode::FAILURE;
            }

            int currentWireLayer = m_cscId->wireLayer(stripId) - 1 ;
            bool isThisLayerPulsed = (pulsedWireLayer >> currentWireLayer)&0x1;
            if(isThisLayerPulsed)
            {
              /*Usefull for debug, but slows things down a surprising amount
                if(!m_cscId->valid(stripId))
                {
                mLog << MSG::ERROR << stripId.getString() << " is not a valid id!" << endreq;
                }*/

              vector<uint16_t> samples;
              cluster->samples(stripItr,samplesPerStrip,samples); //Retrieve samples for a single strip

              float ped;
              float noise;
              if(m_pedFile)
              {
                ped = m_peds[stripHash];
                noise = m_noises[stripHash];
              }
              else{

                if(StatusCode::SUCCESS != m_cscCoolStrSvc->getParameter(ped,"ped",stripHash))
                {
                  ped = 2054;
                  mLog << (m_ignoreDatabaseError ? MSG::WARNING :  MSG::ERROR) 
                    << "Failed at getting pedestal from COOL for hash " << stripHash << endreq;
                  if(!m_ignoreDatabaseError)
                    return StatusCode::RECOVERABLE;
                  mLog << MSG::WARNING << "Setting to " << ped << endreq;
                }
                else
                  mLog << MSG::VERBOSE << "Got pedestal of " << ped << endreq;
                if(StatusCode::SUCCESS != m_cscCoolStrSvc->getParameter(
                      noise, "noise", stripHash))
                {
                  noise = .001;
                  mLog << (m_ignoreDatabaseError ? MSG::WARNING : MSG::ERROR) << "Failed at getting noise from COOL for hash " << stripHash << endreq;
                  if(!m_ignoreDatabaseError)
                    return StatusCode::FAILURE;
                  mLog << MSG::WARNING << "Setting to " << noise << endreq;
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
                      mLog << MSG::WARNING << "Failed recording bits for strip " << stripHash << endreq;
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
                mLog << MSG::WARNING << "Failed at fitting pulse shape. Debug info: " <<endreq;
                mLog << MSG::WARNING << "stripHash "   << stripHash << endreq;
                mLog << MSG::WARNING << "strip in chamber " << stripItr << endreq;
                mLog << MSG::WARNING
                  << " and detailed id " <<  m_cscId->show_to_string(stripId,&channelContext)
                  << endreq;
                mLog	<< "Pulsed layer " << pulsedWireLayer <<endreq;
                mLog << ", Samples: "  << samples[0] <<", " << samples[1] << ", " 
                  << samples[2] << ", " << samples[3] <<  endreq;
              }
            }//end if (islayerPulsedand and is precision layer)
          }//end strip loop

        }//end cluster loop
      }//end if rod >1
    }//end rod loop


    mLog << MSG::DEBUG << "end collectEventInfo()" << endreq;
    m_eventCnt++;

    if(thereIsAnError)
      return StatusCode::RECOVERABLE;

    return StatusCode::SUCCESS;
  }// end collectEventInfo()


  //Calculate parameters is called during finalize, and calculates the parameter values from the
  //data taken by collectEventData()
  StatusCode CscCalcSlope::calculateParameters()
  {
    MsgStream mLog( msgSvc(), name() );
    Chrono chrono(m_chronoSvc,"calculateParameters");
    StatusCode sc; 
    mLog << MSG::INFO << "Calculating calibration constants." << endreq;

    unsigned int numCalibPoints = m_ampProfs->size();	
    mLog << MSG::INFO << "There are " << numCalibPoints << " pulser levels to evaluate." << endreq;




    IdContext channelContext = m_cscId->channel_context();	

    float chargeMax = 530.88; //in fC

    int crossTalkCnt = 0;

    for(unsigned int stripHash = 0 ;stripHash <= m_maxStripHash; stripHash++)
    {

      if(true)//stripHash < 50 || stripHash%1000 == 0)
      {
        mLog << MSG::INFO << "Analyzing strip with hash " << stripHash << " out of " << m_maxStripHash << endreq; 
        //mLog <<MSG::DEBUG << (float)clock()/((float)CLOCKS_PER_SEC) << " is the time " << endreq;
      }

      //**Now tackle slope calculation


      Identifier id;
      m_cscId->get_id((IdentifierHash)stripHash,id,&channelContext);
      int chamberLayer = m_cscId->chamberLayer(id);
      char orientation = (m_cscId->measuresPhi(id) ? 'Y':'X');

      int wireLayer = m_cscId->wireLayer(id);


      int stationName = m_cscId->stationName(id);
      int stationPhi = m_cscId->stationPhi(id);
      int stationEta = m_cscId->stationEta(id);
      int stripNumber = m_cscId->strip(id);


      //Decide if we're fitting to an edge strip or not, which
      //use different TF1s
      /*
      TF1 * myFunc = NULL;
      if(stripNumber <= 2 || stripNumber >= 191)
        myFunc = edgeFunc;
      else
        myFunc = simpleFunc;
        */


      IdentifierHash chamHash;
      m_cscId->get_module_hash(id,chamHash);

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
      sprintf(calName, "calGraph%d",stripHash);
      sprintf(titleSeed, "Calgraph for eta %d, sector %d, layer %d%c, strip %d",stationEta,(2*stationPhi+50 - stationName),wireLayer,orientation, stripNumber);
      calGraph->SetName(calName);
      string title = m_titlePrefix + titleSeed + m_titlePostfix;
      calGraph->SetTitle(title.c_str());
      calGraph->GetYaxis()->SetTitle("ADC counts");
      calGraph->GetXaxis()->SetTitle("Attenuation (-db)");

      //
      mLog << MSG::DEBUG << " Generating " << title << endreq;   

      bool isGoodStrip = false;

      //Loop over all attenuation levels, filling the calGraph with the amplitudes
      //for this strip 
      mLog << MSG::DEBUG << "Number of ampProfs " << m_ampProfs->size() << endreq;
      if(!m_ampProfs){
        mLog << MSG::FATAL << "m_ampProfs empty!" << endreq;
        return StatusCode::FAILURE;
      }
      int calPointItr = 0;
      map<int, TProfile*>::const_iterator ampProfItr = m_ampProfs->begin();
      map<int, TProfile*>::const_iterator ampProfEnd = m_ampProfs->end();
      for(; ampProfItr != ampProfEnd; ampProfItr++)
      {
        if(!ampProfItr->second){
          mLog << MSG::FATAL << "Failed at accessing ampProf!" << endreq;
          return StatusCode::FAILURE;
        }
        mLog << MSG::DEBUG << "\tLooking for data for pulser level "
           << ampProfItr->first << endreq;

        if(ampProfItr->second->GetBinEntries(stripHash+1))
        {

          mLog << MSG::VERBOSE << "\nHave data for strip " << stripHash<< endreq;

          isGoodStrip = true;

          int pulserLevel = ampProfItr->first; 
          float adcValue = ampProfItr->second->GetBinContent(stripHash+1); 
          float adcError = ampProfItr->second->GetBinError(stripHash+1); 
          if(m_doCrossTalkFix)
          {
            mLog <<MSG::VERBOSE << "\tCrosstalk fix " << m_crossTalkFix[crossTalkCnt] << endreq;
            adcValue /= m_crossTalkFix[crossTalkCnt];
            adcError /= m_crossTalkFix[crossTalkCnt];
          }
          if(adcError != adcError)
            adcError = 0.01;

          float db = m_dbLevels[pulserLevel];
          

          float attenValue =0;
          if(m_doLinPlot)
            attenValue = 300*pow(10,db/20);
          else 
            attenValue = db;

          mLog << MSG::DEBUG << "\tStoring at db of " << db << " with attenValue " << attenValue << " from pulser level of " << pulserLevel << " and adcValue " << adcValue << endreq;


          
          //See if the last two drops were far down enough
          if(!foundMin){
            thisDrop = lastVal - adcValue;
            mLog << MSG::DEBUG << "\tFinding fit min:" 
             << "\tlastVal = " << lastVal
             << ";lastDrop " << lastDrop << "; thisDrop " << thisDrop << endreq;
            if(thisDrop > m_minDeltaAdc && lastDrop > m_minDeltaAdc){
              mLog << MSG::DEBUG << "Found fitMin!" << std::endl;
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
          //calGraph->SetPoint(calPointItr,adcValue,attenValue);
          //calGraph->SetPointError(calPointItr,adcError,0.01);
          calPointItr++;
          //fittedFunction->GetParError(1));	//WARNING: Assumed small charge error
        }//done if(entries >0)

      }//Done ampProfItr loop

      if(!foundMin && isGoodStrip){
        mLog << MSG::WARNING << "Failed to find minium for " << title << endreq; 
      }

      //***Do a simple fit to calGraph***
      //Here we only fit the linear part of the plot. m_fitCutoff can be set by user.			
      if(isGoodStrip)
      {
        mLog << MSG::INFO << "we have a good stripHash at " << stripHash << endreq; 

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
        if(abs(slope) < 0.00001 || slope == -999) //watch out for slope==0 
        {
          mLog << MSG::WARNING <<  "Slope invalid " << endreq;
          continue;
        }

        invertedSlope = 1/slope;

        cerr << "Inserting calgraph in for hash " << stripHash << endl;
        (*m_calGraphs)[stripHash] = calGraph;

        mLog << MSG::DEBUG << "StripHash: " << stripHash << "; slope: " <<slope  
          << "; intercept: " << intercept
          << "; chi^2/ndf: " << chiSquared << "/" << ndf << endreq;
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
      mLog << MSG::DEBUG << "Looping over next strip..." << endreq;
    }//end loop over strips
    mLog << MSG::INFO << "Completed calculating parameters for each strip" << endreq;
    return StatusCode::SUCCESS;
  }//End calculateParameters()

  //writeCalibrationFile() dumps the parameters to disk
  StatusCode CscCalcSlope::writeCalibrationFile()
  {
    MsgStream mLog( msgSvc(), name() );
    Chrono chrono(m_chronoSvc,"writeCalibrationFile");
    if(m_calOutputVersion == "00-00"){
      mLog << MSG::INFO << "Printing output file version 00-00" << endreq;
      return calOutput0();
    }
    else if(m_calOutputVersion == "03-00") {
      mLog << MSG::INFO << "Printing output file version 03-00" << endreq;
      return calOutput3();
    }
    else{
      mLog << "Don't know how to write calibration file version " << m_calOutputVersion << endreq;
      return StatusCode::RECOVERABLE;
    }
  }

  StatusCode CscCalcSlope::calOutput0(){
    MsgStream mLog( msgSvc(), name() );
    //***Take conditions data held in summary histograms and  print to the calibration file***//
    mLog << MSG::INFO << "Parameters calculated, preparing to outputing to file: " << m_outputFileName << endreq;
    ofstream out;
    out.open(m_outputFileName.c_str());
    if(!out.is_open())
    {
      mLog << MSG::FATAL << "Can't open file " << m_outputFileName.c_str() << "for writing" <<  endreq;
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
    mLog << MSG::DEBUG <<  "Begining loop over all " << m_maxStripHash  << " hash ids." << endreq;

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
        mLog << MSG::FATAL << "Peaktimes out of sync with slopes. Quiting write." << endreq;

        return StatusCode::FAILURE;
      }

      //Output hashId
      out << (*slopeItr)->hashId();

      //get id for next few outputs
      Identifier id;
      IdContext channelContext = m_cscId->channel_context();	
      m_cscId->get_id((*slopeItr)->hashId(),id, &channelContext);

      //output chamber #
      IdentifierHash chamberHash;
      Identifier chamberId = m_cscId->elementID(id);
      if(!m_cscId->valid(chamberId))
      {
        mLog << MSG::FATAL << chamberId.getString() << " is not a valid id!" << endreq;
        mLog << MSG::FATAL << "identifier is: " << m_cscId->show_to_string(chamberId) << endreq;
        return StatusCode::FAILURE;
      }

      m_cscId->get_module_hash(id,chamberHash);
      out <<" " << chamberHash;

      //output strip details
      out << " " << m_cscId->show_to_string(id) << " ";

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
    MsgStream mLog( msgSvc(), name() );
    mLog << MSG::INFO << "Recording csc calibration report." << endreq;

    StatusCode sc = StatusCode::SUCCESS;

    bool thereIsAnError = false;

    string histKey = "cscSlopeCalibReport";
    mLog <<MSG::DEBUG << "Recording calibration graphs to TDS with key " 
      << histKey << endreq;

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
      mLog << MSG::ERROR << "Failed to record CscCalibReportSlope to storegate" << endreq;
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
      mLog << MSG::ERROR << "Failed to record results to storegate" << endreq;
      thereIsAnError = true;
      //Since storegate isn't taking ownership, we'll delete it
      delete calibResults;
    }

    if(thereIsAnError)
      return StatusCode::RECOVERABLE;

    return StatusCode::SUCCESS;
  }



  StatusCode CscCalcSlope::calOutput3() {
    MsgStream mLog( msgSvc(), name() );

    ofstream out;
    out.open(m_outputFileName.c_str());
    if(!out.is_open())
    {
      mLog << MSG::ERROR << "Can't open file " << m_outputFileName.c_str() << endreq;
      return StatusCode::RECOVERABLE;
    }
    out << "03-00 <END_HEADER>";

    outputParameter3(*m_slopes, out);
    //outputParameter3(*m_intercepts, out);
    //if(m_peakTimes)
    //  outputParameter3(*m_peakTimes, out);
    out << "\n<END_FILE>";
    out.close();

    mLog << MSG::INFO << "Successfully opened file " << m_outputFileName << endreq;

    return StatusCode::SUCCESS;
  }


  void CscCalcSlope::outputParameter3(const CscCalibResultCollection & results, ofstream & out){
    MsgStream mLog( msgSvc(), name() );

    mLog << MSG::INFO << "Printing out parameter " << results.parName() << endreq;

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
}//end namespace MuonCalib
