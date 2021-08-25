/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local
#include "VP1Base/VP1ColorSelectButton.h"
#include "IParticleCollectionSettingsButton.h"
#include "ui_periparticlecollectionsettings_form.h"
#include "ui_settings_cuts_form.h"

//VP1
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"

//SoCoin
#include "Inventor/nodes/SoDrawStyle.h"
#include "Inventor/nodes/SoLightModel.h"

// Qt
#include <QTimer>
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QShortcut>
#include <QMimeData>
#include <QDrag>

// Misc
#include <iostream>

//____________________________________________________________________
class IParticleCollectionSettingsButton::Imp {
public:
  Imp():theclass(0),editwindow(0),matButton(0), trackDrawStyle(0), lightModel(0),dim(0){}
  IParticleCollectionSettingsButton * theclass;
  QWidget * editwindow;
  Ui::IParticleCollectionSettingsForm editwindow_ui{};
  
  VP1MaterialButton* matButton;
  
  SoDrawStyle * trackDrawStyle;
  SoLightModel * lightModel;
  
  //GUI - last values
  VP1Interval last_cutAllowedPt;
  VP1Interval last_cutAllowedP;
  VP1Interval last_cutAllowedEta;
  QList<VP1Interval> last_cutAllowedPhi;  
  int dim;
  QPoint dragStartPosition;
  
  void initEditWindow();
};

//____________________________________________________________________
void IParticleCollectionSettingsButton::Imp::initEditWindow()
{
	/*
	 * Note: the Eta-Phi widget is inserted
	 * promoting an empty QWidget to VP1EtaPhiCutWidget
	 * directly in the periparticlecollectionsettingsform.ui!!
	 */

  if (editwindow)
    return;
  theclass->messageVerbose("Initialising material editor dialog");
  editwindow = new QWidget(0,Qt::WindowStaysOnTopHint);
  editwindow_ui.setupUi(editwindow);
  matButton = editwindow_ui.pushButton_matButton;  
}

void IParticleCollectionSettingsButton::initEditWindow(){
  m_d->initEditWindow();
}


//____________________________________________________________________
void IParticleCollectionSettingsButton::setText(const QString& t)
{
  if (m_d->editwindow)
    m_d->editwindow->setWindowTitle(t);
  setToolTip(t);
}

//____________________________________________________________________
IParticleCollectionSettingsButton::IParticleCollectionSettingsButton(QWidget * parent,int dim)
  : VP1MaterialButtonBase(parent,0,"VP1MaterialButton"), m_d(new Imp)
{
  m_d->dim = dim;
  
  m_d->theclass = this;
  m_d->initEditWindow();
    
  // -> cutAllowedP/Pt
  connect(m_d->editwindow_ui.checkBox_cut_minpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
  connect(m_d->editwindow_ui.checkBox_cut_maxpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
  connect(m_d->editwindow_ui.doubleSpinBox_cut_minpt_gev,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedPt()));
  connect(m_d->editwindow_ui.doubleSpinBox_cut_maxpt_gev,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedPt()));
  connect(m_d->editwindow_ui.checkBox_cut_minpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
  connect(m_d->editwindow_ui.comboBox_momtype,SIGNAL(currentIndexChanged(int)),this,SLOT(possibleChange_cutAllowedPt()));

  // -> cutAllowedEta
  connect(m_d->editwindow_ui.etaPhiCutWidget,SIGNAL(allowedEtaChanged(const VP1Interval&)),this,SLOT(possibleChange_cutAllowedEta()));
  
  // -> cutAllowedPhi
  connect(m_d->editwindow_ui.etaPhiCutWidget,SIGNAL(allowedPhiChanged(const QList<VP1Interval>&)),this,SLOT(possibleChange_cutAllowedPhi()));
  
  connect(this,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
  connect(m_d->editwindow_ui.pushButton_close,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
  connect(m_d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(updateButton()));
  connect(m_d->matButton,SIGNAL(lastAppliedChanged()),this,SIGNAL(lastAppliedChanged()));
  setAcceptDrops(true);
  
  QTimer::singleShot(0, this, SLOT(updateButton()));
  
}

// QWidget& IParticleCollectionSettingsButton::editWindow() {
//   if (!m_d->editwindow)
//     initEditWindow();
//   return *(m_d->editwindow);
// } 
IParticleCollectionSettingsButton::~IParticleCollectionSettingsButton()
{
  delete m_d->editwindow;
  m_d->trackDrawStyle->unref();
  m_d->lightModel->unref();
  delete m_d;
}

void IParticleCollectionSettingsButton::updateButton()
{
  if (objectName().isEmpty())
    setObjectName("IParticleCollectionSettingsButton");
  messageVerbose("setColButtonProperties: color=" + str(m_d->matButton->lastAppliedDiffuseColour()));
  VP1ColorSelectButton::setColButtonProperties(this,m_d->matButton->lastAppliedDiffuseColour(),m_d->dim);
}

void IParticleCollectionSettingsButton::setDimension(int dim)
{
  if (m_d->dim == dim)
    return;
  m_d->dim = dim;
  updateButton();
}

void IParticleCollectionSettingsButton::showEditMaterialDialog()
{
  if (!m_d->editwindow)
    m_d->initEditWindow();

  if (m_d->editwindow->isHidden())
    m_d->editwindow->show();
  else
    m_d->editwindow->hide();
}

bool IParticleCollectionSettingsButton::setMaterial(SoMaterial*mat)
{  
  if (!m_d->matButton) m_d->initEditWindow();
  m_d->matButton->setMaterial(mat);
  return true;
}

void IParticleCollectionSettingsButton::copyValuesFromMaterial(SoMaterial*mat)
{
  if (!m_d->matButton) m_d->initEditWindow();
  m_d->matButton->setMaterial(mat);
}
double IParticleCollectionSettingsButton::lastAppliedTransparency() const 
{
  if (!m_d->matButton) m_d->initEditWindow();
  return m_d->matButton->lastAppliedTransparency();
}
double IParticleCollectionSettingsButton::lastAppliedShininess() const  
{
  if (!m_d->matButton) m_d->initEditWindow();
  return m_d->matButton->lastAppliedShininess();
}
double IParticleCollectionSettingsButton::lastAppliedBrightness() const
{
  if (!m_d->matButton) m_d->initEditWindow();
  return m_d->matButton->lastAppliedBrightness();
}


void IParticleCollectionSettingsButton::updateDrawStyle()
{
  // double val = VP1QtInventorUtils::getValueLineWidthSlider(m_d->editwindow_ui.horizontalSlider_trackWidth);
  // if (m_d->trackDrawStyle->lineWidth.getValue()!=val)
  //   m_d->trackDrawStyle->lineWidth = val;
}

void IParticleCollectionSettingsButton::updateLightModel(bool base)
{
  if (m_d->lightModel->model.getValue()!=(base?SoLightModel::BASE_COLOR:SoLightModel::PHONG)) {
    messageVerbose("lightModel changed (base = "+str(base));
    if (base)
      m_d->lightModel->model.setValue(SoLightModel::BASE_COLOR);
    else
      m_d->lightModel->model.setValue(SoLightModel::PHONG);
  }
}



SoDrawStyle * IParticleCollectionSettingsButton::drawStyle() const
{
  return m_d->trackDrawStyle;
}

SoLightModel * IParticleCollectionSettingsButton::lightModel() const
{
  return m_d->lightModel;
}


//____________________________________________________________________
void IParticleCollectionSettingsButton::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    m_d->dragStartPosition = event->pos();
  QPushButton::mousePressEvent(event);
}

//____________________________________________________________________
void IParticleCollectionSettingsButton::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->source()!=this && event->mimeData()->hasFormat("vp1/material"))
    event->acceptProposedAction();
}

//____________________________________________________________________
void IParticleCollectionSettingsButton::mouseMoveEvent(QMouseEvent *event)
{
  if (!(event->buttons() & Qt::LeftButton))
    return;
  if ((event->pos() - m_d->dragStartPosition).manhattanLength()
      < QApplication::startDragDistance())
    return;

  QDrag *drag = new QDrag(this);
  QMimeData *mimeData = new QMimeData;

  /////////////////////////////////////////////////////
  // For dragging state onto other material buttons: //
  /////////////////////////////////////////////////////

  QByteArray byteArray = saveState();
    
  // buffer.close();
  mimeData->setData("vp1/material", byteArray);

  // ////////////////////////////////////////////////////////
  // //  For dragging c++ code for defining this material  //
  // //  into e.g. an editor:                              //
  // ////////////////////////////////////////////////////////
  // 
  // QString s = "SoMaterial * mat = new SoMaterial;\n";
  // QString str_ambient = m_d->toSbColTxt(m_d->lastapplied_ambient);
  // if (str_ambient!="SbColor(0.2,0.2,0.2)")
  //   s += "mat->ambientColor.setValue("+str_ambient+");\n";
  // QString str_diffuse = m_d->toSbColTxt(m_d->lastapplied_diffuse);
  // if (str_diffuse!="SbColor(0.8,0.8,0.8)")
  //   s += "mat->diffuseColor.setValue("+str_diffuse+");\n";
  // QString str_specular = m_d->toSbColTxt(m_d->lastapplied_specular);
  // if (str_specular!="SbColor(0,0,0)")
  //   s += "mat->specularColor.setValue("+str_specular+");\n";
  // QString str_emissive = m_d->toSbColTxt(m_d->lastapplied_emissive);
  // if (str_emissive!="SbColor(0,0,0)")
  //   s += "mat->emissiveColor.setValue("+str_emissive+");\n";
  // QString str_shininess = m_d->printFloat(m_d->lastapplied_shininess/100.0);
  // if (str_shininess!="0.2")
  //   s +=     "mat->shininess.setValue("+str_shininess+");\n";
  // QString str_transparency = m_d->printFloat(m_d->lastapplied_transparency/100.0);
  // if (str_transparency!="0")
  //   s +=     "mat->transparency.setValue("+str_transparency+");\n";
  // mimeData->setText(s);

  //Execute drag:
  drag->setMimeData(mimeData);//drag assumes ownership of mimeData
  drag->exec(Qt::CopyAction | Qt::MoveAction);
}

//____________________________________________________________________
void IParticleCollectionSettingsButton::dropEvent(QDropEvent *event)
{
  QByteArray data = event->mimeData()->data("vp1/material");
  event->acceptProposedAction();
  restoreFromState(data);  
}


QByteArray IParticleCollectionSettingsButton::saveState() const{
  // messageVerbose("getState");
  // if (m_d->editwindow_ui.checkBox_tracksUseBaseLightModel->isChecked()) messageVerbose("checked!");
  VP1Serialise serialise(1/*version*/);
  
  serialise.save(m_d->matButton);  
  // serialise.disableUnsavedChecks();
	// FIXME - what about eta/phi?
	
  serialise.widgetHandled(this);
  serialise.warnUnsaved(this);
  return serialise.result();
}

void IParticleCollectionSettingsButton::restoreFromState( const QByteArray& ba){
   
  VP1Deserialise state(ba,systemBase());
  if (state.version()<0||state.version()>1)
    return;//Ignore silently
  state.restore(m_d->matButton);
  state.widgetHandled(this);
  state.warnUnrestored(this);
  updateButton();
  //FIXME - anything else need updating?
}

//____________________________________________________________________
VP1Interval IParticleCollectionSettingsButton::cutAllowedPt() const
{
  if (!m_d->editwindow_ui.checkBox_cut_minpt)
    return VP1Interval();

  // will set range to negative if we have momcut=P
  // if minCut unset then min=-inf
  // if minCut set, and Pt selected, then min=-minCut
  // if minCut set, and P selected, then min=-maxCut
  // etc
  bool isPCut = m_d->editwindow_ui.comboBox_momtype->currentText()=="P";
  
  const double minFromInterface=m_d->editwindow_ui.doubleSpinBox_cut_minpt_gev->value()*1000;
  const double maxFromInterface=m_d->editwindow_ui.doubleSpinBox_cut_maxpt_gev->value()*1000;
  
  double min=0.0,max=0.0;
  if (!isPCut) {
    //Pt cut
    min = (m_d->editwindow_ui.checkBox_cut_minpt->isChecked() ? minFromInterface : -std::numeric_limits<double>::infinity());
    max = (m_d->editwindow_ui.checkBox_cut_maxpt->isChecked() ? maxFromInterface : std::numeric_limits<double>::infinity());
  } else {
    min = (m_d->editwindow_ui.checkBox_cut_maxpt->isChecked() ? -maxFromInterface : -std::numeric_limits<double>::infinity());
    max = (m_d->editwindow_ui.checkBox_cut_minpt->isChecked() ? -minFromInterface : std::numeric_limits<double>::infinity());
  }
  
  //message("cutAllowedPt: min,max="+QString::number(min)+","+QString::number(max));
  
  if (max<min)
    return VP1Interval();
    
  return VP1Interval( min, max );//fixme: closed interval??
}

//____________________________________________________________________
VP1Interval IParticleCollectionSettingsButton::cutAllowedEta() const
{
  return m_d->editwindow_ui.etaPhiCutWidget->allowedEta();
}

//____________________________________________________________________
QList<VP1Interval> IParticleCollectionSettingsButton::cutAllowedPhi() const
{
  return m_d->editwindow_ui.etaPhiCutWidget->allowedPhi();
}

void IParticleCollectionSettingsButton::possibleChange_cutAllowedPt()
{
  messageVerbose("IParticleCollectionSettingsButton::possibleChange_cutAllowedPt() ");
  
  if (m_d->last_cutAllowedPt==cutAllowedPt()) return;
  messageVerbose("cutAllowedPt() changed");
  m_d->last_cutAllowedPt = cutAllowedPt();
  emit cutAllowedPtChanged(m_d->last_cutAllowedPt);
}

void IParticleCollectionSettingsButton::possibleChange_cutAllowedEta()
{
  if (m_d->last_cutAllowedEta==cutAllowedEta()) return;
  messageVerbose("cutAllowedEta() changed");
  m_d->last_cutAllowedEta=cutAllowedEta();
  emit cutAllowedEtaChanged(m_d->last_cutAllowedEta);
}

void IParticleCollectionSettingsButton::possibleChange_cutAllowedPhi()
{
  if (m_d->last_cutAllowedPhi==cutAllowedPhi()) return;
  messageVerbose("cutAllowedPhi() changed");
  m_d->last_cutAllowedPhi=cutAllowedPhi();
  emit cutAllowedPhiChanged(m_d->last_cutAllowedPhi);
}



