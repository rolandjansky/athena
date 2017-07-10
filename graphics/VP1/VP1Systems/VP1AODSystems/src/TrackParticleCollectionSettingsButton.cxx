/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local
#include "VP1Base/VP1ColorSelectButton.h"
#include "TrackParticleCollectionSettingsButton.h"
#include "ui_pertrackparticlecollectionsettings_form.h"
#include "ui_settings_cuts_form.h"

//VP1
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
class TrackParticleCollectionSettingsButton::Imp {
public:
  Imp():theclass(0),editwindow(0),matButton(0), defaultParametersMatButton(nullptr),
  trackDrawStyle(0), trackLightModel(0),last_trackTubeRadius(0.0),last_cutOnlyVertexAssocTracks{},
  last_useDefaultCuts{},dim(0){
    //nop
  }
  TrackParticleCollectionSettingsButton * theclass;
  QWidget * editwindow;
  Ui::TrackParticleCollectionSettingsForm editwindow_ui;
  
  VP1MaterialButton* matButton;// main collection colour
  VP1MaterialButton* defaultParametersMatButton;
  
  QList<VP1MaterialButton*> parameterTypeMatButtons;// enum used to access the specific button
  
  SoDrawStyle * trackDrawStyle;
  SoLightModel * trackLightModel;
  
  //GUI - last values
  double last_trackTubeRadius;
  VP1Interval last_cutAllowedPt;
  VP1Interval last_cutAllowedP;
  VP1Interval last_cutAllowedEta;
  QList<VP1Interval> last_cutAllowedPhi;
  QList<unsigned> last_cutRequiredNHits;
  bool last_cutOnlyVertexAssocTracks;
  bool last_useDefaultCuts;
  
  int dim;
  QPoint dragStartPosition;
  
  void initEditWindow();
};

//____________________________________________________________________
void TrackParticleCollectionSettingsButton::Imp::initEditWindow()
{
  if (editwindow)
    return;
  theclass->messageVerbose("Initialising material editor dialog");
  editwindow = new QWidget(0,Qt::WindowStaysOnTopHint);
  editwindow_ui.setupUi(editwindow);
  matButton = editwindow_ui.pushButton_matButton;  
  
	defaultParametersMatButton = editwindow_ui.matButton_parametersDefaultColour;
  SoMaterial* defParamMat = new SoMaterial; // Default (for the moment)
  defaultParametersMatButton->setMaterial(defParamMat);
  editwindow_ui.frame_parameterTypeColours->hide();  

  // Set the default colours for the "colour by parameter type" section
  parameterTypeMatButtons.append(editwindow_ui.pushButton_matButton_parameters_default_colour_2);
  parameterTypeMatButtons.append(editwindow_ui.pushButton_matButton_parameters_default_colour_3);
  parameterTypeMatButtons.append(editwindow_ui.pushButton_matButton_parameters_default_colour_4);
  parameterTypeMatButtons.append(editwindow_ui.pushButton_matButton_parameters_default_colour_5);
  parameterTypeMatButtons.append(editwindow_ui.pushButton_matButton_parameters_default_colour_6);
  parameterTypeMatButtons.append(editwindow_ui.pushButton_matButton_parameters_default_colour_7);
  QList<QColor> colours;
  colours.append(QColor(0.0, 170.0, 255.0));
  colours.append(QColor(170, 85, 255));
  colours.append(QColor(255, 85, 0));
  colours.append(QColor(170, 0, 127));
  colours.append(QColor(170, 255, 0));
  colours.append(QColor(85, 0, 255));

  for (unsigned int i=0; i<6;++i){
    SoMaterial * mat = VP1MaterialButton::createMaterial(colours.at(i));
    (parameterTypeMatButtons.at(i))->setMaterial(mat);
  }
}

//____________________________________________________________________
void TrackParticleCollectionSettingsButton::setMaterialText(const QString& t)
{
  if (d->editwindow)
    d->editwindow->setWindowTitle(t);
  setToolTip(t);
}

//____________________________________________________________________
TrackParticleCollectionSettingsButton::TrackParticleCollectionSettingsButton(QWidget * parent,int _dim)
  : VP1MaterialButtonBase(parent,0,"VP1MaterialButton"), d(new Imp)
{
  d->dim = _dim;
  
  d->theclass = this;
  d->initEditWindow();
  
  //Draw Styles / Complexity:
  VP1QtInventorUtils::setLimitsLineWidthSlider(d->editwindow_ui.horizontalSlider_trackWidth);
  VP1QtInventorUtils::setValueLineWidthSlider(d->editwindow_ui.horizontalSlider_trackWidth,1.0);  
  
  d->trackDrawStyle = new SoDrawStyle;
  d->trackDrawStyle->setName("TrackParticleDrawStyle");
  d->trackDrawStyle->pointSize=5.0;
  d->trackDrawStyle->ref();
  updateTrackDrawStyle();
  connect(d->editwindow_ui.horizontalSlider_trackWidth,SIGNAL(valueChanged(int)),this,SLOT(updateTrackDrawStyle()));
  d->last_trackTubeRadius=trackTubeRadius();
  connect(d->editwindow_ui.checkBox_trackTubes,SIGNAL(toggled(bool)),this,SLOT(updateTrackTubeRadius()));
  connect(d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM,SIGNAL(valueChanged(double)),this,SLOT(updateTrackTubeRadius()));
 
  
  d->trackLightModel = new SoLightModel;
  d->trackLightModel->setName("TrackParticleLightModel");
  d->trackLightModel->ref();
  updateTrackLightModel(false);
  connect(d->editwindow_ui.checkBox_tracksUseBaseLightModel,SIGNAL(toggled(bool)),this,SLOT(updateTrackLightModel(bool)));
  connect(d->editwindow_ui.checkBox_hideactualpaths,SIGNAL(toggled(bool)),this,SLOT(updateHideActualTrackPath(bool)));
  
  // Propagation - for all of these, just emit the propagationOptionsChanged() signal and rely on clients checking to see what has changed.
  connect(d->editwindow_ui.radioButton_existingParameters, SIGNAL(toggled(bool)),    this, SIGNAL(propagationOptionsChanged()));
  connect(d->editwindow_ui.radioButton_extrapolate,        SIGNAL(toggled(bool)),    this, SIGNAL(propagationOptionsChanged()));
  connect(d->editwindow_ui.horizontalSlider_numBezierSteps,SIGNAL(valueChanged(int)),this, SIGNAL(propagationOptionsChanged()));

  // -> parameters
  connect(d->editwindow_ui.checkBox_showparameters,SIGNAL(toggled(bool)),this,SLOT(possibleChange_showParameters()));
  connect(d->editwindow_ui.checkBox_parameters_colourByType,SIGNAL(toggled(bool)),this,SLOT(possibleChange_parameterColours()));

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

  // -> cutRequiredNHits();
  connect(d->editwindow_ui.checkBox_cut_nhits_pixel,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutRequiredNHits()));
  connect(d->editwindow_ui.checkBox_cut_nhits_sct,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutRequiredNHits()));
  connect(d->editwindow_ui.checkBox_cut_nhits_trt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutRequiredNHits()));
  connect(d->editwindow_ui.checkBox_cut_nhits_muon,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutRequiredNHits()));
  connect(d->editwindow_ui.spinBox_cut_nhits_pixel,SIGNAL(valueChanged(int)),this,SLOT(possibleChange_cutRequiredNHits()));
  connect(d->editwindow_ui.spinBox_cut_nhits_sct,SIGNAL(valueChanged(int)),this,SLOT(possibleChange_cutRequiredNHits()));
  connect(d->editwindow_ui.spinBox_cut_nhits_trt,SIGNAL(valueChanged(int)),this,SLOT(possibleChange_cutRequiredNHits()));
  connect(d->editwindow_ui.spinBox_cut_nhits_muon,SIGNAL(valueChanged(int)),this,SLOT(possibleChange_cutRequiredNHits()));

  // -> cutOnlyVertexAssocTracks
  connect(d->editwindow_ui.checkBox_vertexAssociated,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutOnlyVertexAssocTracks()));
  
  connect(this,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
  connect(d->editwindow_ui.pushButton_close,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
  connect(d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(updateButton()));
  connect(d->matButton,SIGNAL(lastAppliedChanged()),this,SIGNAL(lastAppliedChanged()));
  setAcceptDrops(true);
  
  QTimer::singleShot(0, this, SLOT(updateButton()));
  
}

// QWidget& TrackParticleCollectionSettingsButton::editWindow() {
//   if (!d->editwindow)
//     initEditWindow();
//   return *(d->editwindow);
// } 
TrackParticleCollectionSettingsButton::~TrackParticleCollectionSettingsButton()
{
  delete d->editwindow;
  d->trackDrawStyle->unref();
  d->trackLightModel->unref();
  delete d;
}

void TrackParticleCollectionSettingsButton::updateButton()
{
  if (objectName().isEmpty())
    setObjectName("TrackParticleCollectionSettingsButton");
  messageVerbose("setColButtonProperties: color=" + str(d->matButton->lastAppliedDiffuseColour()));
  VP1ColorSelectButton::setColButtonProperties(this,d->matButton->lastAppliedDiffuseColour(),d->dim);
}

void TrackParticleCollectionSettingsButton::setDimension(int _dim)
{
  if (d->dim == _dim)
    return;
  d->dim = _dim;
  updateButton();
}

void TrackParticleCollectionSettingsButton::showEditMaterialDialog()
{
  if (!d->editwindow)
    d->initEditWindow();

  if (d->editwindow->isHidden())
    d->editwindow->show();
  else
    d->editwindow->hide();
}

bool TrackParticleCollectionSettingsButton::setMaterial(SoMaterial*mat)
{  
	// std::cout<<"TrackParticleCollectionSettingsButton::setMaterial with mat="<<mat<<std::endl;
  if (!d->matButton) d->initEditWindow();
  d->matButton->setMaterial(mat);
  return true;
}

void TrackParticleCollectionSettingsButton::copyValuesFromMaterial(SoMaterial*mat)
{
  if (!d->matButton) d->initEditWindow();
  d->matButton->setMaterial(mat);
}
double TrackParticleCollectionSettingsButton::lastAppliedTransparency() const 
{
  if (!d->matButton) d->initEditWindow();
  return d->matButton->lastAppliedTransparency();
}
double TrackParticleCollectionSettingsButton::lastAppliedShininess() const  
{
  if (!d->matButton) d->initEditWindow();
  return d->matButton->lastAppliedShininess();
}
double TrackParticleCollectionSettingsButton::lastAppliedBrightness() const
{
  if (!d->matButton) d->initEditWindow();
  return d->matButton->lastAppliedBrightness();
}

double TrackParticleCollectionSettingsButton::trackTubeRadius() const
{
  return d->editwindow_ui.checkBox_trackTubes->isChecked() ?
    d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM->value() : 0.0;
}

void TrackParticleCollectionSettingsButton::updateTrackTubeRadius()
{
  if (d->last_trackTubeRadius==trackTubeRadius()) return;
  d->last_trackTubeRadius=trackTubeRadius();
  messageVerbose("TrackTubeRadius changed to "+str(d->last_trackTubeRadius));
  emit trackTubeRadiusChanged(d->last_trackTubeRadius);
  return;
}

void TrackParticleCollectionSettingsButton::updateTrackDrawStyle()
{
  double val = VP1QtInventorUtils::getValueLineWidthSlider(d->editwindow_ui.horizontalSlider_trackWidth);
  if (d->trackDrawStyle->lineWidth.getValue()!=val)
    d->trackDrawStyle->lineWidth = val;
}

void TrackParticleCollectionSettingsButton::updateTrackLightModel(bool base)
{
  if (d->trackLightModel->model.getValue()!=(base?SoLightModel::BASE_COLOR:SoLightModel::PHONG)) {
    messageVerbose("TrackLightModel changed (base = "+str(base));
    if (base)
      d->trackLightModel->model.setValue(SoLightModel::BASE_COLOR);
    else
      d->trackLightModel->model.setValue(SoLightModel::PHONG);
  }
}

void TrackParticleCollectionSettingsButton::updateHideActualTrackPath(bool hide)
{
  emit hideActualTrackPathChanged(hide);
}


SoDrawStyle * TrackParticleCollectionSettingsButton::trackDrawStyle() const
{
  return d->trackDrawStyle;
}

SoLightModel * TrackParticleCollectionSettingsButton::trackLightModel() const
{
  return d->trackLightModel;
}

bool TrackParticleCollectionSettingsButton::useExistingParameters() const{
  return d->editwindow_ui.radioButton_existingParameters->isChecked();
}

int TrackParticleCollectionSettingsButton::numOfStepsForInterpolation() const {
  return d->editwindow_ui.horizontalSlider_numBezierSteps->value();
}

SoMaterial* TrackParticleCollectionSettingsButton::defaultParameterMaterial() const {
  return d->defaultParametersMatButton->handledMaterials().at(0);// Only have one material per button.
}
SoMaterial* TrackParticleCollectionSettingsButton::parameterMaterial( xAOD::ParameterPosition position) const{
  return (d->parameterTypeMatButtons.at(static_cast<unsigned int>(position)))->handledMaterials().at(0);// Only have one material per button.
}

// void setDefaultParameterMaterial(SoMaterial* mat) {    
//     // d->defaultParametersMatButton->setObjectName("matButton_default");
//     // d->defaultParametersMatButton->setToolTip(matButtonToolTip());
//     d->defaultParametersMatButton->setMaterial(mat);
//     // connect(d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(possibleChangeMatTranspOrBrightness()));
// }
// 
// void TrackParticleCollectionSettingsButton::setParameterMaterial(SoMaterial*, xAOD::ParameterPosition){
//   // FIXME!
// }

bool  TrackParticleCollectionSettingsButton::hideActualTrackPath() const
{
  if (!d->editwindow)
    d->initEditWindow();
  return d->editwindow_ui.checkBox_hideactualpaths->isChecked();
}

bool TrackParticleCollectionSettingsButton::showParameters() const
{
  if (!d->editwindow)
    d->initEditWindow();
  messageVerbose("TrackParticleCollectionSettingsButton::showParameters returning  "+str(d->editwindow_ui.checkBox_showparameters->isChecked()));
  return d->editwindow_ui.checkBox_showparameters->isChecked();
}

bool TrackParticleCollectionSettingsButton::colourParametersByType() const
{
  if (!d->editwindow)
    d->initEditWindow();
  messageVerbose("TrackParticleCollectionSettingsButton::colourParametersByType returning  "+str(d->editwindow_ui.checkBox_parameters_colourByType->isChecked()));
  return d->editwindow_ui.checkBox_parameters_colourByType->isChecked();
}


bool  TrackParticleCollectionSettingsButton::cutOnlyVertexAssocTracks() const
{
  if (!d->editwindow)
    d->initEditWindow();
  return d->editwindow_ui.checkBox_vertexAssociated->isChecked();
}

//____________________________________________________________________
void TrackParticleCollectionSettingsButton::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    d->dragStartPosition = event->pos();
  QPushButton::mousePressEvent(event);
}

//____________________________________________________________________
void TrackParticleCollectionSettingsButton::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->source()!=this && event->mimeData()->hasFormat("vp1/material"))
    event->acceptProposedAction();
}

//____________________________________________________________________
void TrackParticleCollectionSettingsButton::mouseMoveEvent(QMouseEvent *event)
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
void TrackParticleCollectionSettingsButton::dropEvent(QDropEvent *event)
{
  QByteArray data = event->mimeData()->data("vp1/material");
  event->acceptProposedAction();
  restoreFromState(data);  
}


QByteArray TrackParticleCollectionSettingsButton::saveState() const{
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
  
  // Parameters
  serialise.save(d->editwindow_ui.checkBox_showparameters);  
  serialise.save(d->editwindow_ui.checkBox_parameters_colourByType);  
  
  serialise.widgetHandled(this);
  serialise.warnUnsaved(this);
  return serialise.result();
}

void TrackParticleCollectionSettingsButton::restoreFromState( const QByteArray& ba){
   
  VP1Deserialise state(ba,systemBase());
  if (state.version()<0||state.version()>1)
    return;//Ignore silently
  state.restore(d->matButton);
  state.restore(d->editwindow_ui.horizontalSlider_trackWidth);
  state.restore(d->editwindow_ui.checkBox_trackTubes);
  state.restore(d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM);
  state.restore(d->editwindow_ui.checkBox_tracksUseBaseLightModel);
  state.restore(d->editwindow_ui.checkBox_hideactualpaths);

  // Parameters
  state.restore(d->editwindow_ui.checkBox_showparameters);  
  state.restore(d->editwindow_ui.checkBox_parameters_colourByType);  

  state.widgetHandled(this);
  state.warnUnrestored(this);

  updateTrackTubeRadius();
  updateTrackLightModel(d->editwindow_ui.checkBox_tracksUseBaseLightModel);
  updateButton();
  //FIXME - anything else need updating?
}

//____________________________________________________________________
VP1Interval TrackParticleCollectionSettingsButton::cutAllowedPt() const
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
VP1Interval TrackParticleCollectionSettingsButton::cutAllowedEta() const
{
  if (!d->editwindow)
    d->initEditWindow();
  return d->editwindow_ui.etaPhiCutWidget->allowedEta();
}

//____________________________________________________________________
QList<VP1Interval> TrackParticleCollectionSettingsButton::cutAllowedPhi() const
{
  if (!d->editwindow)
    d->initEditWindow();
  return d->editwindow_ui.etaPhiCutWidget->allowedPhi();
}

//____________________________________________________________________
QList<unsigned> TrackParticleCollectionSettingsButton::cutRequiredNHits() const
{
  if (!d->editwindow)
    d->initEditWindow();
  unsigned npixel = d->editwindow_ui.checkBox_cut_nhits_pixel->isChecked() ? d->editwindow_ui.spinBox_cut_nhits_pixel->value() : 0;
  unsigned nsct = d->editwindow_ui.checkBox_cut_nhits_sct->isChecked() ? d->editwindow_ui.spinBox_cut_nhits_sct->value() : 0;
  unsigned ntrt = d->editwindow_ui.checkBox_cut_nhits_trt->isChecked() ? d->editwindow_ui.spinBox_cut_nhits_trt->value() : 0;
  unsigned nmuon = d->editwindow_ui.checkBox_cut_nhits_muon->isChecked() ? d->editwindow_ui.spinBox_cut_nhits_muon->value() : 0;
  QList<unsigned> l;
  if (!npixel&&!nsct&&!ntrt&&!nmuon)
    return l;
  l << npixel << nsct << ntrt << nmuon;
  return l;
}


void TrackParticleCollectionSettingsButton::possibleChange_cutAllowedPt()
{
  messageVerbose("possibleChange_cutAllowedPt() ");
  
  if (d->last_cutAllowedPt==cutAllowedPt()) return;
  messageVerbose("cutAllowedPt() changed");
  d->last_cutAllowedPt= cutAllowedPt();
  emit cutAllowedPtChanged(d->last_cutAllowedPt);
}

void TrackParticleCollectionSettingsButton::possibleChange_cutAllowedEta()
{
  if (d->last_cutAllowedEta==cutAllowedEta()) return;
  messageVerbose("cutAllowedEta() changed");
  d->last_cutAllowedEta=cutAllowedEta();
  emit cutAllowedEtaChanged(d->last_cutAllowedEta);
}

void TrackParticleCollectionSettingsButton::possibleChange_cutAllowedPhi()
{
  if (d->last_cutAllowedPhi==cutAllowedPhi()) return;
  messageVerbose("cutAllowedPhi() changed");
  d->last_cutAllowedPhi=cutAllowedPhi();
  emit cutAllowedPhiChanged(d->last_cutAllowedPhi);
}

void TrackParticleCollectionSettingsButton::possibleChange_cutRequiredNHits()
{
  if (d->last_cutRequiredNHits!=cutRequiredNHits()) return;
  messageVerbose("cutRequiredNHits() changed");
  d->last_cutRequiredNHits=cutRequiredNHits();
  emit cutRequiredNHitsChanged(d->last_cutRequiredNHits);
}

void TrackParticleCollectionSettingsButton::possibleChange_cutOnlyVertexAssocTracks()
{
  if (d->last_cutOnlyVertexAssocTracks!=cutOnlyVertexAssocTracks()) return;
  messageVerbose("cutOnlyVertexAssocTracks() changed");
  d->last_cutOnlyVertexAssocTracks=cutOnlyVertexAssocTracks();
  emit cutOnlyVertexAssocTracksChanged(d->last_cutOnlyVertexAssocTracks);
}

void TrackParticleCollectionSettingsButton::possibleChange_showParameters()
{
  messageVerbose("TrackParticleCollectionSettingsButton::possibleChange_showParameters emitting showParametersChanged  ");
	emit showParametersChanged(d->editwindow_ui.checkBox_showparameters->isChecked());
}

void TrackParticleCollectionSettingsButton::possibleChange_parameterColours()
{
  messageVerbose("TrackParticleCollectionSettingsButton::possibleChange_parameterColours emitting  colourParametersByTypeChanged ");
	emit colourParametersByTypeChanged(d->editwindow_ui.checkBox_parameters_colourByType->isChecked());
}

