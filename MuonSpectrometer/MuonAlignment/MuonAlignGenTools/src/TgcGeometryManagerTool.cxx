/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAlignGenTools/TgcGeometryManagerTool.h"

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "DataModel/DataVector.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "TrkAlignInterfaces/IAlignModuleTool.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MuonStation.h"

#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"

#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignPar.h"
#include "MuonAlignEvent/CombinedMuonAlignModule.h"
#include "MuonAlignEvent/MdtAlignModule.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include "TFile.h"
#include "TTree.h"
#include <iostream>

using namespace MuonGM;

namespace Muon {
  
//________________________________________________________________________
TgcGeometryManagerTool::TgcGeometryManagerTool(const std::string& type,
						 const std::string& name,
						 const IInterface* parent)
  : CommonGeometryManagerTool(type,name,parent)
  , m_alignModuleTool("Muon::MuonAlignModuleTool")
  , m_idHashToAlignModuleMaps(Trk::AlignModule::NDetectorTypes,(Trk::AlignModuleList*)0)
  , m_alignParList    (0)
  , m_fullAlignParList(0)
 {

  declareInterface<IGeometryManagerTool>(this);

  declareProperty("AlignModuleTool",     m_alignModuleTool);
  declareProperty("MdtAlignParameters",  m_mdtAlignParameters );
  declareProperty("AlignLevel",          m_alignLevel=3);

}

//________________________________________________________________________
TgcGeometryManagerTool::~TgcGeometryManagerTool() 
{
  ATH_MSG_DEBUG("in TgcGeometryManagerTool d'tor");
  
  ATH_MSG_DEBUG("alignModuleList.size: "<<m_alignModuleList.size());
  for (int i=0;i<(int)m_alignModuleList.size();i++) 
    delete m_alignModuleList[i];
  
  ATH_MSG_DEBUG("deleting fullAlignParList");
  delete m_fullAlignParList;
  
  ATH_MSG_DEBUG("deleting alignParList");
  delete m_alignParList;

  ATH_MSG_DEBUG("done with TgcGeometryManagerTool d'tor");

}

//_______________________________________________________________________
StatusCode TgcGeometryManagerTool::initialize() 
{
  msg(MSG::DEBUG) << "initialize() of TgcGeometryManagerTool" << endmsg;  

  if (!CommonGeometryManagerTool::initialize().isSuccess()) {
    ATH_MSG_FATAL("CommonGeometryManagerTool not initialized");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}
  
//________________________________________________________________________
StatusCode TgcGeometryManagerTool::finalize() 
{  
  msg(MSG::DEBUG) << "finalize() of TgcGeometryManagerTool" << endmsg;
  
  if (!CommonGeometryManagerTool::finalize().isSuccess()) {
    ATH_MSG_FATAL("CommonGeometryManagerTool finalize not successful");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
int TgcGeometryManagerTool::ReadGeometry(int)
{
  ATH_MSG_DEBUG("in ReadGeometry()");
  
  // set pointers
  m_idHashToAlignModuleMapsPtr = &m_idHashToAlignModuleMaps;
  m_alignModuleListPtr = &m_alignModuleList;
  
  // build alignment geometry
  buildGeometry();
  
  // now set the alignment parameters
  // first prepare the parameter lists
  m_alignParList     = new DataVector< DataVector<Trk::AlignPar> >(SG::OWN_ELEMENTS);
  m_fullAlignParList = new DataVector< DataVector<Trk::AlignPar> >(SG::OWN_ELEMENTS);

  // loop over modules
  ATH_MSG_DEBUG("Adding module parameters to modules");
  std::vector<Trk::AlignModule *>::const_iterator imod = m_alignModuleListPtr->begin();
  std::vector<Trk::AlignModule *>::const_iterator imod_end = m_alignModuleListPtr->end();
  for( ; imod!=imod_end ; ++imod) {
    ATH_MSG_DEBUG("Module "<<(*imod)->name());
    addModuleParameters(*imod,m_fullAlignParList,m_alignParList);
  }
  
  // set alignModuleList and hash table in the alignModuleTool
  m_alignModuleTool->setAlignModules(&m_alignModuleList, &m_idHashToAlignModuleMaps);
  ATH_MSG_DEBUG(" geometry set in m_alignModuleTool");
  
  // set alignPar lists in the alignModuleTool
  ATH_MSG_DEBUG(" alignParList = "<<m_alignParList);
  ATH_MSG_DEBUG(" fullAlignParList = "<<m_fullAlignParList);
  m_alignModuleTool->setAlignParLists(m_fullAlignParList, m_alignParList);
  ATH_MSG_DEBUG(" AlignParLists set in m_alignModuleTool");
    
  int nDoF= m_alignModuleTool->nAlignParameters();
  ATH_MSG_DEBUG("AlignModuleList has " << nDoF << " alignment parameters");
  
  ATH_MSG_DEBUG("leaving ReadGeometry");
  return nDoF;
}

//________________________________________________________________________
void TgcGeometryManagerTool::buildGeometry()
{
  ATH_MSG_INFO("Preparing MDT geometry");
  if      (m_alignLevel==0)  buildL0();
  else if (m_alignLevel==3)  buildL3();
  else if (m_alignLevel==1)  buildL1();
  else if (m_alignLevel==23) buildL23();
  else
    ATH_MSG_FATAL("Can't do alignLevel "<<m_alignLevel<<" yet!");
}

//________________________________________________________________________
void TgcGeometryManagerTool::buildL3()
{

  
  ATH_MSG_INFO("preparing MDT geometry for L3");

  const int NMDTREL=MuonGM::MuonDetectorManager::MdtDetElMaxHash;  
  if (!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::MDT))
    m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::MDT) = 
      new Trk::AlignModuleList((size_t)NMDTREL,0);
  Trk::AlignModuleList* mdtIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::MDT);

  for (int iname=0; iname<MuonGM::MuonDetectorManager::NMdtStatType; iname++) {
    for (int ieta=0; ieta<MuonGM::MuonDetectorManager::NMdtStatEta; ieta++) {
      for (int iphi=0; iphi<MuonGM::MuonDetectorManager::NMdtStatPhi; iphi++) {
	
	CombinedMuonAlignModule* alignmod(0);
	
	for (int imult=0; imult<MuonGM::MuonDetectorManager::NMdtMultilayer; imult++) {
	  
	  const MuonGM::MdtReadoutElement* mdt=
	    p_muonMgr->getMdtReadoutElement(iname,ieta,iphi,imult);
	  
	  if (!mdt) continue;
	  
          const Amg::Transform3D mdtTransform = mdt->AmdbLRSToGlobalTransform();
          Amg::Transform3D gf2af=mdtTransform.inverse();
	  
	  if (alignmod==0) {
	    
	    if (!chamberSelected(mdt)) continue;
	    
	    alignmod=new CombinedMuonAlignModule(this,gf2af);
	    
	    // set alignModule identifier and idHash
	    Identifier id=mdt->identifyATLAS();
	    alignmod->setIdHash(getNextIDHash());
	    alignmod->setIdentifier(id);
	    
	    // set alignModule name
	    MuonCalib::MuonFixedId fid=m_idTool->idToFixedId(id);
	    std::string name=fid.stationNumberToFixedStationString(fid.stationName());
	    
	    std::stringstream chamberName;
	    chamberName<<name<<std::abs(fid.eta());
	    if (fid.eta()<0) chamberName<<"C"; else chamberName<<"A";
	    int detphi=2*fid.phi(); 
	    if (name.substr(2,1)=="L")      detphi--;
	    else if (name.substr(2,1)!="S") detphi=fid.phi();	      
	    chamberName<<detphi;	      
	    alignmod->setName(chamberName.str());
	    
	    ATH_MSG_DEBUG("created new MdtAlignModule, id "<<id
			  <<", fixedId "<<fid.getIdInt()
			  <<", "<<chamberName.str().c_str());
	    	  
	    m_alignModuleListPtr->push_back(alignmod);
	  } // created new alignModule
	  
	  // add this mdt to alignModule and set map from idHash to AlignModule
	  const Amg::Transform3D transform = Amg::Transform3D::Identity();
	  alignmod->addDetElement(Trk::AlignModule::MDT,mdt,transform);
	  
	  int idHash=mdt->identifyHash(); // this returns idHash for MDT detector, not readout element. But it's ok because there's 1 detector element per align module.
	   
	  ATH_MSG_DEBUG("setting alignmod for idHash "<<idHash);
	  (*mdtIdHashMap)[idHash]=alignmod;
	  ATH_MSG_DEBUG("setting idHash");
	  alignmod->setIdHash(idHash);

	} // imult
      } // iphi
    } // ieta
  } // iname

  return;
  
}

//________________________________________________________________________
void TgcGeometryManagerTool::buildL0()
{
  
  ATH_MSG_INFO("preparing MDT geometry for L0");

  // Create align module.  Alignment will be done in global frame.
  //Amg::Transform3D transformBar=Amg::Transform3D::Identity();
  
  CombinedMuonAlignModule* mod= new CombinedMuonAlignModule(this,Amg::Transform3D::Identity());

  mod->setIdHash(getNextIDHash());
  mod->setName("MDT");
  mod->setIdentifier(p_muonMgr->mdtIdHelper()->channelID("BIL",1,1,1,1,1));
  
  // loop over MDT chambers and add to modules
  const int NMDTREL=MuonGM::MuonDetectorManager::MdtDetElMaxHash;  
  if (!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::MDT))
    m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::MDT) = 
      new Trk::AlignModuleList((size_t)NMDTREL,0);
  Trk::AlignModuleList* mdtIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::MDT);
  
  for (int iname=0; iname<MuonGM::MuonDetectorManager::NMdtStatType; iname++) {
    for (int ieta=0; ieta<MuonGM::MuonDetectorManager::NMdtStatEta; ieta++) {
      for (int iphi=0; iphi<MuonGM::MuonDetectorManager::NMdtStatPhi; iphi++) {
	for (int imult=0; imult<MuonGM::MuonDetectorManager::NMdtMultilayer; imult++) {
	  
	  const MuonGM::MdtReadoutElement* mdt = 
	    p_muonMgr->getMdtReadoutElement(iname,ieta,iphi,imult);
	  if (0==mdt) continue;
	  
	  // get AlignModule, determine whether endcap or barrel
	  unsigned int det=Barrel;
	  bool isBarrel=p_muonMgr->mdtIdHelper()->isBarrel(mdt->identify());
	  
	  // change to endcap if BEE
	  if (isBarrel) {
	    std::string stNameString=p_muonMgr->mdtIdHelper()->stationNameString(p_muonMgr->mdtIdHelper()->stationName(mdt->identify()));
	    if (stNameString.substr(0,3)=="BEE") isBarrel=false;
	  }
	  
	  if (!isBarrel) {
	    int stEta=ieta-MuonGM::MuonDetectorManager::NMdtStEtaOffset;
	    det = (stEta>0) ? EndcapA : EndcapC;
	  }
	  
	  Trk::AlignModule* mod(0);
	  for (int imod=0;imod<(int)m_alignModuleList.size();imod++) {
	    if (m_alignModuleList[imod]->identify32()==det) {
	      mod=m_alignModuleList[imod];
	      break;
	    }
	  }
	  
	  if (!mod) continue;
	  
	  // add detector element to AlignModule
	  IdentifierHash idHash=mdt->identifyHash();	  
	  
	  Amg::Transform3D level1AlignToAmdbLRS = 
	    mod->globalFrameToAlignFrame().inverse() *
	    mdt->AmdbLRSToGlobalTransform().inverse();
	  
	  
	  mod->addDetElement(Trk::AlignModule::MDT,
			     mdt, level1AlignToAmdbLRS);
	  
	  (*mdtIdHashMap)[idHash]=mod;
	  
	} // imult
      } // iphi
    } // ieta
  } // iname
  
  return;
}
//________________________________________________________________________
void TgcGeometryManagerTool::buildL1()
{
  
  ATH_MSG_INFO("preparing MDT geometry for L1");

  // Create align modules.  Alignment will be done in global frame for barrel and 
  // each end cap.
  std::string modulenames[nL1Modules]={"ECA","ECC","Bar"};
  
  for (int imod=0;imod<nL1Modules;imod++) {      
    
    if (!moduleSelected(modulenames[imod])) continue;
    
    CombinedMuonAlignModule* mod = 
      new CombinedMuonAlignModule(this,Amg::Transform3D::Identity());

    m_alignModuleListPtr->push_back(mod);
    
    mod->setIdHash(getNextIDHash());
    mod->setName(modulenames[imod]);

    switch (imod) {
    case 0:
      mod->setIdentifier(p_muonMgr->mdtIdHelper()->channelID("EIL", 1,1,1,1,1));
      break;
    case 1:
      mod->setIdentifier(p_muonMgr->mdtIdHelper()->channelID("EIL",-1,1,1,1,1));
      break;
    case 2:
      mod->setIdentifier(p_muonMgr->mdtIdHelper()->channelID("BIL", 1,1,1,1,1));
      break;
    }
  }

  // loop over MDT chambers and add to modules
  const int NMDTREL=MuonGM::MuonDetectorManager::MdtDetElMaxHash;  
  if (!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::MDT))
    m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::MDT) = 
      new Trk::AlignModuleList((size_t)NMDTREL,0);
  Trk::AlignModuleList* mdtIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::MDT);
  
  for (int iname=0; iname<MuonGM::MuonDetectorManager::NMdtStatType; iname++) {
    for (int ieta=0; ieta<MuonGM::MuonDetectorManager::NMdtStatEta; ieta++) {
      for (int iphi=0; iphi<MuonGM::MuonDetectorManager::NMdtStatPhi; iphi++) {
	for (int imult=0; imult<MuonGM::MuonDetectorManager::NMdtMultilayer; imult++) {
	  
	  const MuonGM::MdtReadoutElement* mdt = 
	    p_muonMgr->getMdtReadoutElement(iname,ieta,iphi,imult);
	  if (0==mdt) continue;
	  
	  // get AlignModule, determine whether endcap or barrel
	  unsigned int det=Barrel;
	  bool isBarrel=p_muonMgr->mdtIdHelper()->isBarrel(mdt->identify());
	  
	  // change to endcap if BEE
	  if (isBarrel) {
	    std::string stNameString=p_muonMgr->mdtIdHelper()->stationNameString(p_muonMgr->mdtIdHelper()->stationName(mdt->identify()));
	    if (stNameString.substr(0,3)=="BEE") isBarrel=false;
	  }
	  
	  if (!isBarrel) {
	    int stEta=ieta-MuonGM::MuonDetectorManager::NMdtStEtaOffset;
	    det = (stEta>0) ? EndcapA : EndcapC;
	  }
	  
	  Trk::AlignModule* mod(0);
	  for (int imod=0;imod<(int)m_alignModuleList.size();imod++) {
	    if (m_alignModuleList[imod]->identify32()==det) {
	      mod=m_alignModuleList[imod];
	      break;
	    }
	  }
	  
	  if (!mod) continue;
	  
	  // add detector element to AlignModule
	  IdentifierHash idHash=mdt->identifyHash();	  
	  
	  Amg::Transform3D level1AlignToAmdbLRS = 
	    mod->globalFrameToAlignFrame().inverse() *
	    mdt->AmdbLRSToGlobalTransform().inverse(); 
	  	  
	  mod->addDetElement(Trk::AlignModule::MDT,
			     mdt, level1AlignToAmdbLRS);
	  
	  (*mdtIdHashMap)[idHash]=mod;
	  
	} // imult
      } // iphi
    } // ieta
  } // iname
  
  return;
}

//________________________________________________________________________
void TgcGeometryManagerTool::buildL23()
{

  ATH_MSG_ERROR("migrate TgcGeometryManagerTool::buildL23() go Eigen");
  return;

  /*
  ATH_MSG_INFO("preparing MDT geometry for L2.3");
  
  //get module names
  unsigned int nSectors = 16;
  std::vector<std::string> moduleNames(64);
  
  unsigned int imod=0;
  for (unsigned int isec=0; isec<nSectors; isec++) {
    
    bool isSmall = ((isec+1)%2 == 0) ? true : false; 
    for (int detSide=0; detSide<2; detSide++){
      
      std::stringstream moduleStream;
      moduleStream<<"Sector";
      
      if (detSide==0) moduleStream<<"A";
      else            moduleStream<<"C";
      
      moduleStream<<(isec+1);
      std::string moduleString=moduleStream.str();
      
      if(!isSmall){
        moduleNames[imod++]=moduleString;
      }
      else{
        moduleNames[imod++]=moduleString+"_I";
        moduleNames[imod++]=moduleString+"_M";
        moduleNames[imod++]=moduleString+"_O";
      }
    }
  }
  
  // create AlignModules
  for (imod=0; imod<moduleNames.size(); ++imod) {
    
    ATH_MSG_DEBUG("building Module: " << moduleNames[imod]);
    if (!moduleSelected(moduleNames[imod])){ 
      ATH_MSG_DEBUG("Module not in selection.");
      continue;
    }
    
    CombinedMuonAlignModule* mod;
    mod = new CombinedMuonAlignModule(this,Amg::Transform3D::Identity());
    m_alignModuleListPtr->push_back(mod);
    
    mod->setName(moduleNames[imod]);
    mod->setIdentifier((Identifier)imod);
    mod->setIdHash(getNextIDHash());
  }
  
  // loop over MDT chamber and add to AlignModules
  const int NMDTREL=MuonGM::MuonDetectorManager::MdtDetElMaxHash;  
  if (!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::MDT))
    m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::MDT) = 
      new Trk::AlignModuleList((size_t)NMDTREL,0);
  Trk::AlignModuleList* mdtIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::MDT);
  
  int etamax = abs(p_muonMgr->mdtIdHelper()->stationEtaMin(true))
    + p_muonMgr->mdtIdHelper()->stationEtaMax(true);

  for (int iname=0; iname<MuonGM::MuonDetectorManager::NMdtStatType; iname++) {
    for (int ietaindex=0; ietaindex<etamax; ietaindex++) {
      for (int iphi=0; iphi<MuonGM::MuonDetectorManager::NMdtStatPhi; iphi++) {	
	for (int imult=1; imult<MuonGM::MuonDetectorManager::NMdtMultilayer; imult++) {
	  
	  
	  //loop first eta=0..8, then eta=-1..-8
          int ieta = ietaindex;
          if (ietaindex > p_muonMgr->mdtIdHelper()->stationEtaMax(true)) {
            ieta = -1*(ietaindex - p_muonMgr->mdtIdHelper()->stationEtaMax(true));
          }
	  
          ATH_MSG_DEBUG("loop -> name: " << iname << ", eta: " << ieta 
                        << ", phi: " << iphi << ", mult: " << imult);
	  
          bool isValid(true);
          Identifier chid = p_muonMgr->mdtIdHelper()->channelID(iname,ieta,iphi,imult,1,1,true,&isValid);
          if (!isValid){ 
            ATH_MSG_DEBUG("not valid ID!");
            continue;
          }
	  
          int stName = p_muonMgr->mdtIdHelper()->stationName(chid);
          int stEta  = p_muonMgr->mdtIdHelper()->stationEta(chid);
          int stPhi  = p_muonMgr->mdtIdHelper()->stationPhi(chid);
          int ml     = p_muonMgr->mdtIdHelper()->multilayer(chid);
       
          ATH_MSG_DEBUG("loop2 -> name: " << stName << ", eta: " << stEta 
                        << ", phi: " << stPhi << ", mult: " << ml << ", valid: " << isValid);

          const MuonGM::MdtReadoutElement* mdt = 
            p_muonMgr->getMdtReadoutElement(chid);
 
	  if (!mdt) continue;
	  
	  bool isBarrel=p_muonMgr->mdtIdHelper()->isBarrel(mdt->identify());
	  if (!isBarrel){
            ATH_MSG_DEBUG("is not Barrel");
            continue;
          }
	  
          MuonCalib::MuonFixedId fid=m_idTool->idToFixedId(mdt->identify());
          std::string name=fid.stationNumberToFixedStationString(fid.stationName());	    
          ATH_MSG_DEBUG("name: " << name);
	  
          if(name=="BEE"){
            ATH_MSG_DEBUG("Is BEE chamber, will be rejected.");
            continue;
	  }
	  
          std::stringstream chamberName;
          chamberName<<name<<std::abs(fid.eta());
          if (fid.eta()<0) chamberName<<"C"; else chamberName<<"A";
          int detphi=2*fid.phi(); 
          if (name.substr(2,1)=="L")      detphi--;
          else if (name.substr(2,1)!="S") detphi=fid.phi();	      
          chamberName<<detphi;	      
          ATH_MSG_DEBUG("chamberName: " << chamberName.str());
	  
          std::stringstream modName;
          modName<<"Sector";
          if (fid.eta()<0) modName<<"C"; 
          else             modName<<"A";
          modName<<detphi;
          
          ATH_MSG_DEBUG("iphi: " << iphi);
          if(detphi%2==0){
            ATH_MSG_DEBUG("is odd");
            if (name.substr(1,1)=="I")      modName<<"_I";
            else if (name.substr(1,1)=="M") modName<<"_M";
            else if (name.substr(1,1)=="O") modName<<"_O";
            else ATH_MSG_WARNING("Could not identify chamber layer for chamber: " << name);
          }
          
          ATH_MSG_DEBUG("modName: " << modName.str());
	  
	  Trk::AlignModule* mod(0);
	  for (unsigned int imod=0;imod<m_alignModuleList.size();imod++) {
	    //ATH_MSG_DEBUG("  modlist["<<imod<<"]: "<<m_alignModuleList[imod]->name());
            if (m_alignModuleList[imod]->name()==modName.str()) {
	      mod=m_alignModuleList[imod];
	      break;
	    }
	  }
	  
	  if (!mod) { ATH_MSG_DEBUG("no module"); continue; }

	  //set alignframe to the local frame of first readout element
          if (mod->globalFrameToAlignFrame().matrix().isIdentity()) {
            const Amg::Transform3D mdtTransform = mdt->AmdbLRSToGlobalTransform();
            Amg::Transform3D gf2af=mdtTransform.inverse();
            mod->setGlobalFrameToAlignFrameTransform(gf2af);
            
            ATH_MSG_DEBUG("setting globalFrameToAlignFrame: "<<endmsg
                          <<     gf2af[0][0]<<" "<< gf2af[0][1]<<" "<< gf2af[0][2]
                          <<" "<<gf2af[1][0]<<" "<< gf2af[1][1]<<" "<< gf2af[1][2]
                          <<" "<<gf2af[2][0]<<" "<< gf2af[2][1]<<" "<< gf2af[2][2]<<" ");
	    
	    
            Identifier id = mdt->identifyATLAS();
            mod->setIdentifier(id);
            ATH_MSG_DEBUG("name: " << name << ", eta: " << ieta << ", phi: " << iphi);
            ATH_MSG_DEBUG("alignmod identifier: " << id.getString());
          } 
	  else 
            ATH_MSG_DEBUG("transform already set");          
          	 	  
	  // add detector element to AlignModule
	  IdentifierHash idHash=mdt->identifyHash();	  

	  Amg::Transform3D level23AlignToAmdbLRS = 
	    mod->globalFrameToAlignFrame().inverse() *
	    mdt->AmdbLRSToGlobalTransform().inverse(); 
	  
           mod->addDetElement(Trk::AlignModule::MDT,
                              mdt, level23AlignToAmdbLRS);
	  (*mdtIdHashMap)[idHash]=mod;

	} // imult
      } // iphi
    } // ieta
  } // iname

  return;
  */
}

} // end namespace
