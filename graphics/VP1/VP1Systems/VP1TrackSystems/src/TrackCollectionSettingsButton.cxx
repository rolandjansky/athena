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
  if (d->editwindow)
    d->editwindow->setWindowTitle(t);
  setToolTip(t);
}

//____________________________________________________________________
TrackCollectionSettingsButton::TrackCollectionSettingsButton(QWidget * parent,int _dim)
  : VP1MaterialButtonBase(parent,0,"VP1MaterialButton"), d(new Imp)
{
  d->dim = _dim;
  
  d->theclass = this;
  d->initEditWindow();
  
  //Draw Styles / Complexity:
  VP1QtInventorUtils::setLimitsLineWidthSlider(d->editwindow_ui.horizontalSlider_trackWidth);
  VP1QtInventorUtils::setValueLineWidthSlider(d->editwindow_ui.horizontalSlider_trackWidth,1.0);  
  
  d->trackDrawStyle = new SoDrawStyle;
  d->trackDrawStyle->setName("TrackDrawStyle");
  d->trackDrawStyle->ref();
  updateTrackDrawStyle();
  connect(d->editwindow_ui.horizontalSlider_trackWidth,SIGNAL(valueChanged(int)),this,SLOT(updateTrackDrawStyle()));
  
  d->trackLightModel = new SoLightModel;
  d->trackLightModel->setName("TrackLightModel");
  d->trackLightModel->ref();
  updateTrackLightModel(false);
  connect(d->editwindow_ui.checkBox_tracksUseBaseLightModel,SIGNAL(toggled(bool)),this,SLOT(updateTrackLightModel(bool)));
  
  d->last_trackTubeRadius=trackTubeRadius();
  connect(d->editwindow_ui.checkBox_trackTubes,SIGNAL(toggled(bool)),this,SLOT(updateTrackTubeRadius()));
  connect(d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM,SIGNAL(valueChanged(double)),this,SLOT(updateTrackTubeRadius()));
  
  connect(d->editwindow_ui.checkBox_hideactualpaths,SIGNAL(toggled(bool)),this,SLOT(updateHideActualTrackPath(bool)));

  // Cuts
  connect(d->editwindow_ui.checkBox_defaultCuts,SIGNAL(toggled(bool)),this,SLOT(possibleChange_useDefaultCuts()));
  d->last_useDefaultCuts=d->editwindow_ui.checkBox_defaultCuts->isChecked();
  // -> cutAllowedP/Pt
  connect(d->editwindow_ui.checkBox_cut_minpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
  connect(d->editwindow_ui.checkBox_cut_maxpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
  connect(d->editwindow_ui.doubleSpinBox_cut_minpt_gev,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedPt()));
  connect(d->editwindow_ui.doubleSpinBox_cut_maxpt_gev,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedPt()));
  connect(d->editwindow_ui.checkBox_cut_minpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
  //connect(d->editwindow_ui.comboBox_momtype,SIGNAL(valueChanged(bool)),this,SLOT(possibleChange_cutAllowedPt()));
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

  // -> cutTruthFromIROnly
  connect(d->editwindow_ui.checkBox_cut_truthtracks_creationvertexinIR,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutTruthFromIROnly()));
  
  // -> cutExcludeBarcodeZero
  connect(d->editwindow_ui.checkBox_cut_truthtracks_excludebarcode0,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutExcludeBarcodeZero()));
  
  // -> cutTruthExcludeNeutrals
  connect(d->editwindow_ui.checkBox_cut_truthtracks_excludeneutrals,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutTruthExcludeNeutrals()));
  
  // -> cutOnlyVertexAssocTracks
  connect(d->editwindow_ui.checkBox_vertexAssociated,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutOnlyVertexAssocTracks()));
  
  connect(this,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
  connect(d->editwindow_ui.pushButton_close,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
  connect(d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(updateButton()));
  connect(d->matButton,SIGNAL(lastAppliedChanged()),this,SIGNAL(lastAppliedChanged()));
  setAcceptDrops(true);
  
  QTimer::singleShot(0, this, SLOT(updateButton()));
  
}

// QWidget& TrackCollectionSettingsButton::editWindow() {
//   if (!d->editwindow)
//     initEditWindow();
//   return *(d->editwindow);
// } 
TrackCollectionSettingsButton::~TrackCollectionSettingsButton()
{
  delete d->editwindow;
  d->trackDrawStyle->unref();
  d->trackLightModel->unref();
  delete d;
}

void TrackCollectionSettingsButton::updateButton()
{
  if (objectName().isEmpty())
    setObjectName("TrackCollectionSettingsButton");
  messageVerbose("setColButtonProperties: color=" + str(d->matButton->lastAppliedDiffuseColour()));
  VP1ColorSelectButton::setColButtonProperties(this,d->matButton->lastAppliedDiffuseColour(),d->dim);
}

void TrackCollectionSettingsButton::setDimension(int _dim)
{
  if (d->dim == _dim)
    return;
  d->dim = _dim;
  updateButton();
}

void TrackCollectionSettingsButton::showEditMaterialDialog()
{
  if (!d->editwindow)
    d->initEditWindow();

  if (d->editwindow->isHidden())
    d->editwindow->show();
  else
    d->editwindow->hide();
}

bool TrackCollectionSettingsButton::setMaterial(SoMaterial*mat)
{  
  if (!d->matButton) d->initEditWindow();
  d->matButton->setMaterial(mat);
  return true;
}

void TrackCollectionSettingsButton::copyValuesFromMaterial(SoMaterial*mat)
{
  if (!d->matButton) d->initEditWindow();
  d->matButton->setMaterial(mat);
}
double TrackCollectionSettingsButton::lastAppliedTransparency() const 
{
  if (!d->matButton) d->initEditWindow();
  return d->matButton->lastAppliedTransparency();
}
double TrackCollectionSettingsButton::lastAppliedShininess() const  
{
  if (!d->matButton) d->initEditWindow();
  return d->matButton->lastAppliedShininess();
}
double TrackCollectionSettingsButton::lastAppliedBrightness() const
{
  if (!d->matButton) d->initEditWindow();
  return d->matButton->lastAppliedBrightness();
}

double TrackCollectionSettingsButton::trackTubeRadius() const
{
  return d->editwindow_ui.checkBox_trackTubes->isChecked() ?
    d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM->value()*CLHEP::mm : 0.0;
}

void TrackCollectionSettingsButton::updateTrackTubeRadius()
{
  if (d->last_trackTubeRadius==trackTubeRadius()) return;
  d->last_trackTubeRadius=trackTubeRadius();
  messageVerbose("TrackTubeRadius changed to "+str(d->last_trackTubeRadius));
  emit trackTubeRadiusChanged(d->last_trackTubeRadius);
  return;
}

void TrackCollectionSettingsButton::updateTrackDrawStyle()
{
  double val = VP1QtInventorUtils::getValueLineWidthSlider(d->editwindow_ui.horizontalSlider_trackWidth);
  if (d->trackDrawStyle->lineWidth.getValue()!=val)
    d->trackDrawStyle->lineWidth = val;
}

void TrackCollectionSettingsButton::updateTrackLightModel(bool base)
{
  if (d->trackLightModel->model.getValue()!=(base?SoLightModel::BASE_COLOR:SoLightModel::PHONG)) {
    messageVerbose("TrackLightModel changed (base = "+str(base));
    if (base)
      d->trackLightModel->model.setValue(SoLightModel::BASE_COLOR);
    else
      d->trackLightModel->model.setValue(SoLightModel::PHONG);
  }
}

void TrackCollectionSettingsButton::updateHideActualTrackPath(bool hide)
{
  emit hideActualTrackPathChanged(hide);
}


SoDrawStyle * TrackCollectionSettingsButton::trackDrawStyle() const
{
  return d->trackDrawStyle;
}

SoLightModel * TrackCollectionSettingsButton::trackLightModel() const
{
  return d->trackLightModel;
}

bool  TrackCollectionSettingsButton::hideActualTrackPath() const
{
  return d->editwindow_ui.checkBox_hideactualpaths->isChecked();
}


bool  TrackCollectionSettingsButton::useDefaultCuts() const
{
  return d->editwindow_ui.checkBox_defaultCuts->isChecked();
}

bool  TrackCollectionSettingsButton::cutOnlyVertexAssocTracks() const
{
  return d->editwindow_ui.checkBox_vertexAssociated->isChecked();
}

//____________________________________________________________________
void TrackCollectionSettingsButton::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    d->dragStartPosition = event->pos();
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
void TrackCollectionSettingsButton::dropEvent(QDropEvent *event)
{
  QByteArray data = event->mimeData()->data("vp1/material");
  event->acceptProposedAction();
  restoreFromState(data);  
}


QByteArray TrackCollectionSettingsButton::saveState() const{
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
  serialise.save(d->editwindow_ui.checkBox_defaultCuts);
  serialise.widgetHandled(this);
  serialise.warnUnsaved(this);
  return serialise.result();
}

void TrackCollectionSettingsButton::restoreFromState( const QByteArray& ba){
   
  VP1Deserialise state(ba,systemBase());
  if (state.version()<0||state.version()>1)
    return;//Ignore silently
  state.restore(d->matButton);
  state.restore(d->editwindow_ui.horizontalSlider_trackWidth);
  state.restore(d->editwindow_ui.checkBox_trackTubes);
  state.restore(d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM);
  state.restore(d->editwindow_ui.checkBox_tracksUseBaseLightModel);
  state.restore(d->editwindow_ui.checkBox_hideactualpaths);
  state.restore(d->editwindow_ui.checkBox_defaultCuts);

  state.widgetHandled(this);
  state.warnUnrestored(this);

  updateTrackTubeRadius();
  updateTrackLightModel(d->editwindow_ui.checkBox_tracksUseBaseLightModel);
  updateButton();
  //FIXME - anything else need updating?
}

//____________________________________________________________________
VP1Interval TrackCollectionSettingsButton::cutAllowedPt() const
{
  if (!d->editwindow_ui.checkBox_cut_minpt)
    return VP1Interval();

  // will set range to negative if we have momcut=P
  // if minCut unset then min=-inf
  // if minCut set, and Pt selected, then min=-minCut
  // if minCut set, and P selected, then min=-maxCut
  // etc
  bool isPCut = d->editwindow_ui.comboBox_momtype->currentText()=="P";
  
  const double minFromInterface=d->editwindow_ui.doubleSpinBox_cut_minpt_gev->value()*CLHEP::GeV;
  const double maxFromInterface=d->editwindow_ui.doubleSpinBox_cut_maxpt_gev->value()*CLHEP::GeV;
  
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
VP1Interval TrackCollectionSettingsButton::cutAllowedEta() const
{
  return d->editwindow_ui.etaPhiCutWidget->allowedEta();
}

//____________________________________________________________________
QList<VP1Interval> TrackCollectionSettingsButton::cutAllowedPhi() const
{
  return d->editwindow_ui.etaPhiCutWidget->allowedPhi();
}

//____________________________________________________________________
QList<unsigned> TrackCollectionSettingsButton::cutRequiredNHits() const
{
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

//____________________________________________________________________
bool TrackCollectionSettingsButton::cutTruthFromIROnly() const
{
  return d->editwindow_ui.checkBox_cut_truthtracks_creationvertexinIR->isChecked();
}

//____________________________________________________________________
bool TrackCollectionSettingsButton::cutExcludeBarcodeZero() const
{
  return d->editwindow_ui.checkBox_cut_truthtracks_excludebarcode0->isChecked();
}

//____________________________________________________________________
bool TrackCollectionSettingsButton::cutTruthExcludeNeutrals() const
{
  return d->editwindow_ui.checkBox_cut_truthtracks_excludeneutrals->isChecked();
}

void TrackCollectionSettingsButton::possibleChange_useDefaultCuts()
{
  if (d->last_useDefaultCuts==useDefaultCuts()) return;
  messageVerbose("useDefaultCuts() changed");
  d->last_useDefaultCuts= useDefaultCuts();
  emit useDefaultCutsChanged(d->last_useDefaultCuts);
}

void TrackCollectionSettingsButton::possibleChange_cutAllowedPt()
{
  messageVerbose("possibleChange_cutAllowedPt() ");
  
  if (d->last_cutAllowedPt==cutAllowedPt()) return;
  messageVerbose("cutAllowedPt() changed");
  d->last_cutAllowedPt= cutAllowedPt();
  emit cutAllowedPtChanged(d->last_cutAllowedPt);
}

void TrackCollectionSettingsButton::possibleChange_cutAllowedEta()
{
  if (d->last_cutAllowedEta==cutAllowedEta()) return;
  messageVerbose("cutAllowedEta() changed");
  d->last_cutAllowedEta=cutAllowedEta();
  emit cutAllowedEtaChanged(d->last_cutAllowedEta);
}

void TrackCollectionSettingsButton::possibleChange_cutAllowedPhi()
{
  if (d->last_cutAllowedPhi==cutAllowedPhi()) return;
  messageVerbose("cutAllowedPhi() changed");
  d->last_cutAllowedPhi=cutAllowedPhi();
  emit cutAllowedPhiChanged(d->last_cutAllowedPhi);
}

void TrackCollectionSettingsButton::possibleChange_cutRequiredNHits()
{
  if (d->last_cutRequiredNHits!=cutRequiredNHits()) return;
  messageVerbose("cutRequiredNHits() changed");
  d->last_cutRequiredNHits=cutRequiredNHits();
  emit cutRequiredNHitsChanged(d->last_cutRequiredNHits);
}

void TrackCollectionSettingsButton::possibleChange_cutTruthFromIROnly()
{
  if (d->last_cutTruthFromIROnly!=cutTruthFromIROnly()) return;
  messageVerbose("cutTruthFromIROnly() changed");
  d->last_cutTruthFromIROnly=cutTruthFromIROnly();
  emit cutTruthFromIROnlyChanged(d->last_cutTruthFromIROnly);
}

void TrackCollectionSettingsButton::possibleChange_cutExcludeBarcodeZero()
{
  if (d->last_cutExcludeBarcodeZero!=cutExcludeBarcodeZero()) return;
  messageVerbose("cutExcludeBarcodeZero() changed");
  d->last_cutExcludeBarcodeZero=cutExcludeBarcodeZero();
  emit cutExcludeBarcodeZeroChanged(d->last_cutExcludeBarcodeZero);
}


void TrackCollectionSettingsButton::possibleChange_cutTruthExcludeNeutrals()
{
  if (d->last_cutTruthExcludeNeutrals!=cutTruthExcludeNeutrals()) return;
  messageVerbose("cutTruthExcludeNeutrals() changed");
  d->last_cutTruthExcludeNeutrals=cutTruthExcludeNeutrals();
  emit cutTruthExcludeNeutralsChanged(d->last_cutTruthExcludeNeutrals);
}


