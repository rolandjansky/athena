/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "DataModel/DataVector.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "MuonAlignGenTools/CommonGeometryManagerTool.h"
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
#include "MuonAlignEvent/RpcAlignModule.h"
#include "MuonAlignEvent/MdtAlignModule.h"
#include "MuonAlignEvent/TgcAlignModule.h"
#include "MuonAlignEvent/CscAlignModule.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include "TFile.h"
#include "TTree.h"
#include <iostream>

using namespace MuonGM;

namespace Muon {
  
//________________________________________________________________________
CommonGeometryManagerTool::CommonGeometryManagerTool(const std::string& type,
						 const std::string& name,
						 const IInterface* parent)
  : AthAlgTool(type,name,parent)
  , p_muonMgr(0)
  , m_idTool("MuonCalib::IdToFixedIdTool")
  , m_regionSvc(0)
 {

  declareProperty("doModuleSelection", m_doModuleSelection = false );
  declareProperty("ModuleSelection",   m_moduleSelection );

  declareProperty("AlignTraX",   m_alignTraX = true);
  declareProperty("AlignTraY",   m_alignTraY = true);
  declareProperty("AlignTraZ",   m_alignTraZ = true);
  declareProperty("AlignRotX",   m_alignRotX = true);
  declareProperty("AlignRotY",   m_alignRotY = true);
  declareProperty("AlignRotZ",   m_alignRotZ = true);

  declareProperty("SigmaTraX",   m_sigmaTraX = 1.);
  declareProperty("SigmaTraY",   m_sigmaTraY = 1.);
  declareProperty("SigmaTraZ",   m_sigmaTraZ = 1.);
  declareProperty("SigmaRotX",   m_sigmaRotX = 0.001);
  declareProperty("SigmaRotY",   m_sigmaRotY = 0.001);
  declareProperty("SigmaRotZ",   m_sigmaRotZ = 0.001);

  declareProperty("SoftCutTraX",   m_softcutTraX = 0.);
  declareProperty("SoftCutTraY",   m_softcutTraY = 0.);
  declareProperty("SoftCutTraZ",   m_softcutTraZ = 0.);
  declareProperty("SoftCutRotX",   m_softcutRotX = 0.);
  declareProperty("SoftCutRotY",   m_softcutRotY = 0.);
  declareProperty("SoftCutRotZ",   m_softcutRotZ = 0.);

}

//________________________________________________________________________
CommonGeometryManagerTool::~CommonGeometryManagerTool() 
{
  ATH_MSG_DEBUG("in CommonGeometryManagerTool d'tor");
  
}

//_______________________________________________________________________
StatusCode CommonGeometryManagerTool::initialize() 
{
  msg(MSG::DEBUG) << "initialize() of CommonGeometryManagerTool" << endmsg;  
  
  StatusCode sc=detStore()->retrieve(p_muonMgr);
  if (sc.isFailure()) {
    msg(MSG::FATAL)<<"Cannot retrieve MuonDetectorManager"<<endmsg;
    return sc;
  }
  else msg(MSG::DEBUG)<<"retrieved MuonDetectorManager"<<endmsg;
         
  if (m_idTool.retrieve().isSuccess())
    msg(MSG::INFO) << "Retrieved " << m_idTool << endmsg; 
  else{
    msg(MSG::FATAL)<<"Could not get " << m_idTool <<endmsg; 
    return StatusCode::FAILURE;
  }

  sc=service("RegionSelectionSvc", m_regionSvc);
  if(!sc.isSuccess())
    {
    msg(MSG::FATAL) << "Could not get RegionSelectionSvc!" << endmsg;
    return sc;
  }
  
  if (m_doModuleSelection){
    msg(MSG::INFO) << "Region selection string: " 
                   << m_regionSvc->GetRegionSelection() << endmsg;
  }

  // Print moduleSelection
  if(m_doModuleSelection && m_moduleSelection.size()>0){
    msg(MSG::INFO) << "The following modules will be selected (nModules="
		   << m_moduleSelection.size()  << "): " << endmsg;
    for(int i=0;i<(int)m_moduleSelection.size();i++){
      msg(MSG::INFO) << "  module: " << m_moduleSelection[i] << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}
  
//________________________________________________________________________
StatusCode CommonGeometryManagerTool::finalize() 
{  
  msg(MSG::DEBUG) << "finalize() of CommonGeometryManagerTool" << endmsg;
 
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
void CommonGeometryManagerTool::setSigmas(DataVector<Trk::AlignPar>* modPars)
{
  for(unsigned int ipar=0;ipar<modPars->size();++ipar) {
    switch(modPars->at(ipar)->paramType()) {
    case Trk::AlignModule::TransX:
      modPars->at(ipar)->setSigma(m_sigmaTraX);
      modPars->at(ipar)->setSoftCut(m_softcutTraX);
      break;
    case Trk::AlignModule::TransY:
      modPars->at(ipar)->setSigma(m_sigmaTraY);
      modPars->at(ipar)->setSoftCut(m_softcutTraY);
      break;
    case Trk::AlignModule::TransZ:
      modPars->at(ipar)->setSigma(m_sigmaTraZ);
      modPars->at(ipar)->setSoftCut(m_softcutTraZ);
      break;
    case Trk::AlignModule::RotX:
      modPars->at(ipar)->setSigma(m_sigmaRotX);
      modPars->at(ipar)->setSoftCut(m_softcutRotX);
      break;
    case Trk::AlignModule::RotY:
      modPars->at(ipar)->setSigma(m_sigmaRotY);
      modPars->at(ipar)->setSoftCut(m_softcutRotY);
      break;
    case Trk::AlignModule::RotZ:
      modPars->at(ipar)->setSigma(m_sigmaRotZ);
      modPars->at(ipar)->setSoftCut(m_softcutRotZ);
      break;
    default:
      break;
    }
  }
}

//________________________________________________________________________
bool CommonGeometryManagerTool::chamberSelected(const MuonGM::MuonReadoutElement* mre)
{
  return chamberSelected(m_idTool->idToFixedId(mre->identifyATLAS()));
}
//________________________________________________________________________
bool CommonGeometryManagerTool::chamberSelected(const MuonCalib::MuonFixedId& fid)
{
  if (!m_doModuleSelection) return true;
  
  return m_regionSvc->isInRegion(fid);
}

//________________________________________________________________________
bool CommonGeometryManagerTool::moduleSelected(std::string name)
{
  bool goodmodule=!m_doModuleSelection;
  
  if (m_doModuleSelection) {
    for (int imod=0;imod<(int)m_moduleSelection.size();imod++) {
      if (name==m_moduleSelection[imod]) {		    
	goodmodule=true;
	break;
      }
    }
  }

  return goodmodule;
}

} // end namespace
