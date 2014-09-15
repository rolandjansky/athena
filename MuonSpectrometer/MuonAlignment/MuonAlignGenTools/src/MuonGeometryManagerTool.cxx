/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "DataModel/DataVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "MuonAlignGenTools/MuonGeometryManagerTool.h"
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

#include "CLHEP/Geometry/Transform3D.h"

#include "TFile.h"
#include "TTree.h"
#include <iostream>

using namespace MuonGM;

namespace Muon {
  
//________________________________________________________________________
MuonGeometryManagerTool::MuonGeometryManagerTool(const std::string& type,
						 const std::string& name,
						 const IInterface* parent)
  : CommonGeometryManagerTool(type,name,parent)
  , m_alignModuleTool("Muon::MuonAlignModuleTool")
  , m_mdtGeoManager("")
  , m_cscGeoManager("")
  , m_tgcGeoManager("")
  , m_idHashToAlignModuleMaps(Trk::AlignModule::NDetectorTypes,(Trk::AlignModuleList*)0)
  , m_alignParList    (0)
  , m_fullAlignParList(0)
 {
   
   declareInterface<IGeometryManagerTool>(this);

  declareProperty("AlignModuleTool",       m_alignModuleTool);

  declareProperty("MdtGeometryManager",    m_mdtGeoManager);
  declareProperty("CscGeometryManager",    m_cscGeoManager);
  declareProperty("TgcGeometryManager",    m_tgcGeoManager);

  declareProperty("AlignLevel", m_alignLevel=3);

  declareProperty("AlignMdt", m_alignMdt = true );
  declareProperty("AlignTgc", m_alignTgc = true );
  declareProperty("AlignCsc", m_alignCsc = true );

}

//________________________________________________________________________
MuonGeometryManagerTool::~MuonGeometryManagerTool() 
{
  ATH_MSG_DEBUG("in MuonGeometryManagerTool d'tor");
  
  ATH_MSG_DEBUG("alignModuleList.size: "<<m_alignModuleList.size());
  for (int i=0;i<(int)m_alignModuleList.size();i++) 
    delete m_alignModuleList[i];
  
  ATH_MSG_DEBUG("deleting fullAlignParList");
  delete m_fullAlignParList;
  
  ATH_MSG_DEBUG("deleting alignParList");
  delete m_alignParList;

  ATH_MSG_DEBUG("done with MuonGeometryManagerTool d'tor");

}

//_______________________________________________________________________
StatusCode MuonGeometryManagerTool::initialize() 
{
  
  ATH_MSG_DEBUG("initialize() of MuonGeometryManagerTool");

  if (!CommonGeometryManagerTool::initialize().isSuccess()) {
    ATH_MSG_FATAL("CommonGeometryManagerTool not initialized");
    return StatusCode::FAILURE;
  }

  // MdtGeometryManagerTool
  if (m_mdtGeoManager.retrieve().isSuccess()) 
    ATH_MSG_INFO("Retrieved "<<m_mdtGeoManager);
  else {
    ATH_MSG_FATAL("Could not get "<<m_mdtGeoManager);
    return StatusCode::FAILURE;
  }

  // CscGeometryManagerTool
  if (m_cscGeoManager.retrieve().isSuccess()) 
    ATH_MSG_INFO("Retrieved "<<m_cscGeoManager);
  else {
    ATH_MSG_FATAL("Could not get "<<m_cscGeoManager);
    return StatusCode::FAILURE;
  }

  // TgcGeometryManagerTool
  if (m_tgcGeoManager.retrieve().isSuccess()) 
    ATH_MSG_INFO("Retrieved "<<m_tgcGeoManager);
  else {
    ATH_MSG_FATAL("Could not get "<<m_tgcGeoManager);
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
  
//________________________________________________________________________
StatusCode MuonGeometryManagerTool::finalize() 
{  
  ATH_MSG_DEBUG("finalize() of MuonGeometryManagerTool");

  if (!CommonGeometryManagerTool::finalize().isSuccess()) {
    ATH_MSG_FATAL("CommonGeometryManagerTool finalize not successful");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//________________________________________________________________________
int MuonGeometryManagerTool::ReadGeometry(int)
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
void MuonGeometryManagerTool::buildGeometry()
{
  int idHash = 0;

  if (m_alignMdt) {
    ATH_MSG_DEBUG("setting first idHash to "<<idHash<<" for MDT");
    m_mdtGeoManager->setFirstIDHash(idHash);
    m_mdtGeoManager->setModuleList(m_alignModuleListPtr);
    m_mdtGeoManager->setHashMaps(m_idHashToAlignModuleMapsPtr);   
    m_mdtGeoManager->buildGeometry();
    
    idHash=m_mdtGeoManager->getNextIDHash();    
  }
  if (m_alignCsc) {
    ATH_MSG_DEBUG("setting first idHash to "<<idHash<<" for CSC");
    m_cscGeoManager->setFirstIDHash(idHash);
    m_cscGeoManager->setModuleList(m_alignModuleListPtr);
    m_cscGeoManager->setHashMaps(m_idHashToAlignModuleMapsPtr);   
    m_cscGeoManager->buildGeometry();
    
    idHash=m_cscGeoManager->getNextIDHash();
  }
  if (m_alignTgc) {
    ATH_MSG_DEBUG("setting first idHash to "<<idHash<<" for TGC");
    m_tgcGeoManager->setFirstIDHash(idHash);
    m_tgcGeoManager->setModuleList(m_alignModuleListPtr);
    m_tgcGeoManager->setHashMaps(m_idHashToAlignModuleMapsPtr);   
    m_tgcGeoManager->buildGeometry();
    
    idHash=m_tgcGeoManager->getNextIDHash();
  }
  
  return;
}
  
//________________________________________________________________________
void MuonGeometryManagerTool::addModuleParameters(Trk::AlignModule * module, 
						  DataVector<DataVector<Trk::AlignPar> >* allFullModPars, 
						  DataVector<DataVector<Trk::AlignPar> >* allActiveModPars)
{
  ATH_MSG_DEBUG("in addModuleParameters");

  Trk::AlignModule::DetectorType detType=Trk::AlignModule::unidentified;
  if (isOneDetOnly(module,detType)) {
    ATH_MSG_DEBUG("isOneDetOnly, detType="<<detType);
    if      (detType==Trk::AlignModule::MDT)
      m_mdtGeoManager->addModuleParameters(module,allFullModPars,allActiveModPars);
    else if (detType==Trk::AlignModule::CSC)
      m_cscGeoManager->addModuleParameters(module,allFullModPars,allActiveModPars);
    else if (detType==Trk::AlignModule::TGC)
      m_tgcGeoManager->addModuleParameters(module,allFullModPars,allActiveModPars);
    else {
      ATH_MSG_WARNING("problem in addModuleParameters!");
    }
    ATH_MSG_DEBUG("returning from addModuleParameters");
    return;
  }

  // prepare all parameters
  ATH_MSG_DEBUG("preparing all parameters");
  DataVector<Trk::AlignPar> * fullModPars = new DataVector<Trk::AlignPar>(SG::OWN_ELEMENTS);
  fullModPars->push_back(new Trk::AlignPar(module,Trk::AlignModule::TransX));
  fullModPars->push_back(new Trk::AlignPar(module,Trk::AlignModule::TransY));
  fullModPars->push_back(new Trk::AlignPar(module,Trk::AlignModule::TransZ));
  fullModPars->push_back(new Trk::AlignPar(module,Trk::AlignModule::RotX));
  fullModPars->push_back(new Trk::AlignPar(module,Trk::AlignModule::RotY));
  fullModPars->push_back(new Trk::AlignPar(module,Trk::AlignModule::RotZ));
  
  // set sigmas
  ATH_MSG_DEBUG("setting sigmas");
  setSigmas(fullModPars);
  
  ATH_MSG_DEBUG("setting active parameters");
  // select active parameters based on jobOption properties
  DataVector<Trk::AlignPar> * activeModPars = new DataVector<Trk::AlignPar>(SG::VIEW_ELEMENTS);
  for(unsigned int ipar=0;ipar<fullModPars->size();++ipar) {
    
    if(  (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransX && alignTraX()) ||
	 (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransY && alignTraY()) ||
	 (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransZ && alignTraZ()) ||
	 (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotX   && alignRotX()) ||
	 (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotY   && alignRotY()) ||
	 (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotZ   && alignRotZ()) ) {
      ATH_MSG_DEBUG("parameter type "
		    <<fullModPars->at(ipar)->paramType()<<" \'"
		    <<fullModPars->at(ipar)->dumpType()<<"\' is now active");
      activeModPars->push_back(fullModPars->at(ipar));
    }
    else
      ATH_MSG_DEBUG("parameter type "
		    <<fullModPars->at(ipar)->paramType()<<" \'"
		    <<fullModPars->at(ipar)->dumpType()<<"\' is NOT active");
  }
  
  // now add parameters to the list
  allFullModPars->push_back(fullModPars);
  allActiveModPars->push_back(activeModPars);
}

//________________________________________________________________________
bool MuonGeometryManagerTool::isOneDetOnly(const Trk::AlignModule* mod,
					   Trk::AlignModule::DetectorType& detType) const
{
  ATH_MSG_DEBUG("in isOneDetOnly");
  
  Trk::AlignModule::DetectorType firstDetType=Trk::AlignModule::unidentified;
  
  for (int i=1;i<Trk::AlignModule::NDetectorTypes; i++) {
    const Trk::AlignModule::DetElementCollection* coll = 
      mod->detElementCollection((Trk::AlignModule::DetectorType)i);
    if (coll && coll->size()>0) { 
      if (firstDetType==Trk::AlignModule::unidentified)
	firstDetType=(Trk::AlignModule::DetectorType)i;
      else {
	ATH_MSG_DEBUG("more than one detector type!");
	return false;
      }
    }
  }

  detType=firstDetType;
  if (detType==Trk::AlignModule::MDT ||
      detType==Trk::AlignModule::CSC ||
      detType==Trk::AlignModule::TGC) return true;
  
  return false;
}

} // end namespace
