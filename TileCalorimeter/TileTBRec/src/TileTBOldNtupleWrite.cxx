/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileTBRec/TileTBOldNtupleWrite.h"
#include "TileTBRec/TileTBOldNtupleRead.h"

//Gaudi Includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

//TileCalo includes
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileTBFrag.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileBeamElemContainer.h"
#include "TileEvent/TileRawChannelContainer.h"

TileTBOldNtupleWrite::TileTBOldNtupleWrite(std::string name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator)
   , m_bigain(0)
   , m_calibrateEnergyThisEvent(0)
   , m_tileHWID(0)
   , m_cabling(0)
   , m_tileToolEmscale("TileCondToolEmscale")
   , m_runNumber(0)
   , m_eventNumber(0)
   , m_trigType(0)
   , m_file(0)
   , m_ntupleWrite(0)
   , m_ntupleRead(0)
   , m_readNtupleAlg(0)
{
  declareProperty("TileCondToolEmscale", m_tileToolEmscale);
  declareProperty("NtupleID",m_ntupleID = "h1000");
  declareProperty("NtupleDir",m_ntupleDir = ".");
  declareProperty("TileDigitsContainer", m_digitsContainer = "TileDigitsCnt");
  declareProperty("TileRawChannelContainerFit", m_rawChannelContainerFit = "TileRawChannelFit");
  declareProperty("TileRawChannelContainerFlat", m_rawChannelContainerFlat = ""); // do not fill by default
  declareProperty("PmtOrder", m_pmtOrder = true);
  declareProperty("CalibrateEnergy", m_calibrateEnergy = true);
}

TileTBOldNtupleWrite::~TileTBOldNtupleWrite()
{
}

StatusCode TileTBOldNtupleWrite::initialize()
{
  ATH_MSG_INFO ( "Initialization started" );
  
  // find TileCablingService
  m_cabling = TileCablingService::getInstance();

  ATH_CHECK( detStore()->retrieve(m_tileHWID) );
  ATH_CHECK( m_tileToolEmscale.retrieve() );

  m_readNtupleAlg=TileTBOldNtupleRead::getInstance();

  m_bigain = m_readNtupleAlg->BiGain();
  m_runNumber = m_readNtupleAlg->RunNumber();
  m_eventNumber = m_readNtupleAlg->EventNumber();
  m_ntupleRead = m_readNtupleAlg->getNtuple();
  
  char fname[100];
  sprintf(fname,"%s/r%7.7d.new.root",m_ntupleDir.c_str(),m_runNumber);
  m_file= new TFile(fname,"RECREATE");
  m_ntupleWrite=new TTree(m_ntupleID.c_str(),m_ntupleRead->GetTitle());
  if (m_ntupleWrite == NULL) {
    ATH_MSG_ERROR ( "Can not create ntuple  " << m_ntupleID );
    return StatusCode::FAILURE;
  }

  // Initialize all branches in the m->writeNtuple, using the branches and 
  // addresses from the m->readNtuple. Like that, no assignments should be 
  // needed. However, need to exclude the branches that are filled in later on
  // from StoreGate.

  for(int i=0; i<m_ntupleRead->GetListOfBranches()->GetEntries(); i++) {
    TBranch * br=(TBranch*)(m_ntupleRead->GetListOfBranches()->At(i));
    std::string name = br->GetName();
    std::string title = br->GetTitle();
    Float_t * ptr=this->BranchWriteNtuple(name);
    if (ptr == NULL) { 
      m_ntupleWrite->Branch(name.c_str(),(void*)(br->GetAddress()),title.c_str());
    } else {
      m_ntupleWrite->Branch(name.c_str(),(void*)ptr,title.c_str());
    }
  }

  ATH_MSG_INFO ( "Initialization completed." );
  return StatusCode::SUCCESS;
}
  
StatusCode TileTBOldNtupleWrite::execute()
{
  m_eventNumber = m_readNtupleAlg->EventNumber();
  m_trigType = m_readNtupleAlg->TrigType();
  ATH_MSG_DEBUG ( "Writing event " << m_eventNumber << " trig Type is " << m_trigType );

  // do not apply Cesium and Laser calibration for CIS events
  m_calibrateEnergyThisEvent = m_calibrateEnergy && (m_trigType != 8);

  // get named container
  const TileRawChannelContainer* rcCnt;

  if (m_rawChannelContainerFit != "" ) {
    
    ATH_CHECK( evtStore()->retrieve(rcCnt, m_rawChannelContainerFit) );

    TileRawChannelUnit::UNIT rChUnit = rcCnt->get_unit();

    // Get iterator for all TRCColl in TRCCont
    TileRawChannelContainer::const_iterator itColl = (*rcCnt).begin(); 
    TileRawChannelContainer::const_iterator itCollEnd = (*rcCnt).end(); 

    // Go through all TileRawChannelCollections
    for(; itColl != itCollEnd; ++itColl) {
      
      // go through all TileRawChannels in collection
      TileRawChannelCollection::const_iterator it = (*itColl)->begin();
      TileRawChannelCollection::const_iterator itEnd = (*itColl)->end();
      
      for(; it != itEnd; ++it) {
        HWIdentifier hwid=(*it)->adc_HWID();

        int gain = m_tileHWID->adc(hwid);
        int channel = m_tileHWID->channel(hwid);
        int drawer = m_tileHWID->drawer(hwid);
        int ros = m_tileHWID->ros(hwid);
        int drawerIdx = TileCalibUtils::getDrawerIdx(ros,drawer);

        /// Cesium and Laser calibration
        /// we assume that CIS calibration was done already in Opt. filtering
        /// and we have already energy in pCb
        /// after cesium calibration we still have the same pCb for A and BC cells
        /// what we do is just set the same scale for D-cells
        double energy =  (*it)->amplitude();
        if(m_calibrateEnergyThisEvent) {         
          energy = m_tileToolEmscale->channelCalib(drawerIdx, channel, gain, energy,
                                                   rChUnit, TileRawChannelUnit::CesiumPicoCoulombs);
          /// probably we want to convert to MeV - use TileRawChannelUnit::MegaElectronVolts ?
        }
        
        // cabling for testbeam (convert channel to pmt#-1)
        int pmt = (m_pmtOrder) ? digiChannel2PMT(ros,channel) : channel;
        
        ATH_MSG_VERBOSE("TRC " << m_tileHWID->to_string(hwid)
                        << " ene="<<energy<<" time="<<(*it)->time()
                        <<" chi2="<<(*it)->quality()<<" ped="<<(*it)->pedestal()
                        <<" pmt-1="<<pmt);

        int gainMode = (m_bigain) ? gain : 0; // 0 - low gain, 1 - high gain
        int drawerInd = (ros%2 == 0) ? drawer : drawer+3; // positive drawers shifted by 3

        m_efit  [gainMode][drawerInd][pmt] = energy;
        m_tfit  [gainMode][drawerInd][pmt] = (*it)->time();
        m_pedfit[gainMode][drawerInd][pmt] = (*it)->pedestal();
        m_chi2  [gainMode][drawerInd][pmt] = (*it)->quality();
        
      } // end of loop over channels
    } // end of loop over collections
  } // end of if for fit method container
  

  if (m_rawChannelContainerFlat != "" ) {
    
    ATH_CHECK( evtStore()->retrieve(rcCnt, m_rawChannelContainerFlat) );

    TileRawChannelUnit::UNIT rChUnit = rcCnt->get_unit();

    // Get iterator for all TRCColl in TRCCont
    TileRawChannelContainer::const_iterator itColl = (*rcCnt).begin(); 
    TileRawChannelContainer::const_iterator itCollEnd = (*rcCnt).end(); 

    // Go through all TileRawChannelCollections
    for(; itColl != itCollEnd; ++itColl) {
      
      // go through all TileRawChannels in collection
      TileRawChannelCollection::const_iterator it = (*itColl)->begin();
      TileRawChannelCollection::const_iterator itEnd = (*itColl)->end();
      
      for(; it != itEnd; ++it) {
        HWIdentifier hwid=(*it)->adc_HWID();

        int gain = m_tileHWID->adc(hwid);
        int channel = m_tileHWID->channel(hwid);
        int drawer = m_tileHWID->drawer(hwid);
        int ros = m_tileHWID->ros(hwid);
        int drawerIdx = TileCalibUtils::getDrawerIdx(ros,drawer);

        /// Cesium and Laser calibration
        /// we assume that CIS calibration was done already in Opt. filtering
        /// and we have already energy in pCb
        /// after cesium calibration we still have the same pCb for A and BC cells
        /// what we do is just set the same scale for D-cells
        double energy =  (*it)->amplitude();
        if(m_calibrateEnergyThisEvent) {         
          energy = m_tileToolEmscale->channelCalib(drawerIdx, channel, gain, energy,
                                                   rChUnit, TileRawChannelUnit::CesiumPicoCoulombs);
          /// probably we want to convert to MeV - use TileRawChannelUnit::MegaElectronVolts ?
        }
        
        // cabling for testbeam (convert channel to pmt#-1)
        int pmt = (m_pmtOrder) ? digiChannel2PMT(ros,channel) : channel;

        ATH_MSG_VERBOSE("TRC " << m_tileHWID->to_string(hwid)
                        << " flat ene="<<energy<<" time="<<(*it)->time()
                        <<" pmt-1="<<pmt);

        int gainMode = (m_bigain) ? gain : 0; // 0 - low gain, 1 - high gain
        int drawerInd = (ros%2 == 0) ? drawer : drawer+3; // positive drawers shifted by 3

        m_ene [gainMode][drawerInd][pmt] = energy;
        m_time[gainMode][drawerInd][pmt] = (*it)->time();
        
      } // end of loop over channels
    } // end of loop over collections
  } // end of if for flat filter method container

  m_ntupleWrite->Fill();

  return StatusCode::SUCCESS;
}

StatusCode TileTBOldNtupleWrite::finalize()
{
  m_file->cd();
  m_ntupleWrite->Write();
  delete m_ntupleWrite;
  m_file->Close();
  delete m_file;

  return StatusCode::SUCCESS;
}

Float_t * TileTBOldNtupleWrite::BranchWriteNtuple(const std::string name)
{
  // copy everything except of EfitXX, TfitXX, PedfitXX, Chi2XX, EneXX, TimeXX
  // for these variables return the pointer to new private arrays

  if (m_bigain) {
    // first low-gain fit variables
    if        (name == "Efitn0lo")   { return &m_efit[0][0][0];   
    } else if (name == "Efitn1lo")   { return &m_efit[0][1][0];   
    } else if (name == "Efitn2lo")   { return &m_efit[0][2][0];   
    } else if (name == "Efitp0lo")   { return &m_efit[0][3][0];   
    } else if (name == "Efitp1lo")   { return &m_efit[0][4][0];   
    } else if (name == "Efitp2lo")   { return &m_efit[0][5][0];   
    } else if (name == "Tfitn0lo")   { return &m_tfit[0][0][0];   
    } else if (name == "Tfitn1lo")   { return &m_tfit[0][1][0];   
    } else if (name == "Tfitn2lo")   { return &m_tfit[0][2][0];   
    } else if (name == "Tfitp0lo")   { return &m_tfit[0][3][0];   
    } else if (name == "Tfitp1lo")   { return &m_tfit[0][4][0];   
    } else if (name == "Tfitp2lo")   { return &m_tfit[0][5][0];   
    } else if (name == "Pedfitn0lo") { return &m_pedfit[0][0][0]; 
    } else if (name == "Pedfitn1lo") { return &m_pedfit[0][1][0]; 
    } else if (name == "Pedfitn2lo") { return &m_pedfit[0][2][0]; 
    } else if (name == "Pedfitp0lo") { return &m_pedfit[0][3][0]; 
    } else if (name == "Pedfitp1lo") { return &m_pedfit[0][4][0]; 
    } else if (name == "Pedfitp2lo") { return &m_pedfit[0][5][0]; 
    } else if (name == "Chi2n0lo")   { return &m_chi2[0][0][0];   
    } else if (name == "Chi2n1lo")   { return &m_chi2[0][1][0];   
    } else if (name == "Chi2n2lo")   { return &m_chi2[0][2][0];   
    } else if (name == "Chi2p0lo")   { return &m_chi2[0][3][0];   
    } else if (name == "Chi2p1lo")   { return &m_chi2[0][4][0];   
    } else if (name == "Chi2p2lo")   { return &m_chi2[0][5][0];

    // now high-gain fit variables
    } else if (name == "Efitn0hi")   { return &m_efit[1][0][0];   
    } else if (name == "Efitn1hi")   { return &m_efit[1][1][0];   
    } else if (name == "Efitn2hi")   { return &m_efit[1][2][0];   
    } else if (name == "Efitp0hi")   { return &m_efit[1][3][0];   
    } else if (name == "Efitp1hi")   { return &m_efit[1][4][0];   
    } else if (name == "Efitp2hi")   { return &m_efit[1][5][0];   
    } else if (name == "Tfitn0hi")   { return &m_tfit[1][0][0];   
    } else if (name == "Tfitn1hi")   { return &m_tfit[1][1][0];   
    } else if (name == "Tfitn2hi")   { return &m_tfit[1][2][0];   
    } else if (name == "Tfitp0hi")   { return &m_tfit[1][3][0];   
    } else if (name == "Tfitp1hi")   { return &m_tfit[1][4][0];   
    } else if (name == "Tfitp2hi")   { return &m_tfit[1][5][0];   
    } else if (name == "Pedfitn0hi") { return &m_pedfit[1][0][0]; 
    } else if (name == "Pedfitn1hi") { return &m_pedfit[1][1][0]; 
    } else if (name == "Pedfitn2hi") { return &m_pedfit[1][2][0]; 
    } else if (name == "Pedfitp0hi") { return &m_pedfit[1][3][0]; 
    } else if (name == "Pedfitp1hi") { return &m_pedfit[1][4][0]; 
    } else if (name == "Pedfitp2hi") { return &m_pedfit[1][5][0]; 
    } else if (name == "Chi2n0hi")   { return &m_chi2[1][0][0];   
    } else if (name == "Chi2n1hi")   { return &m_chi2[1][1][0];   
    } else if (name == "Chi2n2hi")   { return &m_chi2[1][2][0];   
    } else if (name == "Chi2p0hi")   { return &m_chi2[1][3][0];   
    } else if (name == "Chi2p1hi")   { return &m_chi2[1][4][0];   
    } else if (name == "Chi2p2hi")   { return &m_chi2[1][5][0];   

    // now low-gain flat filter variables
    } else if (m_rawChannelContainerFlat != "" && name == "Enen0lo")    { return &m_ene[0][0][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Enen1lo")    { return &m_ene[0][1][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Enen2lo")    { return &m_ene[0][2][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Enep0lo")    { return &m_ene[0][3][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Enep1lo")    { return &m_ene[0][4][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Enep2lo")    { return &m_ene[0][5][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Timen0lo")   { return &m_time[0][0][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Timen1lo")   { return &m_time[0][1][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Timen2lo")   { return &m_time[0][2][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Timep0lo")   { return &m_time[0][3][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Timep1lo")   { return &m_time[0][4][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Timep2lo")   { return &m_time[0][5][0];   

    // now high-gain flat filter variables
    } else if (m_rawChannelContainerFlat != "" && name == "Enen0hi")    { return &m_ene[1][0][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Enen1hi")    { return &m_ene[1][1][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Enen2hi")    { return &m_ene[1][2][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Enep0hi")    { return &m_ene[1][3][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Enep1hi")    { return &m_ene[1][4][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Enep2hi")    { return &m_ene[0][5][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Timen0hi")   { return &m_time[1][0][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Timen1hi")   { return &m_time[1][1][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Timen2hi")   { return &m_time[1][2][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Timep0hi")   { return &m_time[1][3][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Timep1hi")   { return &m_time[1][4][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Timep2hi")   { return &m_time[1][5][0];   

    } else {
      return(NULL);
    }
  } else { // monogain

    if        (name == "Efitn0")   { return &m_efit[0][0][0];   
    } else if (name == "Efitn1")   { return &m_efit[0][1][0];   
    } else if (name == "Efitn2")   { return &m_efit[0][2][0];   
    } else if (name == "Efitp0")   { return &m_efit[0][3][0];   
    } else if (name == "Efitp1")   { return &m_efit[0][4][0];   
    } else if (name == "Efitp2")   { return &m_efit[0][5][0];   
    } else if (name == "Tfitn0")   { return &m_tfit[0][0][0];   
    } else if (name == "Tfitn1")   { return &m_tfit[0][1][0];   
    } else if (name == "Tfitn2")   { return &m_tfit[0][2][0];   
    } else if (name == "Tfitp0")   { return &m_tfit[0][3][0];   
    } else if (name == "Tfitp1")   { return &m_tfit[0][4][0];   
    } else if (name == "Tfitp2")   { return &m_tfit[0][5][0];   
    } else if (name == "Pedfitn0") { return &m_pedfit[0][0][0]; 
    } else if (name == "Pedfitn1") { return &m_pedfit[0][1][0]; 
    } else if (name == "Pedfitn2") { return &m_pedfit[0][2][0]; 
    } else if (name == "Pedfitp0") { return &m_pedfit[0][3][0]; 
    } else if (name == "Pedfitp1") { return &m_pedfit[0][4][0]; 
    } else if (name == "Pedfitp2") { return &m_pedfit[0][5][0]; 
    } else if (name == "Chi2n0")   { return &m_chi2[0][0][0];   
    } else if (name == "Chi2n1")   { return &m_chi2[0][1][0];   
    } else if (name == "Chi2n2")   { return &m_chi2[0][2][0];   
    } else if (name == "Chi2p0")   { return &m_chi2[0][3][0];   
    } else if (name == "Chi2p1")   { return &m_chi2[0][4][0];   
    } else if (name == "Chi2p2")   { return &m_chi2[0][5][0];   

    } else if (m_rawChannelContainerFlat != "" && name == "Enen0")    { return &m_ene[0][0][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Enen1")    { return &m_ene[0][1][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Enen2")    { return &m_ene[0][2][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Enep0")    { return &m_ene[0][3][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Enep1")    { return &m_ene[0][4][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Enep2")    { return &m_ene[0][5][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Timen0")   { return &m_time[0][0][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Timen1")   { return &m_time[0][1][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Timen2")   { return &m_time[0][2][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Timep0")   { return &m_time[0][3][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Timep1")   { return &m_time[0][4][0];   
    } else if (m_rawChannelContainerFlat != "" && name == "Timep2")   { return &m_time[0][5][0];   

    } else {
      return(NULL);
    }
  }
}  
