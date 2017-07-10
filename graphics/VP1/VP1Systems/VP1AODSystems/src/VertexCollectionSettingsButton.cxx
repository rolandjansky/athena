/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local
#include "VP1Base/VP1ColorSelectButton.h"
#include "VertexCollectionSettingsButton.h"
#include "ui_pervertexcollectionsettings_form.h"

//VP1
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
class VertexCollectionSettingsButton::Imp {
public:
  Imp():theclass(0),editwindow(0),editwindow_ui{},matButton(0), dim(0),vertexDrawStyle(0), vertexLightModel(0),
  last_vertexRadius(0.0),last_cutAllowedR{}, last_cutAllowedZ{},dragStartPosition{}{
    //nop
  }
  VertexCollectionSettingsButton * theclass;
  QWidget * editwindow;
  Ui::VertexCollectionSettingsForm editwindow_ui;
  
  VP1MaterialButton* matButton;// main collection colour
  
  int dim;
  
  SoDrawStyle * vertexDrawStyle;
  SoLightModel * vertexLightModel;
  
  //GUI - last values
  double last_vertexRadius;
  VP1Interval last_cutAllowedR;
  VP1Interval last_cutAllowedY;
  VP1Interval last_cutAllowedZ;
  
  QPoint dragStartPosition;
  
  void initEditWindow();
};

//____________________________________________________________________
void VertexCollectionSettingsButton::Imp::initEditWindow()
{
  if (editwindow)
    return;
  theclass->messageVerbose("Initialising material editor dialog");
  editwindow = new QWidget(0,Qt::WindowStaysOnTopHint);
  editwindow_ui.setupUi(editwindow);
  matButton = editwindow_ui.pushButton_matButton;  
}

//____________________________________________________________________
void VertexCollectionSettingsButton::setMaterialText(const QString& t)
{
  if (d->editwindow)
    d->editwindow->setWindowTitle(t);
  setToolTip(t);
}

//____________________________________________________________________
VertexCollectionSettingsButton::VertexCollectionSettingsButton(QWidget * parent,int _dim)
  : VP1MaterialButtonBase(parent,0,"VP1MaterialButton"), d(new Imp)
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
  connect(d->editwindow_ui.horizontalSlider_vertexSize,SIGNAL(valueChanged(int)),this,SIGNAL(vertexSizeChanged(int)));
  
  d->vertexLightModel = new SoLightModel;
  d->vertexLightModel->setName("VertexLightModel");
  d->vertexLightModel->ref();
  updateVertexLightModel(false);
  connect(d->editwindow_ui.checkBox_verticesUseBaseLightModel,SIGNAL(toggled(bool)),this,SLOT(updateVertexLightModel(bool)));
  
  // R 
  connect(d->editwindow_ui.checkBox_cut_r,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedR()));
  connect(d->editwindow_ui.checkBox_cut_r_range_forcesymmetric,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedR()));
  connect(d->editwindow_ui.checkBox_cut_r_excludeRange,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedR()));
  connect(d->editwindow_ui.doubleSpinBox_cut_r_lower,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedR()));
  connect(d->editwindow_ui.doubleSpinBox_cut_r_upper,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedR()));

  // Y
  connect(d->editwindow_ui.checkBox_cut_y,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedY()));
  connect(d->editwindow_ui.checkBox_cut_y_range_forcesymmetric,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedY()));
  connect(d->editwindow_ui.checkBox_cut_y_excludeRange,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedY()));
  connect(d->editwindow_ui.doubleSpinBox_cut_y_lower,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedY()));
  connect(d->editwindow_ui.doubleSpinBox_cut_y_upper,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedY()));
  // Z
  connect(d->editwindow_ui.checkBox_cut_z,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedZ()));
  connect(d->editwindow_ui.checkBox_cut_z_range_forcesymmetric,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedZ()));
  connect(d->editwindow_ui.checkBox_cut_z_excludeRange,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedZ()));
  connect(d->editwindow_ui.doubleSpinBox_cut_z_lower,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedZ()));
  connect(d->editwindow_ui.doubleSpinBox_cut_z_upper,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedZ()));


  
  connect(this,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
  connect(d->editwindow_ui.pushButton_close,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
  connect(d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(updateButton()));
  connect(d->matButton,SIGNAL(lastAppliedChanged()),this,SIGNAL(lastAppliedChanged()));
  setAcceptDrops(true);
  
  QTimer::singleShot(0, this, SLOT(updateButton()));
  
}

// QWidget& VertexCollectionSettingsButton::editWindow() {
//   if (!d->editwindow)
//     initEditWindow();
//   return *(d->editwindow);
// } 
VertexCollectionSettingsButton::~VertexCollectionSettingsButton()
{
  delete d->editwindow;
  d->vertexDrawStyle->unref();
  d->vertexLightModel->unref();
  delete d;
}

void VertexCollectionSettingsButton::updateButton()
{
  if (objectName().isEmpty())
    setObjectName("VertexCollectionSettingsButton");
  messageVerbose("setColButtonProperties: color=" + str(d->matButton->lastAppliedDiffuseColour()));
  VP1ColorSelectButton::setColButtonProperties(this,d->matButton->lastAppliedDiffuseColour(),d->dim);
}

void VertexCollectionSettingsButton::setDimension(int _dim)
{
  if (d->dim == _dim)
    return;
  d->dim = _dim;
  updateButton();
}

void VertexCollectionSettingsButton::showEditMaterialDialog()
{
  if (!d->editwindow)
    d->initEditWindow();

  if (d->editwindow->isHidden())
    d->editwindow->show();
  else
    d->editwindow->hide();
}

bool VertexCollectionSettingsButton::setMaterial(SoMaterial*mat)
{  
	// std::cout<<"VertexCollectionSettingsButton::setMaterial with mat="<<mat<<std::endl;
  if (!d->matButton) d->initEditWindow();
  d->matButton->setMaterial(mat);
  return true;
}

void VertexCollectionSettingsButton::copyValuesFromMaterial(SoMaterial*mat)
{
  if (!d->matButton) d->initEditWindow();
  d->matButton->setMaterial(mat);
}
double VertexCollectionSettingsButton::lastAppliedTransparency() const 
{
  if (!d->matButton) d->initEditWindow();
  return d->matButton->lastAppliedTransparency();
}
double VertexCollectionSettingsButton::lastAppliedShininess() const  
{
  if (!d->matButton) d->initEditWindow();
  return d->matButton->lastAppliedShininess();
}
double VertexCollectionSettingsButton::lastAppliedBrightness() const
{
  if (!d->matButton) d->initEditWindow();
  return d->matButton->lastAppliedBrightness();
}


void VertexCollectionSettingsButton::updateVertexDrawStyle()
{
  // double val = VP1QtInventorUtils::getValueLineWidthSlider(d->editwindow_ui.horizontalSlider_vertexSize);
  // if (d->vertexDrawStyle->lineWidth.getValue()!=val)
  //   d->vertexDrawStyle->lineWidth = val;
}

void VertexCollectionSettingsButton::updateVertexLightModel(bool base)
{
  if (d->vertexLightModel->model.getValue()!=(base?SoLightModel::BASE_COLOR:SoLightModel::PHONG)) {
    messageVerbose("VertexLightModel changed (base = "+str(base));
    if (base)
      d->vertexLightModel->model.setValue(SoLightModel::BASE_COLOR);
    else
      d->vertexLightModel->model.setValue(SoLightModel::PHONG);
  }
}


SoDrawStyle * VertexCollectionSettingsButton::vertexDrawStyle() const
{
  return d->vertexDrawStyle;
}

SoLightModel * VertexCollectionSettingsButton::vertexLightModel() const
{
  return d->vertexLightModel;
}

int VertexCollectionSettingsButton::vertexSize() const
{
  return d->editwindow_ui.horizontalSlider_vertexSize->value();
}

//____________________________________________________________________
void VertexCollectionSettingsButton::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    d->dragStartPosition = event->pos();
  QPushButton::mousePressEvent(event);
}

//____________________________________________________________________
void VertexCollectionSettingsButton::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->source()!=this && event->mimeData()->hasFormat("vp1/material"))
    event->acceptProposedAction();
}

//____________________________________________________________________
void VertexCollectionSettingsButton::mouseMoveEvent(QMouseEvent *event)
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
void VertexCollectionSettingsButton::dropEvent(QDropEvent *event)
{
  QByteArray data = event->mimeData()->data("vp1/material");
  event->acceptProposedAction();
  restoreFromState(data);  
}


QByteArray VertexCollectionSettingsButton::saveState() const{
  // messageVerbose("getState");
  // if (d->editwindow_ui.checkBox_tracksUseBaseLightModel->isChecked()) messageVerbose("checked!");
  VP1Serialise serialise(1/*version*/);
  
  // MATERIAL BUTTON (color,...)
  serialise.save(d->matButton);

  // serialise.disableUnsavedChecks();

  // VERTEX SETTINGS
  serialise.save(d->editwindow_ui.horizontalSlider_vertexSize);
  serialise.save(d->editwindow_ui.checkBox_verticesUseBaseLightModel);
  
  // R
  serialise.save(d->editwindow_ui.checkBox_cut_r);  
  serialise.save(d->editwindow_ui.checkBox_cut_r_range_forcesymmetric);  
  serialise.save(d->editwindow_ui.checkBox_cut_r_excludeRange);  
  serialise.save(d->editwindow_ui.doubleSpinBox_cut_r_lower);    
  serialise.save(d->editwindow_ui.doubleSpinBox_cut_r_upper);    
    
  // Y
  serialise.save(d->editwindow_ui.checkBox_cut_y);  
  serialise.save(d->editwindow_ui.checkBox_cut_y_range_forcesymmetric);  
  serialise.save(d->editwindow_ui.checkBox_cut_y_excludeRange);  
  serialise.save(d->editwindow_ui.doubleSpinBox_cut_y_lower);    
  serialise.save(d->editwindow_ui.doubleSpinBox_cut_y_upper);
  // Z
  serialise.save(d->editwindow_ui.checkBox_cut_z);  
  serialise.save(d->editwindow_ui.checkBox_cut_z_range_forcesymmetric);  
  serialise.save(d->editwindow_ui.checkBox_cut_z_excludeRange);  
  serialise.save(d->editwindow_ui.doubleSpinBox_cut_z_lower);    
  serialise.save(d->editwindow_ui.doubleSpinBox_cut_z_upper); 
    
  serialise.widgetHandled(this);
  serialise.warnUnsaved(this);
  return serialise.result();
}

void VertexCollectionSettingsButton::restoreFromState( const QByteArray& ba){
   
  VP1Deserialise state(ba,systemBase());
  if (state.version()<0||state.version()>1)
    return;//Ignore silently

  // MATERIAL BUTTON (color,...)
  state.restore(d->matButton);

  // VERTEX SETTINGS
  state.restore(d->editwindow_ui.horizontalSlider_vertexSize);
  state.restore(d->editwindow_ui.checkBox_verticesUseBaseLightModel);

  // R
  state.restore(d->editwindow_ui.checkBox_cut_r);  
  state.restore(d->editwindow_ui.checkBox_cut_r_range_forcesymmetric);  
  state.restore(d->editwindow_ui.checkBox_cut_r_excludeRange);  
  state.restore(d->editwindow_ui.doubleSpinBox_cut_r_lower);    
  state.restore(d->editwindow_ui.doubleSpinBox_cut_r_upper);    

  // Y
  state.restore(d->editwindow_ui.checkBox_cut_y);  
  state.restore(d->editwindow_ui.checkBox_cut_y_range_forcesymmetric);  
  state.restore(d->editwindow_ui.checkBox_cut_y_excludeRange);  
  state.restore(d->editwindow_ui.doubleSpinBox_cut_y_lower);    
  state.restore(d->editwindow_ui.doubleSpinBox_cut_y_upper);
  // Z
  state.restore(d->editwindow_ui.checkBox_cut_z);  
  state.restore(d->editwindow_ui.checkBox_cut_z_range_forcesymmetric);  
  state.restore(d->editwindow_ui.checkBox_cut_z_excludeRange);  
  state.restore(d->editwindow_ui.doubleSpinBox_cut_z_lower);    
  state.restore(d->editwindow_ui.doubleSpinBox_cut_z_upper);   

  state.widgetHandled(this);
  state.warnUnrestored(this);

  updateVertexDrawStyle();
  updateVertexLightModel(d->editwindow_ui.checkBox_verticesUseBaseLightModel);
  updateButton();
  //FIXME - anything else need updating?
}

//____________________________________________________________________
VP1Interval VertexCollectionSettingsButton::cutAllowedR() const
{
  if (!d->editwindow)
    d->initEditWindow();
  if (!d->editwindow_ui.checkBox_cut_r)
    return VP1Interval();
  
  const double minFromInterface=d->editwindow_ui.doubleSpinBox_cut_r_lower->value()*1000;
  const double maxFromInterface=d->editwindow_ui.doubleSpinBox_cut_r_upper->value()*1000;
  
  double min=0.0,max=0.0;

  min = (d->editwindow_ui.checkBox_cut_r->isChecked() ? minFromInterface : -std::numeric_limits<double>::infinity());
  max = (d->editwindow_ui.checkBox_cut_r->isChecked() ? maxFromInterface : std::numeric_limits<double>::infinity());
  
  // FIXME - add symmetry logic
  
  //message("cutAllowedPt: min,max="+QString::number(min)+","+QString::number(max));
  
  if (max<min)
    return VP1Interval();
    
  return VP1Interval( min, max );//fixme: closed interval??
}

//____________________________________________________________________
VP1Interval VertexCollectionSettingsButton::cutAllowedY() const
{
  if (!d->editwindow)
    d->initEditWindow();
  if (!d->editwindow_ui.checkBox_cut_y)
    return VP1Interval();
  
  const double minFromInterface = d->editwindow_ui.doubleSpinBox_cut_y_lower->value();//*1000;
  const double maxFromInterface = d->editwindow_ui.doubleSpinBox_cut_y_upper->value();//*1000;
  
  double min=0.0,max=0.0;

  min = (d->editwindow_ui.checkBox_cut_y->isChecked() ? minFromInterface : -std::numeric_limits<double>::infinity());
  max = (d->editwindow_ui.checkBox_cut_y->isChecked() ? maxFromInterface : std::numeric_limits<double>::infinity());
  
  // FIXME - add symmetry logic
  
  //message("cutAllowedPt: min,max="+QString::number(min)+","+QString::number(max));
  
  if (max < min)
    return VP1Interval();
    
  return VP1Interval( min, max );//fixme: closed interval??
}
//____________________________________________________________________
VP1Interval VertexCollectionSettingsButton::cutAllowedZ() const
{
  if (!d->editwindow)
    d->initEditWindow();
  if (!d->editwindow_ui.checkBox_cut_z)
    return VP1Interval();
  
  const double minFromInterface=d->editwindow_ui.doubleSpinBox_cut_z_lower->value();//*1000;
  const double maxFromInterface=d->editwindow_ui.doubleSpinBox_cut_z_upper->value();//*1000;
  
  double min=0.0,max=0.0;

  min = (d->editwindow_ui.checkBox_cut_z->isChecked() ? minFromInterface : -std::numeric_limits<double>::infinity());
  max = (d->editwindow_ui.checkBox_cut_z->isChecked() ? maxFromInterface : std::numeric_limits<double>::infinity());
  
  // FIXME - add symmetry logic
  
  //message("cutAllowedPt: min,max="+QString::number(min)+","+QString::number(max));
  
  if (max<min)
    return VP1Interval();
    
  return VP1Interval( min, max );//fixme: closed interval??
}
//____________________________________________________________________
void VertexCollectionSettingsButton::possibleChange_cutAllowedR()
{
  messageVerbose("possibleChange_cutAllowedR() ");
  
  if (d->last_cutAllowedR==cutAllowedR()) return;
  messageVerbose("cutAllowedR() changed");
  d->last_cutAllowedR= cutAllowedR();
  emit cutAllowedRChanged(d->last_cutAllowedR);
}
//____________________________________________________________________
void VertexCollectionSettingsButton::possibleChange_cutAllowedY()
{
  messageVerbose("possibleChange_cutAllowedY() ");
  
  if (d->last_cutAllowedY == cutAllowedY()) return;
  messageVerbose("cutAllowedY() changed");
  d->last_cutAllowedY = cutAllowedY();
  emit cutAllowedYChanged(d->last_cutAllowedY);
}
//____________________________________________________________________
void VertexCollectionSettingsButton::possibleChange_cutAllowedZ()
{
  messageVerbose("possibleChange_cutAllowedZ() ");
  
  if (d->last_cutAllowedZ==cutAllowedZ()) return;
  messageVerbose("cutAllowedZ() changed");
  d->last_cutAllowedZ= cutAllowedZ();
  emit cutAllowedZChanged(d->last_cutAllowedZ);
}
