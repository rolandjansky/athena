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

#define VP1IMPVARNAME m_d

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
  Ui::VP1RawDataControllerForm ui{};
  Ui::RawDataSysSettingsCutsForm ui_cuts{};
  Ui::RawDataSysSettingsInteractionsForm ui_int{};
  Ui::RawDataSysSettingsDisplayForm ui_display{};
  VP1CollectionWidget * collWidget = nullptr;

  VP1Interval last_cutAllowedEta;
  QList<VP1Interval> last_cutAllowedPhi;//All off: empty list. All on: list with one entry: ]-inf,inf[
  bool last_showVolumeOutLines = false;
  bool last_useSpecialTRTHTMaterial = false;
  bool last_useSpecialBCMHighAttMaterial = false;
  VP1RawDataFlags::InDetPartsFlags last_enabledInDetParts;
  bool last_trtRequireHT = false;
  VP1Interval last_trtAllowedToT;
  unsigned last_lucidMinQDC = 0U;
};

//____________________________________________________________________
VP1RawDataSysController::VP1RawDataSysController(IVP1System * sys)
  : VP1Controller(sys,"VP1RawDataSysController"), m_d(new Imp)
{
  m_d->ui.setupUi(this);

  m_d->collWidget = new VP1CollectionWidget;
  setupCollWidgetInScrollArea(m_d->ui.collWidgetScrollArea,m_d->collWidget);

  initDialog(m_d->ui_display, m_d->ui.pushButton_settings_display);
  initDialog(m_d->ui_int, m_d->ui.pushButton_settings_interactions);
  initDialog(m_d->ui_cuts, m_d->ui.pushButton_settings_cuts);

  m_d->ui_display.widget_drawOptions->setLineWidths(1.5);
  m_d->ui_display.widget_drawOptions->setPointSizes(2.5);
  m_d->ui_display.widget_drawOptions->setComplexityDisabled();

  m_d->ui_cuts.etaPhiCutWidget->setEtaCutEnabled(false);

  m_d->ui_display.matButton_trtht->setMaterial(VP1MaterialButton::createMaterial(1.0,0.0,0.0));
  m_d->ui_display.matButton_bcmhighatt->setMaterial(VP1MaterialButton::createMaterial(1.0,0.0,0.0));


  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  Setup connections which monitor changes in the controller so that we may emit signals as appropriate:  //
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // -> cutAllowedEta:
  addUpdateSlot(SLOT(possibleChange_cutAllowedEta()));
  connectToLastUpdateSlot(m_d->ui_cuts.etaPhiCutWidget,SIGNAL(allowedEtaChanged(const VP1Interval&)));

  // -> cutAllowedPhi:
  addUpdateSlot(SLOT(possibleChange_cutAllowedPhi()));
  connectToLastUpdateSlot(m_d->ui_cuts.etaPhiCutWidget,SIGNAL(allowedPhiChanged(const QList<VP1Interval>&)));

  // -> showVolumeOutLines
  addUpdateSlot(SLOT(possibleChange_showVolumeOutLines()));
  connectToLastUpdateSlot(m_d->ui_display.checkBox_showVolumeOutLines);

  // -> useSpecialTRTHTMaterial
  addUpdateSlot(SLOT(possibleChange_useSpecialTRTHTMaterial()));
  connectToLastUpdateSlot(m_d->ui_display.checkBox_trtHTUseSpecialMat);

  // -> useSpecialBCMHighAttMaterial
  addUpdateSlot(SLOT(possibleChange_useSpecialBCMHighAttMaterial()));
  connectToLastUpdateSlot(m_d->ui_display.checkBox_bcmHighAttUseSpecialMat);

  // -> enabledInDetParts
  addUpdateSlot(SLOT(possibleChange_enabledInDetParts()));
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_cut_indet_barrelA);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_cut_indet_barrelC);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_cut_indet_endcapA);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_cut_indet_endcapC);

  // -> trtRequireHT
  addUpdateSlot(SLOT(possibleChange_trtRequireHT()));
  connectToLastUpdateSlot(m_d->ui_cuts.checkbox_cut_trt_needHT);

  // -> trtAllowedToT
  addUpdateSlot(SLOT(possibleChange_trtAllowedToT()));
  connectToLastUpdateSlot(m_d->ui_cuts.checkbox_cut_trt_TotMin);
  connectToLastUpdateSlot(m_d->ui_cuts.spinBox_cut_trt_ToTMin);
  connectToLastUpdateSlot(m_d->ui_cuts.checkbox_cut_trt_TotMax);
  connectToLastUpdateSlot(m_d->ui_cuts.spinBox_cut_trt_ToTMax);

  // -> lucidMinQDC
  addUpdateSlot(SLOT(possibleChange_lucidMinQDC()));
  connectToLastUpdateSlot(m_d->ui_cuts.spinBox_lucid_qdc_min);

  initLastVars();
}

//____________________________________________________________________
VP1RawDataSysController::~VP1RawDataSysController()
{
  delete m_d;
}


//____________________________________________________________________
int VP1RawDataSysController::currentSettingsVersion() const
{
  return 1;
}

//____________________________________________________________________
void VP1RawDataSysController::actualSaveSettings(VP1Serialise&s) const
{
  s.save(m_d->ui_cuts.etaPhiCutWidget);
  s.save(m_d->ui_cuts.checkBox_cut_indet_barrelA);
  s.save(m_d->ui_cuts.checkBox_cut_indet_barrelC);
  s.save(m_d->ui_cuts.checkBox_cut_indet_endcapA);
  s.save(m_d->ui_cuts.checkBox_cut_indet_endcapC);
  s.save(m_d->ui_cuts.checkbox_cut_trt_needHT);
  s.save(m_d->ui_cuts.checkbox_cut_trt_TotMin);
  s.save(m_d->ui_cuts.spinBox_cut_trt_ToTMin);
  s.save(m_d->ui_cuts.checkbox_cut_trt_TotMax);
  s.save(m_d->ui_cuts.spinBox_cut_trt_ToTMax);
  s.save(m_d->ui_display.checkBox_showVolumeOutLines);
  s.save(m_d->ui_display.widget_drawOptions);
  s.save(m_d->ui_display.checkBox_trtHTUseSpecialMat);
  s.save(m_d->ui_display.checkBox_bcmHighAttUseSpecialMat);
  s.save(m_d->ui_display.matButton_trtht);
  s.save(m_d->ui_display.matButton_bcmhighatt);
  s.save(m_d->ui_int.checkBox_printinfo);
  s.save(m_d->ui_int.checkBox_printinfo_verbose);
  s.save(m_d->ui_int.checkBox_zoom);
  s.save(m_d->ui_cuts.spinBox_lucid_qdc_min);//Version 1+
}

//____________________________________________________________________
void VP1RawDataSysController::actualRestoreSettings(VP1Deserialise& s)
{
  if (s.version()<0||s.version()>1) {
    message("Warning: State data in .vp1 file has unsupported version ("+str(s.version())+")");
    return;
  }
  s.restore(m_d->ui_cuts.etaPhiCutWidget);
  s.restore(m_d->ui_cuts.checkBox_cut_indet_barrelA);
  s.restore(m_d->ui_cuts.checkBox_cut_indet_barrelC);
  s.restore(m_d->ui_cuts.checkBox_cut_indet_endcapA);
  s.restore(m_d->ui_cuts.checkBox_cut_indet_endcapC);
  s.restore(m_d->ui_cuts.checkbox_cut_trt_needHT);
  s.restore(m_d->ui_cuts.checkbox_cut_trt_TotMin);
  s.restore(m_d->ui_cuts.spinBox_cut_trt_ToTMin);
  s.restore(m_d->ui_cuts.checkbox_cut_trt_TotMax);
  s.restore(m_d->ui_cuts.spinBox_cut_trt_ToTMax);
  s.restore(m_d->ui_display.checkBox_showVolumeOutLines);
  s.restore(m_d->ui_display.widget_drawOptions);
  s.restore(m_d->ui_display.checkBox_trtHTUseSpecialMat);
  s.restore(m_d->ui_display.checkBox_bcmHighAttUseSpecialMat);
  s.restore(m_d->ui_display.matButton_trtht);
  s.restore(m_d->ui_display.matButton_bcmhighatt);
  s.restore(m_d->ui_int.checkBox_printinfo);
  s.restore(m_d->ui_int.checkBox_printinfo_verbose);
  s.restore(m_d->ui_int.checkBox_zoom);
  if (s.version()>=1)
    s.restore(m_d->ui_cuts.spinBox_lucid_qdc_min);
}

//____________________________________________________________________
VP1CollectionWidget * VP1RawDataSysController::collWidget() const
{
  return m_d->collWidget;
}

//____________________________________________________________________
SoGroup * VP1RawDataSysController::drawOptions() const
{
  return m_d->ui_display.widget_drawOptions->drawOptionsGroup();
}

//____________________________________________________________________
bool VP1RawDataSysController::printInfoOnClick() const { return m_d->ui_int.checkBox_printinfo->isChecked(); }
bool VP1RawDataSysController::printVerboseInfoOnClick() const { return m_d->ui_int.checkBox_printinfo_verbose->isChecked(); }
bool VP1RawDataSysController::zoomOnClick() const { return m_d->ui_int.checkBox_zoom->isChecked(); }

//____________________________________________________________________
VP1Interval VP1RawDataSysController::cutAllowedEta() const
{
  return m_d->ui_cuts.etaPhiCutWidget->allowedEta();
}

//____________________________________________________________________
QList<VP1Interval> VP1RawDataSysController::cutAllowedPhi() const
{
  return m_d->ui_cuts.etaPhiCutWidget->allowedPhi();
}

//____________________________________________________________________
bool VP1RawDataSysController::showVolumeOutLines() const
{
  return m_d->ui_display.checkBox_showVolumeOutLines->isChecked();
}

//____________________________________________________________________
bool VP1RawDataSysController::useSpecialTRTHTMaterial() const
{
  return m_d->ui_display.checkBox_trtHTUseSpecialMat->isChecked();
}

//____________________________________________________________________
bool VP1RawDataSysController::useSpecialBCMHighAttMaterial() const
{
  return m_d->ui_display.checkBox_bcmHighAttUseSpecialMat->isChecked();
}

//____________________________________________________________________
VP1RawDataFlags::InDetPartsFlags VP1RawDataSysController::enabledInDetParts() const
{
  VP1RawDataFlags::InDetPartsFlags f;
  if (m_d->ui_cuts.checkBox_cut_indet_barrelA->isChecked())
    f |= VP1RawDataFlags::BarrelPositive;
  if (m_d->ui_cuts.checkBox_cut_indet_barrelC->isChecked())
    f |= VP1RawDataFlags::BarrelNegative;
  if (m_d->ui_cuts.checkBox_cut_indet_endcapA->isChecked())
    f |= VP1RawDataFlags::EndCapPositive;
  if (m_d->ui_cuts.checkBox_cut_indet_endcapC->isChecked())
    f |= VP1RawDataFlags::EndCapNegative;
  return f;
}

//____________________________________________________________________
bool VP1RawDataSysController::trtRequireHT() const
{
  return m_d->ui_cuts.checkbox_cut_trt_needHT->isChecked();
}

//____________________________________________________________________
VP1Interval VP1RawDataSysController::trtAllowedToT() const
{
  VP1Interval i( (m_d->ui_cuts.checkbox_cut_trt_TotMin->isChecked() ? (m_d->ui_cuts.spinBox_cut_trt_ToTMin->value()-0.5)*3.125*Gaudi::Units::nanosecond : -VP1Interval::inf() ),
		 (m_d->ui_cuts.checkbox_cut_trt_TotMax->isChecked() ? (m_d->ui_cuts.spinBox_cut_trt_ToTMax->value()+0.5)*3.125*Gaudi::Units::nanosecond : VP1Interval::inf() ) );
  return i.lower()>i.upper() ? VP1Interval() : i;
}

//____________________________________________________________________
unsigned VP1RawDataSysController::lucidMinQDC() const
{
  return m_d->ui_cuts.spinBox_lucid_qdc_min->value();
}

//____________________________________________________________________
SoMaterial * VP1RawDataSysController::trtHTMaterial() const
{
  return getMaterial(m_d->ui_display.matButton_trtht);
}

//____________________________________________________________________
SoMaterial * VP1RawDataSysController::bcmHighAttMaterial() const
{
  return getMaterial(m_d->ui_display.matButton_bcmhighatt);
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
