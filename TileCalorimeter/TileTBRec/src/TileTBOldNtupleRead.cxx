/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Rtypes.h"
#include "TileTBRec/TileTBOldNtupleRead.h"
#include "TileTBRec/TileTBOldNtupleStruct.h"

//Gaudi Includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "StoreGate/StoreGateSvc.h"

//TileCalo includes
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileTBFrag.h"
#include "TileConditions/TileCablingService.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileBeamElemContainer.h"
#include "TileRecUtils/TileBeamInfoProvider.h"

TileTBOldNtupleRead * TileTBOldNtupleRead::s_instance = 0;

TileTBOldNtupleRead * TileTBOldNtupleRead::getInstance()
{
  return s_instance;
}

TileTBOldNtupleRead::TileTBOldNtupleRead(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_tileHWID(0)
  , m_cabling(0)
  , m_beamInfo("TileBeamInfoProvider")
  , m_trigType(0)
  , m_file(0)
  , m_ntuple(0)
  , m_ntupleStruct(0)
{
  m_eventShift.resize(6);
  declareProperty("NtupleID",m_ntupleID = "h1000");
  declareProperty("NtupleDir",m_ntupleDir = ".");
  declareProperty("RunNumber",m_runNumber = 0);
  declareProperty("TileDigitsContainer", m_digitsContainer = "TileDigitsCnt");
  declareProperty("TileBeamElemContainer",m_beamContainer = "TileBeamElemCnt");
  declareProperty("BiGain",m_bigain = false);
  declareProperty("FrameLength", m_nSamp = 9);
  declareProperty("PmtOrder", m_pmtOrder = true);
  declareProperty("BarrelOnly", m_barrelOnly = false);
  declareProperty("SkipEvents", m_eventNumber = 0);
  declareProperty("AdderFit", m_adderFit = true);
  declareProperty("EventShift", m_eventShift);

  m_maxEventNumber=0;

  s_instance = this;
}

TileTBOldNtupleRead::~TileTBOldNtupleRead() 
{
}

//==============================================
// initialize
//==============================================
StatusCode TileTBOldNtupleRead::initialize() 
{
  ATH_MSG_INFO ( "Initialization started" );
  
  // find TileCablingService
  m_cabling = TileCablingService::getInstance();

  ATH_CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_CHECK( m_beamInfo.retrieve() );
  ATH_CHECK( m_beamInfo->setProperty("TileBeamElemContainer",m_beamContainer) );
  ATH_CHECK( m_beamInfo->setProperty("TileDigitsContainer",m_digitsContainer) );

  m_eventShift.resize(8);
  for (int i=0; i<6; ++i) { // find miminum and maximum event shift
    if (m_eventShift[i] < m_eventShift[6] ) m_eventShift[6] = m_eventShift[i];
    if (m_eventShift[i] > m_eventShift[7] ) m_eventShift[7] = m_eventShift[i];
  }
  
  if (m_eventNumber < 0) m_eventNumber = 0;
  
  if (m_eventNumber+m_eventShift[6]<0) {
    ATH_MSG_INFO ( "Adjusting first ntuple entry to read, setting it to " << -m_eventShift[6] );
    m_eventNumber = -m_eventShift[6] - 1;
  } else {
    ATH_MSG_INFO ( "Reading old ntuple starting from entry " << m_eventNumber );
    m_eventNumber -= 1;
  }
  
  if (m_eventShift[6] != 0 || m_eventShift[7] != 0) {
    ATH_MSG_INFO ( "Additional shift for drawers"
                   << "  N0: " << m_eventShift[0]
                   << "  N1: " << m_eventShift[1]
                   << "  N2: " << m_eventShift[2]
                   << "  P0: " << m_eventShift[3]
                   << "  P1: " << m_eventShift[4]
                   << "  P2: " << m_eventShift[5] );
  }

  char fname[100];
  sprintf(fname,"%s/r%7.7d.root",m_ntupleDir.c_str(),m_runNumber);
  m_file=new TFile(fname,"OLD");
  if (m_file == NULL) {
    ATH_MSG_ERROR ( "File not found: " << fname );
    return StatusCode::FAILURE;
  }

  m_ntuple=(TTree*)(m_file->Get(m_ntupleID.c_str()));
  if (m_ntuple == NULL) {
    ATH_MSG_ERROR ( "Ntuple not found: " << m_ntupleID );
    return StatusCode::FAILURE;
  }

  if (m_nSamp != 9) {
    ATH_MSG_ERROR ( "Unexpected number of samples: " << m_nSamp );
    return StatusCode::FAILURE;
  }
  
  m_maxEventNumber = m_ntuple->GetEntries() - m_eventShift[7];
  ATH_MSG_INFO ( "Max available entry to read " << m_maxEventNumber );
  
  if (m_bigain) {
    ATH_MSG_INFO ( "Assuming bi-gain ntuple structure with " << m_nSamp << " samples" );
  } else {
    ATH_MSG_INFO ( "Assuming mono-gain ntuple structure with " << m_nSamp << " samples" );
  }

  m_ntupleStruct = new TileTBOldNtupleStruct(m_ntuple,m_bigain,m_adderFit,m_eventShift);

  ATH_MSG_INFO ( "Initialization completed." );
  return StatusCode::SUCCESS;
}

//==============================================
// execute
//==============================================
StatusCode TileTBOldNtupleRead::execute() 
{
  ++m_eventNumber;
  if (m_eventNumber >= m_maxEventNumber) {
    ATH_MSG_ERROR ( "All " << m_eventNumber << " entries processed. " );
    return StatusCode::FAILURE;
  }

  m_ntupleStruct->GetEntry(m_eventNumber);
  ATH_MSG_DEBUG ( "Reading entry " << m_eventNumber 
                  << " event number from ntuple is " << m_ntupleStruct->Evt );
  
  // Fill beam elements container (PATTERN UNIT and CISPAR only)
  TileBeamElemContainer* pBeamContainer = new TileBeamElemContainer(true);

  m_trigType = m_ntupleStruct->Trig;
  ATH_MSG_VERBOSE ( "Trig type = " << m_trigType );
  HWIdentifier adc_id = m_tileHWID->adc_id(0,LASE_PTN_FRAG,0,0);
  int value = (m_trigType << 8);
  TileBeamElem * rc = new TileBeamElem(adc_id,value);
  pBeamContainer->push_back(rc);

  if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "Cis par:";
  for(int ch=0; ch<12; ++ch) {
    adc_id = m_tileHWID->adc_id(0,DIGI_PAR_FRAG,ch,0);
    value = m_ntupleStruct->cispar[ch];
    if (msgLvl(MSG::VERBOSE)) msg() << "  " << value;
    TileBeamElem * rc = new TileBeamElem(adc_id,value);
    pBeamContainer->push_back(rc);
  }
  if (msgLvl(MSG::VERBOSE)) msg() << endmsg;
  StatusCode sc=evtStore()->record(pBeamContainer, m_beamContainer, false);
  if(sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to register the container: " << m_beamContainer );
  }
  
  // Fill the TileDigitsContainer
  TileDigitsContainer* pDigitsContainer = new TileDigitsContainer(true);
  std::vector<double> digitSamples(m_nSamp);
  
  if (m_bigain) {

    for(int iros=1; iros<=2; ++iros) { // loop over Positive/Negative side

      for(int dr=0; dr<3; ++dr) { // loop over drawers 0,1,2

        // cabling for testbeam (change ros for ext.barrel)
        int ros = drawer2ros(iros,dr);
        int drawerInd = (iros%2 == 0) ? dr : dr+3; // positive drawers shifted by 3

	for(int ch=0; ch<48; ++ch) { // loop over channels

          // cabling for testbeam (convert channel to pmt#-1)
          int pmt = (m_pmtOrder) ? digiChannel2PMT(ros,ch) : ch;

          for (int gain=0; gain<2; ++gain) {
            
            UShort_t * samples = 0;
            switch (drawerInd*2+gain) {
            case  0: samples = m_ntupleStruct->Samplen0lo[pmt]; break;
            case  1: samples = m_ntupleStruct->Samplen0hi[pmt]; break;
            case  2: samples = m_ntupleStruct->Samplen1lo[pmt]; break;
            case  3: samples = m_ntupleStruct->Samplen1hi[pmt]; break;
            case  4: samples = m_ntupleStruct->Samplen2lo[pmt]; break;
            case  5: samples = m_ntupleStruct->Samplen2hi[pmt]; break;
            case  6: samples = m_ntupleStruct->Samplep0lo[pmt]; break;
            case  7: samples = m_ntupleStruct->Samplep0hi[pmt]; break;
            case  8: samples = m_ntupleStruct->Samplep1lo[pmt]; break;
            case  9: samples = m_ntupleStruct->Samplep1hi[pmt]; break;
            case 10: samples = m_ntupleStruct->Samplep2lo[pmt]; break;
            case 11: samples = m_ntupleStruct->Samplep2hi[pmt]; break;
            default:
              ATH_MSG_ERROR ( "Bad drawer ID: IROS=" << iros << ", dr=" << dr );
            }

            HWIdentifier adc_id = m_tileHWID->adc_id(ros,dr,ch,gain);
            if (msgLvl(MSG::VERBOSE))
              msg() << MSG::VERBOSE << m_tileHWID->to_string(adc_id)
                  << " ch=" << ch << " pmt=" << pmt+1 << " gain=" << gain << " samples: ";
          
            if (samples) {
              for(int i=0; i<m_nSamp; ++i) {
                digitSamples[i] = samples[i];
                if (msgLvl(MSG::VERBOSE)) msg() << " " << digitSamples[i];
              }
            } else {
              for(int i=0; i<m_nSamp; ++i) {
                digitSamples[i] = 0;
              }
            }
            if (msgLvl(MSG::VERBOSE)) msg() << endmsg;
          	  
            TileDigits * digi = new TileDigits(adc_id,digitSamples);
            pDigitsContainer->push_back(digi);
            //if (verbose) {
              //log << MSG::VERBOSE << m_tileHWID->to_string(adc_id) << " " << (std::string)(*digi) << endmsg;
            //}
          }
	}
      }
    }

  } else { // mono gain

    for(int iros=1; iros<=2; ++iros) { // loop over Positive/Negative side

      for(int dr=0; dr<3; ++dr) { // loop over drawers 0,1,2

        // cabling for testbeam (change ros for ext.barrel)
        int ros = drawer2ros(iros,dr);
        int drawerInd = (iros%2 == 0) ? dr : dr+3; // positive drawers shifted by 3

	for(int ch=0; ch<48; ++ch) { // loop over channels

          // cabling for testbeam (convert channel to pmt#-1)
          int pmt = (m_pmtOrder) ? digiChannel2PMT(ros,ch) : ch;

          int gain = 0;
          UShort_t * samples = 0;
          switch (drawerInd) {
          case 0: gain = m_ntupleStruct->Gainn0lo[pmt]; samples = m_ntupleStruct->Samplen0lo[pmt]; break;
          case 1: gain = m_ntupleStruct->Gainn1lo[pmt]; samples = m_ntupleStruct->Samplen1lo[pmt]; break;
          case 2: gain = m_ntupleStruct->Gainn2lo[pmt]; samples = m_ntupleStruct->Samplen2lo[pmt]; break;
          case 3: gain = m_ntupleStruct->Gainp0lo[pmt]; samples = m_ntupleStruct->Samplep0lo[pmt]; break;
          case 4: gain = m_ntupleStruct->Gainp1lo[pmt]; samples = m_ntupleStruct->Samplep1lo[pmt]; break;
          case 5: gain = m_ntupleStruct->Gainp2lo[pmt]; samples = m_ntupleStruct->Samplep2lo[pmt]; break;
          default:
            ATH_MSG_ERROR ( "Bad drawer ID: IROS=" << iros << ", dr=" << dr );
          }

	  HWIdentifier adc_id = m_tileHWID->adc_id(ros,dr,ch,gain);
          if (msgLvl(MSG::VERBOSE))
            msg() << MSG::VERBOSE << m_tileHWID->to_string(adc_id)
                << " ch=" << ch << " pmt=" << pmt+1 << " gain=" << gain << " samples: ";
          
	  for(int i=0; i<m_nSamp; ++i) {
            digitSamples[i] = samples[i];
            if (msgLvl(MSG::VERBOSE)) msg() << " " << digitSamples[i];
          }
          if (msgLvl(MSG::VERBOSE)) msg() << endmsg;
          	  
	  TileDigits * digi = new TileDigits(adc_id,digitSamples);
	  pDigitsContainer->push_back(digi);
          //if (verbose) {
            //log << MSG::VERBOSE << m_tileHWID->to_string(adc_id) << " " << (std::string)(*digi) << endmsg;
	  //}
	}
      }
    }
  }
  
  sc=evtStore()->record(pDigitsContainer, m_digitsContainer, false);
  if(sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to register the container: " << m_digitsContainer );
  }

  Incident inc(name(),IncidentType::BeginEvent);
  m_beamInfo->handle (inc);

  return StatusCode::SUCCESS;
}
  
//==============================================
// finalize
//==============================================
StatusCode TileTBOldNtupleRead::finalize() 
{
  ATH_MSG_INFO("finalize()");

  delete m_ntuple;
  m_file->Close();
  delete m_file;
  delete m_ntupleStruct;

  ATH_MSG_INFO("finalize() completed successfully");
  return StatusCode::SUCCESS;
}
