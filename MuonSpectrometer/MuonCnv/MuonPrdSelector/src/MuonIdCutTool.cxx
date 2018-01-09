/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPrdSelector/MuonIdCutTool.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "StoreGate/StoreGateSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonCalibTools/IdToFixedIdTool.h"
#include "MuonCalibIdentifier/MuonFixedId.h"


//Tool to impliment a set of cuts on an Identifier, and then determine if a given ID is in that set

MuonIdCutTool::MuonIdCutTool(const std::string& t,const std::string& n,const IInterface* p)  :  
  AthAlgTool(t,n,p),
  m_idToFixedIdTool("MuonCalib::IdToFixedIdTool"),
  m_detMgr(nullptr),
  m_rpcIdHelper(nullptr),
  m_tgcIdHelper(nullptr),
  m_cscIdHelper(nullptr),
  m_mdtIdHelper(nullptr)
{
  declareInterface<IMuonIdCutTool>(this);
  
  declareProperty("CutStationRegion",m_cutStationRegion = false ); //0 inner, 1 extra(BEE), 2 middle, 3 outer
  declareProperty("CutStationName",m_cutStationName = true ); //integer which maps to BIL etc.
  declareProperty("CutSubstation",m_cutSubstation = false ); //do you want to cut on gas-gap or multilayer
  declareProperty("CutEta",m_cutEta = false );
  declareProperty("CutPhi",m_cutPhi = false ); 
  declareProperty("CutEE",m_cutEE = false ); //cut all EES, leave some EEL


  declareProperty("CutMdtRegionList",m_mdtRegionList );
  declareProperty("CutCscRegionList",m_cscRegionList );
  declareProperty("CutRpcRegionList",m_rpcRegionList );
  declareProperty("CutTgcRegionList",m_tgcRegionList );
  declareProperty("CutMdtStationNameList",m_mdtStationNameList );//cut these station names
  declareProperty("CutCscStationNameList",m_cscStationNameList );
  declareProperty("CutRpcStationNameList",m_rpcStationNameList );
  declareProperty("CutTgcStationNameList",m_tgcStationNameList );
  declareProperty("CutMdtMultilayerList",m_mdtMultilayerList );//entries in these lists are matched 
  declareProperty("CutRpcDoubletRList",m_rpcDoubletRList );//to entries in the stationName list.
  declareProperty("CutRpcGasGapList",m_rpcGasGapList );//the size of each list should be the same
  declareProperty("CutTgcGasGapList",m_tgcGasGapList );
  declareProperty("EELList",m_EELList ); //eta/sector to keep for EEL


  //note that the following properties are the eta/phi sectors too keep
  declareProperty("MdtBarrelEtaList",m_mdtBarEtaList );// -8 to 8, 0 would refer to eta = 0
  declareProperty("MdtEndcapEtaList",m_mdtEndEtaList );// -6 to 6, abs value increases with |R|
  declareProperty("MdtSectorList",m_mdtSectorList );// 1 to 8 for the whole detector, 1 points in +x
  
  declareProperty("RpcEtaList",m_rpcEtaList );// -8 to 8, 0 would refer to eta = 0
  declareProperty("RpcSectorList",m_rpcSectorList );// 1 to 8 for the whole detector, 1 points in +x

  declareProperty("CscEtaList",m_cscEtaList );// 1 is the only value
  declareProperty("CscSectorList",m_cscSectorList );// 1 to 8 for the whole detector, 1 points in +x

  //tgcs go by a different eta/phi numbering scheme and so must be cut separately
  declareProperty("TgcEtaList",m_tgcEtaList );// -5 to 5, [roughly] matches the endcap mdt naming scheme
  declareProperty("TgcEndcapPhiList",m_tgcEndPhiList );// 1 to 48 endcap
  declareProperty("TgcForwardPhiList",m_tgcForPhiList );// 1 to 24 forward


  //The cuts are independent, but if you have both cutEta = true and cutPhi = true make
  //sure to set either both eta and phi or neither lists for a given technology, otherwise, the program will
  //cut the whole technology

}
  
MuonIdCutTool::~MuonIdCutTool() {
  
}

StatusCode MuonIdCutTool::initialize() {
  ATH_CHECK( m_idToFixedIdTool.retrieve() );

  if (detStore()->retrieve(m_detMgr).isFailure()) {
    ATH_MSG_INFO( "Could not find the MuonReadoutGeometry Manager" );
  } 
  else {
    m_mdtIdHelper = m_detMgr->mdtIdHelper();
    m_cscIdHelper = m_detMgr->cscIdHelper();
    m_rpcIdHelper = m_detMgr->rpcIdHelper();
    m_tgcIdHelper = m_detMgr->tgcIdHelper();
  }
  
  if (!m_cutStationName && !m_cutStationRegion && !m_cutEta && !m_cutPhi && !m_cutEE){
    ATH_MSG_WARNING( "MuonIdCutTool invoked with no cuts performed"   );
  }

  //interpret EEL cuts
  int max = m_EELList.size();
  
  for(int i=0;i<max;i++){
    std::string cut = m_EELList[i];
    ATH_MSG_DEBUG( "EEL String: "  << cut  );
    size_t length = cut.size();
    size_t loc = cut.find('/');
    if (loc!=std::string::npos){
      std::string etaString = cut.substr(0,loc);
      std::string sectorString = cut.substr(loc+1,length-loc-1);
      int eta = atoi(etaString.c_str());
      int sector = atoi(sectorString.c_str());

      ATH_MSG_DEBUG( "EEL eta/phi string: "<< etaString << " " << sectorString  );
      ATH_MSG_DEBUG( "EEL eta/phi int: "<< eta << " " << sector  );

  
      if(std::abs(eta) > 0 && std::abs(eta) < 3 && sector > 0 && sector < 17){
	m_EELeta.push_back(eta);
	m_EELsector.push_back(sector);
      
      } else {
	ATH_MSG_WARNING( "Improper EEL Cut Selected, this cut is skipped"   );
      }

    } else {
      ATH_MSG_WARNING( "Improperly formated EEL Cut Selected, this cut is skipped"   );
    }
  }

  for(unsigned int i=0; i < m_EELeta.size();i++){
    ATH_MSG_DEBUG( "Eta: " << m_EELeta[i] << " Sector: " << m_EELsector[i]  );
  }

  return StatusCode::SUCCESS;
}


StatusCode MuonIdCutTool::finalize()
{
  ATH_MSG_DEBUG( "Finalize Called"  );
  return StatusCode::SUCCESS;
}



bool MuonIdCutTool::isCut(Identifier ID) const { //false indicates all cuts are passed
  
  //some checks to see if the tool is configured in a state that makes sense 
  if (m_cutStationName && m_cutStationRegion){
    ATH_MSG_WARNING( "MuonIdCutTool invoked with both Region Cuts and Station name cuts.Are you sure this is what you want?");
  }

  if (m_cutSubstation){
    if ((m_mdtMultilayerList.size() != m_mdtStationNameList.size()) || (m_tgcGasGapList.size() != m_tgcStationNameList.size()) ){
      ATH_MSG_WARNING( "Station and Substation cuts lists should match in length, no cut perfomed"   );
      return false;
    }
    else if ((m_rpcDoubletRList.size() != m_rpcStationNameList.size()) || ((m_rpcGasGapList.size() != m_rpcStationNameList.size()) && m_rpcGasGapList.size() !=0 )){
      ATH_MSG_WARNING( "Station and Substation cuts lists should match in length, no cut perfomed"   );
      return false;
    }
  } 

  //ATH_MSG_DEBUG( "isCut called, give hit ID:" << ID   );
  
  std::vector<int> cutList; //this will chose the correct cut list (StationName or Station Region)
  const MuonIdHelper * pIdHelper = 0; //this will end up pointing to the correct technology IdHelper
  

  //determine technology
  if (m_mdtIdHelper->is_mdt(ID)){
    pIdHelper = m_mdtIdHelper;
    ATH_MSG_DEBUG( "ID is an MDT"   );
  }
  else if (m_cscIdHelper->is_csc(ID)){
    pIdHelper = m_cscIdHelper;
    ATH_MSG_DEBUG( "ID is an CSC"   );
  } 
  else if (m_rpcIdHelper->is_rpc(ID)){
    pIdHelper = m_rpcIdHelper;
    ATH_MSG_DEBUG( "ID is an RPC"   );
  } 
  else if (m_tgcIdHelper->is_tgc(ID)){
    pIdHelper = m_tgcIdHelper;
    ATH_MSG_DEBUG( "ID is a TGC"   );
  } 
  else{
    ATH_MSG_ERROR( "Failure to determine technology type of ID#, returning false" );
    return false;
  }

  unsigned int staName = pIdHelper->stationName(ID);
  unsigned int staPhi = pIdHelper->stationPhi(ID);

  int sector = FindSector(staName,staPhi);

  // ATH_MSG_DEBUG( "Phi Station is " << staPhi
  // << " and Station name is " << staName  );
  
  //ATH_MSG_DEBUG( "Phi Sector is " << sector  );
  

  //Routine for cutting all EES and some EEL chambers
  if(m_cutEE && staName == 15) return true;
  if(m_cutEE && staName == 14){
    int listSize = m_EELeta.size();
    for (int i=0;i<listSize;i++){
      if(m_EELeta[i]==(pIdHelper->stationEta(ID)) && m_EELsector[i]==sector) break;
      if(i==(listSize-1)) return true; //if fails last entry, cut = true
    }
  }

  //Routine for cutting on Station Region

  if (m_cutStationRegion){ 
    if (m_mdtIdHelper->is_mdt(ID)){
      cutList = m_mdtRegionList;
    }
    else if (m_cscIdHelper->is_csc(ID)){
      cutList = m_cscRegionList;
    } 
    else if (m_rpcIdHelper->is_rpc(ID)){
      cutList = m_rpcRegionList;
    } 
    else if (m_tgcIdHelper->is_tgc(ID)){
      cutList = m_tgcRegionList;
    } 
    else{
      ATH_MSG_ERROR( "Failure to determine technology type of ID#, returning false"  );
      return false;
    }
    
    int listSize = cutList.size();
    for (int i=0; i<listSize;i++){                      
      ATH_MSG_DEBUG( "Region " << pIdHelper->stationRegion(ID) 
                     << " compared with " << cutList[i]  );
      if( cutList[i] == pIdHelper->stationRegion(ID)){
	ATH_MSG_DEBUG( "Return True"  );
	return true;
      }
    }
    ATH_MSG_DEBUG( "Passes Region Cut"  );
  }

    
  //Routine for cutting on Station Name and optional sub-region cuts
    
  
  if (m_cutStationName){ 
    if (m_mdtIdHelper->is_mdt(ID)){
      cutList = m_mdtStationNameList;
    }
    else if (m_cscIdHelper->is_csc(ID)){
      cutList = m_cscStationNameList;
    } 
    else if (m_rpcIdHelper->is_rpc(ID)){
      cutList = m_rpcStationNameList;
    } 
    else if (m_tgcIdHelper->is_tgc(ID)){
      cutList = m_tgcStationNameList;
    } 
    else{
      ATH_MSG_ERROR( "Failure to determine technology type of ID#, returning false"  );
      return false;
    }

    int listSize = cutList.size();
    for (int i=0; i<listSize;i++){
      ATH_MSG_DEBUG( "Station Name " << pIdHelper->stationName(ID) <<  " compared with " 
                     <<cutList[i]    );
      if( cutList[i] == pIdHelper->stationName(ID)){
	if (!m_cutSubstation){
	  return true;
	}
	
	else {  //proceed with more specific cuts

	  if (m_mdtIdHelper->is_mdt(ID)){
	    ATH_MSG_DEBUG( "MDT multilayer " <<m_mdtIdHelper->multilayer(ID)
                           <<  " compared with " << m_mdtMultilayerList[i]   );
	    if(m_mdtMultilayerList[i] == m_mdtIdHelper->multilayer(ID))
	      return true;
	  }


	  else if (m_rpcIdHelper->is_rpc(ID)){
	    ATH_MSG_DEBUG( "RPC doublet R " <<m_rpcIdHelper->doubletR(ID)
                           <<  " compared with " << m_rpcDoubletRList[i]   );
	    if( m_rpcDoubletRList[i] == m_rpcIdHelper->doubletR(ID) ){
	      if (m_rpcGasGapList.size() == 0){
		return true;
	      }
	      else {
		ATH_MSG_DEBUG( "RPC gasgap " <<m_rpcIdHelper->gasGap(ID)
                               <<  " compared with " << m_rpcGasGapList[i]   );
		if (m_rpcGasGapList[i] == m_rpcIdHelper->gasGap(ID))
		  return true;
	      }
	          
	    }
	  }
	  
	      
	  else if (m_tgcIdHelper->is_tgc(ID)){
	    ATH_MSG_DEBUG( "TGC gasgap " <<m_tgcIdHelper->gasGap(ID)
                           <<  " compared with " << m_tgcGasGapList[i]   );
	    if (m_tgcGasGapList[i] == m_tgcIdHelper->gasGap(ID))
	      return true;
	  }
	      
	      
	} //end sub-region cuts
      } //end if statement checking station name
    } //end station name loop
  } //end if m_cutStationName
  
  
  //Routine for cutting on eta and phi
  //this routine should always be last because its the only one that
  //ever returns false
  
  if (m_cutEta || m_cutPhi){ 
    
    std::vector<int> genEtaList; //general eta list (can be tgc list)
    std::vector<int> genPhiList; //general phi list (can be tgc list)
    int etaListSize;
    int phiListSize;
    int phi; //this variable is the phi index for tgcs, the sector for everyone else
    unsigned int staName = pIdHelper->stationName(ID);
    unsigned int staPhi = pIdHelper->stationPhi(ID);
    int sector = FindSector(staName,staPhi);
    phi = sector;
    
    ATH_MSG_DEBUG( "Phi Station is " << staPhi
                   << " and Station name is " << staName  );
    
    ATH_MSG_DEBUG( "Phi Sector is " << sector  );
    
    //Is it tgc?
    if (m_tgcIdHelper->is_tgc(ID)){
      //If no cuts specified, don't cut anything
      if (m_tgcEtaList.size()==0 && m_tgcEndPhiList.size()==0 && m_tgcForPhiList.size()==0)
	return false;
      genEtaList = m_tgcEtaList;
      phi =  pIdHelper->stationPhi(ID);
      //Is it forward?
      if (staName == 41 || staName == 43 || staName == 45 || staName == 47)
	genPhiList = m_tgcForPhiList;
      else
	genPhiList = m_tgcEndPhiList;
    }
    
    
    //mdt?
    else if(m_mdtIdHelper->is_mdt(ID)){
      //If no cuts specified, don't cut anything
      if (m_mdtSectorList.size()==0 && m_mdtEndEtaList.size()==0 && m_mdtBarEtaList.size()==0)
	return false;
      genPhiList = m_mdtSectorList;
      if (pIdHelper->isBarrel(ID))
	genEtaList = m_mdtBarEtaList;
      else
	genEtaList = m_mdtEndEtaList;
    }
    
    //rpc?
    else if(m_rpcIdHelper->is_rpc(ID)){
      //If no cuts specified, don't cut anything
      if (m_rpcSectorList.size()==0 && m_rpcEtaList.size()==0)
	return false;
      genPhiList = m_rpcSectorList;
      genEtaList = m_rpcEtaList;
    }
    
    //csc?
    else if(m_cscIdHelper->is_csc(ID)){
      //If no cuts specified, don't cut anything
      if (m_cscSectorList.size()==0 && m_cscEtaList.size()==0)
	return false;
      genPhiList = m_cscSectorList;
      genEtaList = m_cscEtaList;
    }
    
    etaListSize = genEtaList.size();
    phiListSize = genPhiList.size();
    
    bool etapass = true;
    bool phipass = true;
    
    if(m_cutEta){
      etapass = false;
      for (int i=0; i<etaListSize;i++){
	ATH_MSG_DEBUG( "Eta Station " << pIdHelper->stationEta(ID) 
                       << " compared with list to keep " << genEtaList[i]  );
	if( genEtaList[i] == pIdHelper->stationEta(ID))
	  etapass = true;
      }
    }
    
    if(m_cutPhi){
      phipass = false;
      
      for (int i=0; i<phiListSize;i++){
	ATH_MSG_DEBUG( "Phi Station " << phi 
                       << " compared with list to keep " << genPhiList[i]  );
	if( genPhiList[i] == phi)
	  phipass = true;
      }
    }
    
    if (!etapass || !phipass) //if eta or phi fail it is cut
      return true;
    
  }
  
  
  
  
  return false; //keep the event if none of the cuts removed the event
}


//overloaded function to take in MuonFixedId

bool MuonIdCutTool::isCut(MuonCalib::MuonFixedId mfid) const {
//  std::cout << "testing if IdCuts called" << std::endl;
Identifier ID = m_idToFixedIdTool->fixedIdToId(mfid);
bool iscut = MuonIdCutTool::isCut(ID);
return iscut;
}

//function to find phi sector (1-16) from phi station (1-8)
int MuonIdCutTool::FindSector(unsigned int staName, unsigned int staPhi) const {

  int sector=-1;
  
  //This strange looking array specifies whether the station is large or small
  //999 specifies a number not used, or a tgc
  //station name maps to BIL,EOS,etc
  int isStationNameLarge[53] = {1,0,1,0,1,0,0,1,0,0,0,0,999,1,1,0,999,1,0,999,1,0,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,0,0,1,1};
  
  if (staName < 53){
    sector = staPhi*2 - isStationNameLarge[staName];
  }
  
  return sector;
  
}
