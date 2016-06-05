/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/AlignmentBarrelLUT.h"
#include<fstream>

#include "AthenaBaseComps/AthMsgStreamMacros.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_AlignmentBarrelLUT("IID_AlignmentBarrelLUT", 1, 0);

const InterfaceID& TrigL2MuonSA::AlignmentBarrelLUT::interfaceID() { return IID_AlignmentBarrelLUT; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::AlignmentBarrelLUT::AlignmentBarrelLUT(const std::string& type,
						     const std::string& name,
						     const IInterface*  parent):
  AthAlgTool(type, name, parent)
{
  declareInterface<TrigL2MuonSA::AlignmentBarrelLUT>(this);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::AlignmentBarrelLUT::~AlignmentBarrelLUT()
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::AlignmentBarrelLUT::initialize()
{
  ATH_MSG_DEBUG("Initializing AlignmentBarrelLUT - package version " << PACKAGE_VERSION) ;
   
  StatusCode sc;
  sc = AthAlgTool::initialize();
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR("Could not initialize the AthAlgTool base class.");
    return sc;
  }

  // 
  return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::AlignmentBarrelLUT::readLUT(std::string lut_fileName)
{

  int saddress, innerR;
  int N0, N1, N2;
  double A0, A1, A2;
  std::ifstream file;


  for(int i_saddress=0; i_saddress<4; i_saddress++) {
    for(int i_innerR=0; i_innerR<2; i_innerR++) {
      NbinEta[i_saddress][i_innerR]=0;
      EtaMin[i_saddress][i_innerR]=0;
      EtaMax[i_saddress][i_innerR]=0;
      EtaStep[i_saddress][i_innerR]=0;
      NbinPhi[i_saddress][i_innerR]=0;
      PhiMin[i_saddress][i_innerR]=0;
      PhiMax[i_saddress][i_innerR]=0;
      PhiStep[i_saddress][i_innerR]=0;
      
      for(int i_eta=0; i_eta<15; i_eta++) {
	for(int i_phi=0; i_phi<30; i_phi++) {
	  for(int i_etaQ=0; i_etaQ<2; i_etaQ++) {
	    dZ[i_saddress][i_innerR][i_eta][i_phi][i_etaQ] = 0;
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
    file >> EtaMin[saddress][innerR] >> EtaMax[saddress][innerR]
	 >> PhiMin[saddress][innerR] >> PhiMax[saddress][innerR]
	 >> NbinEta[saddress][innerR] >> NbinPhi[saddress][innerR];

    EtaStep[saddress][innerR] = (EtaMax[saddress][innerR] - EtaMin[saddress][innerR]) / (float)NbinEta[saddress][innerR];
    PhiStep[saddress][innerR] = (PhiMax[saddress][innerR] - PhiMin[saddress][innerR]) / (float)NbinPhi[saddress][innerR];

    for (int i_eta=0; i_eta<15; i_eta++) {
      for (int i_phi=0; i_phi<30; i_phi++) {
	for (int i_etaQ=0; i_etaQ<2; i_etaQ++) {

	  file >> N0 >> N1 >> N2 >> A0 >> A1 >> A2;

	  dZ[saddress][innerR][i_eta][i_phi][i_etaQ] = A0;

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
    
    return (dZ[saddress][innerR][iEta_bin][iPhi_bin][0]*sign_etam
	    + dZ[saddress][innerR][iEta_bin][iPhi_bin][1]*sign_etap) / 2.;

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

  int etaBin = (int)((etaMap - EtaMin[saddress][innerR])/EtaStep[saddress][innerR]);
  int phiBin = (int)((phiMap - PhiMin[saddress][innerR])/PhiStep[saddress][innerR]);

  if(etaBin <= -1) etaBin = 0;
  if(etaBin >= NbinEta[saddress][innerR]) etaBin = NbinEta[saddress][innerR] - 1;

  if(phiBin <= -1) phiBin = 0;
  if(phiBin >= NbinPhi[saddress][innerR]) phiBin = NbinPhi[saddress][innerR] - 1;

  return std::make_pair(etaBin,phiBin);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::AlignmentBarrelLUT::finalize()
{
  ATH_MSG_DEBUG("Finalizing AlignmentBarrelLUT - package version " << PACKAGE_VERSION);
   
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
