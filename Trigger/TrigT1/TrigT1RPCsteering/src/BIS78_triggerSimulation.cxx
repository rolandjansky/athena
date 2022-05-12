
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "BIS78_triggerSimulation.h"
#include <iostream>

using namespace std;


/////////////////////////////////////////////////////////////////////////////

BIS78_triggerSimulation::BIS78_triggerSimulation()
{}

unsigned int BIS78_triggerSimulation::AddStrip(int sectoreta, int sectorphi, int igap, int mphi, int strip){
  if (igap<1||igap>3) return false;
  int side = 0;
  if (sectoreta>0) side=1;
  if (mphi==1){ 
    m_strip_phi[side][sectorphi-1][igap-1].push_back(strip);
  } else {
    m_strip_eta[side][sectorphi-1][igap-1].push_back(strip); 
  }

   return 0;
}

StatusCode BIS78_triggerSimulation::clear(){
  
  for (int i=0; i<2; i++){  // 2 sides (0 or 1, just for BIS78, otherwise, for full BIS it should be the eta sector)
    for (int j=0; j<8; j++){  // 8 sectors
      for (int k=0;k<3; k++){  // 3 gas gaps
        m_strip_eta[i][j][k].clear();
        m_strip_phi[i][j][k].clear();
      }
    }
  }
  
  return StatusCode::SUCCESS;
}


void BIS78_triggerSimulation::build_trigRawData(Muon::RpcBis78_TrigRawDataContainer* trgContainer, uint8_t dstrip_phi, uint8_t dstrip_eta, uint16_t bcid)
// take the data from TrigT1RPC
{
  
  uint8_t delta_strip_phi=dstrip_phi;
  uint8_t delta_strip_eta=dstrip_eta;

  for (int i=0; i<2; i++){  // i = side (0,1)
    for (int j=0; j<8; j++){  // j = sector-1 (0,7)
    
      uint16_t side=i;
      uint16_t sector=j;
      auto trgRawData=std::make_unique< Muon::RpcBis78_TrigRawData>(sector, side, bcid);
      
      
      uint32_t trigword = 0;  
      unsigned int majority_eta=0; uint8_t position_eta=0;
      unsigned int majority_phi=0; uint8_t position_phi=0;
      
      // use middle gas gap as pilot
      // SEARCH OF PHI COINCIDENCES
      for (unsigned int istrip1=0; istrip1<m_strip_phi[i][j][1].size(); istrip1++){
      unsigned int majority=0;
        for (unsigned int istrip0=0; istrip0<m_strip_phi[i][j][0].size(); istrip0++){
	  if (std::abs(m_strip_phi[i][j][1].at(istrip1)-m_strip_phi[i][j][0].at(istrip0))<=delta_strip_phi) {
	    majority=1;
	  }
	}
        for (unsigned int istrip2=0; istrip2<m_strip_phi[i][j][2].size(); istrip2++){
	  if (std::abs(m_strip_phi[i][j][1].at(istrip1)-m_strip_phi[i][j][2].at(istrip2))<=delta_strip_phi) {
	    if (majority>0) {
	      majority=2;
	    } else {
	      majority=1;
	    }
	  }
	}
	if (majority > majority_phi) {
	  majority_phi=majority;
	  position_phi=m_strip_phi[i][j][1].at(istrip1);
	}
      }
      if (majority_phi==0) { // try also combination first-last
        for (unsigned int istrip0=0; istrip0<m_strip_phi[i][j][0].size(); istrip0++){  
          for (unsigned int istrip2=0; istrip2<m_strip_phi[i][j][2].size(); istrip2++){ 
	    if (std::abs(m_strip_phi[i][j][2].at(istrip2)-m_strip_phi[i][j][0].at(istrip0))<=delta_strip_phi) {
	      position_phi=m_strip_phi[i][j][0].at(istrip0);
	      majority_phi=1;	  
	    } 
          }
        }
      } // end of phi loop
      // SEARCH OF ETA COINCIDENCES
      for (unsigned int istrip1=0; istrip1<m_strip_eta[i][j][1].size(); istrip1++){
      unsigned int majority=0;
        for (unsigned int istrip0=0; istrip0<m_strip_eta[i][j][0].size(); istrip0++){
	  if (std::abs(m_strip_eta[i][j][1].at(istrip1)-m_strip_eta[i][j][0].at(istrip0))<=delta_strip_eta) {
	    majority=1;
	  }
	}
        for (unsigned int istrip2=0; istrip2<m_strip_eta[i][j][2].size(); istrip2++){
	  if (std::abs(m_strip_eta[i][j][1].at(istrip1)-m_strip_eta[i][j][2].at(istrip2))<=delta_strip_eta) {
	    if (majority>0) {
	      majority=2;
	    } else {
	      majority=1;
	    }
	  }
	}
	if (majority > majority_eta) {
	  majority_eta=majority;
	  position_eta=m_strip_eta[i][j][1].at(istrip1);
	}
      } 
      if (majority_eta==0) { // try also combination first-last
        for (unsigned int istrip0=0; istrip0<m_strip_eta[i][j][0].size(); istrip0++){  
          for (unsigned int istrip2=0; istrip2<m_strip_eta[i][j][2].size(); istrip2++){ 
	    if (std::abs(m_strip_eta[i][j][2].at(istrip2)-m_strip_eta[i][j][0].at(istrip0))<=delta_strip_eta) {
	      position_eta=m_strip_eta[i][j][0].at(istrip0);
	      majority_eta=1;	  
	    } 
          }
        }
      } // end of eta loop
      
      
    // Here one has to compute the trigword for each sector
    // Content of the 24 bits word:
    //   - eta strip index -> 6 bits
    //   - phi strip index -> 6 bits
    //   - eta strip coincidence window -> 3 bits  (NOT IMPLEMENTED YET: TO BE CHECKED WITH FIRMWARE)
    //   - phi strip coincidence window -> 3 bits  (NOT IMPLEMENTED YET: TO BE CHECKED WITH FIRMWARE)
    //   - 3 over 3 gaps coincidence flag for eta/phi-> 2 bits
    //   - reserved -> 4 bits

    
    uint8_t flag3o3_eta=0;
    uint8_t flag3o3_phi=0;
      if (majority_eta>0&&majority_phi>0){
        if (majority_eta>1)  flag3o3_eta=1;
        if (majority_phi>1)  flag3o3_phi=1;
      
        // set the trigger word
        trigword = ((position_eta & 0x3f) << 18)
	  + ((position_phi & 0x3f) << 12 )
	  + ((flag3o3_eta & 0x1)  <<  5 )
	  + ((flag3o3_phi & 0x1)  <<  4 );
      
        auto bis_segment= std::make_unique<Muon::RpcBis78_TrigRawDataSegment>(position_eta,position_phi,
                                                                             delta_strip_eta,delta_strip_phi,
									     flag3o3_eta,flag3o3_phi,trigword );
        
        trgRawData->push_back(std::move(bis_segment));
        trgContainer->push_back(std::move(trgRawData));
      }

    
    }
  }
    
}









