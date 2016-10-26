/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/PtBarrelLUT.h"

#include<fstream>

#include "AthenaBaseComps/AthMsgStreamMacros.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_PtBarrelLUT("IID_PtBarrelLUT", 1, 0);

const InterfaceID& TrigL2MuonSA::PtBarrelLUT::interfaceID() { return IID_PtBarrelLUT; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::PtBarrelLUT::PtBarrelLUT(const std::string& type,
				       const std::string& name,
				       const IInterface*  parent):
  AthAlgTool(type, name, parent)
{
  declareInterface<TrigL2MuonSA::PtBarrelLUT>(this);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::PtBarrelLUT::~PtBarrelLUT()
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtBarrelLUT::initialize()
{
  ATH_MSG_DEBUG("Initializing PtBarrelLUT - package version " << PACKAGE_VERSION) ;
   
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

StatusCode TrigL2MuonSA::PtBarrelLUT::readLUT(std::string lut_fileName,
					      std::string lutSP_fileName)
{
  int charge,chamber;
  float A0,A1,etamin,etamax,phimin,phimax;
  std::ifstream file;
  
  for (int i_saddress=0; i_saddress<4; i_saddress++) {
    for(int i_charge=0; i_charge<2; i_charge++) {
      for(int i_eta=0; i_eta<60; i_eta++) {
	for(int i_phi=0; i_phi<60; i_phi++) {
	  for(int i_parms=0; i_parms<2; i_parms++) {
	    m_lut.table[i_saddress][i_charge][i_eta][i_phi][i_parms] = 0.;
	  }
	}
      }
    }
  }

  for(int i_innerR=0; i_innerR<2; i_innerR++) {
    for(int i_charge=0; i_charge<2; i_charge++) {
      for(int i_eta=0; i_eta<30; i_eta++) {
	for(int i_phi=0; i_phi<30; i_phi++) {
	  for(int i_parms=0; i_parms<2; i_parms++) {
	    m_lutSP.table_LargeSP[i_innerR][i_charge][i_eta][i_phi][i_parms] = 0;
	  }
	}
      }
    }
  }

  file.open(lut_fileName.c_str());
  if(!file) {
    ATH_MSG_INFO("Failed to open barrel LUT file");
    return StatusCode::FAILURE;
  }
  
  for(int nlut=0;nlut<8;++nlut) {
    file >> charge >> chamber;
    file >> etamin >> etamax >> phimin >> phimax
	 >> m_lut.NbinEta[chamber] >> m_lut.NbinPhi[chamber];
    m_lut.EtaMin[chamber] = etamin;
    m_lut.EtaMax[chamber] = etamax;
    m_lut.PhiMin[chamber] = phimin;
    m_lut.PhiMax[chamber] = phimax;
    m_lut.PhiStep[chamber] = (phimax-phimin)/m_lut.NbinPhi[chamber];
    m_lut.EtaStep[chamber] = (etamax-etamin)/m_lut.NbinEta[chamber];
    
    for(int i=0;i<m_lut.NbinEta[chamber];i++) {
      for(int j=0;j<m_lut.NbinPhi[chamber];j++) {

	file >> A1 >> A0;

	m_lut.table[chamber][charge][i][j][0] = A1;
	m_lut.table[chamber][charge][i][j][1] = A0; 
      }
    }
  }	
  
  file.close();

  std::ifstream fileSP;
  fileSP.open(lutSP_fileName.c_str());
  if(!fileSP) {
    ATH_MSG_INFO("Failed to open barrel LUT file");
    return StatusCode::FAILURE;
  }

  int ieta, iphi, iqeta, iR;
  while( !fileSP.eof() ){
    fileSP >> iqeta >> iR >> ieta >> iphi >> A1 >> A0;
    m_lutSP.table_LargeSP[iqeta][iR][ieta][iphi][0] = A1;
    m_lutSP.table_LargeSP[iqeta][iR][ieta][iphi][1] = A0;
  }

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtBarrelLUT::finalize()
{
  ATH_MSG_DEBUG("Finalizing TgcRoadDefiner - package version " << PACKAGE_VERSION);
   
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

