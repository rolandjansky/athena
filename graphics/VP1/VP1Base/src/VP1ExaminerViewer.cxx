/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1ExaminerViewer                 //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
//  Update: Sep 2013, Riccardo-Maria BIANCHI rbianchi@cern.ch //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Base/VP1ExaminerViewer.h"
#include "VP1Base/VP1Controller.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Base/VP1CameraHelper.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1Base/AnimationSequencer.h"
#include "VP1Base/VP1Settings.h"
#include "VP1Base/VP1CustomTourEditor.h"
#include "VP1Base/VP1CustomStereoEditor.h"
#include "VP1HEPVis/VP1HEPVisUtils.h"
#include "VP1HEPVis/actions/SoGL2PSAction.h"
#include "VP1HEPVis/gl2ps.h"

#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/Qt/widgets/SoQtPopupMenu.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoEnvironment.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/nodekits/SoBaseKit.h>

#include <QBuffer>
#include <QByteArray>
#include <QMenu>
#include <QCursor>
#include <QColorDialog>
#include <QFileDialog>
#include <QInputDialog>
#include <QPushButton>
#include <QGLFormat>

#include <map>
#include <iostream>
#include <typeinfo>

//TODO: Statustips.
//TODO: windows menu button should launch the menu (in correct mode?).
//TODO: detectorviewbuttons (+others) in menu so hidden control modes has access to everything!
//TODO: preserve spinning when going to next event (or when tour returns).

//____________________________________________________________________
class VP1ExaminerViewer::Imp {

public:

	Imp(VP1ExaminerViewer*tc,bool dvb)
: theclass(tc),
  takeTourButton(0),
  rotationMode(ZROT),
  actualSceneGraph(0),
  environmentNode(0),
  customtoureditor(0),
  customstereoeditor(0),
  popup_menu(0),
  popup_headLightAction(0),
  popup_antiAliasAction(0),
  popup_bgdColAction(0),
  popup_ambientLightAction(0),
  popup_focalLengthAction(0),
  popup_dumpSceneAction(0),
  popup_dumpSceneVRMLAction(0),
  popup_toSVGAction(0),
  popup_toEPSAction(0),
  popup_resetCameraAction(0),
  popup_hidedecorationsaction(0),
  popup_tourReturnToStartAction(0),
  popup_tourExecute(0),
  popup_tourStartEachEvent(0),
  popup_tourSpeedVerySlow(0),
  popup_tourSpeedSlow(0),
  popup_tourSpeedMedium(0),
  popup_tourSpeedFast(0),
  popup_tourSpeedVeryFast(0),
  popup_tourPartsVertex(0),
  popup_tourPartsInDet(0),
  popup_tourPartsCalo(0),
  popup_tourPartsMuon(0),
  popup_tourLoopOnce(0),
  popup_tourLoopTwice(0),
  popup_tourLoopThrice(0),
  popup_tourLoopForever(0),
  viewmenu_storecurrentview(0),
  viewmenu_deleteview(0),
  viewmenu_restoreview(0),
  viewmenu_zoomtoview(0),
  customtour_launcheditor(0),
  customtour_execute(0),
  stereo_launcheditor(0),
  action_movieenabled(0),
  action_moviewidth(0),
  action_movieheight(0),
  action_moviefps(0),
  action_movieoutdir(0),
  action_moviefadetocurrentview(0),
  stereo_offset_value(0.0),
  popup_focal_value_action(0),
  detectorViewButtons(dvb),
  animationSequencer(tc),
  tourLoopsForever(false),
  tourLoopsForeverSkipFirstFrame(false),
  signalcatcher(new VP1ExaminerViewer_SignalCatcher),
  isantialias(false),
  button_interact(0),
  button_examine(0),
  button_home(0),
  button_sethome(0),
  button_viewall(0),
  button_seek(0),
  button_togglecamera(0),
  sphere(0),
  decorationMenuRemoved(false),
  ambientLightPercentage(20),
  movieFrameFileNamePrefix("vp1_frame"),
  resetCamera_isPerspective(false)
{
		VP1Msg::messageVerbose("VP1ExaminerViewer::Imp() constructor");
		signalcatcher->d = this; init();

		// TEST
		// With this command, Coin does not clear the pixel-buffer,
		// so the previous (or background) image
		// is not removed before the new rendering.
		// theclass->setClearBeforeRender(FALSE, TRUE);
		// theclass->setClearBeforeRender(FALSE, FALSE);
		// theclass->setClearBeforeOverlayRender(FALSE);
}


	~Imp()
	{
		VP1Msg::messageVerbose("VP1ExaminerViewer::~Imp() destructor");
		if (actualSceneGraph)
			actualSceneGraph->unref();
		if (environmentNode)
			environmentNode->unref();
		if (customtoureditor)
			customtoureditor->deleteLater();
		if (customstereoeditor)
			customstereoeditor->deleteLater();
		delete popup_menu;
		delete signalcatcher;
		if (sphere)
			sphere->unref();
	}



	  enum REGION { VERTEX, INDET, CALO, MUON, FORWARDREGION };
	  QString toString(const REGION& r) {
	    switch (r) {
	    case VERTEX: return "VERTEX";
	    case INDET: return "INDET";
	    case FORWARDREGION: return "FORWARDREGION";
	    case CALO: return "CALO";
	    case MUON: return "MUON";
	    default: return "Unknown(ERROR)";
	    }
	  }

	VP1ExaminerViewer * theclass;

	enum VIEW { ASIDE, CSIDE, BARREL };
	std::map<QPushButton*,std::pair<REGION,VIEW> > detectorbuttons;
	QPushButton *takeTourButton; // * JFB

	enum ROTATIONMODE { XROT, ZROT };
	ROTATIONMODE rotationMode; //!< Which mode is left wheel in?


	QPushButton * addNewButton(QString text,QString tooltip,REGION region ,VIEW view,QWidget * tempparent, QString iconname="");
	void init();
	void detectorZoomButtonClicked(std::pair<REGION,VIEW>);
	void takeTourButtonClicked();          // * JFB
	void grabFocus();


	SoGroup * actualSceneGraph;
	SoEnvironment * environmentNode;




	bool ensureMenuInit();
	void updatePopupMenuStates();
	QByteArray customtoureditorState;
	VP1CustomTourEditor * customtoureditor;
	VP1CustomStereoEditor * customstereoeditor;
	QMenu * popup_menu;
	QAction* popup_headLightAction;
	QAction* popup_antiAliasAction;
	QAction* popup_bgdColAction;
	QAction* popup_ambientLightAction;
	QAction* popup_focalLengthAction;
	QAction* popup_dumpSceneAction;
	QAction* popup_dumpSceneVRMLAction;
	QAction* popup_toSVGAction;
	QAction* popup_toEPSAction;
	QAction* popup_resetCameraAction;
	QAction* popup_hidedecorationsaction;
	QList<QAction*> popup_transptype_actions;
	QList<QAction*> popup_drawstyle_still_actions;
	QList<QAction*> popup_drawstyle_interactive_actions;
	QAction* popup_tourReturnToStartAction;
	QAction* popup_tourExecute;
	QAction* popup_tourStartEachEvent;
	QAction* popup_tourSpeedVerySlow;
	QAction* popup_tourSpeedSlow;
	QAction* popup_tourSpeedMedium;
	QAction* popup_tourSpeedFast;
	QAction* popup_tourSpeedVeryFast;
	QAction* popup_tourPartsVertex;
	QAction* popup_tourPartsInDet;
	QAction* popup_tourPartsCalo;
	QAction* popup_tourPartsMuon;
	QAction* popup_tourLoopOnce;
	QAction* popup_tourLoopTwice;
	QAction* popup_tourLoopThrice;
	QAction* popup_tourLoopForever;
	QMenu * viewmenu_storecurrentview;
	QMenu * viewmenu_deleteview;
	QMenu * viewmenu_restoreview;
	QMenu * viewmenu_zoomtoview;
	QAction * customtour_launcheditor;
	QAction * customtour_execute;
	QAction* stereo_launcheditor;
	QAction * action_movieenabled;
	QAction * action_moviewidth;
	QAction * action_movieheight;
	QAction * action_moviefps;
	QAction * action_movieoutdir;
	QAction * action_moviefadetocurrentview;
	// stereo view
	float stereo_offset_value;
	QAction* popup_focal_value_action;
//	QList<QAction*> popup_stereo_offset_actions;
//	QList<QAction*> popup_stereo_type_actions;
//	QList<QAction*> popup_stereo_anaglyph_actions;
//	QList<QAction*> popup_stereo_anaglyph_actions_camera;
//	QMenu * stereo_set_anaglyph_menu;
//	QMenu * stereo_set_stereo_camera_view_menu;
//	QAction* popup_stereo_offset_value_action;
//	QAction* popup_stereo_offset_value_action_camera;
//	QAction* popup_stereo_balance_value_action_camera;


	void updateEnvironmentNode() {
		if (environmentNode) {
			int al = theclass->ambientLight();
			environmentNode->ambientIntensity = (al==0?0.0f:(al==100?1.0f:al/100.0f));
		}
	}


	bool detectorViewButtons;
	AnimationSequencer animationSequencer;
	bool tourLoopsForever;
	bool tourLoopsForeverSkipFirstFrame;
	void applyMovieSettingsToAnimationSequencer();
	bool updateAnimationSequence();//Returns false if nothing is to be animated (i.e. no regions selected)
	VP1ExaminerViewer_SignalCatcher * signalcatcher;
	bool isantialias;
	QPushButton * button_interact;
	QPushButton * button_examine;
	QPushButton * button_home;
	QPushButton * button_sethome;
	QPushButton * button_viewall;
	QPushButton * button_seek;
	QPushButton * button_togglecamera;
	SoSphere * sphere;
	SoSphere * getRegionSphere(REGION,bool perspective);

	bool decorationMenuRemoved;

	float rotZWheelMotion(float value, float oldvalue);
	float rotXWheelMotion(float value, float oldvalue);
	void rotateCamera(SoCamera * cam,const SbVec3f & aroundaxis,const float delta);
	static int drawStyleToInt(SoQtViewer::DrawStyle);
	static SoQtViewer::DrawStyle intToDrawStyle(int);


	void updateAmbientLightText() {
		if (popup_ambientLightAction)
			popup_ambientLightAction->setText("Change ambient &light [current "+QString::number(ambientLightPercentage)+"%]");
	}
	int ambientLightPercentage;


	QString movieFrameFileNamePrefix;

	void updateMovieMenuVisuals();

	void aboutToShowMenu(QMenu *);

	QString lastDumpFile;
	QString lastSVGFile;
	QString lastEPSFile;
	bool resetCamera_isPerspective;
	QByteArray resetCamera_state;

	class StoredView {
	public:
		StoredView(QByteArray camState, bool camPerspective, QPixmap snapShot, QString name)
	: m_camState(camState), m_camPerspective(camPerspective), m_snapShot(snapShot), m_name(name) {}
		StoredView(QByteArray persistifiedState);
		QString name() const { return m_name; }
		QByteArray camState() const { return m_camState; }
		bool camStateIsPerspective() const { return m_camPerspective; }
		QPixmap snapShot() const { return m_snapShot; }
		QByteArray persistifiedState() const;
		QIcon icon() const {
			if (m_cachedIcon.isNull())
				m_cachedIcon = QIcon(m_snapShot);
			return m_cachedIcon;
		}
		bool isValid() { return !m_name.isEmpty()&&!m_camState.isEmpty()&&!m_snapShot.isNull(); }


	private:
		QByteArray m_camState;
		bool m_camPerspective;
		QPixmap m_snapShot;//FIXME: Update this whenever we have just "restored view"
		QString m_name;
		mutable QIcon m_cachedIcon;
	};

	bool fitsCurrentCamType(const StoredView& sv) { return sv.camStateIsPerspective()==theclass->currentCamIsPerspective(); }

	QList<StoredView> storedViews;
	void storedViewsChanged() {
		if (viewmenu_deleteview)
			viewmenu_deleteview->setEnabled(!storedViews.isEmpty());
		if (viewmenu_restoreview)
			viewmenu_restoreview->setEnabled(!storedViews.isEmpty());
		if (viewmenu_zoomtoview)
			viewmenu_zoomtoview->setEnabled(!storedViews.isEmpty());
	}
	QList<QAction*> storeViewActions;
	QList<QAction*> restoreViewActions;
	QList<QAction*> deleteViewActions;
	QList<QAction*> zoomToViewActions;
	QString firstAvailableStoredViewName() {
		const bool perspective(theclass->currentCamIsPerspective());
		int i(1);
		while (true) {
			QString n = QString(perspective?"Perspective":"Orthographic")+"View"+QString::number(i++);
			bool ok(true);
			foreach(StoredView sv, storedViews) {
				if (sv.name()==n) {
					ok = false;
					break;
				}
			}
			if (ok)
				return n;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////
	//    For persistification, etc. of draw styles (possibly move to util class)    //
	///////////////////////////////////////////////////////////////////////////////////
	static QList<SoQtViewer::DrawStyle> getAllViewerDrawStyles();
	static int viewerDrawStyleToInt( SoQtViewer::DrawStyle );//Returns -1 if unknown.
	static SoQtViewer::DrawStyle intToViewerDrawStyle( int );//Return VIEW_AS_IS if unknown;
	static QString viewerDrawStyle2PrettyString( SoQtViewer::DrawStyle );//Returns "" if unknown.
	// Stereo view
		static QList<SoQtViewer::StereoType> getAllStereoViewTypes();
		static int viewerStereoTypeToInt( SoQtViewer::StereoType );//Returns -1 if unknown.
		static SoQtViewer::StereoType intToViewerStereoType( int );//Return STEREO_NONE if unknown;
		static QString viewerStereoType2PrettyString( SoQtViewer::StereoType );//Returns "" if unknown.



};




//____________________________________________________________________
QByteArray VP1ExaminerViewer::Imp::StoredView::persistifiedState() const
{
	VP1Msg::messageVerbose("persistifiedState()");

	// ===> Setup stream writing to a byteArray:
	QByteArray byteArray;
	QBuffer buffer(&byteArray);
	buffer.open(QIODevice::WriteOnly);
	QDataStream out(&buffer);

	// ===> Write Data:
	out << (qint32)0;//version
	out << m_name;
	out << m_camState;
	out << m_camPerspective;
	out << m_snapShot;

	// ===> Finish up:
	buffer.close();
	return byteArray;
}

//____________________________________________________________________
VP1ExaminerViewer::Imp::StoredView::StoredView(QByteArray persistifiedState)
: m_camPerspective(true)
{
	VP1Msg::messageVerbose("StoredView()");

	// ===> Setup stream for getting the contents of the byteArray:
	QBuffer buffer(&persistifiedState);
	buffer.open(QIODevice::ReadOnly);
	QDataStream state(&buffer);
	// ===> Check version and pass on state info to base class:
	qint32 version;
	state >> version;
	if (version!=0) {
		VP1Msg::messageDebug("VP1ExaminerViewer Warning: "
				"Attempt to depersistify stored view ignored due to bad version!");
		return;
	}
	// ===> Decode the state info:
	state >> m_name;
	state >> m_camState;
	state >> m_camPerspective;
	state >> m_snapShot;
	if (!buffer.atEnd())
		m_name = "";//To make isValid() return false;
	// ===> Finish up:
	buffer.close();
}



//____________________________________________________________________
/**
 * tune the camera clipping planes
 */

//  N.B.
//  this code has been taken and adapted from CMSSW:
//  http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/CMSSW/ISpy/Client/src/ISpy3DView.cc?view=co
//
void debugCameraClipPlanes(void * data, const SbVec2f & nearfar)
{
	SoCamera *camera = ((VP1ExaminerViewer *) data)->getCamera();
	SoNode *scenegraph = ((VP1ExaminerViewer *) data)->getSceneGraph();

	SbVec3f pos = camera->position.getValue();

	SoSearchAction action;
	SbBool oldsearch = SoBaseKit::isSearchingChildren();
	SoBaseKit::setSearchingChildren(TRUE);

	action.setSearchingAll(true);
	action.setType(SoCamera::getClassTypeId());
	action.setInterest(SoSearchAction::ALL);
	action.apply(scenegraph);

	SoBaseKit::setSearchingChildren(oldsearch);

	SoPathList &cameras = action.getPaths();

	for (int i = 0, e = cameras.getLength(); i != e; ++i)
		std::cerr << "Camera #" << i << " = " << (void *) cameras[i]->getTail() << "(" << typeid(*cameras[i]->getTail()).name() << ")\n";

	std::cerr << "Calculated clip-planes. Near: " << nearfar[0] << ". Far: " << nearfar[1] << "\n"
			<< "Current camera clip-planes. Near: " << camera->nearDistance.getValue() << ", Far: " << camera->farDistance.getValue() << "\n"
			<< "Camera ptr: " << (void *) camera << "\n"
			<< "Camera name: " << camera->getName() << "\n"
			<< "Camera type: " << typeid(*camera).name() << "\n"
			<< "Camera position:(" << pos[0] << ", " << pos[1] << ", " << pos[2] << ")\n"
			<< "Number of cameras in the scene: " << cameras.getLength() << "\n"
			<< "Focal distance: " << camera->focalDistance.getValue() << std::endl;
}

//____________________________________________________________________
/**
 * set the clipping strategy and fix the clipping
 */

//  N.B.
//  this code has been taken and adapted from CMSSW:
//  http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/CMSSW/ISpy/Client/src/ISpy3DView.cc?view=co
//
SbVec2f fixedDistanceClipPlanesCB(void * data, const SbVec2f & nearfar)
{
	SoCamera *camera = ((VP1ExaminerViewer *) data)->getCamera();
	if (0) debugCameraClipPlanes(data, nearfar);

	if (camera->nearDistance.getValue() != nearfar[0])
		camera->nearDistance = nearfar[0];
	if (camera->farDistance.getValue() != nearfar[1])
		camera->farDistance = nearfar[1];

	return nearfar;
}

//____________________________________________________________________
VP1ExaminerViewer::VP1ExaminerViewer(QWidget * parent,
		bool detectorViewButtons,
		const char * name,
		SbBool embed,
		SoQtFullViewer::BuildFlag flag,
		SoQtViewer::Type type)
: SoQtExaminerViewer(parent,(name?name:"VP1ExaminerViewer"),embed,flag,type,false/*delay createViewerButtons call*/),
  d(new Imp(this,detectorViewButtons))
{
	VP1Msg::messageDebug("VP1ExaminerViewer::VP1ExaminerViewer()");
	// Explicitly trigger the construction of viewer decorations.
	QWidget * widget = buildWidget(getParentWidget());
	if (rightWheelLabel)
		rightWheelLabel->setVisible(false);//it will be shown again by the first setCameraType call
	setBaseWidget(widget);
	setLeftWheelString("Rotz");
	d->rotationMode=Imp::ZROT;
//	setRightWheelString("Zoom");
	setRightWheelString("Zoom test");
	leftWheel->setMinimumHeight(20);
	rightWheel->setMinimumHeight(20);


	/* set the clipping strategy and fix the clipping
	 *
	 *   N.B.
	 *   this code has been taken and adapted from CMSSW:
	 *   http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/CMSSW/ISpy/Client/src/ISpy3DView.cc?view=co
	 */
	setAutoClippingStrategy(CONSTANT_NEAR_PLANE, 0.9, fixedDistanceClipPlanesCB, this);

  // Let's build this now, otherwise there will be problems with making the connections later:
	d->customtoureditor = new VP1CustomTourEditor(this);
	QObject::connect(&(d->animationSequencer),SIGNAL(clipVolumePercentOfATLAS(double)),d->customtoureditor,SLOT(setClipVolumePercentOfATLAS(double)));
  VP1Controller::setCustomTourEditor(d->customtoureditor);
	d->customtoureditor->disableObjectWhenTourNotAvailable(d->customtour_execute);
	if (d->customtoureditorState!=QByteArray()) {
		d->customtoureditor->setState(d->customtoureditorState);
		d->customtoureditorState = QByteArray();
	}

	// test Ric
	VP1Msg::messageVerbose("is stereo: " + QString(isStereoViewing()) + " - offset: " + QString::number(getStereoOffsetSlot()) + " - type: " + QString::number(getStereoType()) );
}


// stereographic view methods
void VP1ExaminerViewer::setStereoOffsetSlot(float offset) {
	VP1Msg::messageDebug("VP1ExaminerViewer::setStereoOffsetSlot() - offset: " + QString::number(offset));
	SoQtExaminerViewer::setStereoOffset(offset);
}
float VP1ExaminerViewer::getStereoOffsetSlot() {
	return SoQtExaminerViewer::getStereoOffset();
}
SbBool VP1ExaminerViewer::setStereoTypeSlot(SoQtViewer::StereoType type) {
	VP1Msg::messageDebug("VP1ExaminerViewer::setStereoTypeSlot() - type: " + QString::number(type));
	return SoQtExaminerViewer::setStereoType(type);
}
SoQtViewer::StereoType VP1ExaminerViewer::getStereoTypeSlot(void) const {
	return SoQtExaminerViewer::getStereoType();
}
void VP1ExaminerViewer::setAnaglyphStereoColorMasksSlot(const SbBool left[3], const SbBool right[3]) {
	SoQtExaminerViewer::setAnaglyphStereoColorMasks(left, right);
}
void VP1ExaminerViewer::getAnaglyphStereoColorMasksSlot(SbBool left[3], SbBool right[3]) {
	SoQtExaminerViewer::getAnaglyphStereoColorMasks(left, right);
}




//____________________________________________________________________
int VP1ExaminerViewer::Imp::drawStyleToInt(SoQtViewer::DrawStyle ds) {
	switch (ds) {
	case VIEW_AS_IS: return 0;
	case VIEW_HIDDEN_LINE: return 1;
	case VIEW_NO_TEXTURE: return 2;
	case VIEW_LOW_COMPLEXITY: return 3;
	case VIEW_LINE: return 4;
	case VIEW_POINT: return 5;
	case VIEW_BBOX: return 6;
	case VIEW_LOW_RES_LINE: return 7;
	case VIEW_LOW_RES_POINT: return 8;
	case VIEW_SAME_AS_STILL: return 9;
	case VIEW_WIREFRAME_OVERLAY 	: return 10;
	default:
		VP1Msg::messageDebug("VP1ExaminerViewer::Imp::drawStyleToInt ERROR: Unknown draw style");
		return 0;
	}
}

//____________________________________________________________________
SoQtViewer::DrawStyle VP1ExaminerViewer::Imp::intToDrawStyle(int i) {
	switch (i) {
	case 0: return SoQtViewer::VIEW_AS_IS;
	case 1: return SoQtViewer::VIEW_HIDDEN_LINE;
	case 2: return SoQtViewer::VIEW_NO_TEXTURE;
	case 3: return SoQtViewer::VIEW_LOW_COMPLEXITY;
	case 4: return SoQtViewer::VIEW_LINE;
	case 5: return SoQtViewer::VIEW_POINT;
	case 6: return SoQtViewer::VIEW_BBOX;
	case 7: return SoQtViewer::VIEW_LOW_RES_LINE;
	case 8: return SoQtViewer::VIEW_LOW_RES_POINT;
	case 9: return SoQtViewer::VIEW_SAME_AS_STILL;
	case 10: return SoQtViewer::VIEW_WIREFRAME_OVERLAY;
	default:
		VP1Msg::messageDebug("VP1ExaminerViewer::Imp::intToDrawStyle ERROR: int out of range "+str(i));
		return SoQtViewer::VIEW_AS_IS;
	}
}

//____________________________________________________________________
QByteArray VP1ExaminerViewer::saveState()
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::saveState");

	// ===> Setup stream writing to a byteArray:
	QByteArray byteArray;
	QBuffer buffer(&byteArray);
	buffer.open(QIODevice::WriteOnly);
	QDataStream out(&buffer);

	// ===> Write Data:
	//Version:
	out << (qint32)6; //version

	//Background color:
	QColor bgdcol(VP1QtInventorUtils::sbcol2qcol(getBackgroundColor()));
	out << bgdcol;
	VP1Msg::messageVerbose("saveState background color = "+str(bgdcol));

	//Render buffer:
	int ibuffer(0);
	if (getBufferingType()==BUFFER_SINGLE)
		ibuffer=1;
	else if (getBufferingType()==BUFFER_INTERACTIVE)
		ibuffer=2;
	out << ibuffer;
	VP1Msg::messageVerbose("saveState Render Buffer = ("+str(ibuffer)+")");

	//Camera type and parameters:
	bool isperspective(currentCamIsPerspective());
	out << isperspective;
	out << currentCameraState();
	VP1Msg::messageVerbose("saveState camera type = "+str(isperspective?"Perspective":"Orthographic"));

	//Misc:
	const bool isdeco(isDecoration()), isheadlight(isHeadlight()),isviewing(isViewing());
	out << isdeco;
	out << isheadlight;
	out << isviewing;
	out << d->decorationMenuRemoved;
	VP1Msg::messageVerbose("saveState (deco,headlight,viewing,nodecomenu) = ("
			+str(isdeco)+", "+str(isheadlight)+", "+str(isviewing)+", "+str(d->decorationMenuRemoved)+")");

	//Antialiasing state:
	out << d->isantialias;//version 6+
	VP1Msg::messageVerbose("saveState (isantialis)) = ("+str(d->isantialias)+")");
	//OLD (up to version 5):
	//   SbBool smoothing; int numPasses;
	//   getAntialiasing(smoothing, numPasses);
	//   out << smoothing;
	//   out << numPasses;
	//   VP1Msg::messageVerbose("saveState (smoothing,numPasses)) = ("+str(smoothing)+", "+str(numPasses)+")");

	//Draw style:
	int idrawstyle_still = Imp::drawStyleToInt(getDrawStyle(STILL));
	int idrawstyle_interactive = Imp::drawStyleToInt(getDrawStyle(INTERACTIVE));
	out << idrawstyle_still;
	out << idrawstyle_interactive;
	VP1Msg::messageVerbose("saveState DrawStyle (still,interactive)) = ("+str(idrawstyle_still)+", "+str(idrawstyle_interactive)+")");

	//Transparency type:
	int itransp = VP1QtInventorUtils::transparencyTypeToInt(getTransparencyType());
	out << itransp;
	VP1Msg::messageVerbose("saveState Transparency Type = ("+str(itransp)+")");

	/////////////////////////
	// Added for version 1 //
	/////////////////////////

	d->ensureMenuInit();
	out << d->popup_tourReturnToStartAction->isChecked();
	out << d->popup_tourStartEachEvent->isChecked();

	qint32 ispeed(0);//medium
	if (d->popup_tourSpeedVerySlow->isChecked()) ispeed = -2;
	else if (d->popup_tourSpeedSlow->isChecked()) ispeed = -1;
	else if (d->popup_tourSpeedFast->isChecked()) ispeed = 1;
	else if (d->popup_tourSpeedVeryFast->isChecked()) ispeed = 2;
	out << ispeed;

	out << d->popup_tourPartsVertex->isChecked();
	out << d->popup_tourPartsInDet->isChecked();
	out << d->popup_tourPartsCalo->isChecked();
	out << d->popup_tourPartsMuon->isChecked();

	qint32 iloop(1);
	if (d->popup_tourLoopTwice->isChecked()) iloop = 2;
	else if (d->popup_tourLoopThrice->isChecked()) iloop = 3;
	else if (d->popup_tourLoopForever->isChecked()) iloop = 999;
	out << iloop;

	/////////////////////////
	// Added for version 2 //
	/////////////////////////
	out << d->resetCamera_isPerspective;
	out << d->resetCamera_state;

	/////////////////////////
	// Added for version 3 //
	/////////////////////////

	QList<QByteArray> persistifiedViews;
	foreach(Imp::StoredView sv, d->storedViews)
	persistifiedViews << sv.persistifiedState();
	out << persistifiedViews;

	/////////////////////////
	// Added for version 4 //
	/////////////////////////

	out << d->ambientLightPercentage;

	/////////////////////////
	// Added for version 5 //
	/////////////////////////

	out << (d->customtoureditor ? d->customtoureditor->state() : QByteArray());

	// ===> Finish up:
	buffer.close();
	return byteArray;

}


//____________________________________________________________________
void VP1ExaminerViewer::restoreFromState(QByteArray ba_state)
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::restoreFromState");

	// ===> Setup stream for getting the contents of the byteArray:
	QBuffer buffer(&ba_state);
	buffer.open(QIODevice::ReadOnly);
	QDataStream state(&buffer);
	// ===> Check version and pass on state info to base class:
	qint32 version;
	state >> version;
	if (version<0||version>6) {
		VP1Msg::messageDebug("VP1ExaminerViewer Warning: Input state data is in wrong format - ignoring!");
		return;
	}
	// ===> Decode the state info:

	//Background color:
	QColor bgdcol;
	state >> bgdcol;
	if (bgdcol.isValid())
		setBackgroundColor(VP1QtInventorUtils::qcol2sbcol(bgdcol));
	VP1Msg::messageVerbose("restoreState background color = "+str(bgdcol));

	//Render buffer:
	int ibuffer;
	state >> ibuffer;
	if (ibuffer==1) setBufferingType(BUFFER_SINGLE);
	else if (ibuffer==2) setBufferingType(BUFFER_INTERACTIVE);
	else setBufferingType(BUFFER_DOUBLE);;
	VP1Msg::messageVerbose("restoreState Render Buffer = ("+str(ibuffer)+")");

	//camtype and camera parameters:
	bool isperspective;
	state >> isperspective;
	if (isperspective != currentCamIsPerspective())
		toggleCameraType();
	QByteArray ba_cam;
	state>>ba_cam;
	if (getCamera()&&ba_cam!=QByteArray())
		VP1QtInventorUtils::deserializeSoCameraParameters(ba_cam,*(getCamera()));//Fixme: Check return value?
	VP1Msg::messageVerbose("restoreState camera type = "+str(isperspective?"Perspective":"Orthographic"));

	//misc:
	bool isdeco, isheadlight, isviewing, decoremoved;
	state >> isdeco; if (isdeco!=isDecoration()) setDecoration(isdeco);
	state >> isheadlight; if (isheadlight!=isHeadlight()) setHeadlight(isheadlight);
	state >> isviewing; if (isviewing!=isViewing()) setViewing(isviewing);
	state >> decoremoved; if (decoremoved) removeDecorationMenuOption();
	VP1Msg::messageVerbose("saveState (deco,headlight,viewing,nodecomenu) = ("
			+str(isdeco)+", "+str(isheadlight)+", "+str(isviewing)+", "+str(decoremoved)+")");

	//Antialiasing state:
	if (version<=5) {
		SbBool smoothing; int numPasses;
		state >> smoothing;
		state >> numPasses;
		d->isantialias=smoothing;
	}
	if (version>=6) {
		state >> d->isantialias;
	}
	setAntiAlias(d->isantialias);
	VP1Msg::messageVerbose("restoreState (isantialias)) = ("+str(d->isantialias)+")");
	//   getAntialiasing(current_smoothing, current_numPasses);
	//   state >> smoothing;
	//   state >> numPasses;
	//   if (numPasses<9999&&(current_smoothing!=smoothing||current_numPasses!=numPasses))
	//     setAntialiasing(smoothing,numPasses);
	//   VP1Msg::messageVerbose("restoreState (smoothing,numPasses)) = ("+str(smoothing)+", "+str(numPasses)+")");

	//Draw style:
	int idrawstyle_still, idrawstyle_interactive;
	state >> idrawstyle_still;
	state >> idrawstyle_interactive;
	setDrawStyle(STILL,Imp::intToDrawStyle(idrawstyle_still));
	setDrawStyle(INTERACTIVE,Imp::intToDrawStyle(idrawstyle_interactive));
	VP1Msg::messageVerbose("restoreState DrawStyle (still,interactive)) = ("+str(idrawstyle_still)+", "+str(idrawstyle_interactive)+")");

	//Transparency type:
	int itransp;
	state >> itransp;
	setTransparencyType(VP1QtInventorUtils::intToTransparencyType(itransp));
	VP1Msg::messageVerbose("restoreState Transparency Type = ("+str(itransp)+")");

	if (version>=1) {

		d->ensureMenuInit();
		bool b;
		state >> b; d->popup_tourReturnToStartAction->setChecked(b);
		state >> b; d->popup_tourStartEachEvent->setChecked(b);

		qint32 ispeed;
		state >> ispeed;
		if (ispeed==-2) d->popup_tourSpeedVerySlow->setChecked(true);
		else if (ispeed==-1) d->popup_tourSpeedSlow->setChecked(true);
		else if (ispeed==1) d->popup_tourSpeedFast->setChecked(true);
		else if (ispeed==2) d->popup_tourSpeedVeryFast->setChecked(true);

		state >> b; d->popup_tourPartsVertex->setChecked(b);
		state >> b; d->popup_tourPartsInDet->setChecked(b);
		state >> b; d->popup_tourPartsCalo->setChecked(b);
		state >> b; d->popup_tourPartsMuon->setChecked(b);

		qint32 iloop;
		state >> iloop;
		if (iloop==2) d->popup_tourLoopTwice->setChecked(true);
		else if (iloop==3) d->popup_tourLoopThrice->setChecked(true);
		else if (iloop==999) d->popup_tourLoopForever->setChecked(true);

	}

	if (version>=2) {
		state >> d->resetCamera_isPerspective;
		state >> d->resetCamera_state;
		if (d->popup_resetCameraAction) {
			bool v(d->resetCamera_state!=QByteArray());
			if (d->popup_resetCameraAction->isVisible()!=v)
				d->popup_resetCameraAction->setVisible(v);
		}
	}
	if (version>=3) {
		d->storedViews.clear();
		QList<QByteArray> persistifiedViews;
		state >> persistifiedViews;
		foreach(QByteArray ba_pv, persistifiedViews) {
			Imp::StoredView sv(ba_pv);
			if (sv.isValid())
				d->storedViews << sv;
		}
		d->storedViewsChanged();
	}

	if (version>=4)
		state >> d->ambientLightPercentage;

	if (version>=5) {
		QByteArray ba;
		state >> ba;
		if (ba!=QByteArray()) {
			if (!d->customtoureditor)
				d->customtoureditorState = ba;
			else
				d->customtoureditor->setState(ba);
		}
	}

	d->updateEnvironmentNode();
	d->updateAmbientLightText();

	// ===> Finish up:
			buffer.close();
}


//____________________________________________________________________
VP1ExaminerViewer::~VP1ExaminerViewer()
{
#if SOQT_MAJOR_VERSION <= 1 && SOQT_MINOR_VERSION <= 4 && SOQT_MICRO_VERSION <= 1
	//Fix bug in SoQt, in which the menu does not get deleted (reported and fixed upstream after SoQt 1.4.1)
	delete prefmenu;
#endif
	delete d;
	SoQtExaminerViewer::setSceneGraph(0);
}

//____________________________________________________________________
void VP1ExaminerViewer::createViewerButtons(QWidget * parent, SbPList * buttonlist)
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::createViewerButtons");

	SoQtExaminerViewer::createViewerButtons(parent,buttonlist);
	//buttonlist holds void pointers to pushbuttons

	if (buttonlist->getLength()==7) {
		//Get pointers:
		d->button_interact = static_cast<QPushButton*>(buttonlist->get(0));
		d->button_examine = static_cast<QPushButton*>(buttonlist->get(1));
		d->button_home = static_cast<QPushButton*>(buttonlist->get(2));
		d->button_sethome = static_cast<QPushButton*>(buttonlist->get(3));
		d->button_viewall = static_cast<QPushButton*>(buttonlist->get(4));
		d->button_seek = static_cast<QPushButton*>(buttonlist->get(5));
		d->button_togglecamera = static_cast<QPushButton*>(buttonlist->get(6));
		//Other stuff? Connections? Change pixmaps?
		d->button_interact->setToolTip("Pick mode (ESC/M toggles). Use this in order to select objects.");
		d->button_examine->setToolTip("Navigation mode (ESC/M toggles). Use to ROTATE (left click), ZOOM (wheel or SHIFT+CTRL+left click) or PAN (middle click or SHIFT+left click or CTRL+left click).");
		d->button_home->setToolTip("Change view to home view");
		d->button_sethome->setToolTip("Store current view as new home");
		d->button_viewall->setToolTip("View all objects currently displayed (V)");
		d->button_seek->setToolTip("Seek to point (S). Sets cameras rotation centre to the clicked point and zooms towards it.");
		d->button_togglecamera->setToolTip("Toggle (C) camera between perspective (P) and orthographic (O) mode.");
		//These are just confusing anyway:
		d->button_home->setVisible(false);
		d->button_sethome->setVisible(false);
	} else {
		VP1Msg::message("VP1ExaminerViewer::createViewerButtons ERROR: Did not get list of exactly 7 buttons from base.");
	}
}

//____________________________________________________________________
QPushButton * VP1ExaminerViewer::Imp::addNewButton(QString text,QString tooltip,REGION region ,VIEW view,QWidget * tempparent, QString iconname)
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::Imp::addNewButton()");

	QPushButton * button = new QPushButton(tempparent);
	if (iconname.isEmpty()) {
		button->setText(text);
	} else {
		button->setIcon( QIcon(iconname) );
	}
	button->setToolTip(tooltip);
	detectorbuttons[button] = std::make_pair(region,view);
	button->setFocusPolicy(Qt::NoFocus);
	//  button->setIconSize(QSize(24, 24));
	//  button->setFixedSize(30, );
	QObject::connect(button,SIGNAL(clicked()),signalcatcher,SLOT(catchSignal()));
	theclass->addAppPushButton(button);
	return button;
}

//____________________________________________________________________
bool VP1ExaminerViewer::Imp::updateAnimationSequence()
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::Imp::updateAnimationSequence()");
	animationSequencer.sequence().clearAllFrames();
	ensureMenuInit();

	//Speed:
	double steptime=3;//seconds (fixme. Also, fix number of loops... Also, calculate total time and display in speed menu...
	if (popup_tourSpeedVerySlow->isChecked())
		steptime *= 3;
	else if (popup_tourSpeedSlow->isChecked())
		steptime *= 1.5;
	else if (popup_tourSpeedFast->isChecked())
		steptime /= 1.5;
	else if (popup_tourSpeedVeryFast->isChecked())
		steptime /= 3;

	//cycles:
	tourLoopsForever = popup_tourLoopForever->isChecked();
	int nloops(1);//define number of loops
	if (popup_tourLoopTwice->isChecked())
		nloops = 2;
	else if (popup_tourLoopThrice->isChecked())
		nloops = 3;

	int nregions(0);
	tourLoopsForeverSkipFirstFrame = false;
	double firstfact(action_movieenabled->isChecked()?0.0:0.25);
	AnimationSequence::REGION firstRegion(AnimationSequence::MUON), latestRegion(AnimationSequence::MUON);
	bool first(true);
	for (int iloop = 0; iloop < nloops; ++iloop) {
		if (popup_tourPartsMuon->isChecked()) {
			AnimationSequence::REGION region=AnimationSequence::MUON;
			if (first||latestRegion!=region)
				animationSequencer.sequence().addFrame(region,  SbVec3f(sin(6*M_PI/6),  0,cos(6*M_PI/6)),  (first?firstfact:1.0)*steptime);
			animationSequencer.sequence().addFrame(region,  SbVec3f(sin(10*M_PI/6),  0,cos(10*M_PI/6)),  steptime);
			animationSequencer.sequence().addFrame(region,  SbVec3f(sin(2*M_PI/6),  0,cos(2*M_PI/6)),  steptime);
			animationSequencer.sequence().addFrame(region,  SbVec3f(sin(6*M_PI/6), 0,cos(6*M_PI/6)),  steptime);
			latestRegion=region;
			if (first)
				firstRegion = region;
			first=false;
			if (iloop==0)
				++nregions;
		}
		if (popup_tourPartsCalo->isChecked()) {
			AnimationSequence::REGION region=AnimationSequence::CALO;
			if (first||latestRegion!=region)
				animationSequencer.sequence().addFrame(region,  SbVec3f(sin(6*M_PI/6),  0,cos(6*M_PI/6)),  (first?firstfact:1.0)*steptime);
			animationSequencer.sequence().addFrame(region,  SbVec3f(sin(10*M_PI/6),  0,cos(10*M_PI/6)),  steptime);
			animationSequencer.sequence().addFrame(region,  SbVec3f(sin(2*M_PI/6),  0,cos(2*M_PI/6)),  steptime);
			animationSequencer.sequence().addFrame(region,  SbVec3f(sin(6*M_PI/6), 0,cos(6*M_PI/6)),  steptime);
			latestRegion=region;
			if (first)
				firstRegion = region;
			first=false;
			if (iloop==0)
				++nregions;
		}
		if (popup_tourPartsInDet->isChecked()) {
			AnimationSequence::REGION region=AnimationSequence::INDET;
			if (first||latestRegion!=region)
				animationSequencer.sequence().addFrame(region,  SbVec3f(sin(6*M_PI/6),  0,cos(6*M_PI/6)),  (first?firstfact:1.0)*steptime);
			animationSequencer.sequence().addFrame(region,  SbVec3f(sin(10*M_PI/6),  0,cos(10*M_PI/6)),  steptime);
			animationSequencer.sequence().addFrame(region,  SbVec3f(sin(2*M_PI/6),  0,cos(2*M_PI/6)),  steptime);
			animationSequencer.sequence().addFrame(region,  SbVec3f(sin(6*M_PI/6), 0,cos(6*M_PI/6)),  steptime);
			latestRegion=region;
			if (first)
				firstRegion = region;
			first=false;
			if (iloop==0)
				++nregions;
		}
		if (popup_tourPartsVertex->isChecked()) {
			AnimationSequence::REGION region=AnimationSequence::VERTEX;
			if (first||latestRegion!=region)
				animationSequencer.sequence().addFrame(region,  SbVec3f(sin(6*M_PI/6),  0,cos(6*M_PI/6)),  (first?firstfact:1.0)*steptime);
			animationSequencer.sequence().addFrame(region,  SbVec3f(sin(10*M_PI/6),  0,cos(10*M_PI/6)),  steptime);
			animationSequencer.sequence().addFrame(region,  SbVec3f(sin(2*M_PI/6),  0,cos(2*M_PI/6)),  steptime);
			animationSequencer.sequence().addFrame(region,  SbVec3f(sin(6*M_PI/6), 0,cos(6*M_PI/6)),  steptime);
			latestRegion=region;
			if (first)
				firstRegion = region;
			first=false;
			if (iloop==0)
				++nregions;
		}
		if (first)//Apparently nothing was enabled
				return false;
	}
	if (tourLoopsForever && nregions==1)
		tourLoopsForeverSkipFirstFrame = true;

	if (!tourLoopsForever&&latestRegion!=firstRegion)
		animationSequencer.sequence().addFrame(firstRegion,  SbVec3f(sin(6*M_PI/6),  0,cos(6*M_PI/6)),  steptime);
	if (firstfact>0.0&&!tourLoopsForever)
		animationSequencer.sequence().addFrame(theclass->currentCameraState(),firstfact*steptime);

	return true;
}


//____________________________________________________________________
void VP1ExaminerViewer::Imp::init()
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::Imp::init");
	QWidget * tempparent = new QWidget();//Use this as temporary parent to
	//avoid the buttons temporarily
	//showing up as individual top
	//widgets.
	tempparent->setVisible(false);
	if (detectorViewButtons) {
		// addNewButton("VA","View vertex region from positive Z axis",VERTEX,ASIDE,tempparent);
		// addNewButton("VC","View vertex region from negative Z axis",VERTEX,CSIDE,tempparent);

		addNewButton("IA","View inner detector from positive Z axis",INDET,ASIDE,tempparent);
		addNewButton("IC","View inner detector from negative Z axis",INDET,CSIDE,tempparent);

		addNewButton("CA","View calorimeters from positive Z axis",CALO,ASIDE,tempparent);
		addNewButton("CC","View calorimeters from negative Z axis",CALO,CSIDE,tempparent);

		addNewButton("MA","View muon spectrometer from positive Z axis",MUON,ASIDE,tempparent);
		addNewButton("MC","View muon spectrometer from negative Z axis",MUON,CSIDE,tempparent);

		addNewButton("FA","View forward region from positive Z axis",FORWARDREGION,ASIDE,tempparent);
		addNewButton("FC","View forward region from negative Z axis",FORWARDREGION,CSIDE,tempparent);

		addNewButton("MB","View muon spectrometer from barrel",MUON,BARREL,tempparent, ":icons/icons/rphi.png");

		// This one is special:
		//addNewButton("XT", "Execute a tour of the detector",INDET, true, tempparent);
		{
			QPushButton *button = new QPushButton(tempparent);
			button->setText("XT");
			button->setToolTip("Execute a tour of the detector (configure in context menu)");
			button->setFocusPolicy(Qt::NoFocus);
			takeTourButton=button;
			QObject::connect(button,SIGNAL(clicked()), signalcatcher, SLOT(catchSignal()));
			theclass->addAppPushButton(button);
			QObject::connect(&animationSequencer,SIGNAL(animationFinishedSuccessfully()),signalcatcher,SLOT(catchSignal()));      
		}
	}
	QWidget * w_spacer = new QWidget(tempparent);
	QSizePolicy sp_spacer(w_spacer->sizePolicy());
	unsigned vertStretch = 99999;
	sp_spacer.setVerticalStretch(vertStretch);
	w_spacer->setSizePolicy(sp_spacer);
	theclass->addAppPushButton(w_spacer);

	tempparent->deleteLater();
}

//____________________________________________________________________
void VP1ExaminerViewer_SignalCatcher::catchSignal()
{
	if (!sender())
		return;
	if (sender()==&(d->animationSequencer)) {
		if (d->tourLoopsForever)
			d->animationSequencer.startAnimating(d->tourLoopsForeverSkipFirstFrame);
		return;
	}
	QPushButton* button = dynamic_cast<QPushButton*>(sender());
	if (button) {
		if (d->detectorbuttons.find(button)!=d->detectorbuttons.end())
			d->detectorZoomButtonClicked(d->detectorbuttons.find(button)->second);
		else if (button==d->takeTourButton)
			d->takeTourButtonClicked();
		return;
	}
	QMenu* menu = dynamic_cast<QMenu*>(sender());
	if (menu) {
		d->aboutToShowMenu(menu);
		return;
	}
	VP1Msg::messageDebug("VP1ExaminerViewer_SignalCatcher::catchSignal ERROR: Received unknown signal");
}

//____________________________________________________________________
void VP1ExaminerViewer::startTour()
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::startTour()");
	if (d->detectorViewButtons)
		d->takeTourButtonClicked();
}

//____________________________________________________________________
bool VP1ExaminerViewer::startTourEachEvent() const
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::startTourEachEvent()");
	return d->popup_tourStartEachEvent ? d->popup_tourStartEachEvent->isChecked() : false;
}

//____________________________________________________________________
SoSphere * VP1ExaminerViewer::Imp::getRegionSphere(REGION region,bool perspective)
{
  if (!sphere) {
    sphere = new SoSphere;
    sphere->ref();
  }
  double r(30);
  switch (region) {
  case VERTEX:
    VP1Msg::messageVerbose("set sphere dimensions for vertex");
    //r = perspective ? 0.5 : 0.5;
    r=0.5;
    break;
  case INDET:
    VP1Msg::messageVerbose("set sphere dimensions for indet");
    r = perspective ? 13 : 7;
    break;
  case CALO:
    VP1Msg::messageVerbose("set sphere dimensions for calo");
    r = perspective ? 35 : 27;
    break;
  case FORWARDREGION:
    VP1Msg::messageVerbose("set sphere dimensions for forward region");
    //r = perspective ? 600 : 600;
    r=600;
    break;
  case MUON:
  default:
    VP1Msg::messageVerbose("set sphere dimensions for muon");
    r = perspective ? 95 : 73;
    break;
  }
  sphere->radius  = r * 1000.0;
  return sphere;
}

//____________________________________________________________________
void VP1ExaminerViewer::Imp::grabFocus()
{
//	VP1Msg::messageVerbose("VP1ExaminerViewer::Imp::grabFocus()");
	QWidget * w = theclass->getGLWidget();
	if (w)
		w->setFocus(Qt::OtherFocusReason);
}

//____________________________________________________________________
void VP1ExaminerViewer::Imp::applyMovieSettingsToAnimationSequencer()
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::Imp::applyMovieSettingsToAnimationSequencer()");

	//Movie pars:
	if (action_movieenabled->isChecked()) {
		int width = action_moviewidth->data().toInt();
		int height = action_movieheight->data().toInt();
		int fps = action_moviefps->data().toInt();
		QString outdir = action_movieoutdir->data().toString();
		QDir().mkpath(outdir);
		animationSequencer.setMovie(true);
		animationSequencer.setMovieParameters(outdir,movieFrameFileNamePrefix,fps,width,height);
	} else {
		animationSequencer.setMovie(false);
	}
}

//____________________________________________________________________
void VP1ExaminerViewer::startCustomTour()
{
	VP1Msg::messageVerbose("startCustomTour begin.");

	if (!d->customtoureditor||!d->customtoureditor->tourAvailable())
		return;

	d->grabFocus();
	if (isAnimating())
		stopAnimating();
	bool jump(d->action_movieenabled->isChecked());
	if (currentCamIsPerspective()!=d->customtoureditor->tourIsPerspective()) {
		toggleCameraType();
		jump = true;
	}
	d->animationSequencer.sequence().clearAllFrames();
	d->customtoureditor->addTourToAnimationSequencer(d->animationSequencer,jump);
	d->applyMovieSettingsToAnimationSequencer();
	//Start animation:
	d->animationSequencer.startAnimating();
}

//____________________________________________________________________
void VP1ExaminerViewer::Imp::takeTourButtonClicked() {
	VP1Msg::messageVerbose("takeTourButtonClicked begin.");
	grabFocus();
	if (theclass->isAnimating())
		theclass->stopAnimating();
	if (updateAnimationSequence()) {
		applyMovieSettingsToAnimationSequencer();
		//animationSequencer
		animationSequencer.startAnimating();
	} else {
		VP1Msg::messageDebug("Aborting tour since no detector parts enabled.");
	}
	VP1Msg::messageVerbose("takeTourButtonClicked end.");
}

//____________________________________________________________________
void VP1ExaminerViewer::Imp::detectorZoomButtonClicked(std::pair<REGION,VIEW> p)
{
	grabFocus();

	VP1Msg::messageVerbose("detectorZoomButtonClicked region = "+toString(p.first)+", from "+QString(p.second +" Z axis") );

	SoNode * rootnode = theclass->getSceneGraph();
	if (!rootnode) {
		VP1Msg::messageDebug("VP1ExaminerViewer::Imp::detectorZoomButtonClicked WARNING: No scenegraph set. Ignoring.");
		return;
	}
	if ( ! ( rootnode->getTypeId().isDerivedFrom(SoGroup::getClassTypeId())) ) {
		VP1Msg::messageDebug("VP1ExaminerViewer::Imp::detectorZoomButtonClicked WARNING: Root node does not derive from SoGroup. Ignoring.");
		return;
	}
	if (theclass->currentCamIsPerspective()) {
		//Fix for bad camera (fixme: this doesn't really work... Perhaps let all detector buttons reset camera?):
		theclass->toggleCameraType();
		theclass->toggleCameraType();
	}
	SoCamera * camera = theclass->getCamera();
	if (!camera) {
		VP1Msg::messageDebug("VP1ExaminerViewer::Imp::detectorZoomButtonClicked WARNING: Could not get camera. Ignoring.");
		return;
	}
	rootnode->ref();
	camera->ref();

	SoGroup * root = static_cast<SoGroup*>(rootnode);

	//Get region:
	SoSphere * regionsphere = getRegionSphere(p.first,
			theclass->currentCamIsPerspective());
	//Get direction:
	int zDir=0;
	int xDir=0;
	switch (p.second){
	case ASIDE:
	{
		zDir=-1;
		rotationMode=ZROT;
		theclass->setLeftWheelString("Rotz");
		break;
	}
	case CSIDE:
	{
		zDir=1;
		rotationMode=ZROT;
		theclass->setLeftWheelString("Rotz");
		break;
	}
	case BARREL:
	{
		xDir=1;
		rotationMode=XROT;
		theclass->setLeftWheelString("Rotx");
	}
	}
	SbVec3f lookat(xDir,0,zDir), upvec(0,1,0);

	bool notifyenabled = root->enableNotify(false);

	root->insertChild(regionsphere,0);
	VP1Msg::messageVerbose("detectorZoomButtonClicked Initiating zoom.");
	if (theclass->isAnimating())
		theclass->stopAnimating();
	camera->unrefNoDelete();//Must be done before we start the zoom (to avoid an abort)
	//   if (resetCamera_state!=QByteArray())
	//     VP1CameraHelper::animatedZoomToCameraState( camera,root,resetCamera_state,1.0, true );
	//   else
	VP1CameraHelper::animatedZoomToSubTree(camera,root,regionsphere,1.0,100.0,100.0,0.1,lookat,upvec);

	root->removeChild(regionsphere);
	if (notifyenabled) {
		root->enableNotify(true);
		root->touch();
	}

	rootnode->unrefNoDelete();

	VP1Msg::messageVerbose("detectorZoomButtonClicked end.");
}



//____________________________________________________________________
void VP1ExaminerViewer::buildPopupMenu()
{
	//We don't actually use this popupmenu.

	// SoQtExaminerViewer::buildPopupMenu();
}

//____________________________________________________________________
void VP1ExaminerViewer::removeDecorationMenuOption()
{

	VP1Msg::messageVerbose("VP1ExaminerViewer::removeDecorationMenuOption()");
	if (d->decorationMenuRemoved || !prefmenu)
		return;
	d->decorationMenuRemoved = true;
	int id = prefmenu->getMenuItem("decoration");
	if (id!=-1)
		prefmenu->removeMenuItem(id);

}

//____________________________________________________________________
SbBool VP1ExaminerViewer::processSoEvent(const SoEvent * const evt )
{
	if (evt->getTypeId().isDerivedFrom(SoKeyboardEvent::getClassTypeId())) {
		//We want to add a few shortcuts:
		// "A": View all
		// "P": Perspective camera
		// "O": Orthogonal camera
		// "C": Toggle camera mode
		// "M": Toggle view/selection mode.
		//  If "Q" then we do NOT pass it on to the base class (because that closes the top window!!)

		d->grabFocus(); //probably redundant since we got the event, but can't hurt.

		if (SO_KEY_PRESS_EVENT(evt,SoKeyboardEvent::V)) {
			viewAll();
			return true;//eat event
		}
		if (SO_KEY_PRESS_EVENT(evt,SoKeyboardEvent::Q))
			return false;//do not eat event, but do not pass through to base.
		if (SO_KEY_PRESS_EVENT(evt,SoKeyboardEvent::C)) {
			toggleCameraType();
			return true;//eat event
		}
		if (SO_KEY_PRESS_EVENT(evt,SoKeyboardEvent::P)) {
			if (!currentCamIsPerspective() )
				toggleCameraType();
			return true;//eat event
		}
		if (SO_KEY_PRESS_EVENT(evt,SoKeyboardEvent::O)) {
			if (currentCamIsPerspective())
				toggleCameraType();
			return true;//eat event
		}
		if (SO_KEY_PRESS_EVENT(evt,SoKeyboardEvent::M)) {
			setViewing(!isViewing());
			return true;//eat event
		}
		if (SO_KEY_PRESS_EVENT(evt,SoKeyboardEvent::A)) {
			setAntiAlias(!isAntiAlias());
			d->grabFocus();//Needed since the GL calls triggered when setting antialiasing makes us loose focus (we obviusly just had it).
			return true;//eat event
		}

		//NB: We could emit keypressed signal when keyboardevent and base
		//    class does not eat event.

	} else if (evt->getTypeId().isDerivedFrom(SoMouseButtonEvent::getClassTypeId())) {
		d->grabFocus();//Any click grabs focus:
		if (isViewing()) {//In viewing mode we open a popup menu on right clicks:
			const SoMouseButtonEvent * ev_mouse = static_cast<const SoMouseButtonEvent*>(evt);
			if ((ev_mouse->getButton() == SoMouseButtonEvent::BUTTON2/*right click*/)) {
				if (ev_mouse->getState() == SoButtonEvent::DOWN) {
					showPopupMenu();
				}
				return true;//eat all right-clicks in viewing mode.
			}
		}
	}

	return SoQtExaminerViewer::processSoEvent(evt);//pass event through
}

//____________________________________________________________________
void VP1ExaminerViewer::viewAll()
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::viewAll()");
	d->grabFocus();
	if (currentCamIsPerspective()) {
		//Fix for bad camera:
		toggleCameraType();
		toggleCameraType();
		//Fixme: Instead make a reset camera followed by a 0 second "zoom" to correct orientation?
	}
	SoQtExaminerViewer::viewAll();

}

//____________________________________________________________________
void VP1ExaminerViewer::toggleCameraType()
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::toggleCameraType()");
	d->grabFocus();
	SoQtExaminerViewer::toggleCameraType();
}

//____________________________________________________________________
void VP1ExaminerViewer::setCameraType(SoType type)
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::setCameraType()");
	d->grabFocus();
	if (rightWheelLabel)
		rightWheelLabel->setUpdatesEnabled(false);
	SoQtExaminerViewer::setCameraType(type);
	setRightWheelString("Zoom");
	if (rightWheelLabel) {
		rightWheelLabel->setVisible(true);
		rightWheelLabel->setUpdatesEnabled(true);
	}

	// get default stereo values and set them as starting point for the stereo editor
	if (getCamera()) {
		stereo_offset_camera = getCamera()->getStereoAdjustment();
		stereo_parallax_camera = getCamera()->getBalanceAdjustment();
		stereo_offset_viewer = getStereoOffset();
	}
}

//____________________________________________________________________
void VP1ExaminerViewer::setSeekMode(SbBool enable)
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::setSeekMode()");
	d->grabFocus();
	SoQtExaminerViewer::setSeekMode(enable);
}

//____________________________________________________________________
void VP1ExaminerViewer::setViewing(SbBool enable)
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::setViewing()");
	d->grabFocus();
	SoQtExaminerViewer::setViewing(enable);
}

//____________________________________________________________________
void VP1ExaminerViewer::bottomWheelFinish()
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::bottomWheelFinish()");
	d->grabFocus();
	SoQtExaminerViewer::bottomWheelFinish();
}

//____________________________________________________________________
void VP1ExaminerViewer::bottomWheelMotion(float val)
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::bottomWheelMotion()");
	d->grabFocus();
	SoQtExaminerViewer::bottomWheelMotion(val);
}

//____________________________________________________________________
void VP1ExaminerViewer::bottomWheelStart()
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::bottomWheelStart()");
	d->grabFocus();
	SoQtExaminerViewer::bottomWheelStart();
}

//____________________________________________________________________
void VP1ExaminerViewer::leftWheelFinish()
{
	d->grabFocus();
	SoQtExaminerViewer::leftWheelFinish();
}

//____________________________________________________________________
void VP1ExaminerViewer::leftWheelMotion(float val)
{
	d->grabFocus();

	if (isAnimating())
		stopAnimating();

	float newval = 0.0;
	if (d->rotationMode==Imp::XROT)
		newval = d->rotXWheelMotion(val, getLeftWheelValue());
	else
		newval = d->rotZWheelMotion(val, getLeftWheelValue());

	SoQtFullViewer::leftWheelMotion(newval);//NB: We bypass SoQtExaminerViewer implementation
}

//____________________________________________________________________
void VP1ExaminerViewer::leftWheelStart()
{
	d->grabFocus();
	SoQtExaminerViewer::leftWheelStart();
}

//____________________________________________________________________
void VP1ExaminerViewer::rightWheelFinish()
{
	d->grabFocus();
	SoQtExaminerViewer::rightWheelFinish();
}

//____________________________________________________________________
void VP1ExaminerViewer::rightWheelMotion(float val)
{
	d->grabFocus();
	SoQtExaminerViewer::rightWheelMotion(val);
}

//____________________________________________________________________
void VP1ExaminerViewer::rightWheelStart()
{
	d->grabFocus();
	SoQtExaminerViewer::rightWheelStart();
}

//____________________________________________________________________
float VP1ExaminerViewer::Imp::rotZWheelMotion(float value, float oldvalue)
{
	SoCamera * cam = theclass->getCamera();
	if (!cam)
		return 0.0f; // can happen for empty scenegraph

	cam->ref();
	rotateCamera(cam, SbVec3f(0, 0, -1), value - oldvalue);
	cam->unrefNoDelete();
	return value;
}

//____________________________________________________________________
float VP1ExaminerViewer::Imp::rotXWheelMotion(float value, float oldvalue)
{
	SoCamera * cam = theclass->getCamera();
	if (!cam)
		return 0.0f; // can happen for empty scenegraph

	cam->ref();
	rotateCamera(cam, SbVec3f(-1, 0, 0), value - oldvalue);
	cam->unrefNoDelete();
	return value;
}

//____________________________________________________________________
void VP1ExaminerViewer::Imp::rotateCamera(SoCamera * cam,
		const SbVec3f & aroundaxis,
		const float delta)
{
	const SbVec3f DEFAULTDIRECTION(0, 0, -1);
	const SbRotation currentorientation = cam->orientation.getValue();

	SbVec3f currentdir;
	currentorientation.multVec(DEFAULTDIRECTION, currentdir);

	const SbVec3f focalpoint = cam->position.getValue() +
			cam->focalDistance.getValue() * currentdir;

	// set new orientation
	cam->orientation = SbRotation(aroundaxis, delta) * currentorientation;

	SbVec3f newdir;
	cam->orientation.getValue().multVec(DEFAULTDIRECTION, newdir);
	cam->position = focalpoint - cam->focalDistance.getValue() * newdir;
}

//____________________________________________________________________
void VP1ExaminerViewer::setBufferingType(SoQtViewer::BufferType bt)
{
	SoQtExaminerViewer::setBufferingType(bt);
}

//____________________________________________________________________
void VP1ExaminerViewer::setAntialiasing(SbBool smoothing, int numPasses)
{
	SoQtExaminerViewer::setAntialiasing(smoothing, numPasses); // Needed for offscreen rendering (i.e. snapshots).
	if (smoothing)
		VP1Msg::message("VP1ExaminerViewer: turning AA on.");
	else
		VP1Msg::message("VP1ExaminerViewer: turning AA off.");
	VP1Msg::message(" AA is now done using a new technique so please let hn-atlas-vp1-help@cern.ch know of any problems. Mac users: if the display goes blank, please try resizing the main VP1 window.");
	//FIXME - remove above messages at some point? EJWM.

	QGLWidget* qglw = (QGLWidget*)getGLWidget();
	QGLFormat fmt = qglw->format();
	fmt.setSampleBuffers(smoothing);
	fmt.setSamples(numPasses);
	qglw->setFormat(fmt);    // note: this is supposedly deprecated..
	qglw->makeCurrent();
	if(smoothing && numPasses > 1)
		glEnable(GL_MULTISAMPLE);
	else
		glDisable(GL_MULTISAMPLE);
	d->isantialias=smoothing;
}

//____________________________________________________________________
void VP1ExaminerViewer::setTransparencyType(SoGLRenderAction::TransparencyType t)
{
	SoQtExaminerViewer::setTransparencyType(t);
}

//____________________________________________________________________
void VP1ExaminerViewer::setDrawStyle(SoQtViewer::DrawType t, SoQtViewer::DrawStyle s)
{
	SoQtExaminerViewer::setDrawStyle(t,s);
}

//____________________________________________________________________
bool VP1ExaminerViewer::isAntiAlias() const
{
	return d->isantialias;
	//   SbBool smoothing; int numPasses;
	//   getAntialiasing(smoothing, numPasses);
	//   return smoothing&&numPasses>1;
}

//____________________________________________________________________
void VP1ExaminerViewer::setAntiAlias(bool b)
{
	d->isantialias=b;
	b ? setAntialiasing(true,4) : setAntialiasing(false,1);
}

//____________________________________________________________________
void VP1ExaminerViewer::resetCamera()
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::resetCamera()");
	if (d->resetCamera_state==QByteArray())
		return;
	if (isAnimating())
		stopAnimating();
	if (d->resetCamera_isPerspective != (currentCamIsPerspective()))
		toggleCameraType();
	if (getCamera()&&d->resetCamera_state!=QByteArray())
		VP1QtInventorUtils::deserializeSoCameraParameters(d->resetCamera_state,*(getCamera()));//Fixme: Check return value?
}


//____________________________________________________________________
bool VP1ExaminerViewer::currentCamIsPerspective() const
{
	return getCameraType() == SoPerspectiveCamera::getClassTypeId();
}

//____________________________________________________________________
QByteArray VP1ExaminerViewer::currentCameraState() const
{
	SoCamera * cam(getCamera());
	return cam ? VP1QtInventorUtils::serializeSoCameraParameters(*cam) : QByteArray();
}

//____________________________________________________________________
QPixmap VP1ExaminerViewer::getSnapShotFromCamState(bool camStateIsPerspective, QByteArray camState,
		int width, int height, bool transp )
{
	SoCamera * cam = getCamera();
	if (!cam)
		return QPixmap();
	bool notifyenabled = cam->enableNotify(false);

	bool currentPersp(currentCamIsPerspective());
	QByteArray currentState(currentCameraState());
	if (currentPersp!=camStateIsPerspective)
		toggleCameraType();
	VP1QtInventorUtils::deserializeSoCameraParameters(camState,*(getCamera()));
	QPixmap pm = VP1QtInventorUtils::renderToPixmap(this, width, height,transp);
	if (currentPersp!=camStateIsPerspective)
		toggleCameraType();
	VP1QtInventorUtils::deserializeSoCameraParameters(currentState,*(getCamera()));

	if (notifyenabled) {
		cam->enableNotify(true);
		//We don't touch since we know that we didn't actually change anything: cam->touch();
	}
	return pm;
}

//____________________________________________________________________
void VP1ExaminerViewer::storeCameraParametersForReset()
{
	if (d->resetCamera_state!=QByteArray())
		return;
	//Camera type and parameters:
	d->resetCamera_isPerspective = (currentCamIsPerspective());
	d->resetCamera_state = currentCameraState();

	if (d->popup_resetCameraAction&&!d->popup_resetCameraAction->isVisible())
		d->popup_resetCameraAction->setVisible(true);
}

//____________________________________________________________________
void VP1ExaminerViewer::Imp::updateMovieMenuVisuals()
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::Imp::updateMovieMenuVisuals()");

	ensureMenuInit();
	int width = action_moviewidth->data().toInt();
	int height = action_movieheight->data().toInt();
	int fps = action_moviefps->data().toInt();
	QString outdir = action_movieoutdir->data().toString();
	action_moviewidth->setText("Change output &width [current: "+QString::number(width)+"]");
	action_movieheight->setText("Change output &height [current: "+QString::number(height)+"]");
	action_moviefps->setText("Change output &FPS [current: "+QString::number(fps)+"]");
	action_movieoutdir->setText("Change output &dir [current: "+outdir+"]");

	bool c(action_movieenabled->isChecked());
	action_moviewidth->setEnabled(c);
	action_movieheight->setEnabled(c);
	action_moviefps->setEnabled(c);
	action_movieoutdir->setEnabled(c);

}

//____________________________________________________________________
bool VP1ExaminerViewer::Imp::ensureMenuInit()
{
	if (popup_menu)
		return true;
	QWidget * w = theclass->getWidget();
	if (!w)
		return false;
	popup_menu = new QMenu (w);

	popup_bgdColAction = popup_menu->addAction("&Background Colour");
	popup_antiAliasAction = popup_menu->addAction("&Anti aliasing [A]");
	popup_resetCameraAction = popup_menu->addAction("&Reset camera");

	if (detectorViewButtons) {
		//Tour - base
		QMenu * tourmenu = popup_menu->addMenu("&Tour");
		popup_tourStartEachEvent = tourmenu->addAction("Start tour on new event");
		popup_tourStartEachEvent->setCheckable(true);
		popup_tourReturnToStartAction = tourmenu->addAction("Tour ends at &original view");
		popup_tourReturnToStartAction->setCheckable(true);
		popup_tourReturnToStartAction->setVisible(false);//Until functionality actually implemented
		QMenu * tourspeedmenu = tourmenu->addMenu("&Speed");
		QMenu * tourpartsmenu = tourmenu->addMenu("&Parts");
		QMenu * tourloopmenu = tourmenu->addMenu("&Looping");
		popup_tourExecute = tourmenu->addAction("E&xecute");

		//Tour - speed:
		popup_tourSpeedVerySlow = tourspeedmenu->addAction("Very s&low");
		popup_tourSpeedSlow = tourspeedmenu->addAction("&Slow");
		popup_tourSpeedMedium = tourspeedmenu->addAction("&Medium");
		popup_tourSpeedFast = tourspeedmenu->addAction("&Fast");
		popup_tourSpeedVeryFast = tourspeedmenu->addAction("Very f&ast");
		popup_tourSpeedVerySlow->setCheckable(true);
		popup_tourSpeedSlow->setCheckable(true);
		popup_tourSpeedMedium->setCheckable(true);
		popup_tourSpeedFast->setCheckable(true);
		popup_tourSpeedVeryFast->setCheckable(true);
		QActionGroup * tour_speed_group = new QActionGroup(tourspeedmenu);
		tour_speed_group->addAction(popup_tourSpeedVerySlow);
		tour_speed_group->addAction(popup_tourSpeedSlow);
		tour_speed_group->addAction(popup_tourSpeedMedium);
		tour_speed_group->addAction(popup_tourSpeedFast);
		tour_speed_group->addAction(popup_tourSpeedVeryFast);
		popup_tourSpeedMedium->setChecked(true);

		//Tour - parts:
		popup_tourPartsVertex = tourpartsmenu->addAction("&Vertex region");
		popup_tourPartsInDet = tourpartsmenu->addAction("&Inner Detector");
		popup_tourPartsCalo = tourpartsmenu->addAction("&Calorimeters");
		popup_tourPartsMuon = tourpartsmenu->addAction("&Muon systems");
		popup_tourPartsVertex->setCheckable(true);
		popup_tourPartsInDet->setCheckable(true);
		popup_tourPartsCalo->setCheckable(true);
		popup_tourPartsMuon->setCheckable(true);
		popup_tourPartsInDet->setChecked(true);
		popup_tourPartsMuon->setChecked(true);

		//Tour - Looping:
		popup_tourLoopOnce = tourloopmenu->addAction("&Once");
		popup_tourLoopTwice = tourloopmenu->addAction("&Twice");
		popup_tourLoopThrice = tourloopmenu->addAction("T&hrice");
		popup_tourLoopForever = tourloopmenu->addAction("&Indefinitely");
		popup_tourLoopOnce->setCheckable(true);
		popup_tourLoopTwice->setCheckable(true);
		popup_tourLoopThrice->setCheckable(true);
		popup_tourLoopForever->setCheckable(true);
		QActionGroup * tour_loop_group = new QActionGroup(tourloopmenu);
		tour_loop_group->addAction(popup_tourLoopOnce);
		tour_loop_group->addAction(popup_tourLoopTwice);
		tour_loop_group->addAction(popup_tourLoopThrice);
		tour_loop_group->addAction(popup_tourLoopForever);
		popup_tourLoopOnce->setChecked(true);

	}

	QMenu * advancedmenu = popup_menu->addMenu("&More");

	popup_hidedecorationsaction = advancedmenu->addAction("Hide &controls");
	popup_headLightAction = advancedmenu->addAction("&Headlight");
	popup_ambientLightAction = advancedmenu->addAction("dummy");
	popup_dumpSceneAction = advancedmenu->addAction("Dump &scene to file");
	popup_dumpSceneVRMLAction = advancedmenu->addAction("Dump &scene to VRML file");
	popup_toSVGAction = advancedmenu->addAction("Produce SV&G image");
	popup_toEPSAction = advancedmenu->addAction("Produce &EPS image");

	if (detectorViewButtons) {
		//Custom views:
		QMenu * viewsmenu = advancedmenu->addMenu("Stored &views");
		viewmenu_zoomtoview = viewsmenu->addMenu("Animated &zoom to view");
		viewmenu_restoreview = viewsmenu->addMenu("&Restore view");
		viewmenu_storecurrentview = viewsmenu->addMenu("Store &current view");
		viewmenu_deleteview = viewsmenu->addMenu("&Delete view");
		QObject::connect(viewmenu_storecurrentview,SIGNAL(aboutToShow()),signalcatcher,SLOT(catchSignal()));
		QObject::connect(viewmenu_deleteview,SIGNAL(aboutToShow()),signalcatcher,SLOT(catchSignal()));
		QObject::connect(viewmenu_restoreview,SIGNAL(aboutToShow()),signalcatcher,SLOT(catchSignal()));
		QObject::connect(viewmenu_zoomtoview,SIGNAL(aboutToShow()),signalcatcher,SLOT(catchSignal()));
		storedViewsChanged();
	}

	if (detectorViewButtons) {
		//Custom tour:
		QMenu * customtourmenu = advancedmenu->addMenu("Custom &tour");
		customtour_launcheditor = customtourmenu->addAction("Launch &editor");
		customtour_execute = customtourmenu->addAction("E&xecute");
		customtour_execute->setEnabled(false);

		//Movies:
		QMenu * moviesmenu = advancedmenu->addMenu("&Movie settings");
		action_movieenabled = moviesmenu->addAction("&Enable movie output when executing tours (one image file per frame)");
		action_movieenabled->setCheckable(true);
		action_movieenabled->setChecked(false);
		action_moviewidth = moviesmenu->addAction("dummy");
		action_moviewidth->setData(1024);
		action_movieheight = moviesmenu->addAction("dummy");
		action_movieheight->setData(768);
		action_moviefps = moviesmenu->addAction("dummy");
		action_moviefps->setData(24);
		action_movieoutdir = moviesmenu->addAction("dummy");
		QString username=VP1QtUtils::environmentVariableValue("USER");
		action_movieoutdir->setData(username.isEmpty() ? "/tmp/vp1frames" : "/tmp/"+username+"/vp1frames");
		action_moviefadetocurrentview = moviesmenu->addAction("Fade last frame to current view (1s)");
		updateMovieMenuVisuals();
	}


	updateAmbientLightText();
	popup_headLightAction->setCheckable(true);
	popup_antiAliasAction->setCheckable(true);
	popup_hidedecorationsaction->setCheckable(true);


	//Draw styles sub menu:
	QMenu * drawstylemenu = advancedmenu->addMenu("&Draw styles");

	QMenu * drawstyle_still_menu = drawstylemenu->addMenu("&Still draw style");
	QMenu * drawstyle_interactive_menu = drawstylemenu->addMenu("&Interactive draw style");
	QMenu * drawstyle_transptype_menu = drawstylemenu->addMenu("&Transparency Type");

	//Drawstyles sub menus:
	QActionGroup * ds_still_group = new QActionGroup(drawstyle_still_menu);
	QActionGroup * ds_interactive_group = new QActionGroup(drawstyle_interactive_menu);
	foreach(SoQtViewer::DrawStyle ds, getAllViewerDrawStyles()) {
		int ids = viewerDrawStyleToInt(ds);
		QString pretty = viewerDrawStyle2PrettyString(ds);
		if (ds!=SoQtViewer::VIEW_SAME_AS_STILL) {
			//still menu:
			QAction * act = drawstyle_still_menu->addAction(pretty);
			act->setData(ids);
			popup_drawstyle_still_actions << act;
			ds_still_group->addAction(act);
			act->setCheckable(true);
		}
		if (ds!=SoQtViewer::VIEW_AS_IS) {
			//interactive menu:
			QAction * act = drawstyle_interactive_menu->addAction(pretty);
			act->setData(ids);
			popup_drawstyle_interactive_actions << act;
			ds_interactive_group->addAction(act);
			act->setCheckable(true);
		}
	}

	//Transparency type sub menu:
	QActionGroup * transptype_group = new QActionGroup(drawstyle_transptype_menu);
	foreach(SoGLRenderAction::TransparencyType type, VP1QtInventorUtils::getAllTransparencyTypes()) {
		QAction * act = drawstyle_transptype_menu->addAction(VP1QtInventorUtils::transparencyType2PrettyString(type));
		act->setData(VP1QtInventorUtils::transparencyTypeToInt(type));
		popup_transptype_actions << act;
		transptype_group->addAction(act);
		act->setCheckable(true);
	}


	//Stereo view sub menu:
	QMenu * stereoviewmenu_main = advancedmenu->addMenu("&Stereographic view");

	stereo_launcheditor = stereoviewmenu_main->addAction("Launch &editor");

	// focal length submenu
	popup_focal_value_action = advancedmenu->addAction("dummy"); // dummy text replaced here below
	// an action to change the let the user change the stereo offset by entering a double
	SoCamera *camera = theclass->getCamera();
	float default_focal_camera = camera->focalDistance.getValue();
	popup_focal_value_action->setData(default_focal_camera);
	popup_focal_value_action->setText("Change camera FOCAL LENGTH [current: "+QString::number(default_focal_camera)+"]");

	// N.B.!! --> all the actions and menus here below are not needed anymore,
	//            because we implemented the new stereo editor

//	QMenu * stereoviewmenu = stereoviewmenu_main->addMenu("stereo menu - viewer");
//	QMenu * stereoviewmenu_camera = stereoviewmenu_main->addMenu("stereo menu - camera");


//	// STEREO settings - SoQtViewer version
//
//	QMenu * stereo_set_offset_menu = stereoviewmenu->addMenu("Set stereo &Offset");
//	popup_stereo_offset_value_action = stereoviewmenu->addAction("dummy"); // dummy text replaced here below
//	QMenu * stereo_set_type_menu = stereoviewmenu->addMenu("Set &Stereo type");
//	stereo_set_anaglyph_menu = stereoviewmenu->addMenu("Set &Anaglyph settings");


//	// build the Stereo Offset submenu
//	QActionGroup * stereo_set_offset_group = new QActionGroup(stereo_set_offset_menu);
//
//	QList<float> offset_values;
//	offset_values << 0 << 1000.00 << 5000.00 << 10000.00 << 20000.00;
//
//	foreach(float value, offset_values) {
//		QAction * act = stereo_set_offset_menu->addAction(QString::number(value));
//		act->setData(QString::number(value));
//		popup_stereo_offset_actions << act;
//		stereo_set_offset_group->addAction(act);
//		act->setCheckable(true);
//	}
//
//
//
//	// an action to change the let the user change the stereo offset by entering a double
//	float default_offset = 0.1;
//	popup_stereo_offset_value_action->setData(default_offset);
//	popup_stereo_offset_value_action->setText("Change STEREO offset [current: "+QString::number(default_offset)+"]");
//
//
//	// build the Stereo Type submenu
//	QActionGroup * stereo_set_type_group = new QActionGroup(stereoviewmenu);
//
//	QList<SoQtViewer::StereoType> stereo_type_values;
//	stereo_type_values << SoQtViewer::STEREO_NONE << SoQtViewer::STEREO_ANAGLYPH;
//
//	foreach(SoQtViewer::StereoType value, getAllStereoViewTypes() ) {
//		QAction * act = stereo_set_type_menu->addAction(viewerStereoType2PrettyString(value));
//		act->setData( viewerStereoTypeToInt(value) );
//		popup_stereo_type_actions << act;
//		stereo_set_type_group->addAction(act);
//		act->setCheckable(true);
//	}
//
//
//	// build the Anaglyph settings submenu
//	QActionGroup * stereo_set_anaglyph_group = new QActionGroup(stereoviewmenu);
//
//	QList<SoQtViewer::StereoType> stereo_type_values;
//	stereo_type_values << SoQtViewer::STEREO_NONE << SoQtViewer::STEREO_ANAGLYPH;
//
//	QStringList options;
//	options << "Standard Red-Cyan view" << "Left-eye only" << "Right-eye only";
//
//	foreach(QString value, options ) {
//		QAction * act = stereo_set_anaglyph_menu->addAction(value);
//		act->setData( value );
//		popup_stereo_anaglyph_actions << act;
//		stereo_set_anaglyph_group->addAction(act);
//		act->setCheckable(true);
//	}




	// STEREO settings - Camera version

//	// offset submenu
//	popup_stereo_offset_value_action_camera = stereoviewmenu_camera->addAction("dummy"); // dummy text replaced here below
//	// an action to change the let the user change the stereo offset by entering a double
//	float default_offset_camera = 0.1;
//	popup_stereo_offset_value_action_camera->setData(default_offset_camera);
//	popup_stereo_offset_value_action_camera->setText("Change STEREO offset [current: "+QString::number(default_offset_camera)+"]");
//
//	// balance submenu
//	popup_stereo_balance_value_action_camera = stereoviewmenu_camera->addAction("dummy"); // dummy text replaced here below
//	// an action to change the let the user change the stereo offset by entering a double
//	float default_balance_camera = 1.0;
//	popup_stereo_balance_value_action_camera->setData(default_balance_camera);
//	popup_stereo_balance_value_action_camera->setText("Change STEREO balance [current: "+QString::number(default_balance_camera)+"]");
//



//	// stereo camera view settings submenu
//	stereo_set_stereo_camera_view_menu = stereoviewmenu_camera->addMenu("Set Stereo &Camera view settings");
//	QActionGroup * stereo_set_stereo_camera_view_group = new QActionGroup(stereoviewmenu_camera);
//
//	QStringList options_camera;
//	options_camera << "Standard view" << "Left-eye view" << "Right-eye view";
//
//	foreach(QString value, options_camera ) {
//		QAction * act = stereo_set_stereo_camera_view_menu->addAction(value);
//		act->setData( value );
//		popup_stereo_anaglyph_actions_camera << act;
//		stereo_set_stereo_camera_view_group->addAction(act);
//		act->setCheckable(true);
//	}



	return true;

}

//____________________________________________________________________
void VP1ExaminerViewer::Imp::aboutToShowMenu(QMenu * menu)
{
	VP1Msg::messageVerbose("aboutToShowMenu()");

	if (viewmenu_storecurrentview==menu) {
		viewmenu_storecurrentview->clear();
		storeViewActions.clear();
		QString fs = firstAvailableStoredViewName();
		QAction * act = viewmenu_storecurrentview->addAction("Store as "+fs);
		act->setData(fs);
		storeViewActions << act;
		if (!storedViews.isEmpty()) {
			viewmenu_storecurrentview->addSeparator();
			foreach(StoredView sv, storedViews) {
				act = viewmenu_storecurrentview->addAction("Overwrite "+sv.name());
				act->setIcon(sv.icon());
				if (!fitsCurrentCamType(sv)) {
					act->setEnabled(false);
				} else {
					act->setData(sv.name());
					storeViewActions << act;
				}
			}
		}
		return;

	}
	if (viewmenu_zoomtoview==menu) {
		viewmenu_zoomtoview->clear();
		zoomToViewActions.clear();
		foreach(StoredView sv, storedViews) {
			QAction * act = viewmenu_zoomtoview->addAction(sv.name());
			act->setIcon(sv.icon());
			if (!fitsCurrentCamType(sv)) {
				act->setEnabled(false);
			} else {
				act->setData(sv.name());
				zoomToViewActions << act;
			}
		}
		return;
	}
	if (viewmenu_restoreview==menu) {
		viewmenu_restoreview->clear();
		restoreViewActions.clear();
		foreach(StoredView sv, storedViews) {
			QAction * act = viewmenu_restoreview->addAction(sv.name());
			act->setIcon(sv.icon());
			act->setData(sv.name());
			restoreViewActions << act;
		}
		return;
	}
	if (viewmenu_deleteview==menu) {
		viewmenu_deleteview->clear();
		deleteViewActions.clear();
		foreach(StoredView sv, storedViews) {
			QAction * act = viewmenu_deleteview->addAction(sv.name());
			act->setIcon(sv.icon());
			act->setData(sv.name());
			deleteViewActions << act;
		}
		return;
	}
	VP1Msg::messageDebug("VP1ExaminerViewer::Imp::aboutToShowMenu ERROR: Unknown menu");
}

//____________________________________________________________________
void VP1ExaminerViewer::Imp::updatePopupMenuStates()
{
	VP1Msg::messageVerbose("updatePopupMenuStates()");

	if (!popup_headLightAction)
		return;
	popup_headLightAction->setChecked(theclass->isHeadlight());
	popup_antiAliasAction->setChecked(theclass->isAntiAlias());
	popup_hidedecorationsaction->setChecked(! theclass->isDecoration());

	int idrawstyle_still = viewerDrawStyleToInt(theclass->getDrawStyle(SoQtViewer::STILL));
	foreach (QAction * act, popup_drawstyle_still_actions) {
		if (act->data().toInt() == idrawstyle_still) {
			act->setChecked(true);
			break;
		}
	}

	int idrawstyle_interactive = viewerDrawStyleToInt(theclass->getDrawStyle(SoQtViewer::INTERACTIVE));
	foreach (QAction * act, popup_drawstyle_interactive_actions) {
		if (act->data().toInt() == idrawstyle_interactive) {
			act->setChecked(true);
			break;
		}
	}

	int itransptype = VP1QtInventorUtils::transparencyTypeToInt(theclass->getTransparencyType());
	foreach (QAction * act, popup_transptype_actions) {
		if (act->data().toInt() == itransptype) {
			act->setChecked(true);
			break;
		}
	}

	popup_resetCameraAction->setVisible(resetCamera_state!=QByteArray());

}

//____________________________________________________________________
void VP1ExaminerViewer::dumpSceneToFile(QString filename)
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::dumpSceneToFile()");

	SoNode * rootnode = getSceneGraph();
	if (!rootnode)
		return;

	QWidget * w = getWidget();
	if (!w)
		return;

	if(filename.isEmpty()) {
		if (isAnimating())
			stopAnimating();
		filename = QFileDialog::getSaveFileName(w, "Select output file",
				(d->lastDumpFile.isEmpty()?VP1Settings::defaultFileSelectDirectory():d->lastDumpFile),
				"Inventor files (*.iv)",0,QFileDialog::DontResolveSymlinks);
		if(filename.isEmpty())
			return;
		if (!filename.endsWith(".iv"))
			filename += ".iv";
		d->lastDumpFile=filename;
	}

	SoGroup * standardisedRoot(0);
	if ( rootnode->getTypeId().isDerivedFrom(SoGroup::getClassTypeId()))
		standardisedRoot = VP1HEPVisUtils::convertToStandardScene(static_cast<SoGroup*>(rootnode));

	if (standardisedRoot&&VP1QtInventorUtils::writeGraphToFile(standardisedRoot, filename))
		VP1Msg::messageDebug("VP1ExaminerViewer: Dumped scene to file "+filename);
	else
		VP1Msg::messageDebug("VP1ExaminerViewer: Error: Problems dumping scene to file "+filename);

}

void VP1ExaminerViewer::dumpSceneToVRMLFile(QString filename){
	VP1Msg::messageVerbose("VP1ExaminerViewer::dumpSceneToVRMLFile()");

	SoNode * rootnode = getSceneGraph();
	if (!rootnode)
		return;

	QWidget * w = getWidget();
	if (!w)
		return;

	if(filename.isEmpty()) {
		if (isAnimating())
			stopAnimating();
		filename = QFileDialog::getSaveFileName(w, "Select output file",
				(d->lastDumpFile.isEmpty()?VP1Settings::defaultFileSelectDirectory():d->lastDumpFile),
				"VRML2.0/X3D files (*.wrl)",0,QFileDialog::DontResolveSymlinks);
		if(filename.isEmpty())
			return;
		if (!filename.endsWith(".wrl"))
			filename += ".wrl";
		d->lastDumpFile=filename;
	}

	SoGroup * standardisedRoot(0);
	if ( rootnode->getTypeId().isDerivedFrom(SoGroup::getClassTypeId()))
		standardisedRoot = VP1HEPVisUtils::convertToStandardScene(static_cast<SoGroup*>(rootnode));

	if (standardisedRoot&&VP1QtInventorUtils::writeGraphToVRMLFile(standardisedRoot, filename))
		VP1Msg::messageDebug("VP1ExaminerViewer: Dumped scene to VRML file "+filename);
	else
		VP1Msg::messageDebug("VP1ExaminerViewer: Error: Problems dumping scene to VRML file "+filename);

}

//____________________________________________________________________
void VP1ExaminerViewer::produceSVGImage(QString filename)
{
	QWidget * w = getWidget();
	if (!w)
		return;
	if(filename.isEmpty()) {
		if (isAnimating())
			stopAnimating();
		filename = QFileDialog::getSaveFileName(w, "Select output file",
				(d->lastSVGFile.isEmpty()?VP1Settings::defaultFileSelectDirectory():d->lastSVGFile),
				"Scalable Vector Graphics files (*.svg)",0,QFileDialog::DontResolveSymlinks);
		if(filename.isEmpty())
			return;
		if (!filename.endsWith(".svg"))
			filename += ".svg";
		d->lastSVGFile=filename;
	}

	VP1Msg::messageVerbose("Attempting to produce svg output: "+filename);

	//The following code contributed by Laurent Duflot:

	SoGL2PSAction::initClass();

	// want to render from above the SceneGraph so we get what the camera sees
	SoNode *rootA = static_cast<SoNode*>(SoQtRenderArea::getSceneGraph());
	if (!rootA ) return;
	SoNode *cam = static_cast<SoNode*>(this->getCamera());
	if ( ! cam ) return;
	SoSearchAction search;
	search.setNode(cam);
	search.apply(rootA);
	assert(search.getPath());
	SoNode* hiddenRoot =
			static_cast<SoFullPath *>(search.getPath())->getNodeFromTail(1);

	const SbViewportRegion& vpRegion = getViewportRegion();
	SoGL2PSAction* action = new SoGL2PSAction(vpRegion);
	action->setFileName(filename.toStdString().c_str());
	action->setFileFormat(SoGL2PSAction::SVG);
	action->enableFileWriting();
	action->apply(hiddenRoot);
	action->disableFileWriting();
	delete action;

}

//____________________________________________________________________
void VP1ExaminerViewer::produceEPSImage(QString filename)
{
	QWidget * w = getWidget();
	if (!w)
		return;
	if(filename.isEmpty()) {
		if (isAnimating())
			stopAnimating();
		filename = QFileDialog::getSaveFileName(w, "Select output file",
				(d->lastEPSFile.isEmpty()?VP1Settings::defaultFileSelectDirectory():d->lastEPSFile),
				"Encapsulated Postscript files (*.eps)",0,QFileDialog::DontResolveSymlinks);
		if(filename.isEmpty())
			return;
		if (!filename.endsWith(".eps"))
			filename += ".eps";
		d->lastEPSFile=filename;
	}

	VP1Msg::messageVerbose("Attempting to produce eps output: "+filename);

	//The following code contributed by Laurent Duflot:

	// first method : direct calls to gl2ps
	if (FILE *output = fopen (filename.toStdString().c_str(), "w"))
	{
		int buffsize = 0, state = GL2PS_OVERFLOW;
		while (state == GL2PS_OVERFLOW)
		{
			buffsize += 1024*1024;
			gl2psBeginPage ("VP1"/*fixme: should contain run/evt number*/, "VP1", NULL,
					GL2PS_EPS, GL2PS_BSP_SORT,
					GL2PS_USE_CURRENT_VIEWPORT
					| GL2PS_SILENT
					| GL2PS_BEST_ROOT | GL2PS_OCCLUSION_CULL
					| GL2PS_DRAW_BACKGROUND
					| 0,
					GL_RGBA, 0, NULL,0, 0, 0,
					buffsize, output, NULL);
			actualRedraw();
			state = gl2psEndPage();
		}
		fclose (output);
	}
}

//____________________________________________________________________
void VP1ExaminerViewer::setAmbientLight(int a)
{
	VP1Msg::messageVerbose("setAmbientLight()");

	a = std::max(0,std::min(100,a));
	if (d->ambientLightPercentage==a)
		return;
	d->ambientLightPercentage = a;
	d->updateAmbientLightText();
	d->updateEnvironmentNode();
}

//____________________________________________________________________
int VP1ExaminerViewer::ambientLight() const
{
	return d->ambientLightPercentage;
}

//____________________________________________________________________
void VP1ExaminerViewer::showPopupMenu()
{
	VP1Msg::messageVerbose("VP1ExaminerViewer: Showing popup menu.");
	if (!d->ensureMenuInit())
		return;
	d->updatePopupMenuStates();

	//Execute
	QAction * selAct = d->popup_menu->exec(QCursor::pos());

	//Act on selection:
	if (!selAct) {
		VP1Msg::messageVerbose("     => No selection.");
		return;
	}
	if ( selAct == d->popup_bgdColAction ) {
		VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu Change background colour.");
		if (isAnimating())
			stopAnimating();
		QColor oldcol = VP1QtInventorUtils::sbcol2qcol(getBackgroundColor());
		QColor col = QColorDialog::getColor(oldcol, getWidget());
		if (col!=oldcol)
			setBackgroundColor(VP1QtInventorUtils::qcol2sbcol(col));
		return;
	}
	if ( selAct == d->popup_ambientLightAction ) {
		VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu ambient light editor triggered");
		bool ok;
		// int newamb = QInputDialog::getInteger(getWidget(), "Change ambient light",
		int newamb = QInputDialog::getInt(getWidget(), "Change ambient light",
				"New ambient light percentage:",
				ambientLight(),0,100,1,&ok);
		if (ok)
			setAmbientLight(newamb);
		return;
	}
	if ( selAct == d->popup_headLightAction ) {
		VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu head light changed to "+VP1Msg::str(d->popup_headLightAction->isChecked()));
		setHeadlight(d->popup_headLightAction->isChecked());
		return;
	}
	if ( selAct == d->popup_hidedecorationsaction ) {
		VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu hide controls changed to "+VP1Msg::str(d->popup_hidedecorationsaction->isChecked()));
		setDecoration(! d->popup_hidedecorationsaction->isChecked());
		return;
	}
	if ( selAct == d->popup_antiAliasAction ) {
		VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu anti aliasing changed to "+VP1Msg::str(d->popup_antiAliasAction->isChecked()));
		setAntiAlias(d->popup_antiAliasAction->isChecked());
		d->grabFocus();//Needed since the GL calls triggered when setting antialiasing makes us loose focus (we obviusly just had it).
		return;
	}
	if ( selAct == d->popup_dumpSceneAction ) {
		VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu Dump scene to file");
		dumpSceneToFile();
		return;
	}
	
	if ( selAct == d->popup_dumpSceneVRMLAction ) {
		VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu Dump scene to file");
		dumpSceneToVRMLFile();
		return;
	}

	if ( selAct == d->popup_toSVGAction ) {
		VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu Produce SVG image");
		produceSVGImage();
		return;
	}

	if ( selAct == d->popup_toEPSAction ) {
		VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu Produce EPS image");
		produceEPSImage();
		return;
	}

	if ( selAct == d->popup_resetCameraAction ) {
		VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu Reset camera");
		resetCamera();
		return;
	}
	if (d->popup_drawstyle_still_actions.contains(selAct)) {
		SoQtViewer::DrawStyle ds = Imp::intToViewerDrawStyle(selAct->data().toInt());
		if (VP1Msg::verbose())
			VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu still draw style selected: "
					+Imp::viewerDrawStyle2PrettyString(ds));
		if (ds!=getDrawStyle(STILL))
			setDrawStyle(STILL,ds);
		return;
	}
	if (d->popup_drawstyle_interactive_actions.contains(selAct)) {
		SoQtViewer::DrawStyle ds = Imp::intToViewerDrawStyle(selAct->data().toInt());
		if (VP1Msg::verbose())
			VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu interactive draw style selected: "
					+Imp::viewerDrawStyle2PrettyString(ds));
		if (ds!=getDrawStyle(INTERACTIVE))
			setDrawStyle(INTERACTIVE,ds);
		return;
	}
	if (d->popup_transptype_actions.contains(selAct)) {
		SoGLRenderAction::TransparencyType type = VP1QtInventorUtils::intToTransparencyType(selAct->data().toInt());
		if (VP1Msg::verbose())
			VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu transparency type selected: "
					+VP1QtInventorUtils::transparencyType2PrettyString(type));
		setTransparencyType(type);
		return;
	}

	if (d->storeViewActions.contains(selAct)) {
		QString name = selAct->data().toString();
		VP1Msg::messageVerbose("Storing current view as "+name);

		bool camPerspective = currentCamIsPerspective();
		QByteArray camState = currentCameraState();
		QPixmap snapShot = VP1QtInventorUtils::renderToPixmap(this, 40, 40,false);

		Imp::StoredView sv(camState, camPerspective, snapShot, name);
		//Remove old stored views with that name (if any)
		int i(0);
		bool replaced(false);
		foreach(Imp::StoredView oldsv, d->storedViews) {
			if (oldsv.name()==name) {
				d->storedViews.replace(i,sv);
				replaced = true;
				break;
			}
			++i;
		}
		if (!replaced)
			d->storedViews << sv;
		d->storedViewsChanged();
		return;
	}

	if (d->zoomToViewActions.contains(selAct)) {
		QString name = selAct->data().toString();
		SoGroup * root = dynamic_cast<SoGroup*>(getSceneGraph());
		SoCamera * camera = getCamera();
		if (root&&camera) {
			foreach(Imp::StoredView sv, d->storedViews) {
				if (sv.name()==name) {
					if (isAnimating())
						stopAnimating();
					VP1CameraHelper::animatedZoomToCameraState( camera,root,sv.camState(),1.5, 100.0, 100.0, true );
					break;
				}
			}
		} else {
			VP1Msg::messageDebug("VP1CameraHelper Error: Attempting to zoom"
					" to stored view, but can't get root and camera pointers");
		}
		return;
	}



	if (d->restoreViewActions.contains(selAct)) {
		QString name = selAct->data().toString();
		SoGroup * root = dynamic_cast<SoGroup*>(getSceneGraph());
		SoCamera * camera = getCamera();
		if (root&&camera) {
			foreach(Imp::StoredView sv, d->storedViews) {
				if (sv.name()==name) {
					if (!d->fitsCurrentCamType(sv))
						toggleCameraType();
					if (isAnimating())
						stopAnimating();
					QByteArray ba = sv.camState();
					VP1QtInventorUtils::deserializeSoCameraParameters(ba,*(getCamera()));
					break;
				}
			}
		} else {
			VP1Msg::messageDebug("VP1CameraHelper Error: Attempting to zoom"
					" to stored view, but can't get root and camera pointers");
		}
		return;

	}

	if (d->deleteViewActions.contains(selAct)) {
		QString name = selAct->data().toString();
		int i(0);
		foreach(Imp::StoredView sv, d->storedViews) {
			if (sv.name()==name) {
				d->storedViews.removeAt(i);
				break;
			}
			++i;
		}
		d->storedViewsChanged();
		return;
	}

	if (selAct==d->customtour_launcheditor) {
		VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu Launch custom tour editor.");
		if (!d->customtoureditor) {
			d->customtoureditor = new VP1CustomTourEditor(this);
			QObject::connect(&(d->animationSequencer),SIGNAL(clipVolumePercentOfATLAS(double)),d->customtoureditor,SLOT(setClipVolumePercentOfATLAS(double)));
      VP1Controller::setCustomTourEditor(d->customtoureditor);
			d->customtoureditor->disableObjectWhenTourNotAvailable(d->customtour_execute);
			if (d->customtoureditorState!=QByteArray()) {
				d->customtoureditor->setState(d->customtoureditorState);
				d->customtoureditorState = QByteArray();
			}
		}
		d->customtoureditor->show();
		//Fixme: deal with minimised state!
		return;
	}

	if (selAct==d->customtour_execute) {
		VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu Execute custom tour.");
		startCustomTour();
		return;
	}

	if (selAct==d->action_movieenabled) {
		VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu movie enabled changed.");
		d->updateMovieMenuVisuals();
		return;
	}
	if (selAct==d->action_moviewidth) {
		int old = d->action_moviewidth->data().toInt();
		bool ok;
		// int newwidth = QInputDialog::getInteger(getWidget(), "Change movie width",
		int newwidth = QInputDialog::getInt(getWidget(), "Change movie width",
				"New movie width:", old,1,4000,1,&ok);
		if (ok&&old!=newwidth) {
			VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu changed movie width to "+VP1Msg::str(newwidth));
			d->action_moviewidth->setData(newwidth);
			d->updateMovieMenuVisuals();
		}
		return;
	}
	if (selAct==d->action_movieheight) {
		int old = d->action_movieheight->data().toInt();
		bool ok;
		// int newheight = QInputDialog::getInteger(getWidget(), "Change movie height",
		int newheight = QInputDialog::getInt(getWidget(), "Change movie height",
				"New movie height:", old,1,4000,1,&ok);
		if (ok&&old!=newheight) {
			VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu changed movie height to "+VP1Msg::str(newheight));
			d->action_movieheight->setData(newheight);
			d->updateMovieMenuVisuals();
		}
		return;
	}
	if (selAct==d->action_moviefps) {
		int old = d->action_moviefps->data().toInt();
		bool ok;
		// int newfps = QInputDialog::getInteger(getWidget(), "Change movie FPS",
		int newfps = QInputDialog::getInt(getWidget(), "Change movie FPS",
				"New movie frames per second:", old,1,4000,1,&ok);
		if (ok&&old!=newfps) {
			VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu changed movie fps to "+VP1Msg::str(newfps));
			d->action_moviefps->setData(newfps);
			d->updateMovieMenuVisuals();
		}
		return;
	}
	if (selAct==d->action_movieoutdir) {
		QString old = d->action_movieoutdir->data().toString();
		QString newoutdir = QFileDialog::getExistingDirectory ( getWidget(), "Select movie frame output directory",old);
		if (!newoutdir.isEmpty()&&old!=newoutdir) {
			VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu changed movie outdir to "+VP1Msg::str(newoutdir));
			d->action_movieoutdir->setData(newoutdir);
			d->updateMovieMenuVisuals();
		}
		return;
	}
	if (selAct==d->action_moviefadetocurrentview) {
		fadeLastRecordedFrameToCurrent(1.0);
		return;
	}

	if (selAct==d->popup_tourReturnToStartAction
			||selAct==d->popup_tourStartEachEvent
			||selAct==d->popup_tourSpeedVerySlow
			||selAct==d->popup_tourSpeedSlow
			||selAct==d->popup_tourSpeedMedium
			||selAct==d->popup_tourSpeedFast
			||selAct==d->popup_tourSpeedVeryFast
			||selAct==d->popup_tourPartsVertex
			||selAct==d->popup_tourPartsInDet
			||selAct==d->popup_tourPartsCalo
			||selAct==d->popup_tourPartsMuon
			||selAct==d->popup_tourLoopOnce
			||selAct==d->popup_tourLoopTwice
			||selAct==d->popup_tourLoopThrice
			||selAct==d->popup_tourLoopForever) {
		VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu tour setting changed.");
		return;
	}
	if (selAct==d->popup_tourExecute) {
		VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu Tour Execute selected => starting tour.");
		startTour();
		return;
	}


	// stereo view actions


	if (selAct==d->stereo_launcheditor) {
			VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu Launch custom STEREO editor.");
			if (!d->customstereoeditor) {
				d->customstereoeditor = new VP1CustomStereoEditor(this);
//				d->customstereoeditor->disableObjectWhenTourNotAvailable(d->customtour_execute);
//				if (d->customstereoeditorState!=QByteArray()) { //TODO: implement save stereo settings
//					d->customstereoeditor->setState(d->customstereoeditorState);
//					d->customstereoeditorState = QByteArray();
//				}
			}
			d->customstereoeditor->show();
			//Fixme: deal with minimised state!
			return;
		}

// N.B.!! --> those separate actions below are not needed anymore,
//            because we introduced the stereo editor window

//	if (d->popup_stereo_offset_actions.contains(selAct)) {
//
////		//old value
//		float old = d->stereo_offset_value; //->data().toFloat();
//
//		// new value
//		float offset = selAct->data().toFloat();
//
//		int newoffset = offset;
//		if (old != newoffset) {
//			VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu stereo offset selected: "
//					+VP1Msg::str(newoffset));
//			d->stereo_offset_value = newoffset;
//		}
//
//		setStereoOffsetSlot(offset);
//		return;
//	}
//
//	if (selAct==d->popup_stereo_offset_value_action) {
//
//			float old = d->popup_stereo_offset_value_action->data().toFloat();
//			bool ok;
//			int newoffset = QInputDialog::getDouble(getWidget(), "Change stereo OFFSET - 0.1 is the standard offset between left and right eye",
//					"New stereo offset: ", old,0.1,4000,1,&ok);
//			if (ok && old != newoffset) {
//				VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu changed stereo offset to "+VP1Msg::str(newoffset));
//				d->popup_stereo_offset_value_action->setData(newoffset);
//				setStereoOffsetSlot(newoffset);
//				d->popup_stereo_offset_value_action->setText("Change STEREO offset [current: "+QString::number(newoffset)+"]");
//			}
//			return;
//		}
//
	if (selAct==d->popup_focal_value_action) {

		//		float old = d->popup_focal_value_action->data().toFloat();
		bool ok;
		SoPerspectiveCamera * camera = dynamic_cast<SoPerspectiveCamera*>(getCamera());
		if (! (camera==NULL) ) {
			float current_value = camera->focalDistance.getValue();
			int newfocal = QInputDialog::getDouble(getWidget(),
					"Change focal length", // title
					"New focal length: ", // label
					current_value, // initial value
					0.1, // min value
					2147483647, // max value
					1, // decimals
					&ok);
			if (ok && current_value != newfocal) {
				VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu changed focal length to "+VP1Msg::str(newfocal));
				d->popup_focal_value_action->setData(newfocal);
				//			SoCamera *camera = getCamera();
				camera->focalDistance.setValue(newfocal);
				//			camera->heightAngle.setValue(newfocal);
				d->popup_focal_value_action->setText("Change FOCAL LENGTH value [current: "+QString::number(newfocal)+"]");
			}
		} else {
			VP1Msg::message("Warning! No 'camera'...");
		}
		return;
	}
//
//
//	if (d->popup_stereo_type_actions.contains(selAct)) {
//
//		SoQtViewer::StereoType type = d->intToViewerStereoType(selAct->data().toInt());
//
//		if (VP1Msg::verbose())
//			VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu stereo type selected: "
//					+d->viewerStereoType2PrettyString(type));
//
//		setStereoTypeSlot(type);
//
//		// enable the relevant menus
//		if (type == SoQtViewer::STEREO_ANAGLYPH) {
//			d->stereo_set_anaglyph_menu->setEnabled(true);
//		} else {
//			d->stereo_set_anaglyph_menu->setEnabled(false);
//		}
//
//
//		return;
//	}
//	if (d->popup_stereo_anaglyph_actions.contains(selAct)) {
//
//		QString type = selAct->data().toString();
//
//		if (VP1Msg::verbose())
//			VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu stereo anaglyph selected: "
//					+type);
//
//		// default values for normal Anaglyph red-cyan view
//		SbBool leftFilter[3] = {true, false, false};
//		SbBool rightFilter[3] = {false, true, true};
//
//		if (type.contains("Left-eye")) {
//			SbBool leftFilter[3] = {true, true, true};
//			SbBool rightFilter[3] = {false, false, false};
//			setAnaglyphStereoColorMasksSlot(leftFilter, rightFilter);
//		}
//		else if (type.contains("Right-eye")) {
//			SbBool leftFilter[3] = {false, false, false};
//			SbBool rightFilter[3] = {true, true, true};
//			setAnaglyphStereoColorMasksSlot(leftFilter, rightFilter);
//		}
//		else {
//			setAnaglyphStereoColorMasksSlot(leftFilter, rightFilter);
//		}
//		return;
//	}
//
//
//	// STEREO - Camera
//	if (selAct==d->popup_stereo_offset_value_action_camera) {
//
//		float old = d->popup_stereo_offset_value_action_camera->data().toFloat();
//		bool ok;
//		float newoffset = QInputDialog::getDouble(getWidget(), "Change stereo OFFSET (Camera) - 0.1 is the standard offset between left and right eye",
//				"New stereo offset: ", old,0.1,4000,1,&ok);
//		if (ok && old != newoffset) {
//			VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu changed CAMERA stereo offset to "+VP1Msg::str(newoffset));
//			d->popup_stereo_offset_value_action_camera->setData(newoffset);
//
//			SoCamera *camera = getCamera();
//			camera->setStereoAdjustment(newoffset);
////			setStereoOffsetSlot(newoffset);
//			d->popup_stereo_offset_value_action_camera->setText("Change CAMERA STEREO offset [current: "+QString::number(newoffset)+"]");
//		}
//		return;
//	}
//	if (selAct==d->popup_stereo_balance_value_action_camera) {
//
//		float old = d->popup_stereo_balance_value_action_camera->data().toFloat();
//		bool ok;
//		float newoffset = QInputDialog::getDouble(getWidget(), "Change stereo OFFSET (Camera) - 0.1 is the standard offset between left and right eye",
//				"New stereo offset: ", old,0.1,4000,1,&ok);
//		if (ok && old != newoffset) {
//			VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu changed CAMERA stereo offset to "+VP1Msg::str(newoffset));
//			d->popup_stereo_balance_value_action_camera->setData(newoffset);
//
//			SoCamera *camera = getCamera();
//			camera->setBalanceAdjustment(newoffset);
////			setStereoOffsetSlot(newoffset);
//			d->popup_stereo_balance_value_action_camera->setText("Change CAMERA STEREO balance [current: "+QString::number(newoffset)+"]");
//		}
//		return;
//	}
//
//
//	if (d->popup_stereo_anaglyph_actions_camera.contains(selAct)) {
//
//			QString type = selAct->data().toString();
//
//			if (VP1Msg::verbose())
//				VP1Msg::messageVerbose("VP1ExaminerViewer::showPopupMenu CAMERA stereo view mode selected: "
//						+type);
//
//			SoCamera *camera = getCamera();
//
//			if (type.contains("Left-eye")) {
//				camera->setStereoMode(SoCamera::LEFT_VIEW);
//			}
//			else if (type.contains("Right-eye")) {
//				camera->setStereoMode(SoCamera::RIGHT_VIEW);
//			}
//			else {
//				camera->setStereoMode(SoCamera::MONOSCOPIC);
//			}
//			return;
//		}




	// default
	VP1Msg::messageDebug("VP1ExaminerViewer::showPopupMenu ERROR: Unknown selected item!");
	return;

}


//____________________________________________________________________
void VP1ExaminerViewer::launchStereoEditor()
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::launchStereoEditor()");

	if (!d->customstereoeditor) {
		d->customstereoeditor = new VP1CustomStereoEditor(this);
		//				d->customstereoeditor->disableObjectWhenTourNotAvailable(d->customtour_execute);
		//				if (d->customstereoeditorState!=QByteArray()) { //TODO: implement save stereo settings
		//					d->customstereoeditor->setState(d->customstereoeditorState);
		//					d->customstereoeditorState = QByteArray();
		//				}
	}
	d->customstereoeditor->show();
	//Fixme: deal with minimised state!
	return;
}


//____________________________________________________________________
QList<SoQtViewer::StereoType> VP1ExaminerViewer::Imp::getAllStereoViewTypes()
{
	VP1Msg::messageVerbose("getAllStereoViewTypes()");
	QList<SoQtViewer::StereoType> l;

	// for the moment we only use these two
	l << SoQtViewer::STEREO_NONE; //Use monoscopic rendering.
	l<< SoQtViewer::STEREO_ANAGLYPH; //Render stereo by superimposing two images of the same scene, but with different color filters over the left and right view (or "eye").
									//This is a way of rendering stereo which works on any display, using color-filter glasses. Such glasses are usually cheap and easy to come by.
									//See also: setAnaglyphStereoColorMasks()

	// test
	l << SoQtViewer::STEREO_QUADBUFFER; //Render stereo by using OpenGL quad-buffers. This is the most common interface for stereo rendering for more expensive hardware devices, such as shutter glasses and polarized glasses.
										//The well known Crystal Eyes glasses are commonly used with this type of stereo display.
	l << SoQtViewer::STEREO_INTERLEAVED_ROWS; //Interleaving / interlacing rows from the left and right eye is another stereo rendering method requiring special hardware. One example of a provider of shutter glasses working with interleaved glasses is VRex:
												//http://www.vrex.com/
	l << SoQtViewer::STEREO_INTERLEAVED_COLUMNS; //Same basic technique as SoQtViewer::STEREO_INTERLEAVED_ROWS, only it is vertical lines that are interleaved / interlaced, instead of horizontal lines.

	return l;
}
//____________________________________________________________________
QString VP1ExaminerViewer::Imp::viewerStereoType2PrettyString( SoQtViewer::StereoType ds )
{
	VP1Msg::messageVerbose("viewerStereoType2PrettyString() - "+QString::number(ds) );
	switch (ds) {
	case SoQtViewer::STEREO_NONE: return "NO Stereo";
	case SoQtViewer::STEREO_ANAGLYPH: return "Anaglyph Stereo (Red-Cyan 3D mode)";
	case SoQtViewer::STEREO_QUADBUFFER: return "QUADBUFFER";
	case SoQtViewer::STEREO_INTERLEAVED_ROWS: return "INTERLEAVED_ROWS";
	case SoQtViewer::STEREO_INTERLEAVED_COLUMNS: return "INTERLEAVED_COLUMNS";
	default:
		VP1Msg::messageDebug("VP1ExaminerViewer::Imp::viewerStereoType2PrettyString ERROR: Unknown viewer draw style");
		return "x";
	}
}
//____________________________________________________________________
int VP1ExaminerViewer::Imp::viewerStereoTypeToInt( SoQtViewer::StereoType ds )
{
	switch (ds) {
	case SoQtViewer::STEREO_NONE: return 0;
	case SoQtViewer::STEREO_ANAGLYPH: return 1;
	case SoQtViewer::STEREO_QUADBUFFER: return 2;
	case SoQtViewer::STEREO_INTERLEAVED_ROWS: return 3;
	case SoQtViewer::STEREO_INTERLEAVED_COLUMNS: return 4;
	default:
		VP1Msg::messageDebug("VP1ExaminerViewer::Imp::viewerStereoTypeToInt ERROR: Unknown viewer draw style");
		return -1;
	}
}
//____________________________________________________________________
SoQtViewer::StereoType VP1ExaminerViewer::Imp::intToViewerStereoType( int i )
{
	VP1Msg::messageVerbose("intToViewerStereoType() - "+QString::number(i) );
	switch (i) {
	case 0: return SoQtViewer::STEREO_NONE;
	case 1: return SoQtViewer::STEREO_ANAGLYPH;
	case 2: return SoQtViewer::STEREO_QUADBUFFER;
	case 3: return SoQtViewer::STEREO_INTERLEAVED_ROWS;
	case 4: return SoQtViewer::STEREO_INTERLEAVED_COLUMNS;
	default:
		VP1Msg::messageDebug("VP1ExaminerViewer::Imp::intToViewerStereoType ERROR: int out of range "+VP1Msg::str(i));
		return SoQtViewer::STEREO_NONE;
	}
}




//____________________________________________________________________
QList<SoQtViewer::DrawStyle> VP1ExaminerViewer::Imp::getAllViewerDrawStyles()
{
	VP1Msg::messageVerbose("getAllViewerDrawStyles()");

	QList<SoQtViewer::DrawStyle> l;
	l << SoQtViewer::VIEW_SAME_AS_STILL
			<< SoQtViewer::VIEW_AS_IS
			<< SoQtViewer::VIEW_HIDDEN_LINE
			<< SoQtViewer::VIEW_NO_TEXTURE
			<< SoQtViewer::VIEW_LOW_COMPLEXITY
			<< SoQtViewer::VIEW_LINE
			<< SoQtViewer::VIEW_POINT
			<< SoQtViewer::VIEW_BBOX
			<< SoQtViewer::VIEW_LOW_RES_LINE
			<< SoQtViewer::VIEW_LOW_RES_POINT
			<< SoQtViewer::VIEW_WIREFRAME_OVERLAY;
	return l;
}

//____________________________________________________________________
int VP1ExaminerViewer::Imp::viewerDrawStyleToInt( SoQtViewer::DrawStyle ds )
{
	switch (ds) {
	case SoQtViewer::VIEW_AS_IS: return 0;
	case SoQtViewer::VIEW_HIDDEN_LINE: return 1;
	case SoQtViewer::VIEW_NO_TEXTURE: return 2;
	case SoQtViewer::VIEW_LOW_COMPLEXITY: return 3;
	case SoQtViewer::VIEW_LINE: return 4;
	case SoQtViewer::VIEW_POINT: return 5;
	case SoQtViewer::VIEW_BBOX: return 6;
	case SoQtViewer::VIEW_LOW_RES_LINE: return 7;
	case SoQtViewer::VIEW_LOW_RES_POINT: return 8;
	case SoQtViewer::VIEW_SAME_AS_STILL: return 9;
	case SoQtViewer::VIEW_WIREFRAME_OVERLAY: return 10;
	default:
		VP1Msg::messageDebug("VP1ExaminerViewer::Imp::viewerDrawStyleToInt ERROR: Unknown viewer draw style");
		return -1;
	}
}

//____________________________________________________________________
SoQtViewer::DrawStyle VP1ExaminerViewer::Imp::intToViewerDrawStyle( int i )
{
	switch (i) {
	case 0: return SoQtViewer::VIEW_AS_IS;
	case 1: return SoQtViewer::VIEW_HIDDEN_LINE;
	case 2: return SoQtViewer::VIEW_NO_TEXTURE;
	case 3: return SoQtViewer::VIEW_LOW_COMPLEXITY;
	case 4: return SoQtViewer::VIEW_LINE;
	case 5: return SoQtViewer::VIEW_POINT;
	case 6: return SoQtViewer::VIEW_BBOX;
	case 7: return SoQtViewer::VIEW_LOW_RES_LINE;
	case 8: return SoQtViewer::VIEW_LOW_RES_POINT;
	case 9: return SoQtViewer::VIEW_SAME_AS_STILL;
	case 10: return SoQtViewer::VIEW_WIREFRAME_OVERLAY;
	default:
		VP1Msg::messageDebug("VP1ExaminerViewer::Imp::intToViewerDrawStyle ERROR: int out of range "+VP1Msg::str(i));
		return SoQtViewer::VIEW_AS_IS;
	}
}

//____________________________________________________________________
QString VP1ExaminerViewer::Imp::viewerDrawStyle2PrettyString( SoQtViewer::DrawStyle ds )
{
	switch (ds) {
	case SoQtViewer::VIEW_AS_IS: return "As is";
	case SoQtViewer::VIEW_HIDDEN_LINE: return "Hidden line";
	case SoQtViewer::VIEW_NO_TEXTURE: return "No texture";
	case SoQtViewer::VIEW_LOW_COMPLEXITY: return "Low complexity";
	case SoQtViewer::VIEW_LINE: return "Line";
	case SoQtViewer::VIEW_POINT: return "Point";
	case SoQtViewer::VIEW_BBOX: return "Bounding boxes";
	case SoQtViewer::VIEW_LOW_RES_LINE: return "Low resolution lines";
	case SoQtViewer::VIEW_LOW_RES_POINT: return "Low resolution points";
	case SoQtViewer::VIEW_SAME_AS_STILL: return "Same as still";
	case SoQtViewer::VIEW_WIREFRAME_OVERLAY: return "Wireframe overlay";
	default:
		VP1Msg::messageDebug("VP1ExaminerViewer::Imp::viewerDrawStyleToInt ERROR: Unknown viewer draw style");
		return "";
	}
}

//____________________________________________________________________
void VP1ExaminerViewer::setSceneGraph(SoNode *n)
{
	VP1Msg::messageVerbose("VP1ExaminerViewer::setSceneGraph()");

	if (!d->actualSceneGraph) {
		d->actualSceneGraph = new SoGroup;
		d->actualSceneGraph->ref();
		if (!d->environmentNode) {
			d->environmentNode = new SoEnvironment;
			d->environmentNode->ref();
		}
		d->updateEnvironmentNode();
		d->actualSceneGraph->addChild(d->environmentNode);
		SoQtExaminerViewer::setSceneGraph(d->actualSceneGraph);
	}
	while(d->actualSceneGraph->getNumChildren()>1)
		d->actualSceneGraph->removeChild(1);

	if (n)
		d->actualSceneGraph->addChild(n);
}

//____________________________________________________________________
SoNode* VP1ExaminerViewer::getSceneGraph()
{
	return (d->actualSceneGraph && d->actualSceneGraph->getNumChildren()>1) ? d->actualSceneGraph->getChild(1) : 0;
}

//____________________________________________________________________
void VP1ExaminerViewer::fadeLastRecordedFrameToCurrent(double time_seconds)
{
	if (!d->ensureMenuInit())
		return;
	QString lastfile, nextfile;
	QString outdir = d->action_movieoutdir->data().toString();
	VP1CameraHelper::getLastAndNextFrameFileNames( outdir,
			d->movieFrameFileNamePrefix,
			lastfile, nextfile );

	QImage img0(lastfile);

	if (lastfile.isEmpty()||img0.isNull()) {
		VP1Msg::messageDebug("VP1ExaminerViewer ERROR: No previous image found!");
		return;
	}
	if (nextfile.isEmpty())
		return;

	QImage img1 = VP1QtInventorUtils::renderToImage(this, img0.width(),img0.height(),false/*transp*/);
	if (img1.isNull())
		return;

	int nTransitionFrames = std::max(1,static_cast<int>(d->action_moviefps->data().toInt()*time_seconds+0.5));
	VP1Msg::messageDebug("VP1ExaminerViewer Creating "+VP1Msg::str(nTransitionFrames)+" transition frames");

	for (int i = 0; i < nTransitionFrames; ++i) {
		double fadefact((i+1.0)/(nTransitionFrames+1.0));//Should not be 0.0 or 1.0
		QString dummy, filename;
		VP1CameraHelper::getLastAndNextFrameFileNames( outdir, d->movieFrameFileNamePrefix,dummy,filename );
		QImage img = VP1QtUtils::fadeImage(img0, img1, fadefact );
		if (img.isNull()) {
			VP1Msg::messageDebug("VP1ExaminerViewer ERROR: Problems creating image!");
		} else {
			if (!img.save(filename))
				VP1Msg::messageDebug("VP1ExaminerViewer ERROR: Could not save image file "+filename);
		}
	}

	QString dummy, filename;
	VP1CameraHelper::getLastAndNextFrameFileNames( outdir, d->movieFrameFileNamePrefix,dummy,filename );
	if (!img1.save(filename))
		VP1Msg::messageDebug("VP1ExaminerViewer ERROR: Could not save image file "+filename);

}
