/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local
#include "MissingEtCollectionSettingsButton.h"

#include "ui_permissingetcollectionsettings_form.h"

//VP1
#include "VP1Base/VP1ColorSelectButton.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Interval.h"

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
class MissingEtCollectionSettingsButton::Imp {
public:
  Imp():theclass(0),editwindow(0),matButton(0), dim(0),vertexDrawStyle(0), vertexLightModel(0),
  last_vertexRadius(0.0){
    //nop
  }
  MissingEtCollectionSettingsButton * theclass;
  QWidget * editwindow;
  Ui::MissingEtSettingsForm editwindow_ui;
  
  VP1MaterialButton* matButton;// main collection colour
  
  int dim;
  
  SoDrawStyle * vertexDrawStyle;
  SoLightModel * vertexLightModel;
  
  //GUI - last values
  double last_vertexRadius;
  VP1Interval last_cutAllowedR;
  VP1Interval last_cutAllowedZ;
  
  QPoint dragStartPosition;
  
  void initEditWindow();
};

//____________________________________________________________________
void MissingEtCollectionSettingsButton::Imp::initEditWindow()
{
  if (editwindow)
    return;
  theclass->messageVerbose("Initializing material editor dialog");
  editwindow = new QWidget(0,Qt::WindowStaysOnTopHint);
  editwindow_ui.setupUi(editwindow);

  // hide Eta cut widget
  editwindow_ui.etaphi_widget->showEtaCut(false);

  matButton = editwindow_ui.pushButton_matButton;  

}

//____________________________________________________________________
void MissingEtCollectionSettingsButton::setMaterialText(const QString& t)
{
  if (d->editwindow)
    d->editwindow->setWindowTitle(t);
  setToolTip(t);
}

//____________________________________________________________________
MissingEtCollectionSettingsButton::MissingEtCollectionSettingsButton(QWidget * parent,int _dim)
  : VP1CollectionSettingsButtonBase(parent,0), d(new Imp)
{
  d->dim = _dim;
  
  d->theclass = this;
  d->initEditWindow();
  
  //Draw Styles / Complexity:
  
  d->vertexDrawStyle = new SoDrawStyle;
  d->vertexDrawStyle->setName("VertexDrawStyle");
  d->vertexDrawStyle->pointSize=5.0;
  d->vertexDrawStyle->ref();
  updateVertexDrawStyle();

  // MET length and thickness
  connect(d->editwindow_ui.horizontalSlider_met_len,SIGNAL(valueChanged(int)),this,SIGNAL(metSizeChanged(int)));
  connect(d->editwindow_ui.horizontalSlider_met_thickness,SIGNAL(valueChanged(int)),this,SIGNAL(metSizeChanged(int)));
  
  // Light model
  d->vertexLightModel = new SoLightModel;
  d->vertexLightModel->setName("METLightModel");
  d->vertexLightModel->ref();

  updateVertexLightModel(false);
  connect(d->editwindow_ui.checkBox_verticesUseBaseLightModel,SIGNAL(toggled(bool)),this,SLOT(updateVertexLightModel(bool)));
  
  // Phi cut
  connect(d->editwindow_ui.etaphi_widget,SIGNAL(allowedPhiChanged(const QList<VP1Interval>&)),this,SIGNAL(cutAllowedPhiChanged(const QList<VP1Interval>&)));

//  // R
//  connect(d->editwindow_ui.checkBox_cut_r,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedR()));
//  connect(d->editwindow_ui.checkBox_cut_r_range_forcesymmetric,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedR()));
//  connect(d->editwindow_ui.checkBox_cut_r_excludeRange,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedR()));
//  connect(d->editwindow_ui.doubleSpinBox_cut_r_lower,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedR()));
//  connect(d->editwindow_ui.doubleSpinBox_cut_r_upper,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedR()));

//  // Z
//  connect(d->editwindow_ui.checkBox_cut_z,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedZ()));
//  connect(d->editwindow_ui.checkBox_cut_z_range_forcesymmetric,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedZ()));
//  connect(d->editwindow_ui.checkBox_cut_z_excludeRange,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedZ()));
//  connect(d->editwindow_ui.doubleSpinBox_cut_z_lower,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedZ()));
//  connect(d->editwindow_ui.doubleSpinBox_cut_z_upper,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedZ()));

  
  // Material
  connect(this,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
  connect(d->editwindow_ui.pushButton_close,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
  connect(d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(updateButton()));
  connect(d->matButton,SIGNAL(lastAppliedChanged()),this,SIGNAL(lastAppliedChanged()));
  setAcceptDrops(true);
  
  QTimer::singleShot(0, this, SLOT(updateButton()));
  
}

// QWidget& MissingEtCollectionSettingsButton::editWindow() {
//   if (!d->editwindow)
//     initEditWindow();
//   return *(d->editwindow);
// } 
MissingEtCollectionSettingsButton::~MissingEtCollectionSettingsButton()
{
  delete d->editwindow;
  d->vertexDrawStyle->unref();
  d->vertexLightModel->unref();
  delete d;
}

void MissingEtCollectionSettingsButton::updateButton()
{
  if (objectName().isEmpty())
    setObjectName("MissingEtCollectionSettingsButton");
  messageVerbose("setColButtonProperties: color=" + str(d->matButton->lastAppliedDiffuseColour()));
  VP1ColorSelectButton::setColButtonProperties(this,d->matButton->lastAppliedDiffuseColour(),d->dim);
}

void MissingEtCollectionSettingsButton::setDimension(int _dim)
{
  if (d->dim == _dim)
    return;
  d->dim = _dim;
  updateButton();
}

void MissingEtCollectionSettingsButton::showEditMaterialDialog()
{
  if (!d->editwindow)
    d->initEditWindow();

  if (d->editwindow->isHidden())
    d->editwindow->show();
  else
    d->editwindow->hide();
}

bool MissingEtCollectionSettingsButton::setMaterial(SoMaterial*mat)
{  
	// std::cout<<"MissingEtCollectionSettingsButton::setMaterial with mat="<<mat<<std::endl;
  if (!d->matButton) d->initEditWindow();
  d->matButton->setMaterial(mat);
  return true;
}

void MissingEtCollectionSettingsButton::copyValuesFromMaterial(SoMaterial*mat)
{
  if (!d->matButton) d->initEditWindow();
  d->matButton->setMaterial(mat);
}
double MissingEtCollectionSettingsButton::lastAppliedTransparency() const
{
  if (!d->matButton) d->initEditWindow();
  return d->matButton->lastAppliedTransparency();
}
double MissingEtCollectionSettingsButton::lastAppliedShininess() const
{
  if (!d->matButton) d->initEditWindow();
  return d->matButton->lastAppliedShininess();
}
double MissingEtCollectionSettingsButton::lastAppliedBrightness() const
{
  if (!d->matButton) d->initEditWindow();
  return d->matButton->lastAppliedBrightness();
}


void MissingEtCollectionSettingsButton::updateVertexDrawStyle()
{
	// TODO: Do I need this???

  // double val = VP1QtInventorUtils::getValueLineWidthSlider(d->editwindow_ui.horizontalSlider_vertexSize);
  // if (d->vertexDrawStyle->lineWidth.getValue()!=val)
  //   d->vertexDrawStyle->lineWidth = val;
}

void MissingEtCollectionSettingsButton::updateVertexLightModel(bool base)
{
  if (d->vertexLightModel->model.getValue()!=(base?SoLightModel::BASE_COLOR:SoLightModel::PHONG)) {
    messageVerbose("VertexLightModel changed (base = "+str(base));
    if (base)
      d->vertexLightModel->model.setValue(SoLightModel::BASE_COLOR);
    else
      d->vertexLightModel->model.setValue(SoLightModel::PHONG);
  }
}


//SoDrawStyle * MissingEtCollectionSettingsButton::vertexDrawStyle() const
//{
//  return d->vertexDrawStyle;
//}
//
//SoLightModel * MissingEtCollectionSettingsButton::vertexLightModel() const
//{
//  return d->vertexLightModel;
//}

float MissingEtCollectionSettingsButton::metLength() const
{
  return d->editwindow_ui.horizontalSlider_met_len->value();
}

float MissingEtCollectionSettingsButton::metThickness() const
{
  return d->editwindow_ui.horizontalSlider_met_thickness->value();
}


//____________________________________________________________________
void MissingEtCollectionSettingsButton::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    d->dragStartPosition = event->pos();
  QPushButton::mousePressEvent(event);
}

//____________________________________________________________________
void MissingEtCollectionSettingsButton::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->source()!=this && event->mimeData()->hasFormat("vp1/material"))
    event->acceptProposedAction();
}

//____________________________________________________________________
void MissingEtCollectionSettingsButton::mouseMoveEvent(QMouseEvent *event)
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
void MissingEtCollectionSettingsButton::dropEvent(QDropEvent *event)
{
  QByteArray data = event->mimeData()->data("vp1/material");
  event->acceptProposedAction();
  restoreFromState(data);  
}


QByteArray MissingEtCollectionSettingsButton::saveState() const{
  // messageVerbose("getState");
  // if (d->editwindow_ui.checkBox_tracksUseBaseLightModel->isChecked()) messageVerbose("checked!");
  VP1Serialise serialise(1/*version*/);
  
  serialise.save(d->matButton);  
  // serialise.disableUnsavedChecks();

  // MET length and thickness
  serialise.save(d->editwindow_ui.horizontalSlider_met_len);
  serialise.save(d->editwindow_ui.horizontalSlider_met_thickness);

  // Light model
  serialise.save(d->editwindow_ui.checkBox_verticesUseBaseLightModel);
  
  // ETA-PHI CUTS (from VP1Base/VP1EtaPhiCutWidget.cxx)
  serialise.save(d->editwindow_ui.etaphi_widget);


//  // R
//  serialise.save(d->editwindow_ui.checkBox_cut_r);
//  serialise.save(d->editwindow_ui.checkBox_cut_r_range_forcesymmetric);
//  serialise.save(d->editwindow_ui.checkBox_cut_r_excludeRange);
//  serialise.save(d->editwindow_ui.doubleSpinBox_cut_r_lower);
//  serialise.save(d->editwindow_ui.doubleSpinBox_cut_r_upper);
    
//// Z
//  serialise.save(d->editwindow_ui.checkBox_cut_z);
//  serialise.save(d->editwindow_ui.checkBox_cut_z_range_forcesymmetric);
//  serialise.save(d->editwindow_ui.checkBox_cut_z_excludeRange);
//  serialise.save(d->editwindow_ui.doubleSpinBox_cut_z_lower);
//  serialise.save(d->editwindow_ui.doubleSpinBox_cut_z_upper);
    
  serialise.widgetHandled(this);
  serialise.warnUnsaved(this);
  return serialise.result();
}

void MissingEtCollectionSettingsButton::restoreFromState( const QByteArray& ba){
   
  VP1Deserialise state(ba,systemBase());
  if (state.version()<0||state.version()>1)
    return;//Ignore silently

  // MATERIAL BUTTON (color,...)
  state.restore(d->matButton);

  // MET length and thickness
  state.restore(d->editwindow_ui.horizontalSlider_met_len);
  state.restore(d->editwindow_ui.horizontalSlider_met_thickness);

  // Light model
  state.restore(d->editwindow_ui.checkBox_verticesUseBaseLightModel);

  // ETA-PHI CUTS (from VP1Base/VP1EtaPhiCutWidget.cxx)
  state.restore(d->editwindow_ui.etaphi_widget);

  state.widgetHandled(this);
  state.warnUnrestored(this);

  updateVertexDrawStyle();
  updateVertexLightModel(d->editwindow_ui.checkBox_verticesUseBaseLightModel);

  updateButton();
  //FIXME - anything else need updating?
}


//____________________________________________________________________
QList<VP1Interval> MissingEtCollectionSettingsButton::cutAllowedPhi() const
{
	if (!d->editwindow)
		d->initEditWindow();
	return d->editwindow_ui.etaphi_widget->allowedPhi();
}




////____________________________________________________________________
//VP1Interval MissingEtCollectionSettingsButton::cutAllowedR() const
//{
//  if (!d->editwindow)
//    d->initEditWindow();
//  if (!d->editwindow_ui.checkBox_cut_r)
//    return VP1Interval();
//
//  const double minFromInterface=d->editwindow_ui.doubleSpinBox_cut_r_lower->value()*1000;
//  const double maxFromInterface=d->editwindow_ui.doubleSpinBox_cut_r_upper->value()*1000;
//
//  double min=0.0,max=0.0;
//
//  min = (d->editwindow_ui.checkBox_cut_r->isChecked() ? minFromInterface : -std::numeric_limits<double>::infinity());
//  max = (d->editwindow_ui.checkBox_cut_r->isChecked() ? maxFromInterface : std::numeric_limits<double>::infinity());
//
//  // FIXME - add symmetry logic
//
//  //message("cutAllowedPt: min,max="+QString::number(min)+","+QString::number(max));
//
//  if (max<min)
//    return VP1Interval();
//
//  return VP1Interval( min, max );//fixme: closed interval??
//}

////____________________________________________________________________
//VP1Interval MissingEtCollectionSettingsButton::cutAllowedZ() const
//{
//  if (!d->editwindow)
//    d->initEditWindow();
//  if (!d->editwindow_ui.checkBox_cut_z)
//    return VP1Interval();
//
//  const double minFromInterface=d->editwindow_ui.doubleSpinBox_cut_z_lower->value()*1000;
//  const double maxFromInterface=d->editwindow_ui.doubleSpinBox_cut_z_upper->value()*1000;
//
//  double min=0.0,max=0.0;
//
//  min = (d->editwindow_ui.checkBox_cut_z->isChecked() ? minFromInterface : -std::numeric_limits<double>::infinity());
//  max = (d->editwindow_ui.checkBox_cut_z->isChecked() ? maxFromInterface : std::numeric_limits<double>::infinity());
//
//  // FIXME - add symmetry logic
//
//  //message("cutAllowedPt: min,max="+QString::number(min)+","+QString::number(max));
//
//  if (max<min)
//    return VP1Interval();
//
//  return VP1Interval( min, max );//fixme: closed interval??
//}

//void MissingEtCollectionSettingsButton::possibleChange_cutAllowedR()
//{
//  messageVerbose("possibleChange_cutAllowedR() ");
//
//  if (d->last_cutAllowedR==cutAllowedR()) return;
//  messageVerbose("cutAllowedR() changed");
//  d->last_cutAllowedR= cutAllowedR();
//  emit cutAllowedRChanged(d->last_cutAllowedR);
//}

//void MissingEtCollectionSettingsButton::possibleChange_cutAllowedZ()
//{
//  messageVerbose("possibleChange_cutAllowedZ() ");
//
//  if (d->last_cutAllowedZ==cutAllowedZ()) return;
//  messageVerbose("cutAllowedZ() changed");
//  d->last_cutAllowedZ= cutAllowedZ();
//  emit cutAllowedZChanged(d->last_cutAllowedZ);
//}
