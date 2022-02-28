/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetIdentifier/TRT_ID.h"
#include "InDetIdentifier/PixelID.h"

#include "TRT_ConditionsServices/ITRT_AlignDbSvc.h"

#include "TrkAlignInterfaces/IAlignModuleTool.h"
#include "TrkAlignInterfaces/IGeometryManagerTool.h"

#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignPar.h"

#include "InDetAlignDBTools/TRTTrkAlignDBTool.h"

#include "AthContainers/DataVector.h"
#include "TFile.h"

#include <vector>
#include <cmath>

namespace InDet
{

//________________________________________________________________________
TRTTrkAlignDBTool::TRTTrkAlignDBTool(const std::string & type, const std::string & name, const IInterface * parent)
   : AthAlgTool(type,name,parent)
   , m_trtHelper(nullptr)
   , m_pixHelper(nullptr)
   , m_outputAlignFile("OutputTRTAlignment.txt")
   , m_outputStrawAlignFile("OutputTRTStrawAlignment.txt")
   , m_oldAlignFile("OldTRTAlignment.txt")
   , m_oldStrawAlignFile("OldTRTStrawAlignment.txt")
   , m_writeTextFile(true)
   , m_writeOldConstants(false)
   , m_updateConstants(true)
   , m_trtAlignDbSvc("TRT_AlignDbSvc",name)
   , m_alignModuleTool("Trk::AlignModuleTool/AlignModuleTool")
   , m_trtGeoManager("")
   , m_trtAlignLevel(-1)
   , m_trtAlignLevelBarrel(-1)
   , m_trtAlignLevelEndcaps(-1)
   , m_ntuple{}
{
   declareInterface<ITrkAlignDBTool>(this);

   declareProperty("OutputTextFile",       m_outputAlignFile,   "Text file to write the final alignment constants");
   declareProperty("OutputStrawTextFile",  m_outputStrawAlignFile,   "Text file to write the final straw alignment constants");
   declareProperty("WriteTextFile",        m_writeTextFile,     "Whether or not to write the final/new alignment constants to text file");
   declareProperty("WriteOldConstants",    m_writeOldConstants, "Whether or not to write the initial/old alignment constants to text file");
   declareProperty("OldAlignFile",         m_oldAlignFile,      "Text file to write the initial/old alignment constants");
   declareProperty("OldStrawAlignFile",    m_oldStrawAlignFile, "Text file to write the initial/old straw alignment constants");
   declareProperty("TrtAlignDbSvc",        m_trtAlignDbSvc,     "Service for interaction with the TRT alignment DB");
   declareProperty("AlignModuleTool",      m_alignModuleTool,   "Tool for handling of align module collections");
   declareProperty("TRTGeometryManager",   m_trtGeoManager,     "Tool for handling the TRT alignment geometry");
   declareProperty("UpdateConstants",      m_updateConstants,   "Whether or no to update the alignment constants");

   m_logStream = nullptr;
}

//________________________________________________________________________
TRTTrkAlignDBTool::~TRTTrkAlignDBTool()
{}

//________________________________________________________________________
StatusCode TRTTrkAlignDBTool::initialize()
{
   ATH_MSG_DEBUG("initialize() of TRTTrkAlignDBTool");

   if(!m_updateConstants) {
      ATH_MSG_WARNING("Not updating TRT constants!");
      return StatusCode::SUCCESS;
   }
   // get TRT db service
   ATH_CHECK( m_trtAlignDbSvc.retrieve() );
   // get AlignModuleTool
   ATH_CHECK( m_alignModuleTool.retrieve());
   // get TRT ID Helper
   ATH_CHECK( detStore()->retrieve(m_trtHelper, "TRT_ID"));
   // get Pixel ID Helper
   ATH_CHECK( detStore()->retrieve(m_pixHelper) );

   // get TRTGeometryManagerTool
   if( !m_trtGeoManager.empty() ) {
      ATH_CHECK( m_trtGeoManager.retrieve());
      ATH_MSG_DEBUG("retrieved " << m_trtGeoManager);
      m_trtAlignLevel = m_trtGeoManager->alignLevel();
      m_trtAlignLevelBarrel = m_trtGeoManager->alignLevelBarrel();
      m_trtAlignLevelEndcaps = m_trtGeoManager->alignLevelEndcaps();
      ATH_MSG_DEBUG("Retrieved TRT Global alignment level: "<<m_trtAlignLevel);
      ATH_MSG_DEBUG("Retrieved TRT Barrel alignment level: "<<m_trtAlignLevelBarrel);
      ATH_MSG_DEBUG("Retrieved TRT Endcap alignment level: "<<m_trtAlignLevelEndcaps);
      if(m_trtAlignLevelEndcaps==200) {
         ATH_MSG_WARNING("Requested update of TRT Endcap constants for the old scheme.");
         ATH_MSG_WARNING("This will only work with \'alignString = \"ALold\"\' option of TRT_AlignDbSvc.");
         m_trtAlignLevelEndcaps=2;
      }
   }

   if(m_trtAlignLevel == 0)
      ATH_MSG_INFO(" Requested update of Level "<<m_trtAlignLevel<<" alignment constants for TRT");
   else {
      ATH_MSG_INFO(" Requested update of Level "<<m_trtAlignLevelBarrel<<" alignment constants for TRT Barrel");
      ATH_MSG_INFO(" Requested update of Level "<<m_trtAlignLevelEndcaps<<" alignment constants for TRT Endcaps");
   }

   // check alignment level but don't stop on error
   // if unknown level given, set to -1
   if(!checkAlignLevel()) {
      ATH_MSG_FATAL( " No geometry manager available or alignment level not given. " );
      // we don't want to stop if there's errors
      // so we comment out the return statement
//      return StatusCode::FAILURE;
  }
   return StatusCode::SUCCESS;
}

//________________________________________________________________________
StatusCode TRTTrkAlignDBTool::finalize()
{
   ATH_MSG_DEBUG("finalize() of TRTTrkAlignDBTool");

   return StatusCode::SUCCESS;
}

//________________________________________________________________________
bool TRTTrkAlignDBTool::checkAlignLevel()
{
   if(m_trtAlignLevel == 0)
      return true;

   m_trtAlignLevel = -1;

   bool ok=false;

   switch(m_trtAlignLevelBarrel) {
      case 1: case 2: case 3:
         ok = true;
         break;
      default:
         ATH_MSG_ERROR(" Storing of alignment constants for level "<<m_trtAlignLevelBarrel<<" for TRT Barrel is not implemented.");
         m_trtAlignLevelBarrel = -1;
         ok = false;
         break;
   }

   switch(m_trtAlignLevelEndcaps) {
      case 1: case 2: case 3:
         ok = ok && true;
         break;
      default:
         ATH_MSG_ERROR(" Storing of alignment constants for level "<<m_trtAlignLevelEndcaps<<" for TRT Endcaps is not implemented.");
         m_trtAlignLevelEndcaps = -1;
         ok = false;
         break;
   }

   return ok;
}

//________________________________________________________________________
void TRTTrkAlignDBTool::writeAlignPar()
{
   ATH_MSG_DEBUG("in TRTTrkAlignDBTool::writeAlignPar()");

   // do we want to update the DB?
   if(!m_updateConstants) {
      ATH_MSG_INFO("Update of TRT constants not requested!");
      return;
   }

   // write initial alignment constants
   if(m_writeOldConstants) {
      ATH_MSG_INFO("Writing old TRT alignment constants to file "<<m_oldAlignFile);
      if( m_trtAlignDbSvc -> writeAlignTextFile(m_oldAlignFile) != StatusCode::SUCCESS )
         ATH_MSG_ERROR("Cannot write to file "<<m_oldAlignFile);
      if( m_trtAlignDbSvc -> writeStrawAlignTextFile(m_oldStrawAlignFile) != StatusCode::SUCCESS )
         ATH_MSG_ERROR("Cannot write to file "<<m_oldStrawAlignFile);
   }

   // now update the DB
   updateDB();

   // and write the final alignment constants
   if(m_writeTextFile) {
     if( m_trtAlignDbSvc->writeAlignTextFile(m_outputAlignFile) != StatusCode::SUCCESS )
         ATH_MSG_ERROR("Cannot write to file "<<m_outputAlignFile);
      else
         ATH_MSG_INFO("Wrote final TRT alignment constants into "<<m_outputAlignFile);

     if( m_trtAlignDbSvc->writeStrawAlignTextFile(m_outputStrawAlignFile) != StatusCode::SUCCESS )
        ATH_MSG_ERROR("Cannot write to file "<<m_outputStrawAlignFile);
     else
        ATH_MSG_INFO("Wrote final TRT Straw alignment constants into "<<m_outputStrawAlignFile);
   }

   return;
}

//________________________________________________________________________
void TRTTrkAlignDBTool::updateDB()
{
   ATH_MSG_DEBUG(" in TRTTrkAlignDBTool::updateDB()");

   // There are 3 sets of alignment constants in the DB for the TRT
   // L0 : whole TRT
   // L1 : barrel + 2 endcaps
   // L2 : barrel modules and endcap discs
   //
   // If we're doing alignment of the level between these, the constants
   // of the closest higher level have to be updated.
   // This is not allways necessary for all alignmodules since some of them
   // can be defined to be identical with modules at lower alignment level,
   // but we keep it like this for the moment.

   // updating level 0, level 1 or level 2 constants in the DB
   ATH_MSG_DEBUG("Global alignment level: "<<m_trtAlignLevel);
   ATH_MSG_DEBUG("Barrel alignment level: "<<m_trtAlignLevelBarrel);
   ATH_MSG_DEBUG("Endcap alignment level: "<<m_trtAlignLevelEndcaps);

   if(m_trtAlignLevel<0 && (m_trtAlignLevelBarrel<0 || m_trtAlignLevelEndcaps<0)) {
      ATH_MSG_ERROR(" No geometry manager available or alignment level not given.");
      ATH_MSG_ERROR(" DB not updated.");
      return;
   }

   ATH_MSG_INFO("-------------------------------------------------------");
   if(m_trtAlignLevel>=0)
      ATH_MSG_INFO("updating Level "<<m_trtAlignLevel<<" alignment constants for modules");
   else {
      ATH_MSG_INFO("updating Level "<<m_trtAlignLevelBarrel<<" alignment constants for Barrel modules");
      ATH_MSG_INFO("updating Level "<<m_trtAlignLevelEndcaps<<" alignment constants for Endcap modules");
   }

   Amg::Transform3D dbtransform;

   // loop over all AlignModules
   std::vector<Trk::AlignModule *>::const_iterator imod     = m_alignModuleTool->getAlignModuleCollection()->begin();
   std::vector<Trk::AlignModule *>::const_iterator imod_end = m_alignModuleTool->getAlignModuleCollection()->end();
   for( ; imod != imod_end; ++imod ) {

      Trk::AlignModule * module = *imod;

      ATH_MSG_DEBUG("-------------------------------------------------------");
      ATH_MSG_DEBUG("Alignment corrections for module: "<<module->name());

      // the identifier of the module for the DB
      Identifier modID = module->identify();

      // The TRT AlignModules of levels 1 and 2 use the TRT Identifier.
      // For level 0 the Identifier can be either TRT of Pixel but we have to check
      // that the AlignModule contains the TRT detector elements
      // (there can also be L0 without TRT in which case we don't want to do anything)
      if(!m_trtHelper->is_trt(modID) && (m_trtAlignLevel!=0 || !containsTRT(module)) ) {
         ATH_MSG_DEBUG("Module with ID: "<<modID<<" is not a TRT module. Skipping.");
         continue;
      }

      // get active alignment parameters for the module
      DataVector<Trk::AlignPar> * alignPars =  m_alignModuleTool->getAlignPars(module);
      if(alignPars->size() == 0) {
         ATH_MSG_INFO("Alignment for module "<<module->name()<<" not available.");
         continue;
      }

      // get final alignment parameters
      DataVector<Trk::AlignPar> * fullAlignPars = m_alignModuleTool->getFullAlignPars(module);
      double apTraX = fullAlignPars->at(Trk::AlignModule::TransX)->par();
      double apTraY = fullAlignPars->at(Trk::AlignModule::TransY)->par();
      double apTraZ = fullAlignPars->at(Trk::AlignModule::TransZ)->par();
      double apRotX = fullAlignPars->at(Trk::AlignModule::RotX)->par();
      double apRotY = fullAlignPars->at(Trk::AlignModule::RotY)->par();
      double apRotZ = fullAlignPars->at(Trk::AlignModule::RotZ)->par();

      // construct the alignment transform
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

      // update TRT level 0 constants
      if(m_trtAlignLevel == 0) {
         updateL0asL1(modID,dbtransform);
         continue;
      }

      int level;
      if(m_trtHelper->is_barrel(modID))
         level = m_trtAlignLevelBarrel;
      else
         level = m_trtAlignLevelEndcaps;

      // update the constants in memory
      ATH_MSG_DEBUG("updating level "<<level<<" constants for module "<<module->name()<<" with ID "<<modID);
      switch(level) {
         case 1: case 2: {
            // for levels 1 and 2 alignment the DB frame equals to the global frame but the align
            // frame does not, so we have to apply an additional transform
            Amg::Transform3D dbFrameToAlignFrame = module->globalFrameToAlignFrame();

            ATH_MSG_DEBUG("DB to align");
            printTransform(dbFrameToAlignFrame);

            dbtransform = dbFrameToAlignFrame.inverse() * transform * dbFrameToAlignFrame;

            break;
         }

         case 3:
            // for level 3 alignment the DB frame and the align frame equals to the local frame
            // so we don't apply any additional transform

            // if we are in the Endcap we need to rotate about global Z
	          //if(abs(m_trtHelper->barrel_ec_id(modID.get_identifier32().get_compact()).get_compact()) == 2) { # Bug!
	          if( abs(m_trtHelper->barrel_ec(modID)) == 2) {
	             ATH_MSG_DEBUG("L3 module in TRT end-cap A so we apply additional rotation about global Z (" << M_PI << ")");
               Amg::Translation3D newtranslation( 0,0,0 );
               Amg::Transform3D newtransform = newtranslation * Amg::AngleAxis3D( M_PI, Amg::Vector3D(0.,0.,1.));
               dbtransform =  newtransform * transform;
            }
            else
               // otherwise we don't do anything
               dbtransform = transform;

            break;

         default:
            // otherwise we don't know what to do
            // so we do nothing for the module
            ATH_MSG_WARNING("Unknown level "<<level<<". Skipping the module.");
            continue;
      }

      ATH_MSG_DEBUG("DB transform");
      printTransform(dbtransform);

      // tweak applies the transform onto already existing transform in the DB
      // or sets it if it doesn't exist yet
      if(m_trtAlignDbSvc->tweakAlignTransform(modID,dbtransform,level).isFailure())
         ATH_MSG_ERROR("Error setting constants for module \'"<<module->name()<<"\'");
      else
         ATH_MSG_DEBUG("Module \'"<<module->name()<<"\': Level "<<level<<" constants updated.");
   }
   ATH_MSG_INFO("-------------------------------------------------------");

}

//________________________________________________________________________
void TRTTrkAlignDBTool::updateL0asL1(Identifier idL0, const Amg::Transform3D & transform)
{
   // splitting the whole TRT into barrel and two endcaps
   // or the whole Inner Detector into TRT and the rest
   // the Identifier has to be equal to:
   // - L1 TRT barrel Identifier for TRT level 0 update
   // - L1 Pixel Identifier for Inner Detector level 0 update
   if( idL0 != m_pixHelper->wafer_id(0,0,0,0) && idL0 != m_trtHelper->barrel_ec_id(-1) ) {
      ATH_MSG_ERROR("wrong identifier for L0 TRT in updateL0asL1() : "<<idL0);
      ATH_MSG_ERROR("allowed are "<<m_trtHelper->barrel_ec_id(-1)<<" for TRT L0");
      ATH_MSG_ERROR("        and "<<m_pixHelper->wafer_id(0,0,0,0)<<" for InDet L0");
      return;
   }

   // for both L0 and L1 the global frame is equal to the DB frame
   // for L0 it is also equal to align frame so no additional transform is needed

   ATH_MSG_INFO("Splitting TRT into L1 modules and updating in the DB");

   std::vector<Identifier> ids;
   ids.push_back(m_trtHelper->barrel_ec_id(-1)); // barrel
   ids.push_back(m_trtHelper->barrel_ec_id(-2));  // endcap A
   ids.push_back(m_trtHelper->barrel_ec_id(2));   // endcap C

   ATH_MSG_INFO("Updating L1 constants for TRT");
   for(unsigned int i=0; i<ids.size(); ++i) {
      // tweak applies the transform onto already existing transform in the DB
      if(m_trtAlignDbSvc->tweakAlignTransform(ids[i],transform,1).isFailure())
         ATH_MSG_ERROR("Error setting constants.");
      else
         ATH_MSG_INFO("Module "<<i<<". - id "<<ids[i]<<": Constants updated.");
   }
}

//________________________________________________________________________
void TRTTrkAlignDBTool::printTransform(const Amg::Transform3D & tr) const
{
   ATH_MSG_DEBUG(" - translation: "<<tr.translation().x()<<"  "<<tr.translation().y()<<"  "<<tr.translation().z());
   ATH_MSG_DEBUG(" - rotation:");
   ATH_MSG_DEBUG("      "<<tr(0,0)<<"  "<<tr(0,1)<<"  "<<tr(0,2));
   ATH_MSG_DEBUG("      "<<tr(1,0)<<"  "<<tr(1,1)<<"  "<<tr(1,2));
   ATH_MSG_DEBUG("      "<<tr(2,0)<<"  "<<tr(2,1)<<"  "<<tr(2,2));
   return;
}

//________________________________________________________________________
bool TRTTrkAlignDBTool::containsTRT(Trk::AlignModule * module) const
{
   ATH_MSG_DEBUG("in containsTRT()");
   const Trk::AlignModule::DetElementCollection * coll = module->detElementCollection(Trk::AlignModule::TRT);
   if(!coll || coll->size()==0)
      return false;

   return true;
}
//________________________________________________________________________

} // end namespace
