/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Implementation of class VP1BPhysSystem                             //
//                                                                     //
//  Author: Daniel Scheirich                                           //
//                                                                     //
//  Initial version: September 2009                                    //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1BPhysSystems/VP1BPhysSystem.h"
#include "ui_bphyscontrollerform.h"

#include "VP1Utils/VP1ToolAccessHelper.h"
#include "VP1Utils/VP1AvailableToolsHelper.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1TrackSystems/TrackPropagationHelper.h"


#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoMatrixTransform.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSwitch.h>
#include <QFileDialog>


#include "StoreGate/StoreGate.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
//#include "TrkParameters/MeasuredPerigee.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkExInterfaces/IExtrapolator.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

//#include "CLHEP/Units/SystemOfUnits.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include "TFile.h"
#include "TTree.h"



const unsigned int darknes = 50;
const unsigned long darkGray = darknes*65536 + darknes*256 + darknes;

bool Br::init(TTree* tree) {
	vp1Filter = tree;
	if(tree==NULL) return false;

	vp1Filter->SetBranchAddress("evtNum", &evtNum);
	vp1Filter->SetBranchAddress("runNum", &runNum);
  
	vp1Filter->SetBranchAddress("vtx_x",              &vtx_x);
	vp1Filter->SetBranchAddress("vtx_y",              &vtx_y);
	vp1Filter->SetBranchAddress("vtx_z",              &vtx_z);
  
	vp1Filter->SetBranchAddress("vtx_xx",             &vtx_xx);
	vp1Filter->SetBranchAddress("vtx_yy",             &vtx_yy);
	vp1Filter->SetBranchAddress("vtx_zz",             &vtx_zz);
	vp1Filter->SetBranchAddress("vtx_xy",             &vtx_xy);
	vp1Filter->SetBranchAddress("vtx_xz",             &vtx_xz);
	vp1Filter->SetBranchAddress("vtx_yz",             &vtx_yz);

	vp1Filter->SetBranchAddress("vtx_mother",         &vtx_mother);
	vp1Filter->SetBranchAddress("vtx_color",          &vtx_color);
	vp1Filter->SetBranchAddress("vtx_daughters",      &vtx_daughters);

  //tracks
	vp1Filter->SetBranchAddress("track_pt",             &track_pt            );
	vp1Filter->SetBranchAddress("track_eta",            &track_eta           );
	vp1Filter->SetBranchAddress("track_phi",            &track_phi           );
	vp1Filter->SetBranchAddress("track_d0",             &track_d0            );
	vp1Filter->SetBranchAddress("track_z0",             &track_z0            );
	vp1Filter->SetBranchAddress("track_charge",         &track_charge        );
	vp1Filter->SetBranchAddress("track_refitted_px",    &track_refitted_px   );
	vp1Filter->SetBranchAddress("track_refitted_py",    &track_refitted_py   );
	vp1Filter->SetBranchAddress("track_refitted_pz",    &track_refitted_pz   );
	vp1Filter->SetBranchAddress("track_color",          &track_color         );
	vp1Filter->SetBranchAddress("track_refitted_color", &track_refitted_color);
  
  //neutral tracks
	vp1Filter->SetBranchAddress("neutral_refitted_px",  &neutral_refitted_px);
	vp1Filter->SetBranchAddress("neutral_refitted_py",  &neutral_refitted_py);
	vp1Filter->SetBranchAddress("neutral_refitted_pz",  &neutral_refitted_pz);
	vp1Filter->SetBranchAddress("neutral_length",       &neutral_length     );
	vp1Filter->SetBranchAddress("neutral_decay",        &neutral_decay      );
	vp1Filter->SetBranchAddress("neutral_color",        &neutral_color      );
  
	return true;

}

int Br::GetEntry(int i) {
	return vp1Filter->GetEntry(i);
}

class VP1BPhysSystem::Clockwork {
	public:
		Ui::BPhysControllerForm ui;
};
//_____________________________________________________________________________________

VP1BPhysSystem::VP1BPhysSystem()
	: IVP13DSystemSimple("BPhys",
			     "VP1BPhysSystem",
			     "daniel.scheirich@cern.ch") ,
	m_vertexStyle{},
  m_showVertices{},
  m_vertexSwitches{},

  m_showAll{},
  m_showSignal{},
  m_showNeutral{},
  m_showRefitted{},

  m_trackSwitches{},
  m_overlapSwitches{},
  m_signalSwitches{},
  m_neutralSwitches{},
  m_refittedSwitches{},
  m_br{},    
	c(new Clockwork)
{
	messageDebug("in BPhysSystem");

	m_rootFile = NULL;
	m_tree = NULL;
	m_sg = NULL;
	m_root = NULL;

	
//   messageDebug("Creating propagationHelper...");
//   propagationHelper = new TrackPropagationHelper(this);
//   messageDebug("...done");

	messageDebug("leaving VP1BPhysSystem");
}

//_____________________________________________________________________________________
QWidget * VP1BPhysSystem::buildController()
{
	messageDebug("in buildController");
  
	QWidget * controller = new QWidget(0);
	c->ui.setupUi(controller);

  //connect slots
	connect(c->ui.bLoad,SIGNAL(released()),this,SLOT(loadFile()));
  
	connect(c->ui.chDisplayVertices,SIGNAL(stateChanged(int)),this,SLOT(displayVerticesChanged(int)));
	connect(c->ui.rbSphere,SIGNAL(toggled(bool)),this,SLOT(sphereToggled(bool)));
	connect(c->ui.rbCross,SIGNAL(toggled(bool)),this,SLOT(crossToggled(bool)));
	connect(c->ui.rbEllipsoid,SIGNAL(toggled(bool)),this,SLOT(ellipsoidToggled(bool)));

	connect(c->ui.chDisplayAllTracks ,SIGNAL(stateChanged(int)),this,SLOT(displayAllTracksChanged(int)));
	connect(c->ui.chDisplayOrigSignal,SIGNAL(stateChanged(int)),this,SLOT(displayOrigSignalChanged(int)));
	connect(c->ui.chDisplayRefTracks ,SIGNAL(stateChanged(int)),this,SLOT(displayRefTracksChanged(int)));
	connect(c->ui.chDisplayNeutral   ,SIGNAL(stateChanged(int)),this,SLOT(displayNeutralChanged(int)));

	if(c->ui.rbSphere->isChecked()) m_vertexStyle = 0;
	if(c->ui.rbCross->isChecked()) m_vertexStyle = 1;
	if(c->ui.rbEllipsoid->isChecked()) m_vertexStyle = 2;
	m_showVertices = c->ui.chDisplayVertices->isChecked();

	m_showAll      = c->ui.chDisplayAllTracks->isChecked();
	m_showSignal   = c->ui.chDisplayOrigSignal->isChecked();
	m_showRefitted  = c->ui.chDisplayRefTracks->isChecked();
	m_showNeutral = c->ui.chDisplayNeutral->isChecked();

  
	messageDebug("leaving buildController");
  
	return controller;
}


//_____________________________________________________________________________________
void VP1BPhysSystem::actualBuild() {

	//delete all objects from the scene
	m_root->removeAllChildren();
	
  //Sanity check:
	if (!m_sg) {
		message("Error: Got null storegate pointer");
		return;
	}

  //check that file was open
	if(m_tree==NULL) return;

  //retrieving event info
	const EventInfo* eventInfo;
	StatusCode sc = m_sg->retrieve(eventInfo);
	if (!sc.isSuccess()) {
		message("Error: Could not retrieve EventInfo");
		return;
	}
  
	EventID* eventID = eventInfo->event_ID(); // Get EventInfo
	int evtNum = eventID->event_number();
	int runNum = eventID->run_number();

  //retrieve TrackParticle candidates
	const Rec::TrackParticleContainer* partCont;
	StatusCode status = m_sg->retrieve(partCont, "TrackParticleCandidate");
	if (status != StatusCode::SUCCESS || !partCont) {
		message("Error: Could not retrieve track particle container (used key=TrackParticleCandidate)");
		return;
	}

  /// --------------------------------------------------------------------
  //cleanup
	m_vertexSwitches.clear();
	m_trackSwitches.clear();
	m_overlapSwitches.clear();
	m_signalSwitches.clear();
	m_neutralSwitches.clear();
	m_refittedSwitches.clear();
  
  /// --------------------------------------------------------------------
  
	std::vector<const Rec::TrackParticle*>* selectedParticles = new std::vector<const Rec::TrackParticle*>();

  
  //find event
  //int e = m_tree->GetEntryNumberWithIndex(runNum, evtNum);  //for some reason this doesn't work 100% reliable

	int e=-1;
	for(int i=0; i<m_tree->GetEntries(); ++i) {
		m_br->GetEntry(i);
		if(m_br->runNum == runNum && m_br->evtNum == evtNum) {
			e=i;
			break;
		}
	}
  
	if(e==-1) {
		c->ui.lStatus->setText("No event to display");
		message("Event is not in the VP1BPhys Display File. Skipping.");
	}else{
		c->ui.lStatus->setText("");
  
		m_br->GetEntry(e);
		for(; m_br->runNum == runNum && m_br->evtNum == evtNum && e<m_tree->GetEntries(); m_br->GetEntry(++e)) {


      //filter tracks
      //sanity check
			if(m_br->track_pt->size() != m_br->track_eta->size()            ||
						m_br->track_pt->size() != m_br->track_eta->size()            ||
						m_br->track_pt->size() != m_br->track_phi->size()            ||
						m_br->track_pt->size() != m_br->track_d0->size()             ||
						m_br->track_pt->size() != m_br->track_z0->size()             ||
						m_br->track_pt->size() != m_br->track_charge->size()         ||
						m_br->track_pt->size() != m_br->track_color->size()          ||
						m_br->track_pt->size() != m_br->track_refitted_px->size()    ||
						m_br->track_pt->size() != m_br->track_refitted_py->size()    ||
						m_br->track_pt->size() != m_br->track_refitted_pz->size()    ||
						m_br->track_pt->size() != m_br->track_refitted_color->size() ||
						m_br->neutral_refitted_px->size() != m_br->neutral_refitted_py->size() ||
						m_br->neutral_refitted_px->size() != m_br->neutral_refitted_py->size() ||
						m_br->neutral_refitted_px->size() != m_br->neutral_refitted_pz->size() ||
						m_br->neutral_refitted_px->size() != m_br->neutral_length->size()      ||
						m_br->neutral_refitted_px->size() != m_br->neutral_color->size())
			{
				message("Different lengths of some filter track collections");
				continue;
			}
  
      //draw selected particles
			for(uint i=0; i<m_br->track_pt->size(); ++i) {
        //TrackParticles
				filterTrack(m_root, partCont,
										m_br->track_pt->at(i),
										m_br->track_eta->at(i),
										m_br->track_phi->at(i),
										m_br->track_d0->at(i),
										m_br->track_z0->at(i),
										m_br->vtx_x, m_br->vtx_y, m_br->vtx_z,
										m_br->track_color->at(i),
										selectedParticles);
        
        //refitted tracks
				drawRefittedTrack(m_root,
													m_br->vtx_x, m_br->vtx_y, m_br->vtx_z,
													m_br->track_refitted_px->at(i),
													m_br->track_refitted_py->at(i),
													m_br->track_refitted_pz->at(i),
													m_br->track_charge->at(i),
													m_br->track_refitted_color->at(i));
        
			}
  
  
      //draw neutral tracks
			std::vector<double>::iterator neutralPxItr = m_br->neutral_refitted_px->begin();
			std::vector<double>::iterator neutralPyItr = m_br->neutral_refitted_py->begin();
			std::vector<double>::iterator neutralPzItr = m_br->neutral_refitted_pz->begin();
			std::vector<double>::iterator neutralLengthItr = m_br->neutral_length->begin();
			std::vector<unsigned long>::iterator neutralColorItr = m_br->neutral_color->begin();
  
			for(; neutralPxItr!=m_br->neutral_refitted_px->end(); ++neutralPxItr, ++neutralPyItr, ++neutralPzItr, ++neutralLengthItr, ++neutralColorItr) {
				drawNeutralTrack(m_root, m_br->vtx_x, m_br->vtx_y, m_br->vtx_z,
												 *neutralPxItr, *neutralPyItr, *neutralPzItr, *neutralLengthItr, *neutralColorItr);
			}
  
      //draw vertices
			drawVertex(m_root, m_br->vtx_x, m_br->vtx_y, m_br->vtx_z, 3/*sigma*/,
								 m_br->vtx_xx,
								 m_br->vtx_xy,
								 m_br->vtx_xz,
								 m_br->vtx_yy,
								 m_br->vtx_yz,
								 m_br->vtx_zz,
								 m_br->vtx_color,
								 m_vertexSwitches);
  
		}
	}
  
  //draw all remaining tracks
	drawAllTrackParticles(m_sg,m_root,selectedParticles);

  //cleaning up
	delete selectedParticles;


}
//_____________________________________________________________________________________
void VP1BPhysSystem::buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root)
{
	messageDebug("in buildEventSceneGraph");

	m_sg = sg;
	m_root = root;

	actualBuild();

	messageDebug("Leaving buildEventSceneGraph");

}
/// drawing methods *******************************************************************
void VP1BPhysSystem::filterTrack(SoSeparator *root, const Rec::TrackParticleContainer* partCont,
				 double pt, double eta, double /*phi*/, double /*d0*/, double /*z0*/,
				 double x, double y, double z, unsigned long color,
				 std::vector<const Rec::TrackParticle*>* selectedParticles)
{
	messageDebug("in filterTrack");
  //loop over TrackParticles
	Rec::TrackParticleContainer::const_iterator partItr, partItrEnd = partCont->end();
	for ( partItr = partCont->begin() ; partItr != partItrEnd; ++partItr) {

		double trk_qOverP = (*partItr)->measuredPerigee()->parameters()[Trk::qOverP];
		double trk_theta  = (*partItr)->measuredPerigee()->parameters()[Trk::theta];
//		double trk_phi    = (*partItr)->measuredPerigee()->parameters()[Trk::phi];

		double trk_pt = fabs(1./trk_qOverP)*sin(trk_theta);
		double trk_eta = -log(tan(trk_theta/2));

    //identify selected track
		if(fabs(trk_pt - pt)<0.1 && fabs(trk_eta - eta)<0.01) {
			drawCutoffTrackParticle(root, *partItr, x, y, z, color);
			if(selectedParticles!=NULL) {
				selectedParticles->push_back(*partItr);
			}
		}
	}
	messageDebug("leaving filterTrack");

}
//___________________________________________________________                                    
void VP1BPhysSystem::drawAllTrackParticles(StoreGateSvc* sg, SoSeparator *root, std::vector<const Rec::TrackParticle*>* selectedParticles) {
	messageDebug("in drawAllTrackParticles");

  //retrieve TrackParticle candidates
	const Rec::TrackParticleContainer* partCont;
	StatusCode status = sg->retrieve(partCont, "TrackParticleCandidate");
	if (status != StatusCode::SUCCESS || !partCont) {
		message("Error: Could not retrieve track particle container (used key=TrackParticleCandidate)");
		return;
	}

	Rec::TrackParticleContainer::const_iterator partItr, partItrEnd = partCont->end();
	for ( partItr = partCont->begin() ; partItr != partItrEnd; ++partItr) {
		bool found = false;
		if(selectedParticles!=NULL) {
			std::vector<const Rec::TrackParticle*>::const_iterator selItr = selectedParticles->begin();
			for(; selItr!=selectedParticles->end(); ++selItr) {
				if(*partItr == *selItr) {
					found = true;
					break;
				}
			}
		}
		if(!found) {
			SoSwitch* trackSwitch = new SoSwitch();
			root->addChild(trackSwitch);
			trackSwitch->whichChild = m_showAll ? SO_SWITCH_ALL : SO_SWITCH_NONE;
			drawTrackParticle(trackSwitch, *partItr, darkGray);
			m_trackSwitches.push_back(trackSwitch);
		}else{
			SoSwitch* trackSwitch = new SoSwitch();
			root->addChild(trackSwitch);
			trackSwitch->whichChild = m_showAll && !m_showSignal && !m_showRefitted ? SO_SWITCH_ALL : SO_SWITCH_NONE;
			drawTrackParticle(trackSwitch, *partItr, darkGray);
			m_overlapSwitches.push_back(trackSwitch);
		}
	}
	messageDebug("leaving drawAllTrackParticles");

}
//___________________________________________________________                                    
void VP1BPhysSystem::drawTrackParticle(SoSwitch* trackSwitch, const Rec::TrackParticle* part, unsigned long color) {

	messageDebug("in drawTrackParticle");

  //getting track space-points
	std::vector<Amg::Vector3D >* points = getPoints(part);

  //drawing line
	drawPoints(trackSwitch,points,color, 0.0f, false);
  
  //cleaning up
	delete points;
  
	messageDebug("leaving drawTrackParticle");

}
//___________________________________________________________
void VP1BPhysSystem::drawCutoffTrackParticle(SoSeparator *root, const Rec::TrackParticle* part, double x, double y, double z, unsigned long color) {
	messageDebug("in drawCutoffTrackParticle");
  
  //getting track space-points
	std::vector<Amg::Vector3D >* points = getPoints(part);

  //getting cut-off point collection
	std::vector<Amg::Vector3D >* cutoffPoints = findClosestApproach(points, x, y, z);
  
  //drawing
	SoSwitch* trackSwitch = new SoSwitch();
	root->addChild(trackSwitch);
	trackSwitch->whichChild = m_showSignal ? SO_SWITCH_ALL : SO_SWITCH_NONE;
	drawPoints(trackSwitch,cutoffPoints,color,2,false);
	m_signalSwitches.push_back(trackSwitch);
  
  //cleaning up
	delete points;
	delete cutoffPoints;

	messageDebug("leaving drawCutoffTrackParticle");

}
//___________________________________________________________
void VP1BPhysSystem::drawNeutralTrack(SoSeparator* root, double x, double y, double z, double px, double py, double pz, double length, unsigned long color) {
	messageDebug("in drawNeutralTrack");

	std::vector<Amg::Vector3D >* points = new std::vector<Amg::Vector3D >();

  //create neutral track vector
	CLHEP::Hep3Vector p(px,py,pz);
	CLHEP::Hep3Vector track = p.unit();
	track *= length;

  //create points
	points->push_back(Amg::Vector3D(x-track.x(), y-track.y(), z-track.z()));
	points->push_back(Amg::Vector3D(x,y,z));

	SoSwitch* trackSwitch = new SoSwitch();
	root->addChild(trackSwitch);
	trackSwitch->whichChild = m_showNeutral ? SO_SWITCH_ALL : SO_SWITCH_NONE;
	drawPoints(trackSwitch, points, color, 2, true);
	m_neutralSwitches.push_back(trackSwitch);

  //cleaning up
	messageDebug("leaving drawNeutralTrack");
	delete points;

}

//___________________________________________________________
void VP1BPhysSystem::drawRefittedTrack(SoSeparator* root, double x, double y, double z,
																		double px, double py, double pz, double charge, unsigned long color)
{
	messageDebug("in drawRefittedTrack");

	Amg::Vector3D mom(px,py,pz);
	Amg::Vector3D pos(x,y,z);
      
	const Trk::Track* track = getRefittedTrack(pos,mom,charge);
  
  //getting track space-points
	std::vector<Amg::Vector3D >* points = getPoints(track);

  //drawing
	SoSwitch* trackSwitch = new SoSwitch();
	root->addChild(trackSwitch);
	trackSwitch->whichChild = m_showRefitted ? SO_SWITCH_ALL : SO_SWITCH_NONE;
	drawPoints(trackSwitch,points,color,2,false);
	m_refittedSwitches.push_back(trackSwitch);
  
  //cleaning up
	delete points;
	delete track;

	messageDebug("leaving drawCutoffTrackParticle");

}
//___________________________________________________________
void VP1BPhysSystem::drawPoints(SoSwitch* trackSwitch, std::vector<Amg::Vector3D >* points,
				unsigned long color, double width, bool isNeutral)
{
	messageDebug("in drawPoints");
	if(points == NULL) return;

	SoSeparator* sep = new SoSeparator();
  
  //setting up draw style
	SoDrawStyle* drawStyle = new SoDrawStyle();
	drawStyle->lineWidth = width;
	if(isNeutral) {
		drawStyle->linePattern = 0x00ff;
	}
	sep->addChild(drawStyle);

  //setting up color
	SoBaseColor* baseColor = new SoBaseColor();
	double r,g,b;
	getColor(color,r,g,b);
	baseColor->rgb.setValue(r,g,b);
	sep->addChild(baseColor);
  
	int iver(0);
	SoVertexProperty *vertices = new SoVertexProperty();
  
	std::vector<Amg::Vector3D >::iterator it, itE = points->end();
	for(it = points->begin(); it != itE; ++it) {
		vertices->vertex.set1Value(iver++,(*it).x(),(*it).y(),(*it).z());
	}

  //Create a set of lines from these vertices:
	SoLineSet * line = new SoLineSet();
	line->numVertices = iver;
	line->vertexProperty = vertices;
  
  //Add to the tree:
	sep->addChild(line);

  //add to the root
	trackSwitch->addChild(sep);
  
  //To avoid GUI freeze-ups:
	updateGUI();
  
	messageDebug("leaving drawPoints");
  
}
//___________________________________________________________                                    

std::vector<Amg::Vector3D >* VP1BPhysSystem::getPoints(const Trk::Track* track) {

	messageDebug("in getPoints(Trk::Track)");

  //retrieve/create VP1Extrapolator
  
	messageDebug("Retrieving propagator...");
	Trk::IExtrapolator* propagator = NULL;
	VP1ToolAccessHelper toolaccess(this);
	propagator = toolaccess.getToolPointer<Trk::IExtrapolator>("Trk::Extrapolator/VP1Extrapolator",false/*silent*/,true/*create if not exists*/);
	if(propagator == NULL) {
		message("Error: propagator Trk::Extrapolator/VP1Extrapolator couldn't be created");
	}
	messageDebug("...done");

	messageDebug("Creating propagationHelper...");
	TrackPropagationHelper* propagationHelper = new TrackPropagationHelper(this);
	messageDebug("...done");

	std::vector< Amg::Vector3D >* points = new std::vector<Amg::Vector3D >();

//   propagator = NULL;
  //propagate track in magnetic field
	if(propagator!=NULL && propagationHelper!=NULL) {
		messageDebug("Starting propagator...");
		propagationHelper->makePointsCharged(*points,track,propagator);
		messageDebug("..done");
	} else {
    //if no propagator is initialized we use straight segments instead

		const DataVector<const Trk::TrackParameters> *params = track->trackParameters();
    //Just a sanity check:
		if ( !params ) {
			message("Error: no track parameters");
		}else{
			DataVector<const Trk::TrackParameters>::const_iterator it, itE = params->end();
			for (it = params->begin();it!=itE;++it) {
				points->push_back(Amg::Vector3D((*it)->position().x(),(*it)->position().y(),(*it)->position().z()));
			}
		}
	}

  //cleaning up
	delete propagationHelper;
  
  //return
	messageDebug("leaving getPoints(Trk::Track)");
	return points;

}
//___________________________________________________________                                    
std::vector<Amg::Vector3D >* VP1BPhysSystem::getPoints(const Rec::TrackParticle* part) {
	messageDebug("in getPoints(Rec::TrackParticle)");

  //creating Trk::Track from TrackParticle
	const Trk::Track *track = getTrack(part);

	std::vector<Amg::Vector3D >* points = getPoints(track);

  //cleaning up
	delete track;
  
  //return
	messageDebug("leaving getPoints(Rec::TrackParticle)");
	return points;
}
//___________________________________________________________
std::vector<Amg::Vector3D >* VP1BPhysSystem::findClosestApproach(std::vector<Amg::Vector3D >* points, double x, double y, double z) {
	messageDebug("in findClosestApproach");

	std::vector<Amg::Vector3D >* pointsOut = new std::vector<Amg::Vector3D >();

  //check # of points
	if(points->size()<2) return pointsOut;

	double lastDist = 1e10;
	Amg::Vector3D point = points->at(0);
	std::vector<Amg::Vector3D >::iterator startFrom = (points->begin())+1;
  
  //find the closest approach
	std::vector<Amg::Vector3D >::iterator it, itE = points->end();
	for(it = points->begin(); (it+1) != itE; ++it) {
		CLHEP::Hep3Vector p = CLHEP::Hep3Vector((*(it+1)).x()-(*it).x(), (*(it+1)).y()-(*it).y(), (*(it+1)).z()-(*it).z());
		CLHEP::Hep3Vector v = CLHEP::Hep3Vector(x - (*it).x(), y - (*it).y(), z - (*it).z());

    //projection of v into the direction of p
		double proj = v.dot(p.unit());
		CLHEP::Hep3Vector vect = p.unit();
		vect *= proj;

    //check that projection lies inside the track segment
		if(proj<0 || proj > p.mag()) continue;

    //check distance to the segment (Pythagorean theroem)
		double dist = sqrt(p.mag2() - proj*proj);
		if(dist<lastDist) {
			lastDist = dist;
			point = Amg::Vector3D((*it).x() + vect.x(), (*it).y() + vect.y(), (*it).z() + vect.z());
			startFrom = it+1;
		}
	}

  //create new cut-off track
	pointsOut->push_back(point);
	for(it = startFrom; it!=itE; ++it) {
		pointsOut->push_back(*it);
	}

	messageDebug("leaving findClosestApproach");
	return pointsOut;

}
//___________________________________________________________

void VP1BPhysSystem::drawVertex(SoSeparator *root, double x, double y, double z, double /*radius*/,
				double xx, double xy, double xz, double yy, double yz, double zz, unsigned long color,
				std::vector<SoSwitch*>& vertexSwitches)
{
	messageDebug("in drawVertex");

	SoSeparator* sep = new SoSeparator();
  
  //move sphere to position
	SoTranslation * translation = new SoTranslation;
	translation->translation.setValue ( x, y, z );
	sep->addChild ( translation );

  //color
	SoBaseColor* baseColor = new SoBaseColor();
	double rr,gg,bb;
	getColor(color,rr,gg,bb);
	baseColor->rgb.setValue(rr,gg,bb);
	sep->addChild(baseColor);

  //switch
	SoSwitch* vtxSwitch = new SoSwitch();
	if(!m_showVertices)
		vtxSwitch->whichChild = SO_SWITCH_NONE;
	else
		vtxSwitch->whichChild = m_vertexStyle;
  
	sep->addChild(vtxSwitch);
  
  ///sphere
	SoSphere * sphere = new SoSphere;
	sphere->radius = 2*CLHEP::mm;
	vtxSwitch->addChild ( sphere );

  ///cross
	SoLineSet* cross = createCross(3*CLHEP::mm);
	vtxSwitch->addChild ( cross );
  
  ///ellipsoid
	SoSeparator* sepEllipsoid = new SoSeparator();
	vtxSwitch->addChild ( sepEllipsoid );
  
  //transorm sphere to elipsoid
	double a=xx*CLHEP::mm;
	double b=xy*CLHEP::mm;
	double c=xz*CLHEP::mm;
	double d=yy*CLHEP::mm;
	double e=yz*CLHEP::mm;
	double f=zz*CLHEP::mm;
  
	double det = a*(d*f-e*e) + 2*b*c*e - d*c*c-f*b*b;
	if (det>0) {
		double sixthrootofdet = exp(log(det)/6.0);
		double invdet = 1.0/sixthrootofdet;
		a *= invdet;
		b *= invdet;
		c *= invdet;
		d *= invdet;
		e *= invdet;
		f *= invdet;
		SbMatrix sbMat(a,b,c,0,
									 b,d,e,0,
									 c,e,f,0,
									 0,0,0,1);
		SoMatrixTransform * matTrans = new SoMatrixTransform();
		matTrans->matrix.setValue(sbMat);
		sepEllipsoid->addChild (matTrans);
	} else {
    //FIXME: warn
	}
	SoSphere * ellipsoid = new SoSphere;
	ellipsoid->radius = 3;
	sepEllipsoid->addChild ( sphere );
  

  
  //vertexSwitches
	vertexSwitches.push_back(vtxSwitch);
  
  //add to root
	root->addChild(sep);

  //To avoid GUI freeze-ups:
	updateGUI();

	messageDebug("leaving drawVertex");

}

//___________________________________________________________
/// slots *****************************************************************************
void VP1BPhysSystem::loadFile() {
  
	messageDebug("in loadFile");

	QString fileName = QFileDialog::getOpenFileName(NULL, tr("Open File"),tr("."),tr("ROOT files (*.root)"));
	if(fileName.isEmpty()) return;

	c->ui.leFileName->setText(fileName);
	m_fileName = fileName;
	
	
  //close previous file
	if(m_rootFile!=NULL) {
		m_rootFile->Close();

		if(m_br!=NULL) {
			delete m_br;
			m_br = NULL;
		}
    
		m_rootFile = NULL;
		m_tree = NULL;
	}

  //try to open a new file
	m_rootFile = TFile::Open(m_fileName.toStdString().c_str());
	if(m_rootFile!=NULL && !m_rootFile->IsZombie()) {
		m_tree = (TTree*)m_rootFile->Get("vp1bphys");
		if(m_tree!=NULL) {
			m_tree->BuildIndex("runNum","evtNum");
			m_br = new Br();
			m_br->init(m_tree);
			c->ui.lStatus->setText("");
		}else{
			c->ui.lStatus->setText("File doesn't contain vp1bphys tree");
			message("File doesn't contain vp1bphys tree");
		}
	}else{
		c->ui.lStatus->setText("File doesn't exist");
		message("File doesn't exist");
		m_tree = NULL;
	}

	if(m_sg!=NULL && m_root!=NULL) actualBuild();
	updateGUI();
	
	messageDebug("leaving loadFile");
}
//___________________________________________________________
void VP1BPhysSystem::displayVerticesChanged ( int state ) {

	SoSFInt32 which;
  
	if(state==Qt::Unchecked) {
		m_showVertices = false;
		which = SO_SWITCH_NONE;
	}else{
		m_showVertices = true;
		which = m_vertexStyle;
	}
  
  //apply
	std::vector<SoSwitch*>::iterator it=m_vertexSwitches.begin();
	for(; it!=m_vertexSwitches.end(); ++it) {
		(*it)->whichChild = which;
	}

}
//___________________________________________________________
void VP1BPhysSystem::sphereToggled ( bool checked ) {

  
	if(checked) {
		messageDebug("show vertices as spheres");
		m_vertexStyle=0;
	}

  //apply
	if(m_showVertices) {
		std::vector<SoSwitch*>::iterator it=m_vertexSwitches.begin();
		for(; it!=m_vertexSwitches.end(); ++it) {
			(*it)->whichChild = m_vertexStyle;
		}
	}
}
//___________________________________________________________
void VP1BPhysSystem::crossToggled ( bool checked ) {
	if(checked) {
		messageDebug("show vertices as crosses");
		m_vertexStyle=1;
	}

  //apply
	if(m_showVertices) {
		std::vector<SoSwitch*>::iterator it=m_vertexSwitches.begin();
		for(; it!=m_vertexSwitches.end(); ++it) {
			(*it)->whichChild = m_vertexStyle;
		}
	}
  
}
//___________________________________________________________
void VP1BPhysSystem::ellipsoidToggled ( bool checked ) {
	if(checked) {
		messageDebug("show vertices as error ellipsoids");
		m_vertexStyle=2;
	}

  //apply
	if(m_showVertices) {
		std::vector<SoSwitch*>::iterator it=m_vertexSwitches.begin();
		for(; it!=m_vertexSwitches.end(); ++it) {
			(*it)->whichChild = m_vertexStyle;
		}
	}
  
}
//___________________________________________________________
void VP1BPhysSystem::displayAllTracksChanged ( int state ) {
	if(state==Qt::Unchecked)
		m_showAll = false;
	else
		m_showAll = true;

  //loop over all tracks
	std::vector<SoSwitch*>::iterator it=m_trackSwitches.begin();
	for(; it!=m_trackSwitches.end(); ++it) {
		(*it)->whichChild = m_showAll ? SO_SWITCH_ALL : SO_SWITCH_NONE;
	}

  //loop over overlap
	it=m_overlapSwitches.begin();
	for(; it!=m_overlapSwitches.end(); ++it) {
		(*it)->whichChild = m_showAll && !m_showSignal && !m_showRefitted ? SO_SWITCH_ALL : SO_SWITCH_NONE;
	}
}
//___________________________________________________________
void VP1BPhysSystem::displayOrigSignalChanged ( int state ) {
	if(state==Qt::Unchecked)
		m_showSignal = false;
	else
		m_showSignal = true;

  //loop over original signal tracks
	std::vector<SoSwitch*>::iterator it=m_signalSwitches.begin();
	for(; it!=m_signalSwitches.end(); ++it) {
		(*it)->whichChild = m_showSignal ? SO_SWITCH_ALL : SO_SWITCH_NONE;
	}

  //loop over overlap
	it=m_overlapSwitches.begin();
	for(; it!=m_overlapSwitches.end(); ++it) {
		(*it)->whichChild = m_showAll && !m_showSignal && !m_showRefitted ? SO_SWITCH_ALL : SO_SWITCH_NONE;
	}
}
//___________________________________________________________
void VP1BPhysSystem::displayRefTracksChanged ( int state ) {
	if(state==Qt::Unchecked)
		m_showRefitted = false;
	else
		m_showRefitted = true;
  
  //loop over refitted signal tracks
	std::vector<SoSwitch*>::iterator it=m_refittedSwitches.begin();
	for(; it!=m_refittedSwitches.end(); ++it) {
		(*it)->whichChild = m_showRefitted ? SO_SWITCH_ALL : SO_SWITCH_NONE;
	}
  //loop over overlap
	it=m_overlapSwitches.begin();
	for(; it!=m_overlapSwitches.end(); ++it) {
		(*it)->whichChild = m_showAll && !m_showSignal && !m_showRefitted ? SO_SWITCH_ALL : SO_SWITCH_NONE;
	}

}
//___________________________________________________________
void VP1BPhysSystem::displayNeutralChanged ( int state ) {
	if(state==Qt::Unchecked)
		m_showNeutral = false;
	else
		m_showNeutral = true;

  //loop over neutral tracks
	std::vector<SoSwitch*>::iterator it=m_neutralSwitches.begin();
	for(; it!=m_neutralSwitches.end(); ++it) {
		(*it)->whichChild = m_showNeutral ? SO_SWITCH_ALL : SO_SWITCH_NONE;
	}
}
//___________________________________________________________

/// utils *****************************************************************************
const Trk::Track* VP1BPhysSystem::getTrack(const Rec::TrackParticle* trackparticle) {
	messageDebug("in getTrack");

	messageDebug("Retrieving track parameters...");

////// OLD //	const std::vector< const Trk::ParametersBase* >&  trackpars = trackparticle->trackParameters();
	std::vector< const Trk::TrackParameters* >  trackpars;
	trackpars = trackparticle->trackParameters();

	DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;
	messageDebug("...done");

	if (!trackpars.empty()) {
		bool needresorting = trackpars.at(0)!=trackparticle->perigee();//Needed since TrackParticles are (at the moment)
                                                                   //created with the first parameter put last
		messageDebug("resorting...");
		if (needresorting) {
			messageDebug("dynamic cast");

			//const Trk::ParametersT<Trk::Charged>* p = dynamic_cast<const Trk::ParametersT<Trk::Charged>* >(trackpars.at(trackpars.size()-1));
			const Trk::TrackParameters* p = dynamic_cast<const Trk::TrackParameters* >(trackpars.at(trackpars.size()-1));

			messageDebug("new TrackStateOnSurface");

			if (p) trackStateOnSurfaces->push_back(new Trk::TrackStateOnSurface(0,p->clone(),0,0));
		}
		unsigned limit(needresorting?trackpars.size()-1:trackpars.size());
		messageDebug("...done");

    //NB: We only ever created this handle if charge()!=0.0:
		messageDebug("Filling the vector...");
		for (unsigned i = 0; i < limit; ++i) {
//			const Trk::ParametersT<Trk::Charged>* p = dynamic_cast<const Trk::ParametersT<Trk::Charged>* >(trackpars.at(i));
			const Trk::TrackParameters* p = dynamic_cast<const Trk::TrackParameters* >(trackpars.at(i));
			if (!p)
				continue;
/*      if (!common()->trackSanityHelper()->isSafe(p))
			continue;*/
			trackStateOnSurfaces->push_back(new Trk::TrackStateOnSurface(0,p->clone(),0,0));
		}
		messageDebug("...done");
    
	}

	messageDebug("Creating the track...");

#ifdef TRKTRACK_TRACKINFO_H
    Trk::TrackInfo ti(Trk::TrackInfo::Unknown,Trk::pion);
    const Trk::Track * trk = new Trk::Track(ti,trackStateOnSurfaces/*track assumes ownership*/,0/*fitquality*/);
#else
    const Trk::Track * trk = new Trk::Track(Trk::Track::unknown, trackStateOnSurfaces/*track assumes ownership*/,
																						0/*fitquality*/,Trk::pion);
#endif
  
  messageDebug("...done");

  messageDebug("leaving getTrack");
	return trk;

}
//___________________________________________________________
const Trk::Track* VP1BPhysSystem::getRefittedTrack(Amg::Vector3D position, Amg::Vector3D momentum, double charge) {

  
	Amg::Vector3D pos(position);
	Amg::Vector3D mom(momentum);

	// init the error matrix
	AmgSymMatrix(5) covMtxP;
	covMtxP.setIdentity();
  
	AmgSymMatrix(5)* errMatr = &covMtxP;
	Trk::Perigee * measuredPerigee  =  new Trk::Perigee( pos, mom, charge, pos, errMatr );

	//creates parameter base for the new track
	const Trk::TrackParameters* p = dynamic_cast<const Trk::TrackParameters* >(measuredPerigee);

  //TODO: check parameters safety

  //creates a vector of TracksStates on surface
	DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>();
	trackStateOnSurfaces->push_back(new Trk::TrackStateOnSurface(0,p->clone(),0,0));

  //create track
#ifdef TRKTRACK_TRACKINFO_H
    Trk::TrackInfo ti(Trk::TrackInfo::Unknown,Trk::pion);
    const Trk::Track * trk = new Trk::Track(ti,trackStateOnSurfaces/*track assumes ownership*/,0/*fitquality*/);
#else
    const Trk::Track * trk = new Trk::Track(Trk::Track::unknown, trackStateOnSurfaces/*track assumes ownership*/,
																						0/*fitquality*/,Trk::pion);
#endif
  
  return trk;
  

}

//___________________________________________________________

void VP1BPhysSystem::getColor(unsigned long icolor, double& r, double& g, double& b) {
	messageDebug("in getColor");
	
	unsigned long red = icolor/65536;
	icolor -= red*65536;

	unsigned long green = icolor/256;
	icolor -= green*256;

	unsigned long blue = icolor;

	r=red/255.;
	g=green/255.;
	b=blue/255.;

	messageDebug("leaving getColor");

}
//___________________________________________________________
SoLineSet* VP1BPhysSystem::createCross(double extent ) {
	messageDebug("in createCross");
  
	SoVertexProperty *vertices = new SoVertexProperty();
	vertices->vertex.set1Value ( 0,-extent, 0      , 0      );
	vertices->vertex.set1Value ( 1, extent, 0      , 0      );
	vertices->vertex.set1Value ( 2, 0     ,-extent , 0      );
	vertices->vertex.set1Value ( 3, 0     , extent , 0      );
	vertices->vertex.set1Value ( 4, 0     , 0      ,-extent );
	vertices->vertex.set1Value ( 5, 0     , 0      , extent );
	SoLineSet * line = new SoLineSet();
	line->numVertices.set1Value(0,2);
	line->numVertices.set1Value(1,2);
	line->numVertices.set1Value(2,2);
	line->vertexProperty = vertices;

	messageDebug("leving createCross");
	return line;
}

