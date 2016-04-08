/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1RawDataSysController           //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: January 2009                             //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1RawDataSystems/VP1RawDataSysController.h"
#include "ui_vp1rawdatacontrollerform.h"
#include "ui_rawdata_settings_display_form.h"
#include "ui_rawdata_settings_interactions_form.h"
#include "ui_rawdata_settings_cuts_form.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1CollectionWidget.h"

#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


//____________________________________________________________________
class VP1RawDataSysController::Imp {
public:
  Ui::VP1RawDataControllerForm ui;
  Ui::RawDataSysSettingsCutsForm ui_cuts;
  Ui::RawDataSysSettingsInteractionsForm ui_int;
  Ui::RawDataSysSettingsDisplayForm ui_display;
  VP1CollectionWidget * collWidget;

  VP1Interval last_cutAllowedEta;
  QList<VP1Interval> last_cutAllowedPhi;//All off: empty list. All on: list with one entry: ]-inf,inf[
  bool last_showVolumeOutLines;
  bool last_useSpecialTRTHTMaterial;
  bool last_useSpecialBCMHighAttMaterial;
  VP1RawDataFlags::InDetPartsFlags last_enabledInDetParts;
  bool last_trtRequireHT;
  VP1Interval last_trtAllowedToT;
  unsigned last_lucidMinQDC;
};

//____________________________________________________________________
VP1RawDataSysController::VP1RawDataSysController(IVP1System * sys)
  : VP1Controller(sys,"VP1RawDataSysController"), d(new Imp)
{
  d->ui.setupUi(this);

  d->collWidget = new VP1CollectionWidget;
  setupCollWidgetInScrollArea(d->ui.collWidgetScrollArea,d->collWidget);

  initDialog(d->ui_display, d->ui.pushButton_settings_display);
  initDialog(d->ui_int, d->ui.pushButton_settings_interactions);
  initDialog(d->ui_cuts, d->ui.pushButton_settings_cuts);

  d->ui_display.widget_drawOptions->setLineWidths(1.5);
  d->ui_display.widget_drawOptions->setPointSizes(2.5);
  d->ui_display.widget_drawOptions->setComplexityDisabled();

  d->ui_cuts.etaPhiCutWidget->setEtaCutEnabled(false);

  d->ui_display.matButton_trtht->setMaterial(VP1MaterialButton::createMaterial(1.0,0.0,0.0));
  d->ui_display.matButton_bcmhighatt->setMaterial(VP1MaterialButton::createMaterial(1.0,0.0,0.0));


  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  Setup connections which monitor changes in the controller so that we may emit signals as appropriate:  //
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // -> cutAllowedEta:
  addUpdateSlot(SLOT(possibleChange_cutAllowedEta()));
  connectToLastUpdateSlot(d->ui_cuts.etaPhiCutWidget,SIGNAL(allowedEtaChanged(const VP1Interval&)));

  // -> cutAllowedPhi:
  addUpdateSlot(SLOT(possibleChange_cutAllowedPhi()));
  connectToLastUpdateSlot(d->ui_cuts.etaPhiCutWidget,SIGNAL(allowedPhiChanged(const QList<VP1Interval>&)));

  // -> showVolumeOutLines
  addUpdateSlot(SLOT(possibleChange_showVolumeOutLines()));
  connectToLastUpdateSlot(d->ui_display.checkBox_showVolumeOutLines);

  // -> useSpecialTRTHTMaterial
  addUpdateSlot(SLOT(possibleChange_useSpecialTRTHTMaterial()));
  connectToLastUpdateSlot(d->ui_display.checkBox_trtHTUseSpecialMat);

  // -> useSpecialBCMHighAttMaterial
  addUpdateSlot(SLOT(possibleChange_useSpecialBCMHighAttMaterial()));
  connectToLastUpdateSlot(d->ui_display.checkBox_bcmHighAttUseSpecialMat);

  // -> enabledInDetParts
  addUpdateSlot(SLOT(possibleChange_enabledInDetParts()));
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_indet_barrelA);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_indet_barrelC);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_indet_endcapA);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_indet_endcapC);

  // -> trtRequireHT
  addUpdateSlot(SLOT(possibleChange_trtRequireHT()));
  connectToLastUpdateSlot(d->ui_cuts.checkbox_cut_trt_needHT);

  // -> trtAllowedToT
  addUpdateSlot(SLOT(possibleChange_trtAllowedToT()));
  connectToLastUpdateSlot(d->ui_cuts.checkbox_cut_trt_TotMin);
  connectToLastUpdateSlot(d->ui_cuts.spinBox_cut_trt_ToTMin);
  connectToLastUpdateSlot(d->ui_cuts.checkbox_cut_trt_TotMax);
  connectToLastUpdateSlot(d->ui_cuts.spinBox_cut_trt_ToTMax);

  // -> lucidMinQDC
  addUpdateSlot(SLOT(possibleChange_lucidMinQDC()));
  connectToLastUpdateSlot(d->ui_cuts.spinBox_lucid_qdc_min);

  initLastVars();
}

//____________________________________________________________________
VP1RawDataSysController::~VP1RawDataSysController()
{
  delete d;
}


//____________________________________________________________________
int VP1RawDataSysController::currentSettingsVersion() const
{
  return 1;
}

//____________________________________________________________________
void VP1RawDataSysController::actualSaveSettings(VP1Serialise&s) const
{
  s.save(d->ui_cuts.etaPhiCutWidget);
  s.save(d->ui_cuts.checkBox_cut_indet_barrelA);
  s.save(d->ui_cuts.checkBox_cut_indet_barrelC);
  s.save(d->ui_cuts.checkBox_cut_indet_endcapA);
  s.save(d->ui_cuts.checkBox_cut_indet_endcapC);
  s.save(d->ui_cuts.checkbox_cut_trt_needHT);
  s.save(d->ui_cuts.checkbox_cut_trt_TotMin);
  s.save(d->ui_cuts.spinBox_cut_trt_ToTMin);
  s.save(d->ui_cuts.checkbox_cut_trt_TotMax);
  s.save(d->ui_cuts.spinBox_cut_trt_ToTMax);
  s.save(d->ui_display.checkBox_showVolumeOutLines);
  s.save(d->ui_display.widget_drawOptions);
  s.save(d->ui_display.checkBox_trtHTUseSpecialMat);
  s.save(d->ui_display.checkBox_bcmHighAttUseSpecialMat);
  s.save(d->ui_display.matButton_trtht);
  s.save(d->ui_display.matButton_bcmhighatt);
  s.save(d->ui_int.checkBox_printinfo);
  s.save(d->ui_int.checkBox_printinfo_verbose);
  s.save(d->ui_int.checkBox_zoom);
  s.save(d->ui_cuts.spinBox_lucid_qdc_min);//Version 1+
}

//____________________________________________________________________
void VP1RawDataSysController::actualRestoreSettings(VP1Deserialise& s)
{
  if (s.version()<0||s.version()>1) {
    message("Warning: State data in .vp1 file has unsupported version ("+str(s.version())+")");
    return;
  }
  s.restore(d->ui_cuts.etaPhiCutWidget);
  s.restore(d->ui_cuts.checkBox_cut_indet_barrelA);
  s.restore(d->ui_cuts.checkBox_cut_indet_barrelC);
  s.restore(d->ui_cuts.checkBox_cut_indet_endcapA);
  s.restore(d->ui_cuts.checkBox_cut_indet_endcapC);
  s.restore(d->ui_cuts.checkbox_cut_trt_needHT);
  s.restore(d->ui_cuts.checkbox_cut_trt_TotMin);
  s.restore(d->ui_cuts.spinBox_cut_trt_ToTMin);
  s.restore(d->ui_cuts.checkbox_cut_trt_TotMax);
  s.restore(d->ui_cuts.spinBox_cut_trt_ToTMax);
  s.restore(d->ui_display.checkBox_showVolumeOutLines);
  s.restore(d->ui_display.widget_drawOptions);
  s.restore(d->ui_display.checkBox_trtHTUseSpecialMat);
  s.restore(d->ui_display.checkBox_bcmHighAttUseSpecialMat);
  s.restore(d->ui_display.matButton_trtht);
  s.restore(d->ui_display.matButton_bcmhighatt);
  s.restore(d->ui_int.checkBox_printinfo);
  s.restore(d->ui_int.checkBox_printinfo_verbose);
  s.restore(d->ui_int.checkBox_zoom);
  if (s.version()>=1)
    s.restore(d->ui_cuts.spinBox_lucid_qdc_min);
}

//____________________________________________________________________
VP1CollectionWidget * VP1RawDataSysController::collWidget() const
{
  return d->collWidget;
}

//____________________________________________________________________
SoGroup * VP1RawDataSysController::drawOptions() const
{
  return d->ui_display.widget_drawOptions->drawOptionsGroup();
}

//____________________________________________________________________
bool VP1RawDataSysController::printInfoOnClick() const { return d->ui_int.checkBox_printinfo->isChecked(); }
bool VP1RawDataSysController::printVerboseInfoOnClick() const { return d->ui_int.checkBox_printinfo_verbose->isChecked(); }
bool VP1RawDataSysController::zoomOnClick() const { return d->ui_int.checkBox_zoom->isChecked(); }

//____________________________________________________________________
VP1Interval VP1RawDataSysController::cutAllowedEta() const
{
  return d->ui_cuts.etaPhiCutWidget->allowedEta();
}

//____________________________________________________________________
QList<VP1Interval> VP1RawDataSysController::cutAllowedPhi() const
{
  return d->ui_cuts.etaPhiCutWidget->allowedPhi();
}

//____________________________________________________________________
bool VP1RawDataSysController::showVolumeOutLines() const
{
  return d->ui_display.checkBox_showVolumeOutLines->isChecked();
}

//____________________________________________________________________
bool VP1RawDataSysController::useSpecialTRTHTMaterial() const
{
  return d->ui_display.checkBox_trtHTUseSpecialMat->isChecked();
}

//____________________________________________________________________
bool VP1RawDataSysController::useSpecialBCMHighAttMaterial() const
{
  return d->ui_display.checkBox_bcmHighAttUseSpecialMat->isChecked();
}

//____________________________________________________________________
VP1RawDataFlags::InDetPartsFlags VP1RawDataSysController::enabledInDetParts() const
{
  VP1RawDataFlags::InDetPartsFlags f;
  if (d->ui_cuts.checkBox_cut_indet_barrelA->isChecked())
    f |= VP1RawDataFlags::BarrelPositive;
  if (d->ui_cuts.checkBox_cut_indet_barrelC->isChecked())
    f |= VP1RawDataFlags::BarrelNegative;
  if (d->ui_cuts.checkBox_cut_indet_endcapA->isChecked())
    f |= VP1RawDataFlags::EndCapPositive;
  if (d->ui_cuts.checkBox_cut_indet_endcapC->isChecked())
    f |= VP1RawDataFlags::EndCapNegative;
  return f;
}

//____________________________________________________________________
bool VP1RawDataSysController::trtRequireHT() const
{
  return d->ui_cuts.checkbox_cut_trt_needHT->isChecked();
}

//____________________________________________________________________
VP1Interval VP1RawDataSysController::trtAllowedToT() const
{
  VP1Interval i( (d->ui_cuts.checkbox_cut_trt_TotMin->isChecked() ? (d->ui_cuts.spinBox_cut_trt_ToTMin->value()-0.5)*3.125*Gaudi::Units::nanosecond : -VP1Interval::inf() ),
		 (d->ui_cuts.checkbox_cut_trt_TotMax->isChecked() ? (d->ui_cuts.spinBox_cut_trt_ToTMax->value()+0.5)*3.125*Gaudi::Units::nanosecond : VP1Interval::inf() ) );
  return i.lower()>i.upper() ? VP1Interval() : i;
}

//____________________________________________________________________
unsigned VP1RawDataSysController::lucidMinQDC() const
{
  return d->ui_cuts.spinBox_lucid_qdc_min->value();
}

//____________________________________________________________________
SoMaterial * VP1RawDataSysController::trtHTMaterial() const
{
  return getMaterial(d->ui_display.matButton_trtht);
}

//____________________________________________________________________
SoMaterial * VP1RawDataSysController::bcmHighAttMaterial() const
{
  return getMaterial(d->ui_display.matButton_bcmhighatt);
}

///////////////////////////////////////////////////////////////////////////
// Test for possible changes in values and emit signals as appropriate:
// (possibleChange_XXX() slots code provided by macros)
#define VP1CONTROLLERCLASSNAME VP1RawDataSysController
#include "VP1Base/VP1ControllerMacros.h"
POSSIBLECHANGE_IMP(cutAllowedEta)
POSSIBLECHANGE_IMP(cutAllowedPhi)
POSSIBLECHANGE_IMP(showVolumeOutLines)
POSSIBLECHANGE_IMP(useSpecialTRTHTMaterial)
POSSIBLECHANGE_IMP(useSpecialBCMHighAttMaterial)
POSSIBLECHANGE_IMP(enabledInDetParts)
POSSIBLECHANGE_IMP(trtRequireHT)
POSSIBLECHANGE_IMP(trtAllowedToT)
POSSIBLECHANGE_IMP(lucidMinQDC)
