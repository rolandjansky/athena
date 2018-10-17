/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Implementation of class Example3DSystem4                           //
//                                                                     //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial version: June 2007                                         //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1TestSystems/Example3DSystem4.h"
#include "ui_example4controllerform.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoSwitch.h>

#include "StoreGate/StoreGate.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
//#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiKernel/SystemOfUnits.h"


//_____________________________________________________________________________________
Example3DSystem4::Example3DSystem4()
  : IVP13DSystemSimple("Ex3DSys4",
		       "This is an illustration of a very basic 3D system.\n"
		       "It transforms track information found in storegate"
		       " into 3D objects, displays track information upon selection, and has a controller which allows the user to set a pt cut on tracks.",
		       "Thomas.Kittelmann@cern.ch"), m_ptcut(0*Gaudi::Units::GeV)
{
}

//_____________________________________________________________________________________
void Example3DSystem4::buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root)
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
      root->addChild(sw);

      //Bookkeep which track this 3D object corresponds to (we will need this to display track info when the user clicks):
      m_nodeToTrack[line] = track;

      //To avoid GUI freeze-ups:
      updateGUI();
  }

}

//_____________________________________________________________________________________
void Example3DSystem4::userPickedNode(SoNode* pickedNode, SoPath * /*pickedPath*/) {

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

}

//_____________________________________________________________________________________
QWidget * Example3DSystem4::buildController()
{
  QWidget * controller = new QWidget(0);
  Ui::Example4ControllerForm ui;
  ui.setupUi(controller);

  m_ptcut = ui.doubleSpinBox_ptcut->value()*Gaudi::Units::GeV;
  connect(ui.doubleSpinBox_ptcut,SIGNAL(valueChanged(double)),this,SLOT(ptCutChanged(double)));

  return controller;
}

//_____________________________________________________________________________________
void Example3DSystem4::ptCutChanged(double ptcut)
{
  //Save this value since we need it when building next event:
  m_ptcut = ptcut*Gaudi::Units::GeV;

  //Update switches to display relevant tracks:
  std::map<SoSwitch*,double>::iterator it, itE = m_switchToPt.end();
  for ( it = m_switchToPt.begin(); it!=itE; ++it) {
    it->first->whichChild = ( it->second>m_ptcut ? SO_SWITCH_ALL : SO_SWITCH_NONE );
  }
}
