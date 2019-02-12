/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class IVP13DStandardChannelWidget    //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Base/IVP13DStandardChannelWidget.h"
#include "VP1Base/SoCooperativeSelection.h"
#include "VP1Base/VP1ExaminerViewer.h"
#include "VP1Base/VP1Msg.h"
#include "ui_vp13dstandardchannelsnapshotgroupbox.h"

#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1Base/IVP13DSystem.h"
#include "VP1Base/IVP13DSystemSimple.h"
#include "VP1Base/VP1ControllerHelper.h"
#include "VP1Base/VP1TabWidget.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/actions/SoLineHighlightRenderAction.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/SoOffscreenRenderer.h>

#include <QSplitter>
#include <QVBoxLayout>
#include <QMap>
#include <QTimer>
#include <QBuffer>
#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <cassert>

class IVP13DStandardChannelWidget::Imp {
public:
  IVP13DStandardChannelWidget*channel;
  bool detectorViewButtons;
  SoSeparator * root;
  VP1ExaminerViewer * viewer;
  bool need_initial_viewall;
  QMap<IVP13DSystem*,SoSeparator*> system2rootNodes;

  QMap<QCheckBox*,IVP1System*> checkbox2system;
  QList<IVP1System*> systemsAllowedControllers;
  QList<IVP13DSystem*> systemsAllowedExtraDisplayWidgets;
  QList<QPair<IVP1System*,bool> > system2switchable;
  QMap<IVP1System*,bool> system2startdisabled;
  QList<IVP13DSystem*> systemsAllowedCameraList;
  void updateSystemState(QCheckBox*);

  //Snapshots:
  Ui::VP13DStandardChannelSnapShotGroupBox uisnapshot;

  void setupSplitter(QWidget * rightwidget);

  void autoSnapshot();

  static const int min3dx = 150;
  static const int min3dy = 180;


  QMap<IVP1System*,QWidget*> sys2tabpage;
  VP1TabWidget* tabwidget;

  QSplitter * extradisplaywidget_splitter;

  // restoreFromState versions
  void restoreFromState_v7(QDataStream& state);


};

//___________________________________________________________________________
void IVP13DStandardChannelWidget::launchStereoEditor()
{
	m_d->viewer->launchStereoEditor();
}


//___________________________________________________________________________
bool IVP13DStandardChannelWidget::setAntiAliasing(bool aa)
{
	m_d->viewer->setAntiAlias(aa);
	return true;
}

//___________________________________________________________________________
bool IVP13DStandardChannelWidget::isAntiAliasing()
{
	return m_d->viewer->isAntiAlias();
}

//___________________________________________________________________________
IVP13DStandardChannelWidget::IVP13DStandardChannelWidget(const QString & name, const QString & information,
							 const QString & contact_info, bool detectorViewButtons)
  : IVP13DChannelWidget(name,information,contact_info), m_d(new Imp)
{
  m_d->detectorViewButtons = detectorViewButtons;
  m_d->need_initial_viewall=true;
  m_d->channel=this;
  m_d->uisnapshot.checkBox_as_shown = 0;//Use this to see if the uisnapshot pointers are initialised/

  SoCooperativeSelection::ensureInitClass();

  m_d->root= new SoSeparator;
  m_d->root->setName("StandardChannelWidgetSceneRoot");
  m_d->root->ref();
  m_d->viewer = 0;

  m_d->tabwidget = 0;

  m_d->extradisplaywidget_splitter = 0;
}

//___________________________________________________________________________
IVP13DStandardChannelWidget::~IVP13DStandardChannelWidget()
{
  m_d->root->unref();
  delete m_d;
}

//___________________________________________________________________________
void IVP13DStandardChannelWidget::addSystem( IVP13DSystem*system, const SystemOptions& options ) {


  bool switchable = ! (options & DisallowSwitchable);
  bool allowController = ! (options & DisallowController);
  bool allowExtraDisplayWidget = ! (options & DisallowExtraDisplayWidget);
  bool startDisabled = (options & StartDisabled);

  registerSystem(system);
  if (!( options & DisallowCameraAccess ))
    m_d->systemsAllowedCameraList << system;

  SoSeparator * sysroot = system->getSceneGraph();
  assert(!m_d->system2rootNodes.contains(system));
  m_d->system2rootNodes.insert(system,sysroot);
  m_d->root->addChild(sysroot);

  m_d->system2switchable << QPair<IVP1System*,bool>(system,switchable);

  assert(!m_d->system2startdisabled.contains(system));
  m_d->system2startdisabled.insert(system,startDisabled);
  assert(m_d->system2startdisabled.contains(system));

  if (allowController) {
    m_d->systemsAllowedControllers << system;
    connect(system,SIGNAL(itemFromSystemSelected()),this,SLOT(showControlsForSystem()));
  }

  if (allowExtraDisplayWidget)
    m_d->systemsAllowedExtraDisplayWidgets << system;
}

//___________________________________________________________________________
void IVP13DStandardChannelWidget::create() {

  //Set up the controller.
  QWidget * snapshotgroupbox = new QWidget();
  m_d->uisnapshot.setupUi(snapshotgroupbox);

  VP1ColorSelectButton * colorselectbutton_dummy;
  registerController(VP1ControllerHelper::compositionController( m_d->systemsAllowedControllers,
								 m_d->sys2tabpage,m_d->tabwidget,
								 m_d->system2switchable,
								 m_d->checkbox2system,
								 colorselectbutton_dummy,
								 snapshotgroupbox,true ));
  short x,y;
  SoOffscreenRenderer::getMaximumResolution().getValue(x,y);

  // now the range is set in the ui file.
//  m_d->uisnapshot.spinBox_width->setRange ( 1, x/4 - 10 );
//  m_d->uisnapshot.spinBox_height->setRange ( 1, y/4 - 10 );

  connect(m_d->uisnapshot.checkBox_as_shown,SIGNAL(toggled(bool)),this,SLOT(updateSnapshotDim()));
  connect(m_d->uisnapshot.spinBox_width,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
  connect(m_d->uisnapshot.spinBox_height,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
  connect(m_d->uisnapshot.radioButton_width,SIGNAL(toggled(bool)),this,SLOT(updateSnapshotDim()));
  connect(m_d->uisnapshot.radioButton_height,SIGNAL(toggled(bool)),this,SLOT(updateSnapshotDim()));

  connect(m_d->uisnapshot.groupBox_imagePresets,SIGNAL(toggled(bool)),this,SLOT(setImageFromPresets()));
  connect(m_d->uisnapshot.radioButton_720p,SIGNAL(toggled(bool)),this,SLOT(setImageFromPresets()));
  connect(m_d->uisnapshot.radioButton_1080p,SIGNAL(toggled(bool)),this,SLOT(setImageFromPresets()));
  connect(m_d->uisnapshot.radioButton_4K,SIGNAL(toggled(bool)),this,SLOT(setImageFromPresets()));
  connect(m_d->uisnapshot.radioButton_8K,SIGNAL(toggled(bool)),this,SLOT(setImageFromPresets()));


  //   m_d->systemsAllowedControllers.clear();
  m_d->system2switchable.clear();

  QMapIterator<QCheckBox*,IVP1System*> it(m_d->checkbox2system);
  while (it.hasNext()) {
    it.next();

    assert(m_d->system2startdisabled.contains(it.value()));
    if (m_d->system2startdisabled[it.value()]) {
      it.key()->setChecked(false);
      m_d->updateSystemState(it.key());
    }

    connect(it.key(),SIGNAL(toggled(bool)),this,SLOT(toggleSystemActive()));

  }

  QTimer::singleShot(0, this, SLOT(updateSnapshotDim()));


  //Setup viewer (with possible splitter for extra widgets):
  QList<QPair<QString,QWidget*> > sysname2extradisplay;
  foreach (IVP13DSystem* sys,m_d->systemsAllowedExtraDisplayWidgets) {
    QWidget * edwidget = sys->buildExtraDisplayWidget();
    if (edwidget)
      sysname2extradisplay << QPair<QString,QWidget*>(sys->name(),edwidget);
  }

  if (sysname2extradisplay.count()==0) {
    m_d->viewer = new VP1ExaminerViewer(this,m_d->detectorViewButtons);
    setMinimumSize(m_d->min3dx,m_d->min3dy);
  } else if (sysname2extradisplay.count()==1) {
    m_d->setupSplitter(sysname2extradisplay.front().second);
  } else if (sysname2extradisplay.count()>1) {
    VP1TabWidget * tabWidget = new VP1TabWidget(0);
    for (int i = 0; i < sysname2extradisplay.count(); ++i) {
      tabWidget->addTab(sysname2extradisplay.at(i).second,sysname2extradisplay.at(i).first);
    }
    tabWidget->setCurrentIndex(0);
    m_d->setupSplitter(tabWidget);
  }

  registerRenderArea(m_d->viewer);//remember this...

  m_d->viewer->setSceneGraph(m_d->root);
  m_d->viewer->setGLRenderAction(new SoLineHighlightRenderAction());

  // Default Transparency Type
  //  m_d->viewer->setTransparencyType( SoGLRenderAction::DELAYED_BLEND ); // old
  m_d->viewer->setTransparencyType( SoGLRenderAction::BLEND ); // this looks better

  //Setup camera info:
  foreach(IVP13DSystem*sys,m_d->systemsAllowedCameraList)
    sys->registerViewer(m_d->viewer);


  /* We want to change the default value for the "As shown" option
   * At the beginning the option was set to True by default, but
   * the most used setting when creating event displays for ATLAS is
   * False, because we want to save the full image as a snapshot,
   * without the frame and buttons from VP1.
   * So we now set it to False by default.
   * (We do that with setChecked() in order to not having to change
   * the whole logics behind.
   */
  m_d->uisnapshot.checkBox_as_shown->setChecked(false); // fixme: check if that works, or if it's still TRUE

}

//___________________________________________________________________________
void IVP13DStandardChannelWidget::Imp::setupSplitter(QWidget * rightwidget)
{
  QVBoxLayout * vboxLayout = new QVBoxLayout(channel);
  vboxLayout->setContentsMargins(0, 0, 0, 0);
  extradisplaywidget_splitter = new QSplitter(channel);
  extradisplaywidget_splitter->setObjectName("3DchannelSplitter");
  extradisplaywidget_splitter->setOrientation(Qt::Horizontal);
  QWidget * widget_viewer = new QWidget(extradisplaywidget_splitter);
  widget_viewer->setObjectName("widget_viewer");
  extradisplaywidget_splitter->addWidget(widget_viewer);
  rightwidget->setParent(extradisplaywidget_splitter);
  extradisplaywidget_splitter->addWidget(rightwidget);
  vboxLayout->addWidget(extradisplaywidget_splitter);
  viewer = new VP1ExaminerViewer(widget_viewer);
  widget_viewer->setMinimumSize(min3dx,min3dy);
  connect(extradisplaywidget_splitter,SIGNAL(splitterMoved (int,int)),channel,SLOT(updateSnapshotDim()));
}

//___________________________________________________________________________
void IVP13DStandardChannelWidget::Imp::autoSnapshot()
{
	VP1Msg::messageVerbose("IVP13DStandardChannelWidget::Imp::autoSnapshot()");

	int runnumber(0);
	unsigned long long eventnumber(0);
	channel->getRunEvtNumber(runnumber,eventnumber);

	QString snapshotDirName = VP1QtUtils::environmentVariableValue("VP1_SCREENSHOTS_DIR");
	QFileInfo snapshotDir(snapshotDirName);
	if(!snapshotDir.exists()||!snapshotDir.isDir()||!snapshotDir.isReadable()||!snapshotDir.isWritable()) {
		channel->message("The directory for storing VP1 snapshots "+snapshotDirName+" either does not exist or is not writable");
		return;
	}

	QString guess;
	QString chnlname = channel->name().toLower();
	chnlname.replace(' ','_');
	QString base=snapshotDirName+QDir::separator()+"vp1_"+chnlname+"_run"+QString::number(runnumber)+"_evt"+QString::number(eventnumber);
	guess=base;
	int i(2);
	while (QFile::exists(guess+".png")) {
		guess=base+"_"+QString::number(i++);
	}
	guess+=".png";

	QPixmap pm = channel->getSnapshot();
	if (pm.isNull()) {
		VP1Msg::messageVerbose("pm==NULL, return...");
		return;
	}
	pm.save(guess);
}

//___________________________________________________________________________
void IVP13DStandardChannelWidget::lastOfActiveSystemsRefreshed()
{
  VP1Msg::messageVerbose("IVP13DStandardChannelWidget::lastOfActiveSystemsRefreshed() called.");

  if (m_d->need_initial_viewall) {
    m_d->viewer->viewAll();
    m_d->viewer->storeCameraParametersForReset();
    m_d->need_initial_viewall=false;
  }

  if(VP1QtUtils::environmentVariableIsSet("VP1_SCREENSHOTS_DIR"))
    m_d->autoSnapshot();

  if (m_d->viewer->startTourEachEvent())
    m_d->viewer->startTour();

  // apparently this is the last method called when all systems have been drawn
  // so we call here the renderPixmap() method is we are in "batch-mode"



}

//___________________________________________________________________________
void IVP13DStandardChannelWidget::systemRefreshed(IVP1System*)
{
}

//___________________________________________________________________________
void IVP13DStandardChannelWidget::systemErased(IVP1System*sys)
{
  IVP13DSystem * sys3d = static_cast<IVP13DSystem*>(sys);
  if (sys3d)
    sys3d->deselectAll();
}

//___________________________________________________________________________
void IVP13DStandardChannelWidget::toggleSystemActive()
{
  QCheckBox * cb = static_cast<QCheckBox*>(sender()); assert(cb);
  m_d->updateSystemState(cb);
}


//___________________________________________________________________________
void IVP13DStandardChannelWidget::Imp::updateSystemState(QCheckBox* cb)
{

  assert(checkbox2system.contains(cb));
  IVP13DSystem*sys = static_cast<IVP13DSystem*>(checkbox2system.value(cb)); assert(sys);

  assert(system2rootNodes.contains(sys));
  SoSeparator * sysroot = system2rootNodes.value(sys);
  int ichild = root->findChild(sysroot);
  if (cb->isChecked()) {
    channel->turnOn(sys);
    if (ichild==-1)
      root->addChild(sysroot);
    sysroot->unref();
    if (tabwidget&&sys2tabpage.contains(sys)) {
      int sysindex = systemsAllowedControllers.indexOf(sys);
      if (sysindex>=0) {
	IVP1System*nextsystem(0);
	for (int i=sysindex+1;i<systemsAllowedControllers.count();++i) {
	  //Loop through latter systems with controllers in order and
	  //find the first of those which currently has an active
	  //controller:
	  IVP1System* testsys = systemsAllowedControllers.at(i);
	  if (sys2tabpage.contains(testsys)&&tabwidget->indexOf(sys2tabpage[testsys])>-1) {
	    nextsystem=testsys;
	    break;
	  }
	}
	//put before tab of "nextsystem" (or at the end if no nextsystem):
	int index = (nextsystem?tabwidget->indexOf(sys2tabpage[nextsystem]):99999);
	tabwidget->insertTab(index,sys2tabpage[sys],sys->name());
	int index2 = tabwidget->indexOf(sys2tabpage[sys]);
	if (index2!=-1) {
	  tabwidget->setTabEnabled(index2,true);
	}
      }
    }
  } else {
    //Fixme: if system being turned off has selections, we should deselect!!
    channel->turnOff(sys,false);
    assert(ichild!=-1);
    sysroot->ref();
    root->removeChild(ichild);
    if (tabwidget&&sys2tabpage.contains(sys)) {
      int index = tabwidget->indexOf(sys2tabpage[sys]);
      if (index!=-1) {
	tabwidget->setTabEnabled(index,false);
	tabwidget->removeTab(index);
      }
    }
  }
}

//___________________________________________________________________________
void IVP13DStandardChannelWidget::addSystem(IVP13DSystemSimple* sys, const SystemOptions& options )
{
  addSystem((IVP13DSystem*)sys,options);
}

//___________________________________________________________________________
void IVP13DStandardChannelWidget::stopSpinning()
{
  if (m_d->viewer->isAnimating())
    m_d->viewer->stopAnimating();
}

//___________________________________________________________________________
QPixmap IVP13DStandardChannelWidget::getSnapshot(bool transp, int width, bool batch)
{
	VP1Msg::messageDebug("IVP13DStandardChannelWidget::getSnapshot()  - transparent bkg: "+QString::number(transp)+" , width: "+QString::number(width)+" , batch: "+QString::number(batch));
  //   SoToVRML2Action tovrml2;
  //   tovrml2.apply(m_d->selection);
  //   SoVRMLGroup *newroot = tovrml2.getVRML2SceneGraph();
  //   newroot->ref();
  //   SoOutput out;
  //   out.openFile("out.wrl");
  //   out.setHeaderString("#VRML V2.0 utf8");
  //   SoWriteAction wra(&out);
  //   wra.apply(newroot);
  //   out.closeFile();


  VP1Msg::messageVerbose("checkBox_as_shown: " + QString::number(m_d->uisnapshot.checkBox_as_shown->isChecked()) );
  VP1Msg::messageVerbose("spinBox_width: " + QString::number(m_d->uisnapshot.spinBox_width->value()) );
  VP1Msg::messageVerbose("spinBox_height: " + QString::number(m_d->uisnapshot.spinBox_height->value()) );
  VP1Msg::messageVerbose("checkBox_transp: " + QString::number(m_d->uisnapshot.checkBox_transp->isChecked()) );

  if (m_d->uisnapshot.checkBox_as_shown->isChecked()) {
	  VP1Msg::messageVerbose("'As shown' option checked - using IVP13DChannelWidget::getSnapshot()");
	  return IVP13DChannelWidget::getSnapshot();
  }
  VP1Msg::messageVerbose("'As shown' option NOT checked");

  // if width is set programmatically, we set the new value in the GUI field
  int original_width = 0;
  if (width) {
  	  VP1Msg::messageVerbose("'width' passed at function call. Setting the value of the GUI field to: " + QString::number(width));
  	  original_width = m_d->uisnapshot.spinBox_width->value();
  	  m_d->uisnapshot.spinBox_width->setValue( width );
  }


  // if transparent bkg is requested programmatically, we force it
  if (transp) {
	  VP1Msg::messageVerbose("'transp' option set - using VP1QtInventorUtils::renderToPixmap() with transp set to 'true'");
	  return VP1QtInventorUtils::renderToPixmap(m_d->viewer,
			  m_d->uisnapshot.spinBox_width->value(),
			  m_d->uisnapshot.spinBox_height->value(),
			  true);
  }

  // if changed programatically, set the width back to the original value set by the user
  if (width) m_d->uisnapshot.spinBox_width->setValue( original_width );

  // default, with user's preferences from the GUI
  VP1Msg::messageVerbose("save the screenshot with the default tool (with settings from GUI) - using VP1QtInventorUtils::renderToPixmap()");
  return VP1QtInventorUtils::renderToPixmap(m_d->viewer,
					    m_d->uisnapshot.spinBox_width->value(),
					    m_d->uisnapshot.spinBox_height->value(),
					    m_d->uisnapshot.checkBox_transp->isChecked());

  //return QPixmap(); // for DEBUG

}

//___________________________________________________________________________
void IVP13DStandardChannelWidget::setImageFromPresets()
{
	VP1Msg::messageDebug("IVP13DStandardChannelWidget::setImageFromPresets()");

	disconnect(m_d->uisnapshot.spinBox_width,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
	disconnect(m_d->uisnapshot.spinBox_height,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));

	if (m_d->uisnapshot.radioButton_720p->isChecked()) {
		m_d->uisnapshot.spinBox_width->setValue(1280);
		m_d->uisnapshot.spinBox_height->setValue(720);
	}
	else if (m_d->uisnapshot.radioButton_1080p->isChecked()) {
		m_d->uisnapshot.spinBox_width->setValue(1920);
		m_d->uisnapshot.spinBox_height->setValue(1080);
	}
	else if (m_d->uisnapshot.radioButton_4K->isChecked()) {
		m_d->uisnapshot.spinBox_width->setValue(4096);
		m_d->uisnapshot.spinBox_height->setValue(2160);
	}
	else if (m_d->uisnapshot.radioButton_8K->isChecked()) {
		m_d->uisnapshot.spinBox_width->setValue(8192);
		m_d->uisnapshot.spinBox_height->setValue(4320);
	}
	else {
	VP1Msg::messageDebug("ERROR! Sender unknown!!");
	}

	connect(m_d->uisnapshot.spinBox_width,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
	connect(m_d->uisnapshot.spinBox_height,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));

	// update
	updateSnapshotDim();
}


//___________________________________________________________________________
void IVP13DStandardChannelWidget::updateSnapshotDim()
{
  VP1Msg::messageDebug("IVP13DStandardChannelWidget::updateSnapshotDim()");

  //  if (!m_d->uisnapshot.checkBox_as_shown)
//    return;
//  if (sender()==m_d->uisnapshot.spinBox_width)
//    m_d->uisnapshot.radioButton_width->setChecked(true);
//  else if (sender()==m_d->uisnapshot.spinBox_height)
//    m_d->uisnapshot.radioButton_height->setChecked(true);

  if (!m_d->uisnapshot.checkBox_as_shown)
    return;

  if (sender()==m_d->uisnapshot.spinBox_width)
    m_d->uisnapshot.radioButton_width->setChecked(true);
  else if (sender()==m_d->uisnapshot.spinBox_height)
    m_d->uisnapshot.radioButton_height->setChecked(true);

  /*
   * Get widget width and height
   * Note: the method "getNormalWidget" is from the "SoQtGLWidget" class
   *       and it returns a QWidget object
   */
	int onscreen_width = m_d->viewer->getNormalWidget()->geometry().width();
	int onscreen_height = m_d->viewer->getNormalWidget()->geometry().height();

	if ( m_d->uisnapshot.checkBox_as_shown->isChecked() ) {
		disconnect(m_d->uisnapshot.spinBox_width,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
		disconnect(m_d->uisnapshot.spinBox_height,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
		m_d->uisnapshot.spinBox_width->setValue(onscreen_width);
		m_d->uisnapshot.spinBox_height->setValue(onscreen_height);
		connect(m_d->uisnapshot.spinBox_width,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
		connect(m_d->uisnapshot.spinBox_height,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
	} else {
		if (m_d->uisnapshot.checkBox_lockRatio->isChecked()) {
			VP1Msg::messageDebug("checkBox_lockRatio is checked - updating only rendering size");
			if (m_d->uisnapshot.radioButton_width->isChecked()) {
				disconnect(m_d->uisnapshot.spinBox_height,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
				m_d->uisnapshot.spinBox_height->setValue(static_cast<int>(m_d->uisnapshot.spinBox_width->value()*static_cast<double>(onscreen_height)/onscreen_width+0.5));
				connect(m_d->uisnapshot.spinBox_height,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
			} else {
				disconnect(m_d->uisnapshot.spinBox_width,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
				m_d->uisnapshot.spinBox_width->setValue(static_cast<int>(m_d->uisnapshot.spinBox_height->value()*static_cast<double>(onscreen_width)/onscreen_height+0.5));
				connect(m_d->uisnapshot.spinBox_width,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
			}
		}
		else {
			VP1Msg::messageDebug("checkBox_lockRatio is NOT checked - Updating the widget size as well");

//			if (m_d->uisnapshot.radioButton_width->isChecked() ) {
//				int newWidth =  static_cast<int>(m_d->uisnapshot.spinBox_width->value() * static_cast<double>(onscreen_height) / m_d->uisnapshot.spinBox_height->value() + 0.5);
//				m_d->viewer->getNormalWidget()->resize(newWidth, onscreen_height);
//			} else if (m_d->uisnapshot.radioButton_height->isChecked()) {
//				int newHeight = static_cast<int>( onscreen_width * static_cast<double>(m_d->uisnapshot.spinBox_height->value()) / m_d->uisnapshot.spinBox_width->value() + 0.5);
//				m_d->viewer->resize(onscreen_width, newHeight);
//			}

		}
	}

  //Finally, ensure that we are within the ranges:
  if (m_d->uisnapshot.spinBox_width->value()>=m_d->uisnapshot.spinBox_width->maximum()) {
    disconnect(m_d->uisnapshot.spinBox_width,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
    disconnect(m_d->uisnapshot.spinBox_height,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
    m_d->uisnapshot.spinBox_width->setValue(m_d->uisnapshot.spinBox_width->maximum());
    m_d->uisnapshot.spinBox_height->setValue(static_cast<int>(m_d->uisnapshot.spinBox_width->value()*static_cast<double>(onscreen_height)/onscreen_width+0.5));
    connect(m_d->uisnapshot.spinBox_width,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
    connect(m_d->uisnapshot.spinBox_height,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
  }
  if (m_d->uisnapshot.spinBox_height->value()>=m_d->uisnapshot.spinBox_height->maximum()) {
    disconnect(m_d->uisnapshot.spinBox_width,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
    disconnect(m_d->uisnapshot.spinBox_height,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
    m_d->uisnapshot.spinBox_height->setValue(m_d->uisnapshot.spinBox_height->maximum());
    m_d->uisnapshot.spinBox_width->setValue(static_cast<int>(m_d->uisnapshot.spinBox_height->value()*static_cast<double>(onscreen_width)/onscreen_width+0.5));
    connect(m_d->uisnapshot.spinBox_width,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
    connect(m_d->uisnapshot.spinBox_height,SIGNAL(valueChanged(int)),this,SLOT(updateSnapshotDim()));
  }

}

//___________________________________________________________________________
void IVP13DStandardChannelWidget::dockResized()
{
  VP1Msg::messageVerbose("IVP13DStandardChannelWidget::dockResized()");
  QTimer::singleShot(0, this, SLOT(updateSnapshotDim()));
  IVP1ChannelWidget::dockResized();
}

//___________________________________________________________________________
QByteArray IVP13DStandardChannelWidget::saveState()
{
  VP1Msg::messageVerbose("IVP13DStandardChannelWidget::saveState");


   // NEW VERSION

//
//  // ===> Setup stream writing to a byteArray:
//  QByteArray byteArray;
//  QBuffer buffer(&byteArray);
//  buffer.open(QIODevice::WriteOnly);
//  QDataStream out(&buffer);
//
//  /*
//   * --- GET DATA TO BE SAVED ---
//   */
//
//  //Systems turned on/off:
//  //Fixme: Make sure that if you have two copies of the same system,
//  //that the text in the checkbox gets appended some stuff like [1],
//  //[2], etc., so that the strings used here will be unique.
//  QMap<QString, bool> sysname2turnedon;
//  QMap<QCheckBox*,IVP1System*>::const_iterator it = m_d->checkbox2system.constBegin();
//  while (it != m_d->checkbox2system.constEnd()) {
//    sysname2turnedon.insert(it.key()->text(),it.key()->isChecked());
//     ++it;
//  }
//
//
//  // snapshot settings
//  bool transp_checked = m_d->uisnapshot.checkBox_transp->isChecked();
//  bool widthfixed = m_d->uisnapshot.radioButton_width->isChecked();
//  bool as_shown = m_d->uisnapshot.checkBox_as_shown->isChecked();
//
//  bool locked_ratio = m_d->uisnapshot.checkBox_lockRatio->isChecked();
//
//  qint32 width = (qint32)(m_d->uisnapshot.spinBox_width->value());
//  qint32 height = (qint32)(m_d->uisnapshot.spinBox_height->value());
//
//  // image presets
//  bool isUsingPreset = m_d->uisnapshot.groupBox_imagePresets->isChecked();
//  bool is720p = m_d->uisnapshot.radioButton_720p->isChecked();
//  bool is1080p = m_d->uisnapshot.radioButton_1080p->isChecked();
//  bool is4K = m_d->uisnapshot.radioButton_4K->isChecked();
//  bool is8K = m_d->uisnapshot.radioButton_8K->isChecked();
//
//  // active tab
//  QString tab_index = (m_d->tabwidget ? m_d->tabwidget->tabText(m_d->tabwidget->currentIndex()) : QString("") );
//
//
//  /*
//   * --- WRITE DATA ---
//   */
//
//  // ------- SAVE -------
//  out << (qint32)7; //version
//  out << IVP13DChannelWidget::saveState();//Always include state info from the base class.
//  out << sysname2turnedon;
//  out << ( m_d->extradisplaywidget_splitter ? m_d->extradisplaywidget_splitter->saveState() : QByteArray() );
//  out << m_d->viewer->saveState();
//  out << transp_checked;
//  out << widthfixed;
//  out << as_shown;
//  out << locked_ratio; // new in v. 7
//  out << width;  // new in v. 7
//  out << height; // new in v. 7
//  out << isUsingPreset; // new in v. 7
//  out << is720p; // new in v. 7
//  out << is1080p; // new in v. 7
//  out << is4K; // new in v. 7
//  out << is8K; // new in v. 7
//  out << tab_index;
//  // --------------------
//
//  // ===> Finish up:
//  buffer.close();
//  return byteArray;
//
//



  // OLD VERSION

  // ===> Setup stream writing to a byteArray:
     QByteArray byteArray;
     QBuffer buffer(&byteArray);
     buffer.open(QIODevice::WriteOnly);
     QDataStream out(&buffer);

     // ===> Write Data:

     //Version & base state:
     out << (qint32)6; //version
     out << IVP13DChannelWidget::saveState();//Always include state info from the base class.

     //version 3 had bgd color output here;

     //Systems turned on/off:
     //Fixme: Make sure that if you have two copies of the same system,
     //that the text in the checkbox gets appended some stuff like [1],
     //[2], etc., so that the strings used here will be unique.
     QMap<QString, bool> sysname2turnedon;
     QMap<QCheckBox*,IVP1System*>::const_iterator it = m_d->checkbox2system.constBegin();
     while (it != m_d->checkbox2system.constEnd()) {
       sysname2turnedon.insert(it.key()->text(),it.key()->isChecked());
        ++it;
     }

     //Versions 3+ follow here.

     out << sysname2turnedon;

     out << ( m_d->extradisplaywidget_splitter ? m_d->extradisplaywidget_splitter->saveState() : QByteArray() );

     out << m_d->viewer->saveState();

     //version <=3 had bool here.

     out << m_d->uisnapshot.checkBox_transp->isChecked();
     bool widthfixed = m_d->uisnapshot.radioButton_width->isChecked();
     bool as_shown = m_d->uisnapshot.checkBox_as_shown->isChecked();
     out << widthfixed;
     out << as_shown;
     if (!as_shown)
       out << (qint32)(widthfixed ? m_d->uisnapshot.spinBox_width->value() : m_d->uisnapshot.spinBox_height->value());

     if (m_d->tabwidget)
       out << m_d->tabwidget->tabText(m_d->tabwidget->currentIndex());
     else
       out << QString("");

     //In version 5 we output a bool here.

     // ===> Finish up:
     buffer.close();
     return byteArray;


}

//___________________________________________________________________________
void IVP13DStandardChannelWidget::restoreFromState(QByteArray ba)
{
	VP1Msg::messageVerbose("IVP13DStandardChannelWidget::restoreFromState");

	// ===> Setup stream for getting the contents of the byteArray:
	QBuffer buffer(&ba);
	buffer.open(QIODevice::ReadOnly);
	QDataStream state(&buffer);
	// ===> Check version and pass on state info to base class:
	qint32 version;
	state >> version;

	message("Configuration file version: " + QString::number(version) );

	if (version<0||version>7) {
		message("Warning: State data in .vp1 file is in wrong format - ignoring!");
		return;
	}

	QByteArray basestate;
	state >> basestate;
	IVP13DChannelWidget::restoreFromState(basestate);


	// ===> Decode the state info:

	if (version == 7)
		m_d->restoreFromState_v7(state);

	if (version<=3) {
		QColor bgdcol_dummy;
		state >> bgdcol_dummy;
	}

	//Switch systems on/off:
	QMap<QString, bool> sysname2turnedon;
	state >> sysname2turnedon;
	QMap<QCheckBox*,IVP1System*>::const_iterator it = m_d->checkbox2system.constBegin();
	while (it != m_d->checkbox2system.constEnd()) {
		if (sysname2turnedon.contains(it.key()->text())) {
			if (sysname2turnedon[it.key()->text()]!=it.key()->isChecked())
				it.key()->setChecked(sysname2turnedon[it.key()->text()]);
		} else {
			message("Warning: Config data does not contain information about switched state of subsystem '"+it.key()->text()+"'");
		}
		++it;
	}

	if (version<=2) {
		// We stop here to avoid too messy code.
		buffer.close();
		return;
	}

	//Splitter:
	QByteArray splitstate;
	state >> splitstate;
	if (m_d->extradisplaywidget_splitter)
		m_d->extradisplaywidget_splitter->restoreState(splitstate);

	//Viewer settings:
	QByteArray ba_viewer;
	state >> ba_viewer;
	m_d->viewer->restoreFromState(ba_viewer);//Fixme: reset camera???
	m_d->need_initial_viewall = false;

	//Snapshot parameters:
	bool aa_dummy, transp, widthfixed, as_shown;
	if (version<=3)
		state >> aa_dummy;
	state >> transp; state >> widthfixed; state >> as_shown;
	m_d->uisnapshot.checkBox_transp->setChecked(transp);
	m_d->uisnapshot.checkBox_as_shown->setChecked(as_shown);
	if (widthfixed) {
		m_d->uisnapshot.radioButton_width->setChecked(true);
		m_d->uisnapshot.radioButton_height->setChecked(false);
	} else {
		m_d->uisnapshot.radioButton_width->setChecked(false);
		m_d->uisnapshot.radioButton_height->setChecked(true);
	}
	if (!as_shown) {
		qint32 fixval;
		state >> fixval;
		if (widthfixed)
			m_d->uisnapshot.spinBox_width->setValue(fixval);
		else
			m_d->uisnapshot.spinBox_height->setValue(fixval);
	}

	QString tabname;
	state >> tabname;
	if (m_d->tabwidget) {
		for (int i = 0; i < m_d->tabwidget->count(); ++i) {
			if (m_d->tabwidget->tabText(i) == tabname) {
				m_d->tabwidget->setCurrentIndex(i);
				break;
			}
		}
	}

	if (version==5) {
		bool b;
		state >> b;
	}


	// ===> Finish up:
			buffer.close();
}

//___________________________________________________________________________
void IVP13DStandardChannelWidget::showControlsForSystem(  )
{
  if (!m_d->tabwidget)
    return;
  IVP1System * sys = static_cast<IVP1System*>(sender());
  if (!sys) {
    message("showControlsForSystem Error: Unable to determine system identity.");
    return;
  }
  if (!m_d->sys2tabpage.contains(sys)) {
    //Dont send warning here. The system in question might simply not have a controller!
    return;
  }

  int index = m_d->tabwidget->indexOf(m_d->sys2tabpage[sys]);
  if (index<0||!m_d->tabwidget->isTabEnabled(index)) {
    message("Warning: Asked to show controller for a disabled system. Surely you jest?");
    return;
  }
  m_d->tabwidget->setCurrentIndex(index);
}


//___________________________________________________________________________
void IVP13DStandardChannelWidget::Imp::restoreFromState_v7(QDataStream& state) {

	VP1Msg::messageVerbose("IVP13DStandardChannelWidget::Imp::restoreFromState_v7");

	/*
	 * --- VARIABLES TO BE RESTORED ---
	 */
	//Switch systems on/off:
	QMap<QString, bool> sysname2turnedon;
	//Splitter:
	QByteArray splitstate;
	//Viewer settings:
	QByteArray ba_viewer;
	// Snapshot parameters:
	bool transp, widthfixed, as_shown;
	bool locked_ratio;
	qint32 width, height;
	// image format presets
	bool isUsingPresets;
	bool is720p;
	bool is1080p;
	bool is4K;
	bool is8K;
	// Active tab
	QString tabname;


	/*
	 * ------- GET STATE -------
	 */
	// 'version' restored already
	// 'basestate' restored already
	state >> sysname2turnedon;
	state >> splitstate;
	state >> ba_viewer;
	state >> transp;
	state >> widthfixed;
	state >> as_shown;
	state >> locked_ratio;
	state >> width;
	state >> height;
	state >> isUsingPresets;
	state >> is720p;
	state >> is1080p;
	state >> is4K;
	state >> is8K;
	state >> tabname;
	// -------------------------




	/*
	 * --- RESTORE VARIABLES ---
	 */

	//Switch systems on/off:
	QMap<QCheckBox*,IVP1System*>::const_iterator it = checkbox2system.constBegin();
	while (it != checkbox2system.constEnd()) {
		if (sysname2turnedon.contains(it.key()->text())) {
			if (sysname2turnedon[it.key()->text()]!=it.key()->isChecked())
				it.key()->setChecked(sysname2turnedon[it.key()->text()]);
		} else {
			channel->message("Warning: Config data does not contain information about switched state of subsystem '"+it.key()->text()+"'");
		}
		++it;
	}


	//Splitter:
	if (extradisplaywidget_splitter)
		extradisplaywidget_splitter->restoreState(splitstate);

	//Viewer settings:
	viewer->restoreFromState(ba_viewer);//Fixme: reset camera???
	need_initial_viewall = false;


	// Snapshot parameters - Set Width and Height
	uisnapshot.checkBox_transp->setChecked(transp);
	uisnapshot.checkBox_as_shown->setChecked(as_shown);
	if (widthfixed) {
		uisnapshot.radioButton_width->setChecked(true);
		uisnapshot.radioButton_height->setChecked(false);
	} else {
		uisnapshot.radioButton_width->setChecked(false);
		uisnapshot.radioButton_height->setChecked(true);
	}
	if (!as_shown) {
		uisnapshot.spinBox_width->setValue(width);
		uisnapshot.spinBox_height->setValue(height);
	}

	// image format presets
	if (isUsingPresets)
		uisnapshot.groupBox_imagePresets->setChecked(true);
	if (is720p)
		uisnapshot.radioButton_720p->setChecked(true);
	else if (is1080p)
		uisnapshot.radioButton_1080p->setChecked(true);
	else if (is4K)
		uisnapshot.radioButton_4K->setChecked(true);
	else if (is8K)
		uisnapshot.radioButton_8K->setChecked(true);


	// Set active tab
	if (tabwidget) {
		for (int i = 0; i < tabwidget->count(); ++i) {
			if (tabwidget->tabText(i) == tabname) {
				tabwidget->setCurrentIndex(i);
				break;
			}
		}
	}
}


