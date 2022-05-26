/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//		jFEXForwardElecAlgo - Algorithm for Forward Electron Algorithm in jFEX
//                              -------------------
//     begin                : 16 11 2021
//     email                : Sergi.Rodriguez@cern.ch
//     email                : ulla.blumenschein@cern.ch     
//***************************************************************************

#include <iostream>
#include <vector>
#include <stdio.h>
#include <math.h>
#include "L1CaloFEXSim/jFEXForwardElecAlgo.h"
#include "L1CaloFEXSim/jFEXForwardElecTOB.h"
#include "L1CaloFEXSim/jFEXForwardElecInfo.h"
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

namespace LVL1 {

  //Default Constructor
  LVL1::jFEXForwardElecAlgo::jFEXForwardElecAlgo(const std::string& type, const std::string& name, const IInterface* parent): AthAlgTool(type, name, parent) {
    declareInterface<IjFEXForwardElecAlgo>(this);
  }
  
  /** Destructor */
  LVL1::jFEXForwardElecAlgo::~jFEXForwardElecAlgo() {
  }
  
  
  StatusCode LVL1::jFEXForwardElecAlgo::initialize() {
    ATH_CHECK(m_jTowerContainerKey.initialize());
    return StatusCode::SUCCESS;
  }
  
  //calls container for TT
  StatusCode LVL1::jFEXForwardElecAlgo::safetyTest() {
    
    m_jTowerContainer = SG::ReadHandle<jTowerContainer>(m_jTowerContainerKey);
    if(! m_jTowerContainer.isValid()) {
      ATH_MSG_FATAL("Could not retrieve jTowerContainer " << m_jTowerContainerKey.key());
      return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
  }

  StatusCode LVL1::jFEXForwardElecAlgo::reset() {
    return StatusCode::SUCCESS;
  }
  
  
  void LVL1::jFEXForwardElecAlgo::setup(int inputTable[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width], int jfex, int fpga) {
    std::copy(&inputTable[0][0], &inputTable[0][0] + (FEXAlgoSpaceDefs::jFEX_algoSpace_height*FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width), &m_jFEXalgoTowerID[0][0]);
    m_jfex=jfex;
    m_fpga=fpga;

  }

  
  //global centre Eta and Phi coord of the TT
  std::array<float,2> LVL1::jFEXForwardElecAlgo::getEtaPhi(uint TTID) {
    if(TTID == 0) {
      return {999,999};
    }
    const LVL1::jTower *tmpTower = m_jTowerContainer->findTower(TTID);
    return {tmpTower->centreEta(),tmpTower->centrePhi()};
  }

  std::array<uint,2> LVL1::jFEXForwardElecAlgo::getEtEmHad(uint TTID) {
    if(TTID == 0) {
      return {0,0};
    }
    uint TT_EtEM = 0;
    if(m_map_Etvalues_EM.find(TTID) != m_map_Etvalues_EM.end()) {
      TT_EtEM = m_map_Etvalues_EM[TTID][0];
    }
    uint TT_EtHad = 0;
    if(m_map_Etvalues_HAD.find(TTID) != m_map_Etvalues_HAD.end()) {
      TT_EtHad = m_map_Etvalues_HAD[TTID][0];
    }

    return {TT_EtEM, TT_EtHad};
  }


  std::unordered_map<uint, jFEXForwardElecInfo> LVL1::jFEXForwardElecAlgo::LocalMaxima() {
    
    std::unordered_map<uint, jFEXForwardElecInfo> localMaximaCandidates = ClusterTTIDLists();
    std::unordered_map<uint, jFEXForwardElecInfo> localMaximaList ;
    
    for (auto& [myTTKey,elCluster] : localMaximaCandidates){
      //Local maxima check takes place here                                                                                                        
      bool isLocalMaximum = 1;
      float centre_phi = elCluster.getCoreTTfPhi();
      float centre_eta = elCluster.getCoreTTfEta();
      uint centre_EtEM = elCluster.getCoreTTEtEM();
      
      const std::vector<uint> TTinSW = elCluster.getTTinSearchWindow();
      
      for (const uint iTTinSW : TTinSW) {
	if(iTTinSW == myTTKey) continue;
	auto [seed_eta,seed_phi] = getEtaPhi(iTTinSW);
        auto [seed_EtEM,seed_EtHad] = getEtEmHad(iTTinSW);

	//Correction for 2PI transition:  FPGA 0 with FPGA 3 
	if(m_fpga==0 || m_fpga==3) {
	  if(m_fpga==0) {
	    if(seed_phi>M_PI){
	      seed_phi = seed_phi-m_2PI;
	    }
	  }
	  else {
	    if(seed_phi<M_PI){
	      seed_phi = seed_phi+m_2PI;
	    }
	  }
	}
	//if (dphi > 0.5)  dphi = dphi - 2.*M_PI; // correct for 2PI border
	//if (dphi < -0.5) dphi = dphi + 2.*M_PI; // correct for 2PI border

	int delta_phi = std::round((seed_phi - centre_phi)*100);
	int delta_eta = std::round((seed_eta - centre_eta)*100);
	
	if( (delta_eta + delta_phi) < 0 || ((delta_eta + delta_phi) == 0 && delta_eta < 0) ) {
	  if(centre_EtEM < seed_EtEM) {
	    isLocalMaximum = false;
	  }
	} else{
	  if(centre_EtEM <= seed_EtEM) {
	    isLocalMaximum = false;
	  }
	}
      }

      //if it is a local maxima, we save the TT ID                                                                                                 
      if(isLocalMaximum) {
	localMaximaList[myTTKey] = elCluster;
      }
    }
    return localMaximaList;
  }


  std::unordered_map<uint, jFEXForwardElecInfo> LVL1::jFEXForwardElecAlgo::calculateEDM() {
    // setting the lower/upper eta range for the FCAL 2 and 3 since they are not added in the seed information yet 
    int lowerFCAL_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_C_lowerFCAL_eta;
    int upperFCAL_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_C_upperFCAL_eta;
    int lowerFCAL2_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_C_lowerFCAL2_eta;
    int upperFCAL2_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_C_upperFCAL_eta;
    if(m_jfex == 5) {
      //Module 5                                                                                                                                   
      lowerFCAL_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_A_lowerFCAL_eta;
      upperFCAL_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_A_upperFCAL_eta;
      lowerFCAL2_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_A_lowerFCAL_eta;
      upperFCAL2_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_A_upperFCAL2_eta;
    }
    // Retrieve FCAl1 clusters          
    std::unordered_map<uint, jFEXForwardElecInfo> mlocalMaxima = LocalMaxima();
    for(auto& [myTTKey,elCluster] : mlocalMaxima) {
      float centreTT_phi = elCluster.getCoreTTfPhi();
      float centreTT_eta = elCluster.getCoreTTfEta();
      if(fabs(centreTT_eta)<3.2) continue;// only FCAL clusters

      // Adding the closest FCAL 2 and 3 TT to the hadronic energy  
      float deltaRminl2 = 999, deltaRminl2b = 999, deltaRminl2c = 999, deltaRminl3 = 999;
      uint TTmin2 = 0, TTmin2b = 0, TTmin2c = 0, TTmin3 = 0;
      for(int nphi = 0; nphi < 8; nphi++) {
	for(int neta = lowerFCAL_eta; neta < upperFCAL_eta; neta++) {
	  int auxTTID = m_jFEXalgoTowerID[nphi][neta];
	  auto [ TT_eta,TT_phi ] = getEtaPhi(auxTTID);
	  // corect for transition over 2PI
	  if(m_fpga==0 || m_fpga==3) {
	    if(m_fpga==0) {
	      if(TT_phi>M_PI){
		TT_phi = TT_phi-m_2PI;
	      }
	    }
	    else {
	      if(TT_phi<M_PI){
		TT_phi = TT_phi+m_2PI;
	      }
	    }
	  }

	  // Search for hadronic SC closest in DR
	  int DeltaR = std::round( (std::pow((centreTT_eta - TT_eta),2) + std::pow((centreTT_phi - TT_phi),2)) * 1e5   );
	  if ( DeltaR < m_Edge_dR4 ) {
	    if (neta> lowerFCAL2_eta-1 && neta < upperFCAL2_eta){
	      // EtHad1, FCAL second layer 
	      if( DeltaR < deltaRminl2){
		deltaRminl2c  = deltaRminl2b;
		deltaRminl2b  = deltaRminl2;
		deltaRminl2   = DeltaR;
		TTmin2c = TTmin2b;
		TTmin2b = TTmin2;
		TTmin2 = auxTTID;
	      }else if ( DeltaR < deltaRminl2b){
                deltaRminl2c  = deltaRminl2b;
		deltaRminl2b  = DeltaR;
                TTmin2c = TTmin2b;
                TTmin2b = auxTTID;
	      } else if ( DeltaR < deltaRminl2c){
                deltaRminl2c  = DeltaR;
		TTmin2c = auxTTID; 
	      }
	    }else{
	      // EtHad2, FCAL 3rd layer
	      if( DeltaR < deltaRminl3){
		deltaRminl3   = DeltaR;
		TTmin3 = auxTTID;
	      }
	    }
	  }// search cone
	}
      }//search end


      //EHad1
      auto [TT_EtEM2, TT_EtHad2] = getEtEmHad(TTmin2);
      elCluster.setTTEtHad1(uint(TT_EtHad2));
      // special treatment for ieta = 22, 2nd cell in FCAL1, eta ~3.2
      if(elCluster.getCoreTTiEta() == FEXAlgoSpaceDefs::jFEX_algoSpace_FCAL1_2nd){ 
	auto [TT_EtEM2b, TT_EtHad2b] = getEtEmHad(TTmin2b);

	elCluster.addTTEtHad1(uint(TT_EtHad2b));
	if((centreTT_phi> 0.9 && centreTT_phi<1.1) || (centreTT_phi> 4.1 && centreTT_phi<4.3)){
	  auto [TT_EtEM2c, TT_EtHad2c] = getEtEmHad(TTmin2c);
	  elCluster.addTTEtHad1(uint(TT_EtHad2c));
	} 
      }//special cases

      //EtHad2
      auto [TT_EtEM3, TT_EtHad3] = getEtEmHad(TTmin3);
      elCluster.setTTEtHad2(TT_EtHad3);

    }// loop ver local maxima

    //Returning the complite information
    return mlocalMaxima;
  }
  
  
  void LVL1::jFEXForwardElecAlgo::setFPGAEnergy(std::unordered_map<int,std::vector<int> > etmapEM,std::unordered_map<int,std::vector<int> > etmapHAD) {
    m_map_Etvalues_EM=etmapEM; 
    m_map_Etvalues_HAD=etmapHAD;
  }

  
  std::unordered_map<uint, LVL1::jFEXForwardElecInfo> LVL1::jFEXForwardElecAlgo::ClusterTTIDLists() {

    std::unordered_map<uint, LVL1::jFEXForwardElecInfo> ClusTowerIDLists;

   std::vector<int> lower_centre_neta;
   std::vector<int> upper_centre_neta;
   m_lowerEM_eta = 0;
   m_upperEM_eta = 0;
   
   //STEP 1: check if we are in module 0 or 5 and assign corrrect eta FEXAlgoSpace parameters
    if(m_jfex == 0) {
      //Module 0 
      lower_centre_neta.assign({FEXAlgoSpaceDefs::jFEX_algoSpace_C_EMB_start_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_C_EMIE_start_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_C_FCAL_start_eta});
      upper_centre_neta.assign({FEXAlgoSpaceDefs::jFEX_algoSpace_C_EMB_end_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_C_EMIE_end_eta,FEXAlgoSpaceDefs::jFEX_algoSpace_C_FCAL_end_eta });
      m_lowerEM_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_C_lowerEM_eta;
      m_upperEM_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_C_upperEM_eta;
    }
    else {
      //Module 5
      lower_centre_neta.assign({FEXAlgoSpaceDefs::jFEX_algoSpace_A_EMB_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_A_EMIE_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_A_FCAL_start_eta});
      upper_centre_neta.assign({FEXAlgoSpaceDefs::jFEX_algoSpace_A_EMIE_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_A_FCAL_start_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_A_FCAL_end_eta});

      m_lowerEM_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_A_lowerEM_eta;
      m_upperEM_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_A_upperEM_eta;
    }

    //STEP 2: define phi FEXAlgoSpace parameters
    std::vector<int> lower_centre_nphi{FEXAlgoSpaceDefs::jFEX_algoSpace_EMB_start_phi, FEXAlgoSpaceDefs::jFEX_algoSpace_EMIE_start_phi,  FEXAlgoSpaceDefs::jFEX_algoSpace_FCAL_start_phi};
    std::vector<int> upper_centre_nphi{FEXAlgoSpaceDefs::jFEX_algoSpace_EMB_end_phi, FEXAlgoSpaceDefs::jFEX_algoSpace_EMIE_end_phi,  FEXAlgoSpaceDefs::jFEX_algoSpace_FCAL_end_phi};

    //STEP 3: loop over different EM/FCAL1 eta phi core fpga regions. These are potential seed  towers for electron clusters 
    for(uint i = 0; i<3; i++) {
      for(int nphi = lower_centre_nphi[i]; nphi < upper_centre_nphi[i]; nphi++) {
	for(int neta = lower_centre_neta[i]; neta < upper_centre_neta[i]; neta++) {
          
          // STEP 4: Ignore  seeds for |eta| < 2.3 or from the first FCAL eta bin                
	  if (m_jfex == 0 && neta >= FEXAlgoSpaceDefs::jFEX_algoSpace_C_FwdEl_start) continue;
	  if (m_jfex == 5 && neta <= FEXAlgoSpaceDefs::jFEX_algoSpace_A_FwdEl_start) continue;
	  if (m_jfex == 0 && neta == FEXAlgoSpaceDefs::jFEX_algoSpace_C_FCAL1_1st)   continue;
	  if (m_jfex == 5 && neta == FEXAlgoSpaceDefs::jFEX_algoSpace_A_FCAL1_1st)   continue;
	         
	  // STEP 5: define TTID (only FCAL1 in the third region) which will be the key for class in map, ignore tower ID = 0
	  uint myTTIDKey = m_jFEXalgoTowerID[nphi][neta];
	  if(myTTIDKey == 0) continue;
	  
	  //STEP 6: instanciate info class 
	  jFEXForwardElecInfo elCluster;
	  elCluster.setup(m_jfex, myTTIDKey, neta, nphi);

	  //STEP 7: fill coordinates and energy,
	  const auto [centreTT_eta, centreTT_phi] = getEtaPhi(myTTIDKey);
          const auto [centreTT_EtEM, centreTT_EtHad] = getEtEmHad(myTTIDKey);
	  elCluster.setCoreTTfPhi(centreTT_phi); //float
	  elCluster.setCoreTTfEta(centreTT_eta); //float
	  elCluster.setCoreTTEtEM(uint(centreTT_EtEM));//uint, check for rounding issues
						
	  //STEP 8 find neighbours, build isolation cone and collect cells for localMaximum window
	  elCluster.setNextTTEtEM(0);
	  elCluster.setNextTTID(0);
	  elCluster.setTTEtEMiso(0);
	  float aeta = fabs(centreTT_eta);

	  for(uint j = 0; j<3; j++) {
	    for(int nnphi = lower_centre_nphi[j]; nnphi < upper_centre_nphi[j]; nnphi++) {
	      for(int nneta = lower_centre_neta[j]; nneta < upper_centre_neta[j]; nneta++) {
		//First bin of FCAL1 excluded, include them as hadronic energy in last HEC bin?
		if (m_jfex == 0 && neta == FEXAlgoSpaceDefs::jFEX_algoSpace_C_FCAL1_1st) continue;
		if (m_jfex == 5 && neta == FEXAlgoSpaceDefs::jFEX_algoSpace_A_FCAL1_1st) continue;
		uint auxTTID = m_jFEXalgoTowerID[nnphi][nneta];
		if(auxTTID == myTTIDKey || auxTTID == 0) continue;
		auto [TT_eta, TT_phi] = getEtaPhi(auxTTID);
                auto [TT_EtEM, TT_EtHad] = getEtEmHad(auxTTID);
		uint deltaNeta = (nneta - neta);
		//float deltaPhi = fabs(centreTT_phi - TT_phi); if (deltaPhi > M_PI) deltaPhi = 2.*M_PI-deltaPhi;
		// corect for transition over 2PI
		if(m_fpga==0 || m_fpga==3) {
		  if(m_fpga==0) {
		    if(TT_phi>M_PI){
		      TT_phi = TT_phi-m_2PI;
		    }
		  }
		  else {
		    if(TT_phi<M_PI){
		      TT_phi = TT_phi+m_2PI;
		    }
		  }
		}
		float deltaPhi = fabs(centreTT_phi - TT_phi);
		if (isNeighbour(aeta, deltaNeta, deltaPhi)){
                  if (TT_EtEM > elCluster.getNextTTEtEM()) {
                    elCluster.setNextTTID(auxTTID);
                    elCluster.setNextTTEtEM(TT_EtEM);
                  }
                }

                int DeltaR = std::round( (std::pow((centreTT_eta - TT_eta),2) + std::pow((centreTT_phi - TT_phi),2)) * 1e5   );

		// DR<4 isolation cone (including neighbour) ToDo: solve border effects for Dphi slighly above 0.4 in FCAL. 
		if(DeltaR < m_Edge_dR4 ) {
		  elCluster.addTTEtEMiso(TT_EtEM);
		}

                // Collect cells in DR<2 or DR<3  window for identification of local maxima. ToDo: check for border effects  
                if((aeta < 2.5 && DeltaR < m_Edge_dR2)||(aeta > 2.5 && DeltaR < m_Edge_dR3) ) {
		  elCluster.includeTTinSearchWindow(auxTTID);
                }

	      }
	    }
	  }// end loop over aux cells
	  
	  // subtract neighbour energy from isolation cone
	  elCluster.addTTEtEMiso((-1)*elCluster.getNextTTEtEM());

	  // Calculating simple Ethad
	  if(aeta < 3.2){
	    elCluster.setTTEtHad1(centreTT_EtHad);
	  }else{
	    elCluster.setTTEtHad1(0);
	  }
	  
	  //STEP 11: Store filled class into map, with central TT ID as Key
	  ClusTowerIDLists[myTTIDKey] = elCluster;
	  
	}//eta
      }//phi
    }// 3 regions

    return ClusTowerIDLists;
  }  
    
  bool LVL1::jFEXForwardElecAlgo::isNeighbour(float aeta, int deltaNeta, float deltaPhi){
    bool isNext = 0;
    if (aeta < 2.3 && aeta < 2.5){
      isNext = (((deltaNeta == -1 || deltaNeta == 1) && deltaPhi < 0.55) || (deltaPhi < 0.11 && deltaNeta == 0));
    }else if (aeta < 2.3 && aeta < 3.1){
      isNext = (((deltaNeta == -1 || deltaNeta == 1) && deltaPhi < 0.55) || (deltaPhi < 0.21 && deltaNeta == 0));
    }else if (aeta > 3.1 && aeta < 3.2){
      isNext = (((deltaNeta == -1 || deltaNeta == 2) && deltaPhi <0.11) || ( deltaPhi < 0.21 && deltaNeta ==0));
    } else if (aeta > 3.2 && aeta < 3.3){
      isNext = ((deltaNeta == -2 || deltaNeta == 1) && deltaPhi <0.11);
    } else if (aeta > 3.3){
      isNext = ((deltaNeta == -1 || deltaNeta == 1) && deltaPhi <0.11);
    }
    return isNext;
  }
    
    
  

}// end of namespace LVL1
  
