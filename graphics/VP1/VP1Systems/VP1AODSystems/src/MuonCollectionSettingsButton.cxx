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
#include "VP1Base/VP1QtInventorUtils.h"

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
  if (d->editwindow)
    d->editwindow->setWindowTitle(t);
  setToolTip(t);
}

//____________________________________________________________________
MuonCollectionSettingsButton::MuonCollectionSettingsButton(QWidget * parent,int _dim)
  : VP1MaterialButtonBase(parent,0,"VP1MaterialButton"), d(new Imp)
{
  d->dim = _dim;
  
  d->theclass = this;
  d->initEditWindow();
  
  //Draw Styles / Complexity:
  VP1QtInventorUtils::setLimitsLineWidthSlider(d->editwindow_ui.horizontalSlider_trackWidth);
  VP1QtInventorUtils::setValueLineWidthSlider(d->editwindow_ui.horizontalSlider_trackWidth,1.0);  
  
  d->trackDrawStyle = new SoDrawStyle;
  d->trackDrawStyle->setName("MuonDrawStyle");
  d->trackDrawStyle->pointSize=5.0;
  d->trackDrawStyle->ref();
  updateTrackDrawStyle();
  connect(d->editwindow_ui.horizontalSlider_trackWidth,SIGNAL(valueChanged(int)),this,SLOT(updateTrackDrawStyle()));
  
  d->trackLightModel = new SoLightModel;
  d->trackLightModel->setName("MuonLightModel");
  d->trackLightModel->ref();
  updateTrackLightModel(false);
  connect(d->editwindow_ui.checkBox_tracksUseBaseLightModel,SIGNAL(toggled(bool)),this,SLOT(updateTrackLightModel(bool)));
  
  d->last_trackTubeRadius=trackTubeRadius();
  connect(d->editwindow_ui.checkBox_trackTubes,SIGNAL(toggled(bool)),this,SLOT(updateTrackTubeRadius()));
  connect(d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM,SIGNAL(valueChanged(double)),this,SLOT(updateTrackTubeRadius()));
  
  connect(d->editwindow_ui.checkBox_hideactualpaths,SIGNAL(toggled(bool)),this,SLOT(updateHideActualTrackPath(bool)));

  // -> cutAllowedP/Pt
  connect(d->editwindow_ui.checkBox_cut_minpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
  connect(d->editwindow_ui.checkBox_cut_maxpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
  connect(d->editwindow_ui.doubleSpinBox_cut_minpt_gev,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedPt()));
  connect(d->editwindow_ui.doubleSpinBox_cut_maxpt_gev,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedPt()));
  connect(d->editwindow_ui.checkBox_cut_minpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
  connect(d->editwindow_ui.comboBox_momtype,SIGNAL(currentIndexChanged(int)),this,SLOT(possibleChange_cutAllowedPt()));

  // -> cutAllowedEta
  connect(d->editwindow_ui.etaPhiCutWidget,SIGNAL(allowedEtaChanged(const VP1Interval&)),this,SLOT(possibleChange_cutAllowedEta()));
  
  // -> cutAllowedPhi
  connect(d->editwindow_ui.etaPhiCutWidget,SIGNAL(allowedPhiChanged(const QList<VP1Interval>&)),this,SLOT(possibleChange_cutAllowedPhi()));
 
  // -> quality
  connect(d->editwindow_ui.comboBox_minimumQuality,SIGNAL(currentIndexChanged(int)),this,SLOT(possibleChange_minimumQuality()));
  d->last_minimumQuality=minimumQuality();
 
  // -> shownObjects
  connect(d->editwindow_ui.checkBox_showTrackParticles,SIGNAL(toggled(bool)),this,SLOT(possibleChange_shownAssociatedObjects()));
  connect(d->editwindow_ui.comboBox_shownTrackParticles,SIGNAL(currentIndexChanged(int)),this,SLOT(possibleChange_shownAssociatedObjects()));
  d->last_shownAssociatedObjects=shownAssociatedObjects();
  
  connect(this,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
  connect(d->editwindow_ui.pushButton_close,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
  connect(d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(updateButton()));
  connect(d->matButton,SIGNAL(lastAppliedChanged()),this,SIGNAL(lastAppliedChanged()));
  setAcceptDrops(true);
  
  QTimer::singleShot(0, this, SLOT(updateButton()));
  
}

// QWidget& MuonCollectionSettingsButton::editWindow() {
//   if (!d->editwindow)
//     initEditWindow();
//   return *(d->editwindow);
// } 
MuonCollectionSettingsButton::~MuonCollectionSettingsButton()
{
  delete d->editwindow;
  d->trackDrawStyle->unref();
  d->trackLightModel->unref();
  delete d;
}

void MuonCollectionSettingsButton::updateButton()
{
  if (objectName().isEmpty())
    setObjectName("MuonCollectionSettingsButton");
  messageVerbose("setColButtonProperties: color=" + str(d->matButton->lastAppliedDiffuseColour()));
  VP1ColorSelectButton::setColButtonProperties(this,d->matButton->lastAppliedDiffuseColour(),d->dim);
}

void MuonCollectionSettingsButton::setDimension(int _dim)
{
  if (d->dim == _dim)
    return;
  d->dim = _dim;
  updateButton();
}

void MuonCollectionSettingsButton::showEditMaterialDialog()
{
  if (!d->editwindow)
    d->initEditWindow();

  if (d->editwindow->isHidden())
    d->editwindow->show();
  else
    d->editwindow->hide();
}

bool MuonCollectionSettingsButton::setMaterial(SoMaterial*mat)
{  
	// std::cout<<"MuonCollectionSettingsButton::setMaterial with mat="<<mat<<std::endl;
  if (!d->matButton) d->initEditWindow();
  d->matButton->setMaterial(mat);
  return true;
}

void MuonCollectionSettingsButton::copyValuesFromMaterial(SoMaterial*mat)
{
  if (!d->matButton) d->initEditWindow();
  d->matButton->setMaterial(mat);
}
double MuonCollectionSettingsButton::lastAppliedTransparency() const 
{
  if (!d->matButton) d->initEditWindow();
  return d->matButton->lastAppliedTransparency();
}
double MuonCollectionSettingsButton::lastAppliedShininess() const  
{
  if (!d->matButton) d->initEditWindow();
  return d->matButton->lastAppliedShininess();
}
double MuonCollectionSettingsButton::lastAppliedBrightness() const
{
  if (!d->matButton) d->initEditWindow();
  return d->matButton->lastAppliedBrightness();
}

double MuonCollectionSettingsButton::trackTubeRadius() const
{
  return d->editwindow_ui.checkBox_trackTubes->isChecked() ?
    d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM->value() : 0.0;
}

void MuonCollectionSettingsButton::updateTrackTubeRadius()
{
  if (d->last_trackTubeRadius==trackTubeRadius()) return;
  d->last_trackTubeRadius=trackTubeRadius();
  messageVerbose("TrackTubeRadius changed to "+str(d->last_trackTubeRadius));
  emit trackTubeRadiusChanged(d->last_trackTubeRadius);
  return;
}

void MuonCollectionSettingsButton::updateTrackDrawStyle()
{
  double val = VP1QtInventorUtils::getValueLineWidthSlider(d->editwindow_ui.horizontalSlider_trackWidth);
  if (d->trackDrawStyle->lineWidth.getValue()!=val)
    d->trackDrawStyle->lineWidth = val;
}

void MuonCollectionSettingsButton::updateTrackLightModel(bool base)
{
  if (d->trackLightModel->model.getValue()!=(base?SoLightModel::BASE_COLOR:SoLightModel::PHONG)) {
    messageVerbose("TrackLightModel changed (base = "+str(base));
    if (base)
      d->trackLightModel->model.setValue(SoLightModel::BASE_COLOR);
    else
      d->trackLightModel->model.setValue(SoLightModel::PHONG);
  }
}

void MuonCollectionSettingsButton::updateHideActualTrackPath(bool hide)
{
  emit hideActualTrackPathChanged(hide);
}


SoDrawStyle * MuonCollectionSettingsButton::trackDrawStyle() const
{
  return d->trackDrawStyle;
}

SoLightModel * MuonCollectionSettingsButton::trackLightModel() const
{
  return d->trackLightModel;
}

SoMaterial* MuonCollectionSettingsButton::defaultParameterMaterial() const {
  // return d->defaultParametersMatButton->handledMaterials().at(0);// Only have one material per button.
  return 0;
}

SoMaterial* MuonCollectionSettingsButton::parameterMaterial( xAOD::ParameterPosition /**position*/ ) const{
  // return (d->parameterTypeMatButtons.at(static_cast<unsigned int>(position)))->handledMaterials().at(0);// Only have one material per button.
  return 0;
}

// void setDefaultParameterMaterial(SoMaterial* mat) {    
//     // d->defaultParametersMatButton->setObjectName("matButton_default");
//     // d->defaultParametersMatButton->setToolTip(matButtonToolTip());
//     d->defaultParametersMatButton->setMaterial(mat);
//     // connect(d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(possibleChangeMatTranspOrBrightness()));
// }
// 
// void MuonCollectionSettingsButton::setParameterMaterial(SoMaterial*, xAOD::ParameterPosition){
//   // FIXME!
// }

unsigned int MuonCollectionSettingsButton::minimumQuality() const
{
  if (!d->editwindow)
    d->initEditWindow();
  
  QString text = d->editwindow_ui.comboBox_minimumQuality->currentText();
  if (text=="Very Loose") return static_cast<unsigned int>(xAOD::Muon::VeryLoose);
  if (text=="Loose")  return static_cast<unsigned int>(xAOD::Muon::Loose);
  if (text=="Medium") return static_cast<unsigned int>(xAOD::Muon::Medium);
  if (text=="Tight")  return static_cast<unsigned int>(xAOD::Muon::Tight);

  message("ERROR! MuonCollectionSettingsButton::minimumQuality - unknown value! Returning 'Very Loose'.");
  return static_cast<unsigned int>(xAOD::Muon::VeryLoose);
}

MuonCollectionSettingsButton::ShownAssociatedObjects MuonCollectionSettingsButton::shownAssociatedObjects() const 
{
  if (!d->editwindow)
    d->initEditWindow();
  ShownAssociatedObjects parts = ShownAssociatedObject::Nothing;
  // if (!d->ui_col.checkBox_hideactualpaths->isChecked()) parts |= TrackCommonFlags::ActualPath;
  if (d->editwindow_ui.checkBox_showTrackParticles->isChecked()) {
    if (d->editwindow_ui.comboBox_shownTrackParticles->currentIndex()==0) parts |= ShownAssociatedObject::TrackParticlesPrimary;
    if (d->editwindow_ui.comboBox_shownTrackParticles->currentIndex()==1) parts |= ShownAssociatedObject::TrackParticlesCB;
    if (d->editwindow_ui.comboBox_shownTrackParticles->currentIndex()==2) parts |= ShownAssociatedObject::TrackParticlesID;
    if (d->editwindow_ui.comboBox_shownTrackParticles->currentIndex()==3) parts |= ShownAssociatedObject::TrackParticlesMS;
    if (d->editwindow_ui.comboBox_shownTrackParticles->currentIndex()==4) parts |= ShownAssociatedObject::TrackParticlesME;
  }
  if (d->editwindow_ui.checkBox_showCaloClusters->isChecked()) parts |= ShownAssociatedObject::CaloClusters;
  if (d->editwindow_ui.checkBox_showSegments->isChecked()) parts |= ShownAssociatedObject::Segments;  
  return parts;
}

bool  MuonCollectionSettingsButton::hideActualTrackPath() const
{
  if (!d->editwindow)
    d->initEditWindow();
  return d->editwindow_ui.checkBox_hideactualpaths->isChecked();
}

//____________________________________________________________________
void MuonCollectionSettingsButton::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    d->dragStartPosition = event->pos();
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
  if ((event->pos() - d->dragStartPosition).manhattanLength()
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
  // QString str_ambient = d->toSbColTxt(d->lastapplied_ambient);
  // if (str_ambient!="SbColor(0.2,0.2,0.2)")
  //   s += "mat->ambientColor.setValue("+str_ambient+");\n";
  // QString str_diffuse = d->toSbColTxt(d->lastapplied_diffuse);
  // if (str_diffuse!="SbColor(0.8,0.8,0.8)")
  //   s += "mat->diffuseColor.setValue("+str_diffuse+");\n";
  // QString str_specular = d->toSbColTxt(d->lastapplied_specular);
  // if (str_specular!="SbColor(0,0,0)")
  //   s += "mat->specularColor.setValue("+str_specular+");\n";
  // QString str_emissive = d->toSbColTxt(d->lastapplied_emissive);
  // if (str_emissive!="SbColor(0,0,0)")
  //   s += "mat->emissiveColor.setValue("+str_emissive+");\n";
  // QString str_shininess = d->printFloat(d->lastapplied_shininess/100.0);
  // if (str_shininess!="0.2")
  //   s +=     "mat->shininess.setValue("+str_shininess+");\n";
  // QString str_transparency = d->printFloat(d->lastapplied_transparency/100.0);
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
  // if (d->editwindow_ui.checkBox_tracksUseBaseLightModel->isChecked()) messageVerbose("checked!");
  VP1Serialise serialise(1/*version*/);
  
  serialise.save(d->matButton);  
  // serialise.disableUnsavedChecks();
  serialise.save(d->editwindow_ui.horizontalSlider_trackWidth);
  serialise.save(d->editwindow_ui.checkBox_trackTubes);
  serialise.save(d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM);
  serialise.save(d->editwindow_ui.checkBox_tracksUseBaseLightModel);
  serialise.save(d->editwindow_ui.checkBox_hideactualpaths);
    
  serialise.widgetHandled(this);
  serialise.warnUnsaved(this);
  return serialise.result();
}

void MuonCollectionSettingsButton::restoreFromState( const QByteArray& ba){
   
  VP1Deserialise state(ba,systemBase());
  if (state.version()<0||state.version()>1)
    return;//Ignore silently
  state.restore(d->matButton);
  state.restore(d->editwindow_ui.horizontalSlider_trackWidth);
  state.restore(d->editwindow_ui.checkBox_trackTubes);
  state.restore(d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM);
  state.restore(d->editwindow_ui.checkBox_tracksUseBaseLightModel);
  state.restore(d->editwindow_ui.checkBox_hideactualpaths);

  state.widgetHandled(this);
  state.warnUnrestored(this);

  updateTrackTubeRadius();
  updateTrackLightModel(d->editwindow_ui.checkBox_tracksUseBaseLightModel);
  updateButton();
  //FIXME - anything else need updating?
}

//____________________________________________________________________
VP1Interval MuonCollectionSettingsButton::cutAllowedPt() const
{
  if (!d->editwindow)
    d->initEditWindow();
  if (!d->editwindow_ui.checkBox_cut_minpt)
    return VP1Interval();

  // will set range to negative if we have momcut=P
  // if minCut unset then min=-inf
  // if minCut set, and Pt selected, then min=-minCut
  // if minCut set, and P selected, then min=-maxCut
  // etc
  bool isPCut = d->editwindow_ui.comboBox_momtype->currentText()=="P";
  
  const double minFromInterface=d->editwindow_ui.doubleSpinBox_cut_minpt_gev->value()*1000;
  const double maxFromInterface=d->editwindow_ui.doubleSpinBox_cut_maxpt_gev->value()*1000;
  
  double min=0.0,max=0.0;
  if (!isPCut) {
    //Pt cut
    min = (d->editwindow_ui.checkBox_cut_minpt->isChecked() ? minFromInterface : -std::numeric_limits<double>::infinity());
    max = (d->editwindow_ui.checkBox_cut_maxpt->isChecked() ? maxFromInterface : std::numeric_limits<double>::infinity());
  } else {
    min = (d->editwindow_ui.checkBox_cut_maxpt->isChecked() ? -maxFromInterface : -std::numeric_limits<double>::infinity());
    max = (d->editwindow_ui.checkBox_cut_minpt->isChecked() ? -minFromInterface : std::numeric_limits<double>::infinity());
  }
  
  //message("cutAllowedPt: min,max="+QString::number(min)+","+QString::number(max));
  
  if (max<min)
    return VP1Interval();
    
  return VP1Interval( min, max );//fixme: closed interval??
}

//____________________________________________________________________
VP1Interval MuonCollectionSettingsButton::cutAllowedEta() const
{
  if (!d->editwindow)
    d->initEditWindow();
  return d->editwindow_ui.etaPhiCutWidget->allowedEta();
}

//____________________________________________________________________
QList<VP1Interval> MuonCollectionSettingsButton::cutAllowedPhi() const
{
  if (!d->editwindow)
    d->initEditWindow();
  return d->editwindow_ui.etaPhiCutWidget->allowedPhi();
}

void MuonCollectionSettingsButton::possibleChange_cutAllowedPt()
{
  messageVerbose("possibleChange_cutAllowedPt() ");
  
  if (d->last_cutAllowedPt==cutAllowedPt()) return;
  messageVerbose("cutAllowedPt() changed");
  d->last_cutAllowedPt= cutAllowedPt();
  emit cutAllowedPtChanged(d->last_cutAllowedPt);
}

void MuonCollectionSettingsButton::possibleChange_cutAllowedEta()
{
  if (d->last_cutAllowedEta==cutAllowedEta()) return;
  messageVerbose("cutAllowedEta() changed");
  d->last_cutAllowedEta=cutAllowedEta();
  emit cutAllowedEtaChanged(d->last_cutAllowedEta);
}

void MuonCollectionSettingsButton::possibleChange_cutAllowedPhi()
{
  if (d->last_cutAllowedPhi==cutAllowedPhi()) return;
  messageVerbose("cutAllowedPhi() changed");
  d->last_cutAllowedPhi=cutAllowedPhi();
  emit cutAllowedPhiChanged(d->last_cutAllowedPhi);
}

void MuonCollectionSettingsButton::possibleChange_minimumQuality()
{
  messageVerbose("possibleChange_minimumQuality()");
  
  if (d->last_minimumQuality==minimumQuality()) return;
  messageVerbose("minimumQuality() changed");
  d->last_minimumQuality=minimumQuality();
  emit minimumQualityChanged(d->last_minimumQuality);
}

// #define VP1CONTROLLERCLASSNAME MuonCollectionSettingsButton
// #include "VP1Base/VP1ControllerMacros.h"
// POSSIBLECHANGE_IMP(shownAssociatedObjects)

void MuonCollectionSettingsButton::possibleChange_shownAssociatedObjects()
{
  messageVerbose("possibleChange_shownAssociatedObjects()");
  
  if (d->last_shownAssociatedObjects==shownAssociatedObjects()) return;
  messageVerbose("shownAssociatedObjects() changed");
  d->last_shownAssociatedObjects=shownAssociatedObjects();
  emit shownAssociatedObjectsChanged(d->last_shownAssociatedObjects);
}
