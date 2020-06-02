/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>

#include "LUCID_HitsMoniTool.h"

LUCID_HitsMoniTool::LUCID_HitsMoniTool(const std::string& type, const std::string& name, const IInterface* parent): 
  ManagedMonitorToolBase(type, name, parent) 
{
}

LUCID_HitsMoniTool::~LUCID_HitsMoniTool() {}

StatusCode LUCID_HitsMoniTool::initialize() {
  ATH_CHECK( ManagedMonitorToolBase::initialize() );
  ATH_CHECK( m_LUCID_RawDataContainerKey.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode LUCID_HitsMoniTool::bookHistograms() {
  
  ATH_MSG_DEBUG(" LUCID_HitsMoniTool::bookHistograms ");
  
  m_LUCID_Histos.clear();
  
  int nbin = m_nLucidTubes;

  m_LUCID_Histos.push_back(new TH1F("tube_occupancy_cBX", "Tube occupancy (current BX)" , nbin, -0.5, nbin - 0.5));
  m_LUCID_Histos.push_back(new TH1F("tube_occupancy_pBX", "Tube occupancy (previous BX)", nbin, -0.5, nbin - 0.5));
  m_LUCID_Histos.push_back(new TH1F("tube_occupancy_nBX", "Tube occupancy (next BX)"    , nbin, -0.5, nbin - 0.5));

  m_LUCID_Histos.push_back(new TH1F("hitsPMT_cBX_sing_sideA", "Number of hitsPMT on sideA (single side mode, current BX)" , nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsPMT_cBX_sing_sideC", "Number of hitsPMT on sideC (single side mode, current BX)" , nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsPMT_cBX_sing_total", "Number of hitsPMT in total (single side mode, current BX)" , nbin + 1, -0.5, nbin + 0.5));
    
  m_LUCID_Histos.push_back(new TH1F("hitsPMT_cBX_coin_sideA", "Number of hitsPMT on sideA (coincidence mode, current BX)" , nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsPMT_cBX_coin_sideC", "Number of hitsPMT on sideC (coincidence mode, current BX)" , nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsPMT_cBX_coin_total", "Number of hitsPMT in total (coincidence mode, current BX)" , nbin + 1, -0.5, nbin + 0.5));
    
  m_LUCID_Histos.push_back(new TH1F("hitsFIB_cBX_sing_sideA", "Number of hitsFIB on sideA (single side mode, current BX)" , nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsFIB_cBX_sing_sideC", "Number of hitsFIB on sideC (single side mode, current BX)" , nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsFIB_cBX_sing_total", "Number of hitsFIB in total (single side mode, current BX)" , nbin + 1, -0.5, nbin + 0.5));
    
  m_LUCID_Histos.push_back(new TH1F("hitsFIB_cBX_coin_sideA", "Number of hitsFIB on sideA (coincidence mode, current BX)" , nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsFIB_cBX_coin_sideC", "Number of hitsFIB on sideC (coincidence mode, current BX)" , nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsFIB_cBX_coin_total", "Number of hitsFIB in total (coincidence mode, current BX)" , nbin + 1, -0.5, nbin + 0.5));
    
  m_LUCID_Histos.push_back(new TH1F("hitsPMT_pBX_sing_sideA", "Number of hitsPMT on sideA (single side mode, previous BX)", nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsPMT_pBX_sing_sideC", "Number of hitsPMT on sideC (single side mode, previous BX)", nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsPMT_pBX_sing_total", "Number of hitsPMT in total (single side mode, previous BX)", nbin + 1, -0.5, nbin + 0.5));
    
  m_LUCID_Histos.push_back(new TH1F("hitsPMT_pBX_coin_sideA", "Number of hitsPMT on sideA (coincidence mode, previous BX)", nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsPMT_pBX_coin_sideC", "Number of hitsPMT on sideC (coincidence mode, previous BX)", nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsPMT_pBX_coin_total", "Number of hitsPMT in total (coincidence mode, previous BX)", nbin + 1, -0.5, nbin + 0.5));
    
  m_LUCID_Histos.push_back(new TH1F("hitsFIB_pBX_sing_sideA", "Number of hitsFIB on sideA (single side mode, previous BX)", nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsFIB_pBX_sing_sideC", "Number of hitsFIB on sideC (single side mode, previous BX)", nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsFIB_pBX_sing_total", "Number of hitsFIB in total (single side mode, previous BX)", nbin + 1, -0.5, nbin + 0.5));
    
  m_LUCID_Histos.push_back(new TH1F("hitsFIB_pBX_coin_sideA", "Number of hitsFIB on sideA (coincidence mode, previous BX)", nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsFIB_pBX_coin_sideC", "Number of hitsFIB on sideC (coincidence mode, previous BX)", nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsFIB_pBX_coin_total", "Number of hitsFIB in total (coincidence mode, previous BX)", nbin + 1, -0.5, nbin + 0.5));
    
  m_LUCID_Histos.push_back(new TH1F("hitsPMT_nBX_sing_sideA", "Number of hitsPMT on sideA (single side mode, next BX)"    , nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsPMT_nBX_sing_sideC", "Number of hitsPMT on sideC (single side mode, next BX)"    , nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsPMT_nBX_sing_total", "Number of hitsPMT in total (single side mode, next BX)"    , nbin + 1, -0.5, nbin + 0.5));
    
  m_LUCID_Histos.push_back(new TH1F("hitsPMT_nBX_coin_sideA", "Number of hitsPMT on sideA (coincidence mode, next BX)"    , nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsPMT_nBX_coin_sideC", "Number of hitsPMT on sideC (coincidence mode, next BX)"    , nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsPMT_nBX_coin_total", "Number of hitsPMT in total (coincidence mode, next BX)"    , nbin + 1, -0.5, nbin + 0.5));

  m_LUCID_Histos.push_back(new TH1F("hitsFIB_nBX_sing_sideA", "Number of hitsFIB on sideA (single side mode, next BX)"    , nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsFIB_nBX_sing_sideC", "Number of hitsFIB on sideC (single side mode, next BX)"    , nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsFIB_nBX_sing_total", "Number of hitsFIB in total (single side mode, next BX)"    , nbin + 1, -0.5, nbin + 0.5));
    
  m_LUCID_Histos.push_back(new TH1F("hitsFIB_nBX_coin_sideA", "Number of hitsFIB on sideA (coincidence mode, next BX)"    , nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsFIB_nBX_coin_sideC", "Number of hitsFIB on sideC (coincidence mode, next BX)"    , nbin + 1, -0.5, nbin + 0.5));
  m_LUCID_Histos.push_back(new TH1F("hitsFIB_nBX_coin_total", "Number of hitsFIB in total (coincidence mode, next BX)"    , nbin + 1, -0.5, nbin + 0.5));
  
  std::vector<TH1F*>::const_iterator histo_itr = m_LUCID_Histos.begin();
  std::vector<TH1F*>::const_iterator histo_end = m_LUCID_Histos.end();
  
  int hCounter = 0;
  
  for(; histo_itr != histo_end; histo_itr++) { 
       
    if (hCounter < 3) {
      
      (*histo_itr)->GetXaxis()->SetTitle("Tube Number");
      (*histo_itr)->GetYaxis()->SetTitle("Occupancy");
    }
    else (*histo_itr)->GetXaxis()->SetTitle("Number of hits");

    regHist(*histo_itr, "LUCID", run).ignore(); 
    
    hCounter++;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode LUCID_HitsMoniTool::fillHistograms() {

  ATH_MSG_DEBUG(" LUCID_HitsMoniTool::fillHistograms ");

  SG::ReadHandle<LUCID_RawDataContainer> rawdata{m_LUCID_RawDataContainerKey};

  if (!rawdata.isValid()) { ATH_MSG_WARNING(" Could not retireved LUCID_RawDataContainer from StoreGate "); return StatusCode::FAILURE; }
  else                      ATH_MSG_DEBUG(" LUCID_RawDataContainer is retireved from StoreGate ");
  
  for (const auto& LUCID_RawData_itr : *rawdata) {
    
    ATH_MSG_DEBUG(LUCID_RawData_itr);

    for (int bxID=0; bxID<3; bxID++) {
      for (int tub=0; tub<m_nLucidTubes; tub++) {
	
	bool isTubeFired = LUCID_RawData_itr->isTubeFired(tub, bxID);
	
	ATH_MSG_DEBUG(
				     " bxID: " << std::setw(10) << bxID
				  << " tub: "  << std::setw(10) << tub
				  << " fire: " << std::setw(10) << isTubeFired
				  );
	
	if (isTubeFired) m_LUCID_Histos[bxID]->Fill(tub);
      }
    }
    
    int  hitsA, hitsC, hitsT;
    bool coinFlag;
    
    hitsA    = LUCID_RawData_itr->getNhitsPMTsideA();
    hitsC    = LUCID_RawData_itr->getNhitsPMTsideC();
    hitsT    = hitsA + hitsC;
    coinFlag = (hitsA>0 && hitsC>0);
    
    m_LUCID_Histos[3]->Fill(hitsA);
    m_LUCID_Histos[4]->Fill(hitsC);
    m_LUCID_Histos[5]->Fill(hitsT);

    m_LUCID_Histos[6]->Fill(hitsA*coinFlag);
    m_LUCID_Histos[7]->Fill(hitsC*coinFlag);
    m_LUCID_Histos[8]->Fill(hitsT*coinFlag);

    hitsA    = LUCID_RawData_itr->getNhitsFIBsideA();
    hitsC    = LUCID_RawData_itr->getNhitsFIBsideC();
    hitsT    = hitsA + hitsC;
    coinFlag = (hitsA>0 && hitsC>0);
    
    m_LUCID_Histos[ 9]->Fill(hitsA);
    m_LUCID_Histos[10]->Fill(hitsC);
    m_LUCID_Histos[11]->Fill(hitsT);

    m_LUCID_Histos[12]->Fill(hitsA*coinFlag);
    m_LUCID_Histos[13]->Fill(hitsC*coinFlag);
    m_LUCID_Histos[14]->Fill(hitsT*coinFlag);
    
    hitsA    = LUCID_RawData_itr->getNhitsPMTsideAprevBX();
    hitsC    = LUCID_RawData_itr->getNhitsPMTsideCprevBX();
    hitsT    = hitsA + hitsC;
    coinFlag = (hitsA>0 && hitsC>0);
    
    m_LUCID_Histos[15]->Fill(hitsA);
    m_LUCID_Histos[16]->Fill(hitsC);
    m_LUCID_Histos[17]->Fill(hitsT);
    
    m_LUCID_Histos[18]->Fill(hitsA*coinFlag);
    m_LUCID_Histos[19]->Fill(hitsC*coinFlag);
    m_LUCID_Histos[20]->Fill(hitsT*coinFlag);

    hitsA    = LUCID_RawData_itr->getNhitsFIBsideAprevBX();
    hitsC    = LUCID_RawData_itr->getNhitsFIBsideCprevBX();
    hitsT    = hitsA + hitsC;
    coinFlag = (hitsA>0 && hitsC>0);
    
    m_LUCID_Histos[21]->Fill(hitsA);
    m_LUCID_Histos[22]->Fill(hitsC);
    m_LUCID_Histos[23]->Fill(hitsT);
    
    m_LUCID_Histos[24]->Fill(hitsA*coinFlag);
    m_LUCID_Histos[25]->Fill(hitsC*coinFlag);
    m_LUCID_Histos[26]->Fill(hitsT*coinFlag);
    
    hitsA    = LUCID_RawData_itr->getNhitsPMTsideAnextBX();
    hitsC    = LUCID_RawData_itr->getNhitsPMTsideCnextBX();
    hitsT    = hitsA + hitsC;
    coinFlag = (hitsA>0 && hitsC>0);
    
    m_LUCID_Histos[27]->Fill(hitsA);
    m_LUCID_Histos[28]->Fill(hitsC);
    m_LUCID_Histos[29]->Fill(hitsT);
    
    m_LUCID_Histos[30]->Fill(hitsA*coinFlag);
    m_LUCID_Histos[31]->Fill(hitsC*coinFlag);
    m_LUCID_Histos[32]->Fill(hitsT*coinFlag);    

    hitsA    = LUCID_RawData_itr->getNhitsFIBsideAnextBX();
    hitsC    = LUCID_RawData_itr->getNhitsFIBsideCnextBX();
    hitsT    = hitsA + hitsC;
    coinFlag = (hitsA>0 && hitsC>0);
    
    m_LUCID_Histos[33]->Fill(hitsA);
    m_LUCID_Histos[34]->Fill(hitsC);
    m_LUCID_Histos[35]->Fill(hitsT);
    
    m_LUCID_Histos[36]->Fill(hitsA*coinFlag);
    m_LUCID_Histos[37]->Fill(hitsC*coinFlag);
    m_LUCID_Histos[38]->Fill(hitsT*coinFlag);    
  }
  
  return StatusCode::SUCCESS;
}

StatusCode LUCID_HitsMoniTool::procHistograms() {

  ATH_MSG_DEBUG(" LUCID_HitsMoniTool::procHistograms ");

  if (endOfRunFlag()) {
    
    ATH_MSG_DEBUG(" m_nEvents: " << m_nEvents);
    
    for (int bxID=0; bxID<3; bxID++) m_LUCID_Histos[18+bxID]->Scale(1./m_nEvents);
  }
  
  return StatusCode::SUCCESS;
}

