/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthContainers/DataVector.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SiliconID.h"

#include "TrkAlignInterfaces/IAlignModuleTool.h"

#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignEvent/AlignPar.h"


#include "InDetAlignGeomTools/PixelGeometryManagerTool.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include <iostream>

using namespace InDetDD;

namespace InDet {

  //________________________________________________________________________
  PixelGeometryManagerTool::PixelGeometryManagerTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface * parent)
    : AthAlgTool(type,name,parent)
    , m_detManager(0)
    , m_idHelper()
    , m_alignModuleTool("Trk::AlignModuleTool/AlignModuleTool")
    , m_idHashToAlignModuleMaps(Trk::AlignModule::NDetectorTypes,(Trk::AlignModuleList*)0)
    , m_alignParList(0)
    , m_fullAlignParList(0)
    , m_rdbAccessSvc("RDBAccessSvc",name)
    , m_geoModelSvc("GeoModelSvc",name)
  {
    declareInterface<IGeometryManagerTool>(this);
    declareProperty("AlignModuleTool",    m_alignModuleTool);

    declareProperty("AlignBarrel",     m_alignBarrel     = true);
    declareProperty("AlignEndcaps",    m_alignEndcaps    = true);

    declareProperty("AlignBarrelX",    m_alignBarrelX    = true);
    declareProperty("AlignBarrelY",    m_alignBarrelY    = true);
    declareProperty("AlignBarrelZ",    m_alignBarrelZ    = true);
    declareProperty("AlignBarrelRotX", m_alignBarrelRotX = true);
    declareProperty("AlignBarrelRotY", m_alignBarrelRotY = true);
    declareProperty("AlignBarrelRotZ", m_alignBarrelRotZ = true);
    declareProperty("AlignBarrelBowX", m_alignBarrelBowX = false);


    declareProperty("AlignEndcapX",    m_alignEndcapX    = true);
    declareProperty("AlignEndcapY",    m_alignEndcapY    = true);
    declareProperty("AlignEndcapZ",    m_alignEndcapZ    = true);
    declareProperty("AlignEndcapRotX", m_alignEndcapRotX = true);
    declareProperty("AlignEndcapRotY", m_alignEndcapRotY = true);
    declareProperty("AlignEndcapRotZ", m_alignEndcapRotZ = true);

    declareProperty("SetSigmaBarrelX",    m_sigmaBarrelX       = 1.);
    declareProperty("SetSigmaBarrelY",    m_sigmaBarrelY       = 1.);
    declareProperty("SetSigmaBarrelZ",    m_sigmaBarrelZ       = 1.);
    declareProperty("SetSigmaBarrelRotX", m_sigmaBarrelRotX    = 0.001);
    declareProperty("SetSigmaBarrelRotY", m_sigmaBarrelRotY    = 0.001);
    declareProperty("SetSigmaBarrelRotZ", m_sigmaBarrelRotZ    = 0.001);
    declareProperty("SetSigmaBarrelBowX", m_sigmaBarrelBowX    = 1);

    declareProperty("SetSigmaEndcapX",    m_sigmaEndcapX       = 1.);
    declareProperty("SetSigmaEndcapY",    m_sigmaEndcapY       = 1.);
    declareProperty("SetSigmaEndcapZ",    m_sigmaEndcapZ       = 1.);
    declareProperty("SetSigmaEndcapRotX", m_sigmaEndcapRotX    = 0.001);
    declareProperty("SetSigmaEndcapRotY", m_sigmaEndcapRotY    = 0.001);
    declareProperty("SetSigmaEndcapRotZ", m_sigmaEndcapRotZ    = 0.001);

    declareProperty("SetSoftCutBarrelX",    m_softcutBarrelX       = 1.);
    declareProperty("SetSoftCutBarrelY",    m_softcutBarrelY       = 1.);
    declareProperty("SetSoftCutBarrelZ",    m_softcutBarrelZ       = 1.);
    declareProperty("SetSoftCutBarrelRotX", m_softcutBarrelRotX    = 1.);
    declareProperty("SetSoftCutBarrelRotY", m_softcutBarrelRotY    = 1.);
    declareProperty("SetSoftCutBarrelRotZ", m_softcutBarrelRotZ    = 1.);
    declareProperty("SetSoftCutBarrelBowX", m_softcutBarrelBowX    = 1.);

    declareProperty("SetSoftCutEndcapX",    m_softcutEndcapX       = 1.);
    declareProperty("SetSoftCutEndcapY",    m_softcutEndcapY       = 1.);
    declareProperty("SetSoftCutEndcapZ",    m_softcutEndcapZ       = 1.);
    declareProperty("SetSoftCutEndcapRotX", m_softcutEndcapRotX    = 1.);
    declareProperty("SetSoftCutEndcapRotY", m_softcutEndcapRotY    = 1.);
    declareProperty("SetSoftCutEndcapRotZ", m_softcutEndcapRotZ    = 1.);

    // defines alignment level
    declareProperty("AlignmentLevel",        m_alignLevel        =  1);
    declareProperty("AlignmentLevelBarrel",  m_alignLevelBarrel  = -1);
    declareProperty("AlignmentLevelEndcaps", m_alignLevelEndcaps = -1);
    declareProperty("AlignmentDBM",          m_alignDBM          =  false);
    
    declareProperty("doModuleSelection",  m_doModuleSelection  = false);
    declareProperty("ModuleSelection",    m_moduleSelection);

    declareProperty("DumpGeometry",       m_dumpGeometry = true);

    m_hashCounter = 0;
    m_logStream = 0;
  }

  //________________________________________________________________________
  PixelGeometryManagerTool::~PixelGeometryManagerTool()
  {
    ATH_MSG_DEBUG("deleting alignModuleList");
    for (int i=0;i<(int)m_alignModuleList.size();i++)
      delete m_alignModuleList[i];
    m_alignModuleList.clear();

    ATH_MSG_DEBUG("deleting fullAlignParList");
    delete m_fullAlignParList;
    ATH_MSG_DEBUG("deleting alignParList");
    delete m_alignParList;
  }

  //________________________________________________________________________
  StatusCode PixelGeometryManagerTool::initialize()
  {
    ATH_MSG_DEBUG("initialize() of PixelGeometryManagerTool");

    // retrieve AlignModuleTool
    if ( m_alignModuleTool.retrieve().isFailure() ) {
      msg(MSG::FATAL)<<"Could not get " << m_alignModuleTool << endmsg;
      return StatusCode::FAILURE;
    }
    else
      ATH_MSG_INFO("Retrieved " << m_alignModuleTool);

    // retrieve Pixel helper
    if ( detStore()->retrieve(m_idHelper).isFailure() ) {
      msg(MSG::FATAL) << " Cannot retrieve Pixel Helper " << endmsg;
      return StatusCode::FAILURE;
    }
    else
      ATH_MSG_INFO("retrieved Pixel Helper");

    // retrieve PIX detector manager
    if ( detStore()->retrieve(m_detManager, "Pixel").isFailure() ) {
      msg(MSG::FATAL) << " Cannot retrieve PIX Detector Manager " << endmsg;
      return StatusCode::FAILURE;
    }
    else
      ATH_MSG_INFO("retrieved PIX Detector Manager");

    // retrieve geomodel service
    StatusCode sc = m_geoModelSvc.retrieve();
    if (sc.isFailure()) {
      msg(MSG::FATAL) << "Cannot retrieve GeoModelSvc" << endmsg;
      return (StatusCode::FAILURE); 
    }  
    else ATH_MSG_INFO("retrieved GeoModelSvc");

    // retrieve geometry DB access service
    sc = m_rdbAccessSvc.retrieve();
    if (sc.isFailure()) {
      msg(MSG::FATAL) << "Cannot retrieve RDBAccessSvc" << endmsg;
      return (StatusCode::FAILURE); 
    }  
    else ATH_MSG_INFO("retrieved RDBAccessSvc");
    
    // dump module selection
    if(m_doModuleSelection && msgLvl(MSG::INFO)) {
      msg(MSG::INFO)<<"Creating geometry for selected "<<m_moduleSelection.size()<<" modules:"<<endmsg;
      for(unsigned int i=0;i<m_moduleSelection.size();i++)
        msg(MSG::INFO)<<"   "<<i<<".  "<<m_moduleSelection.at(i)<<endmsg;
    }

    // check the allowed geometry levels
    if(!checkAlignLevel())
      return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  StatusCode PixelGeometryManagerTool::finalize()
  {
    ATH_MSG_DEBUG("finalize() of PixelGeometryManagerTool");

    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  void PixelGeometryManagerTool::setAlignLevel(int level)
  {
    m_alignLevelBarrel = m_alignLevelEndcaps = -1;
    m_alignLevel = level;
  }

  //________________________________________________________________________
  bool PixelGeometryManagerTool::checkAlignLevel()
  {
    if(!m_alignDBM && m_alignLevel == 1) {
      ATH_MSG_INFO("Alignment level for Pixel is "<<m_alignLevel);
      return true;
    }

    // new IBL alignment level
    if(!m_alignDBM && m_alignLevel == 11) {
      ATH_MSG_INFO("Alignment level for Pixel is "<<m_alignLevel);
      return true;
    }
    
    // check for DBM
    if(m_alignDBM && (m_alignLevel == 1 || m_alignLevel == 2 || m_alignLevel == 3)) {
      ATH_MSG_INFO("Alignment level for DBM is configured and set to "<<m_alignLevel);
      return true;
    }

    if(m_alignLevelBarrel == -1)
      m_alignLevelBarrel = m_alignLevel;

    if(m_alignLevelEndcaps == -1)
      m_alignLevelEndcaps = m_alignLevel;

    bool ok = checkAlignLevelBarrel() && checkAlignLevelEndcaps();

    // if the alignment level for barrel and EC is set correctly
    // reset the global alignment level
    if(ok)
      m_alignLevel = -1;

    return ok;
  }

  //________________________________________________________________________
  bool PixelGeometryManagerTool::checkAlignLevelBarrel()
  {
    // check whether geometry level is allowed
    switch(m_alignLevelBarrel) {
      case 12: case 16: case 2: case 26: case 27: case 3:
        ATH_MSG_INFO("Alignment level for Pixel Barrel is "<<m_alignLevelBarrel);
        return true;
      case 15:
        if(m_detManager->numerology().numLayers() > 3){
          msg(MSG::FATAL)<<"This level is not supported with the configured geometry"<<endmsg;
          msg(MSG::FATAL)<<"The pixel barrel has "<<m_detManager->numerology().numLayers()<<" layers; Only 3 layer geometries supported"<<endmsg;
          return false;
        }
        else {
          ATH_MSG_INFO("Alignment level for Pixel Barrel is "<<m_alignLevelBarrel);
          return true;
        }
      case 22:
        if(m_detManager->numerology().numLayers() > 3){
          msg(MSG::FATAL)<<"This level is not supported with the configured geometry"<<endmsg;
          msg(MSG::FATAL)<<"The pixel barrel has "<<m_detManager->numerology().numLayers()<<" layers; Only 3 layer geometries supported"<<endmsg;
          return false;
        }
        else {
          ATH_MSG_INFO("Alignment level for Pixel Barrel is "<<m_alignLevelBarrel);
          return true;
        }
      default:
        msg(MSG::FATAL)<<"Alignment level "<<m_alignLevelBarrel<<" does not exist for Pixel Barrel"<<endmsg;
        return false;
    }
  }

  //________________________________________________________________________
  bool PixelGeometryManagerTool::checkAlignLevelEndcaps()
  {
    // check whether geometry level is allowed
    switch(m_alignLevelEndcaps) {
      case 12: case 16: case 2: case 3:
        ATH_MSG_INFO("Alignment level for Pixel Endcaps is "<<m_alignLevelEndcaps);
        return true;
      default:
        msg(MSG::FATAL)<<"Alignment level "<<m_alignLevelEndcaps<<" does not exist for Pixel Endcaps"<<endmsg;
        return false;
    }
  }

  //________________________________________________________________________
  int PixelGeometryManagerTool::ReadGeometry(int solveLevel)
  {
    ATH_MSG_DEBUG("in ReadGeometry() solveLevel="<<solveLevel);

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
    std::vector<Trk::AlignModule *>::const_iterator imod = m_alignModuleList.begin();
    std::vector<Trk::AlignModule *>::const_iterator imod_end = m_alignModuleList.end();
    for( ; imod!=imod_end ; ++imod)
      addModuleParameters(*imod,m_fullAlignParList,m_alignParList);

    // set alignModuleList and hash table in the alignModuleTool
    m_alignModuleTool->setAlignModules(&m_alignModuleList, &m_idHashToAlignModuleMaps);
    ATH_MSG_DEBUG(" geometry set in m_alignModuleTool");

    // set alignPar lists in the alignModuleTool
    ATH_MSG_DEBUG(" alignParList = "<<m_alignParList);
    ATH_MSG_DEBUG(" fullAlignParList = "<<m_fullAlignParList);
    m_alignModuleTool->setAlignParLists(m_fullAlignParList, m_alignParList);
    ATH_MSG_DEBUG(" AlignParLists set in m_alignModuleTool");

    // dump summary about the geometry setup
    if (m_dumpGeometry)
      dumpGeometry();

    int nDoF= m_alignModuleTool->nAlignParameters();
    ATH_MSG_INFO("Total number of degrees of freedom: "<<nDoF);

    return nDoF;
  }

  //_______________________________________________________________________
  void PixelGeometryManagerTool::buildGeometry()
  {
    ATH_MSG_INFO("Preparing the Pixel geometry");
      
    // Get skipetazero value for IBL layer if module with eta=0 value is set (e.g. ATLAS-IBL-03-00-00)
    // if noSkipEtaZero is set, loop over iEta stops earlier ( because eta=0 module is defined)
    etaCorrection = 0;
    int iLayer = 0;
    if (!m_detManager->numerology().useLayer(iLayer))
        ATH_MSG_INFO(" When checking for IBL-Layer, layer "<<iLayer<<" not present");
      
    int noSkipEtaZero=getNoSkipEtaValueFromGeometry();
    if (noSkipEtaZero>0 && m_detManager->numerology().skipEtaZeroForLayer(iLayer)){
      etaCorrection = 1;
    }
    ATH_MSG_DEBUG("IBL-etaCorrection value set to: "<<etaCorrection);

    if(!m_alignDBM && m_alignLevel == 1 && (m_alignLevelBarrel == -1 || m_alignLevelEndcaps == -1))
      buildL1();
    else if (!m_alignDBM && m_alignLevel == 11 && (m_alignLevelBarrel == -1 || m_alignLevelEndcaps == -1))
      buildL11();
    
    else if (m_alignDBM){
      
      switch(m_alignLevel) {
        case 1:
          buildL1DBM();
          break;
        case 2:
          buildL2DBM();
          break;
        case 3:
          buildL3DBM();
          break;
      }
    }
    
    else {

      if(m_alignBarrel)
        switch(m_alignLevelBarrel) {
          case 12:
            buildL12Barrel();
            break;
          case 15:
            buildL15Barrel();
            break;
          case 16:
            buildL16Barrel();
            break;
          case 2:
            buildL2Barrel();
            break;
          case 22:
            buildL22Barrel();
            break;
          case 26:
            buildL26Barrel();
            break;
          case 27:
            buildL27Barrel();
            break;
          case 3:
            buildL3Barrel();
            break;
          default:
            break;
        }

      if(m_alignEndcaps)
        switch(m_alignLevelEndcaps) {
          case 12: case 16:
            buildL12Endcaps();
            break;
          case 2:
            buildL2Endcaps();
            break;
          case 3:
            buildL3Endcaps();
            break;
          default:
            break;
        }

    }
  }

  //_______________________________________________________________________
  void PixelGeometryManagerTool::buildL1()
  {
    ATH_MSG_INFO("Preparing the Pixel geometry for L1 : whole Pixel");
    // ========================================
    // get all modules for Level 1 alignment
    // Level 1 is just one module for Pixel
    // - the whole Pixel

    // get maximum number of elements from the helper
    unsigned int pixelmaxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the Pixel "<<pixelmaxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel) = new Trk::AlignModuleList((size_t)(pixelmaxHash),0);
    Trk::AlignModuleList * pixelIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel);

    // use the 'real' identifier of the Pixel
    Trk::AlignModule * pixel = new Trk::AlignModule(this);
    pixel->setIdHash(getNextIDHash());
    pixel->setName("Pixel");
    pixel->setIdentifier(m_idHelper->wafer_id(0,0,0,0));

    // check if we're aligning Pixel
    if(!m_alignBarrel || !moduleSelected(pixel)) {
      ATH_MSG_DEBUG("Module "<<pixel->name()<<" NOT selected");
      delete pixel;
      return;
    }

    ATH_MSG_DEBUG("Created module "<<pixel->name()<<"  idHash: "<<pixel->identifyHash()<<"  identifier: "<<pixel->identify());

    // for L1 alignment the alignment frame is equal to the global frame
    // and since the PIXEL detector element positions are also stored in
    // the global frame in DB, transform is identity
    const Amg::Transform3D transform = Amg::Transform3D::Identity();

    // ==================================================================
    // loop over Pixel elements and add them to respective alignModules
    // ==================================================================
    for (unsigned int index = 0; index < pixelmaxHash; index++)
    {
      IdentifierHash idHash = index;
      Identifier id = m_idHelper->wafer_id(idHash);

      ATH_MSG_DEBUG(" Pixel DetectorElement idhash: " << index);
      // ATH_MSG_DEBUG(" DetectorElement id: " << id);
      
      // just to be sure check for DBM module
      if(m_idHelper->is_dbm(id)){
        ATH_MSG_DEBUG(" Found DBM element, skipping: " << m_idHelper->is_dbm(id));
        continue;
      }
      
      // get the element via hash
      SiDetectorElement * element2 = m_detManager->getDetectorElement(id);
      if (element2) {
        const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

        // add element to the AlignModule
        pixel->addDetElement(Trk::AlignModule::Pixel,element,transform);
        // and fill the corresponding map
        (*pixelIdHashMap)[idHash] = pixel;
      }
      else
        ATH_MSG_DEBUG("No Pixel detector with id:" << id);
    }
    ATH_MSG_DEBUG("-------------------------------------------------------");

    // add created module to the geometry
    m_alignModuleListPtr->push_back(pixel);

    ATH_MSG_DEBUG("Pixel L1 module successfully added to the list");
  }


  //_______________________________________________________________________
  void PixelGeometryManagerTool::buildL11()
  {
    // ========================================
    // alignment for IBL with respect to "old" pixel detector
    // 1 module for all IBL-pixels + 1 module for all other pixels
    ATH_MSG_INFO("Preparing the Pixel geometry for L11 : IBL + rest of pixel detector");
    
    if(m_detManager->numerology().numLayers() < 4) {
        ATH_MSG_WARNING("No IBL in geometry; Maybe this is not the level you want to use");
    }
    
    Amg::Transform3D transform = Amg::Transform3D::Identity();
    
    // get maximum number of elements from the helper
    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the Pixel "<<maxHash);
    
    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel) = new Trk::AlignModuleList((size_t)(maxHash),0);
    Trk::AlignModuleList * pixelIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel);
         
    // create two AlignModules: IBL and rest of pixel
    Trk::AlignModule * ibl = new Trk::AlignModule(this);
    ibl->setIdHash(getNextIDHash());
    ibl->setName("Pixel/IBL");
         
    Trk::AlignModule * pixel_old = new Trk::AlignModule(this);
    pixel_old->setIdHash(getNextIDHash());
    pixel_old->setName("Pixel/oldPixel");
         
    // we use identifier of 0th module in the 0th and 1st layer of barrel
    ibl->setIdentifier(m_idHelper->wafer_id(0, 0, 0, 0));
    pixel_old->setIdentifier(m_idHelper->wafer_id(0, 1, 0, 0));
         
    ATH_MSG_DEBUG("Building module "<<ibl->name());
    ATH_MSG_DEBUG("Building module "<<pixel_old->name());
    
    
    // ==================================================================
    // loop over Pixel elements and add them to respective alignModules
    // ==================================================================
    for (unsigned int index = 0; index < maxHash; index++)
    {
      IdentifierHash idHash = index;
      Identifier id = m_idHelper->wafer_id(idHash);
      
      // just to be sure check for DBM module
      if(m_idHelper->is_dbm(id)){
        ATH_MSG_DEBUG(" Found DBM element, skipping: " << m_idHelper->is_dbm(id));
        continue;
      }
      
      // check what we're filling
      Trk::AlignModule * mod = 0;
      if( m_idHelper->is_barrel(id) && m_idHelper->is_blayer(id) && m_idHelper->layer_disk(id)==0) {
        ATH_MSG_DEBUG("pixel element "<<id<<" at index "<<index<<" is in IBL-layer");
        mod = ibl;
      }
      else {
        ATH_MSG_DEBUG("pixel element "<<id<<" at index "<<index<<" is not in IBL-layer");
        mod = pixel_old;
      }
      
      // get the element via hash
      SiDetectorElement * element2 = m_detManager->getDetectorElement(id);
      if (element2) {
        const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;
        
        // add element to the AlignModule
        mod->addDetElement(Trk::AlignModule::Pixel,element,transform);
        // and fill the corresponding map
        (*pixelIdHashMap)[element->identifyHash()] = mod;
      }
      else
        ATH_MSG_DEBUG("No Pixel detector with id:" << id);
    }
    ATH_MSG_DEBUG("-------------------------------------------------------");
    
    // alignment fram for the barrel is the global frame
    // so we don't have to set any additional transform
    if(!moduleSelected(ibl)) {
      ATH_MSG_DEBUG("Module "<<ibl->name()<<" NOT selected");
      delete ibl;
    }
    else
      m_alignModuleListPtr->push_back(ibl);
        
    if(!moduleSelected(pixel_old)) {
      ATH_MSG_DEBUG("Module "<<pixel_old->name()<<" NOT selected");
      delete pixel_old;
    }
    else
      m_alignModuleListPtr->push_back(pixel_old);
         
  }
                                                                                             

  //_______________________________________________________________________
  void PixelGeometryManagerTool::buildL12Barrel()
  {
    // ========================================
    // BARREL
    ATH_MSG_INFO("Preparing the Pixel Barrel geometry for L12 : whole barrel");

    if(!m_alignBarrel) {
      ATH_MSG_DEBUG("Not aligning barrel");
      return;
    }

    // ========================================
    // get all modules for Level 12 alignment of the barrel

    Amg::Transform3D transform = Amg::Transform3D::Identity();

    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the Pixel "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel) = new Trk::AlignModuleList((size_t)(maxHash),0);
    Trk::AlignModuleList * pixelIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel);

    // create two AlignModules: top half and bottom half
    Trk::AlignModule * mod = new Trk::AlignModule(this);
    mod->setIdHash(getNextIDHash());
    mod->setName("Pixel/Barrel");

    // we use identifier of 0th module in the barrel
    mod->setIdentifier(m_idHelper->wafer_id(0, 0, 0, 0));

    if(!moduleSelected(mod)) {
      ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
      delete mod;
      return;
    }

    ATH_MSG_DEBUG("Building module "<<mod->name());

    for (int iLayer = 0; iLayer < m_detManager->numerology().numLayers(); iLayer++) {
      if (!m_detManager->numerology().useLayer(iLayer))
        ATH_MSG_INFO("  Layer "<<iLayer<<" not present");

      // make sure that we do not correct if we are not looking at IBL
      if(iLayer!=0) etaCorrection = 0;

      for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForLayer(iLayer); iPhi++) {
        ATH_MSG_DEBUG("iPhi "<<iPhi);

        for (int iEta = m_detManager->numerology().beginEtaModuleForLayer(iLayer); iEta < m_detManager->numerology().endEtaModuleForLayer(iLayer)-etaCorrection; iEta++) {
          ATH_MSG_DEBUG("iEta "<<iEta);
          const SiDetectorElement * element2 = m_detManager->getDetectorElement(0, iLayer, iPhi, iEta);
          const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;
          
          if (element) {
            
            // just to be sure check for DBM module
            Identifier id = m_idHelper->wafer_id(0, iLayer, iPhi, iEta);
            if(m_idHelper->is_dbm(id)){
              ATH_MSG_DEBUG(" Found DBM element, skipping: " << m_idHelper->is_dbm(id));
              continue;
            }
            // add element to the AlignModule
            mod->addDetElement(Trk::AlignModule::Pixel,element,transform);
            // and fill the corresponding map
            (*pixelIdHashMap)[element->identifyHash()] = mod;
          }
        }
      }
    }

    // alignment fram for the barrel is the global frame
    // so we don't have to set any additional transform
    m_alignModuleListPtr->push_back(mod);
  }

  //_______________________________________________________________________
  void PixelGeometryManagerTool::buildL15Barrel()
  {
    // ========================================
    // BARREL
    ATH_MSG_INFO("Preparing the Pixel Barrel geometry for L15 : 2 barrel halfs");
    
    if(!m_alignBarrel) {
      ATH_MSG_DEBUG("Not aligning barrel");
      return;
    }
       
    // ========================================
    // get all modules for Level 15 alignment of the barrel
    // 2 Pixel half-shells
    Amg::Transform3D transform = Amg::Transform3D::Identity();

    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the Pixel "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel) = new Trk::AlignModuleList((size_t)(maxHash),0);
    Trk::AlignModuleList * pixelIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel);

    // create two AlignModules: top half and bottom half
    Trk::AlignModule * top = new Trk::AlignModule(this);
    top->setIdHash(getNextIDHash());
    top->setName("Pixel/Barrel/Top-half");

    Trk::AlignModule * bottom = new Trk::AlignModule(this);
    bottom->setIdHash(getNextIDHash());
    bottom->setName("Pixel/Barrel/Bottom-half");

    // we use identifier of 0th eta module in the layer 0 in the 10th phi
    // stave for the top module and 20th stave in the bottom module
    top->setIdentifier(m_idHelper->wafer_id(0, 0, 10, 0));
    bottom->setIdentifier(m_idHelper->wafer_id(0, 0, 20, 0));

    ATH_MSG_DEBUG("Building module "<<top->name());
    ATH_MSG_DEBUG("Building module "<<bottom->name());

    for (int iLayer = 0; iLayer < m_detManager->numerology().numLayers(); iLayer++) {
      if (!m_detManager->numerology().useLayer(iLayer))
        ATH_MSG_INFO("  Layer "<<iLayer<<" not present");

      // make sure that we do not correct if we are not looking at IBL
      if(iLayer!=0) etaCorrection = 0;

      for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForLayer(iLayer); iPhi++) {
        ATH_MSG_DEBUG("iPhi "<<iPhi);

        // check which half we're filling
        Trk::AlignModule * mod = 0;
        if(isTopHalf(iPhi,iLayer)) {
          ATH_MSG_DEBUG("iPhi "<<iPhi<<" is in top half of layer "<<iLayer);
          mod = top;
        }
        else {
          ATH_MSG_DEBUG("iPhi "<<iPhi<<" is in bottom half of layer "<<iLayer);
          mod = bottom;
        }

        for (int iEta = m_detManager->numerology().beginEtaModuleForLayer(iLayer); iEta < m_detManager->numerology().endEtaModuleForLayer(iLayer)-etaCorrection; iEta++) {
          ATH_MSG_DEBUG("iEta "<<iEta);
          const SiDetectorElement * element2 = m_detManager->getDetectorElement(0, iLayer, iPhi, iEta);
          const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

          if (element) {
            // add element to the AlignModule
            mod->addDetElement(Trk::AlignModule::Pixel,element,transform);
            // and fill the corresponding map
            (*pixelIdHashMap)[element->identifyHash()] = mod;
          }
        }
      }
    }

    if(!moduleSelected(top)) {
      ATH_MSG_DEBUG("Module "<<top->name()<<" NOT selected");
      delete top;
    }
    else
      m_alignModuleListPtr->push_back(top);

    if(!moduleSelected(bottom)) {
      ATH_MSG_DEBUG("Module "<<bottom->name()<<" NOT selected");
      delete bottom;
    }
    else
      m_alignModuleListPtr->push_back(bottom);
  }

  //_______________________________________________________________________
  void PixelGeometryManagerTool::buildL12Endcaps()
  {
    // ========================================
    // END CAPS
    ATH_MSG_INFO("Preparing the Pixel Endcap geometry for L12 : 2 endcaps");

    if(!m_alignEndcaps) {
      ATH_MSG_DEBUG("Not aligning endcaps");
      return;
    }

    Amg::Transform3D transform = Amg::Transform3D::Identity();

    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the Pixel "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel) = new Trk::AlignModuleList((size_t)(maxHash),0);
    Trk::AlignModuleList * pixelIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel);

    for (int iEndcapIndex = 0; iEndcapIndex < m_detManager->numerology().numEndcaps(); iEndcapIndex++) {
      int iSide = m_detManager->numerology().endcapId(iEndcapIndex);

      Trk::AlignModule * mod = new Trk::AlignModule(this);
      mod->setIdHash(getNextIDHash());
      mod->setIdentifier(m_idHelper->wafer_id(iSide,0,0,0));

      std::stringstream name;
      if(iEndcapIndex == 0)
        name <<"Pixel/EndcapA";
      else
        name <<"Pixel/EndcapC";
      mod->setName(name.str());

      if(!moduleSelected(mod)) {
        ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
        delete mod;
        continue;
      }

      ATH_MSG_DEBUG("Building module "<<mod->name());

      for (int iWheel = 0; iWheel < m_detManager->numerology().numDisks(); iWheel++) {
        if (!m_detManager->numerology().useDisk(iWheel))
          ATH_MSG_INFO("  Disk "<<iWheel<<" not present");

        // there is only one eta module for all Pixel endcap disks but anyway ...
        for (int iEta = 0; iEta < m_detManager->numerology().numRingsForDisk(iWheel); iEta++) {
          for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForDiskRing(iWheel,iEta); iPhi++) {
            ATH_MSG_DEBUG("iPhi "<<iPhi);
            const SiDetectorElement * element2 = m_detManager->getDetectorElement(iSide, iWheel, iPhi, iEta);
            const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

            if (element) {
              
              // just to be sure check for DBM module
              Identifier id = m_idHelper->wafer_id(iSide, iWheel, iPhi, iEta);
              if(m_idHelper->is_dbm(id)){
                ATH_MSG_DEBUG(" Found DBM element, skipping: " << m_idHelper->is_dbm(id));
                continue;
              }
              // add to the AlignModule
              mod->addDetElement(Trk::AlignModule::Pixel,element,transform);
              // and fill the corresponding map
              (*pixelIdHashMap)[element->identifyHash()] = mod;
            }
          }
        }
      }

      // for endcap we move the CoG
      Amg::Translation3D translation(mod->centerOfGravity());
      ATH_MSG_DEBUG("Endcap:" << iSide << " , CoG (" << translation.x() << " , " << translation.y() << " , " << translation.z() << " ) " );
      Amg::Transform3D localtoglobal = translation * Amg::RotationMatrix3D::Identity();
      mod->setGlobalFrameToAlignFrameTransform(localtoglobal.inverse());

      m_alignModuleListPtr->push_back(mod);
    }
  }


 void PixelGeometryManagerTool::buildL16Barrel()
  {
    // ========================================
    // BARREL
    ATH_MSG_INFO("Preparing the Pixel Barrel geometry for L26 : IBL stave Pixel whole barrel");

    if(!m_alignBarrel) {
      ATH_MSG_DEBUG("Not aligning barrel");
      return;
    }

    // ========================================
    // get all modules for Level 16 alignment of the barrel
    // 14 Pixel ladders (staves) in  the IBL
    // 1 piece for the remainder of the Pixel barrel

    Amg::Transform3D transform = Amg::Transform3D::Identity();

    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the Pixel "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel) = new Trk::AlignModuleList((size_t)(maxHash),0);
    Trk::AlignModuleList * pixelIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel);

         
    // create two AlignModules: IBL and rest of pixel
    Trk::AlignModule * pixel_old = new Trk::AlignModule(this);
    pixel_old->setIdHash(getNextIDHash());
    pixel_old->setName("Pixel/oldPixel");
         
    // we use identifier of 0th module in the 0th and 1st layer of barrel
    pixel_old->setIdentifier(m_idHelper->wafer_id(0, 1, 0, 0));
         
    ATH_MSG_DEBUG("Building module "<<pixel_old->name());
    
    

    for (int iLayer = 0; iLayer < m_detManager->numerology().numLayers(); iLayer++) {
      if (!m_detManager->numerology().useLayer(iLayer))
        ATH_MSG_INFO("  Layer "<<iLayer<<" not present");
        
      // make sure that we do not correct if we are not looking at IBL
      if(iLayer!=0){
        etaCorrection = 0;
        
        if(!moduleSelected(pixel_old)) {
          ATH_MSG_DEBUG("Module "<<pixel_old->name()<<" NOT selected");
          continue;
        }

        ATH_MSG_DEBUG("Adding to module "<<pixel_old->name());
      
        for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForLayer(iLayer); iPhi++) {
          ATH_MSG_DEBUG("iPhi "<<iPhi);
          for (int iEta = m_detManager->numerology().beginEtaModuleForLayer(iLayer); iEta < m_detManager->numerology().endEtaModuleForLayer(iLayer)-etaCorrection; iEta++) {
            ATH_MSG_DEBUG("iEta "<<iEta);
            const SiDetectorElement * element2 = m_detManager->getDetectorElement(0, iLayer, iPhi, iEta);
            const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

            if (element) {
            
              // just to be sure check for DBM module
              Identifier id = m_idHelper->wafer_id(0, iLayer, iPhi, iEta);
              if(m_idHelper->is_dbm(id)){
                ATH_MSG_DEBUG(" Found DBM element, skipping: " << m_idHelper->is_dbm(id));
                continue;
              }
              // add element to the AlignModule
              pixel_old->addDetElement(Trk::AlignModule::Pixel,element,transform);
              // and fill the corresponding map
              (*pixelIdHashMap)[element->identifyHash()] = pixel_old;
            }
          }
        }
      } else {
        
        for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForLayer(iLayer); iPhi++) {
          ATH_MSG_DEBUG("iPhi "<<iPhi);

          // create AlignModule
          Trk::AlignModule * mod = new Trk::AlignModule(this);
          mod->setIdHash(getNextIDHash());

          // we use identifier of 0th eta module in the stave
          mod->setIdentifier(m_idHelper->wafer_id(0, iLayer, iPhi, 0));

          std::stringstream name;
          name <<"Pixel/Barrel/Layer_"<<iLayer<<"/PhiStave_"<<iPhi;
          mod->setName(name.str());

          if(!moduleSelected(mod)) {
            ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
            delete mod;
            continue;
          }

          ATH_MSG_DEBUG("Building module "<<mod->name());

          for (int iEta = m_detManager->numerology().beginEtaModuleForLayer(iLayer); iEta < m_detManager->numerology().endEtaModuleForLayer(iLayer)-etaCorrection; iEta++) {
            ATH_MSG_DEBUG("iEta "<<iEta);
            const SiDetectorElement * element2 = m_detManager->getDetectorElement(0, iLayer, iPhi, iEta);
            const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

            if (element) {
            
              // just to be sure check for DBM module
              Identifier id = m_idHelper->wafer_id(0, iLayer, iPhi, iEta);
              if(m_idHelper->is_dbm(id)){
                ATH_MSG_DEBUG(" Found DBM element, skipping: " << m_idHelper->is_dbm(id));
                continue;
              }
              // add element to the AlignModule
              mod->addDetElement(Trk::AlignModule::Pixel,element,transform);
              // and fill the corresponding map
              (*pixelIdHashMap)[element->identifyHash()] = mod;
            }

          }
          // we use create the align frame from the sum of all modules ;  
          Amg::Transform3D alignModuleToGlobal =  mod->calculateAlignModuleToGlobal();
          mod->setGlobalFrameToAlignFrameTransform( alignModuleToGlobal.inverse() );
          mod->resetAlignModuleToDetElementTransforms();
          

          // add AlignModule to the geometry
          m_alignModuleListPtr->push_back(mod);
        }
      } //end loop over staves
    }
  
    if(!moduleSelected(pixel_old)) {
      ATH_MSG_DEBUG("Module "<<pixel_old->name()<<" NOT selected");
      delete pixel_old;
    }
    else
      m_alignModuleListPtr->push_back(pixel_old);
  
  
  }




  //_______________________________________________________________________
  void PixelGeometryManagerTool::buildL2Barrel()
  {
    // ========================================
    // BARREL
    ATH_MSG_INFO("Preparing the Pixel Barrel geometry for L2 : "<<m_detManager->numerology().numLayers()<<" layers");

    if(!m_alignBarrel) {
      ATH_MSG_DEBUG("Not aligning barrel");
      return;
    }

    // ========================================
    // get all modules for Level 2 alignment of the barrel
    // 3 Pixel layers

    Amg::Transform3D transform = Amg::Transform3D::Identity();

    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the Pixel "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel) = new Trk::AlignModuleList((size_t)(maxHash),0);
    Trk::AlignModuleList * pixelIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel);

    for (int iLayer = 0; iLayer < m_detManager->numerology().numLayers(); iLayer++) {
      if (!m_detManager->numerology().useLayer(iLayer))
        ATH_MSG_INFO("  Layer "<<iLayer<<" not present");

      // make sure that we do not correct if we are not looking at IBL
      if(iLayer!=0) etaCorrection = 0;

      // create the AlignModule
      Trk::AlignModule * mod = new Trk::AlignModule(this);
      mod->setIdHash(getNextIDHash());
      mod->setIdentifier(m_idHelper->wafer_id(0, iLayer, 0, 0));

      std::stringstream name;
      name <<"Pixel/Barrel/Layer_" << iLayer;
      mod->setName(name.str());

      if(!moduleSelected(mod)) {
        ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
        delete mod;
        continue;
      }

      ATH_MSG_DEBUG("Building module "<<mod->name());
      
      for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForLayer(iLayer); iPhi++) {
        ATH_MSG_DEBUG("iPhi "<<iPhi);
        for (int iEta = m_detManager->numerology().beginEtaModuleForLayer(iLayer); iEta < m_detManager->numerology().endEtaModuleForLayer(iLayer)-etaCorrection; iEta++) {
          ATH_MSG_DEBUG("iEta "<<iEta);
          const SiDetectorElement * element2 = m_detManager->getDetectorElement(0, iLayer, iPhi, iEta);
          const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

          if (element) {
            
            // just to be sure check for DBM module
            Identifier id = m_idHelper->wafer_id(0, iLayer, iPhi, iEta);
            if(m_idHelper->is_dbm(id)){
              ATH_MSG_DEBUG(" Found DBM element, skipping: " << m_idHelper->is_dbm(id));
              continue;
            }
            // add element to the AlignModule
            mod->addDetElement(Trk::AlignModule::Pixel,element,transform);
            // and fill the corresponding map
            (*pixelIdHashMap)[element->identifyHash()] = mod;
          }
        }
      }

      m_alignModuleListPtr->push_back(mod);
    }
  }

  //_______________________________________________________________________
  void PixelGeometryManagerTool::buildL22Barrel()
  {
    // ========================================
    // BARREL
    ATH_MSG_INFO("Preparing the Pixel Barrel geometry for L22 "<<m_detManager->numerology().numLayers()*2<<" half-layers");

    if(!m_alignBarrel) {
      ATH_MSG_DEBUG("Not aligning barrel");
      return;
    }
    
    // ========================================
    // get all modules for Level 22 alignment of the barrel
    // 6 Pixel half-layers

    Amg::Transform3D transform = Amg::Transform3D::Identity();

    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the Pixel "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel) = new Trk::AlignModuleList((size_t)(maxHash),0);
    Trk::AlignModuleList * pixelIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel);

    for (int iLayer = 0; iLayer < m_detManager->numerology().numLayers(); iLayer++) {
      if (!m_detManager->numerology().useLayer(iLayer))
        ATH_MSG_INFO("  Layer "<<iLayer<<" not present");

      // create two AlignModules: top half and bottom half
      Trk::AlignModule * top = new Trk::AlignModule(this);
      Trk::AlignModule * bottom = new Trk::AlignModule(this);
      top->setIdHash(getNextIDHash());
      bottom->setIdHash(getNextIDHash());

      // we use identifier of 0th eta module in following phi stave based on the layer
      //
      //  layer   phi-top  phi-bottom
      //    0        10        20
      //    1        10        30
      //    2        10        30
      //
      unsigned int phi_top = 10;
      top->setIdentifier(m_idHelper->wafer_id(0, iLayer, phi_top, 0));
      unsigned int phi_bottom = iLayer ? 30 : 20;
      bottom->setIdentifier(m_idHelper->wafer_id(0, iLayer, phi_bottom, 0));

      std::stringstream nametop;
      nametop <<"Pixel/Barrel/Layer_"<<iLayer<<"_top";
      top->setName(nametop.str());

      std::stringstream namebottom;
      namebottom <<"Pixel/Barrel/Layer_"<<iLayer<<"_bottom";
      bottom->setName(namebottom.str());

      ATH_MSG_DEBUG("Building module "<<top->name());
      ATH_MSG_DEBUG("Building module "<<bottom->name());

      if(iLayer!=0) etaCorrection = 0;      
      
      for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForLayer(iLayer); iPhi++) {
        ATH_MSG_DEBUG("iPhi "<<iPhi);

        // check which half we're filling
        Trk::AlignModule * mod = 0;
        if(isTopHalf(iPhi,iLayer)) {
          ATH_MSG_DEBUG("iPhi "<<iPhi<<" is in top half of layer "<<iLayer);
          mod = top;
        }
        else {
          ATH_MSG_DEBUG("iPhi "<<iPhi<<" is in bottom half of layer "<<iLayer);
          mod = bottom;
        }

        for (int iEta = m_detManager->numerology().beginEtaModuleForLayer(iLayer); iEta < m_detManager->numerology().endEtaModuleForLayer(iLayer)-etaCorrection; iEta++) {
          ATH_MSG_DEBUG("iEta "<<iEta);
          const SiDetectorElement * element2 = m_detManager->getDetectorElement(0, iLayer, iPhi, iEta);
          const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

          if (element) {
            // add element to the AlignModule
            mod->addDetElement(Trk::AlignModule::Pixel,element,transform);
            // and fill the corresponding map
            (*pixelIdHashMap)[element->identifyHash()] = mod;
          }
        }
      }

      if(!moduleSelected(top)) {
        ATH_MSG_DEBUG("Module "<<top->name()<<" NOT selected");
        delete top;
      }
      else
        m_alignModuleListPtr->push_back(top);

      if(!moduleSelected(bottom)) {
        ATH_MSG_DEBUG("Module "<<bottom->name()<<" NOT selected");
        delete bottom;
      }
      else
        m_alignModuleListPtr->push_back(bottom);
    }
  }
  
 
   //_______________________________________________________________________
  void PixelGeometryManagerTool::buildL26Barrel()
  {
    // ========================================
    // BARREL
    ATH_MSG_INFO("Preparing the Pixel Barrel geometry for L26 : IBL stave Pixel L2");

    if(!m_alignBarrel) {
      ATH_MSG_DEBUG("Not aligning barrel");
      return;
    }


    // ========================================
    // get all modules for Level 16 alignment of the barrel
    // 14 Pixel ladders (staves) in  the IBL
    // 3 layers for the remainder of the Pixel barrel
    
    Amg::Transform3D transform = Amg::Transform3D::Identity();

    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the Pixel "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel) = new Trk::AlignModuleList((size_t)(maxHash),0);
    Trk::AlignModuleList * pixelIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel);

    for (int iLayer = 0; iLayer < m_detManager->numerology().numLayers(); iLayer++) {
      if (!m_detManager->numerology().useLayer(iLayer))
        ATH_MSG_INFO("  Layer "<<iLayer<<" not present");
        
      // make sure that we do not correct if we are not looking at IBL
      if(iLayer!=0){
        etaCorrection = 0;
              // create the AlignModule
        Trk::AlignModule * mod = new Trk::AlignModule(this);
        mod->setIdHash(getNextIDHash());
        mod->setIdentifier(m_idHelper->wafer_id(0, iLayer, 0, 0));

        std::stringstream name;
        name <<"Pixel/Barrel/Layer_" << iLayer;
        mod->setName(name.str());

        if(!moduleSelected(mod)) {
          ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
          delete mod;
          continue;
        }

        ATH_MSG_DEBUG("Building module "<<mod->name());
      
        for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForLayer(iLayer); iPhi++) {
          ATH_MSG_DEBUG("iPhi "<<iPhi);
          for (int iEta = m_detManager->numerology().beginEtaModuleForLayer(iLayer); iEta < m_detManager->numerology().endEtaModuleForLayer(iLayer)-etaCorrection; iEta++) {
            ATH_MSG_DEBUG("iEta "<<iEta);
            const SiDetectorElement * element2 = m_detManager->getDetectorElement(0, iLayer, iPhi, iEta);
            const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

            if (element) {
            
              // just to be sure check for DBM module
              Identifier id = m_idHelper->wafer_id(0, iLayer, iPhi, iEta);
              if(m_idHelper->is_dbm(id)){
                ATH_MSG_DEBUG(" Found DBM element, skipping: " << m_idHelper->is_dbm(id));
                continue;
              }
              // add element to the AlignModule
              mod->addDetElement(Trk::AlignModule::Pixel,element,transform);
              // and fill the corresponding map
              (*pixelIdHashMap)[element->identifyHash()] = mod;
            }
          }
        }

        m_alignModuleListPtr->push_back(mod);
      } else {
        
        for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForLayer(iLayer); iPhi++) {
          ATH_MSG_DEBUG("iPhi "<<iPhi);

          // create AlignModule
          Trk::AlignModule * mod = new Trk::AlignModule(this);
          mod->setIdHash(getNextIDHash());

          // we use identifier of 0th eta module in the stave
          mod->setIdentifier(m_idHelper->wafer_id(0, iLayer, iPhi, 0));

          std::stringstream name;
          name <<"Pixel/Barrel/Layer_"<<iLayer<<"/PhiStave_"<<iPhi;
          mod->setName(name.str());

          if(!moduleSelected(mod)) {
            ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
            delete mod;
            continue;
          }

          ATH_MSG_DEBUG("Building module "<<mod->name());

          for (int iEta = m_detManager->numerology().beginEtaModuleForLayer(iLayer); iEta < m_detManager->numerology().endEtaModuleForLayer(iLayer)-etaCorrection; iEta++) {
            ATH_MSG_DEBUG("iEta "<<iEta);
            const SiDetectorElement * element2 = m_detManager->getDetectorElement(0, iLayer, iPhi, iEta);
            const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

            if (element) {
            
              // just to be sure check for DBM module
              Identifier id = m_idHelper->wafer_id(0, iLayer, iPhi, iEta);
              if(m_idHelper->is_dbm(id)){
                ATH_MSG_DEBUG(" Found DBM element, skipping: " << m_idHelper->is_dbm(id));
                continue;
              }
              // add element to the AlignModule
              mod->addDetElement(Trk::AlignModule::Pixel,element,transform);
              // and fill the corresponding map
              (*pixelIdHashMap)[element->identifyHash()] = mod;
            }
          }

          // we use create the align frame from the sum of all modules ;  
          Amg::Transform3D alignModuleToGlobal =  mod->calculateAlignModuleToGlobal();
          mod->setGlobalFrameToAlignFrameTransform( alignModuleToGlobal.inverse() );
          mod->resetAlignModuleToDetElementTransforms();


          // add AlignModule to the geometry
          m_alignModuleListPtr->push_back(mod);
        }
      } //end loop over staves
    }
  }

  
  
  
  //_______________________________________________________________________
  void PixelGeometryManagerTool::buildL27Barrel()
  {
    // ========================================
    // BARREL
    ATH_MSG_INFO("Preparing the Pixel Barrel geometry for L27 : 112(132 incl. IBL) staves (ladders)");

    if(!m_alignBarrel) {
      ATH_MSG_DEBUG("Not aligning barrel");
      return;
    }

    // ========================================
    // get all modules for Level 27 alignment of the barrel
    // 112 Pixel ladders (staves)

    Amg::Transform3D transform = Amg::Transform3D::Identity();

    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the Pixel "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel) = new Trk::AlignModuleList((size_t)(maxHash),0);
    Trk::AlignModuleList * pixelIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel);

    for (int iLayer = 0; iLayer < m_detManager->numerology().numLayers(); iLayer++) {
      if (!m_detManager->numerology().useLayer(iLayer))
        ATH_MSG_INFO("  Layer "<<iLayer<<" not present");
        
      // make sure that we do not correct if we are not looking at IBL
      if(iLayer!=0) etaCorrection = 0;
        
      for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForLayer(iLayer); iPhi++) {
        ATH_MSG_DEBUG("iPhi "<<iPhi);

        // create AlignModule
        Trk::AlignModule * mod = new Trk::AlignModule(this);
        mod->setIdHash(getNextIDHash());

        // we use identifier of 0th eta module in the stave
        mod->setIdentifier(m_idHelper->wafer_id(0, iLayer, iPhi, 0));

        std::stringstream name;
        name <<"Pixel/Barrel/Layer_"<<iLayer<<"/PhiStave_"<<iPhi;
        mod->setName(name.str());

        if(!moduleSelected(mod)) {
          ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
          delete mod;
          continue;
        }

        ATH_MSG_DEBUG("Building module "<<mod->name());

        for (int iEta = m_detManager->numerology().beginEtaModuleForLayer(iLayer); iEta < m_detManager->numerology().endEtaModuleForLayer(iLayer)-etaCorrection; iEta++) {
          ATH_MSG_DEBUG("iEta "<<iEta);
          const SiDetectorElement * element2 = m_detManager->getDetectorElement(0, iLayer, iPhi, iEta);
          const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

          if (element) {
            
            // just to be sure check for DBM module
            Identifier id = m_idHelper->wafer_id(0, iLayer, iPhi, iEta);
            if(m_idHelper->is_dbm(id)){
              ATH_MSG_DEBUG(" Found DBM element, skipping: " << m_idHelper->is_dbm(id));
              continue;
            }
            // add element to the AlignModule
            mod->addDetElement(Trk::AlignModule::Pixel,element,transform);
            // and fill the corresponding map
            (*pixelIdHashMap)[element->identifyHash()] = mod;
          }

          // we use the local frame for the 0th eta wafer as the stave alignment frame
          if (iEta==0) {
            if (!element) { 
              throw std::logic_error("No  detector element at the 0th eta wafer.");
            }
            mod->setGlobalFrameToAlignFrameTransform(element->transform().inverse());
          }
        }

        // add AlignModule to the geometry
        m_alignModuleListPtr->push_back(mod);
      }
    }
  }

  //_______________________________________________________________________
  void PixelGeometryManagerTool::buildL2Endcaps()
  {
    // ========================================
    // END CAPS
    ATH_MSG_INFO("Preparing the Pixel Endcap geometry for L2 : 2 x 3 disks");

    if(!m_alignEndcaps) {
      ATH_MSG_DEBUG("Not aligning endcaps");
      return;
    }

    // ========================================
    // get all modules for Level 2 alignment for the endcaps
    // 2 x 3 Pixel Endcap disks
    Amg::Transform3D transform = Amg::Transform3D::Identity();

    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the Pixel "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel) = new Trk::AlignModuleList((size_t)(maxHash),0);
    Trk::AlignModuleList * pixelIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel);

    for (int iEndcapIndex = 0; iEndcapIndex < m_detManager->numerology().numEndcaps(); iEndcapIndex++) {
      int iSide = m_detManager->numerology().endcapId(iEndcapIndex);
      for (int iWheel = 0; iWheel < m_detManager->numerology().numDisks(); iWheel++) {
        if (!m_detManager->numerology().useDisk(iWheel))
          ATH_MSG_INFO("  Disk "<<iWheel<<" not present");

        Trk::AlignModule * mod = new Trk::AlignModule(this);
        mod->setIdHash(getNextIDHash());
        mod->setIdentifier(m_idHelper->wafer_id(iSide,iWheel,0,0));

        std::stringstream name;
        if(iEndcapIndex == 0)
          name <<"Pixel/EndcapA/Disk_" << iWheel;
        else
          name <<"Pixel/EndcapC/Disk_" << iWheel;
        mod->setName(name.str());

        if(!moduleSelected(mod)) {
          ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
          delete mod;
          continue;
        }

        ATH_MSG_DEBUG("Building module "<<mod->name());

        // there is only one eta module for all Pixel endcap disks but anyway ...
        for (int iEta = 0; iEta < m_detManager->numerology().numRingsForDisk(iWheel); iEta++) {
          for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForDiskRing(iWheel,iEta); iPhi++) {
            ATH_MSG_DEBUG("iPhi "<<iPhi);
            const SiDetectorElement * element2 = m_detManager->getDetectorElement(iSide, iWheel, iPhi, iEta);
            const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

            if (element) {
              
              // just to be sure check for DBM module
              Identifier id = m_idHelper->wafer_id(iSide, iWheel, iPhi, iEta);
              if(m_idHelper->is_dbm(id)){
                ATH_MSG_DEBUG(" Found DBM element, skipping: " << m_idHelper->is_dbm(id));
                continue;
              }
              // add to the AlignModule
              mod->addDetElement(Trk::AlignModule::Pixel,element,transform);
              // and fill the corresponding map
              (*pixelIdHashMap)[element->identifyHash()] = mod;
            }
          }
        }

        // for endcap we move the CoG
        Amg::Translation3D translation(mod->centerOfGravity());
        ATH_MSG_DEBUG("Endcap:" << iSide << " , CoG (" << translation.x() << " , " << translation.y() << " , " << translation.z() << " ) " );
        Amg::Transform3D localtoglobal = translation * Amg::RotationMatrix3D::Identity();
        mod->setGlobalFrameToAlignFrameTransform(localtoglobal.inverse());

        m_alignModuleListPtr->push_back(mod);
      }
    }
  }

  //_______________________________________________________________________
  void PixelGeometryManagerTool::buildL3Barrel()
  {
    // ========================================
    // BARREL
    ATH_MSG_INFO("Preparing the Pixel Barrel geometry for L3 : module (wafer) level");

    if(!m_alignBarrel) {
      ATH_MSG_DEBUG("Not aligning barrel");
      return;
    }

    // ========================================
    // Pixel Barrel:
    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the Pixel "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel) = new Trk::AlignModuleList((size_t)(maxHash),0);
    Trk::AlignModuleList * pixelIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel);

    for (int iLayer = 0; iLayer < m_detManager->numerology().numLayers(); iLayer++) {
      ATH_MSG_DEBUG("iLayer "<<iLayer);
      if (!m_detManager->numerology().useLayer(iLayer))
        ATH_MSG_INFO("  Layer "<<iLayer<<" not present");
        
      // make sure that we do not correct if we are not looking at IBL
      if(iLayer!=0) etaCorrection = 0;
      
      for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForLayer(iLayer); iPhi++) {
        ATH_MSG_DEBUG("iPhi "<<iPhi);
        for (int iEta = m_detManager->numerology().beginEtaModuleForLayer(iLayer); iEta < m_detManager->numerology().endEtaModuleForLayer(iLayer)-etaCorrection; iEta++) {
          ATH_MSG_DEBUG("iEta "<<iEta);

          // just to be sure check for DBM module
          Identifier id = m_idHelper->wafer_id(0, iLayer, iPhi, iEta);
          if(m_idHelper->is_dbm(id)){
            ATH_MSG_DEBUG(" Found DBM element, skipping: " << m_idHelper->is_dbm(id));
            continue;
          }
          
          // create the AlignModule
          Trk::AlignModule * mod = new Trk::AlignModule(this);
          mod->setIdHash(getNextIDHash());
          mod->setIdentifier(m_idHelper->wafer_id(0, iLayer, iPhi, iEta));

          std::stringstream name;
          name <<"Pixel/Barrel/Layer_"<<iLayer<<"/Phi_"<<iPhi<<"/Eta_"<<iEta;
          mod->setName(name.str());

          if(!moduleSelected(mod)) {
            ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
            delete mod;
            continue;
          }

          ATH_MSG_DEBUG("Building module "<<mod->name());

          const SiDetectorElement * element2 = m_detManager->getDetectorElement(0, iLayer, iPhi, iEta);
          const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

          if (element) {
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            // should this be identity? if yes, it means that the
            // db frame constants are in the global frame. Or not?
            Amg::Transform3D transform = Amg::Transform3D::Identity();
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

            // add to the AlignModule
            mod->addDetElement(Trk::AlignModule::Pixel,element,transform);
            // and fill the corresponding map
            (*pixelIdHashMap)[element->identifyHash()] = mod;

            // set transformation
            mod->setGlobalFrameToAlignFrameTransform(element->transform().inverse());

            // add module to the list
            m_alignModuleListPtr->push_back(mod);
          }
          else {
            ATH_MSG_WARNING("No element in module "<<mod->name()<<". Ignoring this module.");
            delete mod;
          }
        }
      }
    }
  }

  //_______________________________________________________________________
  void PixelGeometryManagerTool::buildL3Endcaps()
  {
    // ========================================
    // END CAPS
    ATH_MSG_INFO("Preparing the Pixel geometry for L3 : module (wafer) level");

    if(!m_alignEndcaps) {
      ATH_MSG_DEBUG("Not aligning endcaps");
      return;
    }

    // ========================================
    // Pixel Endcaps: 2x144 modules

    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the Pixel "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel) = new Trk::AlignModuleList((size_t)(maxHash),0);
    Trk::AlignModuleList * pixelIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel);

    for (int iEndcapIndex = 0; iEndcapIndex < m_detManager->numerology().numEndcaps(); iEndcapIndex++) {
      int iSide = m_detManager->numerology().endcapId(iEndcapIndex);
      ATH_MSG_DEBUG("iSide "<<iSide);
      for (int iWheel = 0; iWheel < m_detManager->numerology().numDisks(); iWheel++) {
        ATH_MSG_DEBUG("iWheel "<<iWheel);
        if (!m_detManager->numerology().useDisk(iWheel))
          ATH_MSG_INFO("  Disk "<<iWheel<<" not present");

        // there is only one eta module for all Pixel endcap disks but anyway ...
        for (int iEta = 0; iEta < m_detManager->numerology().numRingsForDisk(iWheel); iEta++) {
          for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForDiskRing(iWheel,iEta); iPhi++) {
            ATH_MSG_DEBUG("iPhi "<<iPhi);
            
            // just to be sure check for DBM module
            Identifier id = m_idHelper->wafer_id(iSide,iWheel,iPhi,iEta);
            if(m_idHelper->is_dbm(id)){
              ATH_MSG_DEBUG(" Found DBM element, skipping: " << m_idHelper->is_dbm(id));
              continue;
            }
            
            Trk::AlignModule * mod = new Trk::AlignModule(this);
            mod->setIdHash(getNextIDHash());
            mod->setIdentifier(m_idHelper->wafer_id(iSide,iWheel,iPhi,iEta));

            std::stringstream name;
            if(iEndcapIndex == 0)
              name <<"Pixel/EndcapA";
            else
              name <<"Pixel/EndcapC";
            name<<"/Disk_"<<iWheel<<"/Phi_"<<iPhi;
            mod->setName(name.str());

            if(!moduleSelected(mod)) {
              ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
              delete mod;
              continue;
            }

            ATH_MSG_DEBUG("Building module "<<mod->name());

            const SiDetectorElement * element2 = m_detManager->getDetectorElement(iSide, iWheel, iPhi, iEta);
            const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

            if (element) {
              // add element to respective AlignModule

              // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
              // should this be identity? if yes, it means that the
              // db frame constants are in the global frame. Or not?
              Amg::Transform3D transform = Amg::Transform3D::Identity();
              // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

              // add to the barrel
              mod->addDetElement(Trk::AlignModule::Pixel,element,transform);
              // and fill the corresponding map
              (*pixelIdHashMap)[element->identifyHash()] = mod;

              // set transformation
              mod->setGlobalFrameToAlignFrameTransform(element->transform().inverse());

              // add module to the list
              m_alignModuleListPtr->push_back(mod);
            }
            else {
              ATH_MSG_WARNING("No element in module "<<mod->name()<<". Ignoring this module.");
              delete mod;
            }
          }
        }
      }
    }
  }

  //_____________ some new DBM alignment levels (1, 2, and 3)_______________
  //________________________________________________________________________
  void PixelGeometryManagerTool::buildL1DBM()
  {
    // ========================================
    // DBM END CAPS
    ATH_MSG_INFO("Preparing the DBM Endcap geometry for L1 : 2 endcaps");
    
    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the Pixel "<<maxHash);
 
    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel))
    m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel) = new Trk::AlignModuleList((size_t)(maxHash),0);
    Trk::AlignModuleList * pixelIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel);
    
    // create two AlignModules:
    Trk::AlignModule * dbm1 = new Trk::AlignModule(this);
    dbm1->setIdHash(getNextIDHash());
    dbm1->setName("Pixel/ECDBM");
    
    Trk::AlignModule * dbm2 = new Trk::AlignModule(this);
    dbm2->setIdHash(getNextIDHash());
    dbm2->setName("Pixel/EADBM");
    
    dbm1->setIdentifier(m_idHelper->wafer_id(-4, 0, 0, 0));
    dbm2->setIdentifier(m_idHelper->wafer_id(4, 0, 0, 0));
    
    ATH_MSG_DEBUG("Building module "<<dbm1->name());
    ATH_MSG_DEBUG("Building module "<<dbm2->name());
  
    const Amg::Transform3D transform = Amg::Transform3D::Identity();

    for (unsigned int index = 0; index < maxHash; index++)
    {
      IdentifierHash idHash = index;
      Identifier id = m_idHelper->wafer_id(idHash);
      
    // just to be sure check for DBM module
      if(!m_idHelper->is_dbm(id)) continue;
      
//      ATH_MSG_DEBUG(" Found DBM element " << m_idHelper->is_dbm(id));
//      ATH_MSG_DEBUG("DBM bec "<<m_idHelper->barrel_ec(id));
//      ATH_MSG_DEBUG("DBM layer disk "<<m_idHelper->layer_disk(id));
//      ATH_MSG_DEBUG("DBM ring (eta module) "<< m_idHelper->eta_module(id));
//      ATH_MSG_DEBUG("DBM sector (phi module) "<< m_idHelper->phi_module(id));
      
      // check what we're filling
      Trk::AlignModule * mod = 0;
      if( m_idHelper->is_dbm(id) && m_idHelper->barrel_ec(id)==-4) {
        ATH_MSG_DEBUG("pixel element "<<id<<" at index "<<index<<" is in ECDBM");
        mod = dbm1;
      }
      else if( m_idHelper->is_dbm(id) && m_idHelper->barrel_ec(id)==4) {
        ATH_MSG_DEBUG("pixel element "<<id<<" at index "<<index<<" is in EADBM");
        mod = dbm2;
      }
      
      // get the element via hash
      SiDetectorElement * element2 = m_detManager->getDetectorElement(id);
      if (element2) {
        const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;
        
        // add element to the AlignModule
        mod->addDetElement(Trk::AlignModule::Pixel,element,transform);
        // and fill the corresponding map
        (*pixelIdHashMap)[element->identifyHash()] = mod;
      }
      else
      ATH_MSG_DEBUG("No Pixel detector with id:" << id);
    }
    ATH_MSG_DEBUG("-------------------------------------------------------");
    
    // alignment fram for the barrel is the global frame
    // so we don't have to set any additional transform
    if(!moduleSelected(dbm1)) {
      ATH_MSG_DEBUG("Module "<<dbm1->name()<<" NOT selected");
      delete dbm1;
    }
    else {
      // for endcap we move the CoG
      Amg::Translation3D translation(dbm1->centerOfGravity());
      ATH_MSG_DEBUG("Endcap:" << dbm1->name() << " , CoG (" << translation.x() << " , " << translation.y() << " , " << translation.z() << " ) " );
      Amg::Transform3D localtoglobal = translation * Amg::RotationMatrix3D::Identity();
      dbm1->setGlobalFrameToAlignFrameTransform(localtoglobal.inverse());

      m_alignModuleListPtr->push_back(dbm1);
    }
    
    
    if(!moduleSelected(dbm2)) {
      ATH_MSG_DEBUG("Module "<<dbm2->name()<<" NOT selected");
      delete dbm2;
    }
    else {
      // for endcap we move the CoG
      Amg::Translation3D translation(dbm2->centerOfGravity());
      ATH_MSG_DEBUG("Endcap:" << dbm2->name() << " , CoG (" << translation.x() << " , " << translation.y() << " , " << translation.z() << " ) " );
      Amg::Transform3D localtoglobal = translation * Amg::RotationMatrix3D::Identity();
      dbm2->setGlobalFrameToAlignFrameTransform(localtoglobal.inverse());
      
      m_alignModuleListPtr->push_back(dbm2);
    }
  }
  
  
  //________________________________________________________________________
  void PixelGeometryManagerTool::buildL2DBM()
  {
    // ========================================
    // DBM telescopes
    ATH_MSG_INFO("Preparing the DBM telescope geometry for L2 : 8 telescopes");
    
    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the Pixel "<<maxHash);
    
    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel))
    m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel) = new Trk::AlignModuleList((size_t)(maxHash),0);
    Trk::AlignModuleList * pixelIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel);
    
    std::vector<Trk::AlignModule*> mod_list;
    // create 8 AlignModules:
    for (unsigned int i = 0; i < 8; i++)
    {
      
      Trk::AlignModule * mod = new Trk::AlignModule(this);
      mod->setIdHash(getNextIDHash());
      std::stringstream name;
      if(i < 4){
        name <<"Pixel/ECDBM";
        name <<i;
      }
      else {
        name <<"Pixel/EADBM";
        name <<i-4;
      }
      mod->setName(name.str());
      if(i<4) mod->setIdentifier(m_idHelper->wafer_id(-4, 0, i, 0));
      else    mod->setIdentifier(m_idHelper->wafer_id(4, 0, i-4, 0));
      
      mod_list.push_back(mod);
      ATH_MSG_DEBUG("Building module "<<mod->name());
    }
    
    const Amg::Transform3D transform = Amg::Transform3D::Identity();
    
    // ==================================================================
    // loop over Pixel elements and add them to respective alignModules
    // ==================================================================
    for (unsigned int index = 0; index < maxHash; index++)
    {
      IdentifierHash idHash = index;
      Identifier id = m_idHelper->wafer_id(idHash);
      
      // just to be sure check for DBM module
      if(!m_idHelper->is_dbm(id)) continue;
      
//      ATH_MSG_DEBUG(" Found DBM element " << m_idHelper->is_dbm(id));
//      ATH_MSG_DEBUG("DBM bec "<<m_idHelper->barrel_ec(id));
//      ATH_MSG_DEBUG("DBM layer disk "<<m_idHelper->layer_disk(id));
//      ATH_MSG_DEBUG("DBM ring (eta module) "<< m_idHelper->eta_module(id));
//      ATH_MSG_DEBUG("DBM sector (phi module) "<< m_idHelper->phi_module(id));
      
      // check what we're filling
      unsigned int telescope = -1; // set to non existing telescope
      Trk::AlignModule * mod = 0;
      if( m_idHelper->is_dbm(id) && m_idHelper->barrel_ec(id)==-4) {
        ATH_MSG_DEBUG("pixel element "<<id<<" at index "<<index<<" with sector "<<m_idHelper->phi_module(id)<<" is in ECDBM");
        telescope = int(m_idHelper->phi_module(id));
        mod = mod_list.at(telescope);
      }
      else if( m_idHelper->is_dbm(id) && m_idHelper->barrel_ec(id)==4) {
        ATH_MSG_DEBUG("pixel element "<<id<<" at index "<<index<<" with sector "<<m_idHelper->phi_module(id)<<" is in EADBM");
        telescope = int(4+m_idHelper->phi_module(id));
        mod = mod_list.at(telescope);
      }
      
      // get the element via hash
      SiDetectorElement * element2 = m_detManager->getDetectorElement(id);
      if (element2) {
        const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;
        
        // add element to the AlignModule
        mod->addDetElement(Trk::AlignModule::Pixel,element,transform);
        // and fill the corresponding map
        (*pixelIdHashMap)[element->identifyHash()] = mod;
      }
      else
      ATH_MSG_DEBUG("No Pixel detector with id:" << id);
    }
    ATH_MSG_DEBUG("-------------------------------------------------------");
    
    // alignment fram for the barrel is the global frame
    // so we don't have to set any additional transform
    for(unsigned int i=0;i<mod_list.size();i++) {
      if(!moduleSelected(mod_list.at(i))) {
        ATH_MSG_DEBUG("Module "<<mod_list.at(i)->name()<<" NOT selected");
        //delete mod_list(i);
      }
      else {
        // for endcap we move the CoG
        Amg::Translation3D translation(mod_list.at(i)->centerOfGravity());
        ATH_MSG_DEBUG("Endcap:" << mod_list.at(i)->name() << " , CoG (" << translation.x() << " , " << translation.y() << " , " << translation.z() << " ) " );
        Amg::Transform3D localtoglobal = translation * Amg::RotationMatrix3D::Identity();
        mod_list.at(i)->setGlobalFrameToAlignFrameTransform(localtoglobal.inverse());
        
        m_alignModuleListPtr->push_back(mod_list.at(i));
      }
    }
  }

  
  //________________________________________________________________________
  void PixelGeometryManagerTool::buildL3DBM()
  {
    // ========================================
    // DBM telescopes
    ATH_MSG_INFO("Preparing the DBM module geometry for L3 : 24 modules");
    
    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the Pixel "<<maxHash);
    
    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel) = new Trk::AlignModuleList((size_t)(maxHash),0);
    Trk::AlignModuleList * pixelIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel);
    
    // ==================================================================
    // loop over Pixel elements and add them to respective alignModules
    // ==================================================================
    for (unsigned int index = 0; index < maxHash; index++)
    {
      IdentifierHash idHash = index;
      Identifier id = m_idHelper->wafer_id(idHash);
      
      // just to be sure check for DBM module
      if(!m_idHelper->is_dbm(id)) continue;
      
//      ATH_MSG_DEBUG(" Found DBM element " << m_idHelper->is_dbm(id));
//      ATH_MSG_DEBUG("DBM bec "<<m_idHelper->barrel_ec(id));
//      ATH_MSG_DEBUG("DBM layer disk "<<m_idHelper->layer_disk(id));
//      ATH_MSG_DEBUG("DBM ring (eta module) "<< m_idHelper->eta_module(id));
//      ATH_MSG_DEBUG("DBM sector (phi module) "<< m_idHelper->phi_module(id));
      
      Trk::AlignModule * mod = new Trk::AlignModule(this);
      mod->setIdHash(getNextIDHash());
      std::stringstream name;
      if(m_idHelper->barrel_ec(id)==-4){
        name <<"Pixel/ECDBM";
      }
      else {
        name <<"Pixel/EADBM";
      }
      name << "/Disk_"<<m_idHelper->layer_disk(id)<<"/Sector_"<<m_idHelper->phi_module(id);
      mod->setName(name.str());
      mod->setIdentifier(m_idHelper->wafer_id(m_idHelper->barrel_ec(id), m_idHelper->layer_disk(id), m_idHelper->phi_module(id), 0));
      
      
      ATH_MSG_DEBUG("Building module "<<mod->name());
      
      const SiDetectorElement * element2 = m_detManager->getDetectorElement(id);
      const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;
      
      if (element) {
 
        // db frame constants are in the global frame. Or not?
        Amg::Transform3D transform = Amg::Transform3D::Identity();
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      
        // add to the barrel
        mod->addDetElement(Trk::AlignModule::Pixel,element,transform);
        // and fill the corresponding map
        (*pixelIdHashMap)[element->identifyHash()] = mod;
      
        // set transformation
        mod->setGlobalFrameToAlignFrameTransform(element->transform().inverse());
      
        // add module to the list
        m_alignModuleListPtr->push_back(mod);
      }
      else {
        ATH_MSG_WARNING("No element in module "<<mod->name()<<". Ignoring this module.");
        delete mod;
      }
    }
  }
  
  
  //________________________________________________________________________
  void PixelGeometryManagerTool::addModuleParameters(Trk::AlignModule * module, DataVector< DataVector<Trk::AlignPar> > * allFullModPars, DataVector< DataVector<Trk::AlignPar> > * allActiveModPars)
  {
    // prepare all parameters
    DataVector<Trk::AlignPar> * fullModPars = new DataVector<Trk::AlignPar>(SG::OWN_ELEMENTS);
    fullModPars->push_back(new Trk::AlignPar(module,Trk::AlignModule::TransX));
    fullModPars->push_back(new Trk::AlignPar(module,Trk::AlignModule::TransY));
    fullModPars->push_back(new Trk::AlignPar(module,Trk::AlignModule::TransZ));
    fullModPars->push_back(new Trk::AlignPar(module,Trk::AlignModule::RotX));
    fullModPars->push_back(new Trk::AlignPar(module,Trk::AlignModule::RotY));
    fullModPars->push_back(new Trk::AlignPar(module,Trk::AlignModule::RotZ));
    fullModPars->push_back(new Trk::AlignPar(module,Trk::AlignModule::BowX));

    // set sigmas
    setSigmas(module,fullModPars);

    // select active parameters based on jobOption properties
    DataVector<Trk::AlignPar> * activeModPars = new DataVector<Trk::AlignPar>(SG::VIEW_ELEMENTS);
    for(unsigned int ipar=0;ipar<fullModPars->size();++ipar) {

      Identifier AlimodID = module->identify();

      Identifier modID = ((module->detElementCollection(Trk::AlignModule::Pixel))->at(0))->identify();

      if( m_idHelper->is_barrel(modID) || m_alignLevel==1  || m_alignLevel==11) {
        ATH_MSG_DEBUG("Pixel barrel module with id "<<AlimodID);
        if(  (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransX && m_alignBarrelX)
	     || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransY && m_alignBarrelY)
	     || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransZ && m_alignBarrelZ)
	     || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotX   && m_alignBarrelRotX)
	     || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotY   && m_alignBarrelRotY)
	     || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotZ   && m_alignBarrelRotZ)
	     || (m_idHelper->layer_disk(modID)==0 && m_idHelper->is_blayer(modID)  && fullModPars->at(ipar)->paramType() == Trk::AlignModule::BowX   && m_alignBarrelBowX) )  { //Bowing is only allowed for the IBL for now
          ATH_MSG_DEBUG("parameter type "<<fullModPars->at(ipar)->paramType()<<" \'"<<fullModPars->at(ipar)->dumpType()<<"\' is now active");
          activeModPars->push_back(fullModPars->at(ipar));
        }
        else
          ATH_MSG_DEBUG("parameter type "<<fullModPars->at(ipar)->paramType()<<" \'"<<fullModPars->at(ipar)->dumpType()<<"\' is NOT active");
      }
      else {
        ATH_MSG_DEBUG("Pixel endcap module with id "<<AlimodID);
        if(  (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransX && m_alignEndcapX)
	     || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransY && m_alignEndcapY)
	     || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransZ && m_alignEndcapZ)
	     || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotX   && m_alignEndcapRotX)
	     || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotY   && m_alignEndcapRotY)
	     || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotZ   && m_alignEndcapRotZ) ) {
          ATH_MSG_DEBUG("parameter type "<<fullModPars->at(ipar)->paramType()<<" \'"<<fullModPars->at(ipar)->dumpType()<<"\' is now active");
          activeModPars->push_back(fullModPars->at(ipar));
        }
        else
          ATH_MSG_DEBUG("parameter type "<<fullModPars->at(ipar)->paramType()<<" \'"<<fullModPars->at(ipar)->dumpType()<<"\' is NOT active");
      }
    }

    // now add parameters to the list
    allFullModPars->push_back(fullModPars);
    allActiveModPars->push_back(activeModPars);
  }

  //________________________________________________________________________
  void PixelGeometryManagerTool::setSigmas(Trk::AlignModule * module, DataVector<Trk::AlignPar> * modPars)
  {
    for(unsigned int ipar=0;ipar<modPars->size();++ipar) {
      // barrel
      if(m_idHelper->is_barrel(module->identify()))
        switch(modPars->at(ipar)->paramType()) {
          case Trk::AlignModule::TransX:
            modPars->at(ipar)->setSigma(m_sigmaBarrelX);
            modPars->at(ipar)->setSoftCut(m_softcutBarrelX);
            break;
          case Trk::AlignModule::TransY:
            modPars->at(ipar)->setSigma(m_sigmaBarrelY);
            modPars->at(ipar)->setSoftCut(m_softcutBarrelY);
            break;
          case Trk::AlignModule::TransZ:
            modPars->at(ipar)->setSigma(m_sigmaBarrelZ);
            modPars->at(ipar)->setSoftCut(m_softcutBarrelZ);
            break;
          case Trk::AlignModule::RotX:
            modPars->at(ipar)->setSigma(m_sigmaBarrelRotX);
            modPars->at(ipar)->setSoftCut(m_softcutBarrelRotX);
            break;
          case Trk::AlignModule::RotY:
            modPars->at(ipar)->setSigma(m_sigmaBarrelRotY);
            modPars->at(ipar)->setSoftCut(m_softcutBarrelRotY);
            break;
          case Trk::AlignModule::RotZ:
            modPars->at(ipar)->setSigma(m_sigmaBarrelRotZ);
            modPars->at(ipar)->setSoftCut(m_softcutBarrelRotZ);
            break;
          case Trk::AlignModule::BowX:
            modPars->at(ipar)->setSigma(m_sigmaBarrelBowX);
            modPars->at(ipar)->setSoftCut(m_softcutBarrelBowX);
          default:
            break;
        }
      // end-caps
      else
        switch(modPars->at(ipar)->paramType()) {
          case Trk::AlignModule::TransX:
            modPars->at(ipar)->setSigma(m_sigmaEndcapX);
            modPars->at(ipar)->setSoftCut(m_softcutEndcapX);
            break;
          case Trk::AlignModule::TransY:
            modPars->at(ipar)->setSigma(m_sigmaEndcapY);
            modPars->at(ipar)->setSoftCut(m_softcutEndcapY);
            break;
          case Trk::AlignModule::TransZ:
            modPars->at(ipar)->setSigma(m_sigmaEndcapZ);
            modPars->at(ipar)->setSoftCut(m_softcutEndcapZ);
            break;
          case Trk::AlignModule::RotX:
            modPars->at(ipar)->setSigma(m_sigmaEndcapRotX);
            modPars->at(ipar)->setSoftCut(m_softcutEndcapRotX);
            break;
          case Trk::AlignModule::RotY:
            modPars->at(ipar)->setSigma(m_sigmaEndcapRotY);
            modPars->at(ipar)->setSoftCut(m_softcutEndcapRotY);
            break;
          case Trk::AlignModule::RotZ:
            modPars->at(ipar)->setSigma(m_sigmaEndcapRotZ);
            modPars->at(ipar)->setSoftCut(m_softcutEndcapRotZ);
            break;
          default:
            break;
        }
    }
  }

  //________________________________________________________________________
  bool PixelGeometryManagerTool::moduleSelected(unsigned long long id)
  {
    if(!m_doModuleSelection)
      return true;

    int nsel = m_moduleSelection.size();
    for(int i=0;i<nsel;++i)
      if(m_moduleSelection.at(i) == id)
        return true;

    return false;
  }

  //________________________________________________________________________
  bool PixelGeometryManagerTool::moduleSelected(Trk::AlignModule * mod)
  {
    return moduleSelected(mod->identify().get_compact());
  }

  //________________________________________________________________________
  void PixelGeometryManagerTool::dumpGeometry()
  {
    ATH_MSG_INFO("---------------------------------------------------");
    ATH_MSG_INFO("Summary of the alignment geometry");
    ATH_MSG_INFO("Number of alignable objects: "<< m_alignModuleList.size());
    for(unsigned int i=0;i<m_alignModuleList.size();i++) {
      const Trk::AlignModule* module = m_alignModuleList.at(i);
      ATH_MSG_INFO(i<<". "<< module->name());
      ATH_MSG_INFO("   - identifier: "<<module->identify());
      ATH_MSG_INFO("   - has "<<module->detElementCollection(Trk::AlignModule::Pixel)->size()<<" Pixel modules");
      
      Amg::Transform3D localtoglobal = (module->globalFrameToAlignFrame()).inverse();
      ATH_MSG_DEBUG("   - local to global : "<<std::setprecision(12)<<localtoglobal.translation()<<" "<<localtoglobal.rotation());
      
      DataVector<Trk::AlignPar> * pars = m_alignModuleTool->getAlignPars(module);
      int npars = pars->size();
      ATH_MSG_DEBUG("   - number of active transform parameters: "<<npars);
      for(int j=0;j<npars;j++)
         ATH_MSG_DEBUG("      * par "<<j<<" \'"<<(*pars)[j]->dumpType()<<"\' : sigma = "<<(*pars)[j]->sigma()<<" , softCut = "<<(*pars)[j]->softCut());
    }
    ATH_MSG_INFO("---------------------------------------------------");
  }

  //________________________________________________________________________
  bool PixelGeometryManagerTool::isTopHalf(unsigned int phi, unsigned int layer) const
  {
    // boundary between top and botom halfs is different for each layer
    // B-layer top, phi index from 1 to 10
    // B-layer bottom, phi index 0 and from 11 to 21
    // Layer-1 top, from 0 to 19
    // Layer-1 botttom, from 20 to 37
    // Layer-2 top, from 49 to 51 and from 0 to 22
    // Layer-2 bottom, from 23 to 48
    if (layer==0) {
      if (phi>21) {
        msg(MSG::ERROR)<<"Layer 0 has only 22 phi modules: range 0-21, requested "<<phi<<endmsg;
        return false;
      }
      if (phi>0 && phi<11)
        return true;
      else
        return false;
    }
    else if (layer==1) {
      if(phi>37) {
        msg(MSG::ERROR)<<"Layer 1 has only 38 phi modules: range 0-37, requested "<<phi<<endmsg;
        return false;
      }
      else if (phi<20)
        return true;
      else
        return false;
    }
    else if (layer==2) {
      if(phi>52) {
        msg(MSG::ERROR)<<"Layer 2 has only 52 phi modules: range 0-51, requested "<<phi<<endmsg;
        return false;
      }
      else if (phi<23 || phi>48)
        return true;
      else
        return false;
    }

    // if we got here layer number is wrong
    msg(MSG::ERROR)<<"Pixel barrel only 3 layers: range 0-2, requested "<<layer<<endmsg;
    return false;
  }

  //________________________________________________________________________
  void PixelGeometryManagerTool::printTransform(const Amg::Transform3D & tr) const
  {
    ATH_MSG_DEBUG(" - translation: "<<tr.translation().x()<<"  "<<tr.translation().y()<<"  "<<tr.translation().z());
    ATH_MSG_DEBUG(" - rotation:");
    ATH_MSG_DEBUG("      "<<tr.rotation()(0,0)<<"  "<<tr.rotation()(0,1)<<"  "<<tr.rotation()(0,2));
    ATH_MSG_DEBUG("      "<<tr.rotation()(1,0)<<"  "<<tr.rotation()(1,1)<<"  "<<tr.rotation()(1,2));
    ATH_MSG_DEBUG("      "<<tr.rotation()(2,0)<<"  "<<tr.rotation()(2,1)<<"  "<<tr.rotation()(2,2));
  }

  //________________________________________________________________________
  int PixelGeometryManagerTool::getNoSkipEtaValueFromGeometry() const
  {

    ATH_MSG_INFO("Decode geometry version key");
    DecodeVersionKey versionKey(&*m_geoModelSvc, "Pixel");
    
    std::string detectorKey  = versionKey.tag();
    std::string detectorNode = versionKey.node();

    ATH_MSG_INFO("--------------------- decode version key " << detectorKey<<"  "<<detectorNode);

    // Read geometry DB to get NOSKIPZERO value

    // StaveIndex corresponding to IBL layer  : CAUTION the assumption is made that layer 0 is defined at line 0
    IRDBRecordset_ptr geoSet_layer = m_rdbAccessSvc->getRecordsetPtr("PixelLayer", detectorKey, detectorNode);
    const IRDBRecord *geoTable_layer = (*geoSet_layer)[0];
    int staveIndex=0;
    if (!geoTable_layer->isFieldNull("STAVEINDEX")) staveIndex=geoTable_layer->getInt("STAVEINDEX");
    
    // Read NoSkipEtaZero for IBL layer
    IRDBRecordset_ptr geoSet_stave = m_rdbAccessSvc->getRecordsetPtr("PixelStave", detectorKey, detectorNode);
    const IRDBRecord *geoTable_stave = (*geoSet_stave)[staveIndex];
    int noSkipZero=0;
    if (!geoTable_stave->isFieldNull("NOSKIPZERO")) noSkipZero=geoTable_stave->getInt("NOSKIPZERO");

    return noSkipZero;
}

} // end namespace

