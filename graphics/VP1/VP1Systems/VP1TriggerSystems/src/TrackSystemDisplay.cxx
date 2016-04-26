/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************************
 * @Package : VP1TriggerSystems
 * @Class   : TrackSystemDisplay
 *
 * @brief   : Temp. Version of Track Display (for testing purposes only)
 *
 * @author  : Manuel Proissl    <mproissl@cern.ch> - University of Edinburgh
 * @author  : Thomas Kittelmann <thomas.kittelmann@cern.ch>
 ***********************************************************************************/

//Local includes
#include "VP1TriggerSystems/TrackSystemDisplay.h"
#include "ui_tracksystemcontrollerform.h"

//Track includes
#include "StoreGate/StoreGateSvc.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"

//Inventor includes
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoSwitch.h>

//Other includes
#include "VP1Base/SoCooperativeSelection.h"
#include "VP1Base/VP1CameraHelper.h"
#include <QTextDocument>
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoPrimitives/GeoPrimitives.h"


//Track System Constructor
//_____________________________________________________________________________________________
TrackSystemDisplay::TrackSystemDisplay()
  : IVP13DSystemSimple("TrackSystemDisplay",
                       "This is a temporary testing version of the basic 3D track display system.",
                       "Manuel Proissl, mproissl@cern.ch"), m_ptcut(0*Gaudi::Units::GeV), m_multiselection(0)
{
  std::cout << "INFO :: TrackSystemDisplay -> constructor" << std::endl;
}//END: constructor


//Building the Event Scene
//_____________________________________________________________________________________________
void TrackSystemDisplay::buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root)
{
  std::cout << "INFO :: TrackSystemDisplay -> building event scene graph" << std::endl;
  
  m_nodeToTrack.clear();
  m_switchToPt.clear();
  
  if(!sg) {
    message("ERROR :: Null storegate pointer received.");
    return;
  }
  
  const TrackCollection *trackColl;
  std::string trackname="Tracks";
  StatusCode status = sg->retrieve(trackColl, trackname); //retrieve tracks from storegate
  if (status != StatusCode::SUCCESS || !trackColl) {
    message("ERROR :: Could not retrieve track collection (used key="+QString(trackname.c_str())+")");
    return;
  }

  m_multiselection = new SoCooperativeSelection;
  m_multiselection->policy = SoCooperativeSelection::TOGGLE;
  registerSelectionNode(m_multiselection);
  updateSelectionMode(true);
  
  TrackCollection::const_iterator trackItr, trackItrEnd = trackColl->end();
  
  for(trackItr=trackColl->begin(); trackItr!=trackItrEnd; ++trackItr) {
    Trk::Track *track = (*trackItr);
    const DataVector<const Trk::TrackParameters> *params = track->trackParameters();
    
    if(!params || params->size()<2)
      continue;
    
    SoVertexProperty *vertices = new SoVertexProperty();
    
    int iver(0);
    DataVector<const Trk::TrackParameters>::const_iterator it, itE = params->end();
    for(it=params->begin(); it!=itE; ++it) {
      vertices->vertex.set1Value(iver++,(*it)->position().x(),(*it)->position().y(),(*it)->position().z());
    }
    
    SoLineSet * line = new SoLineSet();
    line->numVertices = iver;
    line->vertexProperty = vertices;
    
    SoSwitch * sw = new SoSwitch();
    double pt = params->front()->pT();
    sw->whichChild = pt > m_ptcut ? SO_SWITCH_ALL : SO_SWITCH_NONE;
    
    m_switchToPt[sw] = pt;
    sw->addChild(line);
    m_multiselection->addChild(sw);
    m_nodeToTrack[line] = track;
    
    updateGUI();
  }
  
  root->addChild(m_multiselection);
}//END: buildEventSceneGraph


//Actions when user chose <single> track selection in controller and clicked on a track
//_____________________________________________________________________________________________
void TrackSystemDisplay::userPickedNode(SoNode* pickedNode, SoPath * /*pickedPath*/) {

  if (m_nodeToTrack.find(pickedNode)==m_nodeToTrack.end()) {
    message("ERROR :: No track information for selected node.");
    return;
  }
  
  //Get track parameters [m_nodeToTrack contains all tracks]
  const DataVector<const Trk::TrackParameters> *params = m_nodeToTrack[pickedNode]->trackParameters();
  if (!params || params->empty()) {
    message("ERROR :: Track has no track parameters.");
    return;
  }
  
  //Load and print track parameters
  QString title = "TriggerDisplay - Track Parameters";
  QList<QString> paraname, paravalue;
  paraname << QString("|p|"); paravalue << QString::number(params->front()->momentum().mag()/Gaudi::Units::GeV)+" GeV";
  paraname << QString("pT");  paravalue << QString::number(params->front()->pT()/Gaudi::Units::GeV)+" GeV";
  paraname << QString("Q");   paravalue << QString::number(params->front()->charge());
  paraname << QString("Eta"); paravalue << QString::number(params->front()->eta());
  printTrackInfo(title, paraname, paravalue);
  
  //Update camera view
  CamList cameras = getCameraList();
  for (CamListItr itCam = cameras.begin(); itCam!=cameras.end(); ++itCam) {
    VP1CameraHelper::animatedZoomToSubTree(*itCam,m_multiselection,pickedNode,2.0,1.0);
  }
}//END: userPickedNode


//Actions when user chose <multi> track selections in controller and clicked on 2 or more tracks
//_____________________________________________________________________________________________
void TrackSystemDisplay::userChangedSelection(SoCooperativeSelection*, QSet<SoNode*> nodes, QSet<SoPath*>)
{
  if(nodes.count()<2)
    return;
  
  Amg::Vector3D total3mom(0.0,0.0,0.0);
  double totalenergy(0);
  
  foreach (SoNode * node, nodes) {
    //Track pointer:
    if (m_nodeToTrack.find(node)==m_nodeToTrack.end()) {
      message("ERROR :: Does not have track information for all nodes");
      return;
    }
    
    //Parameters:
    const DataVector<const Trk::TrackParameters> *params = m_nodeToTrack[node]->trackParameters();
    if ( !params || params->empty() ) {
      message("ERROR :: Track has no trackparameters");
      return;
    }
    
    total3mom += params->front()->momentum();
    totalenergy += params->front()->momentum().mag();
  }
  
  //Display result:
  double invmasssq = totalenergy*totalenergy - total3mom.mag2();
  QString invmass_str = invmasssq>=0.0 ? QString::number(sqrt(invmasssq)/Gaudi::Units::GeV) : "sqrt(-1)*"+QString::number(sqrt(-invmasssq)/Gaudi::Units::GeV);
  QString title = "Invariant Mass";
  QList<QString> paraname, paravalue;
  
  paraname  << QString::number(nodes.count())+" tracks";
  paravalue << invmass_str+" GeV";
  printTrackInfo(title, paraname, paravalue);
}//END: userChangedSelection


//Setup controller GUI and signals
//_____________________________________________________________________________________________
QWidget * TrackSystemDisplay::buildController()
{  
  QWidget * controller = new QWidget(0);
  Ui::TrackSystemControllerForm ui;
  
  //Setup UI
  ui.setupUi(controller);
  
  //Setup pT Cut controller and its signal
  m_ptcut = ui.doubleSpinBox_ptcut->value()*Gaudi::Units::GeV;
  connect(ui.doubleSpinBox_ptcut,SIGNAL(valueChanged(double)),this,SLOT(ptCutChanged(double)));
  
  //Setup Track Selection signal(s)
  connect(ui.radioButton_select_single,SIGNAL(toggled(bool)),this,SLOT(updateSelectionMode(bool)));
  
  //Setup Trigger Browser Button
  //initDialog(ui_trig, ui.pushButton_display_TriggerBrowser);
  
  //Setup Track Information browser
  m_trackInfoDisplay = ui.trackInfoDisplay;
  m_trackInfoDisplay->setReadOnly(true); //only display, no edit
  
  return controller;
}//END: buildController


//Print track information to trackInfoDisplay
//_____________________________________________________________________________________________
void TrackSystemDisplay::printTrackInfo(QString title, QList<QString> paraname, QList<QString> paravalue)
{
  QString header = "<html><head><link rel='stylesheet' type='text/css' href='format.css'></head><body>", footer = "</body></html>";
  QString css = "#design { font-family: Courier New; font-size: 12px; margin: 0px; width: 100%; text-align: left; } #design th { font-size: 13px; font-weight: normal; padding: 2px; background: #ccc; border-top: 4px solid #000; border-bottom: 1px solid #fff; color: #000; } #design td { padding: 2px; background: #000; border-bottom: 1px solid #fff; color: #fff; border-top: 1px solid transparent; }";
  QString table_b = "<table id='design'>", table_e = "</table>";
  QString thead_b = "<thead><tr><th COLSPAN=2>", thead_e = "</th></tr></thead>";
  QString tbody_b = "<tbody>", tbody_e = "</tbody>";
  QString data_tr_b = "<tr>", data_tr_e = "</tr>";
  QString data_td_b = "<td>", data_td_e = "</td>";
  QString data="";
  
  //Generate layout string
  for(int i=0; i<paraname.size(); ++i) {
    data += data_tr_b+data_td_b+ paraname[i] +data_td_e;
    data += data_td_b+ paravalue[i] +data_td_e+data_tr_e;
  }
  
  //Combine all print segments
  QTextDocument *doc = new QTextDocument;
  doc->addResource(QTextDocument::StyleSheetResource, QUrl("format.css"), css);
  doc->setHtml(header+table_b+thead_b+title+thead_e+tbody_b+data+tbody_e+table_e+footer);
  
  //Display track info
  m_trackInfoDisplay->setDocument(doc);
  m_trackInfoDisplay->show();
}//END: printTrackInfo


//Signal triggered actions when pT Cut has been changed by user
//_____________________________________________________________________________________________
void TrackSystemDisplay::ptCutChanged(double ptcut)
{
  m_ptcut = ptcut*Gaudi::Units::GeV;

  std::map<SoSwitch*,double>::iterator it, itE = m_switchToPt.end();
  for ( it = m_switchToPt.begin(); it!=itE; ++it) {
    it->first->whichChild = ( it->second>m_ptcut ? SO_SWITCH_ALL : SO_SWITCH_NONE );
  }
}//END: ptCutChanged


//Track Selection mode switch
//_____________________________________________________________________________________________
void TrackSystemDisplay::updateSelectionMode( bool single )
{
  if(!m_multiselection || single==(m_multiselection->activePolicy.getValue()==SoCooperativeSelection::INERT))
    return;
  
  deselectAll();

  if(single)
    m_multiselection->activePolicy = SoCooperativeSelection::INERT;
  else
    m_multiselection->activePolicy = SoCooperativeSelection::ACTIVE;
}//END: updateSelectionMode
