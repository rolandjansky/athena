/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "AthContainers/DataVector.h"

#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SiliconID.h"

#include "TrkAlignInterfaces/IAlignModuleTool.h"

#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignEvent/AlignPar.h"

#include "InDetAlignGeomTools/SiGeometryManagerTool.h"
#include <ostream>


using namespace InDetDD;

namespace InDet {

  //________________________________________________________________________
  SiGeometryManagerTool::SiGeometryManagerTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface * parent)
    : AthAlgTool(type,name,parent)
    , m_pixelDetManager(nullptr)
    , m_sctDetManager(nullptr)
    , m_pixHelper()
    , m_sctHelper()
    , m_idHelper()
    , m_pixelGeoManager("")
    , m_sctGeoManager("")
    , m_alignModuleTool("Trk::AlignModuleTool/AlignModuleTool")
    , m_idHashToAlignModuleMaps(Trk::AlignModule::NDetectorTypes,(Trk::AlignModuleList*)nullptr)
    , m_alignParList(nullptr)
    , m_fullAlignParList(nullptr)
  {
    declareInterface<IGeometryManagerTool>(this);
    declareProperty("AlignModuleTool",      m_alignModuleTool);

    declareProperty("PixelGeometryManager", m_pixelGeoManager);
    declareProperty("SCTGeometryManager",   m_sctGeoManager);

    // Silicon joboptions
    declareProperty("AlignPixel",         m_alignPixel        = true);
    declareProperty("AlignSCT",           m_alignSCT          = true);

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
    declareProperty("ActualGeometry",     m_actualGeom = false);

    m_hashCounter = 0;
    m_logStream = nullptr;
  }

  //________________________________________________________________________
  SiGeometryManagerTool::~SiGeometryManagerTool() 
  {
    ATH_MSG_DEBUG("deleting alignModuleList");
    for (const auto & i:m_alignModuleList) delete i;
    m_alignModuleList.clear();

    ATH_MSG_DEBUG("deleting fullAlignParList");
    delete m_fullAlignParList;
    ATH_MSG_DEBUG("deleting alignParList");
    delete m_alignParList;
  }

  //________________________________________________________________________
  StatusCode SiGeometryManagerTool::initialize() 
  {
    ATH_MSG_DEBUG("initialize() of SiGeometryManagerTool");

    // retrieve AlignModuleTool
    ATH_CHECK( m_alignModuleTool.retrieve() );

    // retrieve Pixel helper
    ATH_CHECK( detStore()->retrieve(m_pixHelper) );

    // retrieve SCT helper
    ATH_CHECK( detStore()->retrieve(m_sctHelper));

    // retrieve silicon helper
    ATH_CHECK( detStore()->retrieve(m_idHelper) );
      
    // retrieve SCT detector manager
    ATH_CHECK( detStore()->retrieve(m_sctDetManager, "SCT") );

    // retrieve PIX detector manager
    ATH_CHECK( detStore()->retrieve(m_pixelDetManager, "Pixel") );

    // dump module selection
    if(m_doModuleSelection && msgLvl(MSG::INFO)) {
      int idx{};
      ATH_MSG_INFO("Creating geometry for selected "<<m_moduleSelection.size()<<" modules:");
      for(const auto & mod:m_moduleSelection)
        ATH_MSG_INFO("   "<<idx++<<".  "<<mod);
    }

    // retrieve PixelGeometryManagerTool
    if ( !m_pixelGeoManager.empty() ) {
      ATH_CHECK( m_pixelGeoManager.retrieve() );
    }

    // retrieve SCTGeometryManagerTool
    if ( !m_sctGeoManager.empty() ) {
      ATH_CHECK( m_sctGeoManager.retrieve() );
    }

    if(!m_alignPixel && !m_alignSCT) {
      ATH_MSG_FATAL("Alignment of both Pixel and SCT turned off. Aborting.");
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
  StatusCode SiGeometryManagerTool::finalize() 
  {  
    ATH_MSG_DEBUG("finalize() of SiGeometryManagerTool");

    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  bool SiGeometryManagerTool::checkAlignLevel()
  {
    switch(m_alignLevel) {

      case 0:
        // for L0 we don't need the other two managers so everything is ok
        ATH_MSG_INFO("Setting up level 0 alignment of the Silicon");
        break;

      case 1: case 2: case 3:
        // for levels 1,2,3 we need the managers and we have to
        // set the levels in them
        if( (m_pixelGeoManager.empty() && m_alignPixel) || (m_sctGeoManager.empty() && m_alignSCT) ) {
          ATH_MSG_ERROR("PixelGeometryManagerTool and/or SCTGeometryManagerTool not available");
          ATH_MSG_ERROR("Can't set alignment geometry.");
          return false;
        }

        ATH_MSG_INFO("Setting up level "<<m_alignLevel<<" alignment of the Silicon");
        m_pixelGeoManager->setAlignLevel(m_alignLevel);
        m_sctGeoManager->setAlignLevel(m_alignLevel);

        // we also check that the managers support the levels
        if( (m_alignPixel && !m_pixelGeoManager->checkAlignLevel()) || (m_alignSCT && !m_sctGeoManager->checkAlignLevel()) )
          return false;
        break;

      case -1:
        // if level is not set here (i.e. it is equal to -1) we need the Pixel and SCT
        // managers but we trust their setup, we don't need to check it
        if( (m_pixelGeoManager.empty() && m_alignPixel) || (m_sctGeoManager.empty() && m_alignSCT) ) {
          ATH_MSG_ERROR("PixelGeometryManagerTool and/or SCTGeometryManagerTool not available");
          ATH_MSG_ERROR("Can't set alignment geometry.");
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
  void SiGeometryManagerTool::setLogStream(std::ostream * os)
  {
    m_logStream = os;
    if (!m_pixelGeoManager.empty())
      m_pixelGeoManager->setLogStream(m_logStream);
    if (!m_sctGeoManager.empty())
      m_sctGeoManager->setLogStream(m_logStream);
  }

  //________________________________________________________________________
  int SiGeometryManagerTool::ReadGeometry(int solveLevel)
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

    // dump summary about the geometry setup
    if (m_dumpGeometry)
      dumpGeometry();

    int nDoF= m_alignModuleTool->nAlignParameters();
    ATH_MSG_INFO("Total number of degrees of freedom: "<<nDoF);

    return nDoF;
  }

  //_______________________________________________________________________
  void SiGeometryManagerTool::buildGeometry()
  {
    ATH_MSG_INFO("Preparing the Silicon geometry");

    int idHash = 0;

    if(m_alignLevel==0)
      buildL0();

    else {

      // PIXEL
      if(m_alignPixel) {
        m_pixelGeoManager->setFirstIDHash(idHash);
        m_pixelGeoManager->setModuleList(m_alignModuleListPtr);
        m_pixelGeoManager->setHashMaps(m_idHashToAlignModuleMapsPtr);

        m_pixelGeoManager->buildGeometry();

        idHash=m_pixelGeoManager->getNextIDHash();
      }

      // SCT
      if(m_alignSCT) {
        m_sctGeoManager->setFirstIDHash(idHash);
        m_sctGeoManager->setModuleList(m_alignModuleListPtr);
        m_sctGeoManager->setHashMaps(m_idHashToAlignModuleMapsPtr);

        m_sctGeoManager->buildGeometry();
      }
    }

    return;
  }

  //_______________________________________________________________________
  void SiGeometryManagerTool::buildL0()
  {
    ATH_MSG_INFO("Preparing the Silicon geometry for L0");
    // ========================================
    // Level 0 is just one module containing
    // the whole Silicon detector (Pixel+SCT)

    Trk::AlignModule * silicon = new Trk::AlignModule(this);
    silicon->setIdHash(getNextIDHash());
    silicon->setName("Silicon");

    // use the identifier of the Pixel for the whole Silicon at L0
    silicon->setIdentifier(m_pixHelper->wafer_id(0,0,0,0));

    // check if we're aligning Silicon
    if(!moduleSelected(silicon)) {
      ATH_MSG_DEBUG("Module "<<silicon->name()<<" NOT selected");
      delete silicon;
      return;
    }

    ATH_MSG_DEBUG("Created module "<<silicon->name()<<"  idHash: "<<silicon->identifyHash()<<"  identifier: "<<silicon->identify());

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
        silicon->addDetElement(Trk::AlignModule::Pixel,element,transform);

        // and fill the corresponding map
        (*pixelIdHashMap)[idHash] = silicon;
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
        silicon->addDetElement(Trk::AlignModule::SCT,element,transform);

        // and fill the corresponding map
        (*sctIdHashMap)[idHash] = silicon;
      }
      else
        ATH_MSG_DEBUG("No SCT detector with id: "<<id);
    }

    // add created module to the geometry
    m_alignModuleListPtr->push_back(silicon);

    ATH_MSG_DEBUG("Silicon L0 module successfully added to the list");
  }

  //________________________________________________________________________
  void SiGeometryManagerTool::addModuleParameters(Trk::AlignModule * module, DataVector< DataVector<Trk::AlignPar> > * allFullModPars, DataVector< DataVector<Trk::AlignPar> > * allActiveModPars)
  {
    // for standalone Pixel and SCT modules call the respective methods
    if(isOneDetOnly(module,Trk::AlignModule::Pixel)) {
      m_pixelGeoManager->addModuleParameters(module,allFullModPars,allActiveModPars);
      return;
    }
    else if(isOneDetOnly(module,Trk::AlignModule::SCT)) {
      m_sctGeoManager->addModuleParameters(module,allFullModPars,allActiveModPars);
      return;
    }

    // for combined modules do the work here

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

      ATH_MSG_DEBUG("Silicon module with id "<<AlimodID);
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
  bool SiGeometryManagerTool::moduleSelected(unsigned long long id)
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
  void SiGeometryManagerTool::setSigmas(Trk::AlignModule * module, DataVector<Trk::AlignPar> * modPars)
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
  bool SiGeometryManagerTool::moduleSelected(Trk::AlignModule * mod)
  {
    return moduleSelected(mod->identify().get_compact());
  }

  //________________________________________________________________________
  void SiGeometryManagerTool::dumpGeometry()
  {

    ATH_MSG_INFO("---------------------------------------------------");
    ATH_MSG_INFO("Try to write an Si geom root file:");
    TGeoManager*  gm=new  TGeoManager("Silicon","Silicon");
    TGeoMaterial* mat=new TGeoMaterial("Vacuum",0,0,0);
    TGeoMedium*   med=new TGeoMedium("Vacuum",1,mat);
    TGeoVolume*   top = gm->MakeBox("Silicon",med,2000.,2000.,10000.);
    gm->SetTopVolume(top);
    TGeoVolume*   Si_cog[22000];
    TGeoVolume*   Si[22000];
    int           Si_count=0; 
    TGeoTranslation* tr[22000];
    TGeoRotation*    ro[22000];
    TGeoCombiTrans*  mx[22000];

    TGeoTranslation* nulltrans=new TGeoTranslation(0.0,0.0,0.0);
    TGeoRotation*    nullrota=new TGeoRotation();
    nullrota->SetAngles(0.0,0.0,0.0);    // Euler angles                                                     
    TGeoRotation*    fliprota=new TGeoRotation();
    fliprota->SetAngles(0.0,-90.0,0.0);    // Euler angles (rotation around X)
    TGeoCombiTrans*  donothing=new TGeoCombiTrans(*nulltrans,*nullrota);
    TGeoCombiTrans*  swapaxes=new TGeoCombiTrans(*nulltrans,*fliprota);

    Amg::Vector3D ea;
    Amg::Vector3D xyz;

    // First prepare the higher level (L1, L2) structures:
    TGeoVolume*   L0_A = gm->MakeTube("L0_A",med,0.0,1100.0,5000.0);
    L0_A->SetVisibility(kFALSE);
    top->AddNodeOverlap(L0_A,Si_count,donothing);
    Si_count++;
    TGeoVolume*   L1_IBL_A = gm->MakeTube("L1_IBL_A",med,20.0,40.0,700.0);
    L1_IBL_A->SetVisibility(kFALSE);
    L0_A->AddNodeOverlap(L1_IBL_A,Si_count,donothing);
    Si_count++;
    TGeoVolume*   L1_DBM_A = gm->MakeTube("L1_DBM_A",med,20.0,40.0,1000.0);
    L1_DBM_A->SetVisibility(kFALSE);
    L0_A->AddNodeOverlap(L1_DBM_A,Si_count,donothing);
    Si_count++;
    TGeoVolume*   L1_PIX_A = gm->MakeTube("L1_PIX_A",med,40.0,150.0,700.0);
    L1_PIX_A->SetVisibility(kFALSE);
    L0_A->AddNodeOverlap(L1_PIX_A,Si_count,donothing);
    Si_count++;
    TGeoVolume*   L1_SCTA_A = gm->MakeTube("L1_SCTA_A",med,250.0,550.0,3000.0);
    L1_SCTA_A->SetVisibility(kFALSE);
    L0_A->AddNodeOverlap(L1_SCTA_A,Si_count,donothing);
    Si_count++;
    TGeoVolume*   L1_SCTB_A = gm->MakeTube("L1_SCTB_A",med,250.0,550.0,3000.0);
    L1_SCTB_A->SetVisibility(kFALSE);
    L0_A->AddNodeOverlap(L1_SCTB_A,Si_count,donothing);
    Si_count++;
    TGeoVolume*   L1_SCTC_A = gm->MakeTube("L1_SCTC_A",med,250.0,550.0,3000.0);
    L1_SCTC_A->SetVisibility(kFALSE);
    L0_A->AddNodeOverlap(L1_SCTC_A,Si_count,donothing);
    Si_count++;



    ATH_MSG_INFO("---------------------------------------------------");
    ATH_MSG_INFO("Summary of the alignment geometry");
    ATH_MSG_INFO("Number of alignable objects: "<< m_alignModuleList.size());
    for(unsigned int i=0;i<m_alignModuleList.size();i++) {
      const Trk::AlignModule* module = m_alignModuleList.at(i);
      ATH_MSG_INFO(i<<". "<< module->name());
      ATH_MSG_INFO("   - identifier:     "<<module->identify());
      ATH_MSG_INFO("   - identifierHash: "<<module->identifyHash());

      unsigned int npix(0);
      unsigned int nsct(0);
      unsigned int nSi(0);
      bool isPix(false);
      bool isSCT(false);
      if(module->detElementCollection(Trk::AlignModule::Pixel)) {
        npix = module->detElementCollection(Trk::AlignModule::Pixel)->size();
        nSi  = npix;   isPix = true;
        ATH_MSG_INFO("   - has "<<npix<<" Pixel modules");
      }
      if(module->detElementCollection(Trk::AlignModule::SCT)) {
        nsct = module->detElementCollection(Trk::AlignModule::SCT)->size();
        nSi  = nsct;   isSCT = true;
        ATH_MSG_INFO("   - has "<<nsct<<" SCT modules");
      }
      if(!isPix && !isSCT) ATH_MSG_INFO("   UNKNOWN module found:     "<<module->identify());

      // Loop over all detector elements of this align module:
      for(unsigned int j=0;j<nSi;j++) {
        const SiDetectorElement * element=nullptr;
        if(isPix) element = dynamic_cast<const SiDetectorElement*>(module->detElementCollection(Trk::AlignModule::Pixel)->at(j)); 
	if(isSCT) element = dynamic_cast<const SiDetectorElement*>(module->detElementCollection(Trk::AlignModule::SCT)->at(j)); 
	if (not element){
	  ATH_MSG_WARNING("Dynamic cast to SiDetectorElement from pixel or SCT module failed");
	  return;
	}
  const Identifier element_id = element->identify();
	int det,bec,layer,ring,sector,side;
	// in the future, the InDetAlignDBTool::idToDetSet should be directly used !                                              
	bool resok=false;
	if (m_pixHelper->is_pixel(element_id)) {
	  det=1;
	  bec=m_pixHelper->barrel_ec(element_id);
	  layer=m_pixHelper->layer_disk(element_id);
	  ring=m_pixHelper->eta_module(element_id);
	  sector=m_pixHelper->phi_module(element_id);
	  side=0;
	  resok=true;
	} else if (m_sctHelper->is_sct(element_id)) {
	  det=2;
	  bec=m_sctHelper->barrel_ec(element_id);
	  layer=m_sctHelper->layer_disk(element_id);
	  ring=m_sctHelper->eta_module(element_id);
	  sector=m_sctHelper->phi_module(element_id);
	  side=m_sctHelper->side(element_id);
	  resok=true;
	}
	if(!resok) ATH_MSG_INFO("   UNRESOLVED module found:     "<<element_id);

        if(resok && !(element->isStereo())) {      // take only phi SCT modules and Pixels
	  // extract the transformation parameters from Eigen:
          if( m_actualGeom ) {
            xyz = element->transform().translation();                   // translation (actual)
            ea  = element->transform().rotation().eulerAngles(2, 0, 2); // Euler angles (actual)
	  } else {
            xyz = element->defTransform().translation();                   // translation (nominal)
            ea  = element->defTransform().rotation().eulerAngles(2, 0, 2); // Euler angles (nominal)
	  }

	  ATH_MSG_INFO(">>> Element ident,det,bec,layer,ring,sector,side:      "<<element_id<<", "<<det<<", "<<bec<<", "<<layer<<", "<<ring<<", "<<sector<<", "<<side);
          ATH_MSG_INFO(">>> Element length/width/thickness:      "<<element->length()<<",  "<<element->width()<<",  "<<element->thickness());
          if(element->isSCT() && element->isEndcap()) 
            ATH_MSG_INFO(">>> SCT Endcap wedge,  min/max         "<<element->minWidth()<<" / "<<element->maxWidth());
          ATH_MSG_INFO(">>> Center position:                   "<<element->center());
          ATH_MSG_INFO(">>> Default transformation:            ");
          ATH_MSG_INFO(">>>            translation:            "<<xyz);
          ATH_MSG_INFO(">>>           Euler angles:            Phi="<<57.2957*ea[0]<<"  Theta="<<57.2957*ea[1]<<"  Psi="<<57.2957*ea[2]);


	  ATH_MSG_INFO("Adding a volume to the Silicon geometry:");
	  TString nname = "Si_COG_";
	  TString mname = "Si_MOD_";
    TString undsc = "_";

    std::string det_str = std::to_string(det);
	  std::string bec_str = std::to_string( bec);
	  std::string layer_str = std::to_string( layer);
	  std::string ring_str = std::to_string( ring);
	  std::string sector_str = std::to_string( sector);
	  const auto suffix = TString(det_str)+undsc+TString(bec_str)+undsc+TString(layer_str)+undsc+TString(ring_str)+undsc+TString(sector_str);
    nname += suffix;
    mname += suffix;

    Si_cog[Si_count] = gm->MakeSphere(nname,med,0.0,element->length(),0.0,180.0,0.0,360.0);    // invisible container
	  Si_cog[Si_count]->SetVisibility(kFALSE);
	  // create a wedge for SCT Endcap, otherwise a box:
          if(element->isSCT() && element->isEndcap())  {
            Si[Si_count] = gm->MakeTrd1(mname,med,0.5*element->minWidth(),0.5*element->maxWidth(),0.5*element->thickness(),0.5*element->length());    // this is a wedge!
	  }  else  {
            Si[Si_count] = gm->MakeBox(mname,med,0.5*element->width(),0.5*element->thickness(),0.5*element->length());    // creator takes the half-lengths!
	  }
          tr[Si_count] = new TGeoTranslation(); 
          tr[Si_count]->SetTranslation(xyz[0],xyz[1],xyz[2]);
          ro[Si_count] = new TGeoRotation();
          ro[Si_count]->SetAngles(57.2957*ea[0],57.2957*ea[1],57.2957*ea[2]);
          mx[Si_count] = new TGeoCombiTrans(*tr[Si_count],*ro[Si_count]);

	  TGeoVolume*   parrent_elem = nullptr; 
	  switch(det)
	    {
	    case 1:
	      if(bec==0 && layer==0) parrent_elem = L1_IBL_A;   
              else if(abs(bec)==4)   parrent_elem = L1_DBM_A;
	      else                   parrent_elem = L1_PIX_A;
	      break;
	    case 2:
	      switch(bec)
		{
		case -2:
		  parrent_elem = L1_SCTC_A;
		  break;
		case  0:
                  parrent_elem = L1_SCTB_A;
		  break;
		case  2:
                  parrent_elem = L1_SCTA_A;
		  break;
		default:
                  break;
		}
	      break;
	    default:
	      break;
	    }
          if(parrent_elem) {
	    //            parrent_elem->AddNode(Si_cog[Si_count],Si_count,mx[Si_count]);
	    //            top->AddNode(Si_cog[Si_count],Si_count,mx[Si_count]);
            top->AddNode(Si_cog[Si_count],0,mx[Si_count]);
  	    //            Si_cog[Si_count]->AddNode(Si[Si_count],Si_count,gGeoIdentity);
	    //            Si_cog[Si_count]->AddNode(Si[Si_count],side,swapaxes);
	    Si_cog[Si_count]->AddNode(Si[Si_count],0,swapaxes);
  	    Si_count++;
	  }
	}
      }


      if(npix && nsct)
        ATH_MSG_INFO("   - has "<<npix+nsct<<" Silicon modules in total");

      Amg::Transform3D localtoglobal = (module->globalFrameToAlignFrame()).inverse();
      ATH_MSG_DEBUG("   - local to global : "<<std::setprecision(12)<<localtoglobal.translation()<<" "<<localtoglobal.rotation());

      DataVector<Trk::AlignPar> * pars = m_alignModuleTool->getAlignPars(module);
      int npars = pars->size();
      ATH_MSG_DEBUG("   - number of active transform parameters: "<<npars);
      for(int j=0;j<npars;j++)
         ATH_MSG_DEBUG("      * par "<<j<<": sigma = "<<(*pars)[j]->sigma());

    }


    // close geometry end write the geometry root file:
    gm->CloseGeometry();
    gm->Export("Silicon.root","Silicon","v");

  }

  //________________________________________________________________________
  bool SiGeometryManagerTool::isOneDetOnly(const Trk::AlignModule * mod, Trk::AlignModule::DetectorType dettype) const
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


} // end namespace
