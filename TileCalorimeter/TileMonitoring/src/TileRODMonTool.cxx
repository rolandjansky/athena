/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//********************************************************************//
//
// NAME:     TileRODMonTool.cxx
// PACKAGE:  TileMonitoring  
//
// AUTHOR:    Michael Miller (Michael.Earl.Miller@cern.ch)
//	      Luca Fiorini   (Luca.Fiorini@cern.ch)
//
//********************************************************************//

#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"

#include "TileMonitoring/TileRODMonTool.h"
#include "TileCalibBlobObjs/TileCalibUtils.h" 
#include "TileConditions/ITileBadChanTool.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileRecUtils/TileBeamInfoProvider.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TProfile2D.h"

#include <iostream>
#include <iomanip>
#include <sstream>


/*---------------------------------------------------------*/
TileRODMonTool::TileRODMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_tileRODTrig(0)
  , m_OFI(false)
  , m_OFNI(false)
  , m_corrtime(false)
  , m_tileToolEmscale("TileCondToolEmscale")
  , m_beamInfo("TileBeamInfoProvider")
  , m_tileBadChanTool("TileBadChanTool")
  , m_robSvc("ROBDataProviderSvc", name)
  , m_evEref{}
  , m_evTref{}
  , m_nEventsProcessed{}
  , m_old_lumiblock(-1)
  , m_nLumiblocks(3000)
  , m_nEvents4FragmentSize(50)
  , m_rodFragmentSizeSum{}
  , m_lastRodFragmentSize{}
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("TileRawChannelContainer", m_contName = "TileRawChannelFit"); //SG RC Container
  declareProperty("TileRawChannelContainerDSP", m_contNameDSP = "TileRawChannelCnt"); //SG DSP RC Cell Container
  declareProperty("TileRawChannelContainerOF", m_contNameOF = "TileRawChannelOpt2"); //SG OFF-OF2-I RC Cell Container
  declareProperty("TileRawChannelContainerOFNI", m_contNameOFNI = "TileRawChannelFixed"); //SG OFF-OF2-NI
  declareProperty("TileCondToolEmscale", m_tileToolEmscale);
  declareProperty("TileBeamInfoProvider", m_beamInfo);
  declareProperty("TileBadChanTool", m_tileBadChanTool);

  declareProperty("DspComparisonUnit", m_compUnit = 3); //units, default is 3= MeV
  declareProperty("DspComparisonFit", m_useFitRef = false); //reference type
  declareProperty("DspComparisonOF", m_useOFRef = true);
  declareProperty("doOnline", m_isOnline = false); // Switch for online running
  declareProperty("Details", m_details = false); // Switch for online running
  declareProperty("NumberOfLumiblocks", m_nLumiblocks = 3000);
  declareProperty("FillDetailRODFragmentSize", m_fillDetailFragmentSize = true);
  declareProperty("NumberOfEventsToAverageFragmentSize", m_nEvents4FragmentSize = 50);

  m_path = "/Tile/ROD"; //ROOT File directory

}

/*---------------------------------------------------------*/
TileRODMonTool::~TileRODMonTool()
/*---------------------------------------------------------*/
{
}

/*---------------------------------------------------------*/
StatusCode TileRODMonTool:: initialize() {
/*---------------------------------------------------------*/

  ATH_MSG_INFO( "in initialize()" );

  m_tileRobIds.reserve(4 * 16); // partitions * fragments
  
  for (uint32_t robId = 0x510000; robId < 0x510010; ++robId) m_tileRobIds.push_back(robId); // LBA
  for (uint32_t robId = 0x520000; robId < 0x520010; ++robId) m_tileRobIds.push_back(robId); // LBC
  for (uint32_t robId = 0x530000; robId < 0x530010; ++robId) m_tileRobIds.push_back(robId); // EBA
  for (uint32_t robId = 0x540000; robId < 0x540010; ++robId) m_tileRobIds.push_back(robId); // EBC
 
  CHECK( m_tileToolEmscale.retrieve() );
  CHECK( m_beamInfo.retrieve() );
  CHECK( m_robSvc.retrieve() );

  memset(m_nEventsProcessed, 0, sizeof(m_nEventsProcessed));
  memset(m_rodFragmentSizeSum, 0, sizeof(m_rodFragmentSizeSum));
  memset(m_lastRodFragmentSize, 0, sizeof(m_lastRodFragmentSize));

  return TileFatherMonTool::initialize();
}

/*---------------------------------------------------------*/
StatusCode TileRODMonTool::bookHistTrig( int trig )
/*---------------------------------------------------------*/
{

 std::ostringstream sStr;
           
 std::string runNumStr = getRunNumStr();
 std::string dspName, histDir, histName, histTitle; 

 // Taking care of the mapping between vector element and trigger type: if vector empty, element will be 0 and so on
 //m_activeTrigs[ trig ] =  m_TileDspRefEn[0][0][0].size();
 m_activeTrigs[ trig ] = m_tileRODTrig;
 int element = m_activeTrigs[ trig ];
 m_tileRODTrig++; 

  ///get algorithm and number of iterations from bsflags
  const TileRawChannelContainer* RawChannelCntDsp;
  short unsigned int bin[4] = { 0, 0, 0, 0 };
  if (evtStore()->retrieve(RawChannelCntDsp, m_contNameDSP).isSuccess()) {
    uint32_t info = RawChannelCntDsp->get_bsflags() >> 24;
    for (int i = 0; i < 4; i++) {
      int rem;
      rem = info % 2;
      bin[i] = rem;
      info = info >> 1;
    }
 
  ///bin[0] first bit of iterations
  ///bin[1] second bit of iterations
  ///bin[2] algorithm - 0 is OF1 - 1 is OF2
  ///bin[3] samples - 0 is 7 samples - 1 is 9 samples 
  }

  m_OFI = false;
  m_OFNI = false;
 
  std::string alg[2] = {"OF1", "OF2"};
  std::string iter[4] = {"NI", "I1", "I2", "I3"}; 
  std::string units[4] = {" (ADC counts) "," (pC) "," (Cs pC) "," (MeV) "};
  std::string ref[6] = {"E_{fit}", "E_{of}", "t_{fit}", "t_{of}", "Fit-", "OFF-OF2"};
  std::string iterName, algName, config, eName, tName, xName, yName;
 
  algName = alg[bin[2]];
  if (bin[0] == 0 && bin[1] == 0) {
    iterName = iter[0];
    m_OFNI = true;
    m_corrtime = true;
  } else if (bin[0] == 1 && bin[1] == 0) {
    iterName = iter[1];
    m_OFI = true;
    m_corrtime = false;
  } else if (bin[0] == 0 && bin[1] == 1) {
    iterName = iter[2];
    m_OFI = true;
    m_corrtime = false;
  } else if (bin[0] == 1 && bin[1] == 1) {
    iterName = iter[3];
    m_OFI = true;
    m_corrtime = false;
  }

  //LF Check that the Offline NI method really exists. Otherwise set to offline I method.
  static bool first = true;
  bool OFNI_present = evtStore()->contains<TileRawChannelContainer>(m_contNameOFNI);
  bool OFI_present = evtStore()->contains<TileRawChannelContainer>(m_contNameOF);

  if (!OFNI_present && !OFI_present) {
    if (first)
      ATH_MSG_WARNING( "Neither "<< m_contNameOFNI << " nor " << m_contNameOF
                       << " containers are available - something is wrong!" );

    m_OFI = false;
    m_OFNI = false;
  }
  
  if (m_OFNI) {
    if (!OFNI_present) {
      if (first)
        ATH_MSG_INFO( "Offline NI RawChannels "<< m_contNameOFNI << " are not done, switching to Iterative one " << m_contNameOF );

      m_OFI = true;
      m_OFNI = false;
    }
  }

  if (m_OFI) {
    if (!OFI_present) {
      if (first)
        ATH_MSG_INFO( "Offline I RawChannels "<< m_contNameOF << " are not done, switching to Non-Iterative one " << m_contNameOFNI );

      m_OFI = false;
      m_OFNI = true;
    }
  }

  if (first) first = false;

  if (m_useFitRef) {
    sStr << ref[4] << "--DSP-" << algName << "-" << iterName;
    config = sStr.str();
    sStr.str("");
    eName = ref[0];
    tName = ref[2];
  } else if (m_useOFRef) {
    
    if (m_OFI) {
      sStr << ref[5] << "-I" << "--DSP-" << algName << "-" << iterName;
      config = sStr.str();
      sStr.str("");
      eName = ref[1];
      tName = ref[3];
    } else if (m_OFNI) {
      sStr << ref[5] << "-NI" << "--DSP-" << algName << "-" << iterName;
      config = sStr.str();
      sStr.str("");
      eName = ref[1];
      tName = ref[3];
    }
  }
  
  
  const char *partName[4] = { "LBA", "LBC", "EBA", "EBC" };
  
  const char *modmap[32] = {"61-62","63-64","01-02","03-04","05-06","07-08",
                            "09-10","11-12","13-14","15-16","17-18","19-20",
                            "21-22","23-24","25-26","27-28","29-30","31-32",
                            "33-34","35-36","37-38","39-40","41-42","43-44",
                            "45-46","47-48","49-50","51-52","53-54","55-56",
                            "57-58","59-60"};
  
  if (m_details) {
    for (int part = 0; part < NumPart; part++) {
      int i = 0;
      for (int rod = 0; rod < 8; rod++) {
        for (int dsp = 0; dsp < 4; dsp++) {
          
          sStr << m_TrigNames[trig] << "/Details";
          histDir = sStr.str();
          sStr.str("");
          
          sStr << partName[part] << "_MOD_" << modmap[i];
          dspName = sStr.str();
          sStr.str("");
          
          sStr << "TileDspRefEn_" << dspName << "_" << m_TrigNames[trig];
          histName = sStr.str();
          sStr.str("");
          sStr << "Run: " << runNumStr
               << " Config: " << config
               << " Relative difference between E_{dsp} and " << eName << " for " << dspName;
          
          histTitle = sStr.str();
          sStr.str("");
          sStr << "(E_{dsp}-" << eName << ")/" << eName;
          xName = sStr.str();
          sStr.str("");
          
          m_TileDspRefEn[part][rod][dsp].push_back(book1F(histDir, histName, histTitle, 40, -0.2, 0.2));  //Alberto: It was 40,-0.75,0.75
          m_TileDspRefEn[part][rod][dsp][element]->GetXaxis()->SetTitle(xName.c_str());
          
          sStr << "TileDspRefTim_" << dspName << "_" << m_TrigNames[trig];
          histName = sStr.str();
          sStr.str("");
          sStr << "Run: " << runNumStr
               << " Config: " << config
               << " Difference between t_{dsp} and " << tName << " for " << dspName;
          
          histTitle = sStr.str();
          sStr.str("");
          sStr << "t_{dsp}-" << tName << " (ns) ";
          xName = sStr.str();
          sStr.str("");
          
          m_TileDspRefTim[part][rod][dsp].push_back(book1F(histDir, histName, histTitle, 40, -50., 50.)); //Alberto: It was 40,-75,75
          m_TileDspRefTim[part][rod][dsp][element]->GetXaxis()->SetTitle(xName.c_str());
          
          i++;
        }
      }
    }
  }
  
  
  sStr << m_TrigNames[trig] << "/Summary";
  histDir = sStr.str();
  sStr.str("");

  // book per part histograms 
 
  for (int part = 0; part < NumPart; part++) {

    sStr << "TileAvgDspRefEn_" << partName[part] << "_" << m_TrigNames[trig];
    histName = sStr.str();
    sStr.str("");
    sStr << "Run: " << runNumStr
         << " Config: " << config
         << " Relative difference between E_{dsp} and " << eName << " in " << partName[part];
    histTitle = sStr.str();
    sStr.str("");
    sStr << eName << " " << units[m_compUnit];
    xName = sStr.str();
    sStr.str("");
    sStr << "(E_{dsp}-" << eName << ")/" << eName;
    yName = sStr.str();
    sStr.str("");
//       return back to linear y_bins, the gatherer can't handle histograms with non-equal bins (for point1 online)
//       static const double y_bins[] = {-0.205, -0.2, -0.18, -0.16, -0.14, -0.12, -0.1, -0.095, -0.09, -0.085, -0.08, -0.075, -0.07, -0.065, -0.06, -0.055, -0.05, -0.045, -0.04, -0.035, -0.03, -0.025, -0.02, -0.015, -0.01, -0.005, 0., 0.005, 0.01, 0.015, 0.02, 0.025, 0.03, 0.035, 0.04, 0.045, 0.05, 0.055, 0.06, 0.065, 0.07, 0.075, 0.08, 0.085, 0.09, 0.095, 0.1, 0.12, 0.14, 0.16, 0.18, 0.2, 0.205};

//       m_TileAvgDspRefEn[part].push_back(book2F(histDir, histName, histTitle, 75, 300., 15000., 52, y_bins));  //Tibor: non-linear y-bins
        m_TileAvgDspRefEn[part].push_back(book2F(histDir, histName, histTitle, 75, 300., 15000., 82, -0.205, 0.205));  //Tibor: It was 50, 300., 15000., 25, -0.2, 0.2
//        m_TileAvgDspRefEn[part].push_back(book2F(histDir, histName, histTitle, 75, 300., 15000., 25, -0.2, 0.2));  //Alberto: It was 50, 300., 10000., 25, -0.75, 0.75
       m_TileAvgDspRefEn[part][element]->GetXaxis()->SetTitle(xName.c_str()); 
       m_TileAvgDspRefEn[part][element]->GetYaxis()->SetTitle(yName.c_str());

    sStr << "TileAvgDspRefEnPhase_" << partName[part] << "_" << m_TrigNames[trig];
    histName = sStr.str();
    sStr.str("");
    sStr << "Run: " << runNumStr
         << " Config: " << config
         << " Relative difference between E_{dsp} and " << eName << " in " << partName[part];

    histTitle = sStr.str();
    sStr.str("");
    sStr << "T_{dsp} [ns]";
    xName = sStr.str();
    sStr.str("");
    sStr << "(E_{dsp}-" << eName << ")/" << eName;
    yName = sStr.str();
    sStr.str("");

    m_TileAvgDspRefEnPhase[part].push_back( book2F(histDir, histName, histTitle, 51, -25.5, 25.5, 30, -0.35, 0.1)); //Alberto: It was 50, 300., 10000., 25, -0.75, 0.75
    m_TileAvgDspRefEnPhase[part][element]->GetXaxis()->SetTitle(xName.c_str());
    m_TileAvgDspRefEnPhase[part][element]->GetYaxis()->SetTitle(yName.c_str());

    sStr << "TileAvgDspRefTim_" << partName[part] << "_" << m_TrigNames[trig];
    histName = sStr.str();
    sStr.str("");
    sStr << "Run: " << runNumStr
        << " Config: " << config
        << " Average difference between t_{dsp} and " << tName << " in " << partName[part];

    histTitle = sStr.str();
    sStr.str("");
    sStr << eName << " " << units[m_compUnit];
    xName = sStr.str();
    sStr.str("");
    sStr << "t_{dsp}-" << tName << " [ns] ";
    yName = sStr.str();
    sStr.str("");

    m_TileAvgDspRefTim[part].push_back(bookProfile(histDir, histName, histTitle, 75, 300., 15000., -50., 50.)); //Alberto. It was  50, 300., 10000., -75., 75.
    m_TileAvgDspRefTim[part][element]->GetXaxis()->SetTitle(xName.c_str());
    m_TileAvgDspRefTim[part][element]->GetYaxis()->SetTitle(yName.c_str());

    sStr << "TileAvgDspRefTimPhase_" << partName[part] << "_" << m_TrigNames[trig];
    histName = sStr.str();
    sStr.str("");
    sStr << "Run: " << runNumStr
        << " Config: " << config
        << " Average difference between t_{dsp} and " << tName << " in " << partName[part];

    histTitle = sStr.str();
    sStr.str("");
    sStr << "T_{dsp} [ns]";
    xName = sStr.str();
    sStr.str("");
    sStr << "t_{dsp}-" << tName << " [ns] ";
    yName = sStr.str();
    sStr.str("");

    m_TileAvgDspRefTimPhase[part].push_back(bookProfile(histDir, histName, histTitle, 51, -25.5, 25.5, -50., 50.)); //Alberto. It was  50, 300., 10000., -75., 75.

    // Timing monitoring using Tdsp
    sStr << "Tdsp_Profile_" << partName[part] << "_" << m_TrigNames[trig];
    histName = sStr.str();
    sStr.str("");
    sStr << "Run: " << runNumStr
        << " Config: " << config
        << " Average t_{dsp} in " << partName[part];

    histTitle = sStr.str();
    sStr.str("");
    sStr << "Module";
    xName = sStr.str();
    sStr.str("");
    sStr << "Channel";
    yName = sStr.str();
    sStr.str("");
    m_TdspProfile[part].push_back(bookProfile2D(histDir, histName, histTitle, 64, 0.5, 64.5, 48, -0.5, 47.5, -80, 80));
    m_TdspProfile[part][element]->GetXaxis()->SetTitle(xName.c_str());
    m_TdspProfile[part][element]->GetYaxis()->SetTitle(yName.c_str());


    sStr.str("");
    
    sStr << "TileDspRefEnSummary_" << partName[part] << "_" << m_TrigNames[trig];
    histName = sStr.str();
    
    sStr.str("");
    sStr << "Run: " << runNumStr
         << " Config: " << config
         << " Relative difference between E_{dsp} and " << eName << " for " << partName[part];
    histTitle = sStr.str();
    
    sStr.str("");
    sStr << "(E_{dsp}-" << eName << ")/" << eName;
    xName = sStr.str();
    
    m_TileDspRefEnSummary[part].push_back(book1F(histDir, histName, histTitle, 40, -0.2, 0.2));  //Alberto: It was 40,-0.75,0.75
    m_TileDspRefEnSummary[part][element]->GetXaxis()->SetTitle(xName.c_str());
    
    sStr.str("");          
    sStr << "TileDspRefTimSummary_" << partName[part] << "_" << m_TrigNames[trig];
    histName = sStr.str();
    sStr.str("");
    sStr << "Run: " << runNumStr
         << " Config: " << config
         << " Difference between t_{dsp} and " << tName << " for " << partName[part];
    histTitle = sStr.str();
    
    sStr.str("");
    sStr << "t_{dsp}-" << tName << " (ns) ";
    xName = sStr.str();
    sStr.str("");
    
    m_TileDspRefTimSummary[part].push_back(book1F(histDir, histName, histTitle, 40, -50., 50.)); //Alberto: It was 40,-75,75
    m_TileDspRefTimSummary[part][element]->GetXaxis()->SetTitle(xName.c_str());
    
  } //end book per part
  
  
  
    //book summary plots 
  sStr << "TileDspRefEn_Summary" << "_" << m_TrigNames[trig];
  histName = sStr.str();
  sStr.str("");
  sStr << "Run: " << runNumStr
       << " Config: " << config
       << " Mean (E_{dsp}-" << eName << ")/" << eName << " with rms error per DSP ";

  histTitle = sStr.str();
  sStr.str("");
  sStr << "Mean (E_{dsp}-" << eName << ")/" << eName;
  yName = sStr.str();
  sStr.str("");

  m_TileDspRefSummary[0].push_back(bookProfile(histDir, histName, histTitle, 128, 0.5, 128.5));
  m_TileDspRefSummary[0][element]->GetXaxis()->SetTitle("DSP number");
  m_TileDspRefSummary[0][element]->GetYaxis()->SetTitle(yName.c_str());
  m_TileDspRefSummary[0][element]->SetMarkerStyle(20);

  sStr << "TileDspRefTim_Summary" << "_" << m_TrigNames[trig];
  histName = sStr.str();
  sStr.str("");
  sStr << "Run: " << runNumStr
       << " Config: " << config
       << " Mean t_{dsp}-" << tName << " with rms error per DSP ";

  histTitle = sStr.str();
  sStr.str("");
  sStr << "Mean t_{dsp}-" << tName << " (ns) ";
  yName = sStr.str();
  sStr.str("");

  m_TileDspRefSummary[1].push_back(bookProfile(histDir, histName, histTitle, 128, 0.5, 128.5));
  m_TileDspRefSummary[1][element]->GetXaxis()->SetTitle("DSP number");
  m_TileDspRefSummary[1][element]->GetYaxis()->SetTitle(yName.c_str());
  m_TileDspRefSummary[1][element]->SetMarkerStyle(20); 



  for (int part = 0; part < NumPart; part++) {
    for (int dsp = 0; dsp < 32; dsp++) {

      std::string pName;
      sStr << partName[part] << modmap[dsp];
      pName = sStr.str();
      sStr.str("");

      int bin = 32 * (part) + (dsp + 1);

      m_TileDspRefSummary[0][element]->GetXaxis()->SetBinLabel(bin, pName.c_str());
      m_TileDspRefSummary[0][element]->GetXaxis()->LabelsOption("v");
      m_TileDspRefSummary[0][element]->GetXaxis()->SetLabelSize(0.02);

      m_TileDspRefSummary[1][element]->GetXaxis()->SetBinLabel(bin, pName.c_str());
      m_TileDspRefSummary[1][element]->GetXaxis()->LabelsOption("v");
      m_TileDspRefSummary[1][element]->GetXaxis()->SetLabelSize(0.02);

    }
  }


  m_tileRodFragmentSize.push_back( bookProfile2D(m_TrigNames[trig]
                                                 , "tileRodFragmentSize_" + m_TrigNames[trig]
                                                 , "Run " + runNumStr + " Trigger " + m_TrigNames[trig] +
                                                 ": Tile ROD fragment size (word) (entries = events)"
                                                 , 16, -0.5, 15.5, 4, 1, 5, 0, 2.e6)) ;
  
  m_tileRodFragmentSize[element]->GetXaxis()->SetTitle("Fragment Number");
  m_tileRodFragmentSize[element]->GetYaxis()->SetTitle("Partition Number");
  m_tileRodFragmentSize[element]->GetYaxis()->SetBinLabel(1, "LBA");
  m_tileRodFragmentSize[element]->GetYaxis()->SetBinLabel(2, "LBC");
  m_tileRodFragmentSize[element]->GetYaxis()->SetBinLabel(3, "EBA");
  m_tileRodFragmentSize[element]->GetYaxis()->SetBinLabel(4, "EBC");
  
  if (m_isOnline) {
    m_tileRodFragmentSizeLB.push_back( bookProfile(m_TrigNames[trig]
                                                   ,"tileRodFragmentSizeLB_" + m_TrigNames[trig]
                                                   , "Run " + runNumStr + " Trigger " + m_TrigNames[trig] +
                                                   ": Tile ROD fragment size (word) per LumiBlock"
                                                   , 100, -99.5, 0.5) );
    
    m_tileRodFragmentSizeLB[element]->GetXaxis()->SetTitle("Last LumiBlocks");
  } else {
    m_tileRodFragmentSizeLB.push_back( bookProfile(m_TrigNames[trig]
                                                   ,"tileRodFragmentSizeLB_" + m_TrigNames[trig]
                                                   , "Run " + runNumStr + " Trigger " + m_TrigNames[trig] +
                                                   ": Tile ROD fragment size (word) per LumiBlock"
                                                   , m_nLumiblocks, -0.5, m_nLumiblocks - 0.5) );
    
    m_tileRodFragmentSizeLB[element]->GetXaxis()->SetTitle("LumiBlocks");
  }
 

  if (m_fillDetailFragmentSize) {
    std::vector<std::string> partitions = {"AUX", "LBA", "LBC", "EBA", "EBC"};
    for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
      for (unsigned int fragment = 0; fragment < 16; ++fragment) {
        m_tileRodFragmentSize1D[ros][fragment].push_back( book1F(m_TrigNames[trig]
                                                                 , "tileRodFragmentSize_" + partitions[ros] + 
                                                                 "_Fragment" + std::to_string(fragment) + "_" + m_TrigNames[trig]
                                                                 , "Run " + runNumStr + " Trigger " + m_TrigNames[trig] + 
                                                                 " Partition " + partitions[ros] + " Fragment " + std::to_string(fragment) +
                                                                 ": Tile ROD fragment size (word) averaged over " + std::to_string(m_nEvents4FragmentSize) + 
                                                                 " events;Tile ROD fragment size [word]"
                                                                 , 1000, -0.5, 999.5)) ;
        
      }
    }
  }

  return StatusCode::SUCCESS;

}


/// BookHistogram method is called at every event block, lumi block and run.
/// At the moment we need to book only every run
/// It calls bookHistTrig for the 'AnyTrig' trigger type
/// All concrete trigger type histograms will only be booked when needed
/*---------------------------------------------------------*/
StatusCode TileRODMonTool::bookHistograms() {
/*---------------------------------------------------------*/

  //if(m_runType!=Phys){return StatusCode::SUCCESS;} 
  ATH_MSG_INFO( "in bookHistograms()" );

  cleanHistVec(); //clean vectors before booking

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TileRODMonTool::cleanHistVec()
/*---------------------------------------------------------*/
{
  m_tileRODTrig = 0; //Reset trigger index

  for (int part = 0; part < NumPart; part++) {

    if (m_details) {
      for (int rod = 0; rod < 8; rod++) {
        for (int dsp = 0; dsp < 4; dsp++) {
          m_TileDspRefEn[part][rod][dsp].clear();
          m_TileDspRefTim[part][rod][dsp].clear();
        }
      }
    }

    m_TileDspRefEnSummary[part].clear();
    m_TileDspRefTimSummary[part].clear();

    m_TileAvgDspRefEn[part].clear();
    m_TileAvgDspRefTim[part].clear();
    m_TileAvgDspRefEnPhase[part].clear();
    m_TileAvgDspRefTimPhase[part].clear();
    m_TdspProfile[part].clear();

  }

  m_TileDspRefSummary[0].clear();
  m_TileDspRefSummary[1].clear();

  m_tileRodFragmentSize.clear();

  if (m_fillDetailFragmentSize) {
    for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
      for (unsigned int fragment = 0; fragment < 16; ++fragment) {
        m_tileRodFragmentSize1D[ros][fragment].clear();
      }
    }
  }

  for (int t = 0; t < NTrigHisto; t++) {
    m_activeTrigs[t] = -1;
  }

  memset(m_nEventsProcessed, 0, sizeof(m_nEventsProcessed));
  memset(m_rodFragmentSizeSum, 0, sizeof(m_rodFragmentSizeSum));
  memset(m_lastRodFragmentSize, 0, sizeof(m_lastRodFragmentSize));
}


/*---------------------------------------------------------*/
StatusCode TileRODMonTool::fillHistograms()
/*---------------------------------------------------------*/

{
  fillEvtInfo();
  // get Lvl1 Trigger word
  uint32_t lvl1info = getL1info();
  get_eventTrigs(lvl1info); //fill  m_eventTrigs; it always contains at least one element: AnyTrig.

  //check if trigger type is new and therefore some histograms must be booked
  for (unsigned int i=0; i< m_eventTrigs.size();i++) {
    if ( vecIndx(i) < 0 ) {
      // book histograms
      if ( bookHistTrig(m_eventTrigs[i]).isFailure() ) {
        ATH_MSG_WARNING( "Error booking ROD histograms for Trigger " << m_TrigNames[m_eventTrigs[i]] );
      }	
    }
  }

  ATH_MSG_DEBUG( "in fillHistograms()" );

  memset(m_evEref, 0, sizeof(m_evEref));
  memset(m_evTref, 0, sizeof(m_evTref));

  // Compute Collision Candidates variable to fill Timing Monitoring plot
  //LF: NOT NEEDED, already done once. collcand();  // m_is_collision=true if TMBTSc-TMBSa < 7.5ns

  

///////////////////////////////
//  Get RawChannel Ref data  //
///////////////////////////////

 const TileRawChannelContainer* RawChannelCntRef;

  StatusCode sc;
  if (m_useFitRef) {
    sc = evtStore()->retrieve(RawChannelCntRef, m_contName);
  } else if (m_useOFRef && m_OFI) {
    sc = evtStore()->retrieve(RawChannelCntRef, m_contNameOF);
  } else if (m_useOFRef && m_OFNI) {
    sc = evtStore()->retrieve(RawChannelCntRef, m_contNameOFNI);
  } else {
    static int first = true;
    if (first) {
      first = false;
      ATH_MSG_WARNING( "Inconsistent combination of flags / processing options. Exiting from fillHistograms()" );
      ATH_MSG_WARNING( "m_useFitRef=" << m_useFitRef
                       << "\t m_useOFRef=" << m_useOFRef
                       << "\t m_OFI=" << m_OFI
                       << "\t m_OFNI=" << m_OFNI );
    }

    sc.ignore(); // need to check StatusCode to avoid FATAL error
    return StatusCode::SUCCESS;
  }

  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Can't retrieve reference RawChannel from TES" );
    return StatusCode::SUCCESS;
  }

  TileRawChannelUnit::UNIT RChUnitRef = RawChannelCntRef->get_unit(); 
  
  TileRawChannelContainer::const_iterator collItrRef=RawChannelCntRef->begin();
  TileRawChannelContainer::const_iterator lastCollRef=RawChannelCntRef->end();
  
  for (; collItrRef != lastCollRef; ++collItrRef) {
    
    TileRawChannelCollection::const_iterator chItrRef = (*collItrRef)->begin();
    TileRawChannelCollection::const_iterator lastChRef = (*collItrRef)->end();

    if (chItrRef != lastChRef) {

      HWIdentifier adc_id = (*chItrRef)->adc_HWID();
      int ros = m_tileHWID->ros(adc_id);
      int drawer = m_tileHWID->drawer(adc_id);

      for (; chItrRef != lastChRef; ++chItrRef) {

        const TileRawChannel * rch = (*chItrRef);

        adc_id = rch->adc_HWID();
        unsigned int chan = m_tileHWID->channel(adc_id);
        if (isDisconnected(ros, drawer, chan)) {
          continue;
        }  //leave if channel is disconnected

        //Alberto: Leave if it is a known BadChannel
        if ((m_tileBadChanTool->getAdcStatus(adc_id)).isBad()) {
          continue;
        }

        if (ros > 2) { //check if channel is mbts - no mbts in LB

          int index, pmt;
          /*Identifier cell_id =*/rch->cell_ID_index(index, pmt);
          if (index < -1) {
            continue;
          } // mbts - leave the loop

        }

        int gain = m_tileHWID->adc(adc_id);
        unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

        double ampref = rch->amplitude();
        if (RChUnitRef != m_compUnit) {
          ampref = m_tileToolEmscale->channelCalib(drawerIdx, chan, gain, ampref, RChUnitRef, (TileRawChannelUnit::UNIT) m_compUnit);
        }

        if (ampref > 300.) {

          double timeref = (m_corrtime ? rch->time() : rch->uncorrTime()); //if running DSP w/ iterations, no corrections
          m_evEref[ros - 1][drawer][chan] = ampref;
          m_evTref[ros - 1][drawer][chan] = timeref;

        }

      } // loop over channels

    }

  }


////////////////////////////
//Get RawChannel DSP data //
////////////////////////////

  //for getting rod and dsp number from drawer number -  probably there is a better way to do this
  // array index+1 is drawer number - first number is rod number - second number is dsp number
  short int dspmap[64] = {13,14,13,14,21,22,21,22,23,24,23,24,31,32,31,32,33,34,33,34,41,42,41,42,43,44,43,44,51,52,51,52,53,54,53,54,61,62,61,62,63,64,63,64,71,72,71,72,73,74,73,74,81,82,81,82,83,84,83,84,11,12,11,12};

  const TileRawChannelContainer* RawChannelCntDsp;
  //Alberto

  const TileDQstatus* theDQstatus = 0;
  if (m_beamInfo) theDQstatus = m_beamInfo->getDQstatus();

  if (evtStore()->retrieve(RawChannelCntDsp, m_contNameDSP).isFailure()) {
    ATH_MSG_WARNING("Can't retrieve DSP RawChannel from TES");
    return StatusCode::SUCCESS;
  }

  TileRawChannelUnit::UNIT RChUnitDsp = RawChannelCntDsp->get_unit(); 

  TileRawChannelContainer::const_iterator dspcollItr=RawChannelCntDsp->begin();
  TileRawChannelContainer::const_iterator dsplastColl=RawChannelCntDsp->end();
  
  for (; dspcollItr != dsplastColl; ++dspcollItr) {

    TileRawChannelCollection::const_iterator dspchItr = (*dspcollItr)->begin();
    TileRawChannelCollection::const_iterator dsplastCh = (*dspcollItr)->end();

    if (dspchItr != dsplastCh) {

      HWIdentifier adc_id = (*dspchItr)->adc_HWID();
      int ros = m_tileHWID->ros(adc_id);
      int drawer = m_tileHWID->drawer(adc_id);

      int rod = dspmap[drawer] / 10;
      int dsp = dspmap[drawer] % 10;

      for (; dspchItr != dsplastCh; ++dspchItr) {

        const TileRawChannel * rch = (*dspchItr);

        adc_id = rch->adc_HWID();
        unsigned int chan = m_tileHWID->channel(adc_id);
        int gain = m_tileHWID->adc(adc_id);
        unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

        bool isgood(true);
        if (theDQstatus) {
          isgood = theDQstatus->isAdcDQgood(ros, drawer, chan, gain);
        }

        if ((m_evEref[ros - 1][drawer][chan] > 300.) && isgood) {

          double edsp = rch->amplitude();
          if (RChUnitDsp != m_compUnit) {
            edsp = m_tileToolEmscale->channelCalib(drawerIdx, chan, gain, edsp, RChUnitDsp, (TileRawChannelUnit::UNIT) m_compUnit);
          }

          double tdsp = rch->time();

          double eref = m_evEref[ros - 1][drawer][chan];
          double tref = m_evTref[ros - 1][drawer][chan];

          for (unsigned int element = 0; element < m_eventTrigs.size(); element++) {
      
            m_TileDspRefEnSummary[ros - 1][vecIndx(element)]->Fill(((edsp - eref) / eref), 1.0);
            if (m_details) m_TileDspRefEn[ros - 1][rod - 1][dsp - 1][vecIndx(element)]->Fill(((edsp - eref) / eref), 1.0);

            m_TileAvgDspRefEn[ros - 1][vecIndx(element)]->Fill(eref, ((edsp - eref) / eref));
            m_TileAvgDspRefEnPhase[ros - 1][vecIndx(element)]->Fill(tdsp, ((edsp - eref) / eref));
            m_TileDspRefSummary[0][vecIndx(element)]->Fill((ros - 1) * 32 + (rod - 1) * 4 + dsp, (edsp - eref) / eref);

            if (tref > -65. && tref < 65.) {	// DSP can reconstruct time only to +- 65 ns
            // don't compare if offline time is out of range
              m_TileDspRefTimSummary[ros - 1][vecIndx(element)]->Fill((tdsp - tref), 1.0);
              if (m_details) m_TileDspRefTim[ros - 1][rod - 1][dsp - 1][vecIndx(element)]->Fill((tdsp - tref), 1.0);

              m_TileAvgDspRefTim[ros - 1][vecIndx(element)]->Fill(eref, (tdsp - tref));
              m_TileAvgDspRefTimPhase[ros - 1][vecIndx(element)]->Fill(tdsp, (tdsp - tref));
              m_TileDspRefSummary[1][vecIndx(element)]->Fill((ros - 1) * 32 + (rod - 1) * 4 + dsp, tdsp - tref);
            }

            if (m_is_collision == true)
              m_TdspProfile[ros - 1][vecIndx(element)]->Fill(drawer + 1, chan, tdsp, 1);

          }

        } // if comparison exists
      } //end for on chans     
    }
  }


    
  int allTileRodSize(0);
  

  for (unsigned int trigger : m_eventTrigs) ++m_nEventsProcessed[trigger];

  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> robFragments;
  m_robSvc->getROBData(m_tileRobIds, robFragments);
  
  for (const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* robFragment : robFragments) {
    uint32_t rodSourceId = robFragment->rod_source_id();
    unsigned int ros = (rodSourceId & 0x0F0000) >> 16;
    unsigned int fragment = rodSourceId & 0x00000F;
    allTileRodSize += robFragment->rod_fragment_size_word();
    
    for (unsigned int element = 0; element < m_eventTrigs.size(); ++element) {
      int trigger = m_eventTrigs[element];

      m_tileRodFragmentSize[vecIndx(element)]->Fill(fragment, ros, robFragment->rod_fragment_size_word());
      if (m_fillDetailFragmentSize) {

        m_rodFragmentSizeSum[ros][fragment][m_eventTrigs[element]] += robFragment->rod_fragment_size_word();
        if (m_nEventsProcessed[trigger] > m_nEvents4FragmentSize) {

          m_rodFragmentSizeSum[ros][fragment][trigger] -= m_lastRodFragmentSize[ros][fragment][trigger];
          float averageRodFragmentSize = m_rodFragmentSizeSum[ros][fragment][trigger] / m_nEvents4FragmentSize;
          m_tileRodFragmentSize1D[ros][fragment][vecIndx(element)]->Fill(averageRodFragmentSize);
        }

        m_lastRodFragmentSize[ros][fragment][trigger] = robFragment->rod_fragment_size_word();
      }
    }
  }
  
  
  int32_t current_lumiblock = getLumiBlock();
  if(m_old_lumiblock == -1) {
    m_old_lumiblock = current_lumiblock;
  }
  
  if(m_isOnline && (m_old_lumiblock < current_lumiblock)) {// move bins
    for (unsigned int element = 0; element < m_eventTrigs.size(); ++element) {
      ShiftTprofile(m_tileRodFragmentSizeLB[vecIndx(element)], current_lumiblock - m_old_lumiblock);    
    }
    m_old_lumiblock = current_lumiblock;
  }

  
  for (unsigned int element = 0; element < m_eventTrigs.size(); ++element) {
    int trigger = m_eventTrigs[element];
    int nEvents = m_nEventsProcessed[trigger];
    m_tileRodFragmentSize[vecIndx(element)]->SetEntries(nEvents);
    
    if (m_isOnline) m_tileRodFragmentSizeLB[vecIndx(element)]->Fill(0.0, allTileRodSize);
    else m_tileRodFragmentSizeLB[vecIndx(element)]->Fill(current_lumiblock, allTileRodSize);

  }
  
  

  return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/
StatusCode TileRODMonTool::procHistograms()
/*---------------------------------------------------------*/
{
  if (!endOfRunFlag() || (m_isOnline && endOfRunFlag())) {
    return StatusCode::SUCCESS;
  }

  if (endOfRunFlag()) {
    ATH_MSG_INFO( "in procHistograms()" );
  }

  int NumXBin, NumYBin;
  for (int part = 0; part < NumPart; part++)
    for (unsigned int element = 0; element < m_TileAvgDspRefEn[part].size(); element++) {
      NumXBin = m_TileAvgDspRefEn[part][element]->GetXaxis()->GetNbins();
      NumYBin = m_TileAvgDspRefEn[part][element]->GetYaxis()->GetNbins();
      for (int xbin = 1; xbin <= NumXBin; xbin++) {
        m_TileAvgDspRefEn[part][element]->SetBinContent(xbin, 1, m_TileAvgDspRefEn[part][element]->GetBinContent(xbin, 0) + m_TileAvgDspRefEn[part][element]->GetBinContent(xbin, 1));
        m_TileAvgDspRefEn[part][element]->SetBinContent(xbin, NumYBin, m_TileAvgDspRefEn[part][element]->GetBinContent(xbin, NumYBin) + m_TileAvgDspRefEn[part][element]->GetBinContent(xbin, NumYBin + 1));
      }
    }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileRODMonTool::checkHists(bool /* fromFinalize */)
/*---------------------------------------------------------*/
{
  ATH_MSG_INFO( "in checkHists()" );
  // put content of the underflow/overflow bins to the first/last bins
  return StatusCode::SUCCESS;
}





