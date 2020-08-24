/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AlignmentBarrelLUT.h"
#include<fstream>

#include "AthenaBaseComps/AthMsgStreamMacros.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::AlignmentBarrelLUT::AlignmentBarrelLUT(const std::string& type,
						     const std::string& name,
						     const IInterface*  parent):
  AthAlgTool(type, name, parent)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::AlignmentBarrelLUT::readLUT(std::string lut_fileName)
{

  int saddress, innerR;
  int N0, N1, N2;
  double A0, A1, A2;
  std::ifstream file;


  for(int i_saddress=0; i_saddress<s_saddress; i_saddress++) {
    for(int i_innerR=0; i_innerR<s_innerR; i_innerR++) {
      m_NbinEta[i_saddress][i_innerR]=0;
      m_EtaMin[i_saddress][i_innerR]=0;
      m_EtaMax[i_saddress][i_innerR]=0;
      m_EtaStep[i_saddress][i_innerR]=0;
      m_NbinPhi[i_saddress][i_innerR]=0;
      m_PhiMin[i_saddress][i_innerR]=0;
      m_PhiMax[i_saddress][i_innerR]=0;
      m_PhiStep[i_saddress][i_innerR]=0;
      
      for(int i_eta=0; i_eta<s_eta; i_eta++) {
	for(int i_phi=0; i_phi<s_phi; i_phi++) {
	  for(int i_etaQ=0; i_etaQ<s_etaQ; i_etaQ++) {
	    m_dZ[i_saddress][i_innerR][i_eta][i_phi][i_etaQ] = 0;
          }
        }
      }
    }
  }
  
  file.open(lut_fileName.c_str());
  if (!file) {
    ATH_MSG_INFO("Failed to open barrel alignment LUT file");
    return StatusCode::FAILURE;
  }

  for(int i_lut=0; i_lut<2; i_lut++) {

    file >> saddress >> innerR;
    file >> m_EtaMin[saddress][innerR] >> m_EtaMax[saddress][innerR]
	 >> m_PhiMin[saddress][innerR] >> m_PhiMax[saddress][innerR]
	 >> m_NbinEta[saddress][innerR] >> m_NbinPhi[saddress][innerR];

    m_EtaStep[saddress][innerR] = (m_EtaMax[saddress][innerR] - m_EtaMin[saddress][innerR]) / (float)m_NbinEta[saddress][innerR];
    m_PhiStep[saddress][innerR] = (m_PhiMax[saddress][innerR] - m_PhiMin[saddress][innerR]) / (float)m_NbinPhi[saddress][innerR];

    for (int i_eta=0; i_eta<s_eta; i_eta++) {
      for (int i_phi=0; i_phi<s_phi; i_phi++) {
	for (int i_etaQ=0; i_etaQ<s_etaQ; i_etaQ++) {

	  file >> N0 >> N1 >> N2 >> A0 >> A1 >> A2;

	  m_dZ[saddress][innerR][i_eta][i_phi][i_etaQ] = A0;

	} // etaQ loop
      } // phi loop
    } // eta loop
  } // nlut loop

  file.close();
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigL2MuonSA::AlignmentBarrelLUT::GetDeltaZ(int&    saddress,
						   double& etaMap,
						   double& phiMap,
						   double& MFphi,
						   float&  sp1R) const
{
  if (saddress == 1) {

    int innerR = (sp1R > 600)? 1: 0;
    
    std::pair<int, int> bins = GetBinNumber(saddress, innerR, etaMap, phiMap);
    int iEta = bins.first;
    int iPhi = bins.second;
    
    int iChamber=( fabs(MFphi)>90*TMath::DegToRad() )  ? 1:0;
    
    int iEta_inv=29-iEta;
    int iPhi_inv=29-iPhi;
    
    int iEta_bin=iEta;
    int iPhi_bin=iPhi;
    
    if( iEta<15 ){
      if( iChamber == 0 ){
	//do nothing
      }else{
	iPhi_bin=iPhi_inv;
      }
    }else{
      if( iChamber == 0 ){
	iEta_bin=iEta_inv;
      }else{
	iEta_bin=iEta_inv;
	iPhi_bin=iPhi_inv;
      }
    }
    
    int sign_etam=(iEta>14)?-1:1;
    int sign_etap=-sign_etam;
    
    return (m_dZ[saddress][innerR][iEta_bin][iPhi_bin][0]*sign_etam
	    + m_dZ[saddress][innerR][iEta_bin][iPhi_bin][1]*sign_etap) / 2.;

  } else {
    ATH_MSG_INFO("Barrel alignment is implemented only for Large-SP");
    return 0;
  }

}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

std::pair<int, int> TrigL2MuonSA::AlignmentBarrelLUT::GetBinNumber(int saddress, int innerR, double etaMap, double phiMap) const
{

  if(saddress > 3 || saddress < 0 || innerR < 0 || innerR > 1)  return std::make_pair(-5,-5);

  int etaBin = (int)((etaMap - m_EtaMin[saddress][innerR])/m_EtaStep[saddress][innerR]);
  int phiBin = (int)((phiMap - m_PhiMin[saddress][innerR])/m_PhiStep[saddress][innerR]);

  if(etaBin <= -1) etaBin = 0;
  if(etaBin >= m_NbinEta[saddress][innerR]) etaBin = m_NbinEta[saddress][innerR] - 1;

  if(phiBin <= -1) phiBin = 0;
  if(phiBin >= m_NbinPhi[saddress][innerR]) phiBin = m_NbinPhi[saddress][innerR] - 1;

  return std::make_pair(etaBin,phiBin);
}
