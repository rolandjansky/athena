/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "AthContainers/DataVector.h"

#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"
#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetIdentifier/TRT_ID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SiliconID.h"

#include "TrkAlignInterfaces/IAlignModuleTool.h"

#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignEvent/AlignPar.h"

#include "InDetAlignGeomTools/InDetGeometryManagerTool.h"
#include <ostream>
#include <algorithm>



using namespace InDetDD;

namespace InDet {

  //________________________________________________________________________
  InDetGeometryManagerTool::InDetGeometryManagerTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface * parent)
    : AthAlgTool(type,name,parent)
    , m_pixelDetManager(nullptr)
    , m_sctDetManager(nullptr)
    , m_trtDetManager(nullptr)
    , m_pixHelper()
    , m_sctHelper()
    , m_siHelper()
    , m_trtHelper()
    , m_siGeoManager("")
    , m_trtGeoManager("")
    , m_alignModuleTool("Trk::AlignModuleTool/AlignModuleTool")
    , m_idHashToAlignModuleMaps(Trk::AlignModule::NDetectorTypes,(Trk::AlignModuleList*)nullptr)
    , m_alignParList(nullptr)
    , m_fullAlignParList(nullptr)
  {
    declareInterface<IGeometryManagerTool>(this);
    declareProperty("AlignModuleTool",    m_alignModuleTool);

    declareProperty("SiGeometryManager",  m_siGeoManager);
    declareProperty("TRTGeometryManager", m_trtGeoManager);

    // Inner Detector joboptions
    declareProperty("AlignSilicon",       m_alignSi           = true);
    declareProperty("AlignTRT",           m_alignTRT          = true);

    declareProperty("AlignX",             m_alignX            = true);
    declareProperty("AlignY",             m_alignY            = true);
    declareProperty("AlignZ",             m_alignZ            = true);
    declareProperty("AlignRotX",          m_alignRotX         = true);
    declareProperty("AlignRotY",          m_alignRotY         = true);
    declareProperty("AlignRotZ",          m_alignRotZ         = true);
   
    declareProperty("SetSigmaX",          m_sigmaX            = 1.);
    declareProperty("SetSigmaY",          m_sigmaY            = 1.);
    declareProperty("SetSigmaZ",          m_sigmaZ            = 1.);
    declareProperty("SetSigmaRotX",       m_sigmaRotX         = 0.001);
    declareProperty("SetSigmaRotY",       m_sigmaRotY         = 0.001);
    declareProperty("SetSigmaRotZ",       m_sigmaRotZ         = 0.001);

    // defines alignment level
    declareProperty("AlignmentLevel",     m_alignLevel        = -1);

    declareProperty("doModuleSelection",  m_doModuleSelection = false);
    declareProperty("ModuleSelection",    m_moduleSelection);

    declareProperty("DumpGeometry",       m_dumpGeometry = true);

    // the setarate barrel and endcaps levels are not used here
    m_alignLevelBarrel  = -1;
    m_alignLevelEndcaps = -1;

    m_hashCounter = 0;
    m_logStream = nullptr;
  }

  //________________________________________________________________________
  InDetGeometryManagerTool::~InDetGeometryManagerTool() 
  {
    ATH_MSG_DEBUG("deleting alignModuleList");
    for (const auto & i:m_alignModuleList){
      delete i;
    }
    m_alignModuleList.clear();

    ATH_MSG_DEBUG("deleting fullAlignParList");
    delete m_fullAlignParList;
    ATH_MSG_DEBUG("deleting alignParList");
    delete m_alignParList;
  }

  //________________________________________________________________________
  StatusCode InDetGeometryManagerTool::initialize() 
  {
    ATH_MSG_DEBUG("initialize() of InDetGeometryManagerTool");

    // retrieve AlignModuleTool
    ATH_CHECK( m_alignModuleTool.retrieve() );

    // retrieve Pixel helper
    ATH_CHECK( detStore()->retrieve(m_pixHelper) );

    // retrieve SCT helper
    ATH_CHECK( detStore()->retrieve(m_sctHelper) );

    // retrieve silicon helper
    ATH_CHECK( detStore()->retrieve(m_siHelper) );
      
    // retrieve TRT helper
    ATH_CHECK( detStore()->retrieve(m_trtHelper) );

    // retrieve PIX detector manager
    ATH_CHECK ( detStore()->retrieve(m_pixelDetManager, "Pixel"));

    // retrieve SCT detector manager
    ATH_CHECK( detStore()->retrieve(m_sctDetManager, "SCT"));

    // retrieve SCT detector manager
    ATH_CHECK ( detStore()->retrieve(m_trtDetManager, "TRT") );

    // dump module selection
    if(m_doModuleSelection && msgLvl(MSG::INFO)) {
      unsigned int idx{0};
      ATH_MSG_INFO("Creating geometry for selected "<<m_moduleSelection.size()<<" modules:");
      for(const auto & i:m_moduleSelection)
        ATH_MSG_INFO("   "<<idx++<<".  "<<i);
    }

    // retrieve SiGeometryManagerTool
    if ( !m_siGeoManager.empty() ) {
      ATH_CHECK ( m_siGeoManager.retrieve() );
    }

    // retrieve TRTGeometryManagerTool
    if ( !m_trtGeoManager.empty() ) {
      ATH_CHECK ( m_trtGeoManager.retrieve() );
    }

    if(!m_alignSi && !m_alignTRT && m_alignLevel!=0) {
      ATH_MSG_FATAL("Alignment of both Silicon and TRT turned off. Aborting.");
      return StatusCode::FAILURE;
    }

    // check allowed alignment level
    if(!checkAlignLevel()) {
      ATH_MSG_FATAL("Wrong alignment level.");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  StatusCode InDetGeometryManagerTool::finalize() 
  {  
    ATH_MSG_DEBUG("finalize() of InDetGeometryManagerTool");

    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  bool InDetGeometryManagerTool::checkAlignLevel()
  {
    switch(m_alignLevel) {

      case 0:
        // for L0 we don't need the other two managers so everything is ok
        ATH_MSG_INFO("Setting up level 0 alignment of the InnerDetector");
        break;

      case 1: case 2: case 3:
        // for levels 1,2,3 we need the managers and we have to
        // set the levels in them
        if( (m_siGeoManager.empty() && m_alignSi) || (m_trtGeoManager.empty() && m_alignTRT) ) {
          ATH_MSG_ERROR("SiliconGeometryManagerTool and/or TRTGeometryManagerTool not available");
          ATH_MSG_ERROR("Can't set alignment geometry. ");
          return false;
        }

        ATH_MSG_INFO("Setting up level "<<m_alignLevel<<" alignment of the Inner Detector");
        m_siGeoManager->setAlignLevel(m_alignLevel);
        m_trtGeoManager->setAlignLevel(m_alignLevel);

        // we also check that the managers support the levels
        if( (m_alignSi && !m_siGeoManager->checkAlignLevel()) || (m_alignTRT && !m_trtGeoManager->checkAlignLevel()) )
          return false;
        break;

      case -1:
        // if level is not set here (=-1) we need the Silicon and TRT
        // managers but we trust their setup, we don't need to check it
        if( (m_siGeoManager.empty() && m_alignSi) || (m_trtGeoManager.empty() && m_alignTRT) ) {
          ATH_MSG_ERROR("SiGeometryManagerTool and/or TRTGeometryManagerTool not available");
          ATH_MSG_ERROR("Can't set alignment geometry. ");
          return false;
        }
        break;

      default:
        ATH_MSG_ERROR("Unknown alignment level "<<m_alignLevel<<". Can't set alignment geometry.");
        return false;

    }

    return true;
  }

  //________________________________________________________________________
  void InDetGeometryManagerTool::setLogStream(std::ostream * os)
  {
    m_logStream = os;
    if (!m_siGeoManager.empty())
      m_siGeoManager->setLogStream(m_logStream);
    if (!m_trtGeoManager.empty())
      m_trtGeoManager->setLogStream(m_logStream);
  }

  //________________________________________________________________________
  int InDetGeometryManagerTool::ReadGeometry(int solveLevel)
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
    for( const auto & imod:m_alignModuleList) {
      ATH_MSG_DEBUG("Module "<<imod->name());
      addModuleParameters(imod,m_fullAlignParList,m_alignParList);
    }

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
  void InDetGeometryManagerTool::buildGeometry()
  {
    ATH_MSG_INFO("Preparing the Inner Detector geometry");

    int idHash = 0;

    if(m_alignLevel==0)
      buildL0();

    else {

      // Silicon
      if(m_alignSi) {
        m_siGeoManager->setFirstIDHash(idHash);
        m_siGeoManager->setModuleList(m_alignModuleListPtr);
        m_siGeoManager->setHashMaps(m_idHashToAlignModuleMapsPtr);

        m_siGeoManager->buildGeometry();

        idHash=m_siGeoManager->getNextIDHash();
      }

      // TRT
      if(m_alignTRT) {
        m_trtGeoManager->setFirstIDHash(idHash);
        m_trtGeoManager->setModuleList(m_alignModuleListPtr);
        m_trtGeoManager->setHashMaps(m_idHashToAlignModuleMapsPtr);

        m_trtGeoManager->buildGeometry();
      }
    }

    return;
  }

  //_______________________________________________________________________
  void InDetGeometryManagerTool::buildL0()
  {
    ATH_MSG_INFO("Preparing the Silicon geometry for L0");
    // ========================================
    // Level 0 is just one module containing
    // the whole Inner Detector (Pixel+SCT+TRT)

    Trk::AlignModule * indet = new Trk::AlignModule(this);
    indet->setIdHash(getNextIDHash());
    indet->setName("Inner Detector");

    // use the identifier of the Pixel for the whole Inner Detector at L0
    indet->setIdentifier(m_pixHelper->wafer_id(0,0,0,0));

    // check if we're aligning Silicon
    if(!moduleSelected(indet)) {
      ATH_MSG_DEBUG("Module "<<indet->name()<<" NOT selected");
      delete indet;
      return;
    }

    ATH_MSG_DEBUG("Created module "<<indet->name()<<"  idHash: "<<indet->identifyHash()<<"  identifier: "<<indet->identify());

    // for L0 alignment the alignment frame is equal to the global frame
    // and since the PIXEL and SCT detector element positions are also stored
    // in the global frame in DB, transform is identity
    const Amg::Transform3D transform = Amg::Transform3D::Identity();


    // PIXEL
    // get maximum number of elements from the helper
    unsigned int pixelmaxHash = m_pixHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the Pixel: "<<pixelmaxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel) = new Trk::AlignModuleList((size_t)(pixelmaxHash),nullptr);
    Trk::AlignModuleList * pixelIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::Pixel);

    // ==================================================================
    // loop over Pixel elements and add them to respective alignModules
    // ==================================================================
    for (unsigned int index = 0; index < pixelmaxHash; index++) {
      IdentifierHash idHash = index;
      Identifier id = m_pixHelper->wafer_id(idHash);

      ATH_MSG_DEBUG(" Pixel DetectorElement idhash: "<<index);
      ATH_MSG_DEBUG(" DetectorElement id: "<<id);

      // get the element via hash
      SiDetectorElement * element2 = m_pixelDetManager->getDetectorElement(id);
      if (element2) {
        const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

        // get element location for debugging
        // HepGeom::Point3D<double> center = element->transform() * HepGeom::Point3D<double>();
        // ATH_MSG_DEBUG(" DetectorElement idhash: " << index);
        // ATH_MSG_DEBUG(" DetectorElement id: " << id << " with center = " << center);
        // ATH_MSG_DEBUG(" Is Barrel: "<< m_pixHelper->is_barrel(id));      

        // add element to respective AlignModule

        // add to the pixel structure
        if(msgLvl(MSG::DEBUG)) {
          if (m_pixHelper->is_barrel(id))
            msg(MSG::DEBUG)<<"... Pixel barrel element"<<endmsg;
          else
            msg(MSG::DEBUG)<<"... Pixel endcap element"<<endmsg;
        }
        indet->addDetElement(Trk::AlignModule::Pixel,element,transform);

        // and fill the corresponding map
        (*pixelIdHashMap)[idHash] = indet;
      }
      else
        ATH_MSG_DEBUG("No Pixel detector with id: "<<id);
    }

    // SCT
    // get maximum number of elements from the helper
    unsigned int sctmaxHash = m_sctHelper->wafer_hash_max();
    ATH_MSG_DEBUG("maxHash for the SCT: "<<sctmaxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT) = new Trk::AlignModuleList((size_t)(sctmaxHash),nullptr);
    Trk::AlignModuleList * sctIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::SCT);

    // ================================================================
    // loop over SCT elements and add them to respective alignModules
    // ================================================================
    for (unsigned int index = 0; index < sctmaxHash; index++) {
      IdentifierHash idHash = index;
      Identifier id = m_sctHelper->wafer_id(idHash);

      ATH_MSG_DEBUG(" SCT DetectorElement idhash: "<<index);
      ATH_MSG_DEBUG(" DetectorElement id: "<<id);

      // get the element via hash
      SiDetectorElement * element2 = m_sctDetManager->getDetectorElement(id);
      if (element2) {
        const Trk::TrkDetElementBase * element = (const Trk::TrkDetElementBase*) element2;

        // add element to respective AlignModule

        // add to the sct barrel structure
        if(msgLvl(MSG::DEBUG)) {
          if (m_sctHelper->is_barrel(id))
            msg(MSG::DEBUG)<<"... SCT barrel element"<<endmsg;
          else
            msg(MSG::DEBUG)<<"... SCT endcap element"<<endmsg;
        }
        indet->addDetElement(Trk::AlignModule::SCT,element,transform);

        // and fill the corresponding map
        (*sctIdHashMap)[idHash] = indet;
      }
      else
        ATH_MSG_DEBUG("No SCT detector with id: "<<id);
    }


    // TRT
    // get maximum number of elements from the helper
    unsigned int trtmaxHash = m_trtHelper->straw_layer_hash_max();
    ATH_MSG_DEBUG("maxHash for the TRT "<<trtmaxHash);

    if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT) = new Trk::AlignModuleList((size_t)(trtmaxHash),nullptr);
    Trk::AlignModuleList * trtIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT);

    // ================================================================
    // loop over TRT elements and add them to respective alignModules
    // ================================================================
    for (unsigned int index = 0; index < trtmaxHash; index++) {
      IdentifierHash idHash = index;
      Identifier id = m_trtHelper->layer_id(idHash);

      // get the element via hash
      const TRT_BaseElement * element = m_trtDetManager->getElement(idHash);
      if (element) {
         // get element location for debugging
//         HepGeom::Point3D<double> center = element->transform() * HepGeom::Point3D<double>();
//         ATH_MSG_DEBUG(" TRTDetectorElement id: " << id << " with center = " << center);

        // add element to respective AlignModule

        // add to the Inner Detector structure
        if(msgLvl(MSG::DEBUG)) {
          if (m_trtHelper->is_barrel(id))
            msg(MSG::DEBUG)<<"... TRT barrel element"<<endmsg;
          else
            msg(MSG::DEBUG)<<"... TRT endcap element"<<endmsg;
        }
        indet->addDetElement(Trk::AlignModule::TRT,element,transform);

        // and fill the corresponding map
        (*trtIdHashMap)[idHash] = indet;
      }
      else
         ATH_MSG_DEBUG("No TRTDetectorElement with id: "<<id);
    }

    // add created module to the geometry
    m_alignModuleListPtr->push_back(indet);

    ATH_MSG_DEBUG("Inner Detector L0 module successfully added to the list");
  }

  //________________________________________________________________________
  void InDetGeometryManagerTool::addModuleParameters(Trk::AlignModule * module, DataVector< DataVector<Trk::AlignPar> > * allFullModPars, DataVector< DataVector<Trk::AlignPar> > * allActiveModPars)
  {
    // for standalone Silicon and TRT modules call the respective methods
    if(isSiOnly(module)) {
      m_siGeoManager->addModuleParameters(module,allFullModPars,allActiveModPars);
      return;
    }
    else if(isOneDetOnly(module,Trk::AlignModule::TRT)) {
      m_trtGeoManager->addModuleParameters(module,allFullModPars,allActiveModPars);
      return;
    }

    // for combined modules we do the work here

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

      ATH_MSG_DEBUG("Inner Detector module with id "<<AlimodID);
      if(  (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransX && m_alignX)
        || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransY && m_alignY)
        || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransZ && m_alignZ)
        || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotX   && m_alignRotX)
        || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotY   && m_alignRotY)
        || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotZ   && m_alignRotZ) ) {
        ATH_MSG_DEBUG("parameter type "<<fullModPars->at(ipar)->paramType()<<" is now active");
        activeModPars->push_back(fullModPars->at(ipar));
      }
      else
        ATH_MSG_DEBUG("parameter type "<<fullModPars->at(ipar)->paramType()<<" is NOT active");
    }

    // now add parameters to the list
    allFullModPars->push_back(fullModPars);
    allActiveModPars->push_back(activeModPars);
  }

  //________________________________________________________________________
  void InDetGeometryManagerTool::setSigmas(Trk::AlignModule * module, DataVector<Trk::AlignPar> * modPars)
  {
    ATH_MSG_DEBUG("Setting sigmas for module: "<<module->name());
    for(unsigned int ipar=0;ipar<modPars->size();++ipar)
      switch(modPars->at(ipar)->paramType()) {
        case Trk::AlignModule::TransX:
          modPars->at(ipar)->setSigma(m_sigmaX);
          break;
        case Trk::AlignModule::TransY:
          modPars->at(ipar)->setSigma(m_sigmaY);
          break;
        case Trk::AlignModule::TransZ:
          modPars->at(ipar)->setSigma(m_sigmaZ);
          break;
        case Trk::AlignModule::RotX:
          modPars->at(ipar)->setSigma(m_sigmaRotX);
          break;
        case Trk::AlignModule::RotY:
          modPars->at(ipar)->setSigma(m_sigmaRotY);
          break;
        case Trk::AlignModule::RotZ:
          modPars->at(ipar)->setSigma(m_sigmaRotZ);
          break;
        default:
          break;
      }
  }

  //________________________________________________________________________
  bool InDetGeometryManagerTool::moduleSelected(unsigned long long id)
  {
    if(!m_doModuleSelection) return true;
    const bool found = (std::find(m_moduleSelection.begin(), m_moduleSelection.end(),id) != m_moduleSelection.end());
    return found;
  }

  //________________________________________________________________________
  bool InDetGeometryManagerTool::moduleSelected(Trk::AlignModule * mod)
  {
    return moduleSelected(mod->identify().get_compact());
  }

  //________________________________________________________________________
  void InDetGeometryManagerTool::dumpGeometry()
  {
    ATH_MSG_INFO("---------------------------------------------------");
    ATH_MSG_INFO("Summary of the alignment geometry");
    ATH_MSG_INFO("Number of alignable objects: "<< m_alignModuleList.size());
    for(unsigned int i=0;i<m_alignModuleList.size();i++) {
      const Trk::AlignModule* module = m_alignModuleList.at(i);
      ATH_MSG_INFO(i<<". "<< module->name());
      ATH_MSG_INFO("   - identifier:     "<<module->identify());
      ATH_MSG_INFO("   - identifierHash: "<<module->identifyHash());
      int npix(0);
      int nsct(0);
      int ntrt(0);
      if(module->detElementCollection(Trk::AlignModule::Pixel)) {
        npix = module->detElementCollection(Trk::AlignModule::Pixel)->size();
        ATH_MSG_INFO("   - has "<<npix<<" Pixel modules");
      }
      if(module->detElementCollection(Trk::AlignModule::SCT)) {
        nsct = module->detElementCollection(Trk::AlignModule::SCT)->size();
        ATH_MSG_INFO("   - has "<<nsct<<" SCT modules");
      }
      if(module->detElementCollection(Trk::AlignModule::TRT)) {
        ntrt = module->detElementCollection(Trk::AlignModule::TRT)->size();
        ATH_MSG_INFO("   - has "<<ntrt<<" TRT modules");
      }
      if(npix && nsct)
        ATH_MSG_INFO("   - has "<<npix+nsct<<" Silicon modules in total");
      if((npix || nsct) && ntrt)
        ATH_MSG_INFO("   - has "<<npix+nsct+ntrt<<" Inner Detector modules in total");

      Amg::Transform3D localtoglobal = (module->globalFrameToAlignFrame()).inverse();
      ATH_MSG_DEBUG("   - local to global : "<<std::setprecision(12)<<localtoglobal.translation()<<" "<<localtoglobal.rotation());

      DataVector<Trk::AlignPar> * pars = m_alignModuleTool->getAlignPars(module);
      int npars = pars->size();
      ATH_MSG_DEBUG("   - number of active transform parameters: "<<npars);
      for(int j=0;j<npars;j++)
         ATH_MSG_DEBUG("      * par "<<j<<": sigma = "<<(*pars)[j]->sigma());
    }
    ATH_MSG_INFO("---------------------------------------------------");
  }

  //________________________________________________________________________
  bool InDetGeometryManagerTool::isOneDetOnly(const Trk::AlignModule * mod, Trk::AlignModule::DetectorType dettype) const
  {
    ATH_MSG_DEBUG("in isOneDetOnly for detector type "<<dettype);
    const Trk::AlignModule::DetElementCollection * coll = mod->detElementCollection(dettype);
    if(!coll || coll->size() == 0)
      return false;

    int nelem(0);
    for(int i=1;i<Trk::AlignModule::NDetectorTypes;i++) {
      if(i==dettype)
        continue;
      coll = mod->detElementCollection((Trk::AlignModule::DetectorType)i);
      if(coll)
        nelem += coll->size();
    }

    if(nelem)
      return false;

    ATH_MSG_DEBUG("module IS of type "<<dettype);
    return true;
  }

  //________________________________________________________________________
  bool InDetGeometryManagerTool::isSiOnly(const Trk::AlignModule * mod) const
  {
    ATH_MSG_DEBUG("in isSiOnly");
    const Trk::AlignModule::DetElementCollection * collPix = mod->detElementCollection(Trk::AlignModule::Pixel);
    const Trk::AlignModule::DetElementCollection * collSCT = mod->detElementCollection(Trk::AlignModule::SCT);
    if((!collPix || collPix->size()==0) && (!collSCT || collSCT->size()==0))
      return false;

    int nelem(0);
    for(int i=1;i<Trk::AlignModule::NDetectorTypes;i++) {
      if(i==Trk::AlignModule::Pixel || i==Trk::AlignModule::SCT)
        continue;
      const Trk::AlignModule::DetElementCollection * coll = mod->detElementCollection((Trk::AlignModule::DetectorType)i);
      if(coll)
        nelem += coll->size();
    }

    if(nelem)
      return false;

    ATH_MSG_DEBUG("module IS of type Silicon");
    return true;
  }


} // end namespace
