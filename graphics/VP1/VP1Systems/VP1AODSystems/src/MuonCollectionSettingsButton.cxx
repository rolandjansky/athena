/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local
#include "MuonCollectionSettingsButton.h"
#include "VP1AODSystems/AODSystemController.h"
#include "ui_permuoncollectionsettings_form.h"
#include "ui_settings_cuts_form.h"

//VP1
#include "VP1Base/VP1ColorSelectButton.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"

//SoCoin

#include "Inventor/nodes/SoDrawStyle.h"
#include "Inventor/nodes/SoLightModel.h"
#include "Inventor/nodes/SoMaterial.h"

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
class MuonCollectionSettingsButton::Imp {
public:
  Imp():theclass(0),editwindow(0),matButton(0), trackDrawStyle(0), trackLightModel(0),
  last_trackTubeRadius(0.0), last_minimumQuality(0), dim(0){
    //nop
  }
  MuonCollectionSettingsButton * theclass;
  QWidget * editwindow;
  Ui::MuonCollectionSettingsForm editwindow_ui;
  
  VP1MaterialButton* matButton;// main collection colour
  // VP1MaterialButton* defaultParametersMatButton;
  
  // QList<VP1MaterialButton*> parameterTypeMatButtons;// enum used to access the specific button
  
  SoDrawStyle * trackDrawStyle;
  SoLightModel * trackLightModel;
  
  //GUI - last values
  double last_trackTubeRadius;
  VP1Interval last_cutAllowedPt;
  VP1Interval last_cutAllowedP;
  VP1Interval last_cutAllowedEta;
  QList<VP1Interval> last_cutAllowedPhi;
  unsigned int last_minimumQuality;
  ShownAssociatedObjects last_shownAssociatedObjects;
  
  int dim;
  QPoint dragStartPosition;
  
  void initEditWindow();
};

//____________________________________________________________________
void MuonCollectionSettingsButton::Imp::initEditWindow()
{
  if (editwindow)
    return;
  theclass->messageVerbose("Initialising material editor dialog");
  editwindow = new QWidget(0,Qt::WindowStaysOnTopHint);
  editwindow_ui.setupUi(editwindow);
  matButton = editwindow_ui.pushButton_matButton;  
}

//____________________________________________________________________
void MuonCollectionSettingsButton::setMaterialText(const QString& t)
{
  if (m_d->editwindow)
    m_d->editwindow->setWindowTitle(t);
  setToolTip(t);
}

//____________________________________________________________________
MuonCollectionSettingsButton::MuonCollectionSettingsButton(QWidget * parent,int dim)
  : VP1MaterialButtonBase(parent,0,"VP1MaterialButton"), m_d(new Imp)
{
  m_d->dim = dim;
  
  m_d->theclass = this;
  m_d->initEditWindow();
  
  //Draw Styles / Complexity:
  VP1QtInventorUtils::setLimitsLineWidthSlider(m_d->editwindow_ui.horizontalSlider_trackWidth);
  VP1QtInventorUtils::setValueLineWidthSlider(m_d->editwindow_ui.horizontalSlider_trackWidth,1.0);  
  
  m_d->trackDrawStyle = new SoDrawStyle;
  m_d->trackDrawStyle->setName("MuonDrawStyle");
  m_d->trackDrawStyle->pointSize=5.0;
  m_d->trackDrawStyle->ref();
  updateTrackDrawStyle();
  connect(m_d->editwindow_ui.horizontalSlider_trackWidth,SIGNAL(valueChanged(int)),this,SLOT(updateTrackDrawStyle()));
  
  m_d->trackLightModel = new SoLightModel;
  m_d->trackLightModel->setName("MuonLightModel");
  m_d->trackLightModel->ref();
  updateTrackLightModel(false);
  connect(m_d->editwindow_ui.checkBox_tracksUseBaseLightModel,SIGNAL(toggled(bool)),this,SLOT(updateTrackLightModel(bool)));
  
  m_d->last_trackTubeRadius=trackTubeRadius();
  connect(m_d->editwindow_ui.checkBox_trackTubes,SIGNAL(toggled(bool)),this,SLOT(updateTrackTubeRadius()));
  connect(m_d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM,SIGNAL(valueChanged(double)),this,SLOT(updateTrackTubeRadius()));
  
  connect(m_d->editwindow_ui.checkBox_hideactualpaths,SIGNAL(toggled(bool)),this,SLOT(updateHideActualTrackPath(bool)));

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
 
  // -> quality
  connect(m_d->editwindow_ui.comboBox_minimumQuality,SIGNAL(currentIndexChanged(int)),this,SLOT(possibleChange_minimumQuality()));
  m_d->last_minimumQuality=minimumQuality();
 
  // -> shownObjects
  connect(m_d->editwindow_ui.checkBox_showTrackParticles,SIGNAL(toggled(bool)),this,SLOT(possibleChange_shownAssociatedObjects()));
  connect(m_d->editwindow_ui.comboBox_shownTrackParticles,SIGNAL(currentIndexChanged(int)),this,SLOT(possibleChange_shownAssociatedObjects()));
  m_d->last_shownAssociatedObjects=shownAssociatedObjects();
  
  connect(this,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
  connect(m_d->editwindow_ui.pushButton_close,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
  connect(m_d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(updateButton()));
  connect(m_d->matButton,SIGNAL(lastAppliedChanged()),this,SIGNAL(lastAppliedChanged()));
  setAcceptDrops(true);
  
  QTimer::singleShot(0, this, SLOT(updateButton()));
  
}

// QWidget& MuonCollectionSettingsButton::editWindow() {
//   if (!m_d->editwindow)
//     initEditWindow();
//   return *(m_d->editwindow);
// } 
MuonCollectionSettingsButton::~MuonCollectionSettingsButton()
{
  delete m_d->editwindow;
  m_d->trackDrawStyle->unref();
  m_d->trackLightModel->unref();
  delete m_d;
}

void MuonCollectionSettingsButton::updateButton()
{
  if (objectName().isEmpty())
    setObjectName("MuonCollectionSettingsButton");
  messageVerbose("setColButtonProperties: color=" + str(m_d->matButton->lastAppliedDiffuseColour()));
  VP1ColorSelectButton::setColButtonProperties(this,m_d->matButton->lastAppliedDiffuseColour(),m_d->dim);
}

void MuonCollectionSettingsButton::setDimension(int dim)
{
  if (m_d->dim == dim)
    return;
  m_d->dim = dim;
  updateButton();
}

void MuonCollectionSettingsButton::showEditMaterialDialog()
{
  if (!m_d->editwindow)
    m_d->initEditWindow();

  if (m_d->editwindow->isHidden())
    m_d->editwindow->show();
  else
    m_d->editwindow->hide();
}

bool MuonCollectionSettingsButton::setMaterial(SoMaterial*mat)
{  
	// std::cout<<"MuonCollectionSettingsButton::setMaterial with mat="<<mat<<std::endl;
  if (!m_d->matButton) m_d->initEditWindow();
  m_d->matButton->setMaterial(mat);
  return true;
}

void MuonCollectionSettingsButton::copyValuesFromMaterial(SoMaterial*mat)
{
  if (!m_d->matButton) m_d->initEditWindow();
  m_d->matButton->setMaterial(mat);
}
double MuonCollectionSettingsButton::lastAppliedTransparency() const 
{
  if (!m_d->matButton) m_d->initEditWindow();
  return m_d->matButton->lastAppliedTransparency();
}
double MuonCollectionSettingsButton::lastAppliedShininess() const  
{
  if (!m_d->matButton) m_d->initEditWindow();
  return m_d->matButton->lastAppliedShininess();
}
double MuonCollectionSettingsButton::lastAppliedBrightness() const
{
  if (!m_d->matButton) m_d->initEditWindow();
  return m_d->matButton->lastAppliedBrightness();
}

double MuonCollectionSettingsButton::trackTubeRadius() const
{
  return m_d->editwindow_ui.checkBox_trackTubes->isChecked() ?
    m_d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM->value() : 0.0;
}

void MuonCollectionSettingsButton::updateTrackTubeRadius()
{
  if (m_d->last_trackTubeRadius==trackTubeRadius()) return;
  m_d->last_trackTubeRadius=trackTubeRadius();
  messageVerbose("TrackTubeRadius changed to "+str(m_d->last_trackTubeRadius));
  emit trackTubeRadiusChanged(m_d->last_trackTubeRadius);
  return;
}

void MuonCollectionSettingsButton::updateTrackDrawStyle()
{
  double val = VP1QtInventorUtils::getValueLineWidthSlider(m_d->editwindow_ui.horizontalSlider_trackWidth);
  if (m_d->trackDrawStyle->lineWidth.getValue()!=val)
    m_d->trackDrawStyle->lineWidth = val;
}

void MuonCollectionSettingsButton::updateTrackLightModel(bool base)
{
  if (m_d->trackLightModel->model.getValue()!=(base?SoLightModel::BASE_COLOR:SoLightModel::PHONG)) {
    messageVerbose("TrackLightModel changed (base = "+str(base));
    if (base)
      m_d->trackLightModel->model.setValue(SoLightModel::BASE_COLOR);
    else
      m_d->trackLightModel->model.setValue(SoLightModel::PHONG);
  }
}

void MuonCollectionSettingsButton::updateHideActualTrackPath(bool hide)
{
  emit hideActualTrackPathChanged(hide);
}


SoDrawStyle * MuonCollectionSettingsButton::trackDrawStyle() const
{
  return m_d->trackDrawStyle;
}

SoLightModel * MuonCollectionSettingsButton::trackLightModel() const
{
  return m_d->trackLightModel;
}

SoMaterial* MuonCollectionSettingsButton::defaultParameterMaterial() const {
  // return m_d->defaultParametersMatButton->handledMaterials().at(0);// Only have one material per button.
  return 0;
}

SoMaterial* MuonCollectionSettingsButton::parameterMaterial( xAOD::ParameterPosition /**position*/ ) const{
  // return (m_d->parameterTypeMatButtons.at(static_cast<unsigned int>(position)))->handledMaterials().at(0);// Only have one material per button.
  return 0;
}

// void setDefaultParameterMaterial(SoMaterial* mat) {    
//     // m_d->defaultParametersMatButton->setObjectName("matButton_default");
//     // m_d->defaultParametersMatButton->setToolTip(matButtonToolTip());
//     m_d->defaultParametersMatButton->setMaterial(mat);
//     // connect(m_d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(possibleChangeMatTranspOrBrightness()));
// }
// 
// void MuonCollectionSettingsButton::setParameterMaterial(SoMaterial*, xAOD::ParameterPosition){
//   // FIXME!
// }

unsigned int MuonCollectionSettingsButton::minimumQuality() const
{
  if (!m_d->editwindow)
    m_d->initEditWindow();
  
  QString text = m_d->editwindow_ui.comboBox_minimumQuality->currentText();
  if (text=="Very Loose") return static_cast<unsigned int>(xAOD::Muon::VeryLoose);
  if (text=="Loose")  return static_cast<unsigned int>(xAOD::Muon::Loose);
  if (text=="Medium") return static_cast<unsigned int>(xAOD::Muon::Medium);
  if (text=="Tight")  return static_cast<unsigned int>(xAOD::Muon::Tight);

  message("ERROR! MuonCollectionSettingsButton::minimumQuality - unknown value! Returning 'Very Loose'.");
  return static_cast<unsigned int>(xAOD::Muon::VeryLoose);
}

MuonCollectionSettingsButton::ShownAssociatedObjects MuonCollectionSettingsButton::shownAssociatedObjects() const 
{
  if (!m_d->editwindow)
    m_d->initEditWindow();
  ShownAssociatedObjects parts = ShownAssociatedObject::Nothing;
  // if (!m_d->ui_col.checkBox_hideactualpaths->isChecked()) parts |= TrackCommonFlags::ActualPath;
  if (m_d->editwindow_ui.checkBox_showTrackParticles->isChecked()) {
    if (m_d->editwindow_ui.comboBox_shownTrackParticles->currentIndex()==0) parts |= ShownAssociatedObject::TrackParticlesPrimary;
    if (m_d->editwindow_ui.comboBox_shownTrackParticles->currentIndex()==1) parts |= ShownAssociatedObject::TrackParticlesCB;
    if (m_d->editwindow_ui.comboBox_shownTrackParticles->currentIndex()==2) parts |= ShownAssociatedObject::TrackParticlesID;
    if (m_d->editwindow_ui.comboBox_shownTrackParticles->currentIndex()==3) parts |= ShownAssociatedObject::TrackParticlesMS;
    if (m_d->editwindow_ui.comboBox_shownTrackParticles->currentIndex()==4) parts |= ShownAssociatedObject::TrackParticlesME;
  }
  if (m_d->editwindow_ui.checkBox_showCaloClusters->isChecked()) parts |= ShownAssociatedObject::CaloClusters;
  if (m_d->editwindow_ui.checkBox_showSegments->isChecked()) parts |= ShownAssociatedObject::Segments;  
  return parts;
}

bool  MuonCollectionSettingsButton::hideActualTrackPath() const
{
  if (!m_d->editwindow)
    m_d->initEditWindow();
  return m_d->editwindow_ui.checkBox_hideactualpaths->isChecked();
}

//____________________________________________________________________
void MuonCollectionSettingsButton::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    m_d->dragStartPosition = event->pos();
  QPushButton::mousePressEvent(event);
}

//____________________________________________________________________
void MuonCollectionSettingsButton::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->source()!=this && event->mimeData()->hasFormat("vp1/material"))
    event->acceptProposedAction();
}

//____________________________________________________________________
void MuonCollectionSettingsButton::mouseMoveEvent(QMouseEvent *event)
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
void MuonCollectionSettingsButton::dropEvent(QDropEvent *event)
{
  QByteArray data = event->mimeData()->data("vp1/material");
  event->acceptProposedAction();
  restoreFromState(data);  
}


QByteArray MuonCollectionSettingsButton::saveState() const{
  // messageVerbose("getState");
  // if (m_d->editwindow_ui.checkBox_tracksUseBaseLightModel->isChecked()) messageVerbose("checked!");
  VP1Serialise serialise(1/*version*/);
  
  serialise.save(m_d->matButton);  
  // serialise.disableUnsavedChecks();
  serialise.save(m_d->editwindow_ui.horizontalSlider_trackWidth);
  serialise.save(m_d->editwindow_ui.checkBox_trackTubes);
  serialise.save(m_d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM);
  serialise.save(m_d->editwindow_ui.checkBox_tracksUseBaseLightModel);
  serialise.save(m_d->editwindow_ui.checkBox_hideactualpaths);
    
  serialise.widgetHandled(this);
  serialise.warnUnsaved(this);
  return serialise.result();
}

void MuonCollectionSettingsButton::restoreFromState( const QByteArray& ba){
   
  VP1Deserialise state(ba,systemBase());
  if (state.version()<0||state.version()>1)
    return;//Ignore silently
  state.restore(m_d->matButton);
  state.restore(m_d->editwindow_ui.horizontalSlider_trackWidth);
  state.restore(m_d->editwindow_ui.checkBox_trackTubes);
  state.restore(m_d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM);
  state.restore(m_d->editwindow_ui.checkBox_tracksUseBaseLightModel);
  state.restore(m_d->editwindow_ui.checkBox_hideactualpaths);

  state.widgetHandled(this);
  state.warnUnrestored(this);

  updateTrackTubeRadius();
  updateTrackLightModel(m_d->editwindow_ui.checkBox_tracksUseBaseLightModel);
  updateButton();
  //FIXME - anything else need updating?
}

//____________________________________________________________________
VP1Interval MuonCollectionSettingsButton::cutAllowedPt() const
{
  if (!m_d->editwindow)
    m_d->initEditWindow();
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
VP1Interval MuonCollectionSettingsButton::cutAllowedEta() const
{
  if (!m_d->editwindow)
    m_d->initEditWindow();
  return m_d->editwindow_ui.etaPhiCutWidget->allowedEta();
}

//____________________________________________________________________
QList<VP1Interval> MuonCollectionSettingsButton::cutAllowedPhi() const
{
  if (!m_d->editwindow)
    m_d->initEditWindow();
  return m_d->editwindow_ui.etaPhiCutWidget->allowedPhi();
}

void MuonCollectionSettingsButton::possibleChange_cutAllowedPt()
{
  messageVerbose("possibleChange_cutAllowedPt() ");
  
  if (m_d->last_cutAllowedPt==cutAllowedPt()) return;
  messageVerbose("cutAllowedPt() changed");
  m_d->last_cutAllowedPt= cutAllowedPt();
  emit cutAllowedPtChanged(m_d->last_cutAllowedPt);
}

void MuonCollectionSettingsButton::possibleChange_cutAllowedEta()
{
  if (m_d->last_cutAllowedEta==cutAllowedEta()) return;
  messageVerbose("cutAllowedEta() changed");
  m_d->last_cutAllowedEta=cutAllowedEta();
  emit cutAllowedEtaChanged(m_d->last_cutAllowedEta);
}

void MuonCollectionSettingsButton::possibleChange_cutAllowedPhi()
{
  if (m_d->last_cutAllowedPhi==cutAllowedPhi()) return;
  messageVerbose("cutAllowedPhi() changed");
  m_d->last_cutAllowedPhi=cutAllowedPhi();
  emit cutAllowedPhiChanged(m_d->last_cutAllowedPhi);
}

void MuonCollectionSettingsButton::possibleChange_minimumQuality()
{
  messageVerbose("possibleChange_minimumQuality()");
  
  if (m_d->last_minimumQuality==minimumQuality()) return;
  messageVerbose("minimumQuality() changed");
  m_d->last_minimumQuality=minimumQuality();
  emit minimumQualityChanged(m_d->last_minimumQuality);
}

// #define VP1CONTROLLERCLASSNAME MuonCollectionSettingsButton
// #include "VP1Base/VP1ControllerMacros.h"
// POSSIBLECHANGE_IMP(shownAssociatedObjects)

void MuonCollectionSettingsButton::possibleChange_shownAssociatedObjects()
{
  messageVerbose("possibleChange_shownAssociatedObjects()");
  
  if (m_d->last_shownAssociatedObjects==shownAssociatedObjects()) return;
  messageVerbose("shownAssociatedObjects() changed");
  m_d->last_shownAssociatedObjects=shownAssociatedObjects();
  emit shownAssociatedObjectsChanged(m_d->last_shownAssociatedObjects);
}
