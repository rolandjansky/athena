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
  if (m_d->editwindow)
    m_d->editwindow->setWindowTitle(t);
  setToolTip(t);
}

//____________________________________________________________________
TrackParticleCollectionSettingsButton::TrackParticleCollectionSettingsButton(QWidget * parent,int dim)
  : VP1MaterialButtonBase(parent,0,"VP1MaterialButton"), m_d(new Imp)
{
  m_d->dim = dim;
  
  m_d->theclass = this;
  m_d->initEditWindow();
  
  //Draw Styles / Complexity:
  VP1QtInventorUtils::setLimitsLineWidthSlider(m_d->editwindow_ui.horizontalSlider_trackWidth);
  VP1QtInventorUtils::setValueLineWidthSlider(m_d->editwindow_ui.horizontalSlider_trackWidth,1.0);  
  
  m_d->trackDrawStyle = new SoDrawStyle;
  m_d->trackDrawStyle->setName("TrackParticleDrawStyle");
  m_d->trackDrawStyle->pointSize=5.0;
  m_d->trackDrawStyle->ref();
  updateTrackDrawStyle();
  connect(m_d->editwindow_ui.horizontalSlider_trackWidth,SIGNAL(valueChanged(int)),this,SLOT(updateTrackDrawStyle()));
  m_d->last_trackTubeRadius=trackTubeRadius();
  connect(m_d->editwindow_ui.checkBox_trackTubes,SIGNAL(toggled(bool)),this,SLOT(updateTrackTubeRadius()));
  connect(m_d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM,SIGNAL(valueChanged(double)),this,SLOT(updateTrackTubeRadius()));
 
  
  m_d->trackLightModel = new SoLightModel;
  m_d->trackLightModel->setName("TrackParticleLightModel");
  m_d->trackLightModel->ref();
  updateTrackLightModel(false);
  connect(m_d->editwindow_ui.checkBox_tracksUseBaseLightModel,SIGNAL(toggled(bool)),this,SLOT(updateTrackLightModel(bool)));
  connect(m_d->editwindow_ui.checkBox_hideactualpaths,SIGNAL(toggled(bool)),this,SLOT(updateHideActualTrackPath(bool)));
  
  #if defined BUILDVP1LIGHT
    m_d->editwindow_ui.radioButton_existingParameters->setChecked(true);
    m_d->editwindow_ui.radioButton_existingParameters->setEnabled(true);
    m_d->editwindow_ui.radioButton_extrapolate->setChecked(false);
    m_d->editwindow_ui.radioButton_extrapolate->setEnabled(false);
    m_d->editwindow_ui.radioButton_extrapolate->setToolTip("Not available in VP1 Light");
    m_d->editwindow_ui.horizontalSlider_numBezierSteps->setValue(10);
    m_d->editwindow_ui.horizontalSlider_numBezierSteps->setSliderPosition(10);
  #endif //BUILDVP1LIGHT

  // Propagation - for all of these, just emit the propagationOptionsChanged() signal and rely on clients checking to see what has changed.
  connect(m_d->editwindow_ui.radioButton_existingParameters, SIGNAL(toggled(bool)),    this, SIGNAL(propagationOptionsChanged()));
  connect(m_d->editwindow_ui.radioButton_extrapolate,        SIGNAL(toggled(bool)),    this, SIGNAL(propagationOptionsChanged()));
  connect(m_d->editwindow_ui.horizontalSlider_numBezierSteps,SIGNAL(valueChanged(int)),this, SIGNAL(propagationOptionsChanged()));

  // -> parameters
  connect(m_d->editwindow_ui.checkBox_showparameters,SIGNAL(toggled(bool)),this,SLOT(possibleChange_showParameters()));
  connect(m_d->editwindow_ui.checkBox_parameters_colourByType,SIGNAL(toggled(bool)),this,SLOT(possibleChange_parameterColours()));

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

  // -> cutRequiredNHits();
  connect(m_d->editwindow_ui.checkBox_cut_nhits_pixel,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutRequiredNHits()));
  connect(m_d->editwindow_ui.checkBox_cut_nhits_sct,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutRequiredNHits()));
  connect(m_d->editwindow_ui.checkBox_cut_nhits_trt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutRequiredNHits()));
  connect(m_d->editwindow_ui.checkBox_cut_nhits_muon,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutRequiredNHits()));
  connect(m_d->editwindow_ui.spinBox_cut_nhits_pixel,SIGNAL(valueChanged(int)),this,SLOT(possibleChange_cutRequiredNHits()));
  connect(m_d->editwindow_ui.spinBox_cut_nhits_sct,SIGNAL(valueChanged(int)),this,SLOT(possibleChange_cutRequiredNHits()));
  connect(m_d->editwindow_ui.spinBox_cut_nhits_trt,SIGNAL(valueChanged(int)),this,SLOT(possibleChange_cutRequiredNHits()));
  connect(m_d->editwindow_ui.spinBox_cut_nhits_muon,SIGNAL(valueChanged(int)),this,SLOT(possibleChange_cutRequiredNHits()));

  // -> cutOnlyVertexAssocTracks
  connect(m_d->editwindow_ui.checkBox_vertexAssociated,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutOnlyVertexAssocTracks()));
  
  connect(this,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
  connect(m_d->editwindow_ui.pushButton_close,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
  connect(m_d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(updateButton()));
  connect(m_d->matButton,SIGNAL(lastAppliedChanged()),this,SIGNAL(lastAppliedChanged()));
  setAcceptDrops(true);
  
  QTimer::singleShot(0, this, SLOT(updateButton()));
  
}

// QWidget& TrackParticleCollectionSettingsButton::editWindow() {
//   if (!m_d->editwindow)
//     initEditWindow();
//   return *(m_d->editwindow);
// } 
TrackParticleCollectionSettingsButton::~TrackParticleCollectionSettingsButton()
{
  delete m_d->editwindow;
  m_d->trackDrawStyle->unref();
  m_d->trackLightModel->unref();
  delete m_d;
}

void TrackParticleCollectionSettingsButton::updateButton()
{
  if (objectName().isEmpty())
    setObjectName("TrackParticleCollectionSettingsButton");
  messageVerbose("setColButtonProperties: color=" + str(m_d->matButton->lastAppliedDiffuseColour()));
  VP1ColorSelectButton::setColButtonProperties(this,m_d->matButton->lastAppliedDiffuseColour(),m_d->dim);
}

void TrackParticleCollectionSettingsButton::setDimension(int dim)
{
  if (m_d->dim == dim)
    return;
  m_d->dim = dim;
  updateButton();
}

void TrackParticleCollectionSettingsButton::showEditMaterialDialog()
{
  if (!m_d->editwindow)
    m_d->initEditWindow();

  if (m_d->editwindow->isHidden())
    m_d->editwindow->show();
  else
    m_d->editwindow->hide();
}

bool TrackParticleCollectionSettingsButton::setMaterial(SoMaterial*mat)
{  
	// std::cout<<"TrackParticleCollectionSettingsButton::setMaterial with mat="<<mat<<std::endl;
  if (!m_d->matButton) m_d->initEditWindow();
  m_d->matButton->setMaterial(mat);
  return true;
}

void TrackParticleCollectionSettingsButton::copyValuesFromMaterial(SoMaterial*mat)
{
  if (!m_d->matButton) m_d->initEditWindow();
  m_d->matButton->setMaterial(mat);
}
double TrackParticleCollectionSettingsButton::lastAppliedTransparency() const 
{
  if (!m_d->matButton) m_d->initEditWindow();
  return m_d->matButton->lastAppliedTransparency();
}
double TrackParticleCollectionSettingsButton::lastAppliedShininess() const  
{
  if (!m_d->matButton) m_d->initEditWindow();
  return m_d->matButton->lastAppliedShininess();
}
double TrackParticleCollectionSettingsButton::lastAppliedBrightness() const
{
  if (!m_d->matButton) m_d->initEditWindow();
  return m_d->matButton->lastAppliedBrightness();
}

double TrackParticleCollectionSettingsButton::trackTubeRadius() const
{
  return m_d->editwindow_ui.checkBox_trackTubes->isChecked() ?
    m_d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM->value() : 0.0;
}

void TrackParticleCollectionSettingsButton::updateTrackTubeRadius()
{
  if (m_d->last_trackTubeRadius==trackTubeRadius()) return;
  m_d->last_trackTubeRadius=trackTubeRadius();
  messageVerbose("TrackTubeRadius changed to "+str(m_d->last_trackTubeRadius));
  emit trackTubeRadiusChanged(m_d->last_trackTubeRadius);
  return;
}

void TrackParticleCollectionSettingsButton::updateTrackDrawStyle()
{
  double val = VP1QtInventorUtils::getValueLineWidthSlider(m_d->editwindow_ui.horizontalSlider_trackWidth);
  if (m_d->trackDrawStyle->lineWidth.getValue()!=val)
    m_d->trackDrawStyle->lineWidth = val;
}

void TrackParticleCollectionSettingsButton::updateTrackLightModel(bool base)
{
  if (m_d->trackLightModel->model.getValue()!=(base?SoLightModel::BASE_COLOR:SoLightModel::PHONG)) {
    messageVerbose("TrackLightModel changed (base = "+str(base));
    if (base)
      m_d->trackLightModel->model.setValue(SoLightModel::BASE_COLOR);
    else
      m_d->trackLightModel->model.setValue(SoLightModel::PHONG);
  }
}

void TrackParticleCollectionSettingsButton::updateHideActualTrackPath(bool hide)
{
  emit hideActualTrackPathChanged(hide);
}


SoDrawStyle * TrackParticleCollectionSettingsButton::trackDrawStyle() const
{
  return m_d->trackDrawStyle;
}

SoLightModel * TrackParticleCollectionSettingsButton::trackLightModel() const
{
  return m_d->trackLightModel;
}

bool TrackParticleCollectionSettingsButton::useExistingParameters() const{
  return m_d->editwindow_ui.radioButton_existingParameters->isChecked();
}

int TrackParticleCollectionSettingsButton::numOfStepsForInterpolation() const {
  return m_d->editwindow_ui.horizontalSlider_numBezierSteps->value();
}

SoMaterial* TrackParticleCollectionSettingsButton::defaultParameterMaterial() const {
  return m_d->defaultParametersMatButton->handledMaterials().at(0);// Only have one material per button.
}
SoMaterial* TrackParticleCollectionSettingsButton::parameterMaterial( xAOD::ParameterPosition position) const{
  return (m_d->parameterTypeMatButtons.at(static_cast<unsigned int>(position)))->handledMaterials().at(0);// Only have one material per button.
}

// void setDefaultParameterMaterial(SoMaterial* mat) {    
//     // m_d->defaultParametersMatButton->setObjectName("matButton_default");
//     // m_d->defaultParametersMatButton->setToolTip(matButtonToolTip());
//     m_d->defaultParametersMatButton->setMaterial(mat);
//     // connect(m_d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(possibleChangeMatTranspOrBrightness()));
// }
// 
// void TrackParticleCollectionSettingsButton::setParameterMaterial(SoMaterial*, xAOD::ParameterPosition){
//   // FIXME!
// }

bool  TrackParticleCollectionSettingsButton::hideActualTrackPath() const
{
  if (!m_d->editwindow)
    m_d->initEditWindow();
  return m_d->editwindow_ui.checkBox_hideactualpaths->isChecked();
}

bool TrackParticleCollectionSettingsButton::showParameters() const
{
  if (!m_d->editwindow)
    m_d->initEditWindow();
  messageVerbose("TrackParticleCollectionSettingsButton::showParameters returning  "+str(m_d->editwindow_ui.checkBox_showparameters->isChecked()));
  return m_d->editwindow_ui.checkBox_showparameters->isChecked();
}

bool TrackParticleCollectionSettingsButton::colourParametersByType() const
{
  if (!m_d->editwindow)
    m_d->initEditWindow();
  messageVerbose("TrackParticleCollectionSettingsButton::colourParametersByType returning  "+str(m_d->editwindow_ui.checkBox_parameters_colourByType->isChecked()));
  return m_d->editwindow_ui.checkBox_parameters_colourByType->isChecked();
}


bool  TrackParticleCollectionSettingsButton::cutOnlyVertexAssocTracks() const
{
  if (!m_d->editwindow)
    m_d->initEditWindow();
  return m_d->editwindow_ui.checkBox_vertexAssociated->isChecked();
}

//____________________________________________________________________
void TrackParticleCollectionSettingsButton::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    m_d->dragStartPosition = event->pos();
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
void TrackParticleCollectionSettingsButton::dropEvent(QDropEvent *event)
{
  QByteArray data = event->mimeData()->data("vp1/material");
  event->acceptProposedAction();
  restoreFromState(data);  
}


QByteArray TrackParticleCollectionSettingsButton::saveState() const{
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
  
  // Parameters
  serialise.save(m_d->editwindow_ui.checkBox_showparameters);  
  serialise.save(m_d->editwindow_ui.checkBox_parameters_colourByType);  
  
  serialise.widgetHandled(this);
  serialise.warnUnsaved(this);
  return serialise.result();
}

void TrackParticleCollectionSettingsButton::restoreFromState( const QByteArray& ba){
   
  VP1Deserialise state(ba,systemBase());
  if (state.version()<0||state.version()>1)
    return;//Ignore silently
  state.restore(m_d->matButton);
  state.restore(m_d->editwindow_ui.horizontalSlider_trackWidth);
  state.restore(m_d->editwindow_ui.checkBox_trackTubes);
  state.restore(m_d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM);
  state.restore(m_d->editwindow_ui.checkBox_tracksUseBaseLightModel);
  state.restore(m_d->editwindow_ui.checkBox_hideactualpaths);

  // Parameters
  state.restore(m_d->editwindow_ui.checkBox_showparameters);  
  state.restore(m_d->editwindow_ui.checkBox_parameters_colourByType);  

  state.widgetHandled(this);
  state.warnUnrestored(this);

  updateTrackTubeRadius();
  updateTrackLightModel(m_d->editwindow_ui.checkBox_tracksUseBaseLightModel);
  updateButton();
  //FIXME - anything else need updating?
}

//____________________________________________________________________
VP1Interval TrackParticleCollectionSettingsButton::cutAllowedPt() const
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
VP1Interval TrackParticleCollectionSettingsButton::cutAllowedEta() const
{
  if (!m_d->editwindow)
    m_d->initEditWindow();
  return m_d->editwindow_ui.etaPhiCutWidget->allowedEta();
}

//____________________________________________________________________
QList<VP1Interval> TrackParticleCollectionSettingsButton::cutAllowedPhi() const
{
  if (!m_d->editwindow)
    m_d->initEditWindow();
  return m_d->editwindow_ui.etaPhiCutWidget->allowedPhi();
}

//____________________________________________________________________
QList<unsigned> TrackParticleCollectionSettingsButton::cutRequiredNHits() const
{
  if (!m_d->editwindow)
    m_d->initEditWindow();
  unsigned npixel = m_d->editwindow_ui.checkBox_cut_nhits_pixel->isChecked() ? m_d->editwindow_ui.spinBox_cut_nhits_pixel->value() : 0;
  unsigned nsct = m_d->editwindow_ui.checkBox_cut_nhits_sct->isChecked() ? m_d->editwindow_ui.spinBox_cut_nhits_sct->value() : 0;
  unsigned ntrt = m_d->editwindow_ui.checkBox_cut_nhits_trt->isChecked() ? m_d->editwindow_ui.spinBox_cut_nhits_trt->value() : 0;
  unsigned nmuon = m_d->editwindow_ui.checkBox_cut_nhits_muon->isChecked() ? m_d->editwindow_ui.spinBox_cut_nhits_muon->value() : 0;
  QList<unsigned> l;
  if (!npixel&&!nsct&&!ntrt&&!nmuon)
    return l;
  l << npixel << nsct << ntrt << nmuon;
  return l;
}


void TrackParticleCollectionSettingsButton::possibleChange_cutAllowedPt()
{
  messageVerbose("possibleChange_cutAllowedPt() ");
  
  if (m_d->last_cutAllowedPt==cutAllowedPt()) return;
  messageVerbose("cutAllowedPt() changed");
  m_d->last_cutAllowedPt= cutAllowedPt();
  emit cutAllowedPtChanged(m_d->last_cutAllowedPt);
}

void TrackParticleCollectionSettingsButton::possibleChange_cutAllowedEta()
{
  if (m_d->last_cutAllowedEta==cutAllowedEta()) return;
  messageVerbose("cutAllowedEta() changed");
  m_d->last_cutAllowedEta=cutAllowedEta();
  emit cutAllowedEtaChanged(m_d->last_cutAllowedEta);
}

void TrackParticleCollectionSettingsButton::possibleChange_cutAllowedPhi()
{
  if (m_d->last_cutAllowedPhi==cutAllowedPhi()) return;
  messageVerbose("cutAllowedPhi() changed");
  m_d->last_cutAllowedPhi=cutAllowedPhi();
  emit cutAllowedPhiChanged(m_d->last_cutAllowedPhi);
}

void TrackParticleCollectionSettingsButton::possibleChange_cutRequiredNHits()
{
  if (m_d->last_cutRequiredNHits!=cutRequiredNHits()) return;
  messageVerbose("cutRequiredNHits() changed");
  m_d->last_cutRequiredNHits=cutRequiredNHits();
  emit cutRequiredNHitsChanged(m_d->last_cutRequiredNHits);
}

void TrackParticleCollectionSettingsButton::possibleChange_cutOnlyVertexAssocTracks()
{
  if (m_d->last_cutOnlyVertexAssocTracks!=cutOnlyVertexAssocTracks()) return;
  messageVerbose("cutOnlyVertexAssocTracks() changed");
  m_d->last_cutOnlyVertexAssocTracks=cutOnlyVertexAssocTracks();
  emit cutOnlyVertexAssocTracksChanged(m_d->last_cutOnlyVertexAssocTracks);
}

void TrackParticleCollectionSettingsButton::possibleChange_showParameters()
{
  messageVerbose("TrackParticleCollectionSettingsButton::possibleChange_showParameters emitting showParametersChanged  ");
	emit showParametersChanged(m_d->editwindow_ui.checkBox_showparameters->isChecked());
}

void TrackParticleCollectionSettingsButton::possibleChange_parameterColours()
{
  messageVerbose("TrackParticleCollectionSettingsButton::possibleChange_parameterColours emitting  colourParametersByTypeChanged ");
	emit colourParametersByTypeChanged(m_d->editwindow_ui.checkBox_parameters_colourByType->isChecked());
}

