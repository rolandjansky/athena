/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileL2MonTool.cxx
// PACKAGE:  TileMonitoring  
//
// AUTHORS:   Matteo Volpi (matteo.volpi@cern.ch)
//            Luca Fiorini (Luca.Fiorini@cern.ch) 
//         
// ********************************************************************

#include "TileL2MonTool.h"

#include "TileEvent/TileMuContainer.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"


/*---------------------------------------------------------*/
TileL2MonTool::TileL2MonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_TileL2Trig(0)
  , m_ntag(0)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("L2MuonContainerName",m_TileL2ContName="TileL2Cnt"); //SG L2 Container
  declareProperty("muidsContainerName",m_muidsContName="TileMuObj"); //SG Muid Container
  m_path = "/Tile/L2Muon"; //ROOT File directory


}

/*---------------------------------------------------------*/
TileL2MonTool::~TileL2MonTool() {
/*---------------------------------------------------------*/

}

/*---------------------------------------------------------*/
StatusCode TileL2MonTool:: initialize() {
/*---------------------------------------------------------*/

  ATH_MSG_INFO( "in initialize()" );

  return TileFatherMonTool::initialize();
}

/*---------------------------------------------------------*/
StatusCode TileL2MonTool::bookHistTrig( int trig ) {
/*---------------------------------------------------------*/


  m_activeTrigs[ trig ] = m_TileL2Trig; 
  int element = m_activeTrigs[ trig ];
  
  std::string runNumStr = getRunNumStr();

  m_TileL2Trig++; //Increment trigger index

  /// L2 plots
  m_TileL2Tags.push_back( book1F(m_TrigNames[trig],"tileL2Tags" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal L2 number of tags",10,0., 10.) );
  
  m_TileL2Energy.push_back( book1F(m_TrigNames[trig],"tileL2Energy" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal L2 Energy dep (MeV)",100,0., 10000.) );
  m_TileL2Energy[ element ]->GetXaxis()->SetTitle("L2 Energy (MeV)");
  
  m_TileL2EtaPhi.push_back( book2F(m_TrigNames[trig],"tileL2EtaPhi" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal 2D L2 muon position in eta vs phi plane",40,-2.0, 2.0,64,-3.15,3.15) );
  m_TileL2EtaPhi[ element ]->GetXaxis()->SetTitle("#eta");
  m_TileL2EtaPhi[ element ]->GetYaxis()->SetTitle("#phi");
    
  m_TileL2Eta.push_back( book1F(m_TrigNames[trig],"tileL2Eta" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal L2 muon #eta position",40,-2.0, 2.0) );
  m_TileL2Eta[ element ]->GetXaxis()->SetTitle("#eta");
  
  m_TileL2Phi.push_back( book1F(m_TrigNames[trig],"tileL2Phi" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal L2 muon #phi position",64,-3.15, 3.15) );
  m_TileL2Phi[ element ]->GetXaxis()->SetTitle("#phi");

  m_TileL2EneEta.push_back( bookProfile(m_TrigNames[trig],"tileL2EneEta" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal L2 average muon energy as a function of #eta position",40,-2.0, 2.0, -2.e6, 2.e6) );
  m_TileL2EneEta[ element ]->GetXaxis()->SetTitle("#eta");
  
  m_TileL2EnePhi.push_back( bookProfile(m_TrigNames[trig],"tileL2EnePhi" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal L2 muon average energy as a fucntion of #phi position",64,-3.15, 3.15, -2.e6, 2.e6) );
  m_TileL2EnePhi[ element ]->GetXaxis()->SetTitle("#phi");
  
  m_TileL2Quality.push_back( book1F(m_TrigNames[trig],"tileL2Quality" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal L2 tag quality",4,-0.5, 1.5) );
  m_TileL2Quality[ element ]->GetXaxis()->SetTitle("quality");
  
  m_TileL2EneQuality.push_back( book2F(m_TrigNames[trig],"tileL2EneQuality" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal 2D L2 muon energy (MeV) vs. quality",50,0., 10000.,4,-0.5,1.5) );
  m_TileL2EneQuality[ element ]->GetXaxis()->SetTitle("Energy (MeV)");
  m_TileL2EneQuality[ element ]->GetYaxis()->SetTitle("Quality");

  /////// Confront Muid to L2
  
  m_TileL2DeltaEnergy.push_back( book1F(m_TrigNames[trig],"tileL2DeltaEnergy" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal relative energy difference between Muid and L2Mu algos",100,-1.01, 10.01) );
  m_TileL2DeltaEnergy[ element ]->GetXaxis()->SetTitle("(E_{L2}-E_{MuId})/E_{MuId}");

   m_TileL2TagMuidTag.push_back( book2F(m_TrigNames[trig],"tileL2TagMuidTag" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": number of L2 Tags vs number if Muid Tags",5,0.,5.,5,0.,5.) );
  m_TileL2TagMuidTag[ element ]->GetXaxis()->SetTitle("#Tag Muid");
  m_TileL2TagMuidTag[ element ]->GetYaxis()->SetTitle("#Tag L2");

  m_TileL2EnergyMap.push_back( book2F(m_TrigNames[trig],"tileL2EnergyMap" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": Energy difference in MeV between L2Mu and Muid algos in (eta,phi) Map",40,-2.0,2.0,64,-3.15,3.15) );
  m_TileL2EnergyMap[ element ]->GetXaxis()->SetTitle("#eta");
  m_TileL2EnergyMap[ element ]->GetYaxis()->SetTitle("#phi");  

  m_TileL2RelativeEne.push_back( book2F(m_TrigNames[trig],"tileL2RelativeEne" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": Realtive Energy difference between L2Mu and Muid algos",100,0.,7000.,100,-1,1) );
  m_TileL2RelativeEne[ element ]->GetXaxis()->SetTitle("E_{Muid}");
  m_TileL2RelativeEne[ element ]->GetYaxis()->SetTitle("(E_{L2}-E_{MuId})/E_{MuId}");  

  return StatusCode::SUCCESS;
}

/// BookHistogram method is called at every event block ,lumi block and run.
/// At the moment we need to book only every run
/// It calls bookHistTrig for the 'AnyTrig' trigger type
/// All concrete trigger type histograms will only be booked when needed
/*---------------------------------------------------------*/
StatusCode TileL2MonTool::bookHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_INFO( "in bookHistograms()" );

  cleanHistVec(); //clean vectors before booking
    /*

  if (bookHistTrig(AnyTrig).isFailure()) {
    ATH_MSG_WARNING( "Error booking L2 histograms for Trigger " << m_TrigNames[AnyTrig] );
  }
    */   
  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TileL2MonTool::cleanHistVec() {
/*---------------------------------------------------------*/


  for (int i = 0; i < 9; i++) {
    m_activeTrigs[i] = -1;
  }

  m_TileL2Trig = 0; //reset Trigger index 

  m_TileL2Tags.clear();
  m_TileL2Energy.clear();
  m_TileL2EtaPhi.clear();
  m_TileL2Eta.clear();
  m_TileL2Phi.clear();
  m_TileL2EneEta.clear();
  m_TileL2EnePhi.clear();
  m_TileL2Quality.clear();
  m_TileL2EneQuality.clear();
  // confront L2 with MuId
  m_TileL2DeltaEnergy.clear();
  m_TileL2TagMuidTag.clear();
  m_TileL2EnergyMap.clear();
  m_TileL2RelativeEne.clear();
}


/////////////////////////////////////////////
StatusCode TileL2MonTool::fillHistograms() {
/////////////////////////////////////////////

  /// Level 1 inforamtion for the type of trigger

  fillEvtInfo();
  // get Lvl1 Trigger word
  uint32_t lvl1info = getL1info();
  get_eventTrigs(lvl1info); //fill  m_eventTrigs; it always contains at least one element: AnyTrig.

  //check if trigger type is new and therefore some histograms must be booked
  for (unsigned int i = 0; i < m_eventTrigs.size(); i++) {
    if (vecIndx(i) < 0) {
      // book histograms
      if (bookHistTrig(m_eventTrigs[i]).isFailure()) {
        ATH_MSG_WARNING( "Error booking L2 histograms for Trigger " << m_TrigNames[m_eventTrigs[i]] );
      }
    }
  }

  // step1: read from TES (L2)

  const TileL2Container* mutags_ROD;
  const TileMuContainer* muid_container;  // Pointer to a Tile muid container

  if (!evtStore()->contains<TileL2Container>(m_TileL2ContName)) {
    static int cnt1 = 0;
    if (cnt1 < 5) {
      ++cnt1;
      ATH_MSG_WARNING( "Can't find TileL2Containter with name " << m_TileL2ContName << " in TES" );

      if (cnt1 == 5)
        ATH_MSG_WARNING( "This warning will not be printed anymore" );
    }
    return StatusCode::SUCCESS;
  }

  if (evtStore()->retrieve(mutags_ROD, m_TileL2ContName).isFailure()) {
    ATH_MSG_WARNING( "Can't retrieve " << m_TileL2ContName << " from TES" );
    return StatusCode::SUCCESS;
  }
  
  if (!evtStore()->contains<TileMuContainer>(m_muidsContName)) {
    static int cnt2 = 0;
    if (cnt2 < 5) {
      ++cnt2;
      ATH_MSG_WARNING( "Can't find TileMuContainer with name " << m_muidsContName << " in TES" );

      if (cnt2 == 5)
        ATH_MSG_WARNING( "This warning will not be printed anymore" );
    }

    return StatusCode::SUCCESS;
  }
  
  if (evtStore()->retrieve(muid_container, m_muidsContName).isFailure()) {
    ATH_MSG_WARNING( "Can't retrieve " << m_muidsContName << " from TES" );

    return StatusCode::SUCCESS;
  }

  // nTags for Muid
  int MuidnTags = muid_container->size();

  TileL2Container::const_iterator it = mutags_ROD->begin();
  TileL2Container::const_iterator end = mutags_ROD->end();

  TileMuContainer::const_iterator iMuid = muid_container->begin();
  TileMuContainer::const_iterator lastMuid = muid_container->end();

  m_ntag = 0; // nTag for L2
  Float_t ene, et, ph, qua;
  //Int_t Nmuon;

  std::vector<float> L2_eta;
  std::vector<float> L2_phi;
  std::vector<float> L2_ene;

  std::vector<float> Muid_eta;
  std::vector<float> Muid_phi;
  std::vector<float> Muid_ene;

  for (; it != end; ++it) {
    ene = 0;
    et = 0;
    ph = 0;
    qua = 0;
    //Nmuon=0;

    for (unsigned int a = 0; a < ((*it)->NMuons()); ++a) // if there are more one muon
        {
      //Nmuon=(*it)->NMuons();
      ene = 0;
      et = 0;
      ph = 0;
      qua = 0;
      et = (*it)->eta(a);
      L2_eta.push_back((*it)->eta(a));
      ph = (*it)->phi(a);
      L2_phi.push_back((*it)->phi(a));
      ene = (*it)->enemu0(a) + (*it)->enemu1(a) + (*it)->enemu2(a);
      L2_ene.push_back((*it)->enemu0(a) + (*it)->enemu1(a) + (*it)->enemu2(a));
      qua = (*it)->qual(a);

      // Fill HISTROGRAMS
      for (unsigned int i = 0; i < m_eventTrigs.size(); i++) {

        m_TileL2Energy[vecIndx(i)]->Fill(ene, 1.);
        m_TileL2Quality[vecIndx(i)]->Fill(qua, 1.);
        m_TileL2Eta[vecIndx(i)]->Fill(et, 1.);
        m_TileL2Phi[vecIndx(i)]->Fill(ph, 1.);
        m_TileL2EneEta[vecIndx(i)]->Fill(et, ene);
        m_TileL2EnePhi[vecIndx(i)]->Fill(ph, ene);
        m_TileL2EtaPhi[vecIndx(i)]->Fill(et, ph, 1.);
        m_TileL2EneQuality[vecIndx(i)]->Fill(ene, qua, 1.);

      }
      m_ntag++;
    }
  }

  /// loop ntags for L2
  for (unsigned int i = 0; i < m_eventTrigs.size(); i++) {
    m_TileL2Tags[vecIndx(i)]->Fill(m_ntag, 1.);
  }

  //// loop Muid

  for (; iMuid != lastMuid; ++iMuid) {
    const TileMu* muid_ptr = *iMuid;    // pointer to muid object

    float energy = muid_ptr->enedep()[0] + muid_ptr->enedep()[1] + muid_ptr->enedep()[2]; // SampA+B+D

    Muid_ene.push_back(energy);

    float eta = muid_ptr->eta();

    Muid_eta.push_back(eta);

    float phi = muid_ptr->phi();
    Muid_phi.push_back(phi);

   if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "Number of TileMuId objects: " << MuidnTags << endmsg;
      for (int i = 0; i < 4; i++) {
        msg(MSG::VERBOSE) << "Energy[" << i << "]= " << muid_ptr->enedep()[i] << endmsg;
      }
    }

    if ((fabs(eta) > 2.) || (fabs(phi) > 3.2)) {
      ATH_MSG_WARNING( "Muid information out of Eta - Phi boundaries, skipping this muid:" );
      ATH_MSG_WARNING( "Energy (MeV)=" << energy << "\tEta=" << eta << "\tPhi=" << phi );
      continue;
    }
  }
  
  // loop difference
  
  // #tag difference
  for (unsigned int i = 0; i < m_eventTrigs.size(); i++) {

    m_TileL2TagMuidTag[vecIndx(i)]->Fill(MuidnTags, m_ntag, 1.);
  }

  Float_t diffRel;
  Float_t diff;
  diffRel = 0;
  diff = 0;
  for (int i = 0; i < MuidnTags; i++) {
    for (int j = 0; j < m_ntag; j++) {
      if (fabs(L2_eta[j] - Muid_eta[i]) < 0.1 && fabs(L2_phi[j] - Muid_phi[i]) < 0.1) {
        diffRel = (L2_ene[j] - Muid_ene[i]) / Muid_ene[i];
        diff = (L2_ene[j] - Muid_ene[i]);
        for (unsigned int x = 0; x < m_eventTrigs.size(); x++) {
          m_TileL2DeltaEnergy[vecIndx(x)]->Fill(diffRel, 1.);
          m_TileL2EnergyMap[vecIndx(x)]->Fill(Muid_eta[i], Muid_phi[i], diff);
          m_TileL2RelativeEne[vecIndx(x)]->Fill(Muid_ene[i], diffRel);

        }
      }
    }
  }

  return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/
StatusCode TileL2MonTool::procHistograms() {
/*---------------------------------------------------------*/

  if (endOfLumiBlockFlag() || endOfRunFlag()) {
    ATH_MSG_INFO( "in procHistograms()" );
  }
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileL2MonTool::checkHists(bool /* fromFinalize */) {
/*---------------------------------------------------------*/

  ATH_MSG_INFO(  "in checkHists()" );
  return StatusCode::SUCCESS;
}

