/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1TrackSystems/TrackCollectionSettingsButton.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1ColorSelectButton.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"

#include "ui_pertrackcollectionsettings_form.h"
#include "ui_settings_cuts_form.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "Inventor/nodes/SoDrawStyle.h"
#include "Inventor/nodes/SoLightModel.h"
#include <iostream>

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


//____________________________________________________________________
class TrackCollectionSettingsButton::Imp {
public:
  Imp():theclass(0),editwindow(0),matButton(0), trackDrawStyle(0), trackLightModel(0),
  last_trackTubeRadius(0.0),last_cutTruthFromIROnly{}, last_cutExcludeBarcodeZero{}, 
  last_cutTruthExcludeNeutrals{},last_cutOnlyVertexAssocTracks{},last_useDefaultCuts{},
  dim(0){
    //nop
  }
  TrackCollectionSettingsButton * theclass;
  QWidget * editwindow;
  Ui::TrackCollectionSettingsForm editwindow_ui;
  
  VP1MaterialButton* matButton;
  
  SoDrawStyle * trackDrawStyle;
  SoLightModel * trackLightModel;
  
  //GUI - last values
  double last_trackTubeRadius;
  VP1Interval last_cutAllowedPt;
  VP1Interval last_cutAllowedP;
  VP1Interval last_cutAllowedEta;
  QList<VP1Interval> last_cutAllowedPhi;
  QList<unsigned> last_cutRequiredNHits;
  bool last_cutTruthFromIROnly;
  bool last_cutExcludeBarcodeZero;
  bool last_cutTruthExcludeNeutrals;
  bool last_cutOnlyVertexAssocTracks;
  bool last_useDefaultCuts;
  
  int dim;
  QPoint dragStartPosition;
  
  void initEditWindow();
};

//____________________________________________________________________
void TrackCollectionSettingsButton::Imp::initEditWindow()
{
  if (editwindow)
    return;
  theclass->messageVerbose("Initialising material editor dialog");
  editwindow = new QWidget(0,Qt::WindowStaysOnTopHint);
  editwindow_ui.setupUi(editwindow);
  matButton = editwindow_ui.pushButton_matButton;  

  // FIXME - remove when cuts implemented
  // editwindow_ui.checkBox_defaultCuts->hide();  
  // editwindow_ui.groupBox_cuts_momentum->hide();  
  // editwindow_ui.groupBox_cuts_reconstructed->hide();  
  // editwindow_ui.groupBox_cuts_truth_tracks->hide();  
  // editwindow_ui.checkBox_vertexAssociated->hide();  
  // editwindow->adjustSize();
  // std::cout<<" matButton "<<matButton<<std::endl;
}

//____________________________________________________________________
void TrackCollectionSettingsButton::setText(const QString& t)
{
  if (m_d->editwindow)
    m_d->editwindow->setWindowTitle(t);
  setToolTip(t);
}

//____________________________________________________________________
TrackCollectionSettingsButton::TrackCollectionSettingsButton(QWidget * parent,int dim)
  : VP1MaterialButtonBase(parent,0,"VP1MaterialButton"), m_d(new Imp)
{
  m_d->dim = dim;
  
  m_d->theclass = this;
  m_d->initEditWindow();
  
  //Draw Styles / Complexity:
  VP1QtInventorUtils::setLimitsLineWidthSlider(m_d->editwindow_ui.horizontalSlider_trackWidth);
  VP1QtInventorUtils::setValueLineWidthSlider(m_d->editwindow_ui.horizontalSlider_trackWidth,1.0);  
  
  m_d->trackDrawStyle = new SoDrawStyle;
  m_d->trackDrawStyle->setName("TrackDrawStyle");
  m_d->trackDrawStyle->ref();
  updateTrackDrawStyle();
  connect(m_d->editwindow_ui.horizontalSlider_trackWidth,SIGNAL(valueChanged(int)),this,SLOT(updateTrackDrawStyle()));
  
  m_d->trackLightModel = new SoLightModel;
  m_d->trackLightModel->setName("TrackLightModel");
  m_d->trackLightModel->ref();
  updateTrackLightModel(false);
  connect(m_d->editwindow_ui.checkBox_tracksUseBaseLightModel,SIGNAL(toggled(bool)),this,SLOT(updateTrackLightModel(bool)));
  
  m_d->last_trackTubeRadius=trackTubeRadius();
  connect(m_d->editwindow_ui.checkBox_trackTubes,SIGNAL(toggled(bool)),this,SLOT(updateTrackTubeRadius()));
  connect(m_d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM,SIGNAL(valueChanged(double)),this,SLOT(updateTrackTubeRadius()));
  
  connect(m_d->editwindow_ui.checkBox_hideactualpaths,SIGNAL(toggled(bool)),this,SLOT(updateHideActualTrackPath(bool)));

  // Cuts
  connect(m_d->editwindow_ui.checkBox_defaultCuts,SIGNAL(toggled(bool)),this,SLOT(possibleChange_useDefaultCuts()));
  m_d->last_useDefaultCuts=m_d->editwindow_ui.checkBox_defaultCuts->isChecked();
  // -> cutAllowedP/Pt
  connect(m_d->editwindow_ui.checkBox_cut_minpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
  connect(m_d->editwindow_ui.checkBox_cut_maxpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
  connect(m_d->editwindow_ui.doubleSpinBox_cut_minpt_gev,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedPt()));
  connect(m_d->editwindow_ui.doubleSpinBox_cut_maxpt_gev,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedPt()));
  connect(m_d->editwindow_ui.checkBox_cut_minpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
  //connect(m_d->editwindow_ui.comboBox_momtype,SIGNAL(valueChanged(bool)),this,SLOT(possibleChange_cutAllowedPt()));
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

  // -> cutTruthFromIROnly
  connect(m_d->editwindow_ui.checkBox_cut_truthtracks_creationvertexinIR,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutTruthFromIROnly()));
  
  // -> cutExcludeBarcodeZero
  connect(m_d->editwindow_ui.checkBox_cut_truthtracks_excludebarcode0,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutExcludeBarcodeZero()));
  
  // -> cutTruthExcludeNeutrals
  connect(m_d->editwindow_ui.checkBox_cut_truthtracks_excludeneutrals,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutTruthExcludeNeutrals()));
  
  // -> cutOnlyVertexAssocTracks
  connect(m_d->editwindow_ui.checkBox_vertexAssociated,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutOnlyVertexAssocTracks()));
  
  connect(this,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
  connect(m_d->editwindow_ui.pushButton_close,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
  connect(m_d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(updateButton()));
  connect(m_d->matButton,SIGNAL(lastAppliedChanged()),this,SIGNAL(lastAppliedChanged()));
  setAcceptDrops(true);
  
  QTimer::singleShot(0, this, SLOT(updateButton()));
  
}

// QWidget& TrackCollectionSettingsButton::editWindow() {
//   if (!m_d->editwindow)
//     initEditWindow();
//   return *(m_d->editwindow);
// } 
TrackCollectionSettingsButton::~TrackCollectionSettingsButton()
{
  delete m_d->editwindow;
  m_d->trackDrawStyle->unref();
  m_d->trackLightModel->unref();
  delete m_d;
}

void TrackCollectionSettingsButton::updateButton()
{
  if (objectName().isEmpty())
    setObjectName("TrackCollectionSettingsButton");
  messageVerbose("setColButtonProperties: color=" + str(m_d->matButton->lastAppliedDiffuseColour()));
  VP1ColorSelectButton::setColButtonProperties(this,m_d->matButton->lastAppliedDiffuseColour(),m_d->dim);
}

void TrackCollectionSettingsButton::setDimension(int dim)
{
  if (m_d->dim == dim)
    return;
  m_d->dim = dim;
  updateButton();
}

void TrackCollectionSettingsButton::showEditMaterialDialog()
{
  if (!m_d->editwindow)
    m_d->initEditWindow();

  if (m_d->editwindow->isHidden())
    m_d->editwindow->show();
  else
    m_d->editwindow->hide();
}

bool TrackCollectionSettingsButton::setMaterial(SoMaterial*mat)
{  
  if (!m_d->matButton) m_d->initEditWindow();
  m_d->matButton->setMaterial(mat);
  return true;
}

void TrackCollectionSettingsButton::copyValuesFromMaterial(SoMaterial*mat)
{
  if (!m_d->matButton) m_d->initEditWindow();
  m_d->matButton->setMaterial(mat);
}
double TrackCollectionSettingsButton::lastAppliedTransparency() const 
{
  if (!m_d->matButton) m_d->initEditWindow();
  return m_d->matButton->lastAppliedTransparency();
}
double TrackCollectionSettingsButton::lastAppliedShininess() const  
{
  if (!m_d->matButton) m_d->initEditWindow();
  return m_d->matButton->lastAppliedShininess();
}
double TrackCollectionSettingsButton::lastAppliedBrightness() const
{
  if (!m_d->matButton) m_d->initEditWindow();
  return m_d->matButton->lastAppliedBrightness();
}

double TrackCollectionSettingsButton::trackTubeRadius() const
{
  return m_d->editwindow_ui.checkBox_trackTubes->isChecked() ?
    m_d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM->value()*CLHEP::mm : 0.0;
}

void TrackCollectionSettingsButton::updateTrackTubeRadius()
{
  if (m_d->last_trackTubeRadius==trackTubeRadius()) return;
  m_d->last_trackTubeRadius=trackTubeRadius();
  messageVerbose("TrackTubeRadius changed to "+str(m_d->last_trackTubeRadius));
  emit trackTubeRadiusChanged(m_d->last_trackTubeRadius);
  return;
}

void TrackCollectionSettingsButton::updateTrackDrawStyle()
{
  double val = VP1QtInventorUtils::getValueLineWidthSlider(m_d->editwindow_ui.horizontalSlider_trackWidth);
  if (m_d->trackDrawStyle->lineWidth.getValue()!=val)
    m_d->trackDrawStyle->lineWidth = val;
}

void TrackCollectionSettingsButton::updateTrackLightModel(bool base)
{
  if (m_d->trackLightModel->model.getValue()!=(base?SoLightModel::BASE_COLOR:SoLightModel::PHONG)) {
    messageVerbose("TrackLightModel changed (base = "+str(base));
    if (base)
      m_d->trackLightModel->model.setValue(SoLightModel::BASE_COLOR);
    else
      m_d->trackLightModel->model.setValue(SoLightModel::PHONG);
  }
}

void TrackCollectionSettingsButton::updateHideActualTrackPath(bool hide)
{
  emit hideActualTrackPathChanged(hide);
}


SoDrawStyle * TrackCollectionSettingsButton::trackDrawStyle() const
{
  return m_d->trackDrawStyle;
}

SoLightModel * TrackCollectionSettingsButton::trackLightModel() const
{
  return m_d->trackLightModel;
}

bool  TrackCollectionSettingsButton::hideActualTrackPath() const
{
  return m_d->editwindow_ui.checkBox_hideactualpaths->isChecked();
}


bool  TrackCollectionSettingsButton::useDefaultCuts() const
{
  return m_d->editwindow_ui.checkBox_defaultCuts->isChecked();
}

bool  TrackCollectionSettingsButton::cutOnlyVertexAssocTracks() const
{
  return m_d->editwindow_ui.checkBox_vertexAssociated->isChecked();
}

//____________________________________________________________________
void TrackCollectionSettingsButton::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    m_d->dragStartPosition = event->pos();
  QPushButton::mousePressEvent(event);
}

//____________________________________________________________________
void TrackCollectionSettingsButton::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->source()!=this && event->mimeData()->hasFormat("vp1/material"))
    event->acceptProposedAction();
}

//____________________________________________________________________
void TrackCollectionSettingsButton::mouseMoveEvent(QMouseEvent *event)
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
void TrackCollectionSettingsButton::dropEvent(QDropEvent *event)
{
  QByteArray data = event->mimeData()->data("vp1/material");
  event->acceptProposedAction();
  restoreFromState(data);  
}


QByteArray TrackCollectionSettingsButton::saveState() const{
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
  serialise.save(m_d->editwindow_ui.checkBox_defaultCuts);
  serialise.widgetHandled(this);
  serialise.warnUnsaved(this);
  return serialise.result();
}

void TrackCollectionSettingsButton::restoreFromState( const QByteArray& ba){
   
  VP1Deserialise state(ba,systemBase());
  if (state.version()<0||state.version()>1)
    return;//Ignore silently
  state.restore(m_d->matButton);
  state.restore(m_d->editwindow_ui.horizontalSlider_trackWidth);
  state.restore(m_d->editwindow_ui.checkBox_trackTubes);
  state.restore(m_d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM);
  state.restore(m_d->editwindow_ui.checkBox_tracksUseBaseLightModel);
  state.restore(m_d->editwindow_ui.checkBox_hideactualpaths);
  state.restore(m_d->editwindow_ui.checkBox_defaultCuts);

  state.widgetHandled(this);
  state.warnUnrestored(this);

  updateTrackTubeRadius();
  updateTrackLightModel(m_d->editwindow_ui.checkBox_tracksUseBaseLightModel);
  updateButton();
  //FIXME - anything else need updating?
}

//____________________________________________________________________
VP1Interval TrackCollectionSettingsButton::cutAllowedPt() const
{
  if (!m_d->editwindow_ui.checkBox_cut_minpt)
    return VP1Interval();

  // will set range to negative if we have momcut=P
  // if minCut unset then min=-inf
  // if minCut set, and Pt selected, then min=-minCut
  // if minCut set, and P selected, then min=-maxCut
  // etc
  bool isPCut = m_d->editwindow_ui.comboBox_momtype->currentText()=="P";
  
  const double minFromInterface=m_d->editwindow_ui.doubleSpinBox_cut_minpt_gev->value()*CLHEP::GeV;
  const double maxFromInterface=m_d->editwindow_ui.doubleSpinBox_cut_maxpt_gev->value()*CLHEP::GeV;
  
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
VP1Interval TrackCollectionSettingsButton::cutAllowedEta() const
{
  return m_d->editwindow_ui.etaPhiCutWidget->allowedEta();
}

//____________________________________________________________________
QList<VP1Interval> TrackCollectionSettingsButton::cutAllowedPhi() const
{
  return m_d->editwindow_ui.etaPhiCutWidget->allowedPhi();
}

//____________________________________________________________________
QList<unsigned> TrackCollectionSettingsButton::cutRequiredNHits() const
{
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

//____________________________________________________________________
bool TrackCollectionSettingsButton::cutTruthFromIROnly() const
{
  return m_d->editwindow_ui.checkBox_cut_truthtracks_creationvertexinIR->isChecked();
}

//____________________________________________________________________
bool TrackCollectionSettingsButton::cutExcludeBarcodeZero() const
{
  return m_d->editwindow_ui.checkBox_cut_truthtracks_excludebarcode0->isChecked();
}

//____________________________________________________________________
bool TrackCollectionSettingsButton::cutTruthExcludeNeutrals() const
{
  return m_d->editwindow_ui.checkBox_cut_truthtracks_excludeneutrals->isChecked();
}



// "POSSIBLECHANGE" SLOTS
//____________________________________________________________________
void TrackCollectionSettingsButton::possibleChange_useDefaultCuts()
{
  if (m_d->last_useDefaultCuts==useDefaultCuts()) return;
  messageVerbose("useDefaultCuts() changed");
  m_d->last_useDefaultCuts= useDefaultCuts();
  emit useDefaultCutsChanged(m_d->last_useDefaultCuts);
}
//____________________________________________________________________
void TrackCollectionSettingsButton::possibleChange_cutAllowedPt()
{
  messageVerbose("possibleChange_cutAllowedPt() ");
  
  if (m_d->last_cutAllowedPt==cutAllowedPt()) return;
  messageVerbose("cutAllowedPt() changed");
  m_d->last_cutAllowedPt= cutAllowedPt();
  emit cutAllowedPtChanged(m_d->last_cutAllowedPt);
}
//____________________________________________________________________
void TrackCollectionSettingsButton::possibleChange_cutAllowedEta()
{
  if (m_d->last_cutAllowedEta==cutAllowedEta()) return;
  messageVerbose("cutAllowedEta() changed");
  m_d->last_cutAllowedEta=cutAllowedEta();
  emit cutAllowedEtaChanged(m_d->last_cutAllowedEta);
}
//____________________________________________________________________
void TrackCollectionSettingsButton::possibleChange_cutAllowedPhi()
{
  if (m_d->last_cutAllowedPhi==cutAllowedPhi()) return;
  messageVerbose("cutAllowedPhi() changed");
  m_d->last_cutAllowedPhi=cutAllowedPhi();
  emit cutAllowedPhiChanged(m_d->last_cutAllowedPhi);
}
//____________________________________________________________________
void TrackCollectionSettingsButton::possibleChange_cutRequiredNHits()
{
  if (m_d->last_cutRequiredNHits!=cutRequiredNHits()) return;
  messageVerbose("cutRequiredNHits() changed");
  m_d->last_cutRequiredNHits=cutRequiredNHits();
  emit cutRequiredNHitsChanged(m_d->last_cutRequiredNHits);
}
//____________________________________________________________________
void TrackCollectionSettingsButton::possibleChange_cutTruthFromIROnly()
{
  if (m_d->last_cutTruthFromIROnly!=cutTruthFromIROnly()) return;
  messageVerbose("cutTruthFromIROnly() changed");
  m_d->last_cutTruthFromIROnly=cutTruthFromIROnly();
  emit cutTruthFromIROnlyChanged(m_d->last_cutTruthFromIROnly);
}
//____________________________________________________________________
void TrackCollectionSettingsButton::possibleChange_cutExcludeBarcodeZero()
{
  if (m_d->last_cutExcludeBarcodeZero!=cutExcludeBarcodeZero()) return;
  messageVerbose("cutExcludeBarcodeZero() changed");
  m_d->last_cutExcludeBarcodeZero=cutExcludeBarcodeZero();
  emit cutExcludeBarcodeZeroChanged(m_d->last_cutExcludeBarcodeZero);
}

//____________________________________________________________________
void TrackCollectionSettingsButton::possibleChange_cutTruthExcludeNeutrals()
{
  if (m_d->last_cutTruthExcludeNeutrals!=cutTruthExcludeNeutrals()) return;
  messageVerbose("cutTruthExcludeNeutrals() changed");
  m_d->last_cutTruthExcludeNeutrals=cutTruthExcludeNeutrals();
  emit cutTruthExcludeNeutralsChanged(m_d->last_cutTruthExcludeNeutrals);
}

//____________________________________________________________________
void TrackCollectionSettingsButton::possibleChange_cutOnlyVertexAssocTracks()
{
  if (m_d->last_cutOnlyVertexAssocTracks!=cutOnlyVertexAssocTracks()) return;
  messageVerbose("cutOnlyVertexAssocTracks() changed");
  m_d->last_cutOnlyVertexAssocTracks=cutOnlyVertexAssocTracks();
  emit cutOnlyVertexAssocTracksChanged(m_d->last_cutOnlyVertexAssocTracks);
}
