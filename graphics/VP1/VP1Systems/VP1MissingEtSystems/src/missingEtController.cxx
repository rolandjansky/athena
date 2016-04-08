/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class missingEtController               //
//                                                            //
//  Author: Giorgi Gvaberidze (ggvaberi@cern.ch)              //
//  Initial version: July 2010                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1MissingEtSystems/missingEtController.h"
#include "VP1MissingEtSystems/VP1MissingEtCollWidget.h"
#include "VP1MissingEtSystems/VP1MissingEtHandle.h"
#include "VP1Base/IVP1System.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"

#include "ui_missingetcontroller.h"
#include "ui_missingEtPivot.h"

//____________________________________________________________________
class missingEtController::Imp {
public:
  Ui::missingEtControllerForm ui;
  Ui::missingEtPivotForm      uiPivot;
  VP1MissingEtCollWidget*     collWidget;
  bool 						  last_changeShape;
};

//____________________________________________________________________
missingEtController::missingEtController(IVP1System* sys)
:VP1Controller(sys, "missingEtController"), d(new Imp)
{
  d->ui.setupUi(this);
  d->collWidget = new VP1MissingEtCollWidget;
  setupCollWidgetInScrollArea(d->ui.collWidgetScrollArea, d->collWidget);

  initDialog(d->uiPivot, d->ui.pushButton_display);

  addUpdateSlot(SLOT(changeThickness()));
  connectToLastUpdateSlot(d->uiPivot.horizontalSlider_thickness);

  addUpdateSlot(SLOT(changeScale()));
  connectToLastUpdateSlot(d->uiPivot.horizontalSlider_scale);

  addUpdateSlot(SLOT(possibleChange_changeShape()));
  connectToLastUpdateSlot(d->ui.radioButton_arrow_shape);
  connectToLastUpdateSlot(d->ui.radioButton_dashline_shape);

  connect(this, SIGNAL(changeShapeChanged(bool)), this, SLOT(setArrow(bool)));

  initLastVars();

  VP1MissingEtHandle::useShape(true);
  refreshPivots();
}

//____________________________________________________________________
missingEtController::~missingEtController()
{
  delete d;
}

//____________________________________________________________________
int missingEtController::currentSettingsVersion() const
{
  return 1;
}

//____________________________________________________________________
void missingEtController::actualSaveSettings(VP1Serialise& s) const
{
 s.save(d->uiPivot.horizontalSlider_thickness);
 s.save(d->uiPivot.horizontalSlider_scale);
 s.save(d->ui.radioButton_arrow_shape,d->ui.radioButton_dashline_shape);//Version 1+
}

//____________________________________________________________________
void missingEtController::actualRestoreSettings(VP1Deserialise& s)
{
 if(s.version() < 0 || s.version() > 1){
  message("Warning: State data in .vp1 file has unsupported version ("+str(s.version())+")");
  return;
 }
 if (s.version() == 0)
  s.ignoreInt();

 s.restore(d->uiPivot.horizontalSlider_thickness);
 s.restore(d->uiPivot.horizontalSlider_scale);

 if(s.version() >= 1)
  s.restore(d->ui.radioButton_arrow_shape, d->ui.radioButton_dashline_shape);
}

//____________________________________________________________________
VP1CollectionWidget* missingEtController::collWidget() const
{
  return d->collWidget;
}

void missingEtController::refreshPivots(){
 foreach (VP1MissingEtHandle* handle,d->collWidget->collections<VP1MissingEtHandle>()){
  if(handle)
   handle->refresh();
 }
}

void missingEtController::setArrow(bool s)
{
 if(s && !VP1MissingEtHandle::shape){
  VP1MissingEtHandle::useShape(true);
  refreshPivots();
 }

 if(!s && VP1MissingEtHandle::shape){
  VP1MissingEtHandle::useShape(false);
  refreshPivots();
 }
}

void missingEtController::setDashline(bool /*s*/){
 VP1MissingEtHandle::useShape(false);
 refreshPivots();
}

bool missingEtController::changeShape() const{
 return d->ui.radioButton_arrow_shape->isChecked();
}

void missingEtController::changeThickness(){
 VP1MissingEtHandle::useThickness(d->uiPivot.horizontalSlider_thickness->value());
 refreshPivots();
}

void missingEtController::changeScale(){
 VP1MissingEtHandle::useScale(d->uiPivot.horizontalSlider_scale->value());
 refreshPivots();
}

#define VP1CONTROLLERCLASSNAME missingEtController
#include "VP1Base/VP1ControllerMacros.h"
POSSIBLECHANGE_IMP(changeShape)
//POSSIBLECHANGE_IMP(setDashLine)
//POSSIBLECHANGE_IMP(changeThickness)
//POSSIBLECHANGE_IMP(changeScale)
