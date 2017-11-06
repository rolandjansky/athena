/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     LArCoverage.cxx
// PACKAGE:  LArMonTools
//
// AUTHOR:   Jessica Leveque
//
// Class for monitoring : Detector cells in the readout
//                        Masked Channels from database
//                        Availability of calibration constants 
//                        Bad calibration at FEB level
// ********************************************************************

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IToolSvc.h"

#include "Identifier/IdentifierHash.h"

#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <functional>
#include <map>
#include <utility>

#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile_LW.h"
#include "TMath.h"
#include "xAODEventInfo/EventInfo.h"


#include "LArMonTools/LArCoverage.h"
#include "LArMonTools/LArCoverageFCALBins.h"

using namespace std;

/*---------------------------------------------------------*/
LArCoverage::LArCoverage(const std::string& type, 
			 const std::string& name,
			 const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent), 
    m_larCablingService("LArCablingService"),
    m_badChannelMask("BadLArRawChannelMask"),
    m_hCoverageEMBA(),
    m_hCoverageEMBC(),
    m_hCoverageEMECA(),
    m_hCoverageEMECC(),
    m_hCoverageHECA(),
    m_hCoverageHECC(),
    m_hCoverageFCALA(),
    m_hCoverageFCALC(),
    m_hCoverageHWEMBA(NULL),
    m_hCoverageHWEMBC(NULL),
    m_hCoverageHWEMECA(NULL),
    m_hCoverageHWEMECC(NULL),
    m_hCoverageHWHECA(NULL),
    m_hCoverageHWHECC(NULL),
    m_hCoverageHWFCALA(NULL),
    m_hCoverageHWFCALC(NULL),
    m_hCaloNoiseToolEM(),
    m_hCaloNoiseToolHEC(),
    m_hCaloNoiseToolFCAL()
{
  declareProperty("LArDigitContainerKey",m_LArDigitContainerKey = "FREE");
  declareProperty("LArRawChannelKey",m_channelKey="LArRawChannels");
  declareProperty("LArBadChannelMask",m_badChannelMask);
  declareProperty("LArBadChannelTool",m_badChannelTool);
  declareProperty("LArCaloNoiseTool",m_caloNoiseTool);
  declareProperty("Nevents",m_nevents = 50);
  declareProperty("Nsigma",m_nsigma = 3);

  m_eventsCounter = 0;
  m_noisycells.clear();
  m_FCALBins = 0;

  m_LArOnlineIDHelper	= NULL;
  m_LArEM_IDHelper	= NULL;
  m_LArFCAL_IDHelper	= NULL;
  m_LArHEC_IDHelper	= NULL;
  m_caloIdMgr		= NULL;
  m_CaloDetDescrMgr	= NULL;
  m_FCALBins		= NULL;
  m_hBadChannelsBarrelA = NULL;
  m_hBadChannelsBarrelC = NULL;
  m_hBadChannelsEndcapA = NULL;
  m_hBadChannelsEndcapC = NULL;
  m_strHelper		= NULL;
  m_rootStore		= NULL;
 }

/*---------------------------------------------------------*/
LArCoverage::~LArCoverage()
{
  if ( m_FCALBins ) {
    delete m_FCALBins;
    m_FCALBins = 0;
  }
}

/*---------------------------------------------------------*/
StatusCode 
LArCoverage::initialize()
{
  ATH_MSG_INFO( "Initialize LArCoverage" );
  StatusCode sc;

  sc = detStore()->retrieve(m_LArOnlineIDHelper, "LArOnlineID");
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Could not get LArOnlineIDHelper" );
    return sc;
  }
  
  // Retrieve ID helpers
  sc =  detStore()->retrieve( m_caloIdMgr );
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Could not get CaloIdMgr" );
    return sc;
  }
  m_LArEM_IDHelper   = m_caloIdMgr->getEM_ID();
  m_LArHEC_IDHelper  = m_caloIdMgr->getHEC_ID();
  m_LArFCAL_IDHelper = m_caloIdMgr->getFCAL_ID();
   
  // CaloDetDescrMgr gives "detector description", including real positions of cells
  sc = detStore()->retrieve(m_CaloDetDescrMgr);
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Could not get CaloDetDescrMgr ");
    return sc;
  }

  // Get BadChannelTool
  sc=m_badChannelTool.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not retrieve LArBadChannelTool " << m_badChannelTool );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "LArBadChannelTool" << m_badChannelTool << " retrieved" );
  }

  // Get bad-channel mask
  sc=m_badChannelMask.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not retrieve BadChannelMask" << m_badChannelMask);
    return StatusCode::FAILURE;
  }
   
  // Get LAr Cabling Service
  sc=m_larCablingService.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not retrieve LArCablingService" );
    return StatusCode::FAILURE;
  }
   
  // LArOnlineIDStrHelper
  m_strHelper = new  LArOnlineIDStrHelper(m_LArOnlineIDHelper);
  m_strHelper->setDefaultNameType(LArOnlineIDStrHelper::LARONLINEID);

  // Get CaloNoiseTool
  if ( m_caloNoiseTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << m_caloNoiseTool );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "Retrieved tool " << m_caloNoiseTool );
  }

  // End Initialize
  ManagedMonitorToolBase::initialize().ignore();
  ATH_MSG_DEBUG( "Successful Initialize LArCoverage " );
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode 
LArCoverage::bookHistograms()
{
  ATH_MSG_DEBUG( "in bookHists()" );

  //  if(isNewRun ){// Commented by B.Trocme to comply with new ManagedMonitorToolBase

  const xAOD::EventInfo* thisEventInfo;
    uint32_t lb1 = 0;
    if ((evtStore()->retrieve(thisEventInfo))!=StatusCode::SUCCESS)
      ATH_MSG_WARNING( "No EventInfo object found! Can't read run number!" );
    else{
      lb1 = thisEventInfo->lumiBlock();
    }
  
    // Create top folder for histos
    MonGroup generalGroup( this, "/LAr/Coverage", run, ATTRIB_MANAGED );
    
    //
    // Book Maps vs eta/phi - One map per sampling
    //
    MonGroup CovGroupShift( this, "/LAr/Coverage/perPartition", run, ATTRIB_MANAGED, "", "lowerLB" );

    //
    // Coverage EM Barrel - Store Granularity
    //

    const int size = 4; 
    int nbinsphiEMBarrel[] = {64,256,256,256}; 
    int nbinsetaEMBarrel[] = {61,450,57,27};

    float** etaEMBarrelA=0; 
    etaEMBarrelA = new float*[size];
    for(int it=0;it<4;it++) 
      etaEMBarrelA[it]= new float[nbinsetaEMBarrel[it]+1];

    float** etaEMBarrelC=0; 
    etaEMBarrelC = new float*[size];
    for(int it=0;it<4;it++) 
      etaEMBarrelC[it]= new float[nbinsetaEMBarrel[it]+1];

    float** phiEMBarrel=0; 
    phiEMBarrel = new float*[size];
    for(int it=0;it<4;it++) 
      phiEMBarrel[it]= new float[nbinsphiEMBarrel[it]+1];

    // Sampling 0 
    for(int ibin=0;ibin<=64;ibin++) phiEMBarrel[0][ibin] = -TMath::Pi()+ ibin*2*TMath::Pi()/64;
    for(int ibin=0;ibin<=61;ibin++) etaEMBarrelA[0][ibin] = (ibin)*0.025; 
    for(int ibin=0;ibin<=61;ibin++) etaEMBarrelC[0][ibin] = -(ibin)*0.025; 
    sort(etaEMBarrelC[0],etaEMBarrelC[0]+nbinsetaEMBarrel[0]+1);
    // Sampling 1 
    // First strip : twice the size in eta 
    // Last 3 eta bins are 4 times smaller in phi
    for(int ibin=0;ibin<=256;ibin++)  phiEMBarrel[1][ibin] = -TMath::Pi()+ibin*2*TMath::Pi()/256;
    etaEMBarrelA[1][0] = 0; etaEMBarrelC[1][0] = 0;
    for(int ibin=1;ibin<=446;ibin++) etaEMBarrelA[1][ibin] = 0.025/4+(ibin-1)*0.025/8; 
    for(int ibin=447;ibin<=450;ibin++) etaEMBarrelA[1][ibin] = 1.4+(ibin-447)*0.025; 
    for(int ibin=1;ibin<=446;ibin++) etaEMBarrelC[1][ibin] = -(0.025/4+(ibin-1)*0.025/8);
    for(int ibin=447;ibin<=450;ibin++) etaEMBarrelC[1][ibin] = -(1.4+(ibin-447)*0.025);  
    sort(etaEMBarrelC[1],etaEMBarrelC[1]+nbinsetaEMBarrel[1]+1);
    // Sampling 2
    // 57th cell : three time bigger in eta
    for(int ibin=0;ibin<=256;ibin++) phiEMBarrel[2][ibin] = -TMath::Pi()+ibin*2*TMath::Pi()/256;
    for(int ibin=0;ibin<=56;ibin++)  etaEMBarrelA[2][ibin] = ibin*0.025; 
    for(int ibin=0;ibin<=56;ibin++)  etaEMBarrelC[2][ibin] = -(ibin*0.025); 
    etaEMBarrelA[2][57] = 1.475; etaEMBarrelC[2][57] = -1.475; 
    sort(etaEMBarrelC[2],etaEMBarrelC[2]+nbinsetaEMBarrel[2]+1);
    // Sampling 3
    for(int ibin=0;ibin<=256;ibin++) phiEMBarrel[3][ibin] = -TMath::Pi()+ibin*2*TMath::Pi()/256;
    for(int ibin=0;ibin<=27;ibin++)  etaEMBarrelA[3][ibin] = ibin*0.050; 
    for(int ibin=0;ibin<=27;ibin++)  etaEMBarrelC[3][ibin] = -(ibin*0.050); 
    sort(etaEMBarrelC[3],etaEMBarrelC[3]+nbinsetaEMBarrel[3]+1);

    // Instantiate histograms
    for(int iS=0; iS<4; iS++){
      m_hCoverageEMBA[iS] = TH2I_LW::create(Form("CoverSampling%iEMBA",iS),
					    Form("Coverage - Sampling %i - EMB A - LB %4d",iS,lb1),
				     nbinsetaEMBarrel[iS],etaEMBarrelA[iS],nbinsphiEMBarrel[iS],phiEMBarrel[iS]);
      SetPartCoverageStyle(m_hCoverageEMBA[iS]);
      CovGroupShift.regHist(m_hCoverageEMBA[iS]).ignore();

      m_hCoverageEMBC[iS] = TH2I_LW::create(Form("CoverSampling%iEMBC",iS),
				     Form("Coverage - Sampling %i - EMB C - LB %4d",iS,lb1),
				     nbinsetaEMBarrel[iS],etaEMBarrelC[iS],nbinsphiEMBarrel[iS],phiEMBarrel[iS]);
      SetPartCoverageStyle(m_hCoverageEMBC[iS]);
      CovGroupShift.regHist(m_hCoverageEMBC[iS]).ignore();
    }

    // Delete pointers
    for(int it=0;it<4;it++)
      delete [] etaEMBarrelA[it];
    delete [] etaEMBarrelA;
    for(int it=0;it<4;it++)
      delete [] etaEMBarrelC[it];
    delete [] etaEMBarrelC;
    for(int it=0;it<4;it++)
      delete [] phiEMBarrel[it];
    delete [] phiEMBarrel;
    
    //
    // Coverage EM Endcap - Store cells granularity
    //

    int nbinsphiEMEndcap[] = {64,64,256,256}; 
    int nbinsetaEMEndcap[] = {12,223,51,20};

    float** etaEMEndcapA=0; 
    etaEMEndcapA = new float*[size];
    for(int it=0;it<4;it++) 
      etaEMEndcapA[it]= new float[nbinsetaEMEndcap[it]+1];

    float** etaEMEndcapC=0; 
    etaEMEndcapC = new float*[size];
    for(int it=0;it<4;it++) 
      etaEMEndcapC[it]= new float[nbinsetaEMEndcap[it]+1];

    float** phiEMEndcap=0; 
    phiEMEndcap = new float*[size];
    for(int it=0;it<4;it++) 
      phiEMEndcap[it]= new float[nbinsphiEMEndcap[it]+1];

    // Sampling 0 
    for(int ibin=0;ibin<=64;ibin++) phiEMEndcap[0][ibin] = -TMath::Pi()+ ibin*(2*M_PI/64);
    for(int ibin=0;ibin<=12;ibin++) etaEMEndcapA[0][ibin] = 1.5+ibin*0.025; 
    for(int ibin=0;ibin<=12;ibin++) etaEMEndcapC[0][ibin] = -(1.5+ibin*0.025); 
    sort(etaEMEndcapC[0],etaEMEndcapC[0]+nbinsetaEMEndcap[0]+1);
    // Sampling 1 
    for(int ibin=0;ibin<=64;ibin++) phiEMEndcap[1][ibin] = -TMath::Pi()+ ibin*(2*M_PI/64);
    etaEMEndcapA[1][0] = 1.375; 
    for(int ibin=1;ibin<=3;ibin++) etaEMEndcapA[1][ibin] = 1.425+(ibin-1)*0.025; 
    for(int ibin=4;ibin<=99;ibin++) etaEMEndcapA[1][ibin] = 1.5+(ibin-4)*(0.025/8); 
    for(int ibin=100;ibin<=147;ibin++) etaEMEndcapA[1][ibin] = 1.8+(ibin-100)*(0.025/6); 
    for(int ibin=148;ibin<=211;ibin++) etaEMEndcapA[1][ibin] = 2.0+(ibin-148)*(0.025/4); 
    for(int ibin=212;ibin<=215;ibin++) etaEMEndcapA[1][ibin] = 2.4+(ibin-212)*0.025; 
    for(int ibin=216;ibin<=223;ibin++) etaEMEndcapA[1][ibin] = 2.5+(ibin-216)*0.1; 
    etaEMEndcapC[1][0] = -1.375; 
    for(int ibin=1;ibin<=3;ibin++) etaEMEndcapC[1][ibin] = -(1.425+(ibin-1)*0.025); 
    for(int ibin=4;ibin<=99;ibin++) etaEMEndcapC[1][ibin] = -(1.5+(ibin-4)*(0.025/8)); 
    for(int ibin=100;ibin<=147;ibin++) etaEMEndcapC[1][ibin] = -(1.8+(ibin-100)*(0.025/6)); 
    for(int ibin=148;ibin<=211;ibin++) etaEMEndcapC[1][ibin] = -(2.0+(ibin-148)*(0.025/4)); 
    for(int ibin=212;ibin<=215;ibin++) etaEMEndcapC[1][ibin] = -(2.4+(ibin-212)*0.025); 
    for(int ibin=216;ibin<=223;ibin++) etaEMEndcapC[1][ibin] = -(2.5+(ibin-216)*0.1); 
    sort(etaEMEndcapC[1],etaEMEndcapC[1]+nbinsetaEMEndcap[1]+1);
    // Sampling 2 
    // Last 7 eta bins are 4 times bigger in phi 
    for(int ibin=0;ibin<=256;ibin++) phiEMEndcap[2][ibin] = -TMath::Pi()+ ibin*(2*M_PI/256);
    etaEMEndcapA[2][0] = 1.375; 
    for(int ibin=1;ibin<=43;ibin++) etaEMEndcapA[2][ibin] = 1.425+(ibin-1)*0.025; 
    for(int ibin=44;ibin<=51;ibin++) etaEMEndcapA[2][ibin] = 2.5+(ibin-44)*0.1; 
    etaEMEndcapC[2][0] = -1.375; 
    for(int ibin=1;ibin<=43;ibin++) etaEMEndcapC[2][ibin] = -(1.425+(ibin-1)*0.025); 
    for(int ibin=44;ibin<=51;ibin++) etaEMEndcapC[2][ibin] = -(2.5+(ibin-44)*0.1); 
    sort(etaEMEndcapC[2],etaEMEndcapC[2]+nbinsetaEMEndcap[2]+1);
    // Sampling 3 
    for(int ibin=0;ibin<=256;ibin++) phiEMEndcap[3][ibin] = -TMath::Pi()+ ibin*(2*M_PI/256);
    for(int ibin=0;ibin<=20;ibin++) etaEMEndcapA[3][ibin] = 1.5+ibin*0.05; 
    for(int ibin=0;ibin<=20;ibin++) etaEMEndcapC[3][ibin] = -(1.5+ibin*0.05); 
    sort(etaEMEndcapC[3],etaEMEndcapC[3]+nbinsetaEMEndcap[3]+1);

    // Instantiate histograms
    for(int iS=0; iS<4; iS++){
      m_hCoverageEMECA[iS] = TH2I_LW::create(Form("CoverSampling%iEMECA",iS),
				     Form("Coverage - Sampling %i - EMEC A - LB %4d",iS,lb1),
				     nbinsetaEMEndcap[iS],etaEMEndcapA[iS],nbinsphiEMEndcap[iS],phiEMEndcap[iS]);
      SetPartCoverageStyle(m_hCoverageEMECA[iS]);
      CovGroupShift.regHist(m_hCoverageEMECA[iS]).ignore();

      m_hCoverageEMECC[iS] = TH2I_LW::create(Form("CoverSampling%iEMECC",iS),
				     Form("Coverage - Sampling %i - EMEC C - LB %4d",iS,lb1),
				     nbinsetaEMEndcap[iS],etaEMEndcapC[iS],nbinsphiEMEndcap[iS],phiEMEndcap[iS]);
      SetPartCoverageStyle(m_hCoverageEMECC[iS]);
      CovGroupShift.regHist(m_hCoverageEMECC[iS]).ignore();
    }

    // Delete pointers
    for(int it=0;it<4;it++)
      delete [] etaEMEndcapA[it];
    delete [] etaEMEndcapA;
    for(int it=0;it<4;it++)
      delete [] etaEMEndcapC[it];
    delete [] etaEMEndcapC;
    for(int it=0;it<4;it++)
      delete [] phiEMEndcap[it];
    delete [] phiEMEndcap;

    //
    // Coverage HEC  - Store cells granularity
    // 

    int nbinsphiHEC[] = {64,64,64,64};
    int nbinsetaHEC[] = {14,13,12,12};

    float** etaHECA=0; 
    etaHECA = new float*[size];
    for(int it=0;it<4;it++)
      etaHECA[it]= new float[nbinsetaHEC[it]+1];
    
    float** etaHECC=0; 
    etaHECC = new float*[size];
    for(int it=0;it<4;it++)
      etaHECC[it]= new float[nbinsetaHEC[it]+1];
    
    float** phiHEC=0;
    phiHEC = new float*[size];
    for(int it=0;it<4;it++)
      phiHEC[it]= new float[nbinsphiHEC[it]+1];

    for (int iS=0;iS<4;iS++){
      for(int ibin=0;ibin<=64;ibin++) phiHEC[iS][ibin] = -TMath::Pi()+ibin*2*TMath::Pi()/64;
    }
   
    for(int ibin=0;ibin<=10;ibin++)  etaHECA[0][ibin] = 1.5+ibin*0.1; 
    for(int ibin=11;ibin<=14;ibin++) etaHECA[0][ibin] = 2.5+(ibin-10)*0.2;
    for(int ibin=0;ibin<=10;ibin++)  etaHECA[1][ibin] = 1.5+ibin*0.1; 
    for(int ibin=11;ibin<=13;ibin++) etaHECA[1][ibin] = 2.5+(ibin-10)*0.2; 
    for(int ibin=0;ibin<=9;ibin++)   etaHECA[2][ibin] = 1.6+ibin*0.1; 
    for(int ibin=10;ibin<=12;ibin++) etaHECA[2][ibin] = 2.5+(ibin-9)*0.2; 
    for(int ibin=0;ibin<=8;ibin++)   etaHECA[3][ibin] = 1.7+ibin*0.1; 
    for(int ibin=9;ibin<=12;ibin++)  etaHECA[3][ibin] = 2.5+(ibin-8)*0.2; 

    for(int ibin=0;ibin<=10;ibin++)  etaHECC[0][ibin] = -(1.5+ibin*0.1); 
    for(int ibin=11;ibin<=14;ibin++) etaHECC[0][ibin] = -(2.5+(ibin-10)*0.2);
    for(int ibin=0;ibin<=10;ibin++)  etaHECC[1][ibin] = -(1.5+ibin*0.1); 
    for(int ibin=11;ibin<=13;ibin++) etaHECC[1][ibin] = -(2.5+(ibin-10)*0.2); 
    for(int ibin=0;ibin<=9;ibin++)   etaHECC[2][ibin] = -(1.6+ibin*0.1); 
    for(int ibin=10;ibin<=12;ibin++) etaHECC[2][ibin] = -(2.5+(ibin-9)*0.2); 
    for(int ibin=0;ibin<=8;ibin++)   etaHECC[3][ibin] = -(1.7+ibin*0.1); 
    for(int ibin=9;ibin<=12;ibin++)  etaHECC[3][ibin] = -(2.5+(ibin-8)*0.2); 
    for(int iS=0;iS<4;iS++) sort(etaHECC[iS],etaHECC[iS]+nbinsetaHEC[iS]+1);

    // Instantiate histograms
    for(int iS=0; iS<4; iS++){
      m_hCoverageHECA[iS] = TH2I_LW::create(Form("CoverSampling%iHECA",iS),
				     Form("Coverage - Sampling %i - HEC A - LB %4d",iS,lb1),
				     nbinsetaHEC[iS],etaHECA[iS],nbinsphiHEC[iS],phiHEC[iS]);
      SetPartCoverageStyle(m_hCoverageHECA[iS]);
      CovGroupShift.regHist(m_hCoverageHECA[iS]).ignore();

      m_hCoverageHECC[iS] = TH2I_LW::create(Form("CoverSampling%iHECC",iS),
				     Form("Coverage - Sampling %i - HEC C - LB %4d",iS,lb1),
				     nbinsetaHEC[iS],etaHECC[iS],nbinsphiHEC[iS],phiHEC[iS]);
      SetPartCoverageStyle(m_hCoverageHECC[iS]);
      CovGroupShift.regHist(m_hCoverageHECC[iS]).ignore();
    }

    // Delete pointers
    for(int it=0;it<4;it++)
      delete [] etaHECA[it];
    delete [] etaHECA;
    for(int it=0;it<4;it++)
      delete [] etaHECC[it];
    delete [] etaHECC;
    for(int it=0;it<4;it++){
      delete [] phiHEC[it];
    }
    delete [] phiHEC;

    //
    // Coverage FCAL - Store Cells granularity
    //


    // Instantiate histograms
    for(int iS=1; iS<4; iS++){
      m_hCoverageFCALA[iS] = TH2I_LW::create(Form("CoverSampling%iFCALA",iS),
					     Form("Coverage - Sampling %i - FCAL A - LB %4d",iS,lb1),
					     m_FCALBins->getXBins(+2,iS),
					     -m_FCALBins->getRMax(+2,iS),
					     m_FCALBins->getRMax(+2,iS),
					     m_FCALBins->getYBins(+2,iS),
					     -m_FCALBins->getRMax(+2,iS),
					     m_FCALBins->getRMax(+2,iS));

      SetPartCoverageStyle(m_hCoverageFCALA[iS]);
      // in theta-phi space
      // m_hCoverageFCALA[iS]->GetXaxis()->SetTitle("|tan(#theta)| #times sin(#phi)");
      // m_hCoverageFCALA[iS]->GetYaxis()->SetTitle("|tan(#theta)| #times cos(#phi)");   
      // in x-y space
      m_hCoverageFCALA[iS]->GetXaxis()->SetTitle("x (mm)");
      m_hCoverageFCALA[iS]->GetYaxis()->SetTitle("y (mm)");

      CovGroupShift.regHist(m_hCoverageFCALA[iS]).ignore();

      m_hCoverageFCALC[iS] = TH2I_LW::create(Form("CoverSampling%iFCALC",iS),
				     Form("Coverage - Sampling %i - FCAL C - LB %4d",iS,lb1),
					     m_FCALBins->getXBins(-2,iS),
					     -m_FCALBins->getRMax(-2,iS),
					     m_FCALBins->getRMax(-2,iS),
					     m_FCALBins->getYBins(-2,iS),
					     -m_FCALBins->getRMax(-2,iS),
					     m_FCALBins->getRMax(-2,iS));
      SetPartCoverageStyle(m_hCoverageFCALC[iS]);
      // in theta-phi space
      // m_hCoverageFCALC[iS]->GetXaxis()->SetTitle("|tan(#theta)| #times sin(#phi)");
      // m_hCoverageFCALC[iS]->GetYaxis()->SetTitle("|tan(#theta)| #times cos(#phi)");
      // in x-y space
      m_hCoverageFCALC[iS]->GetXaxis()->SetTitle("x (mm)");
      m_hCoverageFCALC[iS]->GetYaxis()->SetTitle("y (mm)");

      CovGroupShift.regHist(m_hCoverageFCALC[iS]).ignore();
    }

    //
    // Coverage histograms per FT/slot - One per partition.
    //

    m_hCoverageHWEMBA = TH2I_LW::create("CoverageHWEMBA",Form("Coverage - EMBA - LB %4d",lb1),448,0.,32.,128,-0.5,127.5);
    m_strHelper->definePartitionSummProp2(m_hCoverageHWEMBA).ignore();
    SetHWCoverageStyle(m_hCoverageHWEMBA);
    CovGroupShift.regHist(m_hCoverageHWEMBA).ignore();

    m_hCoverageHWEMBC = TH2I_LW::create("CoverageHWEMBC",Form("Coverage - EMBC - LB %4d",lb1),448,0.,32.,128,-0.5,127.5);
    m_strHelper->definePartitionSummProp2(m_hCoverageHWEMBC).ignore();
    SetHWCoverageStyle(m_hCoverageHWEMBC);
    CovGroupShift.regHist(m_hCoverageHWEMBC).ignore();

    m_hCoverageHWEMECA = TH2I_LW::create("CoverageHWEMECA",Form("Coverage - EMECA - LB %4d",lb1),375,0.,25.,128,-0.5,127.5);
    m_strHelper->definePartitionSummProp2(m_hCoverageHWEMECA).ignore();
    SetHWCoverageStyle(m_hCoverageHWEMECA);
    CovGroupShift.regHist(m_hCoverageHWEMECA).ignore();

    m_hCoverageHWEMECC = TH2I_LW::create("CoverageHWEMECC",Form("Coverage - EMECC - LB %4d",lb1),375,0.,25.,128,-0.5,127.5);
    m_strHelper->definePartitionSummProp2(m_hCoverageHWEMECC).ignore();
    SetHWCoverageStyle(m_hCoverageHWEMECC);
    CovGroupShift.regHist(m_hCoverageHWEMECC).ignore();

    m_hCoverageHWHECA = TH2I_LW::create("CoverageHWHECA",Form("Coverage - HECA - LB %4d",lb1),375,0.,25.,128,-0.5,127.5);
    m_strHelper->definePartitionSummProp2(m_hCoverageHWHECA).ignore();
    SetHWCoverageStyle(m_hCoverageHWHECA);
    CovGroupShift.regHist(m_hCoverageHWHECA).ignore();

    m_hCoverageHWHECC = TH2I_LW::create("CoverageHWHECC",Form("Coverage - HECC - LB %4d",lb1),375,0.,25.,128,-0.5,127.5);
    m_strHelper->definePartitionSummProp2(m_hCoverageHWHECC).ignore();
    SetHWCoverageStyle(m_hCoverageHWHECC);
    CovGroupShift.regHist(m_hCoverageHWHECC).ignore();

    m_hCoverageHWFCALA = TH2I_LW::create("CoverageHWFCALA",Form("Coverage - FCALA - LB %4d",lb1),375,0.,25.,128,-0.5,127.5);
    m_strHelper->definePartitionSummProp2(m_hCoverageHWFCALA).ignore();
    SetHWCoverageStyle(m_hCoverageHWFCALA);
    CovGroupShift.regHist(m_hCoverageHWFCALA).ignore();

    m_hCoverageHWFCALC = TH2I_LW::create("CoverageHWFCALC",Form("Coverage - FCALC - LB %4d",lb1),375,0.,25.,128,-0.5,127.5);
    m_strHelper->definePartitionSummProp2(m_hCoverageHWFCALC).ignore();
    SetHWCoverageStyle(m_hCoverageHWFCALC);
    CovGroupShift.regHist(m_hCoverageHWFCALC).ignore();
 
    //
    // CaloNoiseTool histograms - per FT/Slot
    //

    MonGroup CaloNoiseGroupShift( this, "/LAr/Coverage/CaloNoiseTool", run, ATTRIB_MANAGED, "", "lowerLB");

    // EM histos
    for (int i=0; i<4;i++){
      m_hCaloNoiseToolEM[i] = TProfile_LW::create(Form("CaloNoiseEMSampling%i",i),
					   Form("DBNoise in EM Sampling %i - LB %4d",i,lb1),
					   320, -3.2,3.2);
      m_hCaloNoiseToolEM[i]->GetXaxis()->SetTitle("#eta");
      m_hCaloNoiseToolEM[i]->GetYaxis()->SetTitle("[MeV]");
      CaloNoiseGroupShift.regHist(m_hCaloNoiseToolEM[i]).ignore();
    }
  
    // HEC histos
    for (int i=0; i<4;i++){
      m_hCaloNoiseToolHEC[i] = TProfile_LW::create(Form("CaloNoiseHECSampling%i",i),
					   Form("DBNoise in HEC Sampling %i - LB %4d",i,lb1),
					   320, -3.2,3.2);
      m_hCaloNoiseToolHEC[i]->GetXaxis()->SetTitle("#eta");
      m_hCaloNoiseToolHEC[i]->GetYaxis()->SetTitle("[MeV]");
      CaloNoiseGroupShift.regHist(m_hCaloNoiseToolHEC[i]).ignore();
    }  

    // FCAL histos
    for (int i=1; i<4;i++){
      m_hCaloNoiseToolFCAL[i] = TProfile_LW::create(Form("CaloNoiseFCALSampling%i",i),
					   Form("DBNoise in FCAL Sampling %i - LB %4d",i,lb1),
					   500, -5.0,5.0);
      m_hCaloNoiseToolFCAL[i]->GetXaxis()->SetTitle("#eta");
      m_hCaloNoiseToolFCAL[i]->GetYaxis()->SetTitle("[MeV]");
      CaloNoiseGroupShift.regHist(m_hCaloNoiseToolFCAL[i]).ignore();
    }  

    //
    // Bad Channels DataBase content
    //
     
    MonGroup BadChannelsGroupShift( this, "/LAr/Coverage/BadChannels/", run, ATTRIB_MANAGED, "", "lowerLB");
 
    m_hBadChannelsBarrelA = TH2I_LW::create("DBBadChannelsBarrelA",Form("Known Bad Channels - Barrel A - LB %4d",lb1),
					    448,0.,32.,128,-0.5,127.5);
    m_strHelper->definePartitionSummProp2(m_hBadChannelsBarrelA).ignore();
    SetBadChannelZaxisLabels(m_hBadChannelsBarrelA);
    BadChannelsGroupShift.regHist(m_hBadChannelsBarrelA).ignore();
        
    m_hBadChannelsBarrelC = TH2I_LW::create("DBBadChannelsBarrelC",Form("Known Bad Channels - Barrel C - LB %4d",lb1),
					    448,0.,32.,128,-0.5,127.5);
    m_strHelper->definePartitionSummProp2(m_hBadChannelsBarrelC).ignore();
    SetBadChannelZaxisLabels(m_hBadChannelsBarrelC);
    BadChannelsGroupShift.regHist(m_hBadChannelsBarrelC).ignore();
    
    m_hBadChannelsEndcapA = TH2I_LW::create("DBBadChannelsEndcapA",Form("Known Bad Channels - Endcap A - LB %4d",lb1),
					    375,0.,25.,128,-0.5,127.5);
    m_strHelper->definePartitionSummProp2(m_hBadChannelsEndcapA).ignore();
    SetBadChannelZaxisLabels(m_hBadChannelsEndcapA);
    BadChannelsGroupShift.regHist(m_hBadChannelsEndcapA).ignore();
    
    m_hBadChannelsEndcapC = TH2I_LW::create("DBBadChannelsEndcapC",Form("Known Bad Channels - Endcap C - LB %4d",lb1),
					    375,0.,25.,128,-0.5,127.5);
    m_strHelper->definePartitionSummProp2(m_hBadChannelsEndcapC).ignore();
    SetBadChannelZaxisLabels(m_hBadChannelsEndcapC);
    BadChannelsGroupShift.regHist(m_hBadChannelsEndcapC).ignore();
  
    return StatusCode::SUCCESS;
    
    //  }// end isNewRun
  
}


/*---------------------------------------------------------*/
StatusCode 
LArCoverage::fillHistograms()
{
  ATH_MSG_DEBUG( "in fillHists()" );

  m_eventsCounter++;

  if(m_eventsCounter > m_nevents ) return StatusCode::SUCCESS;

  // Retrieve Raw Channels Container
  const LArRawChannelContainer* pRawChannelsContainer;
  StatusCode sc = evtStore()->retrieve(pRawChannelsContainer, m_channelKey);
  if(sc.isFailure()) {
    ATH_MSG_WARNING( "Can\'t retrieve LArRawChannelContainer with key " << m_channelKey );
    return StatusCode::SUCCESS;
  }

  // Loop over LArRawChannels
  SelectAllLArRawChannels AllRaw(pRawChannelsContainer);
  for (SelectAllLArRawChannels::const_iterator itRaw = AllRaw.begin(); itRaw != AllRaw.end(); ++itRaw) {
    const LArRawChannel* pRawChannel = (*itRaw) ;
    int provenanceChan  = pRawChannel->provenance();
    float energyChan  = pRawChannel->energy();
    HWIdentifier id  = pRawChannel->hardwareID();
    //CaloGain::CaloGain gain = pRawChannel->gain();
    Identifier offlineID = m_larCablingService->cnvToIdentifier(id);
    
    // Skip disconnected channels
    if(!m_larCablingService->isOnlineConnected(id)) continue;
    
    // Get ft/slot info
    HWIdentifier febID = m_LArOnlineIDHelper->feb_Id(id);
    HWIdentifier feedthroughID = m_LArOnlineIDHelper->feedthrough_Id(id);
    int channel = m_LArOnlineIDHelper->channel(id);
    int slot = m_LArOnlineIDHelper->slot(febID);
    int ft = m_LArOnlineIDHelper->feedthrough(feedthroughID);
    
    // Get Physical Coordinates
    float etaChan = 0; float phiChan = 0.;
    const CaloDetDescrElement* caloDetElement = m_CaloDetDescrMgr->get_element(offlineID);
    if(caloDetElement == 0 ){
      ATH_MSG_ERROR( "Cannot retrieve (eta,phi) coordinates for raw channels" );
      continue; 
    }else{
      etaChan = caloDetElement->eta_raw();
      phiChan = caloDetElement->phi_raw();
    }
    
    // Fix phi range in HEC
    if (m_LArOnlineIDHelper->isHECchannel(id)) phiChan = CaloPhiRange::fix(phiChan);
    
    // Retrieve expected noise
    float noise = m_caloNoiseTool->getNoise(caloDetElement,ICalorimeterNoiseTool::ELECTRONICNOISE);
    
    if(m_eventsCounter == 1){
      
      // Plot the average expected noise vs eta for reference
      if(m_LArOnlineIDHelper->isEMBchannel(id) || m_LArOnlineIDHelper->isEMECchannel(id)){
	int sampling = m_LArEM_IDHelper->sampling(offlineID);
	m_hCaloNoiseToolEM[sampling]->Fill(etaChan, noise);
      }
      if(m_LArOnlineIDHelper->isHECchannel(id)){
	int sampling = m_LArHEC_IDHelper->sampling(offlineID);
	m_hCaloNoiseToolHEC[sampling]->Fill(etaChan, noise);
      }
      if(m_LArOnlineIDHelper->isFCALchannel(id)){
	int sampling = m_LArFCAL_IDHelper->module(offlineID);;
	m_hCaloNoiseToolFCAL[sampling]->Fill(etaChan, noise);
      }

      // Fill Bad Channels histograms  
      int flag = DBflag(id);
      if (flag==0) continue;
 
      // A-Side
      if(etaChan >= 0){
	if(m_LArOnlineIDHelper->isEMBchannel(id)){
	  m_hBadChannelsBarrelA->SetBinContent(ft*14+slot,channel+1,flag);
	}else{
	  m_hBadChannelsEndcapA->SetBinContent(ft*15+slot,channel+1,flag);
	}
      // C-side
      } else {
	if(m_LArOnlineIDHelper->isEMBchannel(id)){
	  m_hBadChannelsBarrelC->SetBinContent(ft*14+slot,channel+1,flag);
	}else{
	  m_hBadChannelsEndcapC->SetBinContent(ft*15+slot,channel+1,flag);
	}
      }
    }
    
    //
    // Compute cells status
    //

    int cellContent = 0;
    
    // Cell is connected and in the Readout
    // Select raw channels properly reconstructed, with all calib constants available
    // provenance&0x00ff == 0x00a5 : raw channels from OFC iteration, all calib constants found in DB
    // provenance&0x1000 == 0x1000 : raw channels from DSP. If no constant loaded in DSP, energy==0
    if ( (provenanceChan&0x00ff) == 0x00a5 || (provenanceChan&0x1000) == 0x1000 ){

      if(m_badChannelMask->cellShouldBeMasked(id)) cellContent=2;
      else if(energyChan != 0) cellContent=3;

    }

    //
    // Fill Coverage histograms
    //
    
    // A-Side
    if(etaChan >= 0){

      // EM Barrel
      if(m_LArOnlineIDHelper->isEMBchannel(id) ){      
	int sampling = m_LArEM_IDHelper->sampling(offlineID);
        int ix = m_hCoverageEMBA[sampling]->GetXaxis()->FindBin(etaChan); 
        int iy = m_hCoverageEMBA[sampling]->GetYaxis()->FindBin(phiChan);
        cellContent = TMath::Max(cellContent, int(m_hCoverageEMBA[sampling]->GetBinContent(ix,iy)));
	m_hCoverageEMBA[sampling]->SetBinContent(ix,iy,cellContent);
	m_hCoverageHWEMBA->SetBinContent(ft*14+slot,channel+1,cellContent);
      }

      // EM Endcap
      if(m_LArOnlineIDHelper->isEMECchannel(id)){      
	int sampling = m_LArEM_IDHelper->sampling(offlineID);
        int ix = m_hCoverageEMECA[sampling]->GetXaxis()->FindBin(etaChan); 
        int iy = m_hCoverageEMECA[sampling]->GetYaxis()->FindBin(phiChan);
        cellContent = TMath::Max(cellContent, int(m_hCoverageEMECA[sampling]->GetBinContent(ix,iy)));
	m_hCoverageEMECA[sampling]->SetBinContent(ix,iy,cellContent);
	m_hCoverageHWEMECA->SetBinContent(ft*15+slot,channel+1,cellContent);
      }

      // HEC
      if (m_LArOnlineIDHelper->isHECchannel(id)){
	int sampling = m_LArHEC_IDHelper->sampling(offlineID);
        int ix = m_hCoverageHECA[sampling]->GetXaxis()->FindBin(etaChan); 
        int iy = m_hCoverageHECA[sampling]->GetYaxis()->FindBin(phiChan);
        cellContent = TMath::Max(cellContent, int(m_hCoverageHECA[sampling]->GetBinContent(ix,iy)));
	m_hCoverageHECA[sampling]->SetBinContent(ix,iy,cellContent);
	m_hCoverageHWHECA->SetBinContent(ft*15+slot,channel+1,cellContent);
      }
      // FCAL
      if (m_LArOnlineIDHelper->isFCALchannel(id)){
	int sampling = m_LArFCAL_IDHelper->module(offlineID);
	int iEta = m_LArFCAL_IDHelper->eta(offlineID);
	int iPhi = m_LArFCAL_IDHelper->phi(offlineID);
        const unsigned short * iBin = m_FCALBins->getBins(+2,sampling,iEta,iPhi);
	if ( iBin[0] > 0  && iBin[1] > 0 ) {
	  unsigned short ix = iBin[1]%(m_FCALBins->getXBins(+2,sampling)+2);
	  unsigned short iy = (iBin[1]-ix)/(m_FCALBins->getXBins(+2,sampling)+2);
	  cellContent = TMath::Max(cellContent, int(m_hCoverageFCALA[sampling]->GetBinContent(ix,iy)));
	  m_hCoverageFCALA[sampling]->SetBinContent(ix,iy,cellContent);
	}
	m_hCoverageHWFCALA->SetBinContent(ft*15+slot,channel+1,cellContent);
      }

    // C-Side
    }else{
      
      // EM Barrel
      if(m_LArOnlineIDHelper->isEMBchannel(id)){      
	int sampling = m_LArEM_IDHelper->sampling(offlineID);
        int ix = m_hCoverageEMBC[sampling]->GetXaxis()->FindBin(etaChan); 
        int iy = m_hCoverageEMBC[sampling]->GetYaxis()->FindBin(phiChan);
        cellContent = TMath::Max(cellContent, int(m_hCoverageEMBC[sampling]->GetBinContent(ix,iy)));
	m_hCoverageEMBC[sampling]->SetBinContent(ix,iy,cellContent);
	m_hCoverageHWEMBC->SetBinContent(ft*14+slot,channel+1,cellContent);
      }

      // EM Endcap
      if(m_LArOnlineIDHelper->isEMECchannel(id)){      
	int sampling = m_LArEM_IDHelper->sampling(offlineID);
        int ix = m_hCoverageEMECC[sampling]->GetXaxis()->FindBin(etaChan); 
        int iy = m_hCoverageEMECC[sampling]->GetYaxis()->FindBin(phiChan);
        cellContent = TMath::Max(cellContent, int(m_hCoverageEMECC[sampling]->GetBinContent(ix,iy)));
	m_hCoverageEMECC[sampling]->SetBinContent(ix,iy,cellContent);
	m_hCoverageHWEMECC->SetBinContent(ft*15+slot,channel+1,cellContent);
      }

      // HEC
      if (m_LArOnlineIDHelper->isHECchannel(id)){
	int sampling = m_LArHEC_IDHelper->sampling(offlineID);
        int ix = m_hCoverageHECC[sampling]->GetXaxis()->FindBin(etaChan); 
        int iy = m_hCoverageHECC[sampling]->GetYaxis()->FindBin(phiChan);
        cellContent = TMath::Max(cellContent, int(m_hCoverageHECC[sampling]->GetBinContent(ix,iy)));
	m_hCoverageHECC[sampling]->SetBinContent(ix,iy,cellContent);
	m_hCoverageHWHECC->SetBinContent(ft*15+slot,channel+1,cellContent);
      }
      // FCAL
      if (m_LArOnlineIDHelper->isFCALchannel(id)){
	int sampling = m_LArFCAL_IDHelper->module(offlineID);
	int iEta = m_LArFCAL_IDHelper->eta(offlineID);
	int iPhi = m_LArFCAL_IDHelper->phi(offlineID);
        const unsigned short * iBin = m_FCALBins->getBins(-2,sampling,iEta,iPhi);
	if ( iBin[0] > 0 && iBin[1] > 0 ) {
	  unsigned short ix = iBin[1]%(m_FCALBins->getXBins(-2,sampling)+2);
	  unsigned short iy = (iBin[1]-ix)/(m_FCALBins->getXBins(-2,sampling)+2);
	  cellContent = TMath::Max(cellContent, int(m_hCoverageFCALC[sampling]->GetBinContent(ix,iy)));
	  m_hCoverageFCALC[sampling]->SetBinContent(ix,iy,cellContent);
	}
	m_hCoverageHWFCALC->SetBinContent(ft*15+slot,channel+1,cellContent);
      }
    }

  }// end Raw Channels Loop
  

  //
  // Fill known missing FEBs with -1
  //
  
  FillKnownMissingFEBs(m_CaloDetDescrMgr);

  //
  // Fix for Cosmetic : Fill "empty bins" in plots 
  // where granularity varies vs eta&phi
  //
  
  FixEmptyBins();
  
  // End of event treatment

  return StatusCode::SUCCESS;
}
/*---------------------------------------------------------*/
StatusCode LArCoverage::procHistograms()
{

  ATH_MSG_DEBUG( "In procHistograms " );

  return StatusCode::SUCCESS;

}
/*---------------------------------------------------------*/
int LArCoverage::DBflag(HWIdentifier onID){

  int flag = 0;
  LArBadChannel bc = m_badChannelTool->status(onID);

  if(bc.deadCalib()) flag = 1;
  if(bc.lowNoiseHG()||bc.lowNoiseMG()||bc.lowNoiseLG()) flag = 2;
  if(bc.distorted()) flag = 3;
  if(bc.unstable()) flag = 4;
  if(bc.sporadicBurstNoise())  flag = 5; 
  if(bc.highNoiseHG()|| bc.highNoiseMG() || bc.highNoiseLG())  flag = 6; 
  if(bc.deadReadout()||bc.deadPhys()||bc.almostDead()||bc.shortProblem()) flag = 7;

  return flag;
}
/*---------------------------------------------------------*/
void LArCoverage::SetPartCoverageStyle(TH2I_LW* h){

  h->GetXaxis()->SetTitle("#eta cell");
  h->GetYaxis()->SetTitle("#phi cell"); 
  h->SetMinimum(1.);     
  h->SetMaximum(4.4);
}
/*---------------------------------------------------------*/
void LArCoverage::SetHWCoverageStyle(TH2I_LW* h){

  h->SetMinimum(1.);     
  h->SetMaximum(4.4);
}
/*---------------------------------------------------------*/
void LArCoverage::SetBadChannelZaxisLabels(TH2I_LW* h){

      h->SetMinimum(0.);     
      h->SetMaximum(10.);
}
/*---------------------------------------------------------*/
void LArCoverage::FillKnownMissingFEBs(const CaloDetDescrManager* caloDetDescrMgr){

  // Loop over all FEBs
  for (std::vector<HWIdentifier>::const_iterator allFeb = m_LArOnlineIDHelper->feb_begin(); 
       allFeb != m_LArOnlineIDHelper->feb_end(); ++allFeb) {
    HWIdentifier febid = HWIdentifier(*allFeb);

    // Known missing FEB: set content to 1
    // Known missing FEB but cells actually readout: set content to 4
    // Known FEB with error: set content to 0 (CaloCells not produced)

    const LArBadFeb febStatus = m_badChannelTool->febStatus(febid);

    if(m_badChannelTool->febMissing(febid) || febStatus.inError()){

      int barrel_ec = m_LArOnlineIDHelper->barrel_ec(febid);
      int pos_neg   = m_LArOnlineIDHelper->pos_neg(febid);
      int ft        = m_LArOnlineIDHelper->feedthrough(febid);
      int slot      = m_LArOnlineIDHelper->slot(febid);   
      
      // EMBA
      if (barrel_ec==0 and pos_neg==1){
	int content; int replace = 0;
	if (m_badChannelTool->febMissing(febid)){
	  content = int(m_hCoverageHWEMBA->GetBinContent(ft*14+slot,1));
	  if(content==0 || content==1) replace = 1;
	  else replace = 4;
	}
	if (febStatus.inError()) replace = 1;
	for (int ichan=0;ichan<128;ichan++){
	  HWIdentifier chid = m_LArOnlineIDHelper->channel_Id(febid,ichan);
	  // Skip disconnected channels
	  if(!m_larCablingService->isOnlineConnected(chid)) continue;
	  m_hCoverageHWEMBA->SetBinContent(ft*14+slot,ichan+1,replace); 

	  Identifier offid = m_larCablingService->cnvToIdentifier(chid);
	  int sampling = m_LArEM_IDHelper->sampling(offid);
          float eta, phi;
	  const CaloDetDescrElement* caloDetElement = caloDetDescrMgr->get_element(offid);
	  if(caloDetElement == 0 ){
	    ATH_MSG_ERROR( "Cannot retrieve (eta,phi) coordinates for raw channels" );
	    continue; 
	  }else{
	    eta = caloDetElement->eta_raw();
	    phi = caloDetElement->phi_raw();
	  }
          int ix = m_hCoverageEMBA[sampling]->GetXaxis()->FindBin(eta); 
          int iy = m_hCoverageEMBA[sampling]->GetYaxis()->FindBin(phi);
	  m_hCoverageEMBA[sampling]->SetBinContent(ix,iy,replace);
	}
      }

      // EMBC
      if (barrel_ec==0 and pos_neg==0){
	int content; int replace = 0;
	if (m_badChannelTool->febMissing(febid)){
	  content = int(m_hCoverageHWEMBC->GetBinContent(ft*14+slot,1));
	  if(content==0 || content==1) replace = 1;
	  else replace = 4;
	}
	if (febStatus.inError()) replace = 1;
	for (int ichan=0;ichan<128;ichan++){
	  HWIdentifier chid = m_LArOnlineIDHelper->channel_Id(febid,ichan);
	  // Skip disconnected channels
	  if(!m_larCablingService->isOnlineConnected(chid)) continue;
	  m_hCoverageHWEMBC->SetBinContent(ft*14+slot,ichan+1,replace); 

	  Identifier offid = m_larCablingService->cnvToIdentifier(chid);
	  int sampling = m_LArEM_IDHelper->sampling(offid);
          float eta, phi;
	  const CaloDetDescrElement* caloDetElement = caloDetDescrMgr->get_element(offid);
	  if(caloDetElement == 0 ){
	    ATH_MSG_ERROR( "Cannot retrieve (eta,phi) coordinates for raw channels" );
	    continue; 
	  }else{
	    eta = caloDetElement->eta_raw();
	    phi = caloDetElement->phi_raw();
	  }
          int ix = m_hCoverageEMBC[sampling]->GetXaxis()->FindBin(eta); 
          int iy = m_hCoverageEMBC[sampling]->GetYaxis()->FindBin(phi);
	  m_hCoverageEMBC[sampling]->SetBinContent(ix,iy,replace);
	}
      }

      // ENDCAP A
      if (barrel_ec==1 and pos_neg==1){
	// First find out if it's EMEC, HEC or FCAL !
	HWIdentifier test_chid = m_LArOnlineIDHelper->channel_Id(febid,1);

	// EMECA
	if( m_LArOnlineIDHelper->isEMECchannel(test_chid)){
	  int content; int replace = 0;
	  if (m_badChannelTool->febMissing(febid)){
	    content = int(m_hCoverageHWEMECA->GetBinContent(ft*15+slot,1));
	    if(content==0 || content==1) replace = 1;
	    else replace = 4;
	  }
	  if (febStatus.inError()) replace = 1;
	  for (int ichan=0;ichan<128;ichan++){
	    HWIdentifier chid = m_LArOnlineIDHelper->channel_Id(febid,ichan);
	    // Skip disconnected channels
	    if(!m_larCablingService->isOnlineConnected(chid)) continue;
	    m_hCoverageHWEMECA->SetBinContent(ft*15+slot,ichan+1,replace); 

	    Identifier offid = m_larCablingService->cnvToIdentifier(chid);
	    int sampling = m_LArEM_IDHelper->sampling(offid);
	    float eta, phi;
	    const CaloDetDescrElement* caloDetElement = caloDetDescrMgr->get_element(offid);
	    if(caloDetElement == 0 ){
	      ATH_MSG_ERROR( "Cannot retrieve (eta,phi) coordinates for raw channels" );
	      continue; 
	    }else{
	      eta = caloDetElement->eta_raw();
	      phi = caloDetElement->phi_raw();
	    }
	    int ix = m_hCoverageEMECA[sampling]->GetXaxis()->FindBin(eta); 
	    int iy = m_hCoverageEMECA[sampling]->GetYaxis()->FindBin(phi);
	    m_hCoverageEMECA[sampling]->SetBinContent(ix,iy,replace);
	  }
	}

	// HECA
	if( m_LArOnlineIDHelper->isHECchannel(test_chid)){
	  int content; int replace = 0;
	  if (m_badChannelTool->febMissing(febid)){
	    content = int(m_hCoverageHWHECA->GetBinContent(ft*15+slot,1));
	    if(content==0 || content==1) replace = 1;
	    else replace = 4;
	  }
	  if (febStatus.inError()) replace = 1;
	  for (int ichan=0;ichan<128;ichan++){
	    HWIdentifier chid = m_LArOnlineIDHelper->channel_Id(febid,ichan);
	    // Skip disconnected channels
	    if(!m_larCablingService->isOnlineConnected(chid)) continue;
	    m_hCoverageHWHECA->SetBinContent(ft*15+slot,ichan+1,replace); 

	    Identifier offid = m_larCablingService->cnvToIdentifier(chid);
	    int sampling = m_LArHEC_IDHelper->sampling(offid);
	    float eta, phi;
	    const CaloDetDescrElement* caloDetElement = caloDetDescrMgr->get_element(offid);
	    if(caloDetElement == 0 ){
	      ATH_MSG_ERROR( "Cannot retrieve (eta,phi) coordinates for raw channels" );
	      continue; 
	    }else{
	      eta = caloDetElement->eta_raw();
	      phi = caloDetElement->phi_raw();
	    }
	    int ix = m_hCoverageHECA[sampling]->GetXaxis()->FindBin(eta); 
	    int iy = m_hCoverageHECA[sampling]->GetYaxis()->FindBin(phi);
	    m_hCoverageHECA[sampling]->SetBinContent(ix,iy,replace);
	  }
	}

	// FCALA
	if( m_LArOnlineIDHelper->isFCALchannel(test_chid)){
	  int content; int replace = 0;
	  if (m_badChannelTool->febMissing(febid)){
	    content = int(m_hCoverageHWFCALA->GetBinContent(ft*15+slot,1));
	    if(content==0 || content==1) replace = 1;
	    else replace = 4;
	  }
	  if (febStatus.inError()) replace = 1;
	  for (int ichan=0;ichan<128;ichan++){
	    HWIdentifier chid = m_LArOnlineIDHelper->channel_Id(febid,ichan);
	    // Skip disconnected channels
	    if(!m_larCablingService->isOnlineConnected(chid)) continue;
	    m_hCoverageHWFCALA->SetBinContent(ft*15+slot,ichan+1,replace); 

	    Identifier offid = m_larCablingService->cnvToIdentifier(chid);
	    int sampling = m_LArFCAL_IDHelper->module(offid);
	    int ieta = m_LArFCAL_IDHelper->eta(offid);
	    int iphi = m_LArFCAL_IDHelper->phi(offid);
	    const unsigned short * iBin = m_FCALBins->getBins(+2,sampling,ieta,iphi);
	    if ( iBin[0] > 0 && iBin[1] > 0 ) {
	      unsigned short ix = iBin[1]%(m_FCALBins->getXBins(+2,sampling)+2);
	      unsigned short iy = (iBin[1]-ix)/(m_FCALBins->getXBins(+2,sampling)+2);
	      m_hCoverageFCALA[sampling]->SetBinContent(ix,iy,replace);
	    }
	  }
	}
      }

      // ENDCAP C
      if (barrel_ec==1 and pos_neg==0){
	// First find out if it's EMEC, HEC or FCAL !
	HWIdentifier test_chid = m_LArOnlineIDHelper->channel_Id(febid,1);

	// EMECC
	if( m_LArOnlineIDHelper->isEMECchannel(test_chid)){
	  int content; int replace = 0;
	  if (m_badChannelTool->febMissing(febid)){
	    content = int(m_hCoverageHWEMECC->GetBinContent(ft*15+slot,1));
	    if(content==0 || content==1) replace = 1;
	    else replace = 4;
	  }
	  if (febStatus.inError()) replace = 1;
	  for (int ichan=0;ichan<128;ichan++){
	    HWIdentifier chid = m_LArOnlineIDHelper->channel_Id(febid,ichan);
	    // Skip disconnected channels
	    if(!m_larCablingService->isOnlineConnected(chid)) continue;
	    m_hCoverageHWEMECC->SetBinContent(ft*15+slot,ichan+1,replace); 

	    Identifier offid = m_larCablingService->cnvToIdentifier(chid);
	    int sampling = m_LArEM_IDHelper->sampling(offid);
	    float eta, phi;
	    const CaloDetDescrElement* caloDetElement = caloDetDescrMgr->get_element(offid);
	    if(caloDetElement == 0 ){
	      ATH_MSG_ERROR( "Cannot retrieve (eta,phi) coordinates for raw channels" );
	      continue; 
	    }else{
	      eta = caloDetElement->eta_raw();
	      phi = caloDetElement->phi_raw();
	    }
	    int ix = m_hCoverageEMECC[sampling]->GetXaxis()->FindBin(eta); 
	    int iy = m_hCoverageEMECC[sampling]->GetYaxis()->FindBin(phi);
	    m_hCoverageEMECC[sampling]->SetBinContent(ix,iy,replace);
	  }
	}

	// HECC
	if( m_LArOnlineIDHelper->isHECchannel(test_chid)){
	  int content; int replace = 0;
	  if (m_badChannelTool->febMissing(febid)){
	    content = int(m_hCoverageHWHECC->GetBinContent(ft*15+slot,1));
	    if(content==0 || content==1) replace = 1;
	    else replace = 4;
	  }
	  if (febStatus.inError()) replace = 1;
	  for (int ichan=0;ichan<128;ichan++){
	    HWIdentifier chid = m_LArOnlineIDHelper->channel_Id(febid,ichan);
	    // Skip disconnected channels
	    if(!m_larCablingService->isOnlineConnected(chid)) continue;
	    m_hCoverageHWHECC->SetBinContent(ft*15+slot,ichan+1,replace); 

	    Identifier offid = m_larCablingService->cnvToIdentifier(chid);
	    int sampling = m_LArHEC_IDHelper->sampling(offid);
	    float eta, phi;
	    const CaloDetDescrElement* caloDetElement = caloDetDescrMgr->get_element(offid);
	    if(caloDetElement == 0 ){
	      ATH_MSG_ERROR( "Cannot retrieve (eta,phi) coordinates for raw channels" );
	      continue; 
	    }else{
	      eta = caloDetElement->eta_raw();
	      phi = caloDetElement->phi_raw();
	    }
	    int ix = m_hCoverageHECC[sampling]->GetXaxis()->FindBin(eta); 
	    int iy = m_hCoverageHECC[sampling]->GetYaxis()->FindBin(phi);
	    m_hCoverageHECC[sampling]->SetBinContent(ix,iy,replace);
	  }
	}

	// FCALC
	if( m_LArOnlineIDHelper->isFCALchannel(test_chid)){
	  int content; int replace = 0;
	  if (m_badChannelTool->febMissing(febid)){
	    content = int(m_hCoverageHWFCALC->GetBinContent(ft*15+slot,1));
	    if(content==0 || content==1) replace = 1;
	    else replace = 4;
	  }
	  if (febStatus.inError()) replace = 1;
	  for (int ichan=0;ichan<128;ichan++){
	    HWIdentifier chid = m_LArOnlineIDHelper->channel_Id(febid,ichan);
	    // Skip disconnected channels
	    if(!m_larCablingService->isOnlineConnected(chid)) continue;
	    m_hCoverageHWFCALC->SetBinContent(ft*15+slot,ichan+1,replace); 

	    Identifier offid = m_larCablingService->cnvToIdentifier(chid);
	    int sampling = m_LArFCAL_IDHelper->module(offid);
	    int ieta = m_LArFCAL_IDHelper->eta(offid);
	    int iphi = m_LArFCAL_IDHelper->phi(offid);
	    const unsigned short * iBin = m_FCALBins->getBins(-2,sampling,ieta,iphi);
	    if ( iBin[0] > 0 && iBin[1] > 0 ) {
	      unsigned short ix = iBin[1]%(m_FCALBins->getXBins(-2,sampling)+2);
	      unsigned short iy = (iBin[1]-ix)/(m_FCALBins->getXBins(-2,sampling)+2);
	      m_hCoverageFCALC[sampling]->SetBinContent(ix,iy,replace);
	    }
	  }
	}
      }
    } // end missing FEB
  } // end FEB loop

  return;
}
/*---------------------------------------------------------*/
void LArCoverage::FixEmptyBins(){

  // Sampling 1, EM Barrel, |eta| < 1.4
  for (int iphi=1;iphi<=256;iphi+=4){
    for (int ieta=1;ieta<448;ieta++){
      int contentA =  int(m_hCoverageEMBA[1]->GetBinContent(ieta,iphi+1));
      float errorA = m_hCoverageEMBA[1]->GetBinError(ieta,iphi+1);
      m_hCoverageEMBA[1]->SetBinContent(ieta,iphi,contentA);
      m_hCoverageEMBA[1]->SetBinContent(ieta,iphi+2,contentA);
      m_hCoverageEMBA[1]->SetBinContent(ieta,iphi+3,contentA);
      m_hCoverageEMBA[1]->SetBinError(ieta,iphi,errorA);
      m_hCoverageEMBA[1]->SetBinError(ieta,iphi+2,errorA);
      m_hCoverageEMBA[1]->SetBinError(ieta,iphi+3,errorA);
    }
  }
  for (int iphi=1;iphi<=256;iphi+=4){
    for (int ieta=4;ieta<451;ieta++){
      int contentC =  int(m_hCoverageEMBC[1]->GetBinContent(ieta,iphi+2));
      float errorC = m_hCoverageEMBC[1]->GetBinError(ieta,iphi+2);
      m_hCoverageEMBC[1]->SetBinContent(ieta,iphi,contentC);
      m_hCoverageEMBC[1]->SetBinContent(ieta,iphi+1,contentC);
      m_hCoverageEMBC[1]->SetBinContent(ieta,iphi+3,contentC);
      m_hCoverageEMBC[1]->SetBinError(ieta,iphi,errorC);
      m_hCoverageEMBC[1]->SetBinError(ieta,iphi+1,errorC);
      m_hCoverageEMBC[1]->SetBinError(ieta,iphi+3,errorC);
    }
  }
  
  // Sampling 2, EM Endcap, |eta| > 2.5
  
  for (int iphi=1;iphi<=256;iphi+=4){
    for (int ieta=44;ieta<52;ieta++){
      int contentA =  int(m_hCoverageEMECA[2]->GetBinContent(ieta,iphi+1));
      float errorA = m_hCoverageEMECA[2]->GetBinError(ieta,iphi+1);
      m_hCoverageEMECA[2]->SetBinContent(ieta,iphi,contentA);
      m_hCoverageEMECA[2]->SetBinContent(ieta,iphi+2,contentA);
      m_hCoverageEMECA[2]->SetBinContent(ieta,iphi+3,contentA);
      m_hCoverageEMECA[2]->SetBinError(ieta,iphi,errorA);
      m_hCoverageEMECA[2]->SetBinError(ieta,iphi+2,errorA);
      m_hCoverageEMECA[2]->SetBinError(ieta,iphi+3,errorA);
    }
  }
  for (int iphi=1;iphi<=256;iphi+=4){
    for (int ieta=1;ieta<8;ieta++){
      int contentC =  int(m_hCoverageEMECC[2]->GetBinContent(ieta,iphi+2));
      float errorC =  m_hCoverageEMECC[2]->GetBinError(ieta,iphi+2);
      m_hCoverageEMECC[2]->SetBinContent(ieta,iphi,contentC);
      m_hCoverageEMECC[2]->SetBinContent(ieta,iphi+1,contentC);
      m_hCoverageEMECC[2]->SetBinContent(ieta,iphi+3,contentC);
      m_hCoverageEMECC[2]->SetBinError(ieta,iphi,errorC);
      m_hCoverageEMECC[2]->SetBinError(ieta,iphi+1,errorC);
      m_hCoverageEMECC[2]->SetBinError(ieta,iphi+3,errorC);
    }
  }
  
  // All Samplings, HEC, |eta| > 2.5
  
  int etabinmin[]={11,11,9,9};
  int etabinmax[]={14,13,12,12};
  
  for (int iS=0;iS<4;iS++){
    
    for (int iphi=1;iphi<=64;iphi+=2){
      for (int ieta=etabinmin[iS];ieta<=etabinmax[iS];ieta++){
	int contentA =  int(m_hCoverageHECA[iS]->GetBinContent(ieta,iphi+1));
         float errorA = m_hCoverageHECA[iS]->GetBinError(ieta,iphi+1);
	m_hCoverageHECA[iS]->SetBinContent(ieta,iphi,contentA);	   
	m_hCoverageHECA[iS]->SetBinError(ieta,iphi,errorA);	   
      }
    }
    
    for (int iphi=1;iphi<=64;iphi+=2){
      for (int ieta=1; ieta<=(etabinmax[iS]-etabinmin[iS])+1;ieta++){
	int contentC =  int(m_hCoverageHECC[iS]->GetBinContent(ieta,iphi+1));
        float errorC = m_hCoverageHECC[iS]->GetBinError(ieta,iphi+1);
	m_hCoverageHECC[iS]->SetBinContent(ieta,iphi,contentC);	   
	m_hCoverageHECC[iS]->SetBinError(ieta,iphi,errorC);	   
      }
    }  
  }

  // All Samplings, FCAL
  
  int nBinsEtaFCAL[4] = {0,63,32,16};

  for (int iSFCal=1;iSFCal<4;iSFCal++){
    
    for (int iphi=0;iphi<16;iphi++) {
      for (int ieta=0;ieta<nBinsEtaFCAL[iSFCal];ieta++) {
	const unsigned short * theBins = m_FCALBins->getBins(+2,iSFCal,ieta,iphi);
	int iBin = 2;
	if ( theBins[0] > 0 && theBins[1] > 0 ) {
	  unsigned short ix = theBins[1]%(m_FCALBins->getXBins(+2,iSFCal)+2);
	  unsigned short iy = (theBins[1]-ix)/(m_FCALBins->getXBins(+2,iSFCal)+2);
	  int cellContent = m_hCoverageFCALA[iSFCal]->GetBinContent(ix,iy);
	  while ( theBins[iBin] > 0 ) {
	    ix = theBins[iBin]%(m_FCALBins->getXBins(+2,iSFCal)+2);
	    iy = (theBins[iBin]-ix)/(m_FCALBins->getXBins(+2,iSFCal)+2);
	    m_hCoverageFCALA[iSFCal]->SetBinContent(ix,iy,cellContent);
	    iBin++;
	  }
	}
	theBins = m_FCALBins->getBins(-2,iSFCal,ieta,iphi);
	iBin = 2;
	if ( theBins[0] > 0 && theBins[1] > 0 ) {
	  unsigned short ix = theBins[1]%(m_FCALBins->getXBins(-2,iSFCal)+2);
	  unsigned short iy = (theBins[1]-ix)/(m_FCALBins->getXBins(-2,iSFCal)+2);
	  int cellContent = m_hCoverageFCALC[iSFCal]->GetBinContent(ix,iy);
	  while ( theBins[iBin] > 0 ) {
	    ix = theBins[iBin]%(m_FCALBins->getXBins(-2,iSFCal)+2);
	    iy = (theBins[iBin]-ix)/(m_FCALBins->getXBins(-2,iSFCal)+2);
	    m_hCoverageFCALC[iSFCal]->SetBinContent(ix,iy,cellContent);
	    iBin++;
	  }
	}
      }
    }
  }
}
  
