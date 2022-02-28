/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "AthContainers/DataVector.h"

#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetIdentifier/SCT_ID.h"

#include "TrkAlignInterfaces/IAlignModuleTool.h"

#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignEvent/AlignPar.h"



#include "InDetAlignGeomTools/SCTGeometryManagerTool.h"


#include <iostream>

using namespace InDetDD;

namespace InDet {

  //________________________________________________________________________
  SCTGeometryManagerTool::SCTGeometryManagerTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface * parent)
    : AthAlgTool(type,name,parent)
    , m_detManager(nullptr)
    , m_idHelper()
    , m_alignModuleTool("Trk::AlignModuleTool/AlignModuleTool")
    , m_idHashToAlignModuleMaps(Trk::AlignModule::NDetectorTypes,(Trk::AlignModuleList*)nullptr)
    , m_alignParList(nullptr)
    , m_fullAlignParList(nullptr)
  {
    declareInterface<IGeometryManagerTool>(this);
    declareProperty("AlignModuleTool",    m_alignModuleTool);

    // Pixel joboptions
    declareProperty("AlignBarrel",    m_alignBarrel      = true);
    declareProperty("AlignEndcaps",   m_alignEndcaps     = true);

    declareProperty("AlignBarrelX",    m_alignBarrelX    = true);
    declareProperty("AlignBarrelY",    m_alignBarrelY    = true);
    declareProperty("AlignBarrelZ",    m_alignBarrelZ    = true);
    declareProperty("AlignBarrelRotX", m_alignBarrelRotX = true);
    declareProperty("AlignBarrelRotY", m_alignBarrelRotY = true);
    declareProperty("AlignBarrelRotZ", m_alignBarrelRotZ = true);

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

    declareProperty("doModuleSelection",  m_doModuleSelection = false);
    declareProperty("ModuleSelection",    m_moduleSelection);

    declareProperty("UseOldL2",           m_useOldL2 = false);
    declareProperty("DumpGeometry",       m_dumpGeometry = true);

    m_hashCounter = 0;
    m_logStream = nullptr;
  }

  //________________________________________________________________________
  SCTGeometryManagerTool::~SCTGeometryManagerTool()
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
  StatusCode SCTGeometryManagerTool::initialize()
  {
    ATH_MSG_DEBUG("initialize() of SCTGeometryManagerTool");

    // retrieve AlignModuleTool
    if ( m_alignModuleTool.retrieve().isFailure() ) {
      msg(MSG::FATAL)<<"Could not get " << m_alignModuleTool << endmsg;
      return StatusCode::FAILURE;
    }
    else
      ATH_MSG_INFO("Retrieved " << m_alignModuleTool);

    // retrieve SCT helper
    if ( detStore()->retrieve(m_idHelper).isFailure() ) {
      msg(MSG::FATAL) << " Cannot retrieve SCT Helper " << endmsg;
      return StatusCode::FAILURE;
    }
    else
      ATH_MSG_INFO("retrieved Silicon SCT Helper");

    // retrieve SCT detector manager
    if ( detStore()->retrieve(m_detManager, "SCT").isFailure() ) {
      msg(MSG::FATAL) << " Cannot retrieve SCT Detector Manager " << endmsg;
      return StatusCode::FAILURE;
    }
    else
      ATH_MSG_INFO("retrieved SCT Detector Manager");

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
  StatusCode SCTGeometryManagerTool::finalize()
  {
    ATH_MSG_DEBUG("finalize() of SCTGeometryManagerTool");

    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  void SCTGeometryManagerTool::setAlignLevel(int level)
  {
     m_alignLevelBarrel = m_alignLevelEndcaps = -1;
     m_alignLevel = level;
  }

  //________________________________________________________________________
  bool SCTGeometryManagerTool::checkAlignLevel()
  {
    if(m_alignLevel == 0) {
      ATH_MSG_INFO("Alignment level for SCT is "<<m_alignLevel);
      return true;
    }

    if(m_alignLevel == 2 && m_useOldL2) {
      ATH_MSG_INFO("Setting up OLD L2 alignment level for SCT");
      m_alignLevelBarrel = m_alignLevel;
      m_alignLevelEndcaps = m_alignLevel;
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
  bool SCTGeometryManagerTool::checkAlignLevelBarrel()
  {
    if(!m_alignBarrel)
      return true;

    // check whether geometry level is allowed
    switch(m_alignLevelBarrel) {
      case 1: case 2: case 27: case 3:
        ATH_MSG_INFO("Alignment level for SCT Barrel is "<<m_alignLevelBarrel);
        return true;
      default:
        msg(MSG::FATAL)<<"Alignment level "<<m_alignLevelBarrel<<" does not exist for SCT Barrel"<<endmsg;
        return false;
    }
  }

  //________________________________________________________________________
  bool SCTGeometryManagerTool::checkAlignLevelEndcaps()
  {
    if(!m_alignEndcaps)
      return true;

    // check whether geometry level is allowed
    switch(m_alignLevelEndcaps) {
      case 1: case 2: case 25: case 3:
        ATH_MSG_INFO("Alignment level for SCT Endcaps is "<<m_alignLevelBarrel);
        return true;
      default:
        msg(MSG::FATAL)<<"Alignment level "<<m_alignLevelEndcaps<<" does not exist for SCT Endcaps"<<endmsg;
        return false;
    }
  }

  //________________________________________________________________________
  int SCTGeometryManagerTool::ReadGeometry(int solveLevel)
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
  void SCTGeometryManagerTool::buildGeometry()
  {
    ATH_MSG_INFO("Preparing the SCT geometry");

    if(m_alignLevel == 0)
      buildL0();
    else if (m_alignLevel == 2 && m_useOldL2)
      buildL2Old();
    else {

      if(m_alignBarrel)
        switch(m_alignLevelBarrel) {
          case 1:
            buildL1Barrel();
            break;
          case 2:
            buildL2Barrel();
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
          case 1:
            buildL1Endcaps();
            break;
          case 2:
            buildL2Endcaps();
            break;
          case 25:
            buildL25Endcaps();
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
  void SCTGeometryManagerTool::buildL0()
  {
    ATH_MSG_INFO("Preparing the SCT geometry for L0: the whole SCT");
    // ===================================================
    // Level 0 is just one module containing the whole SCT

    Trk::AlignModule * sct = new Trk::AlignModule(this);
    sct->setIdHash(getNextIDHash());
    sct->setName("SCT");

    // use the identifier of the SCT barrel for the whole SCT at L0
    sct->setIdentifier(m_idHelper->wafer_id(0,0,0,0,0));

    ATH_MSG_DEBUG("Created module "<<sct->name()<<"  idHash: "<<sct->identifyHash()<<"  identifier: "<<sct->identify());

    // for L0 alignment the alignment frame is equal to the global frame
    // and since the SCT detector element positions are also stored
    // in the global frame in DB, transform is identity
    const Amg::Transform3D transform = Amg::Transform3D::Identity();

    // get maximum number of elements from the helper
    unsigned int sctmaxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the SCT: "<<sctmaxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT) = new Trk::AlignModuleList((size_t)(sctmaxHash),nullptr);
    Trk::AlignModuleList * sctIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT);

    // ================================================================
    // loop over SCT elements and add them to respective alignModules
    // ================================================================
    for (unsigned int index = 0; index < sctmaxHash; index++) {
      IdentifierHash idHash = index;
      Identifier id = m_idHelper->wafer_id(idHash);

      ATH_MSG_DEBUG(" SCT DetectorElement idhash: "<<index);
      ATH_MSG_DEBUG(" DetectorElement id: "<<id);

      // get the element via hash
      SiDetectorElement * element2 = m_detManager->getDetectorElement(id);
      if (element2) {
        const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

        // add element to respective AlignModule

        // add to the sct barrel structure
        if(msgLvl(MSG::DEBUG)) {
          if (m_idHelper->is_barrel(id))
            msg(MSG::DEBUG)<<"... SCT barrel element"<<endmsg;
          else
            msg(MSG::DEBUG)<<"... SCT endcap element"<<endmsg;
        }
        sct->addDetElement(Trk::AlignModule::SCT,element,transform);

        // and fill the corresponding map
        (*sctIdHashMap)[idHash] = sct;
      }
      else
        ATH_MSG_DEBUG("No SCT detector with id: "<<id);
    }

    // add created module to the geometry
    m_alignModuleListPtr->push_back(sct);

    ATH_MSG_DEBUG("SCT L0 module successfully added to the geometry");
  }

  //_______________________________________________________________________
  void SCTGeometryManagerTool::buildL1Barrel()
  {
    // ===========================
    // BARREL
    // ===========================
    ATH_MSG_INFO("Preparing the SCT Barrel geometry for L1: barrel + 2 endcaps");

    // ========================================
    // Level 1 is whole SCT barrel

    // get maximum number of elements from the helper
    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the SCT "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT) = new Trk::AlignModuleList((size_t)(maxHash),nullptr);
    Trk::AlignModuleList * sctIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT);

    // use the 'real' identifier of the Pixel
    // build barrel module
    Trk::AlignModule * mod = new Trk::AlignModule(this);
    mod->setIdHash(getNextIDHash());
    mod->setName("SCT Barrel");
    mod->setIdentifier(m_idHelper->wafer_id(0,0,0,0,0));

    // check if selected
    if(!m_alignBarrel || !moduleSelected(mod)) {
      ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
      delete mod;
      return;
    }

    // fill module with detector elements
    ATH_MSG_DEBUG("Building module "<<mod->name());

    const Amg::Transform3D transform = Amg::Transform3D::Identity();

    for (int iLayer = 0; iLayer < m_detManager->numerology().numLayers(); iLayer++) {
      if (!m_detManager->numerology().useLayer(iLayer))
        ATH_MSG_INFO("  Layer "<<iLayer<<" not present");

      for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForLayer(iLayer); iPhi++) {
        ATH_MSG_DEBUG("iPhi "<<iPhi);
        for (int iEta = m_detManager->numerology().beginEtaModuleForLayer(iLayer); iEta < m_detManager->numerology().endEtaModuleForLayer(iLayer); iEta++) {
          ATH_MSG_DEBUG("iEta "<<iEta);

          if (!iEta && m_detManager->numerology().skipEtaZeroForLayer(iLayer))
            // iEta=0 is not defined for the SCT
            continue;

          for (int is = 0; is < 2; is++) { // module side

            const SiDetectorElement * element2 = m_detManager->getDetectorElement(0, iLayer, iPhi, iEta, is);
            const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

            if (element) {
              // get element location for debugging
              // HepGeom::Point3D<double> center = element->transform() * HepGeom::Point3D<double>();
              // ATH_MSG_DEBUG(" SCTDetectorElement id: " << id << " with center = " << center);
              // ATH_MSG_DEBUG(" Is Barrel: "<< m_idHelper->is_barrel(id));

              // add element to the AlignModule
              mod->addDetElement(Trk::AlignModule::SCT,element,transform);
              // and fill the corresponding map
              (*sctIdHashMap)[element->identifyHash()] = mod;
            }
          }
        }
      }
    }

    m_alignModuleListPtr->push_back(mod);

    ATH_MSG_DEBUG("SCT Barrel successfully added to the geometry");
  }

  //_______________________________________________________________________
  void SCTGeometryManagerTool::buildL1Endcaps()
  {
    // ===========================
    // ENDCAPs
    // ===========================
    ATH_MSG_INFO("Preparing the SCT Endcap geometry for L1: 2 endcaps");

    // ========================================
    // Level 1 is 2 SCT endcaps

    // get maximum number of elements from the helper
    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the SCT "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT) = new Trk::AlignModuleList((size_t)(maxHash),nullptr);
    Trk::AlignModuleList * sctIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT);

    for (int iEndcapIndex = 0; iEndcapIndex < m_detManager->numerology().numEndcaps(); iEndcapIndex++) {
      int iSide = m_detManager->numerology().endcapId(iEndcapIndex);

      Trk::AlignModule * mod = new Trk::AlignModule(this);
      mod->setIdHash(getNextIDHash());
      mod->setIdentifier(m_idHelper->wafer_id(iSide,0,0,0,0));
      if(iEndcapIndex == 0)
        mod->setName("SCT EndCap A");
      else
        mod->setName("SCT EndCap C");

      if(!m_alignEndcaps || !moduleSelected(mod)) {
        ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
        delete mod;
        continue;
      }

      ATH_MSG_DEBUG("Building module "<<mod->name());

      const Amg::Transform3D transform = Amg::Transform3D::Identity();

      for (int iWheel = 0; iWheel < m_detManager->numerology().numDisks(); iWheel++) {
        if (!m_detManager->numerology().useDisk(iWheel))
          ATH_MSG_INFO("  Disk "<<iWheel<<" not present");

        for (int iEta = 0; iEta < m_detManager->numerology().numRingsForDisk(iWheel); iEta++) {
          ATH_MSG_DEBUG("iEta "<<iEta);
          for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForDiskRing(iWheel,iEta); iPhi++) {
            ATH_MSG_DEBUG("iPhi "<<iPhi);
            for(int is=0; is<2; is++) { // module side

              const SiDetectorElement * element2 = m_detManager->getDetectorElement(iSide, iWheel, iPhi, iEta, is);
              const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;
              if (element) {
                // get element location for debugging
                // HepGeom::Point3D<double> center = element->transform() * HepGeom::Point3D<double>();
                // ATH_MSG_DEBUG(" SCTDetectorElement id: " << id << " with center = " << center);
                // ATH_MSG_DEBUG(" Is Barrel: "<< m_idHelper->is_barrel(id));

                // add element to the AlignModule
                mod->addDetElement(Trk::AlignModule::SCT,element,transform);
                // and fill the corresponding map
                (*sctIdHashMap)[element->identifyHash()] =  mod;
              }
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

    ATH_MSG_DEBUG("SCT End-caps successfully added to the geometry");
  }

  //_______________________________________________________________________
  void SCTGeometryManagerTool::buildL2Barrel()
  {
    // ========================================
    // BARREL
    ATH_MSG_INFO("Preparing the SCT Barrel geometry for L2: 4 layers");

    // ========================================
    // get all modules for Level 2 alignment of the Barrel
    // Level 2 is 4 SCT barrel layers

    const Amg::Transform3D transform = Amg::Transform3D::Identity();

    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the SCT "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT) = new Trk::AlignModuleList((size_t)(maxHash),nullptr);
    Trk::AlignModuleList * sctIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT);

    for (int iLayer = 0; iLayer < m_detManager->numerology().numLayers(); iLayer++) {
      if (!m_detManager->numerology().useLayer(iLayer))
        ATH_MSG_INFO("  Layer "<<iLayer<<" not present");

      // create the AlignModule
      Trk::AlignModule * mod = new Trk::AlignModule(this);
      mod->setIdHash(getNextIDHash());
      mod->setIdentifier(m_idHelper->wafer_id(0, iLayer, 0, 0, 0));

      std::stringstream name;
      name <<"SCT/Barrel/Layer_"<<iLayer;
      mod->setName(name.str());

      if(!m_alignBarrel || !moduleSelected(mod)) {
        ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
        delete mod;
        continue;
      }

      ATH_MSG_DEBUG("Building module "<<mod->name());

      for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForLayer(iLayer); iPhi++) {
        ATH_MSG_DEBUG("iPhi "<<iPhi);
        for (int iEta = m_detManager->numerology().beginEtaModuleForLayer(iLayer); iEta < m_detManager->numerology().endEtaModuleForLayer(iLayer); iEta++) {
          ATH_MSG_DEBUG("iEta "<<iEta);
          if (!iEta && m_detManager->numerology().skipEtaZeroForLayer(iLayer))
            // iEta=0 is not defined for the SCT
            continue;
          for (int is = 0; is < 2; is++) { // module side

            const SiDetectorElement * element2 = m_detManager->getDetectorElement(0, iLayer, iPhi, iEta, is);
            const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

            if (element) {
              // get element location for debugging
              // HepGeom::Point3D<double> center = element->transform() * HepGeom::Point3D<double>();
              // ATH_MSG_DEBUG(" SCTDetectorElement id: " << id << " with center = " << center);
              // ATH_MSG_DEBUG(" Is Barrel: "<< m_idHelper->is_barrel(id));

              // add element to the AlignModule
              mod->addDetElement(Trk::AlignModule::SCT,element,transform);
              // and fill the corresponding map
              (*sctIdHashMap)[element->identifyHash()] = mod;
            }
          }
        }
      }

      m_alignModuleListPtr->push_back(mod);
    }
  }

  //_______________________________________________________________________
  void SCTGeometryManagerTool::buildL27Barrel()
  {
    // ========================================
    // BARREL
    ATH_MSG_INFO("Preparing the SCT Barrel geometry for L27: 176 staves (ladders)");

    if(!m_alignBarrel) {
      ATH_MSG_DEBUG("Not aligning barrel");
      return;
    }

    // ========================================
    // get all modules for Level 27 alignment of the Barrel
    // 176 staves (ladders)

    const Amg::Transform3D transform = Amg::Transform3D::Identity();

    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the SCT "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT) = new Trk::AlignModuleList((size_t)(maxHash),nullptr);
    Trk::AlignModuleList * sctIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT);

    for (int iLayer = 0; iLayer < m_detManager->numerology().numLayers(); iLayer++) {
      if (!m_detManager->numerology().useLayer(iLayer))
        ATH_MSG_INFO("  Layer "<<iLayer<<" not present");
      for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForLayer(iLayer); iPhi++) {
        ATH_MSG_DEBUG("iPhi "<<iPhi);

        // create the AlignModule
        Trk::AlignModule * mod = new Trk::AlignModule(this);
        mod->setIdHash(getNextIDHash());

        // even though there is no iEta=0 module for SCT barrel, the
        // Identifier is still valid so we use it for the stave
        mod->setIdentifier(m_idHelper->wafer_id(0, iLayer, iPhi, 0, 0));

        std::stringstream name;
        name <<"SCT/Barrel/Layer_"<<iLayer<<"/PhiStave_"<<iPhi;
        mod->setName(name.str());

        if(!moduleSelected(mod)) {
          ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
          delete mod;
          continue;
        }

        ATH_MSG_DEBUG("Building module "<<mod->name());

        // rotation of the stave alignment frame
        Amg::RotationMatrix3D rotation; rotation.setIdentity();

        for (int iEta = m_detManager->numerology().beginEtaModuleForLayer(iLayer); iEta < m_detManager->numerology().endEtaModuleForLayer(iLayer); iEta++) {
          ATH_MSG_DEBUG("iEta "<<iEta);
          if (!iEta && m_detManager->numerology().skipEtaZeroForLayer(iLayer))
            // iEta=0 is not defined for the SCT
            continue;
          for (int is = 0; is < 2; is++) { // module side

            const SiDetectorElement * element2 = m_detManager->getDetectorElement(0, iLayer, iPhi, iEta, is);
            const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

            if (element) {
              // get element location for debugging
              // HepGeom::Point3D<double> center = element->transform() * HepGeom::Point3D<double>();
              // ATH_MSG_DEBUG(" SCTDetectorElement id: " << id << " with center = " << center);
              // ATH_MSG_DEBUG(" Is Barrel: "<< m_idHelper->is_barrel(id));

              // add element to the AlignModule
              mod->addDetElement(Trk::AlignModule::SCT,element,transform);
              // and fill the corresponding map
              (*sctIdHashMap)[element->identifyHash()] = mod;
            }

            // for the stave alignment frame rotation we use the one of the iEta=1
            // non-stereo side (which is the module local frame)
            if(iEta==1 && !element2->isStereo())
              rotation = element2->moduleTransform().rotation();
          }
        }

        // we set the alignment frame to be the CoG of the stave
        // with rotation being the one of the iEta=1 module set above
        Amg::Translation3D translation(mod->centerOfGravity());
        Amg::Transform3D localToGlobal = translation * rotation;

      
        ATH_MSG_DEBUG("Prepared local to global transform :");
        ATH_MSG_DEBUG(" - translation: "<<localToGlobal.translation().x()<<"  "<<localToGlobal.translation().y()<<"  "<<localToGlobal.translation().z());
        ATH_MSG_DEBUG(" - rotation:");
        ATH_MSG_DEBUG("      "<<localToGlobal.rotation()(0,0)<<"  "<<localToGlobal.rotation()(0,1)<<"  "<<localToGlobal.rotation()(0,2));
        ATH_MSG_DEBUG("      "<<localToGlobal.rotation()(1,0)<<"  "<<localToGlobal.rotation()(1,1)<<"  "<<localToGlobal.rotation()(1,2));
        ATH_MSG_DEBUG("      "<<localToGlobal.rotation()(2,0)<<"  "<<localToGlobal.rotation()(2,1)<<"  "<<localToGlobal.rotation()(2,2));

        mod->setGlobalFrameToAlignFrameTransform(localToGlobal.inverse());

        m_alignModuleListPtr->push_back(mod);
      }
    }
  }

  //_______________________________________________________________________
  void SCTGeometryManagerTool::buildL2Endcaps()
  {
    // ========================================
    // END CAPS
    ATH_MSG_INFO("Preparing the SCT Endcap geometry for L2: 2 x 9 disks");

    // ========================================
    // get all modules for Level 2 alignment of the Endcaps
    // Level 2 is 2 x 9 SCT Endcap disks

    const Amg::Transform3D transform = Amg::Transform3D::Identity();

    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the SCT "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT) = new Trk::AlignModuleList((size_t)(maxHash),nullptr);
    Trk::AlignModuleList * sctIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT);

    for (int iEndcapIndex = 0; iEndcapIndex < m_detManager->numerology().numEndcaps(); iEndcapIndex++) {
      int iSide = m_detManager->numerology().endcapId(iEndcapIndex);
      for (int iWheel = 0; iWheel < m_detManager->numerology().numDisks(); iWheel++) {
        if (!m_detManager->numerology().useDisk(iWheel))
          ATH_MSG_INFO("  Disk "<<iWheel<<" not present");

        Trk::AlignModule * mod = new Trk::AlignModule(this);
        mod->setIdHash(getNextIDHash());
        mod->setIdentifier(m_idHelper->wafer_id(iSide,iWheel,0,0,0));

        std::stringstream name;
        if(iEndcapIndex == 0)
          name <<"SCT/EndcapA/Disk_" << iWheel;
        else
          name <<"SCT/EndcapC/Disk_" << iWheel;
        mod->setName(name.str());

        if(!m_alignEndcaps || !moduleSelected(mod)) {
          ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
          delete mod;
          continue;
        }

        ATH_MSG_DEBUG("Building module "<<mod->name());

        for (int iEta = 0; iEta < m_detManager->numerology().numRingsForDisk(iWheel); iEta++) {
          ATH_MSG_DEBUG("iEta "<<iEta);
          for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForDiskRing(iWheel,iEta); iPhi++) {
            ATH_MSG_DEBUG("iPhi "<<iPhi);
            for(int is=0; is<2; is++) { // module side

              const SiDetectorElement * element2 = m_detManager->getDetectorElement(iSide, iWheel, iPhi, iEta, is);
              const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;
              if (element) {
                // get element location for debugging
                // HepGeom::Point3D<double> center = element->transform() * HepGeom::Point3D<double>();
                // ATH_MSG_DEBUG(" SCTDetectorElement id: " << id << " with center = " << center);
                // ATH_MSG_DEBUG(" Is Barrel: "<< m_idHelper->is_barrel(id));

                // add element to the AlignModule
                mod->addDetElement(Trk::AlignModule::SCT,element,transform);
                // and fill the corresponding map
                (*sctIdHashMap)[element->identifyHash()] =  mod;
              }
            }
          }
        }

        // for endcap we move the CoG
        Amg::Translation3D translation(mod->centerOfGravity());
        ATH_MSG_DEBUG("Endcap:" << iSide << " , disk: " << iWheel << " , CoG (" << translation.x() << " , " << translation.y() << " , " << translation.z() << " ) " );
        Amg::Transform3D localtoglobal = translation * Amg::RotationMatrix3D::Identity();
        mod->setGlobalFrameToAlignFrameTransform(localtoglobal.inverse());

        m_alignModuleListPtr->push_back(mod);
      }
    }
  }

  //_______________________________________________________________________
  void SCTGeometryManagerTool::buildL25Endcaps()
  {
    // ========================================
    // END CAPS
    ATH_MSG_INFO("Preparing the SCT Endcap geometry for L25: 2 x 22 rings");

    if(!m_alignEndcaps) {
      ATH_MSG_DEBUG("Not aligning endcaps");
      return;
    }

    // ========================================
    // get all modules for Level 2 alignment of the Endcaps
    // 2 x 22 SCT Endcap rings

    const Amg::Transform3D transform = Amg::Transform3D::Identity();

    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the SCT "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT) = new Trk::AlignModuleList((size_t)(maxHash),nullptr);
    Trk::AlignModuleList * sctIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT);

    for (int iEndcapIndex = 0; iEndcapIndex < m_detManager->numerology().numEndcaps(); iEndcapIndex++) {
      int iSide = m_detManager->numerology().endcapId(iEndcapIndex);
      for (int iWheel = 0; iWheel < m_detManager->numerology().numDisks(); iWheel++) {
        if (!m_detManager->numerology().useDisk(iWheel))
          ATH_MSG_INFO("  Disk "<<iWheel<<" not present");
        for (int iEta = 0; iEta < m_detManager->numerology().numRingsForDisk(iWheel); iEta++) {
          ATH_MSG_DEBUG("iEta "<<iEta);

          Trk::AlignModule * mod = new Trk::AlignModule(this);
          mod->setIdHash(getNextIDHash());
          mod->setIdentifier(m_idHelper->wafer_id(iSide,iWheel,iEta,0,0));

          std::stringstream name;
          if(iEndcapIndex == 0)
            name <<"SCT/EndcapA/Disk_"<<iWheel<<"/Ring_"<<iEta;
          else
            name <<"SCT/EndcapC/Disk_"<<iWheel<<"/Ring_"<<iEta;
          mod->setName(name.str());

          if(!moduleSelected(mod)) {
            ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
            delete mod;
            continue;
          }

          ATH_MSG_DEBUG("Building module "<<mod->name());

          for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForDiskRing(iWheel,iEta); iPhi++) {
            ATH_MSG_DEBUG("iPhi "<<iPhi);
            for(int is=0; is<2; is++) { // module side

              const SiDetectorElement * element2 = m_detManager->getDetectorElement(iSide, iWheel, iPhi, iEta, is);
              const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;
              if (element) {
                // get element location for debugging
                // HepGeom::Point3D<double> center = element->transform() * HepGeom::Point3D<double>();
                // ATH_MSG_DEBUG(" SCTDetectorElement id: " << id << " with center = " << center);
                // ATH_MSG_DEBUG(" Is Barrel: "<< m_idHelper->is_barrel(id));

                // add element to the AlignModule
                mod->addDetElement(Trk::AlignModule::SCT,element,transform);
                // and fill the corresponding map
                (*sctIdHashMap)[element->identifyHash()] =  mod;
              }
            }
          }

          // for endcap we move the CoG
          
     
          
          Amg::Translation3D translation(mod->centerOfGravity());
          ATH_MSG_DEBUG("Endcap:" << iSide << " , disk: " << iWheel << " , CoG (" << translation.x() << " , " << translation.y() << " , " << translation.z() << " ) " );
          Amg::Transform3D localtoglobal = translation * Amg::RotationMatrix3D::Identity();
          mod->setGlobalFrameToAlignFrameTransform(localtoglobal.inverse());

          m_alignModuleListPtr->push_back(mod);
        }
      }
    }
  }

  //_______________________________________________________________________
  void SCTGeometryManagerTool::buildL2Old()
  {
    ATH_MSG_INFO("Preparing the old SCT Barrel geometry for L2: 4 barrel layers + 2 x 9 endcap disks");

    const Amg::Transform3D transform = Amg::Transform3D::Identity();

    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the SCT "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT) = new Trk::AlignModuleList((size_t)(maxHash),nullptr);
    Trk::AlignModuleList * sctIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT);

    // first endcaap C
    int iEndcapIndex = 1;
    int iSide = m_detManager->numerology().endcapId(iEndcapIndex);
    for (int iWheel = 0; iWheel < m_detManager->numerology().numDisks(); iWheel++) {
      if (!m_detManager->numerology().useDisk(iWheel))
        ATH_MSG_INFO("  Disk "<<iWheel<<" not present");

      Trk::AlignModule * mod = new Trk::AlignModule(this);
      mod->setIdHash(getNextIDHash());
      mod->setIdentifier(m_idHelper->wafer_id(iSide,iWheel,0,0,0));

      std::stringstream name;
      name <<"SCT/EndcapC/Disk_" << iWheel;
      mod->setName(name.str());

      if(!m_alignEndcaps || !moduleSelected(mod)) {
        ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
        delete mod;
        continue;
      }

      ATH_MSG_DEBUG("Building module "<<mod->name());

      for (int iEta = 0; iEta < m_detManager->numerology().numRingsForDisk(iWheel); iEta++) {
        ATH_MSG_DEBUG("iEta "<<iEta);
        for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForDiskRing(iWheel,iEta); iPhi++) {
          ATH_MSG_DEBUG("iPhi "<<iPhi);
          for(int is=0; is<2; is++) { // module side
            const SiDetectorElement * element2 = m_detManager->getDetectorElement(iSide, iWheel, iPhi, iEta, is);
            const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;
            if (element) {
              // get element location for debugging
              // HepGeom::Point3D<double> center = element->transform() * HepGeom::Point3D<double>();
              // ATH_MSG_DEBUG(" SCTDetectorElement id: " << id << " with center = " << center);
              // ATH_MSG_DEBUG(" Is Barrel: "<< m_idHelper->is_barrel(id));

              // add element to the AlignModule
              mod->addDetElement(Trk::AlignModule::SCT,element,transform);
              // and fill the corresponding map
              (*sctIdHashMap)[element->identifyHash()] =  mod;
            }
          }
        }
      }

      // for endcap we move the CoG
      Amg::Translation3D translation(mod->centerOfGravity());
      ATH_MSG_DEBUG("Endcap:" << iSide << " , disk: " << iWheel << " , CoG (" << translation.x() << " , " << translation.y() << " , " << translation.z() << " ) " );
      Amg::Transform3D localtoglobal = translation * Amg::RotationMatrix3D::Identity();
      mod->setGlobalFrameToAlignFrameTransform(localtoglobal.inverse());

      m_alignModuleListPtr->push_back(mod);
    }

    // now barrel
    for (int iLayer = 0; iLayer < m_detManager->numerology().numLayers(); iLayer++) {
      if (!m_detManager->numerology().useLayer(iLayer))
        ATH_MSG_INFO("  Layer "<<iLayer<<" not present");

      // create the AlignModule
      Trk::AlignModule * mod = new Trk::AlignModule(this);
      mod->setIdHash(getNextIDHash());
      mod->setIdentifier(m_idHelper->wafer_id(0, iLayer, 0, 0, 0));

      std::stringstream name;
      name <<"SCT/Barrel/Layer_"<<iLayer;
      mod->setName(name.str());

      if(!m_alignBarrel || !moduleSelected(mod)) {
        ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
        delete mod;
        continue;
      }

      ATH_MSG_DEBUG("Building module "<<mod->name());

      for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForLayer(iLayer); iPhi++) {
        ATH_MSG_DEBUG("iPhi "<<iPhi);
        for (int iEta = m_detManager->numerology().beginEtaModuleForLayer(iLayer); iEta < m_detManager->numerology().endEtaModuleForLayer(iLayer); iEta++) {
          ATH_MSG_DEBUG("iEta "<<iEta);
          if (!iEta && m_detManager->numerology().skipEtaZeroForLayer(iLayer))
            // iEta=0 is not defined for the SCT
            continue;
          for (int is = 0; is < 2; is++) { // module side

            const SiDetectorElement * element2 = m_detManager->getDetectorElement(0, iLayer, iPhi, iEta, is);
            const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

            if (element) {
              // get element location for debugging
              // HepGeom::Point3D<double> center = element->transform() * HepGeom::Point3D<double>();
              // ATH_MSG_DEBUG(" SCTDetectorElement id: " << id << " with center = " << center);
              // ATH_MSG_DEBUG(" Is Barrel: "<< m_idHelper->is_barrel(id));

              // add element to the AlignModule
              mod->addDetElement(Trk::AlignModule::SCT,element,transform);
              // and fill the corresponding map
              (*sctIdHashMap)[element->identifyHash()] = mod;
            }
          }
        }
      }

      m_alignModuleListPtr->push_back(mod);
    }

    // and endcaap A
    iEndcapIndex = 0;
    iSide = m_detManager->numerology().endcapId(iEndcapIndex);
    for (int iWheel = 0; iWheel < m_detManager->numerology().numDisks(); iWheel++) {
      if (!m_detManager->numerology().useDisk(iWheel))
        ATH_MSG_INFO("  Disk "<<iWheel<<" not present");

      Trk::AlignModule * mod = new Trk::AlignModule(this);
      mod->setIdHash(getNextIDHash());
      mod->setIdentifier(m_idHelper->wafer_id(iSide,iWheel,0,0,0));

      std::stringstream name;
      name <<"SCT/EndcapA/Disk_" << iWheel;
      mod->setName(name.str());

      if(!m_alignEndcaps || !moduleSelected(mod)) {
        ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
        delete mod;
        continue;
      }

      ATH_MSG_DEBUG("Building module "<<mod->name());

      for (int iEta = 0; iEta < m_detManager->numerology().numRingsForDisk(iWheel); iEta++) {
        ATH_MSG_DEBUG("iEta "<<iEta);
        for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForDiskRing(iWheel,iEta); iPhi++) {
          ATH_MSG_DEBUG("iPhi "<<iPhi);
          for(int is=0; is<2; is++) { // module side
            const SiDetectorElement * element2 = m_detManager->getDetectorElement(iSide, iWheel, iPhi, iEta, is);
            const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;
            if (element) {
              // get element location for debugging
              // HepGeom::Point3D<double> center = element->transform() * HepGeom::Point3D<double>();
              // ATH_MSG_DEBUG(" SCTDetectorElement id: " << id << " with center = " << center);
              // ATH_MSG_DEBUG(" Is Barrel: "<< m_idHelper->is_barrel(id));

              // add element to the AlignModule
              mod->addDetElement(Trk::AlignModule::SCT,element,transform);
              // and fill the corresponding map
              (*sctIdHashMap)[element->identifyHash()] =  mod;
            }
          }
        }
      }

      // for endcap we move the CoG
      Amg::Translation3D translation(mod->centerOfGravity());
      ATH_MSG_DEBUG("Endcap:" << iSide << " , disk: " << iWheel << " , CoG (" << translation.x() << " , " << translation.y() << " , " << translation.z() << " ) " );
  
      Amg::Transform3D localtoglobal = translation * Amg::RotationMatrix3D::Identity();

      mod->setGlobalFrameToAlignFrameTransform(localtoglobal.inverse());

      m_alignModuleListPtr->push_back(mod);
    }
  }

  //_______________________________________________________________________
  void SCTGeometryManagerTool::buildL3Barrel()
  {
    // ========================================
    // BARREL
    ATH_MSG_INFO("Preparing the SCT Barrel geometry for L3: module (bi-wafer) level");

    // =========================================
    // get all modules for SCT Barrel Level 3 alignment
    // 2112 modules

    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the SCT "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT) = new Trk::AlignModuleList((size_t)(maxHash),nullptr);
    Trk::AlignModuleList * sctIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT);

    for (int iLayer = 0; iLayer < m_detManager->numerology().numLayers(); iLayer++) {
      if (!m_detManager->numerology().useLayer(iLayer))
        ATH_MSG_INFO("  Layer "<<iLayer<<" not present");
      for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForLayer(iLayer); iPhi++) {
        ATH_MSG_DEBUG("iPhi "<<iPhi);
        for (int iEta = m_detManager->numerology().beginEtaModuleForLayer(iLayer); iEta < m_detManager->numerology().endEtaModuleForLayer(iLayer); iEta++) {
          ATH_MSG_DEBUG("iEta "<<iEta);
          if (!iEta && m_detManager->numerology().skipEtaZeroForLayer(iLayer))
            // iEta=0 is not defined for the SCT
            continue;

          // create the AlignModule
          Trk::AlignModule * mod = new Trk::AlignModule(this);
          mod->setIdHash(getNextIDHash());
          mod->setIdentifier(m_idHelper->wafer_id(0, iLayer, iPhi, iEta, 0));

          std::stringstream name;
          name <<"SCT/Barrel/Layer_"<<iLayer<<"/Phi_"<<iPhi<<"/Eta_"<<iEta;
          mod->setName(name.str());

          if(!m_alignBarrel || !moduleSelected(mod)) {
            ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
            delete mod;
            continue;
          }

          ATH_MSG_DEBUG("Building module "<<mod->name());

          for(int is=0;is<2;is++) { // module side

            const SiDetectorElement * element2 = m_detManager->getDetectorElement(0, iLayer, iPhi, iEta, is);
            const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

            if (element) {
              // get element location for debugging
              // HepGeom::Point3D<double> center = element->transform() * HepGeom::Point3D<double>();
              // ATH_MSG_DEBUG(" SCTDetectorElement id: " << id << " with center = " << center);
              // ATH_MSG_DEBUG(" Is Barrel: "<< m_idHelper->is_barrel(id));

              // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
              // should this be identity? if yes, it means that the
              // db frame constants are in the global frame. Or not?
              const Amg::Transform3D transform = Amg::Transform3D::Identity();
              // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

              // add element to the AlignModule
              mod->addDetElement(Trk::AlignModule::SCT,element,transform);
              // and fill the corresponding map
              (*sctIdHashMap)[element->identifyHash()] = mod;

              // the local frame for the SCT barrel modules is always that
              // of the Rphi element, NOT stereo !
              // in any case the correct frame can be obtained using moduleTransform() call
              // regardless of the side, but we only need it once so we retrieve it
              // for the non-stereo side
              if(!element2->isStereo())
                mod->setGlobalFrameToAlignFrameTransform(element2->moduleTransform().inverse());
            }
          }

          m_alignModuleListPtr->push_back(mod);
        }
      }

    }
  }

  //_______________________________________________________________________
  void SCTGeometryManagerTool::buildL3Endcaps()
  {
    // ========================================
    // ENDCAPs
    ATH_MSG_INFO("Preparing the SCT Endcap geometry for L3: module (bi-wafer) level");

    // =========================================
    // get all modules for SCT Endcap Level 3 alignment
    // 2 * 988

    unsigned int maxHash = m_idHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the SCT "<<maxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT) = new Trk::AlignModuleList((size_t)(maxHash),nullptr);
    Trk::AlignModuleList * sctIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT);

    for (int iEndcapIndex = 0; iEndcapIndex < m_detManager->numerology().numEndcaps(); iEndcapIndex++) {
      int iSide = m_detManager->numerology().endcapId(iEndcapIndex);
      for (int iWheel = 0; iWheel < m_detManager->numerology().numDisks(); iWheel++) {
        if (!m_detManager->numerology().useDisk(iWheel))
          ATH_MSG_INFO("  Disk "<<iWheel<<" not present");
        for (int iEta = 0; iEta < m_detManager->numerology().numRingsForDisk(iWheel); iEta++) {
          ATH_MSG_DEBUG("iEta "<<iEta);
          for (int iPhi = 0; iPhi < m_detManager->numerology().numPhiModulesForDiskRing(iWheel,iEta); iPhi++) {
            ATH_MSG_DEBUG("iPhi "<<iPhi);

            Trk::AlignModule * mod = new Trk::AlignModule(this);
            mod->setIdHash(getNextIDHash());
            mod->setIdentifier(m_idHelper->wafer_id(iSide,iWheel,iPhi,iEta,0));

            std::stringstream name;
            if(iEndcapIndex == 0)
              name<<"SCT/EndcapA";
            else
              name<<"SCT/EndcapC";
            name<<"/Disk_"<<iWheel<<"/Phi_"<<iPhi<<"/Eta_"<<iEta;
            mod->setName(name.str());

            if(!m_alignEndcaps || !moduleSelected(mod)) {
              ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
              delete mod;
              continue;
            }

            ATH_MSG_DEBUG("Building module "<<mod->name());

            for(int is=0; is<2; is++) { // module side

              const SiDetectorElement * element2 = m_detManager->getDetectorElement(iSide, iWheel, iPhi, iEta, is);
              const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;
              if (element) {
                // get element location for debugging
                // HepGeom::Point3D<double> center = element->transform() * HepGeom::Point3D<double>();
                // ATH_MSG_DEBUG(" SCTDetectorElement id: " << id << " with center = " << center);
                // ATH_MSG_DEBUG(" Is Barrel: "<< m_idHelper->is_barrel(id));

                // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                // should this be identity? if yes, it means that the
                // db frame constants are in the global frame. Or not?
                const Amg::Transform3D transform = Amg::Transform3D::Identity();
                // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                // add element to the AlignModule
                mod->addDetElement(Trk::AlignModule::SCT,element,transform);
                // and fill the corresponding map
                (*sctIdHashMap)[element->identifyHash()] =  mod;

                // the local frame for the SCT endcap modules is always that
                // of the Rphi element, NOT stereo !
                // in any case the correct frame can be obtained using moduleTransform() call
                // regardless of the side, but we only need it once so we retrieve it
                // for the non-stereo side
                if(!element2->isStereo())
                  mod->setGlobalFrameToAlignFrameTransform(element2->moduleTransform().inverse());
              }
            }

            m_alignModuleListPtr->push_back(mod);
          }
        }
      }
    }

    return;
  }

  //________________________________________________________________________
  void SCTGeometryManagerTool::addModuleParameters(Trk::AlignModule * module, DataVector< DataVector<Trk::AlignPar> > * allFullModPars, DataVector< DataVector<Trk::AlignPar> > * allActiveModPars)
  {
    // prepare all parameters
    DataVector<Trk::AlignPar> * fullModPars = new DataVector<Trk::AlignPar>(SG::OWN_ELEMENTS);
    fullModPars->push_back(new Trk::AlignPar(module,Trk::AlignModule::TransX));
    fullModPars->push_back(new Trk::AlignPar(module,Trk::AlignModule::TransY));
    fullModPars->push_back(new Trk::AlignPar(module,Trk::AlignModule::TransZ));
    fullModPars->push_back(new Trk::AlignPar(module,Trk::AlignModule::RotX));
    fullModPars->push_back(new Trk::AlignPar(module,Trk::AlignModule::RotY));
    fullModPars->push_back(new Trk::AlignPar(module,Trk::AlignModule::RotZ));

    // set sigmas
    setSigmas(module,fullModPars);

    // select active parameters based on jobOption properties
    DataVector<Trk::AlignPar> * activeModPars = new DataVector<Trk::AlignPar>(SG::VIEW_ELEMENTS);
    for(unsigned int ipar=0;ipar<fullModPars->size();++ipar) {

      Identifier AlimodID = module->identify();
      Identifier modID = ((module->detElementCollection(Trk::AlignModule::SCT))->at(0))->identify();

      if(m_idHelper->is_barrel(modID)) {
        ATH_MSG_DEBUG("SCT barrel module with id "<<AlimodID);
        if(  (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransX && m_alignBarrelX)
          || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransY && m_alignBarrelY)
          || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransZ && m_alignBarrelZ)
          || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotX   && m_alignBarrelRotX)
          || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotY   && m_alignBarrelRotY)
          || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotZ   && m_alignBarrelRotZ) ) {
          ATH_MSG_DEBUG("parameter type "<<fullModPars->at(ipar)->paramType()<<" is now active");
          activeModPars->push_back(fullModPars->at(ipar));
        }
        else
          ATH_MSG_DEBUG("parameter type "<<fullModPars->at(ipar)->paramType()<<" is NOT active");
      }
      else {
        ATH_MSG_DEBUG("SCT endcap module with id "<<AlimodID);
        if(  (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransX && m_alignEndcapX)
          || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransY && m_alignEndcapY)
          || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransZ && m_alignEndcapZ)
          || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotX   && m_alignEndcapRotX)
          || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotY   && m_alignEndcapRotY)
          || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotZ   && m_alignEndcapRotZ) ) {
          ATH_MSG_DEBUG("parameter type "<<fullModPars->at(ipar)->paramType()<<" is now active");
          activeModPars->push_back(fullModPars->at(ipar));
        }
        else
          ATH_MSG_DEBUG("parameter type "<<fullModPars->at(ipar)->paramType()<<" is NOT active");
      }
    }

    // now add parameters to the list
    allFullModPars->push_back(fullModPars);
    allActiveModPars->push_back(activeModPars);
  }

  //________________________________________________________________________
  void SCTGeometryManagerTool::setSigmas(Trk::AlignModule * module, DataVector<Trk::AlignPar> * modPars)
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
  bool SCTGeometryManagerTool::moduleSelected(unsigned long long id)
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
  bool SCTGeometryManagerTool::moduleSelected(Trk::AlignModule * mod)
  {
    return moduleSelected(mod->identify().get_compact());
  }

  //________________________________________________________________________
  void SCTGeometryManagerTool::dumpGeometry()
  {
    ATH_MSG_INFO("---------------------------------------------------");
    ATH_MSG_INFO("Summary of the alignment geometry");
    ATH_MSG_INFO("Number of alignable objects: "<< m_alignModuleList.size());
    for(unsigned int i=0;i<m_alignModuleList.size();i++) {
      const Trk::AlignModule* module = m_alignModuleList.at(i);
      ATH_MSG_INFO(i<<". "<< module->name());
      ATH_MSG_INFO("   - identifier: "<<module->identify());
      ATH_MSG_INFO("   - has "<<module->detElementCollection(Trk::AlignModule::SCT)->size()<<" SCT modules");

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

} // end namespace
