/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Implementation of class Example3DSystem5                           //
//                                                                     //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial version: June 2007                                         //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1TestSystems/Example3DSystem5.h"
#include "ui_example5controllerform.h"

#include "StoreGate/StoreGate.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "GeoPrimitives/GeoPrimitives.h"
//#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiKernel/SystemOfUnits.h"


#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoSwitch.h>

#include "VP1Base/SoCooperativeSelection.h"
#include "VP1Base/VP1CameraHelper.h"

//_____________________________________________________________________________________
Example3DSystem5::Example3DSystem5()
  : IVP13DSystemSimple("Ex3DSys5",
		       "This is an illustration of a very basic 3D system.\n"
		       "It transforms track information found in storegate"
		       " into 3D objects, displays track information and zooms to them upon selection, "
		       "and has a controller which allows the user to set a pt cut on tracks."
		       " It also has a multiple selection mode which can be used to display the combined invariant mass of tracks",
		       "Thomas.Kittelmann@cern.ch"), m_ptcut(0*Gaudi::Units::GeV), m_multiselection(0)
{
}

//_____________________________________________________________________________________
void Example3DSystem5::buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root)
{
  //Clear maps:
  m_nodeToTrack.clear();
  m_switchToPt.clear();

  //1) Try to get the collection of (InDet) tracks from storegate:

  //Sanity check:
  if (!sg) {
    message("Error: Got null storegate pointer");
    return;
  }

  const TrackCollection *trackColl;
  std::string trackname="Tracks";
  StatusCode status = sg->retrieve(trackColl, trackname);
  if (status != StatusCode::SUCCESS || !trackColl) {
    message("Error: Could not retrieve track collection (used key="+QString(trackname.c_str())+")");
    return;
  }

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

  TrackCollection::const_iterator trackItr, trackItrEnd = trackColl->end();

  for ( trackItr = trackColl->begin() ; trackItr != trackItrEnd; ++trackItr) {
      const Trk::Track *track = (*trackItr);
      const DataVector<const Trk::TrackParameters> *params = track->trackParameters();

      //Just a sanity check (we need at least two points on the track):
      if ( !params || params->size()<2 )
	continue;

      //The list of points on this track should be set in a so-called
      //SoVertexProperty (which one realises by reading the
      //documentation for SoLineSet at http://doc.coin3d.org/Coin/):

      SoVertexProperty *vertices = new SoVertexProperty();

      int iver(0);
      DataVector<const Trk::TrackParameters>::const_iterator it, itE = params->end();
      for (it = params->begin();it!=itE;++it) {
	vertices->vertex.set1Value(iver++,(*it)->position().x(),(*it)->position().y(),(*it)->position().z());
      }

      //Create a set of lines from these vertices:
      SoLineSet * line = new SoLineSet();
      line->numVertices = iver;
      line->vertexProperty = vertices;

      //We add the line to the tree below an SoSwitch. The latter is needed to turn the track on/off:
      SoSwitch * sw = new SoSwitch();

      //     initial value of switch is based on current pt cut:
      double pt = params->front()->pT();
      sw->whichChild = pt > m_ptcut ? SO_SWITCH_ALL : SO_SWITCH_NONE;

      //     the user might change the pt cut later, so we need a list of the switches and the pT of their corresponding tracks:
      m_switchToPt[sw] = pt;

      sw->addChild(line);
      m_multiselection->addChild(sw);

      //Bookkeep which track this 3D object corresponds to (we will need this to display track info when the user clicks):
      m_nodeToTrack[line] = track;

      //To avoid GUI freeze-ups:
      updateGUI();
  }

  //Add the selection node under the root:
  root->addChild(m_multiselection);

}

//_____________________________________________________________________________________
void Example3DSystem5::userPickedNode(SoNode* pickedNode, SoPath * /*pickedPath*/) {

  //User clicked on "pickedNode". We should know which track this belongs to:
  if (m_nodeToTrack.find(pickedNode)==m_nodeToTrack.end()) {
    message("Error: Does not have track information for picked node");
    return;
  }

  //Get parameters:
  const DataVector<const Trk::TrackParameters> *params = m_nodeToTrack[pickedNode]->trackParameters();
  if ( !params || params->empty() ) {
    message("Error: Track has no trackparameters");
    return;
  }

  //Show some interesting information (from the first trackparameter):
  message("===> Track info:");
  message("  |p| = "+QString::number(params->front()->momentum().mag()/Gaudi::Units::GeV)+" GeV");
  message("   pT = "+QString::number(params->front()->pT()/Gaudi::Units::GeV)+" GeV");
  message("   Q  = "+QString::number(params->front()->charge()));
  message("  eta = "+QString::number(params->front()->eta()));

  //Zoom to track:
  CamList cameras = getCameraList();
  for (CamListItr itCam = cameras.begin(); itCam!=cameras.end(); ++itCam) {
    VP1CameraHelper::animatedZoomToSubTree(*itCam,m_multiselection,pickedNode,2.0,1.0);
  }
}

//_____________________________________________________________________________________
void Example3DSystem5::userChangedSelection(SoCooperativeSelection*, QSet<SoNode*> nodes, QSet<SoPath*>)
{
  //Calculate and print invariant mass.
  if (nodes.count()<2)
    return;

  //Loop over the nodes, find their track pointers and add up their four-momentum (assuming massless particles):
  Amg::Vector3D total3mom(0.0, 0.0, 0.0); //This is just a three vector
  double totalenergy(0);//for the energy part of the lorentz vector

  foreach (SoNode * node, nodes) {
    //Track pointer:
    if (m_nodeToTrack.find(node)==m_nodeToTrack.end()) {
      message("Error: Does not have track information for all nodes");
      return;
    }
    //Parameters:
    const DataVector<const Trk::TrackParameters> *params = m_nodeToTrack[node]->trackParameters();
    if ( !params || params->empty() ) {
      message("Error: Track has no trackparameters");
      return;
    }
    total3mom += params->front()->momentum();
    totalenergy += params->front()->momentum().mag();
  }
  //Display result:
  double invmasssq = totalenergy*totalenergy - total3mom.mag2();
  QString invmass_str = invmasssq>=0.0 ? QString::number(sqrt(invmasssq)/Gaudi::Units::GeV) : "sqrt(-1)*"+QString::number(sqrt(-invmasssq)/Gaudi::Units::GeV);
  message("Invariant mass of "+QString::number(nodes.count())+" selected tracks (massless particles): "+invmass_str+" GeV");
}

//_____________________________________________________________________________________
QWidget * Example3DSystem5::buildController()
{
  QWidget * controller = new QWidget(0);
  Ui::Example5ControllerForm ui;
  ui.setupUi(controller);

  m_ptcut = ui.doubleSpinBox_ptcut->value()*Gaudi::Units::GeV;
  connect(ui.doubleSpinBox_ptcut,SIGNAL(valueChanged(double)),this,SLOT(ptCutChanged(double)));
  connect(ui.radioButton_select_single,SIGNAL(toggled(bool)),this,SLOT(updateSelectionMode(bool)));

  return controller;
}

//_____________________________________________________________________________________
void Example3DSystem5::ptCutChanged(double ptcut)
{
  //Save this value since we need it when building next event:
  m_ptcut = ptcut*Gaudi::Units::GeV;

  //Update switches to display relevant tracks:
  std::map<SoSwitch*,double>::iterator it, itE = m_switchToPt.end();
  for ( it = m_switchToPt.begin(); it!=itE; ++it) {
    it->first->whichChild = ( it->second>m_ptcut ? SO_SWITCH_ALL : SO_SWITCH_NONE );
  }
}

//_____________________________________________________________________________________
void Example3DSystem5::updateSelectionMode( bool single )
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
