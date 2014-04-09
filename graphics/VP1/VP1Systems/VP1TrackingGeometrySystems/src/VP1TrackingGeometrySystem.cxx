/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Implementation of class VP1TrackingGeometrySystem                  //
//                                                                     //
//  Author: Andreas Salzburger <Andreas.Salzburger@cern.ch> (primary)  //
//          Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial version: June 2007                                         //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1TrackingGeometrySystems/VP1TrackingGeometrySystem.h"
#include "VP1TrackingGeometrySystems/TrackingVolumeToSoNode.h"
#include "VP1Utils/SurfaceToSoNode.h"
#include "VP1TrackingGeometrySystems/ColorCodeConverter.h"
#include "ui_trackinggeometrycontrollerform.h"

#include "VP1Utils/VP1JobConfigInfo.h"

#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoClipPlane.h>

#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"

#include <vector>
#include <map>
#include <string>

// Qt
#include <QInputDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QString>
#include <QBuffer>

struct VP1TrackingGeometrySystem::Imp
{
  bool retrieveSuccess;

  // for signal handling
  bool senderMapFilled;
  std::map<QObject*,std::pair<SoSwitch*,bool> >   checkBoxSwitchMap;
  std::vector<VP1ExtraSepLayerHelper*>            sepHelperContainer;

  // Tracking volumes 
  SoSwitch* volswitch;
  VP1ExtraSepLayerHelper * sephelper_vol;
  
  // Beam pipe
  SoSwitch* beamPipeSwitch;
  VP1ExtraSepLayerHelper* sephelper_beamPipe;
  
  // Pixel section
  SoSwitch* pixSurfBarrelSwitch;
  SoSwitch* pixSurfECASwitch;
  SoSwitch* pixSurfECCSwitch;
  VP1ExtraSepLayerHelper* sephelper_pixSurfBarrel;
  VP1ExtraSepLayerHelper* sephelper_pixSurfECA;
  VP1ExtraSepLayerHelper* sephelper_pixSurfECC;               

  SoSwitch* pixLayBarrelSwitch;
  SoSwitch* pixLayECASwitch;
  SoSwitch* pixLayECCSwitch;
  VP1ExtraSepLayerHelper* sephelper_pixLayBarrel;
  VP1ExtraSepLayerHelper* sephelper_pixLayECA;
  VP1ExtraSepLayerHelper* sephelper_pixLayECC;               
  
  // SCT section
  SoSwitch* sctSurfBarrelSwitch;
  SoSwitch* sctSurfECASwitch;
  SoSwitch* sctSurfECCSwitch;
  VP1ExtraSepLayerHelper* sephelper_sctSurfBarrel;
  VP1ExtraSepLayerHelper* sephelper_sctSurfECA;
  VP1ExtraSepLayerHelper* sephelper_sctSurfECC;               

  SoSwitch* sctLayBarrelSwitch;
  SoSwitch* sctLayECASwitch;
  SoSwitch* sctLayECCSwitch;
  VP1ExtraSepLayerHelper* sephelper_sctLayBarrel;
  VP1ExtraSepLayerHelper* sephelper_sctLayECA;
  VP1ExtraSepLayerHelper* sephelper_sctLayECC;               

  // TRT section
  SoSwitch* trtSurfBarrelSwitch;
  SoSwitch* trtSurfECASwitch;
  SoSwitch* trtSurfECCSwitch;
  VP1ExtraSepLayerHelper* sephelper_trtSurfBarrel;
  VP1ExtraSepLayerHelper* sephelper_trtSurfECA;
  VP1ExtraSepLayerHelper* sephelper_trtSurfECC;               

  SoSwitch* trtLayBarrelSwitch;
  SoSwitch* trtLayECASwitch;
  SoSwitch* trtLayECCSwitch;
  VP1ExtraSepLayerHelper* sephelper_trtLayBarrel;
  VP1ExtraSepLayerHelper* sephelper_trtLayECA;
  VP1ExtraSepLayerHelper* sephelper_trtLayECC;    
  
  // Passive layers           
  SoSwitch* passiveLayBarrelSwitch;
  SoSwitch* passiveLayECASwitch;
  SoSwitch* passiveLayECCSwitch;
  VP1ExtraSepLayerHelper* sephelper_passiveLayBarrel;
  VP1ExtraSepLayerHelper* sephelper_passiveLayECA;
  VP1ExtraSepLayerHelper* sephelper_passiveLayECC;    

  
  SoSwitch* clipswitch;
  SoSwitch* msBswitch;
  SoSwitch* msIswitch;
  SoSwitch* msOswitch;
  SoSwitch* msBIswitch;
  SoSwitch* msBMswitch;
  SoSwitch* msBOswitch;
  SoSwitch* msEIswitch;
  SoSwitch* msEMswitch;
  SoSwitch* msEOswitch;
  SoSwitch* msCSswitch;
  SoSwitch* msTGswitch;
  SoSwitch* msBIlayswitch;
  SoSwitch* msBMlayswitch;
  SoSwitch* msBOlayswitch;
  SoSwitch* msEIlayswitch;
  SoSwitch* msEMlayswitch;
  SoSwitch* msEOlayswitch;
  SoSwitch* msCSlayswitch;
  SoSwitch* msTGlayswitch;
  SoSwitch* msBTenvswitch;
  SoSwitch* msETenvswitch;
  SoSwitch* msShenvswitch;
  SoSwitch* msFeenvswitch;
  SoSwitch* msRaenvswitch;
  SoSwitch* msBTdlswitch;
  SoSwitch* msETdlswitch;
  SoSwitch* msShdlswitch;
  SoSwitch* msFedlswitch;
  SoSwitch* msRadlswitch;


  VP1ExtraSepLayerHelper * sephelper_msB;
  VP1ExtraSepLayerHelper * sephelper_msI;
  VP1ExtraSepLayerHelper * sephelper_msO;
  VP1ExtraSepLayerHelper * sephelper_msBI;
  VP1ExtraSepLayerHelper * sephelper_msBM;
  VP1ExtraSepLayerHelper * sephelper_msBO;
  VP1ExtraSepLayerHelper * sephelper_msEI;
  VP1ExtraSepLayerHelper * sephelper_msEM;
  VP1ExtraSepLayerHelper * sephelper_msEO;
  VP1ExtraSepLayerHelper * sephelper_msCS;
  VP1ExtraSepLayerHelper * sephelper_msTG;
  VP1ExtraSepLayerHelper * sephelper_msBIlay;
  VP1ExtraSepLayerHelper * sephelper_msBMlay;
  VP1ExtraSepLayerHelper * sephelper_msBOlay;
  VP1ExtraSepLayerHelper * sephelper_msEIlay;
  VP1ExtraSepLayerHelper * sephelper_msEMlay;
  VP1ExtraSepLayerHelper * sephelper_msEOlay;
  VP1ExtraSepLayerHelper * sephelper_msCSlay;
  VP1ExtraSepLayerHelper * sephelper_msTGlay;
  VP1ExtraSepLayerHelper * sephelper_msBTenv;
  VP1ExtraSepLayerHelper * sephelper_msETenv;
  VP1ExtraSepLayerHelper * sephelper_msShenv;
  VP1ExtraSepLayerHelper * sephelper_msFeenv;
  VP1ExtraSepLayerHelper * sephelper_msRaenv;
  VP1ExtraSepLayerHelper * sephelper_msBTdl;
  VP1ExtraSepLayerHelper * sephelper_msETdl;
  VP1ExtraSepLayerHelper * sephelper_msShdl;
  VP1ExtraSepLayerHelper * sephelper_msFedl;
  VP1ExtraSepLayerHelper * sephelper_msRadl;

  TrackingVolumeToSoNode* trackvol2sonode;
  SurfaceToSoNode*        surface2sonode;

  // widgets for the sidepane
  QLineEdit*   lineEdit_TrackingGeometrySvcName;
  QLineEdit*   lineEdit_TrackingGeometryName;
  
  QPushButton* pushButton_Retrieve;
  QCheckBox*   checkBox_TrackingVolumes;

  QCheckBox*   checkBox_pixSurfBarrel;
  QCheckBox*   checkBox_pixSurfECA;
  QCheckBox*   checkBox_pixSurfECC;               
  QCheckBox*   checkBox_sctSurfBarrel;
  QCheckBox*   checkBox_sctSurfECA;
  QCheckBox*   checkBox_sctSurfECC;
  QCheckBox*   checkBox_trtSurfBarrel;
  QCheckBox*   checkBox_trtSurfECA;
  QCheckBox*   checkBox_trtSurfECC;
               
  QCheckBox*   checkBox_beamPipe;
  QCheckBox*   checkBox_pixLayBarrel;
  QCheckBox*   checkBox_pixLayECA;
  QCheckBox*   checkBox_pixLayECC;               
  QCheckBox*   checkBox_sctLayBarrel;
  QCheckBox*   checkBox_sctLayECA;
  QCheckBox*   checkBox_sctLayECC;
  QCheckBox*   checkBox_trtLayBarrel;
  QCheckBox*   checkBox_trtLayECA;
  QCheckBox*   checkBox_trtLayECC;
  QCheckBox*   checkBox_passiveLayBarrel;
  QCheckBox*   checkBox_passiveLayECA;
  QCheckBox*   checkBox_passiveLayECC;

  std::vector<std::string>      surfacesNames;
  
  QCheckBox*   checkBox_ClipPlane;
  QCheckBox*   checkBox_MuonStaticFrameB;
  QCheckBox*   checkBox_MuonStaticFrameI;
  QCheckBox*   checkBox_MuonStaticFrameO;
  QCheckBox*   checkBox_msBIvol;
  QCheckBox*   checkBox_msBIlay;
  QCheckBox*   checkBox_msBMvol;
  QCheckBox*   checkBox_msBMlay;
  QCheckBox*   checkBox_msBOvol;
  QCheckBox*   checkBox_msBOlay;
  QCheckBox*   checkBox_msEIvol;
  QCheckBox*   checkBox_msEIlay;
  QCheckBox*   checkBox_msEMvol;
  QCheckBox*   checkBox_msEMlay;
  QCheckBox*   checkBox_msEOvol;
  QCheckBox*   checkBox_msEOlay;
  QCheckBox*   checkBox_msCSvol;
  QCheckBox*   checkBox_msCSlay;
  QCheckBox*   checkBox_msTGvol;
  QCheckBox*   checkBox_msTGlay;
  QCheckBox*   checkBox_msBTenv;
  QCheckBox*   checkBox_msETenv;
  QCheckBox*   checkBox_msShenv;
  QCheckBox*   checkBox_msFeenv;
  QCheckBox*   checkBox_msRaenv;
  QCheckBox*   checkBox_msBTdl;
  QCheckBox*   checkBox_msETdl;
  QCheckBox*   checkBox_msShdl;
  QCheckBox*   checkBox_msFedl;
  QCheckBox*   checkBox_msRadl;

  // variables keeping some defaults
  QString initialTrackingGeometrySvcName;
  QString initialTrackingGeometryName;

  // global stuff
  StoreGateSvc* detStore;
  SoSeparator*  root;

  ColorCodeConverter colorCodeConverter;
};

//_____________________________________________________________________________________
VP1TrackingGeometrySystem::VP1TrackingGeometrySystem()
  : IVP13DSystemSimple("TrkGeo",
		       "This system displays the tracking geometry.",
		       "Andreas.Salzburger@cern.ch"),
    d( new Imp)
{
  d->retrieveSuccess = false;
  d->senderMapFilled = false;
  d->volswitch = 0;
  
  // beam pipe
  d->beamPipeSwitch = 0;
  d->sephelper_beamPipe = 0;
  
  // Pixel section
  d->pixSurfBarrelSwitch = 0;
  d->pixSurfECASwitch  = 0;
  d->pixSurfECCSwitch  = 0;
  d->sephelper_pixSurfBarrel = 0;
  d->sephelper_pixSurfECA  = 0;
  d->sephelper_pixSurfECC  = 0;               

  d->pixLayBarrelSwitch = 0;
  d->pixLayECASwitch  = 0;
  d->pixLayECCSwitch  = 0;
  d->sephelper_pixLayBarrel = 0;
  d->sephelper_pixLayECA  = 0;
  d->sephelper_pixLayECC  = 0;            
  
  // SCT section
  d->sctSurfBarrelSwitch = 0;
  d->sctSurfECASwitch  = 0;
  d->sctSurfECCSwitch  = 0;
  d->sephelper_sctSurfBarrel = 0;
  d->sephelper_sctSurfECA  = 0;
  d->sephelper_sctSurfECC  = 0;           

  d->sctLayBarrelSwitch = 0;
  d->sctLayECASwitch  = 0;
  d->sctLayECCSwitch  = 0;
  d->sephelper_sctLayBarrel = 0;
  d->sephelper_sctLayECA  = 0;
  d->sephelper_sctLayECC  = 0;              

  // TRT section
  d->trtSurfBarrelSwitch = 0;
  d->trtSurfECASwitch  = 0;
  d->trtSurfECCSwitch  = 0;
  d->sephelper_trtSurfBarrel = 0;
  d->sephelper_trtSurfECA  = 0;
  d->sephelper_trtSurfECC  = 0;           

  d->trtLayBarrelSwitch = 0;
  d->trtLayECASwitch  = 0;
  d->trtLayECCSwitch  = 0;
  d->sephelper_trtLayBarrel = 0;
  d->sephelper_trtLayECA  = 0;
  d->sephelper_trtLayECC  = 0;

  // passive layers
  d->passiveLayBarrelSwitch = 0;
  d->passiveLayECASwitch  = 0;
  d->passiveLayECCSwitch  = 0;
  d->sephelper_passiveLayBarrel = 0;
  d->sephelper_passiveLayECA  = 0;
  d->sephelper_passiveLayECC  = 0;

  d->clipswitch = 0;
  d->msBswitch = 0;
  d->msIswitch = 0;
  d->msOswitch = 0;
  d->msBIswitch = 0;
  d->msBMswitch = 0;
  d->msBOswitch = 0;
  d->msEIswitch = 0;
  d->msEMswitch = 0;
  d->msEOswitch = 0;
  d->msCSswitch = 0;
  d->msTGswitch = 0;
  d->msBIlayswitch = 0;
  d->msBMlayswitch = 0;
  d->msBOlayswitch = 0;
  d->msEIlayswitch = 0;
  d->msEMlayswitch = 0;
  d->msEOlayswitch = 0;
  d->msCSlayswitch = 0;
  d->msTGlayswitch = 0;
  d->msBTenvswitch = 0;
  d->msETenvswitch = 0;
  d->msShenvswitch = 0;
  d->msFeenvswitch = 0;
  d->msRaenvswitch = 0;
  d->msBTdlswitch = 0;
  d->msETdlswitch = 0;
  d->msShdlswitch = 0;
  d->msFedlswitch = 0;
  d->msRadlswitch = 0;

  d->sephelper_vol = 0;
  d->sephelper_msB = 0;
  d->sephelper_msI = 0;
  d->sephelper_msO = 0;
  d->sephelper_msBI = 0;
  d->sephelper_msBM = 0;
  d->sephelper_msBO = 0;
  d->sephelper_msEI = 0;
  d->sephelper_msEM = 0;
  d->sephelper_msEO = 0;
  d->sephelper_msCS = 0;
  d->sephelper_msTG = 0;
  d->sephelper_msBIlay = 0;
  d->sephelper_msBMlay = 0;
  d->sephelper_msBOlay = 0;
  d->sephelper_msEIlay = 0;
  d->sephelper_msEMlay = 0;
  d->sephelper_msEOlay = 0;
  d->sephelper_msCSlay = 0;
  d->sephelper_msTGlay = 0;
  d->sephelper_msBTenv = 0;
  d->sephelper_msETenv = 0;
  d->sephelper_msShenv = 0;
  d->sephelper_msFeenv = 0;
  d->sephelper_msRaenv = 0;
  d->sephelper_msBTdl = 0;
  d->sephelper_msETdl = 0;
  d->sephelper_msShdl = 0;
  d->sephelper_msFedl = 0;
  d->sephelper_msRadl = 0;

  d->trackvol2sonode = 0;
  d->surface2sonode = 0;

  d->lineEdit_TrackingGeometrySvcName = 0;
  d->lineEdit_TrackingGeometryName = 0;
  d->pushButton_Retrieve = 0;
  
  d->checkBox_pixSurfBarrel     = 0;
  d->checkBox_pixSurfECA        = 0;
  d->checkBox_pixSurfECC        = 0; 
  d->checkBox_sctSurfBarrel     = 0;
  d->checkBox_sctSurfECA        = 0;
  d->checkBox_sctSurfECC        = 0;
  d->checkBox_trtSurfBarrel     = 0;
  d->checkBox_trtSurfECA        = 0;
  d->checkBox_trtSurfECC        = 0;
  
  d->checkBox_beamPipe          = 0;    
  d->checkBox_pixLayBarrel      = 0;
  d->checkBox_pixLayECA         = 0;
  d->checkBox_pixLayECC         = 0;  
  d->checkBox_sctLayBarrel      = 0;
  d->checkBox_sctLayECA         = 0;
  d->checkBox_sctLayECC         = 0;
  d->checkBox_trtLayBarrel      = 0;
  d->checkBox_trtLayECA         = 0;
  d->checkBox_trtLayECC         = 0;
  d->checkBox_passiveLayBarrel  = 0;
  d->checkBox_passiveLayECA     = 0;
  d->checkBox_passiveLayECC     = 0;
  
  d->checkBox_TrackingVolumes = 0;
  d->checkBox_ClipPlane = 0;
  d->checkBox_MuonStaticFrameB = 0;
  d->checkBox_MuonStaticFrameI = 0;
  d->checkBox_MuonStaticFrameO = 0;
  d->checkBox_msBIvol = 0;
  d->checkBox_msBMvol = 0;
  d->checkBox_msBOvol = 0;
  d->checkBox_msEIvol = 0;
  d->checkBox_msEMvol = 0;
  d->checkBox_msEOvol = 0;
  d->checkBox_msCSvol = 0;
  d->checkBox_msTGvol = 0;
  d->checkBox_msBIlay = 0;
  d->checkBox_msBMlay = 0;
  d->checkBox_msBOlay = 0;
  d->checkBox_msEIlay = 0;
  d->checkBox_msEMlay = 0;
  d->checkBox_msEOlay = 0;
  d->checkBox_msCSlay = 0;
  d->checkBox_msTGlay = 0;
  d->checkBox_msBTenv = 0;
  d->checkBox_msETenv = 0;
  d->checkBox_msShenv = 0;
  d->checkBox_msFeenv = 0;
  d->checkBox_msRaenv = 0;
  d->checkBox_msBTdl = 0;
  d->checkBox_msETdl = 0;
  d->checkBox_msShdl = 0;
  d->checkBox_msFedl = 0;
  d->checkBox_msRadl = 0;

  d->initialTrackingGeometrySvcName = "AtlasTrackingGeometrySvc";
  d->initialTrackingGeometryName    = "";

  d->detStore = 0;
  d->root = 0;
}

//_____________________________________________________________________________________
VP1TrackingGeometrySystem::~VP1TrackingGeometrySystem()
{
    
  std::vector<VP1ExtraSepLayerHelper*>::iterator sepHelperIter    =  d->sepHelperContainer.begin();    
  std::vector<VP1ExtraSepLayerHelper*>::iterator sepHelperIterEnd =  d->sepHelperContainer.end();    
  for ( ; sepHelperIter != sepHelperIterEnd; ++sepHelperIter){   
      if ((*sepHelperIter)) {
          SoSeparator * sep = (*sepHelperIter)->topSeparator();
          delete (*sepHelperIter);
          sep->unref();
      } 
  }

  delete d->trackvol2sonode;
  delete d->surface2sonode;
  delete d;
}

//_____________________________________________________________________________________
void VP1TrackingGeometrySystem::buildPermanentSceneGraph(StoreGateSvc* detstore, SoSeparator *root)
{
  d->detStore = detstore;
  d->root = root;
}

//_____________________________________________________________________________________
void VP1TrackingGeometrySystem::retrieveTrackingGeometry()
{
  if( d->retrieveSuccess)
  {
    message( "TrackingGeometry has already been successfully retrieved");
    return;
  }

  VP1JobConfigInfo::ensureInit(this);
  if (!(VP1JobConfigInfo::hasMuonGeometry()
	||VP1JobConfigInfo::hasLArGeometry()
	||VP1JobConfigInfo::hasTileGeometry()
	||VP1JobConfigInfo::hasPixelGeometry()
	||VP1JobConfigInfo::hasSCTGeometry()
	||VP1JobConfigInfo::hasTRTGeometry()))
  {
    message("Error: Geometry not configured!");
    return;
  }

  if (!d->lineEdit_TrackingGeometrySvcName) {
    message("Error: retrieveTrackingGeometry() invoked before controller built!");
    return;
  }


  /*-----------------------------------------------------------------------
   *  Retrieving service locator
   *-----------------------------------------------------------------------*/
  ISvcLocator* svcLoc = serviceLocator();

  if( !svcLoc)
  {
    message("Error: Got NULL pointer to the service locator!");
    return;
  }

  /*-----------------------------------------------------------------------
   *  Retrieving TrackingGeometrySvc
   *-----------------------------------------------------------------------*/
  message( "Trying to retrieve TrackingGeometrySvc '" +
           d->lineEdit_TrackingGeometrySvcName->text().trimmed() + "'");

  Trk::ITrackingGeometrySvc* trackingGeometrySvc;
  StatusCode status =
      svcLoc->service( d->lineEdit_TrackingGeometrySvcName->text().trimmed().toStdString(), trackingGeometrySvc);

  if( !status.isSuccess())
  {
    message("Error: Could not retrieve TrackingGeometrySvc '" +
            d->lineEdit_TrackingGeometrySvcName->text().trimmed() + "'\n"
            "Check spelling and if you have added the correct jobOptions fragment");
    return;
  }

  message( "Successfully retrieved TrackingGeometrySvc '" +
           d->lineEdit_TrackingGeometrySvcName->text().trimmed() + "'");

  /*-----------------------------------------------------------------------
   *  Retrieving TrackingGeometry
   *-----------------------------------------------------------------------*/
  const Trk::TrackingGeometry * trackingGeometry;
  if( d->lineEdit_TrackingGeometryName->text().trimmed() == "")
  {
    message( "Trying to retrieve default TrackingGeometry");
    trackingGeometry = trackingGeometrySvc->trackingGeometry();

    if( !trackingGeometry)
    {
      return;
      message("Error: Could not retrieve default TrackingGeometry");
    }
    else
    {
      message( "Successfully retrieved default TrackingGeometry");
    }
  }
  else
  {
    message( "Trying to retrieve TrackingGeometry '" +
             d->lineEdit_TrackingGeometryName->text().trimmed() +
             "' from DetectorStore");
    if (!d->detStore)
    {
      message("Error: Got NULL detectorstore.");
      return;
    }

    if (d->detStore->retrieve(trackingGeometry, d->lineEdit_TrackingGeometryName->text().trimmed().toStdString()).isFailure()||!trackingGeometry)
    {
      message("Error: Could not retrieve TrackingGeometry '" +
              d->lineEdit_TrackingGeometryName->text().trimmed() +
              "' from DetectorStore.");
      return;
    }
    else
    {
      message( "Successfully retrieved TrackingGeometry '" +
               d->lineEdit_TrackingGeometryName->text().trimmed() + "'");
    }
  }

  /*-----------------------------------------------------------------------
   *  Setup scene if retrieval was successful
   *-----------------------------------------------------------------------*/
  d->lineEdit_TrackingGeometrySvcName->setReadOnly( true);
  d->lineEdit_TrackingGeometryName->setReadOnly( true);
  d->pushButton_Retrieve->setEnabled( false);

  // Pixel section
  d->beamPipeSwitch          = new SoSwitch(); 
  d->pixSurfBarrelSwitch     = new SoSwitch(); 
  d->pixSurfECASwitch        = new SoSwitch(); 
  d->pixSurfECCSwitch        = new SoSwitch(); 
  d->pixLayBarrelSwitch      = new SoSwitch(); 
  d->pixLayECASwitch         = new SoSwitch(); 
  d->pixLayECCSwitch         = new SoSwitch(); 
  d->sctSurfBarrelSwitch     = new SoSwitch(); 
  d->sctSurfECASwitch        = new SoSwitch(); 
  d->sctSurfECCSwitch        = new SoSwitch(); 
  d->sctLayBarrelSwitch      = new SoSwitch(); 
  d->sctLayECASwitch         = new SoSwitch(); 
  d->sctLayECCSwitch         = new SoSwitch(); 
  d->trtSurfBarrelSwitch     = new SoSwitch(); 
  d->trtSurfECASwitch        = new SoSwitch(); 
  d->trtSurfECCSwitch        = new SoSwitch(); 
  d->trtLayBarrelSwitch      = new SoSwitch(); 
  d->trtLayECASwitch         = new SoSwitch(); 
  d->trtLayECCSwitch         = new SoSwitch(); 
  d->passiveLayBarrelSwitch  = new SoSwitch(); 
  d->passiveLayECASwitch     = new SoSwitch(); 
  d->passiveLayECCSwitch     = new SoSwitch(); 

  d->beamPipeSwitch->whichChild = SO_SWITCH_NONE;
  d->pixSurfBarrelSwitch->whichChild = SO_SWITCH_NONE;
  d->pixSurfECASwitch->whichChild = SO_SWITCH_NONE;
  d->pixSurfECCSwitch->whichChild = SO_SWITCH_NONE; 
  d->pixLayBarrelSwitch->whichChild = SO_SWITCH_NONE;  
  d->pixLayECASwitch->whichChild = SO_SWITCH_NONE;
  d->pixLayECCSwitch->whichChild = SO_SWITCH_NONE; 
  d->sctSurfBarrelSwitch->whichChild = SO_SWITCH_NONE;
  d->sctSurfECASwitch->whichChild = SO_SWITCH_NONE;
  d->sctSurfECCSwitch->whichChild = SO_SWITCH_NONE; 
  d->sctLayBarrelSwitch->whichChild = SO_SWITCH_NONE;
  d->sctLayECASwitch->whichChild = SO_SWITCH_NONE;
  d->sctLayECCSwitch->whichChild = SO_SWITCH_NONE; 
  d->trtSurfBarrelSwitch->whichChild = SO_SWITCH_NONE;
  d->trtSurfECASwitch->whichChild = SO_SWITCH_NONE;
  d->trtSurfECCSwitch->whichChild = SO_SWITCH_NONE; 
  d->trtLayBarrelSwitch->whichChild = SO_SWITCH_NONE;
  d->trtLayECASwitch->whichChild  = SO_SWITCH_NONE;
  d->trtLayECCSwitch->whichChild  = SO_SWITCH_NONE; 
  d->passiveLayBarrelSwitch->whichChild = SO_SWITCH_NONE;
  d->passiveLayECASwitch->whichChild  = SO_SWITCH_NONE;
  d->passiveLayECCSwitch->whichChild  = SO_SWITCH_NONE;

  SoSeparator* sep_beamPipe = new SoSeparator();  sep_beamPipe->ref();
  SoSeparator* sep_pixSurfBarrel = new SoSeparator();  sep_pixSurfBarrel->ref();
  SoSeparator* sep_pixSurfECA  = new SoSeparator();  sep_pixSurfECA->ref(); 
  SoSeparator* sep_pixSurfECC  = new SoSeparator();  sep_pixSurfECC->ref();              
  SoSeparator* sep_pixLayBarrel  = new SoSeparator();  sep_pixLayBarrel->ref(); 
  SoSeparator* sep_pixLayECA   = new SoSeparator();  sep_pixLayECA->ref();  
  SoSeparator* sep_pixLayECC   = new SoSeparator();  sep_pixLayECC->ref();            
  SoSeparator* sep_sctSurfBarrel = new SoSeparator();  sep_sctSurfBarrel->ref();
  SoSeparator* sep_sctSurfECA  = new SoSeparator();  sep_sctSurfECA->ref(); 
  SoSeparator* sep_sctSurfECC  = new SoSeparator();  sep_sctSurfECC->ref();          
  SoSeparator* sep_sctLayBarrel  = new SoSeparator();  sep_sctLayBarrel->ref(); 
  SoSeparator* sep_sctLayECA        = new SoSeparator();  sep_sctLayECA->ref();  
  SoSeparator* sep_sctLayECC        = new SoSeparator();  sep_sctLayECC->ref();              
  SoSeparator* sep_trtSurfBarrel    = new SoSeparator();  sep_trtSurfBarrel->ref();
  SoSeparator* sep_trtSurfECA       = new SoSeparator();  sep_trtSurfECA->ref(); 
  SoSeparator* sep_trtSurfECC       = new SoSeparator();  sep_trtSurfECC->ref();          
  SoSeparator* sep_trtLayBarrel     = new SoSeparator();  sep_trtLayBarrel->ref(); 
  SoSeparator* sep_trtLayECA        = new SoSeparator();  sep_trtLayECA->ref();  
  SoSeparator* sep_trtLayECC        = new SoSeparator();  sep_trtLayECC->ref(); 
  SoSeparator* sep_passiveLayBarrel = new SoSeparator();  sep_passiveLayBarrel->ref(); 
  SoSeparator* sep_passiveLayECA    = new SoSeparator();  sep_passiveLayECA->ref();  
  SoSeparator* sep_passiveLayECC    = new SoSeparator();  sep_passiveLayECC->ref(); 
   
  d->sephelper_beamPipe      = new VP1ExtraSepLayerHelper(sep_beamPipe);
  d->sephelper_pixSurfBarrel = new VP1ExtraSepLayerHelper(sep_pixSurfBarrel);
  d->sephelper_pixSurfECA  = new VP1ExtraSepLayerHelper(sep_pixSurfECA);
  d->sephelper_pixSurfECC  = new VP1ExtraSepLayerHelper(sep_pixSurfECC);               
  d->sephelper_pixLayBarrel  = new VP1ExtraSepLayerHelper(sep_pixLayBarrel);
  d->sephelper_pixLayECA   = new VP1ExtraSepLayerHelper(sep_pixLayECA);
  d->sephelper_pixLayECC   = new VP1ExtraSepLayerHelper(sep_pixLayECC);            
  d->sephelper_sctSurfBarrel = new VP1ExtraSepLayerHelper(sep_sctSurfBarrel);
  d->sephelper_sctSurfECA  = new VP1ExtraSepLayerHelper(sep_sctSurfECA);
  d->sephelper_sctSurfECC  = new VP1ExtraSepLayerHelper(sep_sctSurfECC);           
  d->sephelper_sctLayBarrel  = new VP1ExtraSepLayerHelper(sep_sctLayBarrel);
  d->sephelper_sctLayECA   = new VP1ExtraSepLayerHelper(sep_sctLayECA);
  d->sephelper_sctLayECC   = new VP1ExtraSepLayerHelper(sep_sctLayECC);              
  d->sephelper_trtSurfBarrel = new VP1ExtraSepLayerHelper(sep_trtSurfBarrel);
  d->sephelper_trtSurfECA  = new VP1ExtraSepLayerHelper(sep_trtSurfECA);
  d->sephelper_trtSurfECC  = new VP1ExtraSepLayerHelper(sep_trtSurfECC);           
  d->sephelper_trtLayBarrel  = new VP1ExtraSepLayerHelper(sep_trtLayBarrel);
  d->sephelper_trtLayECA   = new VP1ExtraSepLayerHelper(sep_trtLayECA);
  d->sephelper_trtLayECC   = new VP1ExtraSepLayerHelper(sep_trtLayECC);
  d->sephelper_passiveLayBarrel  = new VP1ExtraSepLayerHelper(sep_passiveLayBarrel);
  d->sephelper_passiveLayECA   = new VP1ExtraSepLayerHelper(sep_passiveLayECA );
  d->sephelper_passiveLayECC   = new VP1ExtraSepLayerHelper(sep_passiveLayECC );  
  
  d->volswitch = new SoSwitch();
  d->clipswitch = new SoSwitch();
  d->msBswitch = new SoSwitch();
  d->msIswitch = new SoSwitch();
  d->msOswitch = new SoSwitch();
  d->msBIswitch = new SoSwitch();
  d->msBMswitch = new SoSwitch();
  d->msBOswitch = new SoSwitch();
  d->msEIswitch = new SoSwitch();
  d->msEMswitch = new SoSwitch();
  d->msEOswitch = new SoSwitch();
  d->msCSswitch = new SoSwitch();
  d->msTGswitch = new SoSwitch();
  d->msBIlayswitch = new SoSwitch();
  d->msBMlayswitch = new SoSwitch();
  d->msBOlayswitch = new SoSwitch();
  d->msEIlayswitch = new SoSwitch();
  d->msEMlayswitch = new SoSwitch();
  d->msEOlayswitch = new SoSwitch();
  d->msCSlayswitch = new SoSwitch();
  d->msTGlayswitch = new SoSwitch();
  d->msBTenvswitch = new SoSwitch();
  d->msETenvswitch = new SoSwitch();
  d->msShenvswitch = new SoSwitch();
  d->msFeenvswitch = new SoSwitch();
  d->msRaenvswitch = new SoSwitch();
  d->msBTdlswitch = new SoSwitch();
  d->msETdlswitch = new SoSwitch();
  d->msShdlswitch = new SoSwitch();
  d->msFedlswitch = new SoSwitch();
  d->msRadlswitch = new SoSwitch();

  d->volswitch->whichChild = SO_SWITCH_NONE;
  d->clipswitch->whichChild = SO_SWITCH_NONE;
  d->msBswitch->whichChild = SO_SWITCH_NONE;
  d->msIswitch->whichChild = SO_SWITCH_NONE;
  d->msOswitch->whichChild = SO_SWITCH_NONE;
  d->msBIswitch->whichChild = SO_SWITCH_NONE;
  d->msBMswitch->whichChild = SO_SWITCH_NONE;
  d->msBOswitch->whichChild = SO_SWITCH_NONE;
  d->msEIswitch->whichChild = SO_SWITCH_NONE;
  d->msEMswitch->whichChild = SO_SWITCH_NONE;
  d->msEOswitch->whichChild = SO_SWITCH_NONE;
  d->msCSswitch->whichChild = SO_SWITCH_NONE;
  d->msTGswitch->whichChild = SO_SWITCH_NONE;
  d->msBIlayswitch->whichChild = SO_SWITCH_NONE;
  d->msBMlayswitch->whichChild = SO_SWITCH_NONE;
  d->msBOlayswitch->whichChild = SO_SWITCH_NONE;
  d->msEIlayswitch->whichChild = SO_SWITCH_NONE;
  d->msEMlayswitch->whichChild = SO_SWITCH_NONE;
  d->msEOlayswitch->whichChild = SO_SWITCH_NONE;
  d->msCSlayswitch->whichChild = SO_SWITCH_NONE;
  d->msTGlayswitch->whichChild = SO_SWITCH_NONE;
  d->msBTenvswitch->whichChild = SO_SWITCH_NONE;
  d->msETenvswitch->whichChild = SO_SWITCH_NONE;
  d->msShenvswitch->whichChild = SO_SWITCH_NONE;
  d->msFeenvswitch->whichChild = SO_SWITCH_NONE;
  d->msRaenvswitch->whichChild = SO_SWITCH_NONE;
  d->msBTdlswitch->whichChild = SO_SWITCH_NONE;
  d->msETdlswitch->whichChild = SO_SWITCH_NONE;
  d->msShdlswitch->whichChild = SO_SWITCH_NONE;
  d->msFedlswitch->whichChild = SO_SWITCH_NONE;
  d->msRadlswitch->whichChild = SO_SWITCH_NONE;

  SoSeparator * sep_vol = new SoSeparator; sep_vol->ref();
  SoSeparator * sep_msB = new SoSeparator; sep_msB->ref();
  SoSeparator * sep_msI = new SoSeparator; sep_msI->ref();
  SoSeparator * sep_msO = new SoSeparator; sep_msO->ref();
  SoSeparator * sep_msBI = new SoSeparator; sep_msBI->ref();
  SoSeparator * sep_msBM = new SoSeparator; sep_msBM->ref();
  SoSeparator * sep_msBO = new SoSeparator; sep_msBO->ref();
  SoSeparator * sep_msEI = new SoSeparator; sep_msEI->ref();
  SoSeparator * sep_msEM = new SoSeparator; sep_msEM->ref();
  SoSeparator * sep_msEO = new SoSeparator; sep_msEO->ref();
  SoSeparator * sep_msCS = new SoSeparator; sep_msCS->ref();
  SoSeparator * sep_msTG = new SoSeparator; sep_msTG->ref();
  SoSeparator * sep_msBIlay = new SoSeparator; sep_msBIlay->ref();
  SoSeparator * sep_msBMlay = new SoSeparator; sep_msBMlay->ref();
  SoSeparator * sep_msBOlay = new SoSeparator; sep_msBOlay->ref();
  SoSeparator * sep_msEIlay = new SoSeparator; sep_msEIlay->ref();
  SoSeparator * sep_msEMlay = new SoSeparator; sep_msEMlay->ref();
  SoSeparator * sep_msEOlay = new SoSeparator; sep_msEOlay->ref();
  SoSeparator * sep_msCSlay = new SoSeparator; sep_msCSlay->ref();
  SoSeparator * sep_msTGlay = new SoSeparator; sep_msTGlay->ref();
  SoSeparator * sep_msBTenv = new SoSeparator; sep_msBTenv->ref();
  SoSeparator * sep_msETenv = new SoSeparator; sep_msETenv->ref();
  SoSeparator * sep_msShenv = new SoSeparator; sep_msShenv->ref();
  SoSeparator * sep_msFeenv = new SoSeparator; sep_msFeenv->ref();
  SoSeparator * sep_msRaenv = new SoSeparator; sep_msRaenv->ref();
  SoSeparator * sep_msBTdl = new SoSeparator; sep_msBTdl->ref();
  SoSeparator * sep_msETdl = new SoSeparator; sep_msETdl->ref();
  SoSeparator * sep_msShdl = new SoSeparator; sep_msShdl->ref();
  SoSeparator * sep_msFedl = new SoSeparator; sep_msFedl->ref();
  SoSeparator * sep_msRadl = new SoSeparator; sep_msRadl->ref();

  d->sephelper_vol = new VP1ExtraSepLayerHelper(sep_vol);
  d->sephelper_msB = new VP1ExtraSepLayerHelper(sep_msB);
  d->sephelper_msI = new VP1ExtraSepLayerHelper(sep_msI);
  d->sephelper_msO = new VP1ExtraSepLayerHelper(sep_msO);
  d->sephelper_msBI = new VP1ExtraSepLayerHelper(sep_msBI);
  d->sephelper_msBM = new VP1ExtraSepLayerHelper(sep_msBM);
  d->sephelper_msBO = new VP1ExtraSepLayerHelper(sep_msBO);
  d->sephelper_msEI = new VP1ExtraSepLayerHelper(sep_msEI);
  d->sephelper_msEM = new VP1ExtraSepLayerHelper(sep_msEM);
  d->sephelper_msEO = new VP1ExtraSepLayerHelper(sep_msEO);
  d->sephelper_msCS = new VP1ExtraSepLayerHelper(sep_msCS);
  d->sephelper_msTG = new VP1ExtraSepLayerHelper(sep_msTG);
  d->sephelper_msBIlay = new VP1ExtraSepLayerHelper(sep_msBIlay);
  d->sephelper_msBMlay = new VP1ExtraSepLayerHelper(sep_msBMlay);
  d->sephelper_msBOlay = new VP1ExtraSepLayerHelper(sep_msBOlay);
  d->sephelper_msEIlay = new VP1ExtraSepLayerHelper(sep_msEIlay);
  d->sephelper_msEMlay = new VP1ExtraSepLayerHelper(sep_msEMlay);
  d->sephelper_msEOlay = new VP1ExtraSepLayerHelper(sep_msEOlay);
  d->sephelper_msCSlay = new VP1ExtraSepLayerHelper(sep_msCSlay);
  d->sephelper_msTGlay = new VP1ExtraSepLayerHelper(sep_msTGlay);
  d->sephelper_msBTenv = new VP1ExtraSepLayerHelper(sep_msBTenv);
  d->sephelper_msETenv = new VP1ExtraSepLayerHelper(sep_msETenv);
  d->sephelper_msShenv = new VP1ExtraSepLayerHelper(sep_msShenv);
  d->sephelper_msFeenv = new VP1ExtraSepLayerHelper(sep_msFeenv);
  d->sephelper_msRaenv = new VP1ExtraSepLayerHelper(sep_msRaenv);
  d->sephelper_msBTdl = new VP1ExtraSepLayerHelper(sep_msBTdl);
  d->sephelper_msETdl = new VP1ExtraSepLayerHelper(sep_msETdl);
  d->sephelper_msShdl = new VP1ExtraSepLayerHelper(sep_msShdl);
  d->sephelper_msFedl = new VP1ExtraSepLayerHelper(sep_msFedl);
  d->sephelper_msRadl = new VP1ExtraSepLayerHelper(sep_msRadl);
  
  d->sepHelperContainer.reserve(100);
  d->sepHelperContainer.push_back(d->sephelper_vol);
  d->sepHelperContainer.push_back(d->sephelper_beamPipe);
  d->sepHelperContainer.push_back(d->sephelper_pixSurfBarrel);
  d->sepHelperContainer.push_back(d->sephelper_pixSurfECA);
  d->sepHelperContainer.push_back(d->sephelper_pixSurfECC);               
  d->sepHelperContainer.push_back(d->sephelper_pixLayBarrel);
  d->sepHelperContainer.push_back(d->sephelper_pixLayECA);
  d->sepHelperContainer.push_back(d->sephelper_pixLayECC);               
  d->sepHelperContainer.push_back(d->sephelper_sctSurfBarrel);
  d->sepHelperContainer.push_back(d->sephelper_sctSurfECA);
  d->sepHelperContainer.push_back(d->sephelper_sctSurfECC);               
  d->sepHelperContainer.push_back(d->sephelper_sctLayBarrel);
  d->sepHelperContainer.push_back(d->sephelper_sctLayECA);
  d->sepHelperContainer.push_back(d->sephelper_sctLayECC);               
  d->sepHelperContainer.push_back(d->sephelper_trtSurfBarrel);
  d->sepHelperContainer.push_back(d->sephelper_trtSurfECA);
  d->sepHelperContainer.push_back(d->sephelper_trtSurfECC);               
  d->sepHelperContainer.push_back(d->sephelper_trtLayBarrel);
  d->sepHelperContainer.push_back(d->sephelper_trtLayECA);
  d->sepHelperContainer.push_back(d->sephelper_trtLayECC);    
  d->sepHelperContainer.push_back(d->sephelper_passiveLayBarrel);
  d->sepHelperContainer.push_back(d->sephelper_passiveLayECA);
  d->sepHelperContainer.push_back(d->sephelper_passiveLayECC);    
  d->sepHelperContainer.push_back(d->sephelper_msB);
  d->sepHelperContainer.push_back(d->sephelper_msI);
  d->sepHelperContainer.push_back(d->sephelper_msO);
  d->sepHelperContainer.push_back(d->sephelper_msBI);
  d->sepHelperContainer.push_back(d->sephelper_msBM);
  d->sepHelperContainer.push_back(d->sephelper_msBO);
  d->sepHelperContainer.push_back(d->sephelper_msEI);
  d->sepHelperContainer.push_back(d->sephelper_msEM);
  d->sepHelperContainer.push_back(d->sephelper_msEO);
  d->sepHelperContainer.push_back(d->sephelper_msCS);
  d->sepHelperContainer.push_back(d->sephelper_msTG);
  d->sepHelperContainer.push_back(d->sephelper_msBIlay);
  d->sepHelperContainer.push_back(d->sephelper_msBMlay);
  d->sepHelperContainer.push_back(d->sephelper_msBOlay);
  d->sepHelperContainer.push_back(d->sephelper_msEIlay);
  d->sepHelperContainer.push_back(d->sephelper_msEMlay);
  d->sepHelperContainer.push_back(d->sephelper_msEOlay);
  d->sepHelperContainer.push_back(d->sephelper_msCSlay);
  d->sepHelperContainer.push_back(d->sephelper_msTGlay);
  d->sepHelperContainer.push_back(d->sephelper_msBTenv);
  d->sepHelperContainer.push_back(d->sephelper_msETenv);
  d->sepHelperContainer.push_back(d->sephelper_msShenv);
  d->sepHelperContainer.push_back(d->sephelper_msFeenv);
  d->sepHelperContainer.push_back(d->sephelper_msRaenv);
  d->sepHelperContainer.push_back(d->sephelper_msBTdl);
  d->sepHelperContainer.push_back(d->sephelper_msETdl);
  d->sepHelperContainer.push_back(d->sephelper_msShdl);
  d->sepHelperContainer.push_back(d->sephelper_msFedl);
  d->sepHelperContainer.push_back(d->sephelper_msRadl);

  std::cout << "To here !" << std::endl;

  const Trk::TrackingVolume* highestVolume = trackingGeometry->highestTrackingVolume();

  if (!highestVolume) {
    message("Error: No highest tracking volume registered.");
    return;
  }

  const Trk::TrackingVolume* idCalo = trackingGeometry->trackingVolume("MuonSpectrometerEntrance");

  d->trackvol2sonode = new TrackingVolumeToSoNode();
  d->surface2sonode = new SurfaceToSoNode();

  if (idCalo) processTrkVolume(idCalo);
  else processTrkVolume(highestVolume);

  processMsVolume(highestVolume,0,0);

  SoRotationXYZ* rotClip = new SoRotationXYZ;
  rotClip->axis.setValue( SoRotationXYZ::X);
  rotClip->angle.setValue( -M_PI/2.);

  d->clipswitch->addChild( rotClip);
  d->clipswitch->addChild( new SoClipPlane);
  d->root->addChild(d->clipswitch);

  d->volswitch->addChild(sep_vol);

  d->beamPipeSwitch->addChild(sep_beamPipe);  
  d->pixSurfBarrelSwitch->addChild(sep_pixSurfBarrel);
  d->pixSurfECASwitch->addChild(sep_pixSurfECA);
  d->pixSurfECCSwitch->addChild(sep_pixSurfECC);
  d->pixLayBarrelSwitch->addChild(sep_pixLayBarrel);
  d->pixLayECASwitch->addChild(sep_pixLayECA);
  d->pixLayECCSwitch->addChild(sep_pixLayECC);
  d->sctSurfBarrelSwitch->addChild(sep_sctSurfBarrel);
  d->sctSurfECASwitch->addChild(sep_sctSurfECA);
  d->sctSurfECCSwitch->addChild(sep_sctSurfECC);
  d->sctLayBarrelSwitch->addChild(sep_sctLayBarrel);
  d->sctLayECASwitch->addChild(sep_sctLayECA);
  d->sctLayECCSwitch->addChild(sep_sctLayECC);
  d->trtSurfBarrelSwitch->addChild(sep_trtSurfBarrel);
  d->trtSurfECASwitch->addChild(sep_trtSurfECA);
  d->trtSurfECCSwitch->addChild(sep_trtSurfECC);
  d->trtLayBarrelSwitch->addChild(sep_trtLayBarrel);
  d->trtLayECASwitch->addChild(sep_trtLayECA);
  d->trtLayECCSwitch->addChild(sep_trtLayECC);
  d->passiveLayBarrelSwitch->addChild(sep_passiveLayBarrel);
  d->passiveLayECASwitch->addChild(sep_passiveLayECA);
  d->passiveLayECCSwitch->addChild(sep_passiveLayECC);
  
  d->msBswitch->addChild(sep_msB);
  d->msIswitch->addChild(sep_msI);
  d->msOswitch->addChild(sep_msO);
  d->msBIswitch->addChild(sep_msBI);
  d->msBMswitch->addChild(sep_msBM);
  d->msBOswitch->addChild(sep_msBO);
  d->msEIswitch->addChild(sep_msEI);
  d->msEMswitch->addChild(sep_msEM);
  d->msEOswitch->addChild(sep_msEO);
  d->msCSswitch->addChild(sep_msCS);
  d->msTGswitch->addChild(sep_msTG);
  d->msBIlayswitch->addChild(sep_msBIlay);
  d->msBMlayswitch->addChild(sep_msBMlay);
  d->msBOlayswitch->addChild(sep_msBOlay);
  d->msEIlayswitch->addChild(sep_msEIlay);
  d->msEMlayswitch->addChild(sep_msEMlay);
  d->msEOlayswitch->addChild(sep_msEOlay);
  d->msCSlayswitch->addChild(sep_msCSlay);
  d->msTGlayswitch->addChild(sep_msTGlay);
  d->msBTenvswitch->addChild(sep_msBTenv);
  d->msETenvswitch->addChild(sep_msETenv);
  d->msShenvswitch->addChild(sep_msShenv);
  d->msFeenvswitch->addChild(sep_msFeenv);
  d->msRaenvswitch->addChild(sep_msRaenv);
  d->msBTdlswitch->addChild(sep_msBTdl);
  d->msETdlswitch->addChild(sep_msETdl);
  d->msShdlswitch->addChild(sep_msShdl);
  d->msFedlswitch->addChild(sep_msFedl);
  d->msRadlswitch->addChild(sep_msRadl);

  d->root->addChild(d->volswitch);
  d->root->addChild(d->beamPipeSwitch);          
  d->root->addChild(d->pixSurfBarrelSwitch);     
  d->root->addChild(d->pixSurfECASwitch);        
  d->root->addChild(d->pixSurfECCSwitch);        
  d->root->addChild(d->pixLayBarrelSwitch);      
  d->root->addChild(d->pixLayECASwitch);         
  d->root->addChild(d->pixLayECCSwitch);         
  d->root->addChild(d->sctSurfBarrelSwitch);     
  d->root->addChild(d->sctSurfECASwitch);        
  d->root->addChild(d->sctSurfECCSwitch);        
  d->root->addChild(d->sctLayBarrelSwitch);      
  d->root->addChild(d->sctLayECASwitch);         
  d->root->addChild(d->sctLayECCSwitch);        
  d->root->addChild(d->trtSurfBarrelSwitch);     
  d->root->addChild(d->trtSurfECASwitch);        
  d->root->addChild(d->trtSurfECCSwitch);        
  d->root->addChild(d->trtLayBarrelSwitch);      
  d->root->addChild(d->trtLayECASwitch);         
  d->root->addChild(d->trtLayECCSwitch);         
  d->root->addChild(d->passiveLayBarrelSwitch);  
  d->root->addChild(d->passiveLayECASwitch);     
  d->root->addChild(d->passiveLayECCSwitch);     
  d->root->addChild(d->msBswitch);
  d->root->addChild(d->msIswitch);
  d->root->addChild(d->msOswitch);
  d->root->addChild(d->msBIswitch);
  d->root->addChild(d->msBMswitch);
  d->root->addChild(d->msBOswitch);
  d->root->addChild(d->msEIswitch);
  d->root->addChild(d->msEMswitch);
  d->root->addChild(d->msEOswitch);
  d->root->addChild(d->msCSswitch);
  d->root->addChild(d->msTGswitch);
  d->root->addChild(d->msBIlayswitch);
  d->root->addChild(d->msBMlayswitch);
  d->root->addChild(d->msBOlayswitch);
  d->root->addChild(d->msEIlayswitch);
  d->root->addChild(d->msEMlayswitch);
  d->root->addChild(d->msEOlayswitch);
  d->root->addChild(d->msCSlayswitch);
  d->root->addChild(d->msTGlayswitch);
  d->root->addChild(d->msBTenvswitch);
  d->root->addChild(d->msETenvswitch);
  d->root->addChild(d->msShenvswitch);
  d->root->addChild(d->msFeenvswitch);
  d->root->addChild(d->msRaenvswitch);
  d->root->addChild(d->msBTdlswitch);
  d->root->addChild(d->msETdlswitch);
  d->root->addChild(d->msShdlswitch);
  d->root->addChild(d->msFedlswitch);
  d->root->addChild(d->msRadlswitch);

  d->retrieveSuccess = true;
}

//_____________________________________________________________________________________
void VP1TrackingGeometrySystem::processTrkVolume(const Trk::TrackingVolume* tvol)
{
  if (!tvol)
    return;
  
  SoNode * volnode = d->trackvol2sonode->translateTrackingVolume(*tvol);
  if (!volnode) {
    message("Error: Volume shape not implemented");
    return;
  }
  
  QString volumeName(tvol->volumeName().c_str());
  
  messageVerbose("Processing volume: "+volumeName);
  d->sephelper_vol->addNodeUnderMaterial(volnode,d->colorCodeConverter.getMaterialFromColorCode( tvol->colorCode()));
  
  const Trk::BinnedArray< Trk::Layer >* confinedLayers = tvol->confinedLayers();
  
  // loop over confined layers
  if (confinedLayers){
    const std::vector<const Trk::Layer*>& layerVector =  confinedLayers->arrayObjects();
    std::vector<const Trk::Layer*>::const_iterator layerIter = layerVector.begin();
    // loop over layers
    for ( ; layerIter != layerVector.end(); ++layerIter){
      // push_back the layer
      if (*layerIter){
        
        // get the layerIndex
        const Trk::LayerIndex& layIndex = (*layerIter)->layerIndex();
        
        // skip navigaion layers for the moment
        if (!layIndex.value()) continue;
        
        /* check the surface array */
        const Trk::SurfaceArray* layerSubSurfaces = (*layerIter)->surfaceArray();

        /** current separator helpers */
        VP1ExtraSepLayerHelper* layerSepHelper = 0;
        VP1ExtraSepLayerHelper* surfaceSepHelper = 0;
        
        if ( volumeName.contains("BeamPipe") > 0 ) {
            layerSepHelper = d->sephelper_beamPipe;
        } else if (volumeName.contains("Pixel") > 0 ) {
        // Pixel sector 
          if ( volumeName.contains("Barrel") > 0 ){
              // pixel barrel surfaces & layers
              layerSepHelper   = d->sephelper_pixLayBarrel;
              surfaceSepHelper = d->sephelper_pixSurfBarrel;
          } else if ( volumeName.contains("Endcap") > 0 ){
              // pixel endcap surfaces & layers
              if ( volumeName.contains("Pos") > 0){
                  layerSepHelper   = d->sephelper_pixLayECA;
                  surfaceSepHelper = d->sephelper_pixSurfECA;
              } else {
                  layerSepHelper   = d->sephelper_pixLayECC;
                  surfaceSepHelper = d->sephelper_pixSurfECC;
              }
           }    
        } else if (volumeName.contains("SCT") > 0) {
            // SCT sector
            if ( volumeName.contains("Barrel") > 0 ){
                // sct barrel surfaces & layers
                layerSepHelper   = d->sephelper_sctLayBarrel;
                surfaceSepHelper = d->sephelper_sctSurfBarrel;
            } else if ( volumeName.contains("Endcap") > 0 ){
                // sct endcap surfaces & layers
                if ( volumeName.contains("Pos") > 0){
                    layerSepHelper   = d->sephelper_sctLayECA;
                    surfaceSepHelper = d->sephelper_sctSurfECA;
                } else {
                    layerSepHelper   = d->sephelper_sctLayECC;
                    surfaceSepHelper = d->sephelper_sctSurfECC;
                }
            }   
        } else if (volumeName.contains("TRT") > 0) {
          // TRT sector
          if ( volumeName.contains("Barrel") > 0 ){
              // trt barrel surfaces & layers
              layerSepHelper   = d->sephelper_trtLayBarrel;
              surfaceSepHelper = d->sephelper_trtSurfBarrel;
          } else if ( volumeName.contains("Endcap") > 0 ){
              // trt endcap surfaces & layers
              if ( volumeName.contains("Pos") > 0 ){
                  layerSepHelper   = d->sephelper_trtLayECA;
                  surfaceSepHelper = d->sephelper_trtSurfECA;
              } else {
                  layerSepHelper   = d->sephelper_trtLayECC;
                  surfaceSepHelper = d->sephelper_trtSurfECC;
              }
          }
        } else {
             if ( volumeName.contains("Barrel") > 0 ){
                  // trt barrel surfaces & layers
                  layerSepHelper   = d->sephelper_passiveLayBarrel;
              } else if ( volumeName.contains("Endcap") > 0 ){
                  // trt endcap surfaces & layers
                  if ( volumeName.contains("Pos") > 0 ){
                      layerSepHelper   = d->sephelper_passiveLayECA;
                  } else {
                      layerSepHelper   = d->sephelper_passiveLayECC;
                  }
              }
        }
        
        std::cout << " VolumeName " << tvol->volumeName() << " / seperators : " << layerSepHelper << " / " << surfaceSepHelper << std::endl;
        
        // process the layer with the correct separator helper
        if (layerSepHelper) processTrkLayer(*layerIter, layerSepHelper, tvol->colorCode());
        // Surface
        if (layerSubSurfaces && surfaceSepHelper){
          const std::vector<const Trk::Surface*>& surfaceVector = layerSubSurfaces->arrayObjects();
          std::vector<const Trk::Surface*>::const_iterator surfaceIter = surfaceVector.begin();
          for ( ; surfaceIter != surfaceVector.end(); ++surfaceIter ){
            // push_back the surface
            if (*surfaceIter)
              processTrkSurface(*surfaceIter, surfaceSepHelper, tvol->colorCode());
          } 
        }
      }
    }
  }
  
  const Trk::BinnedArray< Trk::TrackingVolume >* confinedVolumes = tvol->confinedVolumes();
  // get the confined volumes and loop over it -> call recursively
  if (confinedVolumes){
    const std::vector<const Trk::TrackingVolume*>& volumes = confinedVolumes->arrayObjects();
    std::vector<const Trk::TrackingVolume*>::const_iterator volIter = volumes.begin();
    std::vector<const Trk::TrackingVolume*>::const_iterator volIterEnd = volumes.end();
    for ( ; volIter != volIterEnd; ++volIter)
      if (*volIter) processTrkVolume(*volIter);
  }
}
  
//_____________________________________________________________________________________
void VP1TrackingGeometrySystem::processMsVolume(const Trk::TrackingVolume* tvol, VP1ExtraSepLayerHelper* sepHelper, VP1ExtraSepLayerHelper* layHelper )
{
  if (!tvol)
    return;

  std::string name = tvol->volumeName();

  // only lowest level volumes displayed

  if (!tvol->confinedVolumes()) {
    SoNode * volnode = d->trackvol2sonode->translateTrackingVolume(*tvol);
    if (!volnode) {
      message("Error: Volume shape not implemented");
      return;
    }

    if ( name.substr(0,4) == "Muon" && tvol->colorCode() > 0 ) {
      if ( fabs(tvol->center().z())>12900 ) d->sephelper_msO->addNodeUnderMaterial(volnode,d->colorCodeConverter.getMaterialFromColorCode( tvol->colorCode()));
      else if ( fabs(tvol->center().z())>6750  ) d->sephelper_msI->addNodeUnderMaterial(volnode,d->colorCodeConverter.getMaterialFromColorCode( tvol->colorCode()));
      else d->sephelper_msB->addNodeUnderMaterial(volnode,d->colorCodeConverter.getMaterialFromColorCode( tvol->colorCode()));
    }

    // envelopes
    if ( sepHelper ) {
      unsigned int colorCode = 17;
      if ( name.substr(0,3)=="MDT") colorCode = 2;
      if ( name.substr(0,3)=="RPC") colorCode = 4;
      if ( name.substr(0,2)=="CS")  colorCode = 5;
      if ( name.substr(0,1)=="T")   colorCode = 6;
      sepHelper->addNodeUnderMaterial(volnode,d->colorCodeConverter.getMaterialFromColorCode(colorCode) );
      if (layHelper) processMsLayDense( tvol, sepHelper, layHelper, colorCode );
    } else if  ( layHelper ) {
      unsigned int colorCode = 13;
      layHelper->addNodeUnderMaterial(volnode,d->colorCodeConverter.getMaterialFromColorCode(colorCode) );
    }

  } else {
    const Trk::BinnedArray< Trk::TrackingVolume >* confinedVolumes = tvol->confinedVolumes();
    // get the confined volumes and loop over it -> call recursively
    if (confinedVolumes){
      const std::vector<const Trk::TrackingVolume*>& volumes = confinedVolumes->arrayObjects();
      std::vector<const Trk::TrackingVolume*>::const_iterator volIter = volumes.begin();
      std::vector<const Trk::TrackingVolume*>::const_iterator volIterEnd = volumes.end();
      for ( ; volIter != volIterEnd; ++volIter)
	if (*volIter) processMsVolume(*volIter,sepHelper, layHelper);
    }
  }

  const std::vector<const Trk::DetachedTrackingVolume*>* detVols = tvol->confinedDetachedVolumes();
  if (detVols) {
    // identify separators
    std::vector<const Trk::DetachedTrackingVolume*>::const_iterator dIter = detVols->begin();
    for ( ;dIter!=detVols->end(); dIter++) {
      if ( (tvol->inside((*dIter)->trackingVolume()->center(),0.) || (*dIter)->trackingVolume()->center().perp()<0.001) ) {
	std::string name = (*dIter)->name();
        if ( name.substr(0,2) == "BI" ) { sepHelper = d->sephelper_msBI; layHelper =  d->sephelper_msBIlay; }
        else if ( name.substr(0,2) == "BM" ) { sepHelper = d->sephelper_msBM; layHelper =  d->sephelper_msBMlay; }
        else if ( name.substr(0,2) == "BO" ) { sepHelper = d->sephelper_msBO; layHelper =  d->sephelper_msBOlay; }
        else if ( name.substr(0,2) == "EI" ) { sepHelper = d->sephelper_msEI; layHelper =  d->sephelper_msEIlay; }
        else if ( name.substr(0,2) == "EM" ) { sepHelper = d->sephelper_msEM; layHelper =  d->sephelper_msEMlay; }
        else if ( name.substr(0,2) == "EO" ) { sepHelper = d->sephelper_msEO; layHelper =  d->sephelper_msEOlay; }
        else if ( name.substr(0,2) == "CS" ) { sepHelper = d->sephelper_msCS; layHelper =  d->sephelper_msCSlay; }
        else if ( name.substr(0,1) == "T" )  { sepHelper = d->sephelper_msTG; layHelper =  d->sephelper_msTGlay; }
        else if ( name.substr(0,4) == "Rail" ) { sepHelper = d->sephelper_msRaenv; layHelper =  d->sephelper_msRadl; }
        else if ( name.substr(0,7) == "MDTRail" ) { sepHelper = d->sephelper_msRaenv; layHelper =  d->sephelper_msRadl; }
        else if ( name.substr(0,4) == "SADL" ) { sepHelper = d->sephelper_msRaenv; layHelper =  d->sephelper_msRadl; }
        else if ( name.substr(0,4) == "TBWH" ) { sepHelper = d->sephelper_msRaenv; layHelper =  d->sephelper_msRadl; }
	else if ( name.substr(0,4) == "MBAP" ) { sepHelper = d->sephelper_msRaenv; layHelper =  d->sephelper_msRadl; }
	else if ( name.substr(0,4) == "MBWH" ) { sepHelper = d->sephelper_msRaenv; layHelper =  d->sephelper_msRadl; }
	else if ( name.substr(0,3) == "PP2" ) { sepHelper = d->sephelper_msRaenv; layHelper =  d->sephelper_msRadl; }
        else if ( name.substr(0,3) == "ECT" )  { sepHelper = d->sephelper_msETenv; layHelper =  d->sephelper_msETdl; }
        else if ( name.substr(0,2) == "BT" || name.substr(0,6) == "EdgeBT" || name.substr(0,6) == "HeadBT" )
	  { sepHelper = d->sephelper_msBTenv; layHelper =  d->sephelper_msBTdl; }
        else if ( name.substr(0,3) == "BAR" )
		  // && ( name.substr(0,18) == "BAR_ToroidBAR_Cold" || name.substr(0,22) == "BAR_ToroidBAR_Voussoir")  )
	  { sepHelper = d->sephelper_msBTenv; layHelper =  d->sephelper_msBTdl; }
        else if ( name.substr(0,9) == "ForwardSh" || name.substr(0,8) == "ToroidSh" || name.substr(0,6) == "DiskSh" )
	  { sepHelper = d->sephelper_msShenv; layHelper =  d->sephelper_msShdl; }
        else if ( name.substr(0,5) == "StdFe" || name.substr(0,6) == "ExtrFe" || name.substr(0,6) == "ConnFe" )
	  { sepHelper = d->sephelper_msFeenv; layHelper =  d->sephelper_msFedl; }
        else if ( name.substr(0,4) == "Feet" )
	  { sepHelper = d->sephelper_msFeenv; layHelper =  d->sephelper_msFedl; }
        else   // display somehow
	  { sepHelper = d->sephelper_msFeenv; layHelper =  d->sephelper_msFedl; }
      } else {
	std::string name = (*dIter)->name();
	//std::cout << "VP1 ignores volume:"<<name<< std::endl;
	if ( name.substr(0,3) == "BAR" &&
	     name.substr(0,18) == "BAR_ToroidBAR_Cold" )
	  { sepHelper = d->sephelper_msBTenv; layHelper =  d->sephelper_msBTdl; }
      }
      if (sepHelper) processMsVolume( (*dIter)->trackingVolume(), sepHelper, layHelper);
      sepHelper = 0; layHelper = 0;
    }
  }
}

//_____________________________________________________________________________________
void VP1TrackingGeometrySystem::processTrkLayer(const Trk::Layer* tlayer,
                                                VP1ExtraSepLayerHelper* sepLayerHelper,
                                                unsigned int colorCode)
{
  if (!tlayer) return;
  SoNode * node = d->surface2sonode->translateSurface(tlayer->surfaceRepresentation());
  if (node) {
      sepLayerHelper->addNodeUnderMaterial(node,d->colorCodeConverter.getMaterialFromColorCode( colorCode));
      std::cout << "Adding Layer to Separator helper: " << sepLayerHelper << std::endl;
  }
}

//_____________________________________________________________________________________
void VP1TrackingGeometrySystem::processTrkSurface(const Trk::Surface*tsurf,
                                                 VP1ExtraSepLayerHelper* sepLayerHelper,
                                                  unsigned int colorCode)
{
  if (!tsurf) return;
  SoNode * node = d->surface2sonode->translateSurface(*tsurf);
  if (node) sepLayerHelper->addNodeUnderMaterial(node,d->colorCodeConverter.getMaterialFromColorCode( colorCode));
}

//_____________________________________________________________________________________
void VP1TrackingGeometrySystem::processMsLayDense(const Trk::TrackingVolume* tvol, VP1ExtraSepLayerHelper* sepHelper, VP1ExtraSepLayerHelper* layHelper, int color)
{
  if (!tvol) return;

  // ordered layers
  const Trk::BinnedArray< Trk::Layer >* confinedLayers = tvol->confinedLayers();
  if (confinedLayers){
    const std::vector<const Trk::Layer*>& layerVector =  confinedLayers->arrayObjects();
    std::vector<const Trk::Layer*>::const_iterator layerIter = layerVector.begin();
    // loop over layers
    for ( ; layerIter != layerVector.end(); ++layerIter){
      // push_back the layer
      if (*layerIter){
	SoNode * node = d->surface2sonode->translateSurface((*layerIter)->surfaceRepresentation());
	if (node && layHelper ) {
          if ((*layerIter)->layerType()>0)
	    layHelper->addNodeUnderMaterial(node,d->colorCodeConverter.getMaterialFromColorCode( color));
          else
	    layHelper->addNodeUnderMaterial(node,d->colorCodeConverter.getMaterialFromColorCode( 1));
	}
      }
    }
  }
  // unordered layers
  const std::vector< const Trk::Layer* >* confALays = tvol->confinedArbitraryLayers();
  if (confALays){
    std::vector<const Trk::Layer*>::const_iterator layerIter = confALays->begin();
    // loop over layers
    for ( ; layerIter != confALays->end(); ++layerIter){
      // push_back the layer
      if (*layerIter){
	SoNode * node = d->surface2sonode->translateSurface((*layerIter)->surfaceRepresentation());
	if (node && layHelper ) {
          if ((*layerIter)->layerType()>0)
	    layHelper->addNodeUnderMaterial(node,d->colorCodeConverter.getMaterialFromColorCode( color));
          else
	    layHelper->addNodeUnderMaterial(node,d->colorCodeConverter.getMaterialFromColorCode( 1));
	}
      }
    }
  }
  // dense volumes
  const std::vector< const Trk::TrackingVolume* >* confVols = tvol->confinedDenseVolumes();
  if (confVols){
    std::vector<const Trk::TrackingVolume*>::const_iterator volIter = confVols->begin();
    for ( ; volIter != confVols->end(); ++volIter){
      if (*volIter && sepHelper ) processMsVolume( *volIter, 0, layHelper );
    }
  }
}

//_____________________________________________________________________________________
QWidget * VP1TrackingGeometrySystem::buildController()
{
  VP1JobConfigInfo::ensureInit(this);
  if (!(VP1JobConfigInfo::hasMuonGeometry()
	||VP1JobConfigInfo::hasLArGeometry()
	||VP1JobConfigInfo::hasTileGeometry()
	||VP1JobConfigInfo::hasPixelGeometry()
	||VP1JobConfigInfo::hasSCTGeometry()
	||VP1JobConfigInfo::hasTRTGeometry()))
  {
    return new QLabel("Geometry not configured.");
  }

  QWidget * controller = new QWidget(0);
  Ui::TrackingGeometryControllerForm ui;
  ui.setupUi(controller);

  if (!(VP1JobConfigInfo::hasLArGeometry()
	||VP1JobConfigInfo::hasTileGeometry()
	||VP1JobConfigInfo::hasPixelGeometry()
	||VP1JobConfigInfo::hasSCTGeometry()
	||VP1JobConfigInfo::hasTRTGeometry()))
    ui.groupBox_idandcalogeometry->setVisible(false);

  if (!VP1JobConfigInfo::hasMuonGeometry()) {
    ui.groupBox_muonframe->setVisible(false);
    ui.groupBox_muonactivematerial->setVisible(false);
    ui.groupBox_muoninertmaterial->setVisible(false);
  }

  connect(ui.checkBox_TrackingVolumes,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));

  connect(ui.checkBox_BeamPipeLayer,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_LayersECA,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_LayersBarrel,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_LayersECC,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_PixelLayersECA,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_PixelLayersBarrel,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_PixelLayersECC,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_SctLayersECA,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_SctLayersBarrel,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_SctLayersECC,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_TrtLayersECA,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_TrtLayersBarrel,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_TrtLayersECC,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));

  connect(ui.checkBox_PixelSurfacesECA,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_PixelSurfacesBarrel,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_PixelSurfacesECC,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_SctSurfacesECA,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_SctSurfacesBarrel,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_SctSurfacesECC,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));

  connect(ui.checkBox_ClipPlane,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_MuonStaticFrameB,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_MuonStaticFrameI,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBox_MuonStaticFrameO,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.pushButton_Retrieve,SIGNAL(clicked()),this,SLOT(retrieveTrackingGeometry()));
  connect(ui.checkBoxBIVol,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxBMVol,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxBOVol,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxEIVol,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxEMVol,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxEOVol,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxCSCVol,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxTGCVol,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxBILay,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxBMLay,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxBOLay,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxEILay,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxEMLay,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxEOLay,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxCSCLay,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxTGCLay,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxBTEnv,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxETEnv,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxShEnv,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxFeetEnv,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxRailEnv,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxBTDL,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxETDL,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxShDL,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxFeetDL,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect(ui.checkBoxRailDL,SIGNAL(toggled(bool)),this,SLOT(changeTopLevelView()));
  connect( ui.lineEdit_TrackingGeometrySvcName, SIGNAL( returnPressed()),
           ui.pushButton_Retrieve, SIGNAL( clicked()));
  connect( ui.lineEdit_TrackingGeometryName, SIGNAL( returnPressed()),
           ui.pushButton_Retrieve, SIGNAL( clicked()));

  d->lineEdit_TrackingGeometrySvcName = ui.lineEdit_TrackingGeometrySvcName;
  d->lineEdit_TrackingGeometryName = ui.lineEdit_TrackingGeometryName;

  d->lineEdit_TrackingGeometrySvcName->setText( d->initialTrackingGeometrySvcName);
  d->lineEdit_TrackingGeometryName->setText( d->initialTrackingGeometryName);

  d->pushButton_Retrieve = ui.pushButton_Retrieve;
  d->checkBox_TrackingVolumes = ui.checkBox_TrackingVolumes;

  d->checkBox_beamPipe         = ui.checkBox_BeamPipeLayer;
  d->checkBox_pixLayECA        = ui.checkBox_PixelLayersECA;
  d->checkBox_pixLayBarrel     = ui.checkBox_PixelLayersBarrel;
  d->checkBox_pixLayECC        = ui.checkBox_PixelLayersECC;
  d->checkBox_sctLayECA        = ui.checkBox_SctLayersECA;
  d->checkBox_sctLayBarrel     = ui.checkBox_SctLayersBarrel;
  d->checkBox_sctLayECC        = ui.checkBox_SctLayersECC;
  d->checkBox_trtLayECA        = ui.checkBox_TrtLayersECA;
  d->checkBox_trtLayBarrel     = ui.checkBox_TrtLayersBarrel;
  d->checkBox_trtLayECC        = ui.checkBox_TrtLayersECC;
  d->checkBox_passiveLayECA    = ui.checkBox_LayersECA;
  d->checkBox_passiveLayBarrel = ui.checkBox_LayersBarrel;
  d->checkBox_passiveLayECC    = ui.checkBox_LayersECC;

  d->checkBox_pixSurfECA = ui.checkBox_PixelSurfacesECA;
  d->surfacesNames.push_back("PixelECA");
  d->checkBox_pixSurfBarrel = ui.checkBox_PixelSurfacesBarrel;
  d->surfacesNames.push_back("PixelBarrel");
  d->checkBox_pixSurfECC = ui.checkBox_PixelSurfacesECC;
  d->surfacesNames.push_back("PixelECC");
  d->checkBox_sctSurfECA = ui.checkBox_SctSurfacesECA;
  d->surfacesNames.push_back("SCT_ECA");
  d->checkBox_sctSurfBarrel = ui.checkBox_SctSurfacesBarrel;
  d->surfacesNames.push_back("SCT_Barrel");
  d->checkBox_sctSurfECC = ui.checkBox_SctSurfacesECC;
  d->surfacesNames.push_back("SCT_ECC");

  d->checkBox_ClipPlane = ui.checkBox_ClipPlane;
  d->checkBox_MuonStaticFrameB = ui.checkBox_MuonStaticFrameB;
  d->checkBox_MuonStaticFrameI = ui.checkBox_MuonStaticFrameI;
  d->checkBox_MuonStaticFrameO = ui.checkBox_MuonStaticFrameO;
  d->checkBox_msBIvol = ui.checkBoxBIVol;
  d->checkBox_msBMvol = ui.checkBoxBMVol;
  d->checkBox_msBOvol = ui.checkBoxBOVol;
  d->checkBox_msEIvol = ui.checkBoxEIVol;
  d->checkBox_msEMvol = ui.checkBoxEMVol;
  d->checkBox_msEOvol = ui.checkBoxEOVol;
  d->checkBox_msCSvol = ui.checkBoxCSCVol;
  d->checkBox_msTGvol = ui.checkBoxTGCVol;
  d->checkBox_msBIlay = ui.checkBoxBILay;
  d->checkBox_msBMlay = ui.checkBoxBMLay;
  d->checkBox_msBOlay = ui.checkBoxBOLay;
  d->checkBox_msEIlay = ui.checkBoxEILay;
  d->checkBox_msEMlay = ui.checkBoxEMLay;
  d->checkBox_msEOlay = ui.checkBoxEOLay;
  d->checkBox_msCSlay = ui.checkBoxCSCLay;
  d->checkBox_msTGlay = ui.checkBoxTGCLay;
  d->checkBox_msBTenv = ui.checkBoxBTEnv;
  d->checkBox_msBTdl  = ui.checkBoxBTDL;
  d->checkBox_msETenv = ui.checkBoxETEnv;
  d->checkBox_msETdl  = ui.checkBoxETDL;
  d->checkBox_msShenv = ui.checkBoxShEnv;
  d->checkBox_msShdl  = ui.checkBoxShDL;
  d->checkBox_msFeenv = ui.checkBoxFeetEnv;
  d->checkBox_msFedl  = ui.checkBoxFeetDL;
  d->checkBox_msRaenv = ui.checkBoxRailEnv;
  d->checkBox_msRadl  = ui.checkBoxRailDL;


  
  std::cout << " Filling map entry [ " <<  d->checkBox_pixSurfBarrel  << " ] - with " << d->pixSurfBarrelSwitch << std::endl;
  
  // loop and set initial state
  std::map<QObject*,std::pair<SoSwitch*,bool> >::iterator chbMapIter    =  d->checkBoxSwitchMap.begin();
  std::map<QObject*,std::pair<SoSwitch*,bool> >::iterator chbMapIterEnd =  d->checkBoxSwitchMap.end();  
  
  for ( ; chbMapIter != chbMapIterEnd; ++chbMapIter ){
      QCheckBox* checkBox = dynamic_cast<QCheckBox*>((chbMapIter)->first);
      if (checkBox) checkBox->setChecked( ((chbMapIter)->second).second );
      
  }
  return controller;
}


//_____________________________________________________________________________________
void VP1TrackingGeometrySystem::changeTopLevelView()
{
  if (! d->retrieveSuccess)
    return;
   
   
  if (! d->senderMapFilled ){
      // register the map for states and signal handling
       d->checkBoxSwitchMap[d->checkBox_TrackingVolumes]      = std::make_pair(d->volswitch,false);             
       d->checkBoxSwitchMap[d->checkBox_pixSurfBarrel]        = std::make_pair(d->pixSurfBarrelSwitch,false);   
       d->checkBoxSwitchMap[d->checkBox_pixSurfECA]           = std::make_pair(d->pixSurfECASwitch,false);      
       d->checkBoxSwitchMap[d->checkBox_pixSurfECC]           = std::make_pair(d->pixSurfECCSwitch,false);      
       d->checkBoxSwitchMap[d->checkBox_sctSurfBarrel]        = std::make_pair(d->sctSurfBarrelSwitch,false);   
       d->checkBoxSwitchMap[d->checkBox_sctSurfECA]           = std::make_pair(d->sctSurfECASwitch,false);      
       d->checkBoxSwitchMap[d->checkBox_sctSurfECC]           = std::make_pair(d->sctSurfECCSwitch,false);      
       d->checkBoxSwitchMap[d->checkBox_trtSurfBarrel]        = std::make_pair(d->trtSurfBarrelSwitch,false);   
       d->checkBoxSwitchMap[d->checkBox_trtSurfECA]           = std::make_pair(d->trtSurfECASwitch,false);      
       d->checkBoxSwitchMap[d->checkBox_trtSurfECC]           = std::make_pair(d->trtSurfECCSwitch,false);      
       d->checkBoxSwitchMap[d->checkBox_beamPipe]             = std::make_pair(d->beamPipeSwitch,false);        
       d->checkBoxSwitchMap[d->checkBox_pixLayBarrel]         = std::make_pair(d->pixLayBarrelSwitch,false);    
       d->checkBoxSwitchMap[d->checkBox_pixLayECA]            = std::make_pair(d->pixLayECASwitch,false);       
       d->checkBoxSwitchMap[d->checkBox_pixLayECC]            = std::make_pair(d->pixLayECCSwitch,false);       
       d->checkBoxSwitchMap[d->checkBox_sctLayBarrel]         = std::make_pair(d->sctLayBarrelSwitch,false);    
       d->checkBoxSwitchMap[d->checkBox_sctLayECA]            = std::make_pair(d->sctLayECASwitch,false);       
       d->checkBoxSwitchMap[d->checkBox_sctLayECC]            = std::make_pair(d->sctLayECCSwitch,false);       
       d->checkBoxSwitchMap[d->checkBox_trtLayBarrel]         = std::make_pair(d->trtLayBarrelSwitch,false);    
       d->checkBoxSwitchMap[d->checkBox_trtLayECA]            = std::make_pair(d->trtLayECASwitch,false);       
       d->checkBoxSwitchMap[d->checkBox_trtLayECC]            = std::make_pair(d->trtLayECCSwitch,false);       
       d->checkBoxSwitchMap[d->checkBox_passiveLayBarrel]     = std::make_pair(d->passiveLayBarrelSwitch,false);
       d->checkBoxSwitchMap[d->checkBox_passiveLayECA]        = std::make_pair(d->passiveLayECASwitch,false);   
       d->checkBoxSwitchMap[d->checkBox_passiveLayECC]        = std::make_pair(d->passiveLayECCSwitch,false);   
       d->checkBoxSwitchMap[d->checkBox_ClipPlane]            = std::make_pair(d->clipswitch,false);
       d->checkBoxSwitchMap[d->checkBox_MuonStaticFrameB]     = std::make_pair(d->msBswitch,false);
       d->checkBoxSwitchMap[d->checkBox_MuonStaticFrameI]     = std::make_pair(d->msIswitch,false);
       d->checkBoxSwitchMap[d->checkBox_MuonStaticFrameO]     = std::make_pair(d->msOswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msBIvol]              = std::make_pair(d->msBIswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msBIlay]              = std::make_pair(d->msBMswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msBMvol]              = std::make_pair(d->msBOswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msBMlay]              = std::make_pair(d->msEIswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msBOvol]              = std::make_pair(d->msEMswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msBOlay]              = std::make_pair(d->msEOswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msEIvol]              = std::make_pair(d->msCSswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msEIlay]              = std::make_pair(d->msTGswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msEMvol]              = std::make_pair(d->msBIlayswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msEMlay]              = std::make_pair(d->msBMlayswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msEOvol]              = std::make_pair(d->msBOlayswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msEOlay]              = std::make_pair(d->msEIlayswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msCSvol]              = std::make_pair(d->msEMlayswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msCSlay]              = std::make_pair(d->msEOlayswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msTGvol]              = std::make_pair(d->msCSlayswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msTGlay]              = std::make_pair(d->msTGlayswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msBTenv]              = std::make_pair(d->msBTenvswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msETenv]              = std::make_pair(d->msETenvswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msShenv]              = std::make_pair(d->msShenvswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msFeenv]              = std::make_pair(d->msFeenvswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msRaenv]              = std::make_pair(d->msRaenvswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msBTdl]               = std::make_pair(d->msBTdlswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msETdl]               = std::make_pair(d->msETdlswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msShdl]               = std::make_pair(d->msShdlswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msFedl]               = std::make_pair(d->msFedlswitch,false);
       d->checkBoxSwitchMap[d->checkBox_msRadl]               = std::make_pair(d->msRadlswitch,false);
       d->senderMapFilled = true;
  }      
  // loop and set initial state
  std::map<QObject*,std::pair<SoSwitch*,bool> >::iterator chbMapIterEnd =  d->checkBoxSwitchMap.end();
  std::map<QObject*,std::pair<SoSwitch*,bool> >::iterator chbMapIter    =  d->checkBoxSwitchMap.find(sender());
  
  if ( chbMapIter != chbMapIterEnd ) {
      QCheckBox* checkBox = dynamic_cast<QCheckBox*>((chbMapIter)->first);
      SoSwitch* soswitch = ((chbMapIter)->second).first;
      if (checkBox && soswitch)
         soswitch->whichChild = checkBox->isChecked() ? SO_SWITCH_ALL : SO_SWITCH_NONE;
  } else {
    message("ERROR: Unknown checkbox");
  }
}

QByteArray
VP1TrackingGeometrySystem::saveState()
{
  //TK: BAD!!!!
  messageDebug("WARNING: VP1TrackingGeometrySystem persistification disabled for now due to crashes!");
  return QByteArray();
  ensureBuildController();
  // Setup stream writing to a byteArray:
  QByteArray byteArray;
  QBuffer buffer(&byteArray);
  buffer.open(QIODevice::WriteOnly);
  QDataStream out(&buffer);

  // Write Data:

  // Version & base state:
  out << (qint32)0; //version
  out << IVP13DSystemSimple::saveState();//Always include state info from the base class.

  out << d->lineEdit_TrackingGeometrySvcName->text();
  out << d->lineEdit_TrackingGeometryName->text();

  std::map<QObject*,std::pair<SoSwitch*,bool> >::iterator chbMapIter    =  d->checkBoxSwitchMap.begin();
  std::map<QObject*,std::pair<SoSwitch*,bool> >::iterator chbMapIterEnd =  d->checkBoxSwitchMap.end();
  for ( ; chbMapIter != chbMapIterEnd; ++chbMapIter ){
       QCheckBox* checkBox = dynamic_cast<QCheckBox*>((chbMapIter)->first);
       if (checkBox) out << checkBox->isChecked();

  }

  // Last step
  buffer.close();
  return byteArray;
}

void
VP1TrackingGeometrySystem::restoreFromState( QByteArray ba)
{
  messageDebug("WARNING: VP1TrackingGeometrySystem persistification disabled for now due to crashes!");
  return;
  // Setup stream for getting the contents of the byteArray:
  QBuffer buffer(&ba);
  buffer.open(QIODevice::ReadOnly);
  QDataStream state(&buffer);

  // Check version and pass on state info to base class:
  qint32 version;
  state >> version;
  if (version!=0)
  {
    message("Warning: State data in .vp1 file is in wrong format - ignoring!");
    return;
  }
  ensureBuildController();

  QByteArray basestate;
  state >> basestate;
  IVP13DSystemSimple::restoreFromState(basestate);

  state >> d->initialTrackingGeometrySvcName;
  state >> d->initialTrackingGeometryName;
  d->lineEdit_TrackingGeometrySvcName->setText( d->initialTrackingGeometrySvcName);
  d->lineEdit_TrackingGeometryName->setText( d->initialTrackingGeometryName);

  retrieveTrackingGeometry();

}
