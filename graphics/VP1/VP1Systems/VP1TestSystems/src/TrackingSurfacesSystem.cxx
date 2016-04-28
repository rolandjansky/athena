/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Implementation of class TrackingSurfacesSystem                           //
//                                                                     //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial version: June 2007                                         //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1TestSystems/TrackingSurfacesSystem.h"
#include "ui_trackingsurfacescontrollerform.h"

#include "StoreGate/StoreGate.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceCollection.h"
#include "GeoPrimitives/GeoPrimitives.h"
//#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoMaterial.h>

#include "VP1Base/SoCooperativeSelection.h"
#include "VP1Base/VP1CameraHelper.h"
#include "VP1Utils/SurfaceToSoNode.h"

//_____________________________________________________________________________________
TrackingSurfacesSystem::TrackingSurfacesSystem()
  : IVP13DSystemSimple("TrackingSurfacesSystem","This is an illustration of a very basic 3D system.","edward.moyse@gmail.com"), m_multiselection(0)
{
}

//_____________________________________________________________________________________
void TrackingSurfacesSystem::buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root)
{
  //Clear maps:
  m_nodeToSurface.clear();
  // std::cout<<&m_surfConvertor<<std::endl;
  //1) Try to get the collection of (InDet) tracks from storegate:

  //Sanity check:
  if (!sg) {
    message("Error: Got null storegate pointer");
    return;
  }


  // SurfaceCollection* testsrfcol;
  // StatusCode sc2 = evtStore()->retrieve(testsrfcol,"SurfaceCollection");
  // if ( sc2.isFailure() )
  //   ATH_MSG_ERROR("Could not retrieve SurfaceCollection in StoreGate");
  // else
  //   ATH_MSG_INFO("SurfaceCollection with " << testsrfcol->size() << " elements successfully retrieved in StoreGate");
  //

  const SurfaceCollection *surfColl;
  std::string surfname="SurfaceCollection";
  StatusCode status = sg->retrieve(surfColl, surfname);
  if (status != StatusCode::SUCCESS || !surfColl) {
    message("Error: Could not retrieve SurfaceCollection (used key="+QString(surfname.c_str())+")");
    return;
  }
  message( "Loaded SurfaceCollection with key [" +QString(surfname.c_str())+"] and which contains [" + QString::number(surfColl->size()) + "] surfaces.");
  //2) Add SoCooperativeSelection node under which we will put all the
  //tracks. Register it and put its policy to TOGGLE (i.e. clicking
  //adds or removes tracks from selection). In updateSelectionMode we
  //will update its ACTIVE/INERT status depending on whether multiple
  //selections are enabled: If ACTIVE, we get lists of selected nodes
  //in the userChangedSelection(...) method, otherwise we get single
  //nodes in the userPickedNode(...) method as always. We could also
  //simply have changed its policy and gotten information about the
  //clicks in a userSelectedSingleNode(...)  method instead (the
  //reason for the two available ways of doing this is that you might
  //want to have several different selection nodes in your graph in
  //the case where you have different types of physics objects
  //displayed from the same system):
  m_multiselection = new SoCooperativeSelection;
  m_multiselection->policy = SoCooperativeSelection::TOGGLE;
  registerSelectionNode(m_multiselection);//Always do this for SoCooperativeSelection nodes in your graph.
  updateSelectionMode( true );//Since our controller starts with the single mode selected, we init like that also.

  //2) Loop over the tracks and construct a relevant 3D object for each of them (a SoLineSet):

  SurfaceCollection::const_iterator surfItr, surfItrEnd = surfColl->end();
  SurfaceToSoNode surfCnv;
  for ( surfItr = surfColl->begin() ; surfItr != surfItrEnd; ++surfItr) {
      const Trk::Surface* surf = (*surfItr);

      SoNode* node = surfCnv.translateSurface(*surf, false);
      SoMaterial *material = new SoMaterial;
      if(surf->isActive()){
	material->diffuseColor.setValue(0.6, 0., 0.6);
      } else {
	material->diffuseColor.setValue(1., 0., 1.);
      }
 
      m_multiselection->addChild(material);
      m_multiselection->addChild(node);
        
      //Bookkeep which track this 3D object corresponds to (we will need this to display surface info when the user clicks):
      m_nodeToSurface[node] = surf;

      //To avoid GUI freeze-ups:
      updateGUI();
  }

  //Add the selection node under the root:
  root->addChild(m_multiselection);


}

//_____________________________________________________________________________________
void TrackingSurfacesSystem::userPickedNode(SoNode* pickedNode, SoPath * /*pickedPath*/) {

  //User clicked on "pickedNode". We should know which track this belongs to:
  
  messageDebug("TrackingSurfacesSystem::userPickedNode - have been passed node ["+QString("0x%1").arg((quintptr)pickedNode, 
                    QT_POINTER_SIZE * 2, 16, QChar('0'))+"] and have ["+QString::number(m_nodeToSurface.size())+"] nodes.");
  
  if (m_nodeToSurface.find(pickedNode)==m_nodeToSurface.end()) {
    message("Error: Do not have surface information for picked node");
    return;
  }
  const Trk::Surface * matchedSurface=m_nodeToSurface.find(pickedNode)->second;

  //Show some interesting information (from the first surface):
  message("===> Surface info:");
  QStringList l;
  std::ostringstream s;
  s << *(matchedSurface);
  l << QString(s.str().c_str()).split('\n');
  message(l);
  
  //Zoom to surf:
  CamList cameras = getCameraList();
  for (CamListItr itCam = cameras.begin(); itCam!=cameras.end(); ++itCam) {
    VP1CameraHelper::animatedZoomToSubTree(*itCam,m_multiselection,pickedNode,2.0,1.0);
  }
}

//_____________________________________________________________________________________
void TrackingSurfacesSystem::userChangedSelection(SoCooperativeSelection*, QSet<SoNode*> nodes, QSet<SoPath*>)
{
  foreach (SoNode * node, nodes) {
    //Surface pointer:
    if (m_nodeToSurface.find(node)==m_nodeToSurface.end()) {
      message("Error: Does not have surface information for all nodes");
      return;
    }
  }
}

//_____________________________________________________________________________________
QWidget * TrackingSurfacesSystem::buildController()
{
  QWidget * controller = new QWidget(0);
  Ui::TrackingSurfacesControllerForm ui;
  ui.setupUi(controller);

  connect(ui.radioButton_select_single,SIGNAL(toggled(bool)),this,SLOT(updateSelectionMode(bool)));

  return controller;
}

//_____________________________________________________________________________________
void TrackingSurfacesSystem::updateSelectionMode( bool single )
{
  //If multiselection not created or if we dont need to change anything - abort:
  if ( !m_multiselection || single == (m_multiselection->activePolicy.getValue()==SoCooperativeSelection::INERT) )
    return;

  //When we change selection mode we deselect everything - always a good idea to avoid hard to debug problems later:
  deselectAll();

  if (single)
    m_multiselection->activePolicy = SoCooperativeSelection::INERT;
  else
    m_multiselection->activePolicy = SoCooperativeSelection::ACTIVE;

}
