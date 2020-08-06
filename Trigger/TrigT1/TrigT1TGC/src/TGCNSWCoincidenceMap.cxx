/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

#include "TrigT1TGC/TGCNSWCoincidenceMap.h"
#include "TrigT1TGC/NSWTrigOut.h"
#include "TrigT1TGC/TGCDatabaseManager.h"
#include "PathResolver/PathResolver.h"

#include "MuonCondInterface/ITGCTriggerDbTool.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "TrigT1TGC/TGCArguments.h"

namespace LVL1TGCTrigger {


  TGCNSWCoincidenceMap::TGCNSWCoincidenceMap(TGCArguments* tgcargs,const std::string& version,int side,int oct,int mod)
    :m_verName(version),
     m_side(side),
     m_octant(oct),
     m_module(mod),
     m_condDbTool("TGCTriggerDbTool"),
     m_tgcArgs(tgcargs)
  {

    m_msg = Athena::MsgStreamMember("LVL1TGCTrigger::TGCNSWCoincidenceMap::TGCNSWCoincidenceMap");
    m_msg.get().setLevel(tgcArgs()->MSGLEVEL());

    if(!tgcArgs()->USE_NSW()){return;}

    std::string moduleName = std::to_string(mod);
    if( isForward(mod)){
      m_region=TGCRegionType::FORWARD;
      m_sector  = m_module/3; 
      m_sector += 3*m_octant;
    }
    else{
      m_region=TGCRegionType::ENDCAP;
      m_sector  = m_module%3 + 2*(m_module/3);
      m_sector += 6*m_octant;
    }


    m_Offset_Eta.resize(m_NumberOfRoI[m_region]);
    m_Offset_Phi.resize(m_NumberOfRoI[m_region]);

    for(int dr=0;dr!=N_dEta;dr++){
      for(int dphi=0;dphi!=N_dPhi;dphi++){
	m_EtaPhi_CW[dr][dphi].resize(m_NumberOfRoI[m_region]);
	std::fill(m_EtaPhi_CW[dr][dphi].begin(), m_EtaPhi_CW[dr][dphi].end(),0);
      }
      for(int dTheta=0;dTheta!=N_Dtheta;dTheta++){
	m_EtaDtheta_CW[dr][dTheta].resize(m_NumberOfRoI[m_region]);
	std::fill(m_EtaDtheta_CW[dr][dTheta].begin(), m_EtaDtheta_CW[dr][dTheta].end(),0);
      }
    }

    if( ( oct%2==0 && mod==5 ) || (oct%2==1 && (mod==2 || mod==8)) ){moduleName+="b";}
    else{moduleName+="a";}


    //---------Read out CW data---------
    if(!this->readMap( moduleName, ReadCW_Type::EtaPhi_CW) || 
       !this->readMap( moduleName, ReadCW_Type::EtaDtheta_CW)
       ){
      // NSW trigger flag is set to false when the map reading failed.
      tgcArgs()->set_USE_NSW(false);
    }

    //---------Fill Shift data---------
    if(!this->readShift()){ 
      // NSW trigger flag is set to false when the map reading failed.
      tgcArgs()->set_USE_NSW(false);
    }

  }

  TGCNSWCoincidenceMap::TGCNSWCoincidenceMap(){ }


  TGCNSWCoincidenceMap::~TGCNSWCoincidenceMap()
  {
  }



  bool TGCNSWCoincidenceMap::isForward( int mod ){
    return ( mod==2 || mod==5 || mod==8 );
  }

  //TGC-NSW Eta-Phi Coincidence
  int TGCNSWCoincidenceMap::TGCNSW_pTcalcu_EtaPhi(const NSWTrigOut *nswOut,int roi) const
  {
    std::vector<int> nswEta_vec=nswOut->getNSWeta();
    std::vector<int> nswPhi_vec=nswOut->getNSWphi();
    int highest_pT=0;

    for(unsigned int nswTrk_id=0;nswTrk_id!=nswEta_vec.size();nswTrk_id++){
      int eta_decode=m_Offset_Eta[roi]-nswEta_vec[nswTrk_id];
      int phi_decode=m_Offset_Phi[roi]-nswPhi_vec[nswTrk_id];
      if(eta_decode<0 || eta_decode>=N_dEta){continue;}
      if(phi_decode<0 || phi_decode>=N_dPhi){continue;}

      if(highest_pT<m_EtaPhi_CW[eta_decode][phi_decode][roi]){
	highest_pT=m_EtaPhi_CW[eta_decode][phi_decode][roi];
      }
    }

    return highest_pT;
  }


  //TGC-NSW Eta-DeltaTheta Coincidence
   int TGCNSWCoincidenceMap::TGCNSW_pTcalcu_EtaDtheta(const NSWTrigOut *nswOut,int roi) const
  {
    std::vector<int> nswEta_vec=nswOut->getNSWeta();
    std::vector<int> nswDtheta_vec=nswOut->getNSWDtheta();
    int highest_pT=0;

    for(unsigned int nswTrk_id=0;nswTrk_id!=nswEta_vec.size();nswTrk_id++){
      int eta_decode=m_Offset_Eta[roi]-nswEta_vec[nswTrk_id];
      int dTheta_decode=nswDtheta_vec[nswTrk_id];
      if(eta_decode<0 || eta_decode>=N_dEta){continue;}
      if(highest_pT<m_EtaDtheta_CW[eta_decode][dTheta_decode][roi]){
	highest_pT=m_EtaDtheta_CW[eta_decode][dTheta_decode][roi];
      }
    }

    return highest_pT;
  }


  bool TGCNSWCoincidenceMap::readMap(std::string moduleName, ReadCW_Type cw_type) 
  {
    std::string kSide[2] = {"a", "c"};
    std::string kCWtype[2] = {"EtaPhi","EtaDtheta"};

    std::string dbname="";
    //Files below are put in calib dirctory when the files are fixed.
    //dbname = "/NSW/cm_" + kSide[m_side] + moduleName +kCWtype[cw_type]+"_"+m_verName+".db";
    //std::string fullName = PathResolver::FindCalibDirectory("TrigT1TGC_CW")+fn;
	  
    // Coincidence Map files haven't fixed.
    // The files are temporary and just for test.
    // These files are in lxplus.cern.ch:/afs/cern.ch/user/h/hhibi/public/TrigT1TGC/

    std::string fullName = "./cm_" + kSide[m_side] + moduleName+kCWtype[cw_type]+"_0030.db";

    std::ifstream data(fullName);
    if(!data.is_open()){return false;}
    char delimiter = '\n';
    std::string field;
    std::string tag;

    while (std::getline(data, field, delimiter)) {
      int roi=-1;
      unsigned int n_Etabit=0;
      unsigned int n_Phibit=0;
      std::istringstream header(field); 
      header >> tag;
      if(tag=="#"){ // read header part.     
	header >> roi >> n_Etabit >> n_Phibit;
      }
      // get trigger word
      std::string word;
      unsigned int pT=0;

      ////////////// Read CW ////////////////
      for(size_t posR=0; posR<N_dEta ; posR++){
	std::getline(data, field, delimiter);
	std::istringstream cont(field);
	//----Read EtaPhi CW---------
	if(cw_type==ReadCW_Type::EtaPhi_CW){
	  for(size_t posPHI=0; posPHI<N_dPhi; posPHI++){
	    cont >> word;
	    std::istringstream(word) >> std::hex >> pT;
	    m_EtaPhi_CW[posR][posPHI][roi]=pT;
	  }
	}
	//-----Read EtaDtheta CW-----
	if(cw_type==ReadCW_Type::EtaDtheta_CW){
	  for(size_t posDTHETA=0; posDTHETA<N_Dtheta; posDTHETA++){
	    cont >> word;
	    std::istringstream(word) >> std::hex >> pT;
	    m_EtaDtheta_CW[posR][posDTHETA][roi]=pT;
	  }
	}
      }      
   
    }
    data.close();


    return true;
  } 
  
  
  bool TGCNSWCoincidenceMap::readShift(){

    char delimiter = '\n';
    std::string field;
    std::string tag;
    int side;
    int triggerSector;

    //------- Read Endcap Shift
    std::string dbname="";
	  
    // Files for decording haven't fixed.
    // The files are temporary and just for test.
    // These files are in lxplus.cern.ch:/afs/cern.ch/user/h/hhibi/public/TrigT1TGC/
    if(m_region==ENDCAP){dbname = "./RoIpos_Endcap.db";}
    if(m_region==FORWARD){dbname = "./RoIpos_Forward.db";}

    //Files below are put in calib dirctory when the files are fixed.
    //if(m_region==ENDCAP){dbname = "/NSW/decorder_Endcap_"+m_verName+".db";}
    //if(m_region==FORWARD){dbname = "/NSW/decorder_Forward_"+m_verName+".db";}
    //fullName = PathResolver::FindCalibDirectory("TrigT1TGC_CW")+fn;

    std::ifstream data(dbname);
    if(!data.is_open()){return false;}

    while (std::getline(data, field, delimiter)) {
      std::istringstream header(field); 
      header >> tag;
      if(tag=="#"){ // read header part.
	header >> side >> triggerSector ;
      }

      // get trigger word
      std::getline(data, field, delimiter);
      std::istringstream cont(field);
      std::string word;
      int shift=0;
      int phi_shift[4];
      int eta_shift[37];

      if(side!=m_side || triggerSector!=m_sector){ continue; }
      for(int phiN=0; phiN!=4; phiN++){
	cont >> word;
	std::istringstream(word) >> shift;
	phi_shift[phiN]=shift;
      }
      for(int etaN=0; etaN!=m_NumberOfEtaRaw[m_region]; etaN++){
	cont >> word;
	std::istringstream(word) >> shift;
	eta_shift[etaN]=shift;
      }
      for(int roi=0;roi!=m_NumberOfRoI[m_region];roi++){
	m_Offset_Eta[roi]=eta_shift[roi/4]+N_dEta/2;
	m_Offset_Phi[roi]=phi_shift[roi%4]+N_dPhi/2;
      }
      
      break;
    }
    data.close();

    return true;
  }

} //end of namespace bracket
