/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AtlasDetDescr/AtlasDetectorID.h"

#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/Track.h"

#include "TrkAlignEvent/AlignPar.h"
#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignModuleList.h"

#include "TrkAlignGenTools/AlignModuleTool.h"

namespace Trk {

  //______________________________________________________________
  AlignModuleTool::AlignModuleTool(const std::string& type, const std::string& name,
           const IInterface* parent)
    
    : AthAlgTool(type,name,parent)
    , m_idHelper         (nullptr)
    , m_alignModules     (nullptr)
    , m_alignModuleMaps  (AlignModule::NDetectorTypes,(const AlignModuleList*)nullptr)
    , m_alignParList     (nullptr)
    , m_fullAlignParList (nullptr)
    , m_alignParList1D   (SG::VIEW_ELEMENTS)
    , m_subDetElement    (AlignModule::NDetectorTypes,false)
  {
    declareInterface<IAlignModuleTool>(this);
    
    declareProperty("AlignModuleListType", m_alignModuleListType = Trk::L3);

    m_logStream = nullptr;
  }

  //________________________________________________________________________
  AlignModuleTool::~AlignModuleTool()
  {

  }

  //________________________________________________________________________
  StatusCode AlignModuleTool::initialize()
  {
    // Number of layers in the superstructures
    if (m_alignModuleListType>Trk::L3 || m_alignModuleListType<Trk::L0) {
      msg(MSG::FATAL)<<"AlignModuleListType can be 0, 1, 2, or 3" << endmsg;
      return StatusCode::FAILURE;
    }

    msg(MSG::INFO).setColor(MSG::GREEN);
    ATH_MSG_INFO("AlignModuleListType set to " << m_alignModuleListType);

    // Set up ATLAS ID helper 
    if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
      msg(MSG::FATAL)<<"Could not get Atlas ID helper"<<endmsg;
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  StatusCode AlignModuleTool::finalize()
  {

    return StatusCode::SUCCESS;
  }
  
  //________________________________________________________________________
  void AlignModuleTool::setAlignModules(AlignModuleList* alignmods,
          std::vector<AlignModuleList*>* idHashMaps)
  {
    ATH_MSG_DEBUG("in setAlignModules");

    m_alignModules=alignmods;

    for (int i=0;i<(int)idHashMaps->size();i++) {
      if ((*idHashMaps)[i]) m_alignModuleMaps[i]=(*idHashMaps)[i];
    } 

    // reset 1-D collection
    ATH_MSG_DEBUG("clearing alignModules1D");
    int nmods(0);
    for (int imod=0;imod<(int)m_alignModules->size();imod++) {
      m_alignModules1D.push_back((*m_alignModules)[imod]);
      (*m_alignModules)[imod]->setIdHash(nmods++); // this overwrites what's set in geometry manager tool... should we do this?
    }
    
    ATH_MSG_DEBUG("done setting AlignModules");
 }


  //________________________________________________________________________
  void AlignModuleTool::setAlignParLists(AlignPar2DVec* fullAlignParList, 
           AlignPar2DVec* alignParList) 
  { 
    ATH_MSG_DEBUG("setting fullAlignParList="<<fullAlignParList
      <<", alignParList="<<alignParList);

    m_fullAlignParList = fullAlignParList;
    m_alignParList     = (alignParList) ? alignParList : fullAlignParList;

    // recalculate 1-D collection of alignment parameters
    ATH_MSG_DEBUG("getting AlignParList1D");
    m_alignParList1D.clear();
    int index(0);
    
    AlignPar2DVec* aplist=m_alignParList;
    
    for (AlignPar2DVec::iterator apVec=aplist->begin(); apVec!=aplist->end();++apVec) {
      for (AlignParVec::iterator ap=(*apVec)->begin(); ap!=(*apVec)->end();++ap) {
        m_alignParList1D.push_back(*ap);
        (*ap)->setIndex(index++);
      }
      
    }
     }

  //________________________________________________________________________  
  AlignModule* AlignModuleTool::findAlignModule(const TrkDetElementBase* det,
            AlignModule::DetectorType detType) const
  {
    ATH_MSG_DEBUG("in AlignModuleTool::findAlignModule(det,detType)");

    if (!det) {
      ATH_MSG_VERBOSE("no det!");
      return nullptr;
    }

    if (m_subDetElement[detType]) {
      ATH_MSG_ERROR("please use findAlignModule method passing RIO_OnTrack");
      return nullptr;
    }
    
    ATH_MSG_VERBOSE("in findAlignModule, detType="<<detType);
    
    if (detType==AlignModule::NDetectorTypes)
      detType=getDetectorType(det);
    if (detType==AlignModule::NDetectorTypes || !m_alignModuleMaps[detType]) {
      ATH_MSG_VERBOSE("bad detector type or no map for detType "<<detType);
      return nullptr;
    }
    
    // find align module
    int idHash = det->identifyHash();    
    return (*m_alignModuleMaps[detType])[idHash];
    
  }  

  //________________________________________________________________________  
  AlignModule * AlignModuleTool::findAlignModule(const RIO_OnTrack * rio,
                                                 AlignModule::DetectorType detType) const
  {

    ATH_MSG_DEBUG("in AlignModuleTool::findAlignModule(rio,detType)");

    if (!rio) {
      ATH_MSG_VERBOSE("no rio!");
      return nullptr;
    }

    // find detector type using TrkDetElementBase
    const TrkDetElementBase * detelement = rio->detectorElement();
    if (detelement) {
      ATH_MSG_DEBUG("have detelement");

      if (detType==AlignModule::NDetectorTypes)
        detType=getDetectorType(detelement);
      if (detType==AlignModule::NDetectorTypes || !m_alignModuleMaps[detType]) {
        ATH_MSG_VERBOSE("bad detector type or no map for detType "<<detType);
        return nullptr;
      }
    
      // find align module
      int idHash(0);
      if (m_subDetElement[detType]) {
        idHash = subDetElementIDHash(rio->identify());      
        ATH_MSG_DEBUG("idHash (from subDetElement): "<<idHash);      
      }
      else {
        idHash = detelement->identifyHash();
        ATH_MSG_DEBUG("idHash: "<<idHash);            
      }

      return (*m_alignModuleMaps[detType])[idHash];
      
    }

    ATH_MSG_VERBOSE("no detector element!");
    return nullptr;
  }
  
  
  //________________________________________________________________________
  DataVector<AlignPar>* AlignModuleTool::getAlignPars(const AlignModule* alignModule) const
  {
    if (!alignModule) return nullptr;
    
    int idHash =alignModule->identifyHash();
    ATH_MSG_DEBUG("getting alignPars for idHash "<<idHash);

    return (*m_alignParList)[idHash];
  }

  //________________________________________________________________________
  void AlignModuleTool::fillDetAlignmentParameters(const TrkDetElementBase* det,
               DataVector<AlignPar>* detAPVec,
               AlignModule::DetectorType detType) const
  {
    if (detType==AlignModule::NDetectorTypes)
      detType=getDetectorType(det);
    if (detType==AlignModule::NDetectorTypes || !m_alignModuleMaps[detType]) return;
    
    // get AlignModule
    int idHash=det->identifyHash();
    AlignModule* alignModule=(*(m_alignModuleMaps[detType]))[idHash];
    
    ATH_MSG_DEBUG("have alignModule "<<alignModule->identify());
    // get alignment parameters
    AlignParVec* modAlignPar = getFullAlignPars(alignModule);
    ATH_MSG_DEBUG("modAlignPar size: "<<modAlignPar->size());
    
    double alignModPar[AlignModule::NTransformPar];
    double alignModErr[AlignModule::NTransformPar];      
    for (int i=0;i<AlignModule::NTransformPar;i++) {
      if ((*modAlignPar)[i]) {
        alignModPar[i]=(*modAlignPar)[i]->par();
        alignModErr[i]=(*modAlignPar)[i]->err();
      }
      else {
        alignModPar[i]=alignModErr[i]=0.;
        ATH_MSG_DEBUG("no alignModPar!");
      }
    }
  
    ATH_MSG_DEBUG("getting transforms");  
  
    Amg::Translation3D alignModXTranslation(alignModPar[AlignModule::TransX],
                                            alignModPar[AlignModule::TransY],
                                            alignModPar[AlignModule::TransZ]);
                                      
    Amg::Transform3D alignModXform = alignModXTranslation * Amg::RotationMatrix3D::Identity();
    alignModXform *= Amg::AngleAxis3D(alignModPar[AlignModule::RotZ], Amg::Vector3D(0.,0.,1.));
    alignModXform *= Amg::AngleAxis3D(alignModPar[AlignModule::RotY], Amg::Vector3D(0.,1.,0.));
    alignModXform *= Amg::AngleAxis3D(alignModPar[AlignModule::RotX], Amg::Vector3D(1.,0.,0.));


    Amg::Translation3D alignModXTranslationErr(alignModErr[AlignModule::TransX],
                                               alignModErr[AlignModule::TransY],
                                               alignModErr[AlignModule::TransZ]);
                                      
    Amg::Transform3D alignModXformErr = alignModXTranslationErr * Amg::RotationMatrix3D::Identity();
    alignModXformErr *= Amg::AngleAxis3D(alignModErr[AlignModule::RotZ], Amg::Vector3D(0.,0.,1.));
    alignModXformErr *= Amg::AngleAxis3D(alignModErr[AlignModule::RotY], Amg::Vector3D(0.,1.,0.));
    alignModXformErr *= Amg::AngleAxis3D(alignModErr[AlignModule::RotX], Amg::Vector3D(1.,0.,0.));
  
    ATH_MSG_DEBUG("getting AlignPars for detector element "<<det->identify());

    // get the transform for this detector element
    const Amg::Transform3D* transform = alignModule->alignModuleToDetElementTransform(detType,det);
    
    // get the alignment parameters for this detector element
    ATH_MSG_WARNING("Check that order is correct: AlignModuleTool:271,284");
    Amg::Transform3D xformPar( (*transform) * alignModXform );
    double* detpars = new double[AlignModule::NTransformPar];
    double* deterrs = new double[AlignModule::NTransformPar];
    decomposeTransform(xformPar,detpars);
    
    ATH_MSG_DEBUG("transx="<<detpars[0]);
    ATH_MSG_DEBUG("transy="<<detpars[1]);
    ATH_MSG_DEBUG("transz="<<detpars[2]);
    ATH_MSG_DEBUG("rotx="  <<detpars[3]);
    ATH_MSG_DEBUG("roty="  <<detpars[4]);
    ATH_MSG_DEBUG("rotz="  <<detpars[5]);
    
    Amg::Transform3D xformErr( (*transform) * alignModXformErr);
    decomposeTransform(xformErr,deterrs);
    
    // set in AlignParList for this detector element
    ATH_MSG_DEBUG("setting AlignPar for detector element ");
    for (int ipar=0;ipar<AlignModule::NTransformPar;ipar++) {
      AlignPar* ap=(*detAPVec)[ipar];
      ap->setPar(detpars[ipar],deterrs[ipar]);
      ap->setFinalPar(ap->initPar()+detpars[ipar], 
                      std::sqrt(ap->initErr()*ap->initErr() + deterrs[ipar]*deterrs[ipar]) );
    }

    ATH_MSG_DEBUG("set");
 
  }

  //________________________________________________________________________
  AlignModule::DetectorType 
  AlignModuleTool::getDetectorType(const TrkDetElementBase* det) const
  {
    Identifier id = det->identify();
    
    // get dector type
    if ( id.is_valid()) {
      if    (m_idHelper->is_pixel(id)) return AlignModule::Pixel;
      else if (m_idHelper->is_sct(id)) return AlignModule::SCT;
      else if (m_idHelper->is_trt(id)) return AlignModule::TRT;
      else if (m_idHelper->is_mdt(id)) return AlignModule::MDT;
      else if (m_idHelper->is_csc(id)) return AlignModule::CSC;
      else if (m_idHelper->is_rpc(id)) return AlignModule::RPC;
      else if (m_idHelper->is_tgc(id)) return AlignModule::TGC;      
    }
    
    return AlignModule::NDetectorTypes;    
  }
 
  //________________________________________________________________________
  DataVector<AlignPar>* AlignModuleTool::getFullAlignPars(const AlignModule* alignModule) const
  {
    int idHash =alignModule->identifyHash();
    return (*m_fullAlignParList)[idHash];
  }

  //________________________________________________________________________
  MsgStream& operator << ( MsgStream& sl, AlignModuleTool& alignModTool)
  {
    const AlignModuleList* modules=alignModTool.alignModules1D();
    for (int imod=0;imod<(int)modules->size(); imod++) {
      sl << "AML: "<<*((*modules)[imod]);
    }
    
    DataVector<AlignPar>* alignParList=alignModTool.alignParList1D();
    for (int iap=0;iap<(int)alignParList->size();iap++) {
      sl << *((*alignParList)[iap]);
    } 
    return sl;
  }
}
