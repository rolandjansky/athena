/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1MaterialButton              //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: September 2007                        //
//                                                         //
/////////////////////////////////////////////////////////////

//Todo:
//      ~32 colours directly in simple gui + more button launching colour browser.
//      mode switching closes all open colour dialogs!

#include "VP1Base/VP1MaterialButton.h"
#include "VP1Base/VP1ExaminerViewer.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1Base/VP1ColorSelectButton.h"
#include "ui_vp1materialbuttonform.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"

#include "Inventor/nodes/SoSwitch.h"
#include "Inventor/nodes/SoCube.h"
#include "Inventor/nodes/SoCone.h"
#include "Inventor/nodes/SoComplexity.h"
#include "Inventor/nodes/SoDrawStyle.h"
#include "Inventor/nodes/SoMaterial.h"
#include <Inventor/nodes/SoSeparator.h>

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
class VP1MaterialButton::Imp {
public:
  VP1MaterialButton * theclass;

  QPoint dragStartPosition;

  bool simplemode;

  int dim;

  QString materialtext;
  QWidget * editwindow;
  SoMaterial * preview_material;
  Ui::VP1MaterialButtonForm editwindow_ui;
  void initEditWindow();

  SoQtRenderArea * renderarea;
  SoSwitch * previewswitch;

  void applyValuesToMaterial(SoMaterial*, bool preview = false);
  QList<SoMaterial*> handledmaterials;

  static SbColor qcol2sbcol(const QColor&);
  static QColor sbcol2qcol(const SbColor&);
  static double distSq(const QColor& a, const QColor& b);
  static bool equal(const QColor& a, const QColor& b, const double& tol = 1.0e-6);

  //Settings returned from the gui, with correct dependency on simplemode:
  QColor guiAmbientColour() const;
  QColor guiDiffuseColour() const;
  QColor guiSpecularColour() const;
  QColor guiEmissiveColour() const;
  int guiTransparency() const;
  int guiShininess() const;

  //Last applied settings:
  QColor lastapplied_ambient;
  QColor lastapplied_diffuse;
  QColor lastapplied_specular;
  QColor lastapplied_emissive;
  int lastapplied_shininess;
  int lastapplied_transparency;
  int lastapplied_brightness;//redundant but we use it to give exact
			     //reset functionality to simple mode.

  QString printFloat(const double& d) {
    if (d==0)
      return "0";
    QString s = QString::number(d,'f',5);
    while (s.right(1)=="0"&&s.contains("."))
      s = s.left(s.count()-1);
    if (s=="0.") return "0";
    if (s=="1.") return "1";
    return s;
  }
  QString toSbColTxt(const QColor& c) { return "SbColor("+printFloat(c.redF())+","
					  +printFloat(c.greenF())+","+printFloat(c.blueF())+")";}
  void setLastAppliedFromCurrent();
  bool previewEqualToLastApplied() const;
  void updateApplyResetButtons();
  void adaptGuiForMode();
  void switchModeBasedOnLastApplied();
  void adaptGuiAndMaterialsToLastApplied();

  QColor emissiveColourFromSimpleParameters(const QColor& simpleColour, const double& brightness) const;
  double brightnessEstimateFromDetailedParameters( const QColor& diffuseColour, const QColor& emissiveColour ) const;

  int blockcount;
  void blockGuiSignals(bool);

  static QColor simpleAmbient() { return QColor::fromRgbF(0.2,0.2,0.2); }
  static QColor simpleSpecular() { return Qt::black; }
  static int simpleShininess() { return 0; }
};

//____________________________________________________________________
VP1MaterialButton::VP1MaterialButton(QWidget * parent,int dim)
  : VP1MaterialButtonBase(parent,0,"VP1MaterialButton"), m_d(new Imp)
{
  m_d->simplemode = false;
  m_d->dim = dim;
  m_d->theclass = this;
  m_d->editwindow = 0;
  m_d->preview_material = 0;
  m_d->renderarea = 0;
  m_d->previewswitch = 0;
  m_d->blockcount = 0;
  connect(this,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));

  m_d->lastapplied_ambient = QColor::fromRgbF(0.2,0.2,0.2);
  m_d->lastapplied_diffuse = QColor::fromRgbF(0.8,0.8,0.8);
  m_d->lastapplied_specular = Qt::black;
  m_d->lastapplied_emissive = Qt::black;
  m_d->lastapplied_shininess = 20;
  m_d->lastapplied_transparency = 0;
  m_d->lastapplied_brightness = 0;
  setMaterialText("");
  setAcceptDrops(true);
  QTimer::singleShot(0, this, SLOT(updateButton()));
}

//____________________________________________________________________
VP1MaterialButton::~VP1MaterialButton()
{
  clearHandledMaterials();
  setUpdatesEnabled(false);
  if (m_d->renderarea) {
    m_d->renderarea->setAutoRedraw(false);
    SoNode * root = m_d->renderarea->getSceneGraph();
    root->ref();
    m_d->renderarea->setSceneGraph(0);
    delete m_d->renderarea;
    root->unref();
  }
  delete m_d->editwindow;
  delete m_d;
}

//____________________________________________________________________
QColor VP1MaterialButton::lastAppliedDiffuseColour() const { return m_d->lastapplied_diffuse; }
QColor VP1MaterialButton::lastAppliedAmbientColour() const { return m_d->lastapplied_ambient; }
QColor VP1MaterialButton::lastAppliedSpecularColour() const { return m_d->lastapplied_specular; }
QColor VP1MaterialButton::lastAppliedEmissiveColour() const { return m_d->lastapplied_emissive; }
double VP1MaterialButton::lastAppliedTransparency() const { return std::max<double>(0.0,std::min<double>(1.0,m_d->lastapplied_transparency/100.0)); }
double VP1MaterialButton::lastAppliedShininess() const { return std::max<double>(0.0,std::min<double>(1.0,m_d->lastapplied_shininess/100.0)); }
double VP1MaterialButton::lastAppliedBrightness() const { return std::max<double>(0.0,std::min<double>(1.0,m_d->lastapplied_brightness/100.0)); }

QWidget& VP1MaterialButton::editWindow() {
  if (!m_d->editwindow)
    m_d->initEditWindow();
  return *(m_d->editwindow);
}

//____________________________________________________________________
void VP1MaterialButton::showEditMaterialDialog()
{
  if (editWindow().isHidden())
    editWindow().show();
  else
    editWindow().hide();
}

//____________________________________________________________________
void VP1MaterialButton::setMaterialText(const QString& t)
{
  m_d->materialtext = ( t.isEmpty() ? "Edit Material" : "Edit Material "+t );
  if (m_d->editwindow)
    m_d->editwindow->setWindowTitle(m_d->materialtext);
  setToolTip(m_d->materialtext);
}

//____________________________________________________________________
void VP1MaterialButton::Imp::adaptGuiAndMaterialsToLastApplied()
{
  switchModeBasedOnLastApplied();

  if (theclass->verbose()) {
    theclass->messageVerbose("Copied values from material: "
			     "diffuse="+str(lastapplied_diffuse)
			     +", ambient="+str(lastapplied_ambient)
			     +", specular="+str(lastapplied_specular)
			     +", emissive="+str(lastapplied_emissive)
			     +", shininess="+str(lastapplied_shininess)
			     +", transparency="+str(lastapplied_transparency)
			     );
  }

  if (editwindow) {
    if (theclass->verbose())
      theclass->messageVerbose(" => and updating preview.");
    blockGuiSignals(true);
    editwindow_ui.colbutton_ambient->setColor(lastapplied_ambient);
    editwindow_ui.colbutton_diffuse->setColor(lastapplied_diffuse);
    editwindow_ui.colbutton_specular->setColor(lastapplied_specular);
    editwindow_ui.colbutton_emissive->setColor(lastapplied_emissive);
    editwindow_ui.slider_shininess->setValue(lastapplied_shininess);
    editwindow_ui.label_num_shininess->setNum(lastapplied_shininess);
    editwindow_ui.colbutton_simple_colour->setColor(lastapplied_diffuse);
    editwindow_ui.slider_simple_transparency->setValue(lastapplied_transparency);
    if (lastapplied_brightness!=-1) {
      editwindow_ui.slider_simple_brightness->setValue(lastapplied_brightness);
      editwindow_ui.label_num_simple_brightness->setNum(lastapplied_brightness);
    }
    editwindow_ui.slider_transparency->setValue(lastapplied_transparency);
    editwindow_ui.label_num_transparency->setNum(lastapplied_transparency);
    editwindow_ui.label_num_simple_transparency->setNum(lastapplied_transparency);
    blockGuiSignals(false);
    theclass->updatePreview();
  }
  theclass->apply();
  updateApplyResetButtons();
}

//____________________________________________________________________
void VP1MaterialButton::copyValuesFromMaterial(SoMaterial*m)
{
  if (!m) {
    message("VP1MaterialButton::copyValuesFromMaterial Error: Passed material is null!!");
    return;
  }
  m->ref();
  if (m->ambientColor.getNum()!=1
      ||m->diffuseColor.getNum()!=1
      ||m->specularColor.getNum()!=1
      ||m->emissiveColor.getNum()!=1
      ||m->transparency.getNum()!=1
      ||m->shininess.getNum()!=1) {
    message("VP1MaterialButton::copyValuesFromMaterial Error: Passed material must have exactly one value in each of the 6 fields!!");
    m->unrefNoDelete();
    return;
  }

  int new_shininess = std::min(std::max(0,static_cast<int>(m->shininess[0]*100.0f+0.5f)),100);
  int new_transparency = std::min(std::max(0,static_cast<int>(m->transparency[0]*100.0f+0.5f)),100);

  m_d->lastapplied_ambient = m_d->sbcol2qcol(m->ambientColor[0]);
  m_d->lastapplied_diffuse = m_d->sbcol2qcol(m->diffuseColor[0]);
  m_d->lastapplied_specular = m_d->sbcol2qcol(m->specularColor[0]);
  m_d->lastapplied_emissive = m_d->sbcol2qcol(m->emissiveColor[0]);
  m_d->lastapplied_shininess = new_shininess;
  m_d->lastapplied_transparency = new_transparency;
  m_d->lastapplied_brightness = std::max<int>(0,std::min<int>(100,static_cast<int>(100*m_d->brightnessEstimateFromDetailedParameters(m_d->lastapplied_diffuse,m_d->lastapplied_emissive)+0.5)));

  m->unrefNoDelete();

  m_d->adaptGuiAndMaterialsToLastApplied();

  emit lastAppliedChanged();
}

//____________________________________________________________________
void VP1MaterialButton::Imp::blockGuiSignals(bool b)
{
  if (b)
    ++blockcount;
  else
    --blockcount;
  if ( (b&&blockcount==1) || (!b&&blockcount==0) ) {
    editwindow_ui.colbutton_ambient->blockSignals(b);
    editwindow_ui.colbutton_diffuse->blockSignals(b);
    editwindow_ui.colbutton_specular->blockSignals(b);
    editwindow_ui.colbutton_emissive->blockSignals(b);
    editwindow_ui.slider_shininess->blockSignals(b);
    editwindow_ui.colbutton_simple_colour->blockSignals(b);
    editwindow_ui.slider_simple_brightness->blockSignals(b);
    editwindow_ui.slider_simple_transparency->blockSignals(b);
    editwindow_ui.slider_transparency->blockSignals(b);
    editwindow_ui.radioButton_cone->blockSignals(b);
    editwindow_ui.radioButton_lines->blockSignals(b);
    editwindow_ui.radioButton_box->blockSignals(b);
    editwindow_ui.pushButton_apply->blockSignals(b);
    editwindow_ui.pushButton_reset->blockSignals(b);
    editwindow_ui.pushButton_switch_mode->blockSignals(b);
  }
}


//____________________________________________________________________
void VP1MaterialButton::Imp::initEditWindow()
{
  if (editwindow)
    return;
  theclass->messageVerbose("Initialising material editor dialog");
  editwindow = new QWidget(0,Qt::WindowStaysOnTopHint);
  editwindow_ui.setupUi(editwindow);
  editwindow_ui.colbutton_ambient->setColor(lastapplied_ambient);
  editwindow_ui.colbutton_diffuse->setColor(lastapplied_diffuse);
  editwindow_ui.colbutton_specular->setColor(lastapplied_specular);
  editwindow_ui.colbutton_emissive->setColor(lastapplied_emissive);
  editwindow_ui.slider_shininess->setValue(lastapplied_shininess);
  editwindow_ui.label_num_shininess->setNum(lastapplied_shininess);
  editwindow_ui.slider_transparency->setValue(lastapplied_transparency);
  editwindow_ui.slider_simple_transparency->setValue(lastapplied_transparency);
  editwindow_ui.label_num_transparency->setNum(lastapplied_transparency);
  editwindow_ui.label_num_simple_transparency->setNum(lastapplied_transparency);
  editwindow_ui.slider_simple_brightness->setValue(lastapplied_brightness);
  editwindow_ui.label_num_simple_brightness->setNum(lastapplied_brightness);
  editwindow_ui.colbutton_simple_colour->setColor(lastapplied_diffuse);

  if (!VP1QtUtils::environmentVariableIsOn("VP1_DISALLOW_MULTIPLE_CHANNELS")) {
    SoSeparator * userroot = new SoSeparator;
    SoComplexity * complexity = new SoComplexity;
    complexity->value = 1.0;
    userroot->addChild(complexity);
    preview_material = new SoMaterial;
    userroot->addChild(preview_material);
    previewswitch = new SoSwitch;
    previewswitch->addChild(new SoCube);
    previewswitch->addChild(new SoCone);
    SoSeparator * linesep = new SoSeparator;
    SoDrawStyle * linedrawstyle = new SoDrawStyle;
    linedrawstyle->style = SoDrawStyleElement::LINES;
    linedrawstyle->lineWidth = 2;
    linesep->addChild(linedrawstyle);
    linesep->addChild(new SoCube);
    previewswitch->addChild(linesep);
    previewswitch->whichChild = 0;
    userroot->addChild(previewswitch);
    VP1ExaminerViewer * viewer = new VP1ExaminerViewer(editwindow_ui.widget_3dpreview,false);
    viewer->setDecoration(false);
    viewer->removeDecorationMenuOption();
    viewer->setTransparencyType( SoGLRenderAction::DELAYED_BLEND );
    viewer->viewAll();
    renderarea=viewer;
    renderarea->setAntialiasing(true,4);
    renderarea->setSceneGraph(userroot);
    renderarea->setBackgroundColor(SbColor(0.0f, 0.0f, 0.0f));
  } else {
    //Put some sort of warning in that 'preview disabled by VP1_DISALLOW_MULTIPLE_CHANNELS env. var.'
    //int editwindow_ui.widget_3dpreview. Fixme.
  }

  connect(editwindow_ui.colbutton_ambient,SIGNAL(colorChanged(const QColor&)),theclass,SLOT(updatePreview()));
  connect(editwindow_ui.colbutton_diffuse,SIGNAL(colorChanged(const QColor&)),theclass,SLOT(updatePreview()));
  connect(editwindow_ui.colbutton_specular,SIGNAL(colorChanged(const QColor&)),theclass,SLOT(updatePreview()));
  connect(editwindow_ui.colbutton_emissive,SIGNAL(colorChanged(const QColor&)),theclass,SLOT(updatePreview()));
  connect(editwindow_ui.slider_shininess,SIGNAL(valueChanged(int)),theclass,SLOT(updatePreview()));
  connect(editwindow_ui.colbutton_simple_colour,SIGNAL(colorChanged(const QColor&)),theclass,SLOT(updatePreview()));
  connect(editwindow_ui.slider_simple_brightness,SIGNAL(valueChanged(int)),theclass,SLOT(updatePreview()));
  //Transparency sliders needs to be coordinated:
  connect(editwindow_ui.slider_simple_transparency,SIGNAL(valueChanged(int)),theclass,SLOT(transparencyChanged()));
  connect(editwindow_ui.slider_transparency,SIGNAL(valueChanged(int)),theclass,SLOT(transparencyChanged()));

  editwindow_ui.radioButton_cone->setChecked(true);

  switchModeBasedOnLastApplied();

  theclass->updatePreviewSceneAndBgd();

  connect(editwindow_ui.radioButton_cone,SIGNAL(toggled(bool)),theclass,SLOT(updatePreviewSceneAndBgd()));
  connect(editwindow_ui.radioButton_box,SIGNAL(toggled(bool)),theclass,SLOT(updatePreviewSceneAndBgd()));

  theclass->updatePreview();
  setLastAppliedFromCurrent();

  connect(editwindow_ui.pushButton_apply,SIGNAL(clicked()),theclass,SLOT(apply()));
  connect(editwindow_ui.pushButton_reset,SIGNAL(clicked()),theclass,SLOT(reset()));
  connect(editwindow_ui.pushButton_switch_mode,SIGNAL(clicked()),theclass,SLOT(switchMode()));

  adaptGuiForMode();

  connect(new QShortcut(QKeySequence(Qt::Key_Escape), editwindow),
	  SIGNAL(activated()),editwindow,SLOT(hide()));

}


//____________________________________________________________________
void VP1MaterialButton::Imp::applyValuesToMaterial(SoMaterial* m, bool preview)
{
  if (!m)
    return;
  bool save = m->enableNotify(false);
  if (preview) {
    //Take value directly from gui:
    if (!editwindow)
      initEditWindow();
    m->ambientColor.setValue(qcol2sbcol(guiAmbientColour()));
    m->diffuseColor.setValue(qcol2sbcol(guiDiffuseColour()));
    m->specularColor.setValue(qcol2sbcol(guiSpecularColour()));
    m->emissiveColor.setValue(qcol2sbcol(guiEmissiveColour()));
    m->shininess.setValue(std::max(0.0f,std::min(1.0f,guiShininess()/100.0f)));
    m->transparency.setValue(std::max(0.0f,std::min(1.0f,guiTransparency()/100.0f)));
  } else {
    //Take values from stored "last" settings:
    m->ambientColor.setValue(qcol2sbcol(lastapplied_ambient));
    m->diffuseColor.setValue(qcol2sbcol(lastapplied_diffuse));
    m->specularColor.setValue(qcol2sbcol(lastapplied_specular));
    m->emissiveColor.setValue(qcol2sbcol(lastapplied_emissive));
    m->shininess.setValue(std::max(0.0f,std::min(1.0f,lastapplied_shininess/100.0f)));
    m->transparency.setValue(std::max(0.0f,std::min(1.0f,lastapplied_transparency/100.0f)));
  }
  if (save) {
    m->enableNotify(true);
    m->touch();
  }
}

//____________________________________________________________________
bool VP1MaterialButton::handleMaterial( SoMaterial * m )
{
  if (!m||m_d->handledmaterials.contains(m))
    return false;
  m->ref();
  m_d->handledmaterials << m;
  m_d->applyValuesToMaterial(m);
  return true;
}

//____________________________________________________________________
bool VP1MaterialButton::setMaterial(SoMaterial*m)
{
  if (!m)
    return false;
  m->ref();
  copyValuesFromMaterial(m);
  bool ok = handleMaterial(m);
  m->unrefNoDelete();
  return ok;
}

//____________________________________________________________________
bool VP1MaterialButton::stopHandlingMaterial( SoMaterial * m )
{
  if (!m||!m_d->handledmaterials.contains(m))
    return false;
  m_d->handledmaterials.removeAll(m);
  m->unref();
  return true;
}

//____________________________________________________________________
void VP1MaterialButton::clearHandledMaterials()
{
  foreach (SoMaterial * m,m_d->handledmaterials)
    m->unref();
  m_d->handledmaterials.clear();
}

//____________________________________________________________________
void VP1MaterialButton::updatePreview()
{
  if (!m_d->editwindow)
    return;
  if (m_d->preview_material)
    m_d->applyValuesToMaterial(m_d->preview_material,true);
  m_d->updateApplyResetButtons();
}

//____________________________________________________________________
void VP1MaterialButton::updatePreviewSceneAndBgd()
{
  if (!m_d->editwindow||!m_d->renderarea||!m_d->previewswitch)
    return;

  if (m_d->editwindow_ui.radioButton_box->isChecked())
    m_d->previewswitch->whichChild = 0;
  else
    m_d->previewswitch->whichChild = (m_d->editwindow_ui.radioButton_cone->isChecked() ? 1 : 2);
}

//____________________________________________________________________
SbColor VP1MaterialButton::Imp::qcol2sbcol(const QColor& col)
{
  return SbColor( col.red()/255.0, col.green()/255.0, col.blue()/255.0 );
}

//____________________________________________________________________
QColor VP1MaterialButton::Imp::sbcol2qcol(const SbColor& col)
{
  float r,g,b;
  col.getValue(r,g,b);
  return QColor::fromRgbF( r,g,b );
}

//____________________________________________________________________
double VP1MaterialButton::Imp::distSq(const QColor& a, const QColor& b)
{
  const double dRed(a.redF()-b.redF());
  const double dGreen(a.greenF()-b.greenF());
  const double dBlue(a.blueF()-b.blueF());
  return dRed*dRed+dGreen*dGreen+dBlue*dBlue;
}

//____________________________________________________________________
bool VP1MaterialButton::Imp::equal(const QColor& a, const QColor& b, const double& tol)
{
  return a == b || distSq(a,b) < tol;
}

//____________________________________________________________________
QColor VP1MaterialButton::Imp::emissiveColourFromSimpleParameters(const QColor& simpleColour, const double& brightness) const
{
  return QColor::fromRgbF(std::min(1.0,std::max(0.0,brightness*simpleColour.redF())),
			  std::min(1.0,std::max(0.0,brightness*simpleColour.greenF())),
			  std::min(1.0,std::max(0.0,brightness*simpleColour.blueF())));
}

//____________________________________________________________________
double VP1MaterialButton::Imp::brightnessEstimateFromDetailedParameters( const QColor& diffuseColour, const QColor& emissiveColour ) const
{
  double df_r(diffuseColour.redF()),df_g(diffuseColour.greenF()),df_b(diffuseColour.blueF());
  double em_r(emissiveColour.redF()),em_g(emissiveColour.greenF()),em_b(emissiveColour.blueF());
  const double dotp = em_r*df_r + em_g*df_g + em_b*df_b;
  const double df_mag2 = df_r*df_r + df_g*df_g + df_b*df_b;
  return std::max<double>(0.0,std::min<double>(100.0,dotp/df_mag2));
}

//____________________________________________________________________
QColor VP1MaterialButton::Imp::guiAmbientColour() const
{
  return simplemode ? simpleAmbient() : editwindow_ui.colbutton_ambient->color();
}

//____________________________________________________________________
QColor VP1MaterialButton::Imp::guiDiffuseColour() const
{
  return simplemode ? editwindow_ui.colbutton_simple_colour->color() : editwindow_ui.colbutton_diffuse->color();
}

//____________________________________________________________________
QColor VP1MaterialButton::Imp::guiSpecularColour() const
{
  return simplemode ? simpleSpecular() : editwindow_ui.colbutton_specular->color();
}

//____________________________________________________________________
QColor VP1MaterialButton::Imp::guiEmissiveColour() const
{
  if (simplemode)
    return emissiveColourFromSimpleParameters(editwindow_ui.colbutton_simple_colour->color(),editwindow_ui.slider_simple_brightness->value()/100.0);
  else
    return editwindow_ui.colbutton_emissive->color();
}

//____________________________________________________________________
int VP1MaterialButton::Imp::guiTransparency() const
{
  return simplemode ? editwindow_ui.slider_transparency->value() : editwindow_ui.slider_simple_transparency->value();
}


//____________________________________________________________________
int VP1MaterialButton::Imp::guiShininess() const
{
  return simplemode ? simpleShininess() : editwindow_ui.slider_shininess->value();
}

//____________________________________________________________________
void VP1MaterialButton::Imp::setLastAppliedFromCurrent()
{
  if (!editwindow)
    return;
  lastapplied_ambient = guiAmbientColour();
  lastapplied_diffuse = guiDiffuseColour();
  lastapplied_specular = guiSpecularColour();
  lastapplied_emissive = guiEmissiveColour();
  lastapplied_shininess = guiShininess();
  lastapplied_transparency = guiTransparency();
  lastapplied_brightness = std::max<int>(0,std::min<int>(100,static_cast<int>(100*brightnessEstimateFromDetailedParameters(lastapplied_diffuse,lastapplied_emissive)+0.5)));
  updateApplyResetButtons();

  emit theclass->lastAppliedChanged();

}

//____________________________________________________________________
bool VP1MaterialButton::Imp::previewEqualToLastApplied() const
{
  if (!editwindow)
    return false;
  return (equal(lastapplied_ambient,guiAmbientColour())
	  &&equal(lastapplied_diffuse,guiDiffuseColour())
	  &&equal(lastapplied_specular,guiSpecularColour())
	  &&equal(lastapplied_emissive,guiEmissiveColour())
	  &&lastapplied_shininess==guiShininess()
	  &&lastapplied_transparency==guiTransparency());
}

//____________________________________________________________________
void VP1MaterialButton::Imp::updateApplyResetButtons()
{
  if (!editwindow)
    return;
  if (previewEqualToLastApplied()) {
    editwindow_ui.pushButton_apply->setEnabled(false);
    editwindow_ui.pushButton_reset->setEnabled(false);
  } else {
    editwindow_ui.pushButton_apply->setEnabled(true);
    editwindow_ui.pushButton_reset->setEnabled(true);
  }

}

//____________________________________________________________________
void VP1MaterialButton::apply()
{
  m_d->setLastAppliedFromCurrent();
  foreach (SoMaterial * m, m_d->handledmaterials)
    m_d->applyValuesToMaterial(m);
  updateButton();
}

//____________________________________________________________________
void VP1MaterialButton::reset()
{
  m_d->adaptGuiAndMaterialsToLastApplied();
}

//____________________________________________________________________
QList<SoMaterial*> VP1MaterialButton::handledMaterials() const
{
  return m_d->handledmaterials;
}

//____________________________________________________________________
void VP1MaterialButton::setText ( const QString & s )
{
  if (!s.isEmpty())
    message("VP1MaterialButton::setText() called (with '"+s+"'), but not allowed");
	//Forbidden!! Only here since Designer generated code needs it in public.
}

//____________________________________________________________________
void VP1MaterialButton::updateButton()
{
  if (objectName().isEmpty())
    setObjectName("VP1MaterialButton");
  messageVerbose("setColButtonProperties: color=" + str(m_d->lastapplied_diffuse));
  VP1ColorSelectButton::setColButtonProperties(this,m_d->lastapplied_diffuse,m_d->dim);
}

//____________________________________________________________________
void VP1MaterialButton::setDimension(int dim)
{
  if (m_d->dim == dim)
    return;
  m_d->dim = dim;
  updateButton();
}

//____________________________________________________________________
void VP1MaterialButton::transparencyChanged()
{
  QSlider * slider1(0);
  QSlider * slider2(0);
  if (sender()==m_d->editwindow_ui.slider_simple_transparency) {
    slider1 = m_d->editwindow_ui.slider_simple_transparency;
    slider2 = m_d->editwindow_ui.slider_transparency;
  } else {
    slider1 = m_d->editwindow_ui.slider_transparency;
    slider2 = m_d->editwindow_ui.slider_simple_transparency;
  }
  bool save = slider2->blockSignals(true);
  slider2->setValue(slider1->value());
  slider2->blockSignals(save);
  m_d->editwindow_ui.label_num_transparency->setNum(slider1->value());
  m_d->editwindow_ui.label_num_simple_transparency->setNum(slider1->value());

  updatePreview();
}

//____________________________________________________________________
void VP1MaterialButton::Imp::adaptGuiForMode()
{
  if (!editwindow)
    return;
  editwindow_ui.stackedWidget->setCurrentWidget(simplemode?editwindow_ui.page_simple:editwindow_ui.page_detailed);
  editwindow_ui.pushButton_switch_mode->setText("&Switch to "+QString(simplemode?"detailed":"simple")+" mode");
  if (simplemode)
    editwindow_ui.pushButton_switch_mode->setToolTip("Switch to detailed material editing for more options.");
  else
    editwindow_ui.pushButton_switch_mode->setToolTip("Switch to a simpler material editing. This might discard some information.");
}

//____________________________________________________________________
void VP1MaterialButton::Imp::switchModeBasedOnLastApplied()
{
  if (!editwindow)
    return;

  bool lastappliedaresimple(false);

  if (lastapplied_shininess==simpleShininess()
      &&equal(lastapplied_specular,simpleSpecular())
      &&equal(lastapplied_ambient,simpleAmbient())) {
    //Could be simple - test if emissive can be described as a
    //brightness multiplied by diffusive:
    double brightness = brightnessEstimateFromDetailedParameters(lastapplied_diffuse,lastapplied_emissive);
    if (brightness<1.0e-4) {
      lastappliedaresimple = equal(lastapplied_emissive,Qt::black);
    } else if (brightness>(1.0-1.0e-4)) {
      lastappliedaresimple = equal(lastapplied_emissive,lastapplied_diffuse);
    } else {
      lastappliedaresimple = equal(lastapplied_emissive,
				   emissiveColourFromSimpleParameters(lastapplied_diffuse, brightness));
    }
  }

  if (lastappliedaresimple!=simplemode)
    theclass->switchMode();
}


//____________________________________________________________________
void VP1MaterialButton::switchMode()
{
  if (!m_d->editwindow)
    return;

  const bool new_simplemode = !m_d->simplemode;

  //Transparency sliders should already be updated.

  m_d->blockGuiSignals(true);

  if (new_simplemode) {
    //Update detailed gui elements from simple gui elements. NB: All
    //the m_d->guiXX() methods returns detailed parameters until we
    //update m_d->simplemode below.
    double brightness = m_d->brightnessEstimateFromDetailedParameters(m_d->guiDiffuseColour(),m_d->guiEmissiveColour());
    int brightness_i = std::max<int>(0,std::min<int>(100,static_cast<int>(brightness*100+0.5)));
    QColor col = m_d->guiDiffuseColour();
    m_d->editwindow_ui.colbutton_simple_colour->setColor(col);
    m_d->editwindow_ui.slider_simple_brightness->setValue(brightness_i);
    m_d->editwindow_ui.label_num_simple_brightness->setNum(brightness_i);
  } else {
    //Update detailed gui elements from simple gui elements. NB: All
    //the m_d->guiXX() methods returns simple parameters until we
    //update m_d->simplemode below.
    m_d->editwindow_ui.colbutton_ambient->setColor(m_d->guiAmbientColour());
    m_d->editwindow_ui.colbutton_diffuse->setColor(m_d->guiDiffuseColour());
    m_d->editwindow_ui.colbutton_specular->setColor(m_d->guiSpecularColour());
    m_d->editwindow_ui.colbutton_emissive->setColor(m_d->guiEmissiveColour());
    m_d->editwindow_ui.slider_shininess->setValue(m_d->guiShininess());
    m_d->editwindow_ui.label_num_shininess->setNum(m_d->guiShininess());
  }

  m_d->simplemode = new_simplemode;
  m_d->adaptGuiForMode();

  m_d->blockGuiSignals(false);
  updatePreview();
}

//____________________________________________________________________
SoMaterial* VP1MaterialButton::createMaterial( const QColor& col, const double& brightness, const double& transp )
{
  SoMaterial * m = new SoMaterial;
  setMaterialParameters(m,col,brightness,transp);
  return m;
}

//____________________________________________________________________
SoMaterial* VP1MaterialButton::createMaterial( const double& r, const double& g, double b, const double& brightness,const double& transp )
{
  SoMaterial * m = new SoMaterial;
  setMaterialParameters(m,r,g,b,brightness,transp);
  return m;
}

//____________________________________________________________________
void VP1MaterialButton::setMaterialParameters( SoMaterial * m, const QColor& col, const double& brightness, const double& transp )
{
  setMaterialParameters( m, col.redF(), col.greenF(), col.blueF(), brightness, transp);
}

//____________________________________________________________________
void VP1MaterialButton::setMaterialParameters( SoMaterial * m, const double& in_r, const double& in_g, const double& in_b,
					       const double& in_brightness, const double& in_transp )
{
  if (!m)
    return;

  m->ref();
  bool save = m->enableNotify(false);

  const double r(std::max(0.0,std::min(1.0,in_r)));
  const double g(std::max(0.0,std::min(1.0,in_g)));
  const double b(std::max(0.0,std::min(1.0,in_b)));
  const double brightness(std::max(0.0,std::min(1.0,in_brightness)));


  m->diffuseColor.setValue(SbColor(r,g,b));
  m->emissiveColor.setValue(SbColor(std::max(0.0f,std::min(1.0f,float(brightness*r))),
				    std::max(0.0f,std::min(1.0f,float(brightness*g))),
				    std::max(0.0f,std::min(1.0f,float(brightness*b)))));
  m->transparency.setValue(std::max<float>(0.0f,std::min<float>(1.0f,in_transp)));
  m->shininess.setValue(std::max<float>(0.0f,std::min<float>(1.0f,Imp::simpleShininess()/100.0f)));
  m->ambientColor.setValue(Imp::qcol2sbcol(Imp::simpleAmbient()));
  m->specularColor.setValue(Imp::qcol2sbcol(Imp::simpleSpecular()));

  if (save) {
    m->enableNotify(true);
    m->touch();
  }
  m->unrefNoDelete();
}

//____________________________________________________________________
void VP1MaterialButton::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    m_d->dragStartPosition = event->pos();
  QPushButton::mousePressEvent(event);
}

//____________________________________________________________________
void VP1MaterialButton::mouseMoveEvent(QMouseEvent *event)
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

  QByteArray byteArray;
  QBuffer buffer(&byteArray);
  buffer.open(QIODevice::WriteOnly);
  QDataStream out(&buffer);
  out << m_d->lastapplied_ambient;
  out << m_d->lastapplied_diffuse;
  out << m_d->lastapplied_specular;
  out << m_d->lastapplied_emissive;
  out << m_d->lastapplied_shininess;
  out << m_d->lastapplied_transparency;
  out << m_d->lastapplied_brightness;
  buffer.close();
  mimeData->setData("vp1/material", byteArray);

  ////////////////////////////////////////////////////////
  //  For dragging c++ code for defining this material  //
  //  into e.g. an editor:                              //
  ////////////////////////////////////////////////////////

  QString s = "SoMaterial * mat = new SoMaterial;\n";
  QString str_ambient = m_d->toSbColTxt(m_d->lastapplied_ambient);
  if (str_ambient!="SbColor(0.2,0.2,0.2)")
    s += "mat->ambientColor.setValue("+str_ambient+");\n";
  QString str_diffuse = m_d->toSbColTxt(m_d->lastapplied_diffuse);
  if (str_diffuse!="SbColor(0.8,0.8,0.8)")
    s += "mat->diffuseColor.setValue("+str_diffuse+");\n";
  QString str_specular = m_d->toSbColTxt(m_d->lastapplied_specular);
  if (str_specular!="SbColor(0,0,0)")
    s += "mat->specularColor.setValue("+str_specular+");\n";
  QString str_emissive = m_d->toSbColTxt(m_d->lastapplied_emissive);
  if (str_emissive!="SbColor(0,0,0)")
    s += "mat->emissiveColor.setValue("+str_emissive+");\n";
  QString str_shininess = m_d->printFloat(m_d->lastapplied_shininess/100.0);
  if (str_shininess!="0.2")
    s +=     "mat->shininess.setValue("+str_shininess+");\n";
  QString str_transparency = m_d->printFloat(m_d->lastapplied_transparency/100.0);
  if (str_transparency!="0")
    s +=     "mat->transparency.setValue("+str_transparency+");\n";
  mimeData->setText(s);

  //Execute drag:
  drag->setMimeData(mimeData);//drag assumes ownership of mimeData
  drag->exec(Qt::CopyAction | Qt::MoveAction);
}

//____________________________________________________________________
void VP1MaterialButton::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->source()!=this && event->mimeData()->hasFormat("vp1/material"))
    event->acceptProposedAction();
}

//____________________________________________________________________
void VP1MaterialButton::dropEvent(QDropEvent *event)
{
  QByteArray data = event->mimeData()->data("vp1/material");
  event->acceptProposedAction();

  QBuffer buffer(&data);
  buffer.open(QIODevice::ReadOnly);
  QDataStream state(&buffer);
  state >> m_d->lastapplied_ambient;
  state >> m_d->lastapplied_diffuse;
  state >> m_d->lastapplied_specular;
  state >> m_d->lastapplied_emissive;
  state >> m_d->lastapplied_shininess;
  state >> m_d->lastapplied_transparency;
  state >> m_d->lastapplied_brightness;
  buffer.close();
  m_d->adaptGuiAndMaterialsToLastApplied();
}

QByteArray VP1MaterialButton::saveState() const{
  VP1Serialise serialise(1/*version*/);
  serialise.save(this);
  return serialise.result();
}

void VP1MaterialButton::restoreFromState( const QByteArray& ba){
  VP1Deserialise state(ba,systemBase());
  if (state.version()<0||state.version()>1)
    return;//Ignore silently
  state.restore(this);
}
