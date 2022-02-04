/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
  bool retrieveSuccess = false;

  // for signal handling
  bool senderMapFilled = false;
  std::map<QObject*,std::pair<SoSwitch*,bool> >   checkBoxSwitchMap;
  std::vector<VP1ExtraSepLayerHelper*>            sepHelperContainer;

  // Tracking volumes 
  SoSwitch* volswitch = nullptr;
  VP1ExtraSepLayerHelper * sephelper_vol = nullptr;
  
  // Beam pipe
  SoSwitch* beamPipeSwitch = nullptr;
  VP1ExtraSepLayerHelper* sephelper_beamPipe = nullptr;
  
  // Pixel section
  SoSwitch* pixSurfBarrelSwitch = nullptr;
  SoSwitch* pixSurfECASwitch = nullptr;
  SoSwitch* pixSurfECCSwitch = nullptr;
  VP1ExtraSepLayerHelper* sephelper_pixSurfBarrel = nullptr;
  VP1ExtraSepLayerHelper* sephelper_pixSurfECA = nullptr;
  VP1ExtraSepLayerHelper* sephelper_pixSurfECC = nullptr;               

  SoSwitch* pixLayBarrelSwitch = nullptr;
  SoSwitch* pixLayECASwitch = nullptr;
  SoSwitch* pixLayECCSwitch = nullptr;
  VP1ExtraSepLayerHelper* sephelper_pixLayBarrel = nullptr;
  VP1ExtraSepLayerHelper* sephelper_pixLayECA = nullptr;
  VP1ExtraSepLayerHelper* sephelper_pixLayECC = nullptr;               
  
  // SCT section
  SoSwitch* sctSurfBarrelSwitch = nullptr;
  SoSwitch* sctSurfECASwitch = nullptr;
  SoSwitch* sctSurfECCSwitch = nullptr;
  VP1ExtraSepLayerHelper* sephelper_sctSurfBarrel = nullptr;
  VP1ExtraSepLayerHelper* sephelper_sctSurfECA = nullptr;
  VP1ExtraSepLayerHelper* sephelper_sctSurfECC = nullptr;               

  SoSwitch* sctLayBarrelSwitch = nullptr;
  SoSwitch* sctLayECASwitch = nullptr;
  SoSwitch* sctLayECCSwitch = nullptr;
  VP1ExtraSepLayerHelper* sephelper_sctLayBarrel = nullptr;
  VP1ExtraSepLayerHelper* sephelper_sctLayECA = nullptr;
  VP1ExtraSepLayerHelper* sephelper_sctLayECC = nullptr;               

  // TRT section
  SoSwitch* trtSurfBarrelSwitch = nullptr;
  SoSwitch* trtSurfECASwitch = nullptr;
  SoSwitch* trtSurfECCSwitch = nullptr;
  VP1ExtraSepLayerHelper* sephelper_trtSurfBarrel = nullptr;
  VP1ExtraSepLayerHelper* sephelper_trtSurfECA = nullptr;
  VP1ExtraSepLayerHelper* sephelper_trtSurfECC = nullptr;               

  SoSwitch* trtLayBarrelSwitch = nullptr;
  SoSwitch* trtLayECASwitch = nullptr;
  SoSwitch* trtLayECCSwitch = nullptr;
  VP1ExtraSepLayerHelper* sephelper_trtLayBarrel = nullptr;
  VP1ExtraSepLayerHelper* sephelper_trtLayECA = nullptr;
  VP1ExtraSepLayerHelper* sephelper_trtLayECC = nullptr;    
  
  // Passive layers           
  SoSwitch* passiveLayBarrelSwitch = nullptr;
  SoSwitch* passiveLayECASwitch = nullptr;
  SoSwitch* passiveLayECCSwitch = nullptr;
  VP1ExtraSepLayerHelper* sephelper_passiveLayBarrel = nullptr;
  VP1ExtraSepLayerHelper* sephelper_passiveLayECA = nullptr;
  VP1ExtraSepLayerHelper* sephelper_passiveLayECC = nullptr;    

  
  SoSwitch* clipswitch = nullptr;
  SoSwitch* msBswitch = nullptr;
  SoSwitch* msIswitch = nullptr;
  SoSwitch* msOswitch = nullptr;
  SoSwitch* msBIswitch = nullptr;
  SoSwitch* msBMswitch = nullptr;
  SoSwitch* msBOswitch = nullptr;
  SoSwitch* msEIswitch = nullptr;
  SoSwitch* msEMswitch = nullptr;
  SoSwitch* msEOswitch = nullptr;
  SoSwitch* msCSswitch = nullptr;
  SoSwitch* msTGswitch = nullptr;
  SoSwitch* msBIlayswitch = nullptr;
  SoSwitch* msBMlayswitch = nullptr;
  SoSwitch* msBOlayswitch = nullptr;
  SoSwitch* msEIlayswitch = nullptr;
  SoSwitch* msEMlayswitch = nullptr;
  SoSwitch* msEOlayswitch = nullptr;
  SoSwitch* msCSlayswitch = nullptr;
  SoSwitch* msTGlayswitch = nullptr;
  SoSwitch* msBTenvswitch = nullptr;
  SoSwitch* msETenvswitch = nullptr;
  SoSwitch* msShenvswitch = nullptr;
  SoSwitch* msFeenvswitch = nullptr;
  SoSwitch* msRaenvswitch = nullptr;
  SoSwitch* msBTdlswitch = nullptr;
  SoSwitch* msETdlswitch = nullptr;
  SoSwitch* msShdlswitch = nullptr;
  SoSwitch* msFedlswitch = nullptr;
  SoSwitch* msRadlswitch = nullptr;


  VP1ExtraSepLayerHelper * sephelper_msB = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msI = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msO = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msBI = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msBM = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msBO = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msEI = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msEM = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msEO = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msCS = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msTG = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msBIlay = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msBMlay = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msBOlay = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msEIlay = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msEMlay = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msEOlay = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msCSlay = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msTGlay = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msBTenv = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msETenv = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msShenv = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msFeenv = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msRaenv = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msBTdl = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msETdl = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msShdl = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msFedl = nullptr;
  VP1ExtraSepLayerHelper * sephelper_msRadl = nullptr;

  TrackingVolumeToSoNode* trackvol2sonode = nullptr;
  SurfaceToSoNode*        surface2sonode = nullptr;

  // widgets for the sidepane
  QLineEdit*   lineEdit_TrackingGeometrySvcName = nullptr;
  QLineEdit*   lineEdit_TrackingGeometryName = nullptr;
  
  QPushButton* pushButton_Retrieve = nullptr;
  QCheckBox*   checkBox_TrackingVolumes = nullptr;

  QCheckBox*   checkBox_pixSurfBarrel = nullptr;
  QCheckBox*   checkBox_pixSurfECA = nullptr;
  QCheckBox*   checkBox_pixSurfECC = nullptr;               
  QCheckBox*   checkBox_sctSurfBarrel = nullptr;
  QCheckBox*   checkBox_sctSurfECA = nullptr;
  QCheckBox*   checkBox_sctSurfECC = nullptr;
  QCheckBox*   checkBox_trtSurfBarrel = nullptr;
  QCheckBox*   checkBox_trtSurfECA = nullptr;
  QCheckBox*   checkBox_trtSurfECC = nullptr;
               
  QCheckBox*   checkBox_beamPipe = nullptr;
  QCheckBox*   checkBox_pixLayBarrel = nullptr;
  QCheckBox*   checkBox_pixLayECA = nullptr;
  QCheckBox*   checkBox_pixLayECC = nullptr;               
  QCheckBox*   checkBox_sctLayBarrel = nullptr;
  QCheckBox*   checkBox_sctLayECA = nullptr;
  QCheckBox*   checkBox_sctLayECC = nullptr;
  QCheckBox*   checkBox_trtLayBarrel = nullptr;
  QCheckBox*   checkBox_trtLayECA = nullptr;
  QCheckBox*   checkBox_trtLayECC = nullptr;
  QCheckBox*   checkBox_passiveLayBarrel = nullptr;
  QCheckBox*   checkBox_passiveLayECA = nullptr;
  QCheckBox*   checkBox_passiveLayECC = nullptr;

  std::vector<std::string>      surfacesNames;
  
  QCheckBox*   checkBox_ClipPlane = nullptr;
  QCheckBox*   checkBox_MuonStaticFrameB = nullptr;
  QCheckBox*   checkBox_MuonStaticFrameI = nullptr;
  QCheckBox*   checkBox_MuonStaticFrameO = nullptr;
  QCheckBox*   checkBox_msBIvol = nullptr;
  QCheckBox*   checkBox_msBIlay = nullptr;
  QCheckBox*   checkBox_msBMvol = nullptr;
  QCheckBox*   checkBox_msBMlay = nullptr;
  QCheckBox*   checkBox_msBOvol = nullptr;
  QCheckBox*   checkBox_msBOlay = nullptr;
  QCheckBox*   checkBox_msEIvol = nullptr;
  QCheckBox*   checkBox_msEIlay = nullptr;
  QCheckBox*   checkBox_msEMvol = nullptr;
  QCheckBox*   checkBox_msEMlay = nullptr;
  QCheckBox*   checkBox_msEOvol = nullptr;
  QCheckBox*   checkBox_msEOlay = nullptr;
  QCheckBox*   checkBox_msCSvol = nullptr;
  QCheckBox*   checkBox_msCSlay = nullptr;
  QCheckBox*   checkBox_msTGvol = nullptr;
  QCheckBox*   checkBox_msTGlay = nullptr;
  QCheckBox*   checkBox_msBTenv = nullptr;
  QCheckBox*   checkBox_msETenv = nullptr;
  QCheckBox*   checkBox_msShenv = nullptr;
  QCheckBox*   checkBox_msFeenv = nullptr;
  QCheckBox*   checkBox_msRaenv = nullptr;
  QCheckBox*   checkBox_msBTdl = nullptr;
  QCheckBox*   checkBox_msETdl = nullptr;
  QCheckBox*   checkBox_msShdl = nullptr;
  QCheckBox*   checkBox_msFedl = nullptr;
  QCheckBox*   checkBox_msRadl = nullptr;

  // variables keeping some defaults
  QString initialTrackingGeometrySvcName;
  QString initialTrackingGeometryName;

  // global stuff
  StoreGateSvc* detStore = nullptr;
  SoSeparator*  root = nullptr;

  ColorCodeConverter colorCodeConverter;
};

//_____________________________________________________________________________________
VP1TrackingGeometrySystem::VP1TrackingGeometrySystem()
  : IVP13DSystemSimple("TrkGeo",
		       "This system displays the tracking geometry.",
		       "Andreas.Salzburger@cern.ch"),
    m_d( new Imp)
{
  m_d->retrieveSuccess = false;
  m_d->senderMapFilled = false;
  m_d->volswitch = 0;
  
  // beam pipe
  m_d->beamPipeSwitch = 0;
  m_d->sephelper_beamPipe = 0;
  
  // Pixel section
  m_d->pixSurfBarrelSwitch = 0;
  m_d->pixSurfECASwitch  = 0;
  m_d->pixSurfECCSwitch  = 0;
  m_d->sephelper_pixSurfBarrel = 0;
  m_d->sephelper_pixSurfECA  = 0;
  m_d->sephelper_pixSurfECC  = 0;               

  m_d->pixLayBarrelSwitch = 0;
  m_d->pixLayECASwitch  = 0;
  m_d->pixLayECCSwitch  = 0;
  m_d->sephelper_pixLayBarrel = 0;
  m_d->sephelper_pixLayECA  = 0;
  m_d->sephelper_pixLayECC  = 0;            
  
  // SCT section
  m_d->sctSurfBarrelSwitch = 0;
  m_d->sctSurfECASwitch  = 0;
  m_d->sctSurfECCSwitch  = 0;
  m_d->sephelper_sctSurfBarrel = 0;
  m_d->sephelper_sctSurfECA  = 0;
  m_d->sephelper_sctSurfECC  = 0;           

  m_d->sctLayBarrelSwitch = 0;
  m_d->sctLayECASwitch  = 0;
  m_d->sctLayECCSwitch  = 0;
  m_d->sephelper_sctLayBarrel = 0;
  m_d->sephelper_sctLayECA  = 0;
  m_d->sephelper_sctLayECC  = 0;              

  // TRT section
  m_d->trtSurfBarrelSwitch = 0;
  m_d->trtSurfECASwitch  = 0;
  m_d->trtSurfECCSwitch  = 0;
  m_d->sephelper_trtSurfBarrel = 0;
  m_d->sephelper_trtSurfECA  = 0;
  m_d->sephelper_trtSurfECC  = 0;           

  m_d->trtLayBarrelSwitch = 0;
  m_d->trtLayECASwitch  = 0;
  m_d->trtLayECCSwitch  = 0;
  m_d->sephelper_trtLayBarrel = 0;
  m_d->sephelper_trtLayECA  = 0;
  m_d->sephelper_trtLayECC  = 0;

  // passive layers
  m_d->passiveLayBarrelSwitch = 0;
  m_d->passiveLayECASwitch  = 0;
  m_d->passiveLayECCSwitch  = 0;
  m_d->sephelper_passiveLayBarrel = 0;
  m_d->sephelper_passiveLayECA  = 0;
  m_d->sephelper_passiveLayECC  = 0;

  m_d->clipswitch = 0;
  m_d->msBswitch = 0;
  m_d->msIswitch = 0;
  m_d->msOswitch = 0;
  m_d->msBIswitch = 0;
  m_d->msBMswitch = 0;
  m_d->msBOswitch = 0;
  m_d->msEIswitch = 0;
  m_d->msEMswitch = 0;
  m_d->msEOswitch = 0;
  m_d->msCSswitch = 0;
  m_d->msTGswitch = 0;
  m_d->msBIlayswitch = 0;
  m_d->msBMlayswitch = 0;
  m_d->msBOlayswitch = 0;
  m_d->msEIlayswitch = 0;
  m_d->msEMlayswitch = 0;
  m_d->msEOlayswitch = 0;
  m_d->msCSlayswitch = 0;
  m_d->msTGlayswitch = 0;
  m_d->msBTenvswitch = 0;
  m_d->msETenvswitch = 0;
  m_d->msShenvswitch = 0;
  m_d->msFeenvswitch = 0;
  m_d->msRaenvswitch = 0;
  m_d->msBTdlswitch = 0;
  m_d->msETdlswitch = 0;
  m_d->msShdlswitch = 0;
  m_d->msFedlswitch = 0;
  m_d->msRadlswitch = 0;

  m_d->sephelper_vol = 0;
  m_d->sephelper_msB = 0;
  m_d->sephelper_msI = 0;
  m_d->sephelper_msO = 0;
  m_d->sephelper_msBI = 0;
  m_d->sephelper_msBM = 0;
  m_d->sephelper_msBO = 0;
  m_d->sephelper_msEI = 0;
  m_d->sephelper_msEM = 0;
  m_d->sephelper_msEO = 0;
  m_d->sephelper_msCS = 0;
  m_d->sephelper_msTG = 0;
  m_d->sephelper_msBIlay = 0;
  m_d->sephelper_msBMlay = 0;
  m_d->sephelper_msBOlay = 0;
  m_d->sephelper_msEIlay = 0;
  m_d->sephelper_msEMlay = 0;
  m_d->sephelper_msEOlay = 0;
  m_d->sephelper_msCSlay = 0;
  m_d->sephelper_msTGlay = 0;
  m_d->sephelper_msBTenv = 0;
  m_d->sephelper_msETenv = 0;
  m_d->sephelper_msShenv = 0;
  m_d->sephelper_msFeenv = 0;
  m_d->sephelper_msRaenv = 0;
  m_d->sephelper_msBTdl = 0;
  m_d->sephelper_msETdl = 0;
  m_d->sephelper_msShdl = 0;
  m_d->sephelper_msFedl = 0;
  m_d->sephelper_msRadl = 0;

  m_d->trackvol2sonode = 0;
  m_d->surface2sonode = 0;

  m_d->lineEdit_TrackingGeometrySvcName = 0;
  m_d->lineEdit_TrackingGeometryName = 0;
  m_d->pushButton_Retrieve = 0;
  
  m_d->checkBox_pixSurfBarrel     = 0;
  m_d->checkBox_pixSurfECA        = 0;
  m_d->checkBox_pixSurfECC        = 0; 
  m_d->checkBox_sctSurfBarrel     = 0;
  m_d->checkBox_sctSurfECA        = 0;
  m_d->checkBox_sctSurfECC        = 0;
  m_d->checkBox_trtSurfBarrel     = 0;
  m_d->checkBox_trtSurfECA        = 0;
  m_d->checkBox_trtSurfECC        = 0;
  
  m_d->checkBox_beamPipe          = 0;    
  m_d->checkBox_pixLayBarrel      = 0;
  m_d->checkBox_pixLayECA         = 0;
  m_d->checkBox_pixLayECC         = 0;  
  m_d->checkBox_sctLayBarrel      = 0;
  m_d->checkBox_sctLayECA         = 0;
  m_d->checkBox_sctLayECC         = 0;
  m_d->checkBox_trtLayBarrel      = 0;
  m_d->checkBox_trtLayECA         = 0;
  m_d->checkBox_trtLayECC         = 0;
  m_d->checkBox_passiveLayBarrel  = 0;
  m_d->checkBox_passiveLayECA     = 0;
  m_d->checkBox_passiveLayECC     = 0;
  
  m_d->checkBox_TrackingVolumes = 0;
  m_d->checkBox_ClipPlane = 0;
  m_d->checkBox_MuonStaticFrameB = 0;
  m_d->checkBox_MuonStaticFrameI = 0;
  m_d->checkBox_MuonStaticFrameO = 0;
  m_d->checkBox_msBIvol = 0;
  m_d->checkBox_msBMvol = 0;
  m_d->checkBox_msBOvol = 0;
  m_d->checkBox_msEIvol = 0;
  m_d->checkBox_msEMvol = 0;
  m_d->checkBox_msEOvol = 0;
  m_d->checkBox_msCSvol = 0;
  m_d->checkBox_msTGvol = 0;
  m_d->checkBox_msBIlay = 0;
  m_d->checkBox_msBMlay = 0;
  m_d->checkBox_msBOlay = 0;
  m_d->checkBox_msEIlay = 0;
  m_d->checkBox_msEMlay = 0;
  m_d->checkBox_msEOlay = 0;
  m_d->checkBox_msCSlay = 0;
  m_d->checkBox_msTGlay = 0;
  m_d->checkBox_msBTenv = 0;
  m_d->checkBox_msETenv = 0;
  m_d->checkBox_msShenv = 0;
  m_d->checkBox_msFeenv = 0;
  m_d->checkBox_msRaenv = 0;
  m_d->checkBox_msBTdl = 0;
  m_d->checkBox_msETdl = 0;
  m_d->checkBox_msShdl = 0;
  m_d->checkBox_msFedl = 0;
  m_d->checkBox_msRadl = 0;

  m_d->initialTrackingGeometrySvcName = "AtlasTrackingGeometrySvc";
  m_d->initialTrackingGeometryName    = "";

  m_d->detStore = 0;
  m_d->root = 0;
}

//_____________________________________________________________________________________
VP1TrackingGeometrySystem::~VP1TrackingGeometrySystem()
{
    
  std::vector<VP1ExtraSepLayerHelper*>::iterator sepHelperIter    =  m_d->sepHelperContainer.begin();    
  std::vector<VP1ExtraSepLayerHelper*>::iterator sepHelperIterEnd =  m_d->sepHelperContainer.end();    
  for ( ; sepHelperIter != sepHelperIterEnd; ++sepHelperIter){   
      if ((*sepHelperIter)) {
          SoSeparator * sep = (*sepHelperIter)->topSeparator();
          delete (*sepHelperIter);
          sep->unref();
      } 
  }

  delete m_d->trackvol2sonode;
  delete m_d->surface2sonode;
  delete m_d;
}

//_____________________________________________________________________________________
void VP1TrackingGeometrySystem::buildPermanentSceneGraph(StoreGateSvc* detstore, SoSeparator *root)
{
  m_d->detStore = detstore;
  m_d->root = root;
}

//_____________________________________________________________________________________
void VP1TrackingGeometrySystem::retrieveTrackingGeometry()
{
  if( m_d->retrieveSuccess)
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

  if (!m_d->lineEdit_TrackingGeometrySvcName) {
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
           m_d->lineEdit_TrackingGeometrySvcName->text().trimmed() + "'");

  Trk::ITrackingGeometrySvc* trackingGeometrySvc;
  StatusCode status =
      svcLoc->service( m_d->lineEdit_TrackingGeometrySvcName->text().trimmed().toStdString(), trackingGeometrySvc);

  if( !status.isSuccess())
  {
    message("Error: Could not retrieve TrackingGeometrySvc '" +
            m_d->lineEdit_TrackingGeometrySvcName->text().trimmed() + "'\n"
            "Check spelling and if you have added the correct jobOptions fragment");
    return;
  }

  message( "Successfully retrieved TrackingGeometrySvc '" +
           m_d->lineEdit_TrackingGeometrySvcName->text().trimmed() + "'");

  /*-----------------------------------------------------------------------
   *  Retrieving TrackingGeometry
   *-----------------------------------------------------------------------*/
  const Trk::TrackingGeometry * trackingGeometry;
  if( m_d->lineEdit_TrackingGeometryName->text().trimmed() == "")
  {
    message( "Trying to retrieve default TrackingGeometry");
    trackingGeometry = trackingGeometrySvc->trackingGeometry();

    if( !trackingGeometry)
    {
      message("Error: Could not retrieve default TrackingGeometry");
      return;
    }
    else
    {
      message( "Successfully retrieved default TrackingGeometry");
    }
  }
  else
  {
    message( "Trying to retrieve TrackingGeometry '" +
             m_d->lineEdit_TrackingGeometryName->text().trimmed() +
             "' from DetectorStore");
    if (!m_d->detStore)
    {
      message("Error: Got NULL detectorstore.");
      return;
    }

    if (m_d->detStore->retrieve(trackingGeometry, m_d->lineEdit_TrackingGeometryName->text().trimmed().toStdString()).isFailure()||!trackingGeometry)
    {
      message("Error: Could not retrieve TrackingGeometry '" +
              m_d->lineEdit_TrackingGeometryName->text().trimmed() +
              "' from DetectorStore.");
      return;
    }
    else
    {
      message( "Successfully retrieved TrackingGeometry '" +
               m_d->lineEdit_TrackingGeometryName->text().trimmed() + "'");
    }
  }

  /*-----------------------------------------------------------------------
   *  Setup scene if retrieval was successful
   *-----------------------------------------------------------------------*/
  m_d->lineEdit_TrackingGeometrySvcName->setReadOnly( true);
  m_d->lineEdit_TrackingGeometryName->setReadOnly( true);
  m_d->pushButton_Retrieve->setEnabled( false);

  // Pixel section
  m_d->beamPipeSwitch          = new SoSwitch(); 
  m_d->pixSurfBarrelSwitch     = new SoSwitch(); 
  m_d->pixSurfECASwitch        = new SoSwitch(); 
  m_d->pixSurfECCSwitch        = new SoSwitch(); 
  m_d->pixLayBarrelSwitch      = new SoSwitch(); 
  m_d->pixLayECASwitch         = new SoSwitch(); 
  m_d->pixLayECCSwitch         = new SoSwitch(); 
  m_d->sctSurfBarrelSwitch     = new SoSwitch(); 
  m_d->sctSurfECASwitch        = new SoSwitch(); 
  m_d->sctSurfECCSwitch        = new SoSwitch(); 
  m_d->sctLayBarrelSwitch      = new SoSwitch(); 
  m_d->sctLayECASwitch         = new SoSwitch(); 
  m_d->sctLayECCSwitch         = new SoSwitch(); 
  m_d->trtSurfBarrelSwitch     = new SoSwitch(); 
  m_d->trtSurfECASwitch        = new SoSwitch(); 
  m_d->trtSurfECCSwitch        = new SoSwitch(); 
  m_d->trtLayBarrelSwitch      = new SoSwitch(); 
  m_d->trtLayECASwitch         = new SoSwitch(); 
  m_d->trtLayECCSwitch         = new SoSwitch(); 
  m_d->passiveLayBarrelSwitch  = new SoSwitch(); 
  m_d->passiveLayECASwitch     = new SoSwitch(); 
  m_d->passiveLayECCSwitch     = new SoSwitch(); 

  m_d->beamPipeSwitch->whichChild = SO_SWITCH_NONE;
  m_d->pixSurfBarrelSwitch->whichChild = SO_SWITCH_NONE;
  m_d->pixSurfECASwitch->whichChild = SO_SWITCH_NONE;
  m_d->pixSurfECCSwitch->whichChild = SO_SWITCH_NONE; 
  m_d->pixLayBarrelSwitch->whichChild = SO_SWITCH_NONE;  
  m_d->pixLayECASwitch->whichChild = SO_SWITCH_NONE;
  m_d->pixLayECCSwitch->whichChild = SO_SWITCH_NONE; 
  m_d->sctSurfBarrelSwitch->whichChild = SO_SWITCH_NONE;
  m_d->sctSurfECASwitch->whichChild = SO_SWITCH_NONE;
  m_d->sctSurfECCSwitch->whichChild = SO_SWITCH_NONE; 
  m_d->sctLayBarrelSwitch->whichChild = SO_SWITCH_NONE;
  m_d->sctLayECASwitch->whichChild = SO_SWITCH_NONE;
  m_d->sctLayECCSwitch->whichChild = SO_SWITCH_NONE; 
  m_d->trtSurfBarrelSwitch->whichChild = SO_SWITCH_NONE;
  m_d->trtSurfECASwitch->whichChild = SO_SWITCH_NONE;
  m_d->trtSurfECCSwitch->whichChild = SO_SWITCH_NONE; 
  m_d->trtLayBarrelSwitch->whichChild = SO_SWITCH_NONE;
  m_d->trtLayECASwitch->whichChild  = SO_SWITCH_NONE;
  m_d->trtLayECCSwitch->whichChild  = SO_SWITCH_NONE; 
  m_d->passiveLayBarrelSwitch->whichChild = SO_SWITCH_NONE;
  m_d->passiveLayECASwitch->whichChild  = SO_SWITCH_NONE;
  m_d->passiveLayECCSwitch->whichChild  = SO_SWITCH_NONE;

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
   
  m_d->sephelper_beamPipe      = new VP1ExtraSepLayerHelper(sep_beamPipe);
  m_d->sephelper_pixSurfBarrel = new VP1ExtraSepLayerHelper(sep_pixSurfBarrel);
  m_d->sephelper_pixSurfECA  = new VP1ExtraSepLayerHelper(sep_pixSurfECA);
  m_d->sephelper_pixSurfECC  = new VP1ExtraSepLayerHelper(sep_pixSurfECC);               
  m_d->sephelper_pixLayBarrel  = new VP1ExtraSepLayerHelper(sep_pixLayBarrel);
  m_d->sephelper_pixLayECA   = new VP1ExtraSepLayerHelper(sep_pixLayECA);
  m_d->sephelper_pixLayECC   = new VP1ExtraSepLayerHelper(sep_pixLayECC);            
  m_d->sephelper_sctSurfBarrel = new VP1ExtraSepLayerHelper(sep_sctSurfBarrel);
  m_d->sephelper_sctSurfECA  = new VP1ExtraSepLayerHelper(sep_sctSurfECA);
  m_d->sephelper_sctSurfECC  = new VP1ExtraSepLayerHelper(sep_sctSurfECC);           
  m_d->sephelper_sctLayBarrel  = new VP1ExtraSepLayerHelper(sep_sctLayBarrel);
  m_d->sephelper_sctLayECA   = new VP1ExtraSepLayerHelper(sep_sctLayECA);
  m_d->sephelper_sctLayECC   = new VP1ExtraSepLayerHelper(sep_sctLayECC);              
  m_d->sephelper_trtSurfBarrel = new VP1ExtraSepLayerHelper(sep_trtSurfBarrel);
  m_d->sephelper_trtSurfECA  = new VP1ExtraSepLayerHelper(sep_trtSurfECA);
  m_d->sephelper_trtSurfECC  = new VP1ExtraSepLayerHelper(sep_trtSurfECC);           
  m_d->sephelper_trtLayBarrel  = new VP1ExtraSepLayerHelper(sep_trtLayBarrel);
  m_d->sephelper_trtLayECA   = new VP1ExtraSepLayerHelper(sep_trtLayECA);
  m_d->sephelper_trtLayECC   = new VP1ExtraSepLayerHelper(sep_trtLayECC);
  m_d->sephelper_passiveLayBarrel  = new VP1ExtraSepLayerHelper(sep_passiveLayBarrel);
  m_d->sephelper_passiveLayECA   = new VP1ExtraSepLayerHelper(sep_passiveLayECA );
  m_d->sephelper_passiveLayECC   = new VP1ExtraSepLayerHelper(sep_passiveLayECC );  
  
  m_d->volswitch = new SoSwitch();
  m_d->clipswitch = new SoSwitch();
  m_d->msBswitch = new SoSwitch();
  m_d->msIswitch = new SoSwitch();
  m_d->msOswitch = new SoSwitch();
  m_d->msBIswitch = new SoSwitch();
  m_d->msBMswitch = new SoSwitch();
  m_d->msBOswitch = new SoSwitch();
  m_d->msEIswitch = new SoSwitch();
  m_d->msEMswitch = new SoSwitch();
  m_d->msEOswitch = new SoSwitch();
  m_d->msCSswitch = new SoSwitch();
  m_d->msTGswitch = new SoSwitch();
  m_d->msBIlayswitch = new SoSwitch();
  m_d->msBMlayswitch = new SoSwitch();
  m_d->msBOlayswitch = new SoSwitch();
  m_d->msEIlayswitch = new SoSwitch();
  m_d->msEMlayswitch = new SoSwitch();
  m_d->msEOlayswitch = new SoSwitch();
  m_d->msCSlayswitch = new SoSwitch();
  m_d->msTGlayswitch = new SoSwitch();
  m_d->msBTenvswitch = new SoSwitch();
  m_d->msETenvswitch = new SoSwitch();
  m_d->msShenvswitch = new SoSwitch();
  m_d->msFeenvswitch = new SoSwitch();
  m_d->msRaenvswitch = new SoSwitch();
  m_d->msBTdlswitch = new SoSwitch();
  m_d->msETdlswitch = new SoSwitch();
  m_d->msShdlswitch = new SoSwitch();
  m_d->msFedlswitch = new SoSwitch();
  m_d->msRadlswitch = new SoSwitch();

  m_d->volswitch->whichChild = SO_SWITCH_NONE;
  m_d->clipswitch->whichChild = SO_SWITCH_NONE;
  m_d->msBswitch->whichChild = SO_SWITCH_NONE;
  m_d->msIswitch->whichChild = SO_SWITCH_NONE;
  m_d->msOswitch->whichChild = SO_SWITCH_NONE;
  m_d->msBIswitch->whichChild = SO_SWITCH_NONE;
  m_d->msBMswitch->whichChild = SO_SWITCH_NONE;
  m_d->msBOswitch->whichChild = SO_SWITCH_NONE;
  m_d->msEIswitch->whichChild = SO_SWITCH_NONE;
  m_d->msEMswitch->whichChild = SO_SWITCH_NONE;
  m_d->msEOswitch->whichChild = SO_SWITCH_NONE;
  m_d->msCSswitch->whichChild = SO_SWITCH_NONE;
  m_d->msTGswitch->whichChild = SO_SWITCH_NONE;
  m_d->msBIlayswitch->whichChild = SO_SWITCH_NONE;
  m_d->msBMlayswitch->whichChild = SO_SWITCH_NONE;
  m_d->msBOlayswitch->whichChild = SO_SWITCH_NONE;
  m_d->msEIlayswitch->whichChild = SO_SWITCH_NONE;
  m_d->msEMlayswitch->whichChild = SO_SWITCH_NONE;
  m_d->msEOlayswitch->whichChild = SO_SWITCH_NONE;
  m_d->msCSlayswitch->whichChild = SO_SWITCH_NONE;
  m_d->msTGlayswitch->whichChild = SO_SWITCH_NONE;
  m_d->msBTenvswitch->whichChild = SO_SWITCH_NONE;
  m_d->msETenvswitch->whichChild = SO_SWITCH_NONE;
  m_d->msShenvswitch->whichChild = SO_SWITCH_NONE;
  m_d->msFeenvswitch->whichChild = SO_SWITCH_NONE;
  m_d->msRaenvswitch->whichChild = SO_SWITCH_NONE;
  m_d->msBTdlswitch->whichChild = SO_SWITCH_NONE;
  m_d->msETdlswitch->whichChild = SO_SWITCH_NONE;
  m_d->msShdlswitch->whichChild = SO_SWITCH_NONE;
  m_d->msFedlswitch->whichChild = SO_SWITCH_NONE;
  m_d->msRadlswitch->whichChild = SO_SWITCH_NONE;

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

  m_d->sephelper_vol = new VP1ExtraSepLayerHelper(sep_vol);
  m_d->sephelper_msB = new VP1ExtraSepLayerHelper(sep_msB);
  m_d->sephelper_msI = new VP1ExtraSepLayerHelper(sep_msI);
  m_d->sephelper_msO = new VP1ExtraSepLayerHelper(sep_msO);
  m_d->sephelper_msBI = new VP1ExtraSepLayerHelper(sep_msBI);
  m_d->sephelper_msBM = new VP1ExtraSepLayerHelper(sep_msBM);
  m_d->sephelper_msBO = new VP1ExtraSepLayerHelper(sep_msBO);
  m_d->sephelper_msEI = new VP1ExtraSepLayerHelper(sep_msEI);
  m_d->sephelper_msEM = new VP1ExtraSepLayerHelper(sep_msEM);
  m_d->sephelper_msEO = new VP1ExtraSepLayerHelper(sep_msEO);
  m_d->sephelper_msCS = new VP1ExtraSepLayerHelper(sep_msCS);
  m_d->sephelper_msTG = new VP1ExtraSepLayerHelper(sep_msTG);
  m_d->sephelper_msBIlay = new VP1ExtraSepLayerHelper(sep_msBIlay);
  m_d->sephelper_msBMlay = new VP1ExtraSepLayerHelper(sep_msBMlay);
  m_d->sephelper_msBOlay = new VP1ExtraSepLayerHelper(sep_msBOlay);
  m_d->sephelper_msEIlay = new VP1ExtraSepLayerHelper(sep_msEIlay);
  m_d->sephelper_msEMlay = new VP1ExtraSepLayerHelper(sep_msEMlay);
  m_d->sephelper_msEOlay = new VP1ExtraSepLayerHelper(sep_msEOlay);
  m_d->sephelper_msCSlay = new VP1ExtraSepLayerHelper(sep_msCSlay);
  m_d->sephelper_msTGlay = new VP1ExtraSepLayerHelper(sep_msTGlay);
  m_d->sephelper_msBTenv = new VP1ExtraSepLayerHelper(sep_msBTenv);
  m_d->sephelper_msETenv = new VP1ExtraSepLayerHelper(sep_msETenv);
  m_d->sephelper_msShenv = new VP1ExtraSepLayerHelper(sep_msShenv);
  m_d->sephelper_msFeenv = new VP1ExtraSepLayerHelper(sep_msFeenv);
  m_d->sephelper_msRaenv = new VP1ExtraSepLayerHelper(sep_msRaenv);
  m_d->sephelper_msBTdl = new VP1ExtraSepLayerHelper(sep_msBTdl);
  m_d->sephelper_msETdl = new VP1ExtraSepLayerHelper(sep_msETdl);
  m_d->sephelper_msShdl = new VP1ExtraSepLayerHelper(sep_msShdl);
  m_d->sephelper_msFedl = new VP1ExtraSepLayerHelper(sep_msFedl);
  m_d->sephelper_msRadl = new VP1ExtraSepLayerHelper(sep_msRadl);
  
  m_d->sepHelperContainer.reserve(100);
  m_d->sepHelperContainer.push_back(m_d->sephelper_vol);
  m_d->sepHelperContainer.push_back(m_d->sephelper_beamPipe);
  m_d->sepHelperContainer.push_back(m_d->sephelper_pixSurfBarrel);
  m_d->sepHelperContainer.push_back(m_d->sephelper_pixSurfECA);
  m_d->sepHelperContainer.push_back(m_d->sephelper_pixSurfECC);               
  m_d->sepHelperContainer.push_back(m_d->sephelper_pixLayBarrel);
  m_d->sepHelperContainer.push_back(m_d->sephelper_pixLayECA);
  m_d->sepHelperContainer.push_back(m_d->sephelper_pixLayECC);               
  m_d->sepHelperContainer.push_back(m_d->sephelper_sctSurfBarrel);
  m_d->sepHelperContainer.push_back(m_d->sephelper_sctSurfECA);
  m_d->sepHelperContainer.push_back(m_d->sephelper_sctSurfECC);               
  m_d->sepHelperContainer.push_back(m_d->sephelper_sctLayBarrel);
  m_d->sepHelperContainer.push_back(m_d->sephelper_sctLayECA);
  m_d->sepHelperContainer.push_back(m_d->sephelper_sctLayECC);               
  m_d->sepHelperContainer.push_back(m_d->sephelper_trtSurfBarrel);
  m_d->sepHelperContainer.push_back(m_d->sephelper_trtSurfECA);
  m_d->sepHelperContainer.push_back(m_d->sephelper_trtSurfECC);               
  m_d->sepHelperContainer.push_back(m_d->sephelper_trtLayBarrel);
  m_d->sepHelperContainer.push_back(m_d->sephelper_trtLayECA);
  m_d->sepHelperContainer.push_back(m_d->sephelper_trtLayECC);    
  m_d->sepHelperContainer.push_back(m_d->sephelper_passiveLayBarrel);
  m_d->sepHelperContainer.push_back(m_d->sephelper_passiveLayECA);
  m_d->sepHelperContainer.push_back(m_d->sephelper_passiveLayECC);    
  m_d->sepHelperContainer.push_back(m_d->sephelper_msB);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msI);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msO);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msBI);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msBM);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msBO);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msEI);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msEM);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msEO);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msCS);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msTG);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msBIlay);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msBMlay);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msBOlay);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msEIlay);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msEMlay);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msEOlay);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msCSlay);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msTGlay);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msBTenv);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msETenv);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msShenv);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msFeenv);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msRaenv);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msBTdl);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msETdl);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msShdl);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msFedl);
  m_d->sepHelperContainer.push_back(m_d->sephelper_msRadl);

  std::cout << "To here !" << std::endl;

  const Trk::TrackingVolume* highestVolume = trackingGeometry->highestTrackingVolume();

  if (!highestVolume) {
    message("Error: No highest tracking volume registered.");
    return;
  }

  const Trk::TrackingVolume* idCalo = trackingGeometry->trackingVolume("MuonSpectrometerEntrance");

  m_d->trackvol2sonode = new TrackingVolumeToSoNode();
  m_d->surface2sonode = new SurfaceToSoNode();

  if (idCalo) processTrkVolume(idCalo);
  else processTrkVolume(highestVolume);

  processMsVolume(highestVolume,0,0);

  SoRotationXYZ* rotClip = new SoRotationXYZ;
  rotClip->axis.setValue( SoRotationXYZ::X);
  rotClip->angle.setValue( -M_PI/2.);

  m_d->clipswitch->addChild( rotClip);
  m_d->clipswitch->addChild( new SoClipPlane);
  m_d->root->addChild(m_d->clipswitch);

  m_d->volswitch->addChild(sep_vol);

  m_d->beamPipeSwitch->addChild(sep_beamPipe);  
  m_d->pixSurfBarrelSwitch->addChild(sep_pixSurfBarrel);
  m_d->pixSurfECASwitch->addChild(sep_pixSurfECA);
  m_d->pixSurfECCSwitch->addChild(sep_pixSurfECC);
  m_d->pixLayBarrelSwitch->addChild(sep_pixLayBarrel);
  m_d->pixLayECASwitch->addChild(sep_pixLayECA);
  m_d->pixLayECCSwitch->addChild(sep_pixLayECC);
  m_d->sctSurfBarrelSwitch->addChild(sep_sctSurfBarrel);
  m_d->sctSurfECASwitch->addChild(sep_sctSurfECA);
  m_d->sctSurfECCSwitch->addChild(sep_sctSurfECC);
  m_d->sctLayBarrelSwitch->addChild(sep_sctLayBarrel);
  m_d->sctLayECASwitch->addChild(sep_sctLayECA);
  m_d->sctLayECCSwitch->addChild(sep_sctLayECC);
  m_d->trtSurfBarrelSwitch->addChild(sep_trtSurfBarrel);
  m_d->trtSurfECASwitch->addChild(sep_trtSurfECA);
  m_d->trtSurfECCSwitch->addChild(sep_trtSurfECC);
  m_d->trtLayBarrelSwitch->addChild(sep_trtLayBarrel);
  m_d->trtLayECASwitch->addChild(sep_trtLayECA);
  m_d->trtLayECCSwitch->addChild(sep_trtLayECC);
  m_d->passiveLayBarrelSwitch->addChild(sep_passiveLayBarrel);
  m_d->passiveLayECASwitch->addChild(sep_passiveLayECA);
  m_d->passiveLayECCSwitch->addChild(sep_passiveLayECC);
  
  m_d->msBswitch->addChild(sep_msB);
  m_d->msIswitch->addChild(sep_msI);
  m_d->msOswitch->addChild(sep_msO);
  m_d->msBIswitch->addChild(sep_msBI);
  m_d->msBMswitch->addChild(sep_msBM);
  m_d->msBOswitch->addChild(sep_msBO);
  m_d->msEIswitch->addChild(sep_msEI);
  m_d->msEMswitch->addChild(sep_msEM);
  m_d->msEOswitch->addChild(sep_msEO);
  m_d->msCSswitch->addChild(sep_msCS);
  m_d->msTGswitch->addChild(sep_msTG);
  m_d->msBIlayswitch->addChild(sep_msBIlay);
  m_d->msBMlayswitch->addChild(sep_msBMlay);
  m_d->msBOlayswitch->addChild(sep_msBOlay);
  m_d->msEIlayswitch->addChild(sep_msEIlay);
  m_d->msEMlayswitch->addChild(sep_msEMlay);
  m_d->msEOlayswitch->addChild(sep_msEOlay);
  m_d->msCSlayswitch->addChild(sep_msCSlay);
  m_d->msTGlayswitch->addChild(sep_msTGlay);
  m_d->msBTenvswitch->addChild(sep_msBTenv);
  m_d->msETenvswitch->addChild(sep_msETenv);
  m_d->msShenvswitch->addChild(sep_msShenv);
  m_d->msFeenvswitch->addChild(sep_msFeenv);
  m_d->msRaenvswitch->addChild(sep_msRaenv);
  m_d->msBTdlswitch->addChild(sep_msBTdl);
  m_d->msETdlswitch->addChild(sep_msETdl);
  m_d->msShdlswitch->addChild(sep_msShdl);
  m_d->msFedlswitch->addChild(sep_msFedl);
  m_d->msRadlswitch->addChild(sep_msRadl);

  m_d->root->addChild(m_d->volswitch);
  m_d->root->addChild(m_d->beamPipeSwitch);          
  m_d->root->addChild(m_d->pixSurfBarrelSwitch);     
  m_d->root->addChild(m_d->pixSurfECASwitch);        
  m_d->root->addChild(m_d->pixSurfECCSwitch);        
  m_d->root->addChild(m_d->pixLayBarrelSwitch);      
  m_d->root->addChild(m_d->pixLayECASwitch);         
  m_d->root->addChild(m_d->pixLayECCSwitch);         
  m_d->root->addChild(m_d->sctSurfBarrelSwitch);     
  m_d->root->addChild(m_d->sctSurfECASwitch);        
  m_d->root->addChild(m_d->sctSurfECCSwitch);        
  m_d->root->addChild(m_d->sctLayBarrelSwitch);      
  m_d->root->addChild(m_d->sctLayECASwitch);         
  m_d->root->addChild(m_d->sctLayECCSwitch);        
  m_d->root->addChild(m_d->trtSurfBarrelSwitch);     
  m_d->root->addChild(m_d->trtSurfECASwitch);        
  m_d->root->addChild(m_d->trtSurfECCSwitch);        
  m_d->root->addChild(m_d->trtLayBarrelSwitch);      
  m_d->root->addChild(m_d->trtLayECASwitch);         
  m_d->root->addChild(m_d->trtLayECCSwitch);         
  m_d->root->addChild(m_d->passiveLayBarrelSwitch);  
  m_d->root->addChild(m_d->passiveLayECASwitch);     
  m_d->root->addChild(m_d->passiveLayECCSwitch);     
  m_d->root->addChild(m_d->msBswitch);
  m_d->root->addChild(m_d->msIswitch);
  m_d->root->addChild(m_d->msOswitch);
  m_d->root->addChild(m_d->msBIswitch);
  m_d->root->addChild(m_d->msBMswitch);
  m_d->root->addChild(m_d->msBOswitch);
  m_d->root->addChild(m_d->msEIswitch);
  m_d->root->addChild(m_d->msEMswitch);
  m_d->root->addChild(m_d->msEOswitch);
  m_d->root->addChild(m_d->msCSswitch);
  m_d->root->addChild(m_d->msTGswitch);
  m_d->root->addChild(m_d->msBIlayswitch);
  m_d->root->addChild(m_d->msBMlayswitch);
  m_d->root->addChild(m_d->msBOlayswitch);
  m_d->root->addChild(m_d->msEIlayswitch);
  m_d->root->addChild(m_d->msEMlayswitch);
  m_d->root->addChild(m_d->msEOlayswitch);
  m_d->root->addChild(m_d->msCSlayswitch);
  m_d->root->addChild(m_d->msTGlayswitch);
  m_d->root->addChild(m_d->msBTenvswitch);
  m_d->root->addChild(m_d->msETenvswitch);
  m_d->root->addChild(m_d->msShenvswitch);
  m_d->root->addChild(m_d->msFeenvswitch);
  m_d->root->addChild(m_d->msRaenvswitch);
  m_d->root->addChild(m_d->msBTdlswitch);
  m_d->root->addChild(m_d->msETdlswitch);
  m_d->root->addChild(m_d->msShdlswitch);
  m_d->root->addChild(m_d->msFedlswitch);
  m_d->root->addChild(m_d->msRadlswitch);

  m_d->retrieveSuccess = true;
}

//_____________________________________________________________________________________
void VP1TrackingGeometrySystem::processTrkVolume(const Trk::TrackingVolume* tvol)
{
  if (!tvol)
    return;
  
  SoNode * volnode = m_d->trackvol2sonode->translateTrackingVolume(*tvol);
  if (!volnode) {
    message("Error: Volume shape not implemented");
    return;
  }
  
  QString volumeName(tvol->volumeName().c_str());
  
  messageVerbose("Processing volume: "+volumeName);
  m_d->sephelper_vol->addNodeUnderMaterial(volnode,m_d->colorCodeConverter.getMaterialFromColorCode( tvol->colorCode()));
  
  const Trk::BinnedArray< Trk::Layer >* confinedLayers = tvol->confinedLayers();
  
  // loop over confined layers
  if (confinedLayers){
    Trk::BinnedArraySpan<Trk::Layer const * const> layerVector =  confinedLayers->arrayObjects();
    Trk::BinnedArraySpan<Trk::Layer const * const>::const_iterator layerIter = layerVector.begin();
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
            layerSepHelper = m_d->sephelper_beamPipe;
        } else if (volumeName.contains("Pixel") > 0 ) {
        // Pixel sector 
          if ( volumeName.contains("Barrel") > 0 ){
              // pixel barrel surfaces & layers
              layerSepHelper   = m_d->sephelper_pixLayBarrel;
              surfaceSepHelper = m_d->sephelper_pixSurfBarrel;
          } else if ( volumeName.contains("Endcap") > 0 ){
              // pixel endcap surfaces & layers
              if ( volumeName.contains("Pos") > 0){
                  layerSepHelper   = m_d->sephelper_pixLayECA;
                  surfaceSepHelper = m_d->sephelper_pixSurfECA;
              } else {
                  layerSepHelper   = m_d->sephelper_pixLayECC;
                  surfaceSepHelper = m_d->sephelper_pixSurfECC;
              }
           }    
        } else if ((volumeName.contains("SCT") > 0) or (volumeName.contains("Strip") > 0)) {
            // SCT sector
            if ( volumeName.contains("Barrel") > 0 ){
                // sct barrel surfaces & layers
                layerSepHelper   = m_d->sephelper_sctLayBarrel;
                surfaceSepHelper = m_d->sephelper_sctSurfBarrel;
            } else if ( volumeName.contains("Endcap") > 0 ){
                // sct endcap surfaces & layers
                if ( volumeName.contains("Pos") > 0){
                    layerSepHelper   = m_d->sephelper_sctLayECA;
                    surfaceSepHelper = m_d->sephelper_sctSurfECA;
                } else {
                    layerSepHelper   = m_d->sephelper_sctLayECC;
                    surfaceSepHelper = m_d->sephelper_sctSurfECC;
                }
            }   
        } else if (volumeName.contains("TRT") > 0) {
          // TRT sector
          if ( volumeName.contains("Barrel") > 0 ){
              // trt barrel surfaces & layers
              layerSepHelper   = m_d->sephelper_trtLayBarrel;
              surfaceSepHelper = m_d->sephelper_trtSurfBarrel;
          } else if ( volumeName.contains("Endcap") > 0 ){
              // trt endcap surfaces & layers
              if ( volumeName.contains("Pos") > 0 ){
                  layerSepHelper   = m_d->sephelper_trtLayECA;
                  surfaceSepHelper = m_d->sephelper_trtSurfECA;
              } else {
                  layerSepHelper   = m_d->sephelper_trtLayECC;
                  surfaceSepHelper = m_d->sephelper_trtSurfECC;
              }
          }
        } else {
             if ( volumeName.contains("Barrel") > 0 ){
                  // trt barrel surfaces & layers
                  layerSepHelper   = m_d->sephelper_passiveLayBarrel;
              } else if ( volumeName.contains("Endcap") > 0 ){
                  // trt endcap surfaces & layers
                  if ( volumeName.contains("Pos") > 0 ){
                      layerSepHelper   = m_d->sephelper_passiveLayECA;
                  } else {
                      layerSepHelper   = m_d->sephelper_passiveLayECC;
                  }
              }
        }
        
        std::cout << " VolumeName " << tvol->volumeName() << " / seperators : " << layerSepHelper << " / " << surfaceSepHelper << std::endl;
        
        // process the layer with the correct separator helper
        if (layerSepHelper) processTrkLayer(*layerIter, layerSepHelper, tvol->colorCode());
        // Surface
        if (layerSubSurfaces && surfaceSepHelper){
          Trk::BinnedArraySpan<Trk::Surface const * const> surfaceVector = layerSubSurfaces->arrayObjects();
          Trk::BinnedArraySpan<Trk::Surface const * const>::const_iterator surfaceIter = surfaceVector.begin();
          for ( ; surfaceIter != surfaceVector.end(); ++surfaceIter ){
            // push_back the surface
            if (*surfaceIter)
              processTrkSurface(*surfaceIter, surfaceSepHelper, tvol->colorCode());
          } 
        }
      }
    }
  }
  
  const Trk::BinnedArray< const Trk::TrackingVolume >* confinedVolumes = tvol->confinedVolumes();
  // get the confined volumes and loop over it -> call recursively
  if (confinedVolumes){
    Trk::BinnedArraySpan<Trk::TrackingVolume const * const> volumes = confinedVolumes->arrayObjects();
    Trk::BinnedArraySpan<Trk::TrackingVolume const * const>::const_iterator volIter = volumes.begin();
    Trk::BinnedArraySpan<Trk::TrackingVolume const * const>::const_iterator volIterEnd = volumes.end();
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
    SoNode * volnode = m_d->trackvol2sonode->translateTrackingVolume(*tvol);
    if (!volnode) {
      message("Error: Volume shape not implemented");
      return;
    }

    if ( name.substr(0,4) == "Muon" && tvol->colorCode() > 0 ) {
      if ( fabs(tvol->center().z())>12900 ) m_d->sephelper_msO->addNodeUnderMaterial(volnode,m_d->colorCodeConverter.getMaterialFromColorCode( tvol->colorCode()));
      else if ( fabs(tvol->center().z())>6750  ) m_d->sephelper_msI->addNodeUnderMaterial(volnode,m_d->colorCodeConverter.getMaterialFromColorCode( tvol->colorCode()));
      else m_d->sephelper_msB->addNodeUnderMaterial(volnode,m_d->colorCodeConverter.getMaterialFromColorCode( tvol->colorCode()));
    }

    // envelopes
    if ( sepHelper ) {
      unsigned int colorCode = 17;
      if ( name.substr(0,3)=="MDT") colorCode = 2;
      if ( name.substr(0,3)=="RPC") colorCode = 4;
      if ( name.substr(0,2)=="CS")  colorCode = 5;
      if ( name.substr(0,1)=="T")   colorCode = 6;
      sepHelper->addNodeUnderMaterial(volnode,m_d->colorCodeConverter.getMaterialFromColorCode(colorCode) );
      if (layHelper) processMsLayDense( tvol, sepHelper, layHelper, colorCode );
    } else if  ( layHelper ) {
      unsigned int colorCode = 13;
      layHelper->addNodeUnderMaterial(volnode,m_d->colorCodeConverter.getMaterialFromColorCode(colorCode) );
    }

  } else {
    const Trk::BinnedArray< const Trk::TrackingVolume >* confinedVolumes = tvol->confinedVolumes();
    // get the confined volumes and loop over it -> call recursively
    if (confinedVolumes){
      Trk::BinnedArraySpan<Trk::TrackingVolume const * const> volumes = confinedVolumes->arrayObjects();
      Trk::BinnedArraySpan<Trk::TrackingVolume const * const>::const_iterator volIter = volumes.begin();
      Trk::BinnedArraySpan<Trk::TrackingVolume const * const>::const_iterator volIterEnd = volumes.end();
      for ( ; volIter != volIterEnd; ++volIter)
	if (*volIter) processMsVolume(*volIter,sepHelper, layHelper);
    }
  }

  const std::vector<const Trk::DetachedTrackingVolume*>* detVols = tvol->confinedDetachedVolumes();
  if (detVols) {
    // identify separators
    std::vector<const Trk::DetachedTrackingVolume*>::const_iterator dIter = detVols->begin();
    for ( ;dIter!=detVols->end(); ++dIter) {
      if ( (tvol->inside((*dIter)->trackingVolume()->center(),0.) || (*dIter)->trackingVolume()->center().perp()<0.001) ) {
	std::string name = (*dIter)->name();
        if ( name.substr(0,2) == "BI" ) { sepHelper = m_d->sephelper_msBI; layHelper =  m_d->sephelper_msBIlay; }
        else if ( name.substr(0,2) == "BM" ) { sepHelper = m_d->sephelper_msBM; layHelper =  m_d->sephelper_msBMlay; }
        else if ( name.substr(0,2) == "BO" ) { sepHelper = m_d->sephelper_msBO; layHelper =  m_d->sephelper_msBOlay; }
        else if ( name.substr(0,2) == "EI" ) { sepHelper = m_d->sephelper_msEI; layHelper =  m_d->sephelper_msEIlay; }
        else if ( name.substr(0,2) == "EM" ) { sepHelper = m_d->sephelper_msEM; layHelper =  m_d->sephelper_msEMlay; }
        else if ( name.substr(0,2) == "EO" ) { sepHelper = m_d->sephelper_msEO; layHelper =  m_d->sephelper_msEOlay; }
        else if ( name.substr(0,2) == "CS" ) { sepHelper = m_d->sephelper_msCS; layHelper =  m_d->sephelper_msCSlay; }
        else if ( name.substr(0,1) == "T" )  { sepHelper = m_d->sephelper_msTG; layHelper =  m_d->sephelper_msTGlay; }
        else if ( name.substr(0,4) == "Rail" ) { sepHelper = m_d->sephelper_msRaenv; layHelper =  m_d->sephelper_msRadl; }
        else if ( name.substr(0,7) == "MDTRail" ) { sepHelper = m_d->sephelper_msRaenv; layHelper =  m_d->sephelper_msRadl; }
        else if ( name.substr(0,4) == "SADL" ) { sepHelper = m_d->sephelper_msRaenv; layHelper =  m_d->sephelper_msRadl; }
        else if ( name.substr(0,4) == "TBWH" ) { sepHelper = m_d->sephelper_msRaenv; layHelper =  m_d->sephelper_msRadl; }
	else if ( name.substr(0,4) == "MBAP" ) { sepHelper = m_d->sephelper_msRaenv; layHelper =  m_d->sephelper_msRadl; }
	else if ( name.substr(0,4) == "MBWH" ) { sepHelper = m_d->sephelper_msRaenv; layHelper =  m_d->sephelper_msRadl; }
	else if ( name.substr(0,3) == "PP2" ) { sepHelper = m_d->sephelper_msRaenv; layHelper =  m_d->sephelper_msRadl; }
        else if ( name.substr(0,3) == "ECT" )  { sepHelper = m_d->sephelper_msETenv; layHelper =  m_d->sephelper_msETdl; }
        else if ( name.substr(0,2) == "BT" || name.substr(0,6) == "EdgeBT" || name.substr(0,6) == "HeadBT" )
	  { sepHelper = m_d->sephelper_msBTenv; layHelper =  m_d->sephelper_msBTdl; }
        else if ( name.substr(0,3) == "BAR" )
		  // && ( name.substr(0,18) == "BAR_ToroidBAR_Cold" || name.substr(0,22) == "BAR_ToroidBAR_Voussoir")  )
	  { sepHelper = m_d->sephelper_msBTenv; layHelper =  m_d->sephelper_msBTdl; }
        else if ( name.substr(0,9) == "ForwardSh" || name.substr(0,8) == "ToroidSh" || name.substr(0,6) == "DiskSh" )
	  { sepHelper = m_d->sephelper_msShenv; layHelper =  m_d->sephelper_msShdl; }
        else if ( name.substr(0,5) == "StdFe" || name.substr(0,6) == "ExtrFe" || name.substr(0,6) == "ConnFe" )
	  { sepHelper = m_d->sephelper_msFeenv; layHelper =  m_d->sephelper_msFedl; }
        else if ( name.substr(0,4) == "Feet" )
	  { sepHelper = m_d->sephelper_msFeenv; layHelper =  m_d->sephelper_msFedl; }
        else   // display somehow
	  { sepHelper = m_d->sephelper_msFeenv; layHelper =  m_d->sephelper_msFedl; }
      } else {
	std::string name = (*dIter)->name();
	//std::cout << "VP1 ignores volume:"<<name<< std::endl;
	if ( name.substr(0,3) == "BAR" &&
	     name.substr(0,18) == "BAR_ToroidBAR_Cold" )
	  { sepHelper = m_d->sephelper_msBTenv; layHelper =  m_d->sephelper_msBTdl; }
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
  SoNode * node = m_d->surface2sonode->translateSurface(tlayer->surfaceRepresentation());
  if (node) {
      sepLayerHelper->addNodeUnderMaterial(node,m_d->colorCodeConverter.getMaterialFromColorCode( colorCode));
      std::cout << "Adding Layer to Separator helper: " << sepLayerHelper << std::endl;
  }
}

//_____________________________________________________________________________________
void VP1TrackingGeometrySystem::processTrkSurface(const Trk::Surface*tsurf,
                                                 VP1ExtraSepLayerHelper* sepLayerHelper,
                                                  unsigned int colorCode)
{
  if (!tsurf) return;
  SoNode * node = m_d->surface2sonode->translateSurface(*tsurf);
  if (node) sepLayerHelper->addNodeUnderMaterial(node,m_d->colorCodeConverter.getMaterialFromColorCode( colorCode));
}

//_____________________________________________________________________________________
void VP1TrackingGeometrySystem::processMsLayDense(const Trk::TrackingVolume* tvol, VP1ExtraSepLayerHelper* sepHelper, VP1ExtraSepLayerHelper* layHelper, int color)
{
  if (!tvol) return;

  // ordered layers
  const Trk::BinnedArray< Trk::Layer >* confinedLayers = tvol->confinedLayers();
  if (confinedLayers){
    Trk::BinnedArraySpan<Trk::Layer const * const> layerVector =  confinedLayers->arrayObjects();
    Trk::BinnedArraySpan<Trk::Layer const * const>::const_iterator layerIter = layerVector.begin();
    // loop over layers
    for ( ; layerIter != layerVector.end(); ++layerIter){
      // push_back the layer
      if (*layerIter){
	SoNode * node = m_d->surface2sonode->translateSurface((*layerIter)->surfaceRepresentation());
	if (node && layHelper ) {
          if ((*layerIter)->layerType()>0)
	    layHelper->addNodeUnderMaterial(node,m_d->colorCodeConverter.getMaterialFromColorCode( color));
          else
	    layHelper->addNodeUnderMaterial(node,m_d->colorCodeConverter.getMaterialFromColorCode( 1));
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
	SoNode * node = m_d->surface2sonode->translateSurface((*layerIter)->surfaceRepresentation());
	if (node && layHelper ) {
          if ((*layerIter)->layerType()>0)
	    layHelper->addNodeUnderMaterial(node,m_d->colorCodeConverter.getMaterialFromColorCode( color));
          else
	    layHelper->addNodeUnderMaterial(node,m_d->colorCodeConverter.getMaterialFromColorCode( 1));
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

  m_d->lineEdit_TrackingGeometrySvcName = ui.lineEdit_TrackingGeometrySvcName;
  m_d->lineEdit_TrackingGeometryName = ui.lineEdit_TrackingGeometryName;

  m_d->lineEdit_TrackingGeometrySvcName->setText( m_d->initialTrackingGeometrySvcName);
  m_d->lineEdit_TrackingGeometryName->setText( m_d->initialTrackingGeometryName);

  m_d->pushButton_Retrieve = ui.pushButton_Retrieve;
  m_d->checkBox_TrackingVolumes = ui.checkBox_TrackingVolumes;

  m_d->checkBox_beamPipe         = ui.checkBox_BeamPipeLayer;
  m_d->checkBox_pixLayECA        = ui.checkBox_PixelLayersECA;
  m_d->checkBox_pixLayBarrel     = ui.checkBox_PixelLayersBarrel;
  m_d->checkBox_pixLayECC        = ui.checkBox_PixelLayersECC;
  m_d->checkBox_sctLayECA        = ui.checkBox_SctLayersECA;
  m_d->checkBox_sctLayBarrel     = ui.checkBox_SctLayersBarrel;
  m_d->checkBox_sctLayECC        = ui.checkBox_SctLayersECC;
  m_d->checkBox_trtLayECA        = ui.checkBox_TrtLayersECA;
  m_d->checkBox_trtLayBarrel     = ui.checkBox_TrtLayersBarrel;
  m_d->checkBox_trtLayECC        = ui.checkBox_TrtLayersECC;
  m_d->checkBox_passiveLayECA    = ui.checkBox_LayersECA;
  m_d->checkBox_passiveLayBarrel = ui.checkBox_LayersBarrel;
  m_d->checkBox_passiveLayECC    = ui.checkBox_LayersECC;

  m_d->checkBox_pixSurfECA = ui.checkBox_PixelSurfacesECA;
  m_d->surfacesNames.push_back("PixelECA");
  m_d->checkBox_pixSurfBarrel = ui.checkBox_PixelSurfacesBarrel;
  m_d->surfacesNames.push_back("PixelBarrel");
  m_d->checkBox_pixSurfECC = ui.checkBox_PixelSurfacesECC;
  m_d->surfacesNames.push_back("PixelECC");
  m_d->checkBox_sctSurfECA = ui.checkBox_SctSurfacesECA;
  m_d->surfacesNames.push_back("SCT_ECA");
  m_d->checkBox_sctSurfBarrel = ui.checkBox_SctSurfacesBarrel;
  m_d->surfacesNames.push_back("SCT_Barrel");
  m_d->checkBox_sctSurfECC = ui.checkBox_SctSurfacesECC;
  m_d->surfacesNames.push_back("SCT_ECC");

  m_d->checkBox_ClipPlane = ui.checkBox_ClipPlane;
  m_d->checkBox_MuonStaticFrameB = ui.checkBox_MuonStaticFrameB;
  m_d->checkBox_MuonStaticFrameI = ui.checkBox_MuonStaticFrameI;
  m_d->checkBox_MuonStaticFrameO = ui.checkBox_MuonStaticFrameO;
  m_d->checkBox_msBIvol = ui.checkBoxBIVol;
  m_d->checkBox_msBMvol = ui.checkBoxBMVol;
  m_d->checkBox_msBOvol = ui.checkBoxBOVol;
  m_d->checkBox_msEIvol = ui.checkBoxEIVol;
  m_d->checkBox_msEMvol = ui.checkBoxEMVol;
  m_d->checkBox_msEOvol = ui.checkBoxEOVol;
  m_d->checkBox_msCSvol = ui.checkBoxCSCVol;
  m_d->checkBox_msTGvol = ui.checkBoxTGCVol;
  m_d->checkBox_msBIlay = ui.checkBoxBILay;
  m_d->checkBox_msBMlay = ui.checkBoxBMLay;
  m_d->checkBox_msBOlay = ui.checkBoxBOLay;
  m_d->checkBox_msEIlay = ui.checkBoxEILay;
  m_d->checkBox_msEMlay = ui.checkBoxEMLay;
  m_d->checkBox_msEOlay = ui.checkBoxEOLay;
  m_d->checkBox_msCSlay = ui.checkBoxCSCLay;
  m_d->checkBox_msTGlay = ui.checkBoxTGCLay;
  m_d->checkBox_msBTenv = ui.checkBoxBTEnv;
  m_d->checkBox_msBTdl  = ui.checkBoxBTDL;
  m_d->checkBox_msETenv = ui.checkBoxETEnv;
  m_d->checkBox_msETdl  = ui.checkBoxETDL;
  m_d->checkBox_msShenv = ui.checkBoxShEnv;
  m_d->checkBox_msShdl  = ui.checkBoxShDL;
  m_d->checkBox_msFeenv = ui.checkBoxFeetEnv;
  m_d->checkBox_msFedl  = ui.checkBoxFeetDL;
  m_d->checkBox_msRaenv = ui.checkBoxRailEnv;
  m_d->checkBox_msRadl  = ui.checkBoxRailDL;


  
  std::cout << " Filling map entry [ " <<  m_d->checkBox_pixSurfBarrel  << " ] - with " << m_d->pixSurfBarrelSwitch << std::endl;
  
  // loop and set initial state
  std::map<QObject*,std::pair<SoSwitch*,bool> >::iterator chbMapIter    =  m_d->checkBoxSwitchMap.begin();
  std::map<QObject*,std::pair<SoSwitch*,bool> >::iterator chbMapIterEnd =  m_d->checkBoxSwitchMap.end();  
  
  for ( ; chbMapIter != chbMapIterEnd; ++chbMapIter ){
      QCheckBox* checkBox = dynamic_cast<QCheckBox*>((chbMapIter)->first);
      if (checkBox) checkBox->setChecked( ((chbMapIter)->second).second );
      
  }
  return controller;
}


//_____________________________________________________________________________________
void VP1TrackingGeometrySystem::changeTopLevelView()
{
  if (! m_d->retrieveSuccess)
    return;
   
   
  if (! m_d->senderMapFilled ){
      // register the map for states and signal handling
       m_d->checkBoxSwitchMap[m_d->checkBox_TrackingVolumes]      = std::make_pair(m_d->volswitch,false);             
       m_d->checkBoxSwitchMap[m_d->checkBox_pixSurfBarrel]        = std::make_pair(m_d->pixSurfBarrelSwitch,false);   
       m_d->checkBoxSwitchMap[m_d->checkBox_pixSurfECA]           = std::make_pair(m_d->pixSurfECASwitch,false);      
       m_d->checkBoxSwitchMap[m_d->checkBox_pixSurfECC]           = std::make_pair(m_d->pixSurfECCSwitch,false);      
       m_d->checkBoxSwitchMap[m_d->checkBox_sctSurfBarrel]        = std::make_pair(m_d->sctSurfBarrelSwitch,false);   
       m_d->checkBoxSwitchMap[m_d->checkBox_sctSurfECA]           = std::make_pair(m_d->sctSurfECASwitch,false);      
       m_d->checkBoxSwitchMap[m_d->checkBox_sctSurfECC]           = std::make_pair(m_d->sctSurfECCSwitch,false);      
       m_d->checkBoxSwitchMap[m_d->checkBox_trtSurfBarrel]        = std::make_pair(m_d->trtSurfBarrelSwitch,false);   
       m_d->checkBoxSwitchMap[m_d->checkBox_trtSurfECA]           = std::make_pair(m_d->trtSurfECASwitch,false);      
       m_d->checkBoxSwitchMap[m_d->checkBox_trtSurfECC]           = std::make_pair(m_d->trtSurfECCSwitch,false);      
       m_d->checkBoxSwitchMap[m_d->checkBox_beamPipe]             = std::make_pair(m_d->beamPipeSwitch,false);        
       m_d->checkBoxSwitchMap[m_d->checkBox_pixLayBarrel]         = std::make_pair(m_d->pixLayBarrelSwitch,false);    
       m_d->checkBoxSwitchMap[m_d->checkBox_pixLayECA]            = std::make_pair(m_d->pixLayECASwitch,false);       
       m_d->checkBoxSwitchMap[m_d->checkBox_pixLayECC]            = std::make_pair(m_d->pixLayECCSwitch,false);       
       m_d->checkBoxSwitchMap[m_d->checkBox_sctLayBarrel]         = std::make_pair(m_d->sctLayBarrelSwitch,false);    
       m_d->checkBoxSwitchMap[m_d->checkBox_sctLayECA]            = std::make_pair(m_d->sctLayECASwitch,false);       
       m_d->checkBoxSwitchMap[m_d->checkBox_sctLayECC]            = std::make_pair(m_d->sctLayECCSwitch,false);       
       m_d->checkBoxSwitchMap[m_d->checkBox_trtLayBarrel]         = std::make_pair(m_d->trtLayBarrelSwitch,false);    
       m_d->checkBoxSwitchMap[m_d->checkBox_trtLayECA]            = std::make_pair(m_d->trtLayECASwitch,false);       
       m_d->checkBoxSwitchMap[m_d->checkBox_trtLayECC]            = std::make_pair(m_d->trtLayECCSwitch,false);       
       m_d->checkBoxSwitchMap[m_d->checkBox_passiveLayBarrel]     = std::make_pair(m_d->passiveLayBarrelSwitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_passiveLayECA]        = std::make_pair(m_d->passiveLayECASwitch,false);   
       m_d->checkBoxSwitchMap[m_d->checkBox_passiveLayECC]        = std::make_pair(m_d->passiveLayECCSwitch,false);   
       m_d->checkBoxSwitchMap[m_d->checkBox_ClipPlane]            = std::make_pair(m_d->clipswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_MuonStaticFrameB]     = std::make_pair(m_d->msBswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_MuonStaticFrameI]     = std::make_pair(m_d->msIswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_MuonStaticFrameO]     = std::make_pair(m_d->msOswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msBIvol]              = std::make_pair(m_d->msBIswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msBIlay]              = std::make_pair(m_d->msBMswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msBMvol]              = std::make_pair(m_d->msBOswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msBMlay]              = std::make_pair(m_d->msEIswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msBOvol]              = std::make_pair(m_d->msEMswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msBOlay]              = std::make_pair(m_d->msEOswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msEIvol]              = std::make_pair(m_d->msCSswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msEIlay]              = std::make_pair(m_d->msTGswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msEMvol]              = std::make_pair(m_d->msBIlayswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msEMlay]              = std::make_pair(m_d->msBMlayswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msEOvol]              = std::make_pair(m_d->msBOlayswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msEOlay]              = std::make_pair(m_d->msEIlayswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msCSvol]              = std::make_pair(m_d->msEMlayswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msCSlay]              = std::make_pair(m_d->msEOlayswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msTGvol]              = std::make_pair(m_d->msCSlayswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msTGlay]              = std::make_pair(m_d->msTGlayswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msBTenv]              = std::make_pair(m_d->msBTenvswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msETenv]              = std::make_pair(m_d->msETenvswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msShenv]              = std::make_pair(m_d->msShenvswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msFeenv]              = std::make_pair(m_d->msFeenvswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msRaenv]              = std::make_pair(m_d->msRaenvswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msBTdl]               = std::make_pair(m_d->msBTdlswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msETdl]               = std::make_pair(m_d->msETdlswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msShdl]               = std::make_pair(m_d->msShdlswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msFedl]               = std::make_pair(m_d->msFedlswitch,false);
       m_d->checkBoxSwitchMap[m_d->checkBox_msRadl]               = std::make_pair(m_d->msRadlswitch,false);
       m_d->senderMapFilled = true;
  }      
  // loop and set initial state
  std::map<QObject*,std::pair<SoSwitch*,bool> >::iterator chbMapIterEnd =  m_d->checkBoxSwitchMap.end();
  std::map<QObject*,std::pair<SoSwitch*,bool> >::iterator chbMapIter    =  m_d->checkBoxSwitchMap.find(sender());
  
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
  /** sroe: unreachable code, coverity 17207
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

  out << m_d->lineEdit_TrackingGeometrySvcName->text();
  out << m_d->lineEdit_TrackingGeometryName->text();

  std::map<QObject*,std::pair<SoSwitch*,bool> >::iterator chbMapIter    =  m_d->checkBoxSwitchMap.begin();
  std::map<QObject*,std::pair<SoSwitch*,bool> >::iterator chbMapIterEnd =  m_d->checkBoxSwitchMap.end();
  for ( ; chbMapIter != chbMapIterEnd; ++chbMapIter ){
       QCheckBox* checkBox = dynamic_cast<QCheckBox*>((chbMapIter)->first);
       if (checkBox) out << checkBox->isChecked();

  }

  // Last step
  buffer.close();
  return byteArray;
  **/
}

void
VP1TrackingGeometrySystem::restoreFromState( QByteArray /*ba*/) //unused parameter
{
  messageDebug("WARNING: VP1TrackingGeometrySystem persistification disabled for now due to crashes!");
  return;
  /** sroe: none of the following code was reachable, coverity defect 17185
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

  state >> m_d->initialTrackingGeometrySvcName;
  state >> m_d->initialTrackingGeometryName;
  m_d->lineEdit_TrackingGeometrySvcName->setText( m_d->initialTrackingGeometrySvcName);
  m_d->lineEdit_TrackingGeometryName->setText( m_d->initialTrackingGeometryName);

  retrieveTrackingGeometry();
  **/
}
