/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1Controller                     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: August 2008                              //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Base/VP1Controller.h"
#include "VP1Base/VP1CollectionWidget.h"
#include "VP1Base/IVP1System.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1ColorSelectButton.h"
#include "VP1Base/VP1MaterialButton.h"
#include "VP1Base/CloseEventEater.h"

#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QSlider>
#include <QScrollArea>
#include <QScrollBar>
#include <QShortcut>
#include <QButtonGroup>
#include <QTimer>

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/SbColor4f.h>
#include <Inventor/nodes/SoMaterial.h>

VP1CustomTourEditor* VP1Controller::m_customTourEditor=0;


//____________________________________________________________________
class VP1Controller::Imp {
public:

  const char * lastUpdateSlot;
  VP1CollectionWidget* collWidget;
  class DialogInfo {
  public:
    DialogInfo(QPushButton * lb,QWidget * dw,QAbstractButton * ec)
      : enabledButton(ec), dialogWidget(dw), launchButton(lb), neverShown(true) {}
    ~DialogInfo() { delete dialogWidget; }
    QAbstractButton * enabledButton;
    QWidget * dialogWidget;
    QPushButton * launchButton;
    bool neverShown;
  };

  QList<DialogInfo *> dialogs;

  SoMaterial * fallBackMaterial;
};

//____________________________________________________________________
void VP1Controller::initLastVars()
{
  m_initVarsMode = true;
  testForChanges();
  m_initVarsMode = false;
}

//____________________________________________________________________
VP1Controller::VP1Controller(IVP1System * sys, const QString& classname )
  : QWidget(0),//Controllers always created parentless.
    VP1HelperClassBase(sys,classname),
    m_d(new Imp),
    m_initVarsMode(true)//Start as true in case any possible_change
			//stuff are called before intLastVars()?
{
  m_d->fallBackMaterial = 0;
  m_d->lastUpdateSlot = 0;
  m_d->collWidget = 0;
  setObjectName(classname);
}

//____________________________________________________________________
VP1Controller::~VP1Controller()
{
  foreach (Imp::DialogInfo* di,m_d->dialogs)
    delete di;
  if (m_d->fallBackMaterial)
    m_d->fallBackMaterial->unref();
  delete m_d;
}

//____________________________________________________________________
QString VP1Controller::toString( const bool& par )
{
  return par?"On":"Off";
}

//____________________________________________________________________
QString VP1Controller::toString( const QList<VP1Interval>& l )
{
  QString s;
  for(int i=0;i<l.count();++i)
    s+= l.at(i).toString()+(i==l.count()-1?"":", ");
  return s;
}

//____________________________________________________________________
QString VP1Controller::toString( SbColor4f p )
{
  float r, g, b, a;
  p.getValue(r, g, b, a);
  return "("+str(r)+", "+str(g)+", "+str(b)+", "+str(1-a)+")";
}

//____________________________________________________________________
void VP1Controller::initDialog(QWidget * dialog, QPushButton* launchButton, QPushButton* closeButton,QAbstractButton* enabledButton)
{
  QString txt(enabledButton?enabledButton->text():launchButton->text());
  txt.replace("&&","&");
  dialog->setWindowTitle(txt+" ["+systemBase()->name()+"]");
  dialog->setWindowIcon(QIcon(QString(":/vp1/icons/icons/3d_32x32.png")));
  launchButton->setMaximumHeight(static_cast<int>(0.5+QFontMetricsF(launchButton->font()).height()*1.05+2));
  launchButton->setMinimumHeight(launchButton->maximumHeight());
  launchButton->setCheckable(true);

  QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(launchButton->sizePolicy().hasHeightForWidth());
  launchButton->setSizePolicy(sizePolicy);
  launchButton->setFocusPolicy(Qt::NoFocus);

  if (enabledButton)
    launchButton->setEnabled(enabledButton->isChecked());

  m_d->dialogs << new Imp::DialogInfo(launchButton,dialog,enabledButton);

  connect(launchButton,SIGNAL(clicked()),this,SLOT(toggleDialogState()));
  if (enabledButton)
    connect(enabledButton,SIGNAL(toggled(bool)),this,SLOT(enabledButtonStateChanged()));
  CloseEventEater* closeEventEater = new CloseEventEater(this);
  dialog->installEventFilter(closeEventEater);

  if(closeButton) {
    closeButton->setText("&Close");
    closeButton->setToolTip("Close this dialog [Esc]");
    connect(closeButton,SIGNAL(clicked()),dialog,SLOT(close()));
  }
  connect(new QShortcut(QKeySequence(Qt::Key_Escape), dialog),SIGNAL(activated()),dialog,SLOT(close()));

  //To avoid an extra call in the code we do this a few times more than necessary (fixme: put in initLastVars!!):
  int maxwidth(0);
  foreach (Imp::DialogInfo* di,m_d->dialogs)
    maxwidth = std::max(maxwidth,di->launchButton->sizeHint().width());
  foreach (Imp::DialogInfo* di,m_d->dialogs)
    di->launchButton->setMinimumWidth(maxwidth);
}

//____________________________________________________________________
void VP1Controller::enabledButtonStateChanged()
{
  Imp::DialogInfo * di(0);
  foreach (Imp::DialogInfo* di2,m_d->dialogs) {
    if (di2->enabledButton==sender()) {
      di = di2;
      break;
    }
  }
  if (!di) {
    message("ERROR: enabledButtonStateChanged() triggered with unrecognised sender() ="
	    +QString(sender()?sender()->objectName():"NULL"));
    return;
  }

  bool on(di->enabledButton->isChecked());
  if (di->launchButton->isEnabled()!=on)
    di->launchButton->setEnabled(on);
  if (!on) {
    if (di->dialogWidget->isVisible())
      di->dialogWidget->setVisible(false);
    if (di->dialogWidget->windowState()==Qt::WindowMinimized)
      di->dialogWidget->setWindowState(Qt::WindowNoState);
  }
}

//____________________________________________________________________
void VP1Controller::toggleDialogState(QObject* widget)
{
  Imp::DialogInfo * di(0);
  foreach (Imp::DialogInfo* di2,m_d->dialogs) {
    if (di2->launchButton==sender() || (widget && di2->dialogWidget==widget)) {
      di = di2;
      break;
    }
  }

  if (!di) {
    message("ERROR: toggleDialogState() triggered with unrecognised sender() ="
	    +QString(sender()?sender()->objectName():"NULL"));
    return;
  }

  bool oldvis = di->dialogWidget->isVisible();

  if (oldvis&&di->dialogWidget->windowState()==Qt::WindowMinimized) {
    di->dialogWidget->setWindowState(Qt::WindowNoState);
  } else {
    di->dialogWidget->setVisible(!oldvis);
    if (!oldvis && di->neverShown) {
      di->neverShown = false;
      //do stuff
      // 	  QSize size(955, 717);
      // 	  size = size.expandedTo(VP1MainWindow->minimumSizeHint());
      // di->dialogWidget->resize(di->dialogWidget->minimumSizeHint());
    }
  }

  //synchronise toggle button state (for example if this was called by CloseEventEater)
  if (di->launchButton->isCheckable() && oldvis){
    di->launchButton->setChecked(false);
  }
}

//____________________________________________________________________
void VP1Controller::collWidgetContentChanged()
{
  if (sender()!=m_d->collWidget||!m_d->collWidget) {
    message("ERROR: Unexpected signal in collWidgetContentChanged slot!!");
    return;
  }
  QScrollArea* scrollarea = dynamic_cast<QScrollArea*>(m_d->collWidget->parent());
  if (!scrollarea&&m_d->collWidget->parent())
    scrollarea = dynamic_cast<QScrollArea*>(m_d->collWidget->parent()->parent());
  if (!scrollarea) {
    message("ERROR: m_d->collWidget not child or grandchild of a scroll area!!");
    return;
  }
  int width = m_d->collWidget->appropriateFixedWidth();
  m_d->collWidget->setMinimumWidth(width);
  m_d->collWidget->setMaximumWidth(width);
  QScrollBar * sb = scrollarea->verticalScrollBar();
  if (sb)
    width+=sb->sizeHint().width();
  scrollarea->setMinimumWidth(width+2);//2 is just some margin
}

//____________________________________________________________________
void VP1Controller::setupCollWidgetInScrollArea(QScrollArea * scrollarea, VP1CollectionWidget* collWidget)
{
  if  (!scrollarea || !collWidget) {
    message("ERROR: NULL scrollarea or collWidget");
    return;
  }
  QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(scrollarea->sizePolicy().verticalStretch());
  sizePolicy.setHeightForWidth(scrollarea->sizePolicy().hasHeightForWidth());
  scrollarea->setSizePolicy(sizePolicy);

  collWidget->setContentsMargins(0,0,0,0);
  if (!collWidget->children().isEmpty()&&collWidget->children().at(0)->isWidgetType()) {
    static_cast<QWidget*>(collWidget->children().at(0))->setContentsMargins(0,0,0,0);
  }
  scrollarea->setContentsMargins(0,0,0,0);

  scrollarea->setWidgetResizable(true);
  scrollarea->setFrameShape(QFrame::NoFrame);
  scrollarea->setFrameShadow(QFrame::Plain);
  collWidget->setParent(scrollarea);
  scrollarea->setWidget(collWidget);
  scrollarea->viewport()->setContentsMargins(0,0,0,0);
  scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  collWidget->setObjectName(objectName()+"_CollWidget");
  scrollarea->setObjectName(objectName()+"_CollWidgetScrollArea");
  m_d->collWidget = collWidget;
  connect(collWidget,SIGNAL(visibleContentsChanged()),this,SLOT(collWidgetContentChanged()));
}


//____________________________________________________________________
QButtonGroup * VP1Controller::defineRadioButtonGroup( QRadioButton * rb0, QRadioButton * rb1,QRadioButton * rb2,
						      QRadioButton * rb3,QRadioButton * rb4,QRadioButton * rb5,
						      QRadioButton * rb6,QRadioButton * rb7,QRadioButton * rb8,
						      QRadioButton * rb9 )
{
  QList<QRadioButton *> l;
  l << rb0 << rb1 << rb2 << rb3 << rb4 << rb5 << rb6 << rb7 << rb8 << rb9;
  l.removeAll(0);
  return defineRadioButtonGroup(l);
}

//____________________________________________________________________
QButtonGroup * VP1Controller::defineRadioButtonGroup( QList<QRadioButton *> l )
{
  QRadioButton * rb_checked(0);
  foreach(QRadioButton *rb,l) {
    if (rb->isChecked()) {
      rb_checked = rb;
      break;
    }
  }
  QButtonGroup * radiogroup = new QButtonGroup(this);
  foreach(QRadioButton *rb,l)
    radiogroup->addButton(rb);
  if (rb_checked)
    rb_checked->setChecked(true);
  return radiogroup;
}

//____________________________________________________________________
const char * VP1Controller::addUpdateSlot(const char * slot)
{
  messageVerbose("Adding update slot: "+QString(slot));
  connect(this,SIGNAL(dummyUpdateTrigger()),this,slot);
  m_d->lastUpdateSlot = slot;
  return slot;
}

//____________________________________________________________________
void VP1Controller::connectToLastUpdateSlot(QObject* sender,const char * signal)
{
  connect(sender,signal,this,m_d->lastUpdateSlot);
}

//____________________________________________________________________
void VP1Controller::connectToLastUpdateSlot(QCheckBox* cb)
{
  connectToLastUpdateSlot(cb,SIGNAL(toggled(bool)));
}

//____________________________________________________________________
void VP1Controller::connectToLastUpdateSlot(QComboBox* cb)
{
  connectToLastUpdateSlot(cb,SIGNAL(currentIndexChanged(int)));
}

//____________________________________________________________________
void VP1Controller::connectToLastUpdateSlot(QGroupBox* gb)
{
  connectToLastUpdateSlot(gb,SIGNAL(toggled(bool)));
}

//____________________________________________________________________
void VP1Controller::connectToLastUpdateSlot(QSpinBox*sb)
{
  connectToLastUpdateSlot(sb,SIGNAL(valueChanged(int)));
}

//____________________________________________________________________
void VP1Controller::connectToLastUpdateSlot(QDoubleSpinBox*dsb)
{
  connectToLastUpdateSlot(dsb,SIGNAL(valueChanged(double)));
}

//____________________________________________________________________
void VP1Controller::connectToLastUpdateSlot(QRadioButton*rb)
{
  connectToLastUpdateSlot(rb,SIGNAL(toggled(bool)));
}

//____________________________________________________________________
void VP1Controller::connectToLastUpdateSlot(QButtonGroup* bg)
{
  connectToLastUpdateSlot(bg,SIGNAL(buttonClicked(int)));
}

//____________________________________________________________________
void VP1Controller::connectToLastUpdateSlot(QSlider*s)
{
  connectToLastUpdateSlot(s,SIGNAL(valueChanged(int)));
}

//____________________________________________________________________
void VP1Controller::connectToLastUpdateSlot(VP1ColorSelectButton* csb)
{
  connectToLastUpdateSlot(csb,SIGNAL(colorChanged(const QColor&)));
}

//____________________________________________________________________
void VP1Controller::restoreSettings(QByteArray ba)
{
	messageDebug("VP1Controller::restoreSettings()");

  VP1Deserialise s(ba,systemBase());
  actualRestoreSettings(s);

  //warn unrestored:
  if (m_d->collWidget)
    s.ignoreWidget(m_d->collWidget);
  s.warnUnrestored((QWidget*)(this));
  foreach (Imp::DialogInfo* di,m_d->dialogs)
    s.warnUnrestored(di->dialogWidget);

  QTimer::singleShot(0, this, SLOT(testForChanges()));
}

//____________________________________________________________________
QByteArray VP1Controller::saveSettings() const
{
	messageDebug("VP1Controller::saveSettings()");

  VP1Serialise s(currentSettingsVersion(),systemBase());

  messageDebug("calling actualSaveSettings()...");
  actualSaveSettings(s);

  //Warn unsaved:
  if (m_d->collWidget)
    s.ignoreWidget(m_d->collWidget);

  s.warnUnsaved((QWidget*)(this));
  foreach (Imp::DialogInfo* di,m_d->dialogs)
    s.warnUnsaved(di->dialogWidget);

  return s.result();

}

//____________________________________________________________________
SoMaterial * VP1Controller::fallBackMaterial() const
{
  if (!m_d->fallBackMaterial) {
    m_d->fallBackMaterial = new SoMaterial;
    m_d->fallBackMaterial->ref();
  }
  return m_d->fallBackMaterial;
}

//____________________________________________________________________
SoMaterial * VP1Controller::getMaterial(VP1MaterialButton*mb) const
{
  if (!mb) {
    message("ERROR: Null material button pointer");
    return fallBackMaterial();
  }
  QList<SoMaterial*> mats = mb->handledMaterials();
  if (mats.count()!=1) {
    message("ERROR: No material in button "+mb->objectName());
    return fallBackMaterial();
  }
  return mats.at(0);


}
