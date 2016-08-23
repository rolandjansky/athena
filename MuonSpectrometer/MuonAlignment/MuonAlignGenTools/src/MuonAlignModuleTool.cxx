/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "DataModel/DataVector.h"

#include "TrkPrepRawData/PrepRawData.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkAlignEvent/AlignTSOS.h"
#include "TrkAlignEvent/AlignModule.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrack/Track.h"

#include "MuonAlignEvent/CombinedMuonAlignModule.h"
#include "MuonAlignGenTools/MuonAlignModuleTool.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorElement.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"

namespace Muon {

  //________________________________________________________________________
  MuonAlignModuleTool::MuonAlignModuleTool(const std::string& type,
					 const std::string& name,
					 const IInterface* parent)
    : Trk::AlignModuleTool(type,name,parent)
    , p_muonMgr(0)

  {
    //declareInterface<Trk::IAlignModuleTool>(this);

  }
  
  //________________________________________________________________________
  MuonAlignModuleTool::~MuonAlignModuleTool() 
  {
    for (int i=0;i<(int)m_origTransforms.size();i++) 
      delete m_origTransforms[i];
  }

  //_______________________________________________________________________
  StatusCode MuonAlignModuleTool::initialize() 
  {
    msg(MSG::DEBUG) << "initialize() of MuonAlignModuleTool" << endmsg;  
  
    StatusCode sc = AlignModuleTool::initialize();
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Unable to initialize AlignModuleTool");
      return sc;
    }

    sc=detStore()->retrieve(p_muonMgr);
    if (sc.isFailure()) {
      msg(MSG::FATAL)<<"Cannot retrieve MuonDetectorManager"<<endmsg;
      return sc;
    }
    else msg(MSG::DEBUG)<<"retrieved MuonDetectorManager"<<endmsg;
    
    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  StatusCode MuonAlignModuleTool::finalize() 
  {  
    StatusCode sc = AlignModuleTool::finalize();
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Unable to finalize AlignModuleTool");
      return sc;
    }

    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  void MuonAlignModuleTool::shiftModule(const Trk::AlignModule* module, 
					const Trk::Track* track, 
					Trk::AlignModule::TransformParameters dimension, 
					double value) const 
  {
    ATH_MSG_DEBUG("in shiftModule, dimension="<<dimension
		    <<", value="<<value);
        
    if (m_origTransforms.size()>0) {
      ATH_MSG_WARNING("shifting module without restoring!");
      restoreModule(module);   
    }
    m_origTransforms.assign(Trk::AlignModule::NDetectorTypes,(std::vector<Amg::Transform3D*>*)0);

    // loop over track and get the detelements from the track that are in this AlignModule
    std::vector<Identifier> mdtReadoutElementIDs;
    std::vector<Identifier> cscReadoutElementIDs;
    std::vector<Identifier> tgcReadoutElementIDs;
    std::vector<Identifier> rpcReadoutElementIDs;
    
    // loop over track and shift tubes/surfaces in this AlignModule
    for (std::vector<const Trk::TrackStateOnSurface*>::const_iterator itTsos=
	   track->trackStateOnSurfaces()->begin();
	 itTsos!=track->trackStateOnSurfaces()->end(); ++itTsos) {
      
      // this will continue if TSOS is a scatterer and NOT a measurement
      if ((**itTsos).type(Trk::TrackStateOnSurface::Scatterer) ||
	  (**itTsos).type(Trk::TrackStateOnSurface::Perigee)) continue;

      const Trk::MeasurementBase*      mesb  = (**itTsos).measurementOnTrack();
      const Trk::RIO_OnTrack*          rio   = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
      const Trk::CompetingRIOsOnTrack* crio  = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);

      if (!rio && crio) rio=&crio->rioOnTrack(0);
      if (!rio) continue;

      std::vector<const Trk::TrkDetElementBase*> detelements;
      if (crio) {
	ATH_MSG_VERBOSE("have CRIO");
	for (unsigned int irio = 0; irio < crio->numberOfContainedROTs(); irio++) {
	  ATH_MSG_VERBOSE("irio "<<irio);
	  const Trk::RIO_OnTrack* trio = &crio->rioOnTrack(irio);
	  detelements.push_back(trio->associatedSurface().associatedDetectorElement());
	}
	ATH_MSG_VERBOSE("crio->assocSurface().associatedDetElemId()="<<
			crio->associatedSurface().associatedDetectorElementIdentifier());
      }
      else {
	//detelements.push_back(mesb->associatedSurface().associatedDetectorElement());      
	detelements.push_back(rio->associatedSurface().associatedDetectorElement());      
	ATH_MSG_VERBOSE("no crio, rio->associatedSurface().associatedDetectorElementIdentifier()="<<
			rio->associatedSurface().associatedDetectorElementIdentifier());
      }      
    
      // MDT
      if (!m_idHelper->is_muon(rio->identify())) continue; // this is necessary until AtlasDetDescr bug fixed

      if (m_idHelper->is_mdt(rio->identify())) {
	
	ATH_MSG_VERBOSE("storing MDT hit");
	
	// MdtReadoutElements are stored in AlignModule, so get MdtReadoutElement from atsos
	const MuonGM::MdtReadoutElement* mdt=
	  dynamic_cast<const MuonGM::MdtReadoutElement*>(detelements[0]);
	
	if (!mdt) {
	  ATH_MSG_ERROR("no MdtReadoutElement for Measurement!");
	  continue;
	}
	ATH_MSG_VERBOSE("finding MdtAlignModule");

	if (findAlignModule(mdt,Trk::AlignModule::MDT)) {
	  std::vector<Identifier>::iterator it=find(mdtReadoutElementIDs.begin(),
						    mdtReadoutElementIDs.end(),
						    mdt->identify());
	  if (it==mdtReadoutElementIDs.end()) {
	    mdtReadoutElementIDs.push_back(mdt->identify());      
	    ATH_MSG_DEBUG("pushed back "<<mdt->identify());
	  }
	}
      }
      // CSC
      else if (m_idHelper->is_csc(rio->identify())) {
	
	ATH_MSG_VERBOSE("storing CSC hit");
	
	// CscReadoutElements are stored in AlignModule, so get CscReadoutElement from atsos
	const MuonGM::CscReadoutElement* csc=
	  dynamic_cast<const MuonGM::CscReadoutElement*>(detelements[0]);
	
	if (!csc) {
	  ATH_MSG_ERROR("no CscReadoutElement for Measurement!");
	  continue;
	}
	ATH_MSG_VERBOSE("finding CscAlignModule");

	if (findAlignModule(csc,Trk::AlignModule::CSC)) {
	  std::vector<Identifier>::iterator it=find(cscReadoutElementIDs.begin(),
						    cscReadoutElementIDs.end(),
						    csc->identify());
	  if (it==cscReadoutElementIDs.end()) {
	    cscReadoutElementIDs.push_back(csc->identify());
	    ATH_MSG_DEBUG("pushed back "<<csc->identify());
	  }
	}
      }
      // TGC 
      else if (m_idHelper->is_tgc(rio->identify())) {
	
	ATH_MSG_VERBOSE("storing TGC hit");

	for (int idet=0;idet<(int)detelements.size();idet++) {
	  const MuonGM::TgcReadoutElement* tgc=
	    dynamic_cast<const MuonGM::TgcReadoutElement*>(detelements[idet]);
	  if (!tgc) { 
	    ATH_MSG_ERROR("no TgcReadoutElement for Measurement!");
	    continue;
	  }
	  if (findAlignModule(tgc,Trk::AlignModule::TGC)) {	    
	    std::vector<Identifier>::iterator it=find(tgcReadoutElementIDs.begin(),
						      tgcReadoutElementIDs.end(),
						      tgc->identify());
	    if (it==tgcReadoutElementIDs.end()) {
	      tgcReadoutElementIDs.push_back(tgc->identify());      
	      ATH_MSG_DEBUG("pushed back "<<tgc->identify());
	    }
	  }
        }
      }      
      // RPC
      else if (m_idHelper->is_rpc(rio->identify())) {
	
	if (!m_idHelper->is_muon(rio->identify()))
	  ATH_MSG_ERROR("is rpc but not muon!!!!");

	ATH_MSG_VERBOSE("storing RPC hit");
	
	for (int idet=0;idet<(int)detelements.size();idet++) {
	  ATH_MSG_DEBUG("idet="<<idet);
	  const MuonGM::RpcReadoutElement* rpc=
	    dynamic_cast<const MuonGM::RpcReadoutElement*>(detelements[idet]);
	  if (!rpc) {
	    ATH_MSG_ERROR("no RpcReadoutElement for Measurement!");
	    continue;
	  }
	  if (findAlignModule(rpc,Trk::AlignModule::RPC)) {	    
	    std::vector<Identifier>::iterator it=find(rpcReadoutElementIDs.begin(),
						      rpcReadoutElementIDs.end(),
						      rpc->identify());
	    if (it==rpcReadoutElementIDs.end()) {
	      rpcReadoutElementIDs.push_back(rpc->identify());      
	      ATH_MSG_DEBUG("pushed back "<<rpc->identify());
	    }
	  }
	}
      }
    }
    ATH_MSG_DEBUG("this track has "
		  <<mdtReadoutElementIDs.size()<<" mdt elements, "
		  <<cscReadoutElementIDs.size()<<" csc elements, "
		  <<tgcReadoutElementIDs.size()<<" tgc elements, "
		  <<rpcReadoutElementIDs.size()<<" rpc elements");
    
    if (mdtReadoutElementIDs.size()>0) 
      shiftDetElements(Trk::AlignModule::MDT,mdtReadoutElementIDs,
		       module,track,dimension,value);
    if (cscReadoutElementIDs.size()>0) 
      shiftDetElements(Trk::AlignModule::CSC,cscReadoutElementIDs,
		       module,track,dimension,value);
    if (tgcReadoutElementIDs.size()>0) 
      shiftDetElements(Trk::AlignModule::TGC,tgcReadoutElementIDs,
		       module,track,dimension,value);
    if (rpcReadoutElementIDs.size()>0) 
      shiftDetElements(Trk::AlignModule::RPC,rpcReadoutElementIDs,
		       module,track,dimension,value);
    m_shiftedMuonStations.clear();
    return;
  }

  //________________________________________________________________________
  void MuonAlignModuleTool::shiftDetElements(Trk::AlignModule::DetectorType /*detType*/,
					     const std::vector<Identifier>& /*readoutElementIDs*/,
					     const Trk::AlignModule* /*module*/,
					     const Trk::Track* /*track*/,
					     Trk::AlignModule::TransformParameters /*dimension*/, 
					     double /*value*/) const
  {

    ATH_MSG_ERROR("need to migrate shiftDetElements method to Eigen");
    return;

    /*
    ATH_MSG_DEBUG("in shiftDetElements, detType "<<detType);

    const Trk::AlignModule::DetElementCollection* detelements=
      module->detElementCollection(detType);
    
    if (0==detelements) return;

    m_origTransforms[detType]=new std::vector<HepGeom::Transform3D*>;
    m_origTransforms[detType]->assign(detelements->size(),0);
    for (int ielem=0;ielem<(int)detelements->size();ielem++) {

      // see if this detelement is on the track
      bool goodDetElement=false;
      for (int i=0;i<(int)readoutElementIDs.size();i++) {
	
	if ((*detelements)[ielem] &&
	    (*detelements)[ielem]->identify()==readoutElementIDs[i]) {
	  goodDetElement=true;
	  ATH_MSG_VERBOSE("found goodDetElement, leaving loop over detelements");
	  break;
	}
      }
      if (!goodDetElement) continue;
      
      const Trk::TrkDetElementBase* detelement=(*detelements)[ielem];
      
      // A-lines applied at MuonStation level
      MuonGM::MuonStation* ms=
	const_cast<MuonGM::MuonStation*>(dynamic_cast<const MuonGM::MuonReadoutElement*>(detelement)->parentMuonStation());
            
      if (find(m_shiftedMuonStations.begin(),
	       m_shiftedMuonStations.end(),ms)==m_shiftedMuonStations.end()) {

	m_shiftedMuonStations.push_back(ms);
	
        // get original transform
        HepGeom::Transform3D* orig_transform=new HepGeom::Transform3D(*(ms->getGeoTransform()->getDelta()));
	ATH_MSG_DEBUG("setting orig transform for ielem "<<ielem<<", id "<<(*detelements)[ielem]->identify());
        (*(m_origTransforms[detType]))[ielem]=orig_transform;

        // add shift to original transform
        double shifts[6];
        for (int i=0;i<6;i++) shifts[i]=0.;
        if (dimension==Trk::AlignModule::TransX || 
	    dimension==Trk::AlignModule::TransY || 
	    dimension==Trk::AlignModule::TransZ) 
	  shifts[dimension]=value; // shift value in mm
        else 
	  shifts[dimension]=value; // shift value in rad
        ATH_MSG_DEBUG("shifts["<<dimension<<"]="<<shifts[dimension]);
	
	// shift in alignment frame
	HepGeom::Transform3D delta_alignmentframe= 
	  HepGeom::TranslateX3D(shifts[0]) *
	  HepGeom::TranslateY3D(shifts[1]) *
	  HepGeom::TranslateZ3D(shifts[2]) *
	  HepGeom::RotateX3D(shifts[3]) *
	  HepGeom::RotateY3D(shifts[4]) *
	  HepGeom::RotateZ3D(shifts[5]);

	// transform to det element frame
	const Amg::Transform3D* alignmodule_to_detelement=
	  module->alignModuleToDetElementTransform(detType,detelement);

	HepGeom::Transform3D native_to_amdb = *(ms->getNativeToAmdbLRS());

	HepGeom::Transform3D delta_native = 
	  native_to_amdb.inverse() * (*alignmodule_to_detelement) * delta_alignmentframe * 
	  alignmodule_to_detelement->inverse() * native_to_amdb;
	
	HepGeom::Transform3D xform=delta_native * (*orig_transform); 

	ms->getGeoTransform()->setDelta( (*orig_transform) * delta_native );
      }
            
      // loop over track and shift tubes/surfaces in this detector element
      for (std::vector<const Trk::TrackStateOnSurface*>::const_iterator itTsos=
	     track->trackStateOnSurfaces()->begin();
	   itTsos!=track->trackStateOnSurfaces()->end(); ++itTsos) {
	
	if ((**itTsos).type(Trk::TrackStateOnSurface::Scatterer) ||
	    (**itTsos).type(Trk::TrackStateOnSurface::Perigee)) continue;
	
	const Trk::MeasurementBase*      mesb  = (**itTsos).measurementOnTrack();
	const Trk::RIO_OnTrack*          rio   = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
	const Trk::CompetingRIOsOnTrack* crio  = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);      
	if (!rio && crio) rio=crio->rioOnTrack(0);
	if (!rio) continue;
	
	const Trk::TrkDetElementBase* hitDetelement = crio ?
	  crio->rioOnTrack(0)->associatedSurface().associatedDetectorElement() :
	  mesb->associatedSurface().associatedDetectorElement();
	Identifier hitId=hitDetelement->identify();
	
	if (//!(*atsos)->module() ||
	    hitId != detelement->identify() ) continue;
	
	// RIO_OnTrack
	if (rio && !crio) {
	  
	  // is this an MDT or CSC hit?
	  if (!m_idHelper->is_mdt(hitId) && 
	      !m_idHelper->is_csc(hitId) &&
	      !m_idHelper->is_tgc(hitId)) ATH_MSG_ERROR("something wrong, not MDT or CSC hit");
	  
	  ATH_MSG_DEBUG("calling shiftSurface for rio "<<rio->identify()<<", detelement "
			<<detelement->identify());
	  
	  shiftSurface(module,detelement,rio->identify());
	}
	
	// CompetingRIOsOnTrack
	else {
	  
	  // is it a TGC hit?
	  if (!m_idHelper->is_tgc(hitId) &&
	      !m_idHelper->is_rpc(hitId)) ATH_MSG_ERROR("something wrong, not TGC or RPC hit");
	  
	  ATH_MSG_DEBUG("TGC hit pos before: "<<
			*(crio->associatedSurface().localToGlobal(crio->localParameters())));
	  ATH_MSG_DEBUG("crio->assocSurface()="<<&(crio->associatedSurface()));
	  if (dynamic_cast<const Trk::PlaneSurface*>(&crio->associatedSurface()))
	    ATH_MSG_DEBUG("crio->planeSurface: "<<dynamic_cast<const Trk::PlaneSurface*>(&crio->associatedSurface()));
	  
	  
	  ATH_MSG_DEBUG("crio->assocSurface().associatedDetectorElementIdentifier="<<crio->associatedSurface().associatedDetectorElementIdentifier());
	  
	  shiftSurface(module,detelement,rio->identify());
	  
	  ATH_MSG_DEBUG("TGC hit pos after: "<<
			*(crio->associatedSurface().localToGlobal(crio->localParameters())));
	  ATH_MSG_DEBUG("crio->assocSurface()="<<&(crio->associatedSurface()));
	  ATH_MSG_DEBUG("crio->assocSurface().associatedDetectorElementIdentifier="<<crio->associatedSurface().associatedDetectorElementIdentifier());
	  
	}
      }	
      
    }
    ATH_MSG_DEBUG("returning from shiftDetElements");
    return;
    */

  }
   
  //________________________________________________________________________
  void MuonAlignModuleTool::restoreModule(const Trk::AlignModule* /*module*/) const 
  {
    ATH_MSG_ERROR("migrate restoreModule to Eigen");
    return;

    /*
    ATH_MSG_DEBUG("in restoreModule");

    const Muon::CombinedMuonAlignModule* combModule=
      dynamic_cast<const Muon::CombinedMuonAlignModule*>(module);
    if (!combModule) {
      ATH_MSG_ERROR("not CombinedMuonAlignModule!");
      return;
    }

    MuonGM::MuonStation* current_ms(0);

    for (int idetType=0;idetType<(int)m_origTransforms.size();idetType++) {
      if (!m_origTransforms[idetType]) continue;
      
      const Trk::AlignModule::DetElementCollection* detelements=
	module->detElementCollection(Trk::AlignModule::DetectorType(idetType));

      if (0==detelements) continue;
      
      for (int ielem=0;ielem<(int)detelements->size();ielem++) {
	
	const MuonGM::MuonReadoutElement* const constmre=
	  dynamic_cast<const MuonGM::MuonReadoutElement*>((*detelements)[ielem]);
	
	
	MuonGM::MuonStation* ms=const_cast<MuonGM::MuonStation*>(constmre->parentMuonStation());
	
	HepGeom::Transform3D* origTransform=(*(m_origTransforms[idetType]))[ielem];
	if (origTransform){ 
	  HepGeom::Transform3D xform=*origTransform;
	  ms->getGeoTransform()->setDelta(*origTransform);
	}
	
	if(ms!=current_ms){      
          current_ms=ms;
          if(0==origTransform) continue;
	}
	

	Trk::TrkDetElementBase* detelement = const_cast<Trk::TrkDetElementBase*>((*detelements)[ielem]);
	combModule->restoreSurfaces(detelement);

      }
      delete m_origTransforms[idetType];      
    }
    
    m_origTransforms.clear();
    
    return;
    */

  }

  //________________________________________________________________________
  void MuonAlignModuleTool::shiftSurface(const Trk::AlignModule* module,
					 const Trk::TrkDetElementBase* detelement, 
					 const Identifier id) const
  {
    const Muon::CombinedMuonAlignModule* combModule=
      dynamic_cast<const Muon::CombinedMuonAlignModule*>(module);
    if (!combModule) {
      ATH_MSG_ERROR("not CombinedMuonAlignModule!");
      return;
    }
    combModule->shiftSurface(const_cast<Trk::TrkDetElementBase*>(detelement),id);       

    return;
  }

  //________________________________________________________________________
  void MuonAlignModuleTool::setMeasurementProperties(Trk::AlignTSOS* alignTSOS) const
  {
    bool measPhi=false;
    
    // eta measurements need measurement direction set to Trk::y
    if (alignTSOS->measType()==Trk::TrackState::CSC) {
      Identifier id = alignTSOS->rio()->prepRawData()->identify();
      measPhi = p_muonMgr->cscIdHelper()->measuresPhi(id);
    }
    else if (alignTSOS->measType()==Trk::TrackState::TGC) {
      Identifier id = alignTSOS->rio()->prepRawData()->identify();
      measPhi = p_muonMgr->tgcIdHelper()->isStrip(id);
    }
    else if (alignTSOS->measType()==Trk::TrackState::RPC) {
      Identifier id = alignTSOS->rio()->prepRawData()->identify();
      measPhi = p_muonMgr->rpcIdHelper()->measuresPhi(id);
    }
    else if (alignTSOS->measType()==Trk::TrackState::MDT) 
      measPhi = true;

    if (!measPhi) alignTSOS->setMeasDir(Trk::y);
    return;
  }

  //________________________________________________________________________
  int MuonAlignModuleTool::subDetElementIDHash(Identifier id) const
  {
    if (id.is_valid()) {
      if (m_idHelper->is_mdt(id))
	return p_muonMgr->getMdtReadoutElement(id)->identifyHash();
      else if (m_idHelper->is_csc(id))
	return p_muonMgr->getCscReadoutElement(id)->identifyHash();
      else if (m_idHelper->is_tgc(id))
	return p_muonMgr->getTgcReadoutElement(id)->identifyHash();
    }
    return 0;  
  }
  
} // end namespace
