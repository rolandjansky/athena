/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAlignGenTools/CscGeometryManagerTool.h"

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
CscGeometryManagerTool::CscGeometryManagerTool(const std::string& type,
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
  declareProperty("CscAlignParameters",  m_cscAlignParameters );
  declareProperty("AlignLevel",          m_alignLevel=3);

}

//________________________________________________________________________
CscGeometryManagerTool::~CscGeometryManagerTool() 
{
  ATH_MSG_DEBUG("in CscGeometryManagerTool d'tor");
  
  ATH_MSG_DEBUG("alignModuleList.size: "<<m_alignModuleList.size());
  for (int i=0;i<(int)m_alignModuleList.size();i++) 
    delete m_alignModuleList[i];
  
  ATH_MSG_DEBUG("deleting fullAlignParList");
  delete m_fullAlignParList;
  
  ATH_MSG_DEBUG("deleting alignParList");
  delete m_alignParList;

  ATH_MSG_DEBUG("done with CscGeometryManagerTool d'tor");

}

//_______________________________________________________________________
StatusCode CscGeometryManagerTool::initialize() 
{
  msg(MSG::DEBUG) << "initialize() of CscGeometryManagerTool" << endmsg;  

  if (!CommonGeometryManagerTool::initialize().isSuccess()) {
    ATH_MSG_FATAL("CommonGeometryManagerTool not initialized");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}
  
//________________________________________________________________________
StatusCode CscGeometryManagerTool::finalize() 
{  
  msg(MSG::DEBUG) << "finalize() of CscGeometryManagerTool" << endmsg;
  
  if (!CommonGeometryManagerTool::finalize().isSuccess()) {
    ATH_MSG_FATAL("CommonGeometryManagerTool finalize not successful");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
int CscGeometryManagerTool::ReadGeometry(int)
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
void CscGeometryManagerTool::buildGeometry()
{
  ATH_MSG_INFO("Preparing CSC geometry");
  if      (m_alignLevel==0)  buildL0();
  else if (m_alignLevel==3)  buildL3();
  else if (m_alignLevel==1)  buildL1();
  else if (m_alignLevel==23) return;
  else
    ATH_MSG_FATAL("Can't do alignLevel "<<m_alignLevel<<" yet!");
  return;
}

//________________________________________________________________________
void CscGeometryManagerTool::buildL3()
{
  ATH_MSG_ERROR("migrate CscGeometryManagerTool::buildL3() to Eigen!");
  return;

  /*
  ATH_MSG_INFO("preparing CSC geometry for L3");

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
  */
}

//________________________________________________________________________
void CscGeometryManagerTool::buildL0()
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
void CscGeometryManagerTool::buildL1()
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

} // end namespace
