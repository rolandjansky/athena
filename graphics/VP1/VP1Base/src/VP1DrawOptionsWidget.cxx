/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1DrawOptionsWidget              //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: August 2008                              //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Base/VP1DrawOptionsWidget.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "ui_vp1drawoptionsform.h"
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoDrawStyle.h>


//____________________________________________________________________
class VP1DrawOptionsWidget::Imp {
public:
  Ui::DrawOptionsForm ui;
  SoGroup * group;
  SoComplexity * complexity;
  SoLightModel * baseLightModel;
  SoDrawStyle * drawStyle;
  bool linewidthsDisabled;
  bool pointsizesDisabled;
  bool complexityDisabled;
  bool baselightingDisabled;
  double lastEmittedComplexity;
};


//____________________________________________________________________
VP1DrawOptionsWidget::VP1DrawOptionsWidget(QWidget * parent)
  : QWidget(parent), m_d(new Imp)
{
  m_d->ui.setupUi(this);
  VP1QtInventorUtils::setLimitsLineWidthSlider(m_d->ui.horizontalSlider_linewidths);
  VP1QtInventorUtils::setValueLineWidthSlider(m_d->ui.horizontalSlider_linewidths,1.2);
  VP1QtInventorUtils::setLimitsPointSizeSlider(m_d->ui.horizontalSlider_pointsizes);
  VP1QtInventorUtils::setValuePointSizeSlider(m_d->ui.horizontalSlider_pointsizes,1.2);
  m_d->linewidthsDisabled = false;
  m_d->pointsizesDisabled = false;
  m_d->complexityDisabled = false;
  m_d->baselightingDisabled = false;
  m_d->group = new SoGroup;
  m_d->group->setName("DrawOptionsGroup");
  m_d->group->ref();
  m_d->complexity = new SoComplexity;
  m_d->complexity->ref();
  m_d->drawStyle = new SoDrawStyle;
  m_d->drawStyle->ref();
  m_d->baseLightModel = new SoLightModel;
  m_d->baseLightModel->ref();
  m_d->baseLightModel->model = SoLightModel::BASE_COLOR;
  m_d->lastEmittedComplexity = 0.5;
  updateNodes();
  connect(m_d->ui.horizontalSlider_linewidths,SIGNAL(valueChanged(int)),this,SLOT(updateNodes()));
  connect(m_d->ui.horizontalSlider_pointsizes,SIGNAL(valueChanged(int)),this,SLOT(updateNodes()));
  connect(m_d->ui.horizontalSlider_complexity,SIGNAL(valueChanged(int)),this,SLOT(updateNodes()));
  connect(m_d->ui.checkBox_useBaseLightModel,SIGNAL(toggled(bool)),this,SLOT(updateNodes()));
}

//____________________________________________________________________
VP1DrawOptionsWidget::~VP1DrawOptionsWidget()
{
  m_d->group->unref();
  m_d->complexity->unref();
  m_d->drawStyle->unref();
  m_d->baseLightModel->unref();
  delete m_d;
}

//____________________________________________________________________
SoGroup * VP1DrawOptionsWidget::drawOptionsGroup() const
{
  return m_d->group;
}

//____________________________________________________________________
double VP1DrawOptionsWidget::complexity() const
{
  if (m_d->complexityDisabled)
    return 0.5;
  const int val = m_d->ui.horizontalSlider_complexity->value();
  const int min = m_d->ui.horizontalSlider_complexity->minimum();
  const int max = m_d->ui.horizontalSlider_complexity->maximum();
  if(val==min)
    return 0.0;
  if(val==max)
    return 1.0;
  return std::min<double>(1.0,std::max<double>(0.0,(val-min)/(max*1.0)));
}

//____________________________________________________________________
void VP1DrawOptionsWidget::setComplexity(const double& c)
{
  int min = m_d->ui.horizontalSlider_complexity->minimum();
  int max = m_d->ui.horizontalSlider_complexity->maximum();
  int newval = std::min(max,std::max(min,(min+static_cast<int>((max-min)*c+0.5))));
  if (m_d->ui.horizontalSlider_complexity->value()!=newval) {
    m_d->ui.horizontalSlider_complexity->setValue(newval);
    updateNodes();
  }
}

//____________________________________________________________________
void VP1DrawOptionsWidget::setLineWidths(const double& lw)
{
  VP1QtInventorUtils::setValueLineWidthSlider(m_d->ui.horizontalSlider_linewidths,lw);
  updateNodes();
}

//____________________________________________________________________
void VP1DrawOptionsWidget::setPointSizes(const double& ps)
{
  VP1QtInventorUtils::setValuePointSizeSlider(m_d->ui.horizontalSlider_pointsizes,ps);
  updateNodes();
}

//____________________________________________________________________
void VP1DrawOptionsWidget::setUseBaseLighting(bool b)
{
  if (m_d->ui.checkBox_useBaseLightModel->isChecked()==b)
    return;
  m_d->ui.checkBox_useBaseLightModel->setChecked(b);
  updateNodes();
}

//____________________________________________________________________
void VP1DrawOptionsWidget::setComplexityDisabled(bool b)
{
  if (m_d->complexityDisabled==b)
    return;
  m_d->complexityDisabled = b;
  m_d->ui.label_curve_realism->setVisible(!b);
  m_d->ui.horizontalSlider_complexity->setVisible(!b);
  updateNodes();
}

//____________________________________________________________________
void VP1DrawOptionsWidget::setLineWidthsDisabled(bool b)
{
  if (m_d->linewidthsDisabled==b)
    return;
  m_d->linewidthsDisabled=b;
  m_d->ui.label_linewidths->setVisible(!b);
  m_d->ui.horizontalSlider_linewidths->setVisible(!b);
  updateNodes();
}

//____________________________________________________________________
void VP1DrawOptionsWidget::setPointSizesDisabled(bool b)
{
  if (m_d->pointsizesDisabled==b)
    return;
  m_d->pointsizesDisabled = b;
  m_d->ui.label_pointsizes->setVisible(!b);
  m_d->ui.horizontalSlider_pointsizes->setVisible(!b);
  updateNodes();
}

//____________________________________________________________________
void VP1DrawOptionsWidget::setBaseLightingDisabled(bool b)
{
  if (m_d->baselightingDisabled==b)
    return;
  m_d->baselightingDisabled=b;
  m_d->ui.checkBox_useBaseLightModel->setVisible(!b);
  updateNodes();
}

//____________________________________________________________________
void VP1DrawOptionsWidget::updateNodes()
{
  // 1) Update attachments

  bool drawStyleAttached(m_d->group->findChild(m_d->drawStyle)>=0);
  if (m_d->linewidthsDisabled&&m_d->pointsizesDisabled) {
    if (drawStyleAttached)
      m_d->group->removeChild(m_d->drawStyle);
  } else {
    if (!drawStyleAttached)
      m_d->group->addChild(m_d->drawStyle);
  }

  bool complexityAttached(m_d->group->findChild(m_d->complexity)>=0);
  if (m_d->complexityDisabled) {
    if (complexityAttached)
      m_d->group->removeChild(m_d->complexity);
  } else {
    if (!complexityAttached)
      m_d->group->addChild(m_d->complexity);
  }

  bool lightModelAttached(m_d->group->findChild(m_d->baseLightModel)>=0);
  bool lightModelAttachedGoal = !m_d->baselightingDisabled && m_d->ui.checkBox_useBaseLightModel->isChecked();
  if (lightModelAttachedGoal!=lightModelAttached) {
    if (lightModelAttached)
      m_d->group->removeChild(m_d->baseLightModel);
    else
      m_d->group->addChild(m_d->baseLightModel);
  }

  // 2) Emit complexityChanged?
  double complexityval = complexity();
  if (m_d->lastEmittedComplexity!=complexityval) {
    m_d->lastEmittedComplexity=complexityval;
    complexityChanged(m_d->lastEmittedComplexity);
  }

  // 3) Update fields
  if (!m_d->complexityDisabled){
    //We avoid setting the complexity value exactly to 0:
    complexityval = std::min<double>(1.0,std::max<double>(0.0,0.01+0.991*complexityval));
  }
  if (m_d->complexity->value.getValue()!=complexityval){
      m_d->complexity->value.setValue(complexityval);
  }
  if (!m_d->linewidthsDisabled||!m_d->pointsizesDisabled) {
    double val_lw = m_d->linewidthsDisabled ? 0 : VP1QtInventorUtils::getValueLineWidthSlider(m_d->ui.horizontalSlider_linewidths);
    double val_ps = m_d->pointsizesDisabled ? 0 : VP1QtInventorUtils::getValuePointSizeSlider(m_d->ui.horizontalSlider_pointsizes);
    if (m_d->drawStyle->lineWidth.getValue()!=val_lw)
      m_d->drawStyle->lineWidth = val_lw;
    if (m_d->drawStyle->pointSize.getValue()!=val_ps)
      m_d->drawStyle->pointSize = val_ps;
  }


}

//____________________________________________________________________
QByteArray VP1DrawOptionsWidget::state() const
{
  VP1Serialise s(0/*version*/);

  //Line width/point size:
  s.save(VP1QtInventorUtils::getValueLineWidthSlider(m_d->ui.horizontalSlider_linewidths));
  s.save(VP1QtInventorUtils::getValuePointSizeSlider(m_d->ui.horizontalSlider_pointsizes));
  s.widgetHandled(m_d->ui.horizontalSlider_linewidths);
  s.widgetHandled(m_d->ui.horizontalSlider_pointsizes);

  s.save(m_d->ui.horizontalSlider_complexity);
  s.save(m_d->ui.checkBox_useBaseLightModel);

  s.widgetHandled(this);
  s.warnUnsaved(this);

  return s.result();
}

//____________________________________________________________________
void VP1DrawOptionsWidget::applyState(QByteArray ba)
{
  VP1Deserialise s(ba);

  if (s.version()!=0)
    return;//ignore silently

  VP1QtInventorUtils::setValueLineWidthSlider(m_d->ui.horizontalSlider_linewidths,s.restoreDouble());
  VP1QtInventorUtils::setValuePointSizeSlider(m_d->ui.horizontalSlider_pointsizes,s.restoreDouble());
  s.widgetHandled(m_d->ui.horizontalSlider_linewidths);
  s.widgetHandled(m_d->ui.horizontalSlider_pointsizes);

  s.restore(m_d->ui.horizontalSlider_complexity);
  s.restore(m_d->ui.checkBox_useBaseLightModel);

  s.widgetHandled(this);
  s.warnUnrestored(this);

  updateNodes();

}

