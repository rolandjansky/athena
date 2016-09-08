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
  : QWidget(parent), d(new Imp)
{
  d->ui.setupUi(this);
  VP1QtInventorUtils::setLimitsLineWidthSlider(d->ui.horizontalSlider_linewidths);
  VP1QtInventorUtils::setValueLineWidthSlider(d->ui.horizontalSlider_linewidths,1.2);
  VP1QtInventorUtils::setLimitsPointSizeSlider(d->ui.horizontalSlider_pointsizes);
  VP1QtInventorUtils::setValuePointSizeSlider(d->ui.horizontalSlider_pointsizes,1.2);
  d->linewidthsDisabled = false;
  d->pointsizesDisabled = false;
  d->complexityDisabled = false;
  d->baselightingDisabled = false;
  d->group = new SoGroup;
  d->group->setName("DrawOptionsGroup");
  d->group->ref();
  d->complexity = new SoComplexity;
  d->complexity->ref();
  d->drawStyle = new SoDrawStyle;
  d->drawStyle->ref();
  d->baseLightModel = new SoLightModel;
  d->baseLightModel->ref();
  d->baseLightModel->model = SoLightModel::BASE_COLOR;
  d->lastEmittedComplexity = 0.5;
  updateNodes();
  connect(d->ui.horizontalSlider_linewidths,SIGNAL(valueChanged(int)),this,SLOT(updateNodes()));
  connect(d->ui.horizontalSlider_pointsizes,SIGNAL(valueChanged(int)),this,SLOT(updateNodes()));
  connect(d->ui.horizontalSlider_complexity,SIGNAL(valueChanged(int)),this,SLOT(updateNodes()));
  connect(d->ui.checkBox_useBaseLightModel,SIGNAL(toggled(bool)),this,SLOT(updateNodes()));
}

//____________________________________________________________________
VP1DrawOptionsWidget::~VP1DrawOptionsWidget()
{
  d->group->unref();
  d->complexity->unref();
  d->drawStyle->unref();
  d->baseLightModel->unref();
  delete d;
}

//____________________________________________________________________
SoGroup * VP1DrawOptionsWidget::drawOptionsGroup() const
{
  return d->group;
}

//____________________________________________________________________
double VP1DrawOptionsWidget::complexity() const
{
  if (d->complexityDisabled)
    return 0.5;
  const int val = d->ui.horizontalSlider_complexity->value();
  const int min = d->ui.horizontalSlider_complexity->minimum();
  const int max = d->ui.horizontalSlider_complexity->maximum();
  if(val==min)
    return 0.0;
  if(val==max)
    return 1.0;
  return std::min<double>(1.0,std::max<double>(0.0,(val-min)/(max*1.0)));
}

//____________________________________________________________________
void VP1DrawOptionsWidget::setComplexity(const double& c)
{
  int min = d->ui.horizontalSlider_complexity->minimum();
  int max = d->ui.horizontalSlider_complexity->maximum();
  int newval = std::min(max,std::max(min,(min+static_cast<int>((max-min)*c+0.5))));
  if (d->ui.horizontalSlider_complexity->value()!=newval) {
    d->ui.horizontalSlider_complexity->setValue(newval);
    updateNodes();
  }
}

//____________________________________________________________________
void VP1DrawOptionsWidget::setLineWidths(const double& lw)
{
  VP1QtInventorUtils::setValueLineWidthSlider(d->ui.horizontalSlider_linewidths,lw);
  updateNodes();
}

//____________________________________________________________________
void VP1DrawOptionsWidget::setPointSizes(const double& ps)
{
  VP1QtInventorUtils::setValuePointSizeSlider(d->ui.horizontalSlider_pointsizes,ps);
  updateNodes();
}

//____________________________________________________________________
void VP1DrawOptionsWidget::setUseBaseLighting(bool b)
{
  if (d->ui.checkBox_useBaseLightModel->isChecked()==b)
    return;
  d->ui.checkBox_useBaseLightModel->setChecked(b);
  updateNodes();
}

//____________________________________________________________________
void VP1DrawOptionsWidget::setComplexityDisabled(bool b)
{
  if (d->complexityDisabled==b)
    return;
  d->complexityDisabled = b;
  d->ui.label_curve_realism->setVisible(!b);
  d->ui.horizontalSlider_complexity->setVisible(!b);
  updateNodes();
}

//____________________________________________________________________
void VP1DrawOptionsWidget::setLineWidthsDisabled(bool b)
{
  if (d->linewidthsDisabled==b)
    return;
  d->linewidthsDisabled=b;
  d->ui.label_linewidths->setVisible(!b);
  d->ui.horizontalSlider_linewidths->setVisible(!b);
  updateNodes();
}

//____________________________________________________________________
void VP1DrawOptionsWidget::setPointSizesDisabled(bool b)
{
  if (d->pointsizesDisabled==b)
    return;
  d->pointsizesDisabled = b;
  d->ui.label_pointsizes->setVisible(!b);
  d->ui.horizontalSlider_pointsizes->setVisible(!b);
  updateNodes();
}

//____________________________________________________________________
void VP1DrawOptionsWidget::setBaseLightingDisabled(bool b)
{
  if (d->baselightingDisabled==b)
    return;
  d->baselightingDisabled=b;
  d->ui.checkBox_useBaseLightModel->setVisible(!b);
  updateNodes();
}

//____________________________________________________________________
void VP1DrawOptionsWidget::updateNodes()
{
  // 1) Update attachments

  bool drawStyleAttached(d->group->findChild(d->drawStyle)>=0);
  if (d->linewidthsDisabled&&d->pointsizesDisabled) {
    if (drawStyleAttached)
      d->group->removeChild(d->drawStyle);
  } else {
    if (!drawStyleAttached)
      d->group->addChild(d->drawStyle);
  }

  bool complexityAttached(d->group->findChild(d->complexity)>=0);
  if (d->complexityDisabled) {
    if (complexityAttached)
      d->group->removeChild(d->complexity);
  } else {
    if (!complexityAttached)
      d->group->addChild(d->complexity);
  }

  bool lightModelAttached(d->group->findChild(d->baseLightModel)>=0);
  bool lightModelAttachedGoal = !d->baselightingDisabled && d->ui.checkBox_useBaseLightModel->isChecked();
  if (lightModelAttachedGoal!=lightModelAttached) {
    if (lightModelAttached)
      d->group->removeChild(d->baseLightModel);
    else
      d->group->addChild(d->baseLightModel);
  }

  // 2) Emit complexityChanged?
  double complexityval = complexity();
  if (d->lastEmittedComplexity!=complexityval) {
    d->lastEmittedComplexity=complexityval;
    complexityChanged(d->lastEmittedComplexity);
  }

  // 3) Update fields
  if (!d->complexityDisabled){
    //We avoid setting the complexity value exactly to 0:
    complexityval = std::min<double>(1.0,std::max<double>(0.0,0.01+0.991*complexityval));
  }
  if (d->complexity->value.getValue()!=complexityval){
      d->complexity->value.setValue(complexityval);
  }
  if (!d->linewidthsDisabled||!d->pointsizesDisabled) {
    double val_lw = d->linewidthsDisabled ? 0 : VP1QtInventorUtils::getValueLineWidthSlider(d->ui.horizontalSlider_linewidths);
    double val_ps = d->pointsizesDisabled ? 0 : VP1QtInventorUtils::getValuePointSizeSlider(d->ui.horizontalSlider_pointsizes);
    if (d->drawStyle->lineWidth.getValue()!=val_lw)
      d->drawStyle->lineWidth = val_lw;
    if (d->drawStyle->pointSize.getValue()!=val_ps)
      d->drawStyle->pointSize = val_ps;
  }


}

//____________________________________________________________________
QByteArray VP1DrawOptionsWidget::state() const
{
  VP1Serialise s(0/*version*/);

  //Line width/point size:
  s.save(VP1QtInventorUtils::getValueLineWidthSlider(d->ui.horizontalSlider_linewidths));
  s.save(VP1QtInventorUtils::getValuePointSizeSlider(d->ui.horizontalSlider_pointsizes));
  s.widgetHandled(d->ui.horizontalSlider_linewidths);
  s.widgetHandled(d->ui.horizontalSlider_pointsizes);

  s.save(d->ui.horizontalSlider_complexity);
  s.save(d->ui.checkBox_useBaseLightModel);

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

  VP1QtInventorUtils::setValueLineWidthSlider(d->ui.horizontalSlider_linewidths,s.restoreDouble());
  VP1QtInventorUtils::setValuePointSizeSlider(d->ui.horizontalSlider_pointsizes,s.restoreDouble());
  s.widgetHandled(d->ui.horizontalSlider_linewidths);
  s.widgetHandled(d->ui.horizontalSlider_pointsizes);

  s.restore(d->ui.horizontalSlider_complexity);
  s.restore(d->ui.checkBox_useBaseLightModel);

  s.widgetHandled(this);
  s.warnUnrestored(this);

  updateNodes();

}

