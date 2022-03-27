/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "AthContainers/DataVector.h"

#include "TRT_ReadoutGeometry/TRT_BarrelElement.h"
#include "TRT_ReadoutGeometry/TRT_EndcapElement.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"
#include "TRT_ReadoutGeometry/TRT_Numerology.h"
#include "InDetIdentifier/TRT_ID.h"

#include "TrkAlignInterfaces/IAlignModuleTool.h"

#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignEvent/AlignPar.h"


#include "InDetAlignGeomTools/TRTGeometryManagerTool.h"


#include <iostream>

using namespace InDetDD;

namespace InDet {

//________________________________________________________________________
TRTGeometryManagerTool::TRTGeometryManagerTool(const std::string& type,
      const std::string& name,
      const IInterface * parent)
  : AthAlgTool(type,name,parent)
      , m_trtDetManager(nullptr)
      , m_idHelper()
      , m_alignModuleTool("Trk::AlignModuleTool/AlignModuleTool")
      , m_idHashToAlignModuleMaps(Trk::AlignModule::NDetectorTypes,(Trk::AlignModuleList*)nullptr)
      , m_alignParList(nullptr)
      , m_fullAlignParList(nullptr)
{
   declareInterface<IGeometryManagerTool>(this);
   declareProperty("AlignModuleTool",    m_alignModuleTool);

   declareProperty("AlignBarrel",        m_alignBarrel        = true);
   declareProperty("AlignBarrelX",       m_alignBarrelX       = true);
   declareProperty("AlignBarrelY",       m_alignBarrelY       = true);
   declareProperty("AlignBarrelZ",       m_alignBarrelZ       = false);
   declareProperty("AlignBarrelRotX",    m_alignBarrelRotX    = true);
   declareProperty("AlignBarrelRotY",    m_alignBarrelRotY    = true);
   declareProperty("AlignBarrelRotZ",    m_alignBarrelRotZ    = true);

   declareProperty("AlignEndcaps",       m_alignEndcaps       = true);
   declareProperty("AlignEndcapX",       m_alignEndcapX       = true);
   declareProperty("AlignEndcapY",       m_alignEndcapY       = true);
   declareProperty("AlignEndcapZ",       m_alignEndcapZ       = true);
   declareProperty("AlignEndcapRotX",    m_alignEndcapRotX    = true);
   declareProperty("AlignEndcapRotY",    m_alignEndcapRotY    = true);
   declareProperty("AlignEndcapRotZ",    m_alignEndcapRotZ    = true);

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

   declareProperty("AlignmentLevel",        m_alignLevel        = 1);
   declareProperty("AlignmentLevelBarrel",  m_alignLevelBarrel  = -1);
   declareProperty("AlignmentLevelEndcaps", m_alignLevelEndcaps = -1);

   declareProperty("AlignBarrelPhiSectors", m_barrelPhiSectors);
   declareProperty("AlignBarrelLayers",     m_barrelLayers);

   declareProperty("AlignSingleEndcap",     m_endcap = 0);
   declareProperty("AlignEndcapWheels",     m_endcapWheels);
   declareProperty("AlignEndcapPhiSectors", m_endcapPhiSectors);

   declareProperty("doModuleSelection",  m_doModuleSelection  = false);
   declareProperty("ModuleSelection",    m_moduleSelection);

   declareProperty("DumpGeometry",       m_dumpGeometry = true);

   m_hashCounter = 0;
   m_logStream = nullptr;
}

//________________________________________________________________________
TRTGeometryManagerTool::~TRTGeometryManagerTool()
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
StatusCode TRTGeometryManagerTool::initialize()
{
   ATH_MSG_DEBUG("initialize() of TRTGeometryManagerTool");

   // retrieve AlignModuleTool
   if ( m_alignModuleTool.retrieve().isFailure() ) {
      msg(MSG::FATAL)<<"Could not get " << m_alignModuleTool << endmsg;
      return StatusCode::FAILURE;
   }
   else
      ATH_MSG_INFO("Retrieved " << m_alignModuleTool);

   // retrieve TRT ID helper
   if ( detStore()->retrieve(m_idHelper, "TRT_ID").isFailure() ) {
      msg(MSG::FATAL) << " Cannot retrieve TRT ID Helper " << endmsg;
      return StatusCode::FAILURE;
   }
   else
      ATH_MSG_INFO("retrieved TRT ID Helper");

   // retrieve TRT detector manager
   if ( detStore()->retrieve(m_trtDetManager).isFailure() ) {
      msg(MSG::FATAL) << " Cannot retrieve TRT Detector Manager " << endmsg;
      return StatusCode::FAILURE;
   }
   else
      ATH_MSG_INFO("retrieved TRT Detector Manager");

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
StatusCode TRTGeometryManagerTool::finalize()
{
   ATH_MSG_DEBUG("finalize() of TRTGeometryManagerTool");

   return StatusCode::SUCCESS;
}

//________________________________________________________________________
void TRTGeometryManagerTool::setAlignLevel(int level)
{
   m_alignLevelBarrel = m_alignLevelEndcaps = -1;
   m_alignLevel = level;
}

//________________________________________________________________________
bool TRTGeometryManagerTool::checkAlignLevel()
{
   if(m_alignLevel == 0) {
      ATH_MSG_INFO("Alignment level for TRT is "<<m_alignLevel);
      return true;
   }

   if(m_alignLevelBarrel == -1) {
      if(m_alignLevel == 200)
         m_alignLevelBarrel = 2;
      else
         m_alignLevelBarrel = m_alignLevel;
   }

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
bool TRTGeometryManagerTool::checkAlignLevelBarrel()
{
   if(!m_alignBarrel)
      return true;

   // check whether geometry level is allowed
   switch(m_alignLevelBarrel) {
      case 1: case 2: case 3:
         ATH_MSG_INFO("Alignment level for TRT Barrel is "<<m_alignLevelBarrel);
         return true;
      default:
         msg(MSG::FATAL)<<"Alignment level "<<m_alignLevelBarrel<<" does not exist for TRT Barrel"<<endmsg;
         return false;
   }
}

//________________________________________________________________________
bool TRTGeometryManagerTool::checkAlignLevelEndcaps()
{
   if(!m_alignEndcaps)
      return true;

   // check whether geometry level is allowed
   switch(m_alignLevelEndcaps) {
      case 1: case 2: case 200: case 3:
         ATH_MSG_INFO("Alignment level for TRT Endcaps is "<<m_alignLevelEndcaps);
         return true;
      default:
         msg(MSG::FATAL)<<"Alignment level "<<m_alignLevelEndcaps<<" does not exist for TRT Endcaps"<<endmsg;
         return false;
   }
}

//________________________________________________________________________
int TRTGeometryManagerTool::ReadGeometry(int solveLevel)
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
   std::vector<Trk::AlignModule *>::const_iterator imod = m_alignModuleList.begin();
   std::vector<Trk::AlignModule *>::const_iterator imod_end = m_alignModuleList.end();
   for( ; imod!=imod_end; ++imod)
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
void TRTGeometryManagerTool::buildGeometry()
{
   ATH_MSG_INFO("Preparing the TRT geometry");

   if(m_alignLevel == 0)
      buildL0();
   else {

      if(m_alignBarrel)
         switch(m_alignLevelBarrel) {
            case 1:
               buildL1Barrel();
               break;
            case 2:
               buildL2Barrel();
               break;
            case 3:
               buildL3Barrel();
               m_alignModuleTool->setSubDetElement(Trk::AlignModule::TRT,true);
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
            case 200:
               buildL2EndcapsOLD();
               break;
            case 3:
               buildL3Endcap();
               m_alignModuleTool->setSubDetElement(Trk::AlignModule::TRT,true);
               break;
            default:
               break;
         }
   }
}

//_______________________________________________________________________
void TRTGeometryManagerTool::buildL0()
{
   ATH_MSG_INFO("Preparing the TRT geometry for L0: whole TRT");

   // ========================================
   // get all modules for Level 0 alignment
   // Level 0 is the whole TRT, i.e., 1 AlignModule in total

   // define modules
   // we'll stick with the base AlignModule here since we don't want to
   // do shifting for the moment but we might need to extend AlignModule
   // in the future

   // for L1 alignment the alignment frame is equal to the global frame
   // and since the TRT detector element positions are also stored in
   // the global frame in DB, transform is identity
   Amg::Transform3D transform = Amg::Transform3D::Identity();

   // use the 'real' identifier of the TRT barrel for TRT L0 module
   Trk::AlignModule * trt = new Trk::AlignModule(this);
   trt->setIdHash(getNextIDHash());
   trt->setName("TRT");
   trt->setIdentifier(m_idHelper->barrel_ec_id(-1));

   if(!moduleSelected(trt)) {
      ATH_MSG_DEBUG("Module "<<trt->name()<<" NOT selected");
      delete trt;
      return;
   }

   // get maximum number of elements from the helper
   unsigned int trtmaxHash = m_idHelper->straw_layer_hash_max();
   ATH_MSG_DEBUG("maxHash for the TRT "<<trtmaxHash);

   if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT) = new Trk::AlignModuleList((size_t)(trtmaxHash),nullptr);
   Trk::AlignModuleList * trtIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT);

   // ================================================================
   // loop over TRT elements and add them to respective alignModules
   // ================================================================
   for (unsigned int index = 0; index < trtmaxHash; index++) {
      IdentifierHash idHash = index;
      Identifier id = m_idHelper->layer_id(idHash);

      // get the element via hash
      const TRT_BaseElement * element = m_trtDetManager->getElement(idHash);
      if (element) {
         // get element location for debugging
//         HepGeom::Point3D<double> center = element->transform() * HepGeom::Point3D<double>();
//         ATH_MSG_DEBUG(" TRTDetectorElement id: " << id << " with center = " << center);

         // add element to respective AlignModule

         // add to the Inner Detector structure
         if(msgLvl(MSG::DEBUG)) {
            if (m_idHelper->is_barrel(id))
               msg(MSG::DEBUG)<<"... TRT barrel element"<<endmsg;
            else
               msg(MSG::DEBUG)<<"... TRT endcap element"<<endmsg;
         }
         trt->addDetElement(Trk::AlignModule::TRT,element,transform);

         // and fill the corresponding map
         (*trtIdHashMap)[idHash] = trt;
      }
      else
         ATH_MSG_DEBUG("No TRTDetectorElement with id: "<<id);
   }

   // add created module to the geometry
   m_alignModuleListPtr->push_back(trt);

   ATH_MSG_DEBUG("TRT L0 module successfully added to the list");

   return;
}

//_______________________________________________________________________
void TRTGeometryManagerTool::buildL1Barrel()
{
   // ===========================
   // BARREL
   // ===========================
   ATH_MSG_INFO("Preparing the TRT Barrelgeometry for L1: whole barrel");

   // ========================================
   // get all modules for barrel Level 1 alignment
   // Level 1 is barrel as a single AlignModule

   // define modules
   // we'll stick with the base AlignModule here since we don't want to
   // do shifting for the moment but we might need to extend AlignModule
   // in the future

   // get maximum number of elements from the helper
   unsigned int maxHash = 0;
   if(m_alignLevelEndcaps==3)
      maxHash = m_idHelper->straw_hash_max();
   else
      maxHash = m_idHelper->straw_layer_hash_max();
   ATH_MSG_DEBUG("maxHash for the TRT "<<maxHash);

   if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT) = new Trk::AlignModuleList((size_t)maxHash,nullptr);
   Trk::AlignModuleList * trtIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT);

   // for L1 alignment the alignment frame is equal to the global frame
   // and since the TRT detector element positions are also stored in
   // the global frame in DB, transform is identity
   Amg::Transform3D transform = Amg::Transform3D::Identity();

   // BARREL
   // use the 'real' identifier of the TRT barrel that can be
   // used by idHelper->barrel_ec()
   // TRT barrel has two sides so both -1 and +1 are ok
   // but -1 is used in the DB so we use it here as well
   Trk::AlignModule * trtBarrel = new Trk::AlignModule(this);
   trtBarrel->setIdHash(getNextIDHash());
   trtBarrel->setName("TRT Barrel");
   trtBarrel->setIdentifier(m_idHelper->barrel_ec_id(-1));

   if(!moduleSelected(trtBarrel)) {
      ATH_MSG_DEBUG("Module "<<trtBarrel->name()<<" NOT selected");
      delete trtBarrel;
   }
   else {
      // loop over detector elements in the barrel and add them to respective alignModules
      for (unsigned int iLayer = 0; iLayer < m_trtDetManager->getNumerology()->getNBarrelRings(); iLayer++) // barrel layers
         for (unsigned int iPhiModule = 0; iPhiModule < m_trtDetManager->getNumerology()->getNBarrelPhi(); iPhiModule++) // modules in phi
            for (unsigned int iStrawLayer = 0; iStrawLayer < m_trtDetManager->getNumerology()->getNBarrelLayers(iLayer); iStrawLayer++) // straw layers inside one phi module
               for (int iSide = 0; iSide < 2; iSide++) { // side of the straw
                  const TRT_BarrelElement * element = m_trtDetManager->getBarrelElement(iSide, iLayer, iPhiModule, iStrawLayer);
                  if (element) {
                     // get element location for debugging
//                     HepGeom::Point3D<double> center = element->transform() * HepGeom::Point3D<double>();
//                     ATH_MSG_DEBUG(" TRTDetectorElement id: " << element->identify() << " with center = " << center);

                     // add to the barrel
                     trtBarrel->addDetElement(Trk::AlignModule::TRT,element,transform);
                     // and fill the corresponding map
                     (*trtIdHashMap)[element->identifyHash()] = trtBarrel;
                  }
                  else
                     ATH_MSG_DEBUG("No TRT_BarrelElement with side-layer-phi-strawLayer:" <<iSide<<"-"<<iLayer<<"-"<<iPhiModule<<"-"<<iStrawLayer);
               }

      // add
      m_alignModuleListPtr->push_back(trtBarrel);
   }
}


//_______________________________________________________________________
void TRTGeometryManagerTool::buildL1Endcaps()
{
   // ===========================
   // ENDCAPs
   // ===========================
   ATH_MSG_INFO("Preparing the TRT Endcap geometry for L1: 2 endcaps");

   // ========================================
   // get all modules for Endcap Level 1 alignment
   // Level 1 is endcap AlignModules

   // define modules
   // we'll stick with the base AlignModule here since we don't want to
   // do shifting for the moment but we might need to extend AlignModule
   // in the future

   // get maximum number of elements from the helper
   unsigned int maxHash = 0;
   if(m_alignLevelBarrel==3)
      maxHash = m_idHelper->straw_hash_max();
   else
      maxHash = m_idHelper->straw_layer_hash_max();
   ATH_MSG_DEBUG("maxHash for the TRT "<<maxHash);

   if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT) = new Trk::AlignModuleList((size_t)maxHash,nullptr);
   Trk::AlignModuleList * trtIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT);

   // for L1 alignment the alignment frame is equal to the global frame
   // and since the TRT detector element positions are also stored in
   // the global frame in DB, transform is identity
   Amg::Transform3D transform = Amg::Transform3D::Identity();

   // ENDCAPs
   // identifiers for endcaps are set in analogy to barrel
   // above to -2 and +2 so they can be used by
   // idHelper->barrel_ec()
   Trk::AlignModule * trtEndCap[2];
   for (int iSide=0;iSide<2;iSide++) {
      trtEndCap[iSide] = new Trk::AlignModule(this);
      trtEndCap[iSide] -> setIdHash(getNextIDHash());

      if(iSide)
         trtEndCap[iSide]->setName("TRT EndCap C");
      else
         trtEndCap[iSide]->setName("TRT EndCap A");

      int idSide = iSide ? 2 : -2;
      trtEndCap[iSide]->setIdentifier(m_idHelper->barrel_ec_id(idSide));

      if( !moduleSelected(trtEndCap[iSide]) || (m_endcap!=idSide && m_endcap!=0 ) ) {
         ATH_MSG_DEBUG("Module "<<trtEndCap[iSide]->name()<<" NOT selected");
         delete trtEndCap[iSide];
      }
      else {
         for (unsigned int iWheel = 0; iWheel < m_trtDetManager->getNumerology()->getNEndcapWheels(); iWheel++)
            for (unsigned int iStrawLayer = 0; iStrawLayer < m_trtDetManager->getNumerology()->getNEndcapLayers(iWheel); iStrawLayer++)
               for(unsigned int iPhi = 0; iPhi < m_trtDetManager->getNumerology()->getNEndcapPhi(); iPhi++) {
                  const TRT_EndcapElement * element = m_trtDetManager->getEndcapElement(iSide, iWheel, iStrawLayer, iPhi);
                  if (element) {
                     // get element location for debugging
//                     HepGeom::Point3D<double> center = element->transform() * HepGeom::Point3D<double>();
//                     ATH_MSG_DEBUG(" TRTDetectorElement id: " << id << " with center = " << center);

                     // add element to respective AlignModule
                     trtEndCap[iSide]->addDetElement(Trk::AlignModule::TRT,element,transform);
                     // and fill the corresponding map
                     (*trtIdHashMap)[element->identifyHash()] = trtEndCap[iSide];
                  }
                  else
                     ATH_MSG_DEBUG("No TRT_EndcapElement with side-wheel-strawLayer-phi:" <<iSide<<"-"<<iWheel<<"-"<<iStrawLayer<<"-"<<iPhi);
               }

         // for endcap we move the CoG
         Amg::Translation3D translation(trtEndCap[iSide]->centerOfGravity());
         Amg::Transform3D localtoglobal = translation * Amg::RotationMatrix3D::Identity();
         trtEndCap[iSide]->setGlobalFrameToAlignFrameTransform(localtoglobal.inverse());

         // add
         m_alignModuleListPtr->push_back(trtEndCap[iSide]);
      }
   }
}


//_______________________________________________________________________
void TRTGeometryManagerTool::buildL2Barrel()
{
   // ===========================
   // BARREL
   // ===========================
   ATH_MSG_INFO("Preparing the TRT Barrel geometry for L2: 3 layers with 32 modules each (96 modules)");

   // ========================================
   // get all modules for TRT Barrel Level 2 alignment
   // Level 2 in the barrel is 3 layers with 32 phi modules each
   // i.e. 96 AlignModules in total

   // define modules
   // we'll stick with the base AlignModule here since we don't want to
   // do shifting for the moment but we might need to extend AlignModule
   // in the future

   // TRT detector element positions are stored in the DB in global frame
   // so the DB frame to global frame transform is identity
   Amg::Transform3D transform = Amg::Transform3D::Identity();

   // get maximum number of elements from the helper
   unsigned int maxHash = 0;
   if(m_alignLevelEndcaps==3)
      maxHash = m_idHelper->straw_hash_max();
   else
      maxHash = m_idHelper->straw_layer_hash_max();
   ATH_MSG_DEBUG("maxHash for the TRT "<<maxHash);

   if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT) = new Trk::AlignModuleList((size_t)maxHash,nullptr);
   Trk::AlignModuleList * trtIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT);

   // for the alignment frame we would like to use something like
   // the 'center-of-gravity' (cog) of the module and the rotation
   // of the strawlayer planes. the main problem here is that the
   // planes are not entirely parallel. for the definition of the
   // alignment of the module this is all irrelevant, but we would
   // like to give the twist some meaning, like 'the rotation
   // around the most central straw': since strawlayer planes are
   // even and straws as well, there is no such thing as a really
   // central straw. we'll do something that brings us as close as
   // possible: for the central plane take strawlayer that is at
   // (N+1)/2, where N is the maximum strawlayer. (strawlayers
   // start counting at 0). The reason to pick the outer of the two
   // central strawlayers is that there are more straws per layer
   // in the outer layers and that the outer layers are also
   // further apart, so this is closer to the 'real' cog.

   for (unsigned int iPhiModule = 0; iPhiModule < m_trtDetManager->getNumerology()->getNBarrelPhi(); iPhiModule++) { // modules in phi
      for (unsigned int iLayer = 0; iLayer < m_trtDetManager->getNumerology()->getNBarrelRings(); iLayer++) {// barrel layers
         // create the AlignModule
         Trk::AlignModule * mod = new Trk::AlignModule(this);
         mod->setIdHash(getNextIDHash());
         mod->setIdentifier(m_idHelper->module_id(-1, iPhiModule, iLayer));

         std::stringstream name;
         name<<"TRT/Barrel/Module_-1_"<<iPhiModule<<"_"<<iLayer;
         mod->setName(name.str());

         if(!moduleSelected(mod) || !moduleSelectedBarrel(iLayer,iPhiModule)) {
            ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
            delete mod;
            continue;
         }

         ATH_MSG_DEBUG("Building module "<<mod->name());

         // create the collection of central layers for cog and transform calculation
         std::vector<const Trk::TrkDetElementBase *> centralLayers;

         int maxstrawlayer = m_trtDetManager->getNumerology()->getNBarrelLayers(iLayer);
         ATH_MSG_DEBUG("maxstrawlayer "<<maxstrawlayer);
         for (int iStrawLayer = 0; iStrawLayer < maxstrawlayer; iStrawLayer++) { // straw layers inside one phi module
            ATH_MSG_DEBUG("iStrawLayer "<<iStrawLayer);
            for (int iSide = 0; iSide < 2; iSide++) { // side of the straw
               ATH_MSG_DEBUG("iSide "<<iSide);
               const TRT_BarrelElement * element = m_trtDetManager->getBarrelElement(iSide, iLayer, iPhiModule, iStrawLayer);
               if (element) {
                  // get element location for debugging
//                  HepGeom::Point3D<double> center = element->transform() * HepGeom::Point3D<double>();
//                  ATH_MSG_DEBUG(" TRTDetectorElement id: " << element->identify() << " with center = " << center);

                  // update maximum straw layer
                  maxstrawlayer = std::max(maxstrawlayer,m_idHelper->straw_layer(element->identify()));

                  // add element to respective AlignModule
                  mod->addDetElement(Trk::AlignModule::TRT,element,transform);

                  // and fill the corresponding map
                  (*trtIdHashMap)[element->identifyHash()] = mod;

                  // add central layers to centralLayers collection
                  // maxstrawlayer is probably odd (if there is an even number
                  // of strawlayers), but I could be wrong, so let's make this always work:
                  // if maxstrawlayer==23, we want to pick strawlayer 12
                  // if maxstrawlayer==24, we still want to pick strawlayer 12
                  if( iStrawLayer == maxstrawlayer/2 ) {
                     //Amg::Vector3D center = element->transform() * Amg::Vector3D(0,0,0);
                     centralLayers.push_back(element);
                  }
               }
               else
                  ATH_MSG_DEBUG("No TRT_BarrelElement with side-layer-phi-strawLayer:" <<iSide<<"-"<<iLayer<<"-"<<iPhiModule<<"-"<<iStrawLayer);
            }
         }

         // This is how the frames line up for a bec=-1 module at global
         // position (0,R,-360):
         //
         // global       straw   det-element
         //     x          +x       -x
         //     y          +y       +z
         //     z          +z       +y
         //
         // For other bec=-1 modules you just need to rotate around
         // global y. For bec=+1 modules it looks like this:
         //
         // global       straw   det-element
         //     x          -x       -x
         //     y          +y       +z
         //     z          -z       +y
         // So that just involves one rotation around local-y for the strawreference frame.
         //
         std::vector<std::vector<const Trk::TrkDetElementBase *> *> centralLayersVec;
         centralLayersVec.push_back(&centralLayers);

         Amg::Translation3D translation(mod->centerOfGravity(centralLayersVec));
         // for the rotation, we choose the element on the '-1' side.
         Amg::RotationMatrix3D rotation = ( m_idHelper->barrel_ec(centralLayers.front()->identify()) == -1 ) ?
            centralLayers.front()->transform().rotation() : centralLayers.back()->transform().rotation();
         Amg::Transform3D localToGlobal = translation * rotation;



         ATH_MSG_DEBUG("centerOfGravity "<< mod->centerOfGravity(centralLayersVec));
         ATH_MSG_DEBUG("Prepared local to global transform : "<<std::setprecision(12)<<localToGlobal.translation()<<" "<<localToGlobal.rotation());

         mod->setGlobalFrameToAlignFrameTransform(localToGlobal.inverse());

         // add AlignModule to the geometry
         m_alignModuleListPtr->push_back(mod);
      }
   }

   return;
}


//_______________________________________________________________________
void TRTGeometryManagerTool::buildL2Endcaps()
{
   // ===========================
   // ENDCAPs
   // ===========================
   ATH_MSG_INFO("Preparing the TRT Endcap geometry for L2: 2 x 40 rings");

   // ========================================
   // get all modules for TRT Endcap Level 2 alignment
   // Level 2 in the endcaps is 40 rings per endcap
   // i.e. 40 AlignModules in total

   // define modules
   // we'll stick with the base AlignModule here since we don't want to
   // do shifting for the moment but we might need to extend AlignModule
   // in the future

   // TRT detector element positions are stored in the DB in global frame
   // so the DB frame to global frame transform is identity
   Amg::Transform3D transform = Amg::Transform3D::Identity();

   // get maximum number of elements from the helper
   unsigned int maxHash = 0;
   if(m_alignLevelBarrel==3)
      maxHash = m_idHelper->straw_hash_max();
   else
      maxHash = m_idHelper->straw_layer_hash_max();
   ATH_MSG_DEBUG("maxHash for the TRT "<<maxHash);

   if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT) = new Trk::AlignModuleList((size_t)maxHash,nullptr);
   Trk::AlignModuleList * trtIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT);

   for (int iSide = 0; iSide < 2; iSide++) {
      int idSide = iSide ? 2 : -2;
      ATH_MSG_DEBUG("Side : "<<iSide<<" , idSide : "<<idSide);
      if( m_endcap!=idSide && m_endcap!=0 ) {
         ATH_MSG_DEBUG("Endcap "<<idSide<<" NOT aligned");
         continue;
      }

      int iRing(-1);
      Trk::AlignModule * mod = nullptr;
      for (unsigned int iWheel = 0; iWheel < m_trtDetManager->getNumerology()->getNEndcapWheels(); iWheel++) {
         ATH_MSG_DEBUG("Wheel : "<<iWheel);
         for (unsigned int iStrawLayer = 0; iStrawLayer < m_trtDetManager->getNumerology()->getNEndcapLayers(iWheel); iStrawLayer++) {
            ATH_MSG_DEBUG("StrawLayer : "<<iStrawLayer);

            if(iStrawLayer%4==0) {

               if(iRing >= 0) {
                  // before creating module for new ring we set the alignment
                  // frame for the previous one and add it to the list of modules
                  ATH_MSG_DEBUG("Setting frame for ring : "<<iRing);

                  // for endcap we move the CoG with no additional rotation
                  Amg::Translation3D translation(mod->centerOfGravity());
            
                  Amg::Transform3D localToGlobal = translation * Amg::RotationMatrix3D::Identity();
 
                  mod->setGlobalFrameToAlignFrameTransform(localToGlobal.inverse());

                  // add AlignModule to the geometry
                  m_alignModuleListPtr->push_back(mod);
               }

               // new ring
               iRing++;

               ATH_MSG_DEBUG("Ring : "<<iRing);
               // create the AlignModule
               mod = new Trk::AlignModule(this);
               mod->setIdHash(getNextIDHash());
               // Identifier for a ring is the Identifier for the first Straw layer in that ring
               // one ring has 4 straw layers
               int strawLayerForRing = (iStrawLayer/4) * 4;
               mod->setIdentifier(m_idHelper->layer_id((iSide ? 2:-2),0,iWheel,strawLayerForRing));

               std::stringstream name;
               name<<"TRT/Endcap/Module_"<<(iSide ? 2:-2)<<"_"<<iRing;
               mod->setName(name.str());

               if(!moduleSelected(mod)) {
                  ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
                  delete mod;
                  mod=nullptr;
                  continue;
               }

               ATH_MSG_DEBUG("Building module "<<mod->name());
            }

            for(unsigned int iPhi = 0; iPhi < m_trtDetManager->getNumerology()->getNEndcapPhi(); iPhi++) {
               const TRT_EndcapElement * element = m_trtDetManager->getEndcapElement(iSide, iWheel, iStrawLayer, iPhi);
               if (element) {
                  // get element location for debugging
//                  HepGeom::Point3D<double> center = element->transform() * HepGeom::Point3D<double>();
//                  ATH_MSG_DEBUG(" TRTDetectorElement id: " << id << " with center = " << center);

                  // add element to respective AlignModule
                  mod->addDetElement(Trk::AlignModule::TRT,element,transform);

                  // and fill the corresponding map
                  (*trtIdHashMap)[element->identifyHash()] = mod;
               }
               else
                  ATH_MSG_DEBUG("No TRT_EndcapElement with side-wheel-strawLayer-phi:" <<iSide<<"-"<<iWheel<<"-"<<iStrawLayer<<"-"<<iPhi);
            }
         }
      }

      if (!mod) throw std::logic_error("No AlignmentModule");

      // for the last ring we have to explicitly set the alignment
      // frame add it to the list of modules here, at the end of
      // loop over wheels
      ATH_MSG_DEBUG("Setting frame for ring : "<<iRing);
      
      // for endcap we move the CoG with no additional rotation
      Amg::Translation3D translation(mod->centerOfGravity());      
      Amg::Transform3D localToGlobal = translation * Amg::RotationMatrix3D::Identity();

      mod->setGlobalFrameToAlignFrameTransform(localToGlobal.inverse());

      // add AlignModule to the geometry
      m_alignModuleListPtr->push_back(mod);
   }
}


//_______________________________________________________________________
void TRTGeometryManagerTool::buildL2EndcapsOLD()
{
   // !!!!   WARNING   !!!!
   // !!!!   This method sets up the TRT Endcap level 2 geometry which
   // !!!!   is now obsolete. The method buildL2Endcap() should be used
   // !!!!   instead !!!

   // ===========================
   // ENDCAPs
   // ===========================
   ATH_MSG_INFO("Preparing the _OLD_ TRT Endcap geometry for L2: 2 x 14 wheels");

   // ========================================
   // get all modules for TRT Endcap Level 2 alignment
   // Level 2 in the endcaps is 14 wheels per endcap
   // i.e. 28 AlignModules in total

   // define modules
   // we'll stick with the base AlignModule here since we don't want to
   // do shifting for the moment but we might need to extend AlignModule
   // in the future

   // TRT detector element positions are stored in the DB in global frame
   // so the DB frame to global frame transform is identity
   Amg::Transform3D transform = Amg::Transform3D::Identity();

   // get maximum number of elements from the helper
   unsigned int maxHash = 0;
   if(m_alignLevelBarrel==3)
      maxHash = m_idHelper->straw_hash_max();
   else
      maxHash = m_idHelper->straw_layer_hash_max();
   ATH_MSG_DEBUG("maxHash for the TRT "<<maxHash);

   if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT) = new Trk::AlignModuleList((size_t)maxHash,nullptr);
   Trk::AlignModuleList * trtIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT);

   for (int iSide = 0; iSide < 2; iSide++) {
      int idSide = iSide ? 2 : -2;
      ATH_MSG_DEBUG("Side : "<<iSide<<" , idSide : "<<idSide);
      if( m_endcap!=idSide && m_endcap!=0 ) {
         ATH_MSG_DEBUG("Endcap "<<idSide<<" NOT aligned");
         continue;
      }

      for (unsigned int iWheel = 0; iWheel < m_trtDetManager->getNumerology()->getNEndcapWheels(); iWheel++) {
         // create the AlignModule
         Trk::AlignModule * mod = new Trk::AlignModule(this);
         mod->setIdHash(getNextIDHash());
         mod->setIdentifier(m_idHelper->module_id(idSide,0,iWheel));

         std::stringstream name;
         name<<"TRT/Endcap/Module_"<<idSide<<"_"<<iWheel;
         mod->setName(name.str());

         if(!moduleSelected(mod)) {
            ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
            delete mod;
            continue;
         }

         ATH_MSG_DEBUG("Building module "<<mod->name());

         for (unsigned int iStrawLayer = 0; iStrawLayer < m_trtDetManager->getNumerology()->getNEndcapLayers(iWheel); iStrawLayer++) {
            for(unsigned int iPhi = 0; iPhi < m_trtDetManager->getNumerology()->getNEndcapPhi(); iPhi++) {
               const TRT_EndcapElement * element = m_trtDetManager->getEndcapElement(iSide, iWheel, iStrawLayer, iPhi);
               if (element) {
                  // get element location for debugging
//                  HepGeom::Point3D<double> center = element->transform() * HepGeom::Point3D<double>();
//                  ATH_MSG_DEBUG(" TRTDetectorElement id: " << id << " with center = " << center);

                  // add element to respective AlignModule
                  mod->addDetElement(Trk::AlignModule::TRT,element,transform);

                  // and fill the corresponding map
                  (*trtIdHashMap)[element->identifyHash()] = mod;
               }
               else
                  ATH_MSG_DEBUG("No TRT_EndcapElement with side-wheel-strawLayer-phi:" <<iSide<<"-"<<iWheel<<"-"<<iStrawLayer<<"-"<<iPhi);
            }
         }

         // for endcap we move the CoG with no additional rotation
         Amg::Translation3D translation(mod->centerOfGravity());
         Amg::Transform3D localtoglobal = translation * Amg::RotationMatrix3D::Identity();

         mod->setGlobalFrameToAlignFrameTransform(localtoglobal.inverse());

         // add AlignModule to the geometry
         m_alignModuleListPtr->push_back(mod);
      }
   }

   return;
}


//_______________________________________________________________________
void TRTGeometryManagerTool::buildL3Barrel()
{
   // ===========================
   // BARREL Level 3
   // ===========================
   ATH_MSG_INFO("Preparing the TRT Barrel geometry for L3");

   // ========================================
   // get all modules for TRT Barrel Level 3 alignment
   // Level 3 is one AlignModule per straw

   // define modules
   // we'll stick with the base AlignModule here since we don't want to
   // do shifting for the moment but we might need to extend AlignModule
   // in the future

   // TRT detector element positions are stored in the DB in global frame
   // so the DB frame to global frame transform is identity
   Amg::Transform3D transform = Amg::Transform3D::Identity();

   // get maximum number of elements from the helper
   unsigned int maxHash = m_idHelper->straw_hash_max();
   ATH_MSG_DEBUG("maxHash for the TRT "<<maxHash);

   if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT))
      m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT) = new Trk::AlignModuleList((size_t)maxHash,nullptr);
   Trk::AlignModuleList * trtIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT);

   for (unsigned int iPhiModule = 0; iPhiModule < m_trtDetManager->getNumerology()->getNBarrelPhi(); iPhiModule++) { // modules in phi
      ATH_MSG_DEBUG("iPhiModule "<<iPhiModule);
      for (unsigned int iLayer = 0; iLayer < m_trtDetManager->getNumerology()->getNBarrelRings(); iLayer++) {// barrel layers
         ATH_MSG_DEBUG("iLayer "<<iLayer);

         if(!moduleSelectedBarrel(iLayer,iPhiModule)) {
            ATH_MSG_INFO("Skipping barrel layer "<<iLayer<<"  phi sector "<<iPhiModule);
            continue;
         }

         for (unsigned int iStrawLayer = 0; iStrawLayer < m_trtDetManager->getNumerology()->getNBarrelLayers(iLayer); iStrawLayer++) { // straw layers inside one phi module
            ATH_MSG_DEBUG("iStrawLayer "<<iStrawLayer);
            for (int iSide = 0; iSide < 2; iSide++) { // side of the straw
               ATH_MSG_DEBUG("iSide "<<iSide);
               int idSide = iSide ? 1 : -1;

               const TRT_BarrelElement * element = m_trtDetManager->getBarrelElement(iSide, iLayer, iPhiModule, iStrawLayer);
               if(element) {
                  for (unsigned int iStraw = 0; iStraw< element->nStraws(); iStraw++)
                  {
                     ATH_MSG_DEBUG("iStraw "<<iStraw);

                     // create the AlignModule
                     Trk::AlignModule * mod = new Trk::AlignModule(this);
                     mod->setIdHash(getNextIDHash());
                     Identifier moduleId = m_idHelper->straw_id(idSide,iPhiModule,iLayer,iStrawLayer,iStraw);
                     mod->setIdentifier(moduleId);

                     std::stringstream name;
                     name<<"TRT/Barrel/Straw_"<<idSide<<"_"<<iPhiModule<<"_"<<iLayer<<"_"<<iStrawLayer<<"_"<<iStraw;
                     mod->setName(name.str());

                    if(!moduleSelected(mod)) {
                       ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
                       delete mod;
                       mod=nullptr;
                       continue;
                    }

                    ATH_MSG_DEBUG("Building module "<<mod->name());

                    // add element to respective AlignModule
                    mod->addDetElement(Trk::AlignModule::TRT, element, transform, moduleId);

                    // and fill the corresponding map
                    (*trtIdHashMap)[m_idHelper->straw_hash(moduleId)] = mod;

                    // set the align frame transform
                    Amg::Transform3D localToGlobal = element->strawTransform(iStraw);
                    Amg::RotationMatrix3D lgrot = localToGlobal.rotation();
                    ATH_MSG_DEBUG("Prepared local to global transform : "<<std::setprecision(12)
                                  <<localToGlobal.translation()<<" "<<localToGlobal.rotation());
                    ATH_MSG_DEBUG("  rotation = (" << lgrot(2,1) << "," << lgrot(0,2) << "," << lgrot(1,0) << ")");

                    mod->setGlobalFrameToAlignFrameTransform(localToGlobal.inverse());

                    Amg::Transform3D aatr = element->transform();
                    Amg::RotationMatrix3D aarot = aatr.rotation();

                    ATH_MSG_DEBUG("Straw layer transform :              "<<std::setprecision(12)
                          <<aatr.translation()<<" "<<aatr.rotation());
                    ATH_MSG_DEBUG("  rotation = (" << aarot(2,1) << "," << aarot(0,2) << "," << aarot(1,0) << ")");

                    // add AlignModule to the geometry
                    m_alignModuleListPtr->push_back(mod);
                  }
               }
               else
                  ATH_MSG_DEBUG("No TRT_BarrelElement with side-layer-phi-strawLayer:" <<iSide<<"-"<<iLayer<<"-"<<iPhiModule<<"-"<<iStrawLayer);
            }
         }
      }
   }

   return;
}

//_______________________________________________________________________
void TRTGeometryManagerTool::buildL3Endcap()
{
   // ===========================
   // ENDCAP Level 3
   // ===========================
   ATH_MSG_INFO("Preparing the TRT Endcap geometry for L3");

   // ========================================
   // get all modules for TRT Endcap Level 3 alignment
   // Level 3 is one AlignModule per straw

   // define modules
   // we'll stick with the base AlignModule here since we don't want to
   // do shifting for the moment but we might need to extend AlignModule
   // in the future

   // TRT detector element positions are stored in the DB in the local frame
   // so the DB frame to local frame transform is identity
   Amg::Transform3D transform = Amg::Transform3D::Identity();

   // get maximum number of elements from the helper
   unsigned int maxHash = m_idHelper->straw_hash_max();
   ATH_MSG_DEBUG("maxHash for the TRT "<<maxHash);

   if(!m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT))
     m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT) = new Trk::AlignModuleList((size_t)maxHash,nullptr);
   Trk::AlignModuleList * trtIdHashMap = m_idHashToAlignModuleMapsPtr->at(Trk::AlignModule::TRT);


   for (int iSide = 0; iSide < 2; iSide++) { // endcap
     int idSide = iSide ? 2 : -2;
     ATH_MSG_DEBUG("Side : "<<iSide<<" , idSide : "<<idSide);
     if( m_endcap!=idSide && m_endcap!=0 ) {
        ATH_MSG_DEBUG("Endcap "<<idSide<<" NOT aligned");
        continue;
     }

     // wheels in Z
     for (unsigned int iWheel = 0; iWheel < m_trtDetManager->getNumerology()->getNEndcapWheels(); iWheel++) {
       ATH_MSG_DEBUG("iWheel "<<iWheel);

       // sectors in Phi
       for (unsigned int iPhiModule = 0; iPhiModule < m_trtDetManager->getNumerology()->getNEndcapPhi(); iPhiModule++) {

         ATH_MSG_DEBUG("iPhiModule "<<iPhiModule);

         if(!moduleSelectedEndcap(iWheel,iPhiModule)) {
           ATH_MSG_INFO("Skipping endcap wheel "<<iWheel<<"  phi sector "<<iPhiModule);
           continue;
         }

         // endcap straw layers in z (with in a wheel)
         for (unsigned int iStrawLayer = 0; iStrawLayer < m_trtDetManager->getNumerology()->getNEndcapLayers(iWheel); iStrawLayer++) {

           const TRT_EndcapElement * element = m_trtDetManager->getEndcapElement(iSide, iWheel, iStrawLayer, iPhiModule);

           if(element) {
              for (unsigned int iStraw = 0; iStraw< element->nStraws(); iStraw++) {
                 ATH_MSG_DEBUG("iStraw "<<iStraw);

                 // create the AlignModule
                 Trk::AlignModule * mod = new Trk::AlignModule(this);
                 mod->setIdHash(getNextIDHash());
                 Identifier moduleId = m_idHelper->straw_id(idSide,iPhiModule,iWheel,iStrawLayer,iStraw);
                 mod->setIdentifier(moduleId);

                 std::stringstream name;
                 name<<"TRT/Endcap/Straw_"<<idSide<<"_"<<iPhiModule<<"_"<<iWheel<<"_"<<iStrawLayer<<"_"<<iStraw;
                 mod->setName(name.str());

                 if(!moduleSelected(mod)) {
                    ATH_MSG_DEBUG("Module "<<mod->name()<<" NOT selected");
                    delete mod;
                    mod=nullptr;
                    continue;
                 }

                 ATH_MSG_DEBUG("Building module "<<mod->name());

                 // add element to respective AlignModule
                 mod->addDetElement(Trk::AlignModule::TRT, element, transform, moduleId);

                 // and fill the corresponding map
                 (*trtIdHashMap)[m_idHelper->straw_hash(moduleId)] = mod;

                 // set the align frame transform
                 Amg::Transform3D localToGlobal = element->strawTransform(iStraw);
                 Amg::RotationMatrix3D lgrot = localToGlobal.rotation();

                 ATH_MSG_DEBUG("Prepared local to global transform : "<<std::setprecision(12)
                               <<localToGlobal.translation()<<" "<<localToGlobal.rotation());

                 ATH_MSG_DEBUG("  rotation = (" << lgrot(2,1) << "," << lgrot(0,2) << "," << lgrot(1,0) << ")");

                 mod->setGlobalFrameToAlignFrameTransform(localToGlobal.inverse());

                 Amg::Transform3D aatr = element->transform();
                 Amg::RotationMatrix3D aarot = aatr.rotation();

                 ATH_MSG_DEBUG("Straw layer transform :              "<<std::setprecision(12)
                               <<aatr.translation()<<" "<<aatr.rotation());
                 ATH_MSG_DEBUG("  rotation = (" << aarot(2,1) << "," << aarot(0,2) << "," << aarot(1,0) << ")");

                 // add AlignModule to the geometry
                 m_alignModuleListPtr->push_back(mod);
              }// Straws
           }// if the EndcapElement Exists
           else
             ATH_MSG_DEBUG("No TRT_EndcapElement with endcap-wheel-strawLayer-phi:" <<iSide<<"-"<<iWheel<<"-"<<iStrawLayer<<"-"<<iPhiModule);
         }// StrawLayer
       }// PhiModule
     }// Wheel
   }// Endcap

   return;
}

//________________________________________________________________________
void TRTGeometryManagerTool::addModuleParameters(Trk::AlignModule * module, DataVector< DataVector<Trk::AlignPar> > * allFullModPars, DataVector< DataVector<Trk::AlignPar> > * allActiveModPars)
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

      Identifier modID = module->identify();

      if(m_idHelper->is_barrel(modID)) {
         ATH_MSG_DEBUG("Barrel module with id "<<modID);
         if( (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransX && m_alignBarrelX)
            || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransY && m_alignBarrelY)
            || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransZ && m_alignBarrelZ)
            || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotX && m_alignBarrelRotX)
            || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotY && m_alignBarrelRotY)
            || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotZ && m_alignBarrelRotZ) ) {
            ATH_MSG_DEBUG("parameter type "<<fullModPars->at(ipar)->paramType()<<" is now active");
            activeModPars->push_back(fullModPars->at(ipar));
         }
         else
            ATH_MSG_DEBUG("parameter type "<<fullModPars->at(ipar)->paramType()<<" is NOT active");
      }
      else {
         ATH_MSG_DEBUG("Endcap module with id "<<modID);
         if( (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransX && m_alignEndcapX)
            || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransY && m_alignEndcapY)
            || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::TransZ && m_alignEndcapZ)
            || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotX && m_alignEndcapRotX)
            || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotY && m_alignEndcapRotY)
            || (fullModPars->at(ipar)->paramType() == Trk::AlignModule::RotZ && m_alignEndcapRotZ) ) {
            ATH_MSG_DEBUG("parameter type "<<fullModPars->at(ipar)->paramType()<<" is now active");
            activeModPars->push_back(fullModPars->at(ipar));
         }
         else
            ATH_MSG_DEBUG("parameter type "<<fullModPars->at(ipar)->paramType()<<" is NOT active");
      }
   }

   // add to parameter lists
   allFullModPars->push_back(fullModPars);
   allActiveModPars->push_back(activeModPars);
}


//________________________________________________________________________
void TRTGeometryManagerTool::setSigmas(Trk::AlignModule * module, DataVector<Trk::AlignPar> * modPars)
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
bool TRTGeometryManagerTool::moduleSelected(unsigned long long id)
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
bool TRTGeometryManagerTool::moduleSelected(Trk::AlignModule * mod)
{
   return moduleSelected(mod->identify().get_compact());
}


//________________________________________________________________________
bool TRTGeometryManagerTool::moduleSelectedBarrel(unsigned int ilayer, unsigned int iphi)
{
   bool select = true;
   if(m_barrelLayers.size()) {
      select = false;
      for(unsigned int i=0;i<m_barrelLayers.size();i++) {
         if(m_barrelLayers[i] == (int)ilayer) {
            select = true;
            break;
         }
      }
   }

   if(!select)
      return false;

   if(m_barrelPhiSectors.size()) {
      select = false;
      for(unsigned int i=0;i<m_barrelPhiSectors.size();i++) {
         if(m_barrelPhiSectors[i] == (int)iphi) {
            select = true;
            break;
         }
      }
   }

   return select;
}

//________________________________________________________________________
bool TRTGeometryManagerTool::moduleSelectedEndcap(unsigned int iwheel, unsigned int iphi)
{
  bool select = true;
  if(m_endcapWheels.size()) {
    select = false;
    for(unsigned int i=0;i<m_endcapWheels.size();++i) {
      if(m_endcapWheels[i] == (int)iwheel) {
        select = true;
        break;
      }
    }
  }

  if(!select)
    return false;

  if(m_endcapPhiSectors.size()) {
    select = false;
    for(unsigned int i=0;i<m_endcapPhiSectors.size();i++) {
      if(m_endcapPhiSectors[i] == (int)iphi) {
        select = true;
        break;
      }
    }
  }

  return select;
}


//________________________________________________________________________
void TRTGeometryManagerTool::dumpGeometry()
{
   ATH_MSG_INFO("---------------------------------------------------");
   ATH_MSG_INFO("Summary of the alignment geometry");
   ATH_MSG_INFO("Number of alignable objects: "<<m_alignModuleList.size());
   ATH_MSG_INFO("Total number of aligment DoFs: "<<m_alignModuleTool->nAlignParameters());

   for(unsigned int i=0;i<m_alignModuleList.size();i++) {
      const Trk::AlignModule * module = m_alignModuleList.at(i);
      //ATH_MSG_DEBUG(i<<". "<< module->name());
      //ATH_MSG_DEBUG("   - identifier: "<<module->identify());
      //ATH_MSG_DEBUG("   - has "<<module->detElementCollection(Trk::AlignModule::TRT)->size()<<" TRT modules");

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

