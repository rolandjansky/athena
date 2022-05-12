/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileMuidMonTool.cxx
// PACKAGE:  TileMonitoring  
//
// AUTHORS:   Alexander Solodkov
//            Luca Fiorini (Luca.Fiorini@cern.ch) 
//
// ********************************************************************

#include "TileMuIdMonTool.h"

#include "TileEvent/TileMu.h"
#include "TileEvent/TileMuContainer.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"


/*---------------------------------------------------------*/
TileMuIdMonTool::TileMuIdMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_TileMuidTrig(0)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("muidsContainerName", m_muidsContName = "TileMuObj"); //SG Muid Container
  m_path = "/Tile/Muid"; //ROOT File directory
}

/*---------------------------------------------------------*/
TileMuIdMonTool::~TileMuIdMonTool() {
/*---------------------------------------------------------*/

}

/*---------------------------------------------------------*/
StatusCode TileMuIdMonTool::initialize() {
/*---------------------------------------------------------*/


  ATH_MSG_INFO( "in initialize()" );
  return TileFatherMonTool::initialize();
}

/*---------------------------------------------------------*/
StatusCode TileMuIdMonTool::bookHistTrig( int trig ) {
/*---------------------------------------------------------*/


  m_activeTrigs[trig] = m_TileMuidTrig;
  int element = m_activeTrigs[trig];

  std::string runNumStr = getRunNumStr();

  m_TileMuidTrig++; //Increment trigger index

  m_TileMuidTags.push_back( book1F(m_TrigNames[trig],"tileMuidTags" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal Muid number of tags",5,0., 5.) );
  
  m_TileMuidEnergy.push_back( book1F(m_TrigNames[trig],"tileMuidEnergy" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal Muid Energy dep (MeV)",100,0., 10000.) );
  m_TileMuidEnergy[ element ]->GetXaxis()->SetTitle("Muid Energy (MeV)");
  
  m_TileMuidEtaPhi.push_back( book2F(m_TrigNames[trig],"tileMuidEtaPhi" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal 2D Muid muon position",40,-2.0, 2.0,64,-3.15,3.15) );
  m_TileMuidEtaPhi[ element ]->GetXaxis()->SetTitle("#eta");
  m_TileMuidEtaPhi[ element ]->GetYaxis()->SetTitle("#phi");
    
  m_TileMuidEta.push_back( book1F(m_TrigNames[trig],"tileMuidEta" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal Muid muon #eta position",40,-2.0, 2.0) );
  m_TileMuidEta[ element ]->GetXaxis()->SetTitle("#eta");
  
  m_TileMuidPhi.push_back( book1F(m_TrigNames[trig],"tileMuidPhi" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal Muid muon #phi position",64,-3.15, 3.15) );
  m_TileMuidPhi[ element ]->GetXaxis()->SetTitle("#phi");
  
  m_TileMuidEneEta.push_back( bookProfile(m_TrigNames[trig],"tileMuidEneEta" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal Muid muon average energy as a function of #eta position",40,-2.0, 2.0,-2.e6,2.e6) );
  m_TileMuidEneEta[ element ]->GetXaxis()->SetTitle("#eta");

  m_TileMuidEnePhi.push_back( bookProfile(m_TrigNames[trig],"tileMuidEnePhi" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal Muid muon average energy as a function of #phi position",64,-3.15, 3.15,-2.e6,2.e6) );
  m_TileMuidEnePhi[ element ]->GetXaxis()->SetTitle("#phi");

  m_TileMuidQuality.push_back( book1F(m_TrigNames[trig],"tileMuidQuality" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal Muid tag quality",4,-0.5, 1.5) );
  m_TileMuidQuality[ element ]->GetXaxis()->SetTitle("quality");
  
  m_TileMuidEneQuality.push_back( book2F(m_TrigNames[trig],"tileMuidEneQuality" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal 2D Muid muon energy (MeV) vs. quality",50,0., 10000.,4,-0.5,1.5) );
  m_TileMuidEneQuality[ element ]->GetXaxis()->SetTitle("Energy (MeV)");
  m_TileMuidEneQuality[ element ]->GetYaxis()->SetTitle("Quality");
  
    
  m_TileMuid1TagEnergy.push_back( book1F(m_TrigNames[trig],"tileMuid1TagEnergy" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal Muid Tags=1 Energy dep (MeV)",100,0., 10000.) );
  m_TileMuid1TagEnergy[ element ]->GetXaxis()->SetTitle("Muid1Tag Energy (MeV)");
  
  m_TileMuid1TagEtaPhi.push_back( book2F(m_TrigNames[trig],"tileMuid1TagEtaPhi" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal 2D Muid Tags=1 muon position",40,-2.0, 2.0,64,-3.15,3.15) );
  m_TileMuid1TagEtaPhi[ element ]->GetXaxis()->SetTitle("#eta");
  m_TileMuid1TagEtaPhi[ element ]->GetYaxis()->SetTitle("#phi");
  
  m_TileMuid1TagEta.push_back( book1F(m_TrigNames[trig],"tileMuid1TagEta" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal Muid Tags=1 muon #eta position",40,-2.0, 2.0) );
  m_TileMuid1TagEta[ element ]->GetXaxis()->SetTitle("#eta");
  
  m_TileMuid1TagPhi.push_back( book1F(m_TrigNames[trig],"tileMuid1TagPhi" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal Muid Tags=1 muon #phi position",64,-3.15, 3.15) );
  m_TileMuid1TagPhi[ element ]->GetXaxis()->SetTitle("#phi");
    
  m_TileMuid1TagQuality.push_back( book1F(m_TrigNames[trig],"tileMuid1TagQuality" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal Muid Tags=1 tag quality",4,-0.5, 1.5) );
  m_TileMuid1TagQuality[ element ]->GetXaxis()->SetTitle("quality");
    
  m_TileMuid1TagEneQuality.push_back( book2F(m_TrigNames[trig],"tileMuid1TagEneQuality" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal 2D Muid Tags=1 muon energy (MeV) vs. quality",50,0., 10000.,4,-0.5,1.5) );
  m_TileMuid1TagEneQuality[ element ]->GetXaxis()->SetTitle("Energy (MeV)");
  m_TileMuid1TagEneQuality[ element ]->GetYaxis()->SetTitle("Quality");
    
    
  m_TileMuid2TagEnergy.push_back( book1F(m_TrigNames[trig],"tileMuid2TagEnergy" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal Muid Tags=2 Energy dep (MeV)",100,0., 10000.) );
  m_TileMuid2TagEnergy[ element ]->GetXaxis()->SetTitle("Muid2Tag Energy (MeV)");
    
  m_TileMuid2TagEtaPhi.push_back( book2F(m_TrigNames[trig],"tileMuid2TagEtaPhi" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal 2D Muid Tags=2 muon position",40,-2.0, 2.0,64,-3.15,3.15) );
  m_TileMuid2TagEtaPhi[ element ]->GetXaxis()->SetTitle("#eta");
  m_TileMuid2TagEtaPhi[ element ]->GetYaxis()->SetTitle("#phi");
  
  m_TileMuid2TagEta.push_back( book1F(m_TrigNames[trig],"tileMuid2TagEta" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal Muid Tags=2 muon #eta position",40,-2.0, 2.0) );
  m_TileMuid2TagEta[ element ]->GetXaxis()->SetTitle("#eta");

  m_TileMuid2TagPhi.push_back( book1F(m_TrigNames[trig],"tileMuid2TagPhi" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal Muid Tags=2 muon #phi position",64,-3.15, 3.15) );
  m_TileMuid2TagPhi[ element ]->GetXaxis()->SetTitle("#phi");
    
  m_TileMuid2TagQuality.push_back( book1F(m_TrigNames[trig],"tileMuid2TagQuality" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal Muid Tags=2 tag quality",4,-0.5, 1.5) );
  m_TileMuid2TagQuality[ element ]->GetXaxis()->SetTitle("quality");
    
  m_TileMuid2TagEneQuality.push_back( book2F(m_TrigNames[trig],"tileMuid2TagEneQuality" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal 2D Muid Tags=2 muon energy (MeV) vs. quality",50,0., 10000.,4,-0.5,1.5) );
  m_TileMuid2TagEneQuality[ element ]->GetXaxis()->SetTitle("Energy (MeV)");
  m_TileMuid2TagEneQuality[ element ]->GetYaxis()->SetTitle("Quality");


  return StatusCode::SUCCESS;
}

/// BookHistogram method is called at every event block ,lumi block and run.
/// At the moment we need to book only every run
/// It calls bookHistTrig for the 'AnyTrig' trigger type
/// All concrete trigger type histograms will only be booked when needed
/*---------------------------------------------------------*/
StatusCode TileMuIdMonTool::bookHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_INFO( "in bookHistograms()" );

  cleanHistVec(); //clean vectors before booking
    /*
  if (bookHistTrig(AnyTrig).isFailure()) {
    ATH_MSG_WARNING( "Error booking MuId histograms for Trigger " << m_TrigNames[AnyTrig] );
  }
    */   
  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TileMuIdMonTool::cleanHistVec() {
/*---------------------------------------------------------*/


  for (int i = 0; i < 9; i++) {
    m_activeTrigs[i] = -1;
  }

  m_TileMuidTrig = 0; //Reset index

  m_TileMuidTags.clear();
  m_TileMuidEnergy.clear();
  m_TileMuidEtaPhi.clear();
  m_TileMuidEta.clear();
  m_TileMuidPhi.clear();
  m_TileMuidEneEta.clear();
  m_TileMuidEnePhi.clear();
  m_TileMuidQuality.clear();
  m_TileMuidEneQuality.clear();

  m_TileMuid1TagEnergy.clear();
  m_TileMuid1TagEtaPhi.clear();
  m_TileMuid1TagEta.clear();
  m_TileMuid1TagPhi.clear();
  m_TileMuid1TagQuality.clear();
  m_TileMuid1TagEneQuality.clear();

  m_TileMuid2TagEnergy.clear();
  m_TileMuid2TagEtaPhi.clear();
  m_TileMuid2TagEta.clear();
  m_TileMuid2TagPhi.clear();
  m_TileMuid2TagQuality.clear();
  m_TileMuid2TagEneQuality.clear();

}

/*---------------------------------------------------------*/
StatusCode TileMuIdMonTool::fillHistograms() {
/*---------------------------------------------------------*/


  ATH_MSG_DEBUG( "in fillHistograms()" );

  fillEvtInfo();
  // get Lvl1 Trigger word
  uint32_t lvl1info = getL1info();
  get_eventTrigs(lvl1info); //fill  m_eventTrigs; it always contains at least one element: AnyTrig.

  //check if trigger type is new and therefore some histograms must be booked
  for (unsigned int i = 0; i < m_eventTrigs.size(); i++) {
    if (vecIndx(i) < 0) {
      // book histograms
      if (bookHistTrig(m_eventTrigs[i]).isFailure()) {
        ATH_MSG_WARNING( "Error booking MuId histograms for Trigger " << m_TrigNames[m_eventTrigs[i]] );
      }
    }
  }


  // Pointer to a Tile muid container
  const TileMuContainer* muid_container;

  //Retrieve Muid collection from SG
  CHECK( evtStore()->retrieve(muid_container, m_muidsContName) );

  ATH_MSG_VERBOSE( "Retrieval of Tile muids from container " << m_muidsContName << " succeeded" );

  int nTags= muid_container->size();
  
  for (unsigned int i=0; i< m_eventTrigs.size(); i++ ) {
    m_TileMuidTags[ vecIndx(i) ]->Fill(nTags,1.);
  }

  TileMuContainer::const_iterator iMuid = muid_container->begin();
  TileMuContainer::const_iterator lastMuid  = muid_container->end();
  
  for( ; iMuid != lastMuid; ++iMuid) {

    const TileMu* muid_ptr = *iMuid;    // pointer to muid object

    float energy = muid_ptr->enedep()[0]+ // SampA
                    muid_ptr->enedep()[1]+ // SampB
                    muid_ptr->enedep()[2]; // SampD
                //  muid_ptr->enedep()[3] is the energy AROUND the tower

    float quality = muid_ptr->quality();
    float eta = muid_ptr->eta();
    float phi = muid_ptr->phi();

    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "Number of TileMuId objects: " << nTags << endmsg;
      for (int i = 0; i < 4; i++) {
        msg(MSG::VERBOSE) << "Energy[" << i << "]= " << muid_ptr->enedep()[i] << endmsg;
      }
    }

    if ( (fabs(eta)>2.) || (fabs(phi)>3.2) ) {
      ATH_MSG_WARNING( "Muid information out of Eta - Phi boundaries, skipping this muid:" );
      ATH_MSG_WARNING( "Energy (MeV)=" << energy << "\tEta=" << eta << "\tPhi=" << phi );
      continue;
    }

    for (unsigned int i = 0; i < m_eventTrigs.size(); i++) {

      m_TileMuidEnergy[vecIndx(i)]->Fill(energy, 1.);
      m_TileMuidQuality[vecIndx(i)]->Fill(quality, 1.);
      m_TileMuidEneEta[vecIndx(i)]->Fill(eta, energy);
      m_TileMuidEnePhi[vecIndx(i)]->Fill(phi, energy);
      m_TileMuidEta[vecIndx(i)]->Fill(eta, 1.);
      m_TileMuidPhi[vecIndx(i)]->Fill(phi, 1.);
      m_TileMuidEtaPhi[vecIndx(i)]->Fill(eta, phi, 1.);
      m_TileMuidEneQuality[vecIndx(i)]->Fill(energy, quality, 1.);
    
    
      if (nTags == 1) {

        m_TileMuid1TagEnergy[vecIndx(i)]->Fill(energy, 1.);
        m_TileMuid1TagQuality[vecIndx(i)]->Fill(quality, 1.);
        m_TileMuid1TagEta[vecIndx(i)]->Fill(eta, energy);
        m_TileMuid1TagPhi[vecIndx(i)]->Fill(phi, energy);
        m_TileMuid1TagEtaPhi[vecIndx(i)]->Fill(eta, phi, 1.);
        m_TileMuid1TagEneQuality[vecIndx(i)]->Fill(energy, quality, 1.);

      } else if (nTags == 2) {

        m_TileMuid2TagEnergy[vecIndx(i)]->Fill(energy, 1.);
        m_TileMuid2TagQuality[vecIndx(i)]->Fill(quality, 1.);
        m_TileMuid2TagEta[vecIndx(i)]->Fill(eta, energy);
        m_TileMuid2TagPhi[vecIndx(i)]->Fill(phi, energy);
        m_TileMuid2TagEtaPhi[vecIndx(i)]->Fill(eta, phi, 1.);
        m_TileMuid2TagEneQuality[vecIndx(i)]->Fill(energy, quality, 1.);

      }
    }

    //m_test->Fill(time,energy,1.);
  }
  

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileMuIdMonTool::procHistograms() {
/*---------------------------------------------------------*/

  if (endOfLumiBlockFlag() || endOfRunFlag()) {
    ATH_MSG_INFO( "in procHistograms()" );
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileMuIdMonTool::checkHists(bool /* fromFinalize */) {
/*---------------------------------------------------------*/

  ATH_MSG_INFO( "in checkHists()" );

  return StatusCode::SUCCESS;
}

