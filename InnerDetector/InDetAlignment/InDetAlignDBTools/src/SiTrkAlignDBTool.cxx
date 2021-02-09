/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "CLHEP/Geometry/Transform3D.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IOVTime.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetAlignGenTools/IInDetAlignDBTool.h"

#include "TrkAlignInterfaces/IAlignModuleTool.h"
#include "TrkAlignInterfaces/IGeometryManagerTool.h"

#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignPar.h"

#include "InDetAlignDBTools/SiTrkAlignDBTool.h"

#include "DataModel/DataVector.h"
#include "TFile.h"

#include <vector>

namespace InDet
{

//________________________________________________________________________
SiTrkAlignDBTool::SiTrkAlignDBTool(const std::string & type, const std::string & name, const IInterface * parent)
   : AthAlgTool(type,name,parent)
   , m_pixHelper(0)
   , m_sctHelper(0)
   , m_idHelper(0)
   , m_writeSQLFile(true)
   , m_SQLiteTag("test_tag")
   , m_outputAlignFile("OutputSiAlignment.txt")
   , m_outIBLDistFile("OutputIBLDist.txt")
   , m_outGlobalFolderFile("OutputSiGlobalFolder.txt")
   , m_writeTextFile(true)
   , m_writeOldConstants(false)
   , m_oldAlignFile("OldSiAlignment.txt")
   , m_oldIBLDistFile("OldIBLDist.txt")
   , m_oldGlobalFolderFile("OldSiGlobalFolder.txt")
   , m_updateConstants(true)
   , m_IDAlignDBTool("InDetAlignDBTool")
   , m_alignModuleTool("Trk::AlignModuleTool/AlignModuleTool")
   , m_pixelGeoManager("")
   , m_sctGeoManager("")
   , m_siGeoManager("")
   , m_pixelAlignLevel(-1)
   , m_pixelaAlignDBM(false)
   , m_sctAlignLevel(-1)
   , m_siAlignLevel(-1)
   , m_doSi(false)
   , m_doPixel(false)
   , m_doSCT(false)
   , m_writeAsL3(false)
   , m_writeAsL2(false)
   , m_writeAsL16(false)
{
  declareInterface<ITrkAlignDBTool>(this);

  declareProperty("WriteSQLFile",      m_writeSQLFile);
  declareProperty("SQLiteTag",         m_SQLiteTag);
  declareProperty("OutputTextFile",    m_outputAlignFile);
  declareProperty("OutputIBLDistFile", m_outIBLDistFile);
  declareProperty("OutputGlobalFolderFile",m_outGlobalFolderFile);
  declareProperty("WriteTextFile",     m_writeTextFile);
  declareProperty("WriteOldConstants", m_writeOldConstants);
  declareProperty("OldAlignFile",      m_oldAlignFile);
  declareProperty("OldIBLDistFile",    m_oldIBLDistFile);
  declareProperty("OldGlobalFolderFile",m_oldGlobalFolderFile);
  declareProperty("UpdateConstants",   m_updateConstants);
  declareProperty("WriteAsL3",         m_writeAsL3);

  declareProperty("IDAlignDBTool",     m_IDAlignDBTool);
  declareProperty("AlignModuleTool",   m_alignModuleTool, "Tool for handling of align module collections");
  declareProperty("AlignmentDBM",      m_pixelaAlignDBM);

  declareProperty("PixelGeometryManager", m_pixelGeoManager, "Tool for handling the Pixel alignment geometry");
  declareProperty("SCTGeometryManager",   m_sctGeoManager,   "Tool for handling the SCT alignment geometry");
  declareProperty("SiGeometryManager",    m_siGeoManager,    "Tool for handling the Silicon alignment geometry");
}

//________________________________________________________________________
SiTrkAlignDBTool::~SiTrkAlignDBTool()
{}

//________________________________________________________________________
StatusCode SiTrkAlignDBTool::initialize() {

  ATH_MSG_DEBUG("initialize() of SiTrkAlignDBTool");

  if(!m_updateConstants) {
    ATH_MSG_WARNING("Not updating Silicon constants!");
    return StatusCode::SUCCESS;
  }

  // Si alignment database tool
  if ( m_IDAlignDBTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL("SiTrkAlignDBTool::initialize() -> Can not retrieve IDAlignDBTool of type " << m_IDAlignDBTool.typeAndName());
    return StatusCode::FAILURE;
  }
  else
    ATH_MSG_INFO("Retrieved tool " << m_IDAlignDBTool.typeAndName());

  // get AlignModuleTool
  if( m_alignModuleTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL("SiTrkAlignDBTool::initialize() ->  Could not retrieve " << m_alignModuleTool);
    return StatusCode::FAILURE;
  }
  else
    ATH_MSG_INFO("retrieved " << m_alignModuleTool);

  // retrieve Pixel helper
  if ( detStore()->retrieve(m_pixHelper).isFailure() ) {
    ATH_MSG_FATAL("SiTrkAlignDBTool::initialize() ->  Cannot retrieve Pixel Helper ");
    return StatusCode::FAILURE;
  }
  else
    ATH_MSG_INFO("retrieved Pixel Helper");

  // retrieve SCT helper
  if ( detStore()->retrieve(m_sctHelper).isFailure() ) {
    ATH_MSG_FATAL("SiTrkAlignDBTool::initialize() -> Cannot retrieve SCT Helper ");
    return StatusCode::FAILURE;
  }
  else
    ATH_MSG_INFO("retrieved Silicon SCT Helper");

  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    ATH_MSG_FATAL("SiTrkAlignDBTool::initialize() -> Could not get AtlasDetectorID helper");
    return StatusCode::FAILURE;
  }

  // check which geometry managers are loaded and which are to be used

  // get SiGeometryManagerTool
  if( !m_siGeoManager.empty() ) {
    if( m_siGeoManager.retrieve().isFailure() ) {
      ATH_MSG_FATAL("SiTrkAlignDBTool::initialize() ->  Could not retrieve " << m_siGeoManager);
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("retrieved " << m_siGeoManager);
    m_siAlignLevel = m_siGeoManager->alignLevel();
    ATH_MSG_INFO(" Requested update of Level "<<m_siAlignLevel<<" alignment constants for Silicon");

    // check alignment level but don't stop on error
    // if unknown level given, set to -1
    if(!checkSiLevel()) {
      ATH_MSG_FATAL("SiTrkAlignDBTool::initialize() -> Storing of alignment constants for level "<<m_siAlignLevel<<" for Silicon is not implemented.");
      m_siAlignLevel=-1;
    }
  }

  // for anything else then level 0 we need the other two geometry managers
  if(m_siAlignLevel!=0) {

    // get PixelGeometryManagerTool
    if( !m_pixelGeoManager.empty() ) {
      if( m_pixelGeoManager.retrieve().isFailure() ) {
        ATH_MSG_FATAL("SiTrkAlignDBTool::initialize() -> Could not retrieve " << m_pixelGeoManager);
        return StatusCode::FAILURE;
      }

      ATH_MSG_INFO("retrieved " << m_pixelGeoManager);
      m_pixelAlignLevel = m_pixelGeoManager->alignLevel();
      m_pixelAlignLevelBarrel = m_pixelGeoManager->alignLevelBarrel();
      m_pixelAlignLevelEndcaps = m_pixelGeoManager->alignLevelEndcaps();

      ATH_MSG_DEBUG("Retrieved Pixel Global alignment level: "<<m_pixelAlignLevel);
      ATH_MSG_DEBUG("Retrieved Pixel Barrel alignment level: "<<m_pixelAlignLevelBarrel);
      ATH_MSG_DEBUG("Retrieved Pixel Endcap alignment level: "<<m_pixelAlignLevelEndcaps);
      ATH_MSG_DEBUG("Retrieved DBM alignment configuration: "<<m_pixelaAlignDBM);
      
      if(m_pixelAlignLevel == 1)
        ATH_MSG_INFO(" Requested update of Level "<<m_pixelAlignLevel<<" alignment constants for Pixel");
      else {
        ATH_MSG_INFO(" Requested update of Level "<<m_pixelAlignLevelBarrel<<" alignment constants for Pixel Barrel");
        ATH_MSG_INFO(" Requested update of Level "<<m_pixelAlignLevelEndcaps<<" alignment constants for Pixel Endcaps");
      }

       // check alignment level but don't stop on error
      if(!checkPixelLevel()) {
        ATH_MSG_FATAL("SiTrkAlignDBTool::initialize() ->  Pixel alignment level not given.");
      }
    }

    // get SCTGeometryManagerTool
    if( !m_sctGeoManager.empty() ) {
      if( m_sctGeoManager.retrieve().isFailure() ) {
        ATH_MSG_FATAL("SiTrkAlignDBTool::initialize() -> Could not retrieve " << m_sctGeoManager);
        return StatusCode::FAILURE;
      }

      ATH_MSG_INFO("retrieved " << m_sctGeoManager);
      m_sctAlignLevel = m_sctGeoManager->alignLevel();
      m_sctAlignLevelBarrel = m_sctGeoManager->alignLevelBarrel();
      m_sctAlignLevelEndcaps = m_sctGeoManager->alignLevelEndcaps();
      ATH_MSG_DEBUG("Retrieved SCT Global alignment level: "<<m_sctAlignLevel);
      ATH_MSG_DEBUG("Retrieved SCT Barrel alignment level: "<<m_sctAlignLevelBarrel);
      ATH_MSG_DEBUG("Retrieved SCT Endcap alignment level: "<<m_sctAlignLevelEndcaps);

      if(m_sctAlignLevel == 0)
        ATH_MSG_INFO(" Requested update of Level "<<m_sctAlignLevel<<" alignment constants for SCT");
      else {
        ATH_MSG_INFO(" Requested update of Level "<<m_sctAlignLevelBarrel<<" alignment constants for SCT Barrel");
        ATH_MSG_INFO(" Requested update of Level "<<m_sctAlignLevelEndcaps<<" alignment constants for SCT Endcaps");
      }

       // check alignment level but don't stop on error
      if(!checkSCTLevel()) {
        ATH_MSG_FATAL("SiTrkAlignDBTool::initialize() -> SCT alignment level not given.");
      }
    }

    // if we're here it means that we need at least one the two managers
    if(m_pixelAlignLevel<0 && m_pixelAlignLevelBarrel<0 && m_pixelAlignLevelEndcaps<0
       && m_sctAlignLevel<0 && m_sctAlignLevelBarrel<0 && m_sctAlignLevelEndcaps<0) {
      ATH_MSG_FATAL("SiTrkAlignDBTool::initialize() -> No geometry manager available or alignment level not given. ");
      // we don't want to stop if there's errors
      // so we comment out the return statement
//      return StatusCode::FAILURE;
    }
  }

  m_doSi = (m_siAlignLevel==0);
  m_doPixel = (checkPixelLevel() && !m_doSi);
  m_doSCT = (checkSCTLevel() && !m_doSi);

  if (m_writeAsL3)
    ATH_MSG_INFO(" Storing as level 3 constants.");

  if (m_writeAsL16)
    ATH_MSG_INFO(" Storing IBLDist as level 16 constants in separate DB");

  if(!m_doPixel && !m_doSCT && !m_doSi) {
    ATH_MSG_FATAL(" No geometry manager available or alignment level not given.");
    ATH_MSG_FATAL(" WARNING -> DB not updated.");
  }
  ATH_MSG_DEBUG("SiTrkAlignDBTool::initialize() -> Made it till here..... "<<m_doSi<<" "<<m_doPixel<<" "<<m_doSCT);
  ATH_MSG_DEBUG("update constants..... "<<m_updateConstants);
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
StatusCode SiTrkAlignDBTool::finalize()
{
  ATH_MSG_DEBUG("finalize() of SiTrkAlignDBTool");

  return StatusCode::SUCCESS;
}

//________________________________________________________________________
bool SiTrkAlignDBTool::checkPixelLevel()
{
  if(!m_pixelaAlignDBM && m_pixelAlignLevel==1)
    return true;

  if(!m_pixelaAlignDBM && m_pixelAlignLevel==11){// the new one with IBL (seems simplest solution)
    m_writeAsL2 = true;
    return true;
  }

  if(m_pixelaAlignDBM && (m_pixelAlignLevel==1 || m_pixelAlignLevel==2 )){// the new one with DBM (seems simplest solution)
    m_writeAsL2 = true;
    return true;
  }
  
  if(m_pixelaAlignDBM && m_pixelAlignLevel==3 ){// the new one with DBM (seems simplest solution)
    return true;
  }
  
  if(!m_pixelaAlignDBM && m_pixelAlignLevel==16){// the new IBL bowing level                   
    m_writeAsL16 = true; 
    // We need some fix here with respect to current CL implementation! There bowing is updated at L3!! (this would currently do both..)
    return true;
  }

  if (m_writeAsL3)
    return true;

  m_pixelAlignLevel = -1;

  bool ok = false;

  switch(m_pixelAlignLevelBarrel) {
    case 11: case 12: case 15: case 16: case 2: case 22: case 26: case 27: case 3:
      ok = true;
      ATH_MSG_INFO("Storing alignment constants for m_pixelAlignLevelBarrel as level "<< m_pixelAlignLevelBarrel);
      break;
    default:
      ATH_MSG_ERROR(" Storing of alignment constants for level "<<m_pixelAlignLevelBarrel<<" for Pixel Barrel is not implemented.");
      m_pixelAlignLevelBarrel = -1;
      ok = false;
      break;
  }

  switch(m_pixelAlignLevelEndcaps) {
    case 11: case 16: case 2: case 3: case 12:
      ok = ok && true;
      break;
    default:
      ATH_MSG_ERROR(" Storing of alignment constants for level "<<m_pixelAlignLevelEndcaps<<" for Pixel Endcaps is not implemented.");
      m_pixelAlignLevelEndcaps = -1;
      ok = false;
      break;
  }

  return ok;
}

//________________________________________________________________________
bool SiTrkAlignDBTool::checkSCTLevel()
{
  if(m_sctAlignLevel==0)
    return true;

  if (m_writeAsL3)
    return true;

  m_sctAlignLevel = -1;

  bool ok = false;

  switch(m_sctAlignLevelBarrel) {
  case 1: case 2: case 25: case 27: case 3:
    ok = true;
    break;
  default:
    ATH_MSG_ERROR("SiTrkAlignDBTool -> Storing of alignment constants for level "<<m_sctAlignLevelBarrel<<" for SCT Barrel is not implemented.");
    m_sctAlignLevelBarrel = -1;
    ok = false;
    break;
  }

  switch(m_sctAlignLevelEndcaps) {
  case 1: case 2: case 25: case 3:
    ok = ok && true;
    break;
  default:
    ATH_MSG_ERROR("SiTrkAlignDBTool -> Storing of alignment constants for level "<<m_sctAlignLevelEndcaps<<" for SCT Endcaps is not implemented.");
    m_sctAlignLevelEndcaps = -1;
    ok = false;
    break;
  }

  return ok;
}

//________________________________________________________________________
bool SiTrkAlignDBTool::checkSiLevel()
{
  if (m_writeAsL3)
    return true;

  switch(m_siAlignLevel) {
    case 0:
    case 1:
    case 2:
    case 3:
      return true;

    default:
      return false;
  }
}

//________________________________________________________________________
void SiTrkAlignDBTool::writeAlignPar()
{
  ATH_MSG_DEBUG("in SiTrkAlignDBTool::writeAlignPar()");

  // do we want to update the DB?
  if(!m_updateConstants) {
    ATH_MSG_INFO("Update of Silicon constants not requested!");
    return;
  }

  // write initial alignment constants
  if(m_writeOldConstants) {
    ATH_MSG_INFO("Writing old Silicon alignment constants to file "<<m_oldAlignFile);
    m_IDAlignDBTool->writeFile(false, m_oldAlignFile);
    m_IDAlignDBTool->writeIBLDistFile(m_oldIBLDistFile);
    m_IDAlignDBTool->writeGlobalFolderFile(m_oldGlobalFolderFile);
  }

  // now update the DB
  updateDB();

  // and write the final alignment constants
  if(m_writeTextFile) {
    ATH_MSG_INFO("Writing Silicon alignment constants to file "<<m_outputAlignFile);
    m_IDAlignDBTool->writeFile(false, m_outputAlignFile);
    m_IDAlignDBTool->writeIBLDistFile(m_outIBLDistFile);
    m_IDAlignDBTool->writeGlobalFolderFile(m_outGlobalFolderFile);
  }

  return;
}

//________________________________________________________________________
void SiTrkAlignDBTool::updateDB()
{
  ATH_MSG_DEBUG(" in SiTrkAlignDBTool::updateDB()");

  Amg::Transform3D dbtransform;

  ATH_MSG_INFO("-------------------------------------------------");
  // updating level 1, level 2 or level 3 constants in the DB
  if(m_doSi)
    ATH_MSG_INFO("Updating Level "<<m_siAlignLevel<<" alignment constants for Silicon");

  if(m_doPixel) {
    if(m_pixelAlignLevel==1)
      ATH_MSG_INFO("Updating Level "<<m_pixelAlignLevel<<" alignment constants for Pixel");
    else {
      ATH_MSG_INFO("Updating Level "<<m_pixelAlignLevelBarrel<<" alignment constants for Pixel Barrel");
      ATH_MSG_INFO("Updating Level "<<m_pixelAlignLevelEndcaps<<" alignment constants for Pixel Endcaps");
    }
  }

  if(m_doSCT) {
    if(m_sctAlignLevel==0)
      ATH_MSG_INFO("Updating Level "<<m_sctAlignLevel<<" alignment constants for SCT");
    else {
      ATH_MSG_INFO("Updating Level "<<m_sctAlignLevelBarrel<<" alignment constants for SCT Barrel");
      ATH_MSG_INFO("Updating Level "<<m_sctAlignLevelEndcaps<<" alignment constants for SCT Endcaps");
    }
  }

  if (m_writeAsL3)
    ATH_MSG_INFO("Updating Level 3 constants.");

  // loop over all AlignModules
  std::vector<Trk::AlignModule *>::const_iterator imod     = m_alignModuleTool->getAlignModuleCollection()->begin();
  std::vector<Trk::AlignModule *>::const_iterator imod_end = m_alignModuleTool->getAlignModuleCollection()->end();
  for( ; imod != imod_end; ++imod ) {

    Trk::AlignModule * module = *imod;

    ATH_MSG_DEBUG("-------------------------------------------------------");
    ATH_MSG_DEBUG("Alignment corrections for module: "<<module->name());

    // the the identifier of the module for the DB
    Identifier modID = module->identify();

    // The Si AlignModules of levels 0, 1, 2 and 3 use the Silicon Identifier.
    if(!m_idHelper->is_pixel(modID) && !m_idHelper->is_sct(modID)) {
      ATH_MSG_DEBUG("Module with ID: "<<modID<<" is not a Silicon module. Skipping.");
      continue;
    }

    // get active alignment parameters for the module
    DataVector<Trk::AlignPar> * alignPars =  m_alignModuleTool->getAlignPars(module);
    if(alignPars->size() == 0) {
      ATH_MSG_INFO("Alignment constants for module "<<module->name()<<" not available.");
      continue;
    }

    ATH_MSG_DEBUG("Retrieved alignment constants for module "<<module->name());
    ATH_MSG_DEBUG("Module ID : "<<modID);
    ATH_MSG_DEBUG("Expanded module ID : "<<m_idHelper->show_to_string(modID,0,'/'));

    // get final alignment parameters
    DataVector<Trk::AlignPar> * fullAlignPars = m_alignModuleTool->getFullAlignPars(module);
    double apTraX = fullAlignPars->at(Trk::AlignModule::TransX)->par();
    double apTraY = fullAlignPars->at(Trk::AlignModule::TransY)->par();
    double apTraZ = fullAlignPars->at(Trk::AlignModule::TransZ)->par();
    double apRotX = fullAlignPars->at(Trk::AlignModule::RotX)->par();
    double apRotY = fullAlignPars->at(Trk::AlignModule::RotY)->par();
    double apRotZ = fullAlignPars->at(Trk::AlignModule::RotZ)->par();
    
    // Need to make sure not to call SCT modules - only defined for PIX 
    double apBowX = 0;
    // This should be sufficient as bowing-DoF is only enabled for IBL
    if(m_idHelper->is_pixel(modID) && m_pixHelper->is_barrel(modID) ){
      apBowX = fullAlignPars->at(Trk::AlignModule::BowX)->par();
    
      // The last check ensures that it also updates for L11 in case bowing DoF is enables.
      if(m_pixelAlignLevelBarrel==16 || m_writeAsL16 || apBowX!=0){     
	updateAsL16(module,apBowX);  
	// Can we get a baseline as well? include here in case it is possible!
	// Allow other DoF to be updated as well!
	// Initial CL (Oct. 2016) update at L3 --> commented for now
      }
    }
    
    Amg::Translation3D translation( apTraX, apTraY, apTraZ );
    Amg::Transform3D transform = translation * Amg::RotationMatrix3D::Identity();
    transform *= Amg::AngleAxis3D(apRotZ, Amg::Vector3D(0.,0.,1.));
    transform *= Amg::AngleAxis3D(apRotY, Amg::Vector3D(0.,1.,0.));
    transform *= Amg::AngleAxis3D(apRotX, Amg::Vector3D(1.,0.,0.));
    
    
    ATH_MSG_DEBUG(" - translation X = "<<apTraX);
    ATH_MSG_DEBUG(" - translation Y = "<<apTraY);
    ATH_MSG_DEBUG(" - translation Z = "<<apTraZ);
    ATH_MSG_DEBUG(" - rotation X    = "<<apRotX);
    ATH_MSG_DEBUG(" - rotation Y    = "<<apRotY);
    ATH_MSG_DEBUG(" - rotation Z    = "<<apRotZ);

    int level=-1;

    // this is really ugly and quite confusing !!!
    // but will do for now...
    if(m_idHelper->is_pixel(modID)) {
      if(!m_doPixel && !m_doSi) {
        ATH_MSG_INFO("--> Pixel module - NOT storing");
        continue;
      }
      if(m_doPixel) {
        if(m_pixelAlignLevel==1 || m_pixelAlignLevel==2 || m_pixelAlignLevel==3)
          level = m_pixelAlignLevel;
        else if(m_pixHelper->is_barrel(modID))
          level = m_pixelAlignLevelBarrel;
        else
          level = m_pixelAlignLevelEndcaps;
      }
      else // this makes sure that Si L0 is handled since pixel identifier is used for it
        level = m_siAlignLevel;
    }
    else if(m_idHelper->is_sct(modID)) {
      if(!m_doSCT) {
        ATH_MSG_INFO("--> SCT module - NOT storing");
        continue;
      }
      if(m_sctAlignLevel==0)
        level = m_sctAlignLevel;
      else if(m_sctHelper->is_barrel(modID))
        level = m_sctAlignLevelBarrel;
      else
        level = m_sctAlignLevelEndcaps;
    }
    // to better understand what is going on
    ATH_MSG_DEBUG("We are having level: "<<level);

    if (m_writeAsL3) {
      updateAsL3(module,transform,apBowX);
      continue;
    }

    if (m_idHelper->is_pixel(modID) && m_writeAsL2) {
      level=2;
      updateAsL2(module,transform,level);
      continue;
    }
    
    // calculate the DB transform
    switch(level) {

      case -1:
        // level not recognized, do nothing
        continue;

      case 0:
        updateSiL0asL1(modID,transform);
        continue;

      case 1: case 2: {
        // for levels 1 and 2 the DB frame equals to the global frame but the align
        // frame doesnot, so we have to apply an additional transform
        Amg::Transform3D dbFrameToAlignFrame = module->globalFrameToAlignFrame();

        ATH_MSG_DEBUG("DB to align");
        printTransform(dbFrameToAlignFrame);

        dbtransform = dbFrameToAlignFrame.inverse() * transform * dbFrameToAlignFrame;

        ATH_MSG_DEBUG("DB transform");
        printTransform(dbtransform);
        break;
      }

      case 3:
        // for level 3 alignment the DB frame and the align frame equals to the local frame
        // so we don't apply any additional transform
        dbtransform = transform;

        ATH_MSG_DEBUG("DB transform");
        printTransform(dbtransform);
        break;

      default:
        // otherwise we split the AlignModule into level 3 modules
        ATH_MSG_DEBUG("We are doing this one");
        updateAsL3(module,transform,apBowX);
        continue;
    }

    // tweak applies the transform onto already existing transform in the DB
    if( m_IDAlignDBTool->tweakTrans(modID, level, dbtransform) )
      ATH_MSG_INFO("Module \'"<<module->name()<<"\': Level "<<level<<" constants updated.");
    else {
      // set replaces the transform in the DB
      ATH_MSG_DEBUG("Module \'"<<module->name()<<"\': Call to tweak failed. Constants probably don't exist yet. Use set instead.");
      if( m_IDAlignDBTool->setTrans(modID, level, dbtransform) )
        ATH_MSG_INFO("Module \'"<<module->name()<<"\': Level "<<level<<" constants updated.");
      else
        ATH_MSG_ERROR("Error setting level "<<level<<" constants for module "<<module->name());
    }

  }
  ATH_MSG_INFO("-------------------------------------------------");

}

//________________________________________________________________________
void SiTrkAlignDBTool::updateSiL0asL1(Identifier idL0, const Amg::Transform3D & transform)
{
  // splitting the whole Silicon into Pixel, SCT barrel and 2 SCT endcaps
  // the id has to be equal to L1 Pixel ID for Silicon level 0 update
  // or to SCT barrel L1 ID for SCT level 0 update
  if( idL0 != m_pixHelper->wafer_id(0,0,0,0) && idL0 != m_sctHelper->wafer_id(0,0,0,0,0) ) {
    ATH_MSG_ERROR("wrong identifier for L0 Silicon in updateSiL0asL1()");
    return;
  }

  // for both L0 and L1 the global frame is equal to the DB frame
  // for L0 it is also equal to align frame so no additional transform is needed

  if(idL0 == m_pixHelper->wafer_id(0,0,0,0)) {
    ATH_MSG_INFO("Splitting Silicon into L1 modules and updating in the DB");

    // Pixel
    Identifier idPixel = m_pixHelper->wafer_id(0,0,0,0);
    ATH_MSG_INFO("Updating L1 constants for Pixel");
    // tweak applies the transform onto already existing transform in the DB
    if( m_IDAlignDBTool->tweakTrans(idPixel, 1, transform) )
      ATH_MSG_INFO("Call to tweak OK. Constants updated.");
    else {
      // set replaces the transform in the DB
      ATH_MSG_INFO("Call to tweak failed. Constants probably don't exist yet. Use set instead.");
      if( !m_IDAlignDBTool->setTrans(idPixel, 1, transform) )
        ATH_MSG_ERROR("Error setting constants.");
    }
  }
  else
    ATH_MSG_INFO("Splitting SCT into L1 modules and updating in the DB");

  // SCT
  std::vector<Identifier> ids;
  ids.push_back(m_sctHelper->wafer_id(0,0,0,0,0));  // SCT barrel
  ids.push_back(m_sctHelper->wafer_id(-2,0,0,0,0)); // SCT endcap A
  ids.push_back(m_sctHelper->wafer_id(2,0,0,0,0));  // SCT endcap C

  ATH_MSG_INFO("Updating L1 constants for SCT");
  for(unsigned int i=0; i<ids.size(); ++i) {
    // tweak applies the transform onto already existing transform in the DB
    if( m_IDAlignDBTool->tweakTrans(ids[i], 1, transform) )
      ATH_MSG_INFO("Module "<<i<<". - id "<<ids[i]<<": Call to tweak OK. Constants updated.");
    else {
      // set replaces the transform in the DB
      ATH_MSG_INFO("Module "<<i<<". - id "<<ids[i]<<": Call to tweak failed. Constants probably don't exist yet. Use set instead.");
      if( !m_IDAlignDBTool->setTrans(ids[i], 1, transform) )
        ATH_MSG_ERROR("Error setting constants.");
    }
  }
}

//_________________________________________________________________________________________
void SiTrkAlignDBTool::updateAsL3(const Trk::AlignModule * module, const Amg::Transform3D & transform, double apBowX)
{
  ATH_MSG_INFO("Updating constants for module "<<module->name()<<" as level 3 constants");

  // store as level 3 constants
  const int level = 3;

  // transform from global frame to align frame
  Amg::Transform3D globaltoalign = module->globalFrameToAlignFrame();

  // same code pplies to both Pixel and SCT
  for (unsigned int idet=0; idet<2; idet++) {

    Trk::AlignModule::DetectorType det = idet ? Trk::AlignModule::SCT : Trk::AlignModule::Pixel;

    ATH_MSG_DEBUG("getting elements of type "<<det<<" from the module");

    const std::vector<const Trk::TrkDetElementBase *> * elements = module->detElementCollection(det);

    if( !elements ) {
      ATH_MSG_DEBUG("no elements of type "<<det);
      continue;
    }

    ATH_MSG_DEBUG("looping over "<<elements->size()<<" elements");

    std::vector<const Trk::TrkDetElementBase*>::const_iterator ielem     = elements->begin();
    std::vector<const Trk::TrkDetElementBase*>::const_iterator ielem_end = elements->end();
    for ( ; ielem != ielem_end; ++ielem) {
      const InDetDD::SiDetectorElement * sielem = dynamic_cast<const InDetDD::SiDetectorElement *>(*ielem);
      if (!sielem) {
        ATH_MSG_WARNING("Should be Silicon detector element but is not. Skipping.");
        continue;
      }  

      Identifier elemID = sielem->identify();

      // for the SCT the 0 side Identifier is used in the DB
      if (m_idHelper->is_sct(elemID)) {
        if (m_sctHelper->side(elemID) > 0)
          continue;
      }

      ATH_MSG_DEBUG("-----------------------------------------------------");

      ATH_MSG_DEBUG("ModuleID value: "<<elemID);
      ATH_MSG_DEBUG("ModuleID: "<<m_idHelper->show_to_string(elemID,0,'/'));

      // this should work for both Pixel and SCT
      // module transform is the transform between the db frame and global frame
      Amg::Transform3D dbtoglobal = sielem->moduleTransform();
      //      ATH_MSG_DEBUG("DB to Global");
      //      printTransform(dbtoglobal);

      //      ATH_MSG_DEBUG("Global to Align");
      //      printTransform(globaltoalign);

      // from DB frame to alignment frame transform
      Amg::Transform3D dbtoalign = globaltoalign * dbtoglobal;
      //      ATH_MSG_DEBUG("DB to Align");
      //      printTransform(dbtoalign);

      ATH_MSG_DEBUG("Alignment transform");
      printTransform(transform);

      // alignment transform in DB frame
      Amg::Transform3D dbtransform = dbtoalign.inverse() * transform * dbtoalign; 

      // This should work as Bowing is in DB frame, i.e. local module frame
      // This implementation is need in the CL as of Oct2015; 
      // For now, just comment for offline
      if (apBowX!=0){
	double z = sielem->center()[2];        
	const double  y0y0  = 366.5*366.5;
	double bowx = apBowX * ( z*z - y0y0 ) / y0y0;
	ATH_MSG_DEBUG("Z position: "<< z << "   apBowX:" << apBowX << "    bowx: " << bowx);
	// This is in the module frame, as bowing corrections are directly L3
	// Amg::Translation3D translation_bow(bowx , 0, 0 );
	// dbtransform *= translation_bow;
      }
      
      ATH_MSG_DEBUG("DB transform");
      printTransform(dbtransform);

      // tweak applies the transform onto already existing transform in the DB
      if( !(m_IDAlignDBTool->tweakTrans(elemID, level, dbtransform)) ) {
        // set replaces the transform in the DB
        if( !(m_IDAlignDBTool->setTrans(elemID, level, dbtransform)) )
          ATH_MSG_ERROR("Error setting level "<<level<<" constants for element "<<elemID<<" in module "<<module->name());
      }
    } // end loop over detElements
    ATH_MSG_DEBUG("-----------------------------------------------------");
  } // end loop over Pixel and SCT
}

  //_________________________________________________________________________________________
void SiTrkAlignDBTool::updateAsL2(const Trk::AlignModule * module, const Amg::Transform3D & transform, const int level)
{
  ATH_MSG_INFO("Updating constants for module "<<module->name()<<" as level 2 constants (new)");
    
  // check the level we store in
  ATH_MSG_INFO("DB aligning at level: "<<level);
  
  Trk::AlignModule::DetectorType det = Trk::AlignModule::Pixel;
  const std::vector<const Trk::TrkDetElementBase *> * elements = module->detElementCollection(det);

  if( !elements ) {
    ATH_MSG_FATAL("no elements of type "<<det);
    return;
  }

  ATH_MSG_DEBUG("looping over "<<elements->size()<<" elements");
  std::vector<std::string> level_mods; // where we store the unique identifiers we want to update
    
  std::vector<const Trk::TrkDetElementBase*>::const_iterator ielem     = elements->begin();
  std::vector<const Trk::TrkDetElementBase*>::const_iterator ielem_end = elements->end();
  for ( ; ielem != ielem_end; ++ielem) {
    const InDetDD::SiDetectorElement * sielem = dynamic_cast<const InDetDD::SiDetectorElement *>(*ielem);
    if (!sielem) {
      ATH_MSG_WARNING("Should be Silicon detector element but is not. Skipping.");
      continue;
    }
    
    Identifier elemID = sielem->identify();
    Identifier level_mod = GetL1L2fromL3Identifier(elemID,level);
    std::string key = m_IDAlignDBTool->dirkey(level_mod,level+1);
    
    ATH_MSG_DEBUG("TESTING value: "<<level_mod);
    ATH_MSG_DEBUG("TESTING ID: "<<m_idHelper->show_to_string(level_mod,0,'/'));
    ATH_MSG_DEBUG("TESTING retrieved key: "<<key);
    std::vector<std::string>::const_iterator ix = find(level_mods.begin(),level_mods.end(),key); // check whether it is unique
    if (ix==level_mods.end()) {
      level_mods.push_back(key);
      
      ATH_MSG_DEBUG("-----------------------------------------------------");
      
      ATH_MSG_DEBUG("ModuleID value: "<<level_mod);
      ATH_MSG_DEBUG("ModuleID: "<<m_idHelper->show_to_string(level_mod,0,'/'));
      
      // for levels 1 and 2 the DB frame equals to the global frame but the align
      // frame doesnot, so we have to apply an additional transform
      Amg::Transform3D dbFrameToAlignFrame = module->globalFrameToAlignFrame();
        
      ATH_MSG_DEBUG("DB to align");
      printTransform(dbFrameToAlignFrame);
        
      Amg::Transform3D dbtransform = dbFrameToAlignFrame.inverse() * transform * dbFrameToAlignFrame;
        
      ATH_MSG_DEBUG("DB transform");
      printTransform(dbtransform);
        
        
      // tweak applies the transform onto already existing transform in the DB
      if( !(m_IDAlignDBTool->tweakTrans(level_mod, level, dbtransform)) ) {
        // set replaces the transform in the DB
        if( !(m_IDAlignDBTool->setTrans(level_mod, level, dbtransform)) )
          ATH_MSG_INFO("Error setting level "<<level<<" constants for element "<<level_mod<<" in module "<<module->name() );
      }
    }
    else {
      ATH_MSG_DEBUG("Skipping ModuleID: "<<m_idHelper->show_to_string(elemID,0,'/')<<" --> not unique");
    }
  } // end loop over detElements
}
  

//_________________________________________________________________________________________                                                                                    
void SiTrkAlignDBTool::updateAsL16(const Trk::AlignModule * module, double bowx)
{
  ATH_MSG_INFO("Updating constants for module "<<module->name()<<" as level 16 IBLDist constants (new)");

  Trk::AlignModule::DetectorType det = Trk::AlignModule::Pixel;
  const std::vector<const Trk::TrkDetElementBase *> * elements = module->detElementCollection(det);

  if( !elements ) {
    ATH_MSG_FATAL("no elements of type "<<det);
    return;
  }

  ATH_MSG_DEBUG("looping over "<<elements->size()<<" elements");
  std::vector<int> stave_phis; // where we store the unique identifiers we want to update                                                                                
  std::vector<const Trk::TrkDetElementBase*>::const_iterator ielem     = elements->begin();
  std::vector<const Trk::TrkDetElementBase*>::const_iterator ielem_end = elements->end();
  for ( ; ielem != ielem_end; ++ielem) {
    const InDetDD::SiDetectorElement * sielem = dynamic_cast<const InDetDD::SiDetectorElement *>(*ielem);
    if (!sielem) {
      ATH_MSG_WARNING("Should be Silicon detector element but is not. Skipping.");
      continue;
    }

    Identifier elemID = sielem->identify();
    const int this_stave_phi = m_pixHelper->phi_module( elemID );

    ATH_MSG_DEBUG("module ID: "<<m_idHelper->show_to_string(elemID,0,'/'));
    ATH_MSG_DEBUG("phi identifier: "<<m_pixHelper->phi_module( elemID )) ;    

    std::vector<int>::const_iterator ix = find(stave_phis.begin(),stave_phis.end(),this_stave_phi); // check whether it is unique
    if (ix==stave_phis.end()) {
      stave_phis.push_back(this_stave_phi);

      // tweak applies the transform onto already existing transform in the DB                                              
      if( !(m_IDAlignDBTool->tweakIBLDist(this_stave_phi,bowx)) ) {
	ATH_MSG_INFO("Error tweaking IBLDist DB for stave "<<this_stave_phi);
      }
    }
    else {
      ATH_MSG_DEBUG("Skipping ModuleID: "<<m_idHelper->show_to_string(elemID,0,'/')<<" --> not unique");
    }
    
  } // end loop over detElements                                                                                                                                                 
}

    
//_________________________________________________________________________________________
  void SiTrkAlignDBTool::printTransform(const Amg::Transform3D & tr) const
  {
    ATH_MSG_DEBUG(" - translation: "<<tr.translation().x()<<"  "<<tr.translation().y()<<"  "<<tr.translation().z());
    ATH_MSG_DEBUG(" - rotation:");
    ATH_MSG_DEBUG("      "<<tr(0,0)<<"  "<<tr(0,1)<<"  "<<tr(0,2));
    ATH_MSG_DEBUG("      "<<tr(1,0)<<"  "<<tr(1,1)<<"  "<<tr(1,2));
    ATH_MSG_DEBUG("      "<<tr(2,0)<<"  "<<tr(2,1)<<"  "<<tr(2,2));
    return;
  }

//_________________________________________________________________________________________
/** convert L3 module identifier to L1 or L2 */
Identifier SiTrkAlignDBTool::GetL1L2fromL3Identifier( const Identifier& ident, const int& level) const {
  if( level == 3 ) return ident ; //!< no translation needed
  /** check whether PIX */
  if( m_pixHelper->is_pixel(ident) ) {
    if( level == 1 ) {
      return m_pixHelper->wafer_id( 0, 0, 0, 0 ) ; //!< Whole pixel det. at L1
    }
    if( level == 2 ) {
      int barrel_ec  = m_pixHelper->barrel_ec(  ident ) ;
      int layer_disk = m_pixHelper->layer_disk( ident ) ;
      int phi_sector = m_pixHelper->phi_module( ident ) ;
      if (abs(barrel_ec)==4) return m_pixHelper->wafer_id( barrel_ec, layer_disk, phi_sector, 0 ) ;
      else return m_pixHelper->wafer_id( barrel_ec, layer_disk, 0, 0 ) ;
    }
  }
  /** check whether SCT */
  if( m_sctHelper->is_sct(ident) ) {
    if( level == 1 ) {
      int barrel_ec  = m_sctHelper->barrel_ec(  ident ) ;
      return m_sctHelper->wafer_id( barrel_ec, 0, 0, 0, 0 ) ; //!< barrel + 2 x EC at L1
    }
    if( level == 2 ) {
      int barrel_ec  = m_sctHelper->barrel_ec(  ident ) ;
      int layer_disk = m_sctHelper->layer_disk( ident ) ;
      return m_sctHelper->wafer_id( barrel_ec, layer_disk, 0, 0, 0 ) ;
    }
  }
  return ident ; //!< take care of the case where level != 1,2,3 or ident neither pix nor sct
}
  
  
  

} // end namespace
