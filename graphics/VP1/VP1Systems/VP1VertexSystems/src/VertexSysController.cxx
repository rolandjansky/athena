/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VertexSysController               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1VertexSystems/VertexSysController.h"
#include "ui_vp1vertexcontrollerform.h"
#include "ui_vertex_settings_display_form.h"
#include "ui_vertex_settings_interactions_form.h"
#include "ui_vertex_settings_cuts_form.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1CollectionWidget.h"

//#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiKernel/SystemOfUnits.h"

//____________________________________________________________________
class VertexSysController::Imp {
public:
  Ui::VP1VertexControllerForm ui;
  Ui::VertexSysSettingsCutsForm ui_cuts;
  Ui::VertexSysSettingsInteractionsForm ui_int;
  Ui::VertexSysSettingsColouringForm ui_display;
  VP1CollectionWidget * collWidget;

  VP1Interval last_truthCutAllowedEta;
  QList<VP1Interval> last_truthCutAllowedPhi;//All off: empty list. All on: list with one entry: ]-inf,inf[
  bool last_truthCutPrimaryVertexOnly;
  QPair<VertexCommonFlags::QUANTITY,VP1Interval> last_truthCutQuantity;
  QPair<VertexCommonFlags::QUANTITY,double> last_truthCrossLength;
  VertexCommonFlags::ReconVertexTypeFlags last_reconCutAllowedTypes;
  double last_reconVertexRepresentation;
};

//____________________________________________________________________
VertexSysController::VertexSysController(IVP1System * sys)
  : VP1Controller(sys,"VertexSysController"), d(new Imp)
{
  d->ui.setupUi(this);

  d->collWidget = new VP1CollectionWidget;
  setupCollWidgetInScrollArea(d->ui.collWidgetScrollArea,d->collWidget);

  initDialog(d->ui_display, d->ui.pushButton_settings_display);
  initDialog(d->ui_int, d->ui.pushButton_settings_interactions);
  initDialog(d->ui_cuts, d->ui.pushButton_settings_cuts);

  d->ui_cuts.etaPhiCutWidget->setEtaCutEnabled(false);
  d->ui_display.widget_drawOptions->setBaseLightingDisabled();

  defineRadioButtonGroup(d->ui_display.radioButton_truthCrossSize_ProptoQuantity,
			 d->ui_display.radioButton_truthCrossSize_Fixed);
  d->ui_display.radioButton_truthCrossSize_Fixed->setChecked(true);

  defineRadioButtonGroup(d->ui_display.radioButton_truthCrossSize_quantityIsEnergy,
			 d->ui_display.radioButton_truthCrossSize_quantityIsAbsMomentum,
			 d->ui_display.radioButton_truthCrossSize_quantityIsAbsTransverseMomentum,
			 d->ui_display.radioButton_truthCrossSize_quantityIsMass);
  d->ui_display.radioButton_truthCrossSize_quantityIsEnergy->setChecked(true);

  addUpdateSlot(SLOT(possibleChange_truthCutAllowedEta()));
  connectToLastUpdateSlot(d->ui_cuts.etaPhiCutWidget,SIGNAL(allowedEtaChanged(const VP1Interval&)));

  addUpdateSlot(SLOT(possibleChange_truthCutAllowedPhi()));
  connectToLastUpdateSlot(d->ui_cuts.etaPhiCutWidget,SIGNAL(allowedPhiChanged(const QList<VP1Interval>&)));

  addUpdateSlot(SLOT(possibleChange_truthCutPrimaryVertexOnly()));
  connectToLastUpdateSlot(d->ui_cuts.checkBox_truthcut_primaryonly);

  addUpdateSlot(SLOT(possibleChange_truthCutQuantity()));
  connectToLastUpdateSlot(d->ui_cuts.checkBox_truthcut_incomingParticleRequirement);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_truthcut_quantity_min);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_truthcut_quantity_max);
  connectToLastUpdateSlot(d->ui_cuts.doubleSpinBox_truthcut_quantity_min_gev);
  connectToLastUpdateSlot(d->ui_cuts.doubleSpinBox_truthcut_quantity_max_gev);
  connectToLastUpdateSlot(d->ui_cuts.radioButton_truthcut_quantityIsEnergy);
  connectToLastUpdateSlot(d->ui_cuts.radioButton_truthcut_quantityIsAbsMomentum);
  connectToLastUpdateSlot(d->ui_cuts.radioButton_truthcut_quantityIsAbsTransverseMomentum);
  connectToLastUpdateSlot(d->ui_cuts.radioButton_truthcut_quantityIsMass);

  addUpdateSlot(SLOT(possibleChange_truthCrossLength()));
  connectToLastUpdateSlot(d->ui_display.radioButton_truthCrossSize_ProptoQuantity);
  connectToLastUpdateSlot(d->ui_display.doubleSpinBox_truthCrossSize_ProptoQuantity);
  connectToLastUpdateSlot(d->ui_display.radioButton_truthCrossSize_quantityIsEnergy);
  connectToLastUpdateSlot(d->ui_display.radioButton_truthCrossSize_quantityIsAbsMomentum);
  connectToLastUpdateSlot(d->ui_display.radioButton_truthCrossSize_quantityIsAbsTransverseMomentum);
  connectToLastUpdateSlot(d->ui_display.radioButton_truthCrossSize_quantityIsMass);
  connectToLastUpdateSlot(d->ui_display.radioButton_truthCrossSize_Fixed);
  connectToLastUpdateSlot(d->ui_display.doubleSpinBox_truthCrossSize_fixed);

  addUpdateSlot(SLOT(possibleChange_reconCutAllowedTypes()));
  connectToLastUpdateSlot(d->ui_cuts.checkBox_reconcuts_onlyShowTypes);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_reconcut_type_primary);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_reconcut_type_secondary);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_reconcut_type_v0);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_reconcut_type_conversion);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_reconcut_type_kink);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_reconcut_type_pileup);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_reconcut_type_other);

  addUpdateSlot(SLOT(possibleChange_reconVertexRepresentation()));
  connectToLastUpdateSlot(d->ui_display.radioButton_reconVert_point);
  connectToLastUpdateSlot(d->ui_display.radioButton_reconVert_sphere);
  connectToLastUpdateSlot(d->ui_display.doubleSpinBox_reconVert_sphereRadius_mm);
  connectToLastUpdateSlot(d->ui_display.radioButton_reconVert_ellipsoid);
  connectToLastUpdateSlot(d->ui_display.doubleSpinBox_reconVert_ellipsoid_nsigma);
  // Special: -> rerandomise
  connect(d->ui_display.pushButton_colourbyrandom_rerandomise,SIGNAL(clicked()),this,SLOT(emitRerandomise()));

  initLastVars();
}

//____________________________________________________________________
VertexSysController::~VertexSysController()
{
  delete d;
}

//____________________________________________________________________
int VertexSysController::currentSettingsVersion() const
{
  return 0;
}

//____________________________________________________________________
void VertexSysController::actualSaveSettings(VP1Serialise&s) const
{
  //Display:
  s.save(d->ui_display.widget_drawOptions);
  s.save(d->ui_display.radioButton_reconVert_point,
	 d->ui_display.radioButton_reconVert_sphere,
	 d->ui_display.radioButton_reconVert_ellipsoid);
  s.save(d->ui_display.doubleSpinBox_reconVert_sphereRadius_mm);
  s.save(d->ui_display.doubleSpinBox_reconVert_ellipsoid_nsigma);
  s.save(d->ui_display.radioButton_truthCrossSize_ProptoQuantity,
	 d->ui_display.radioButton_truthCrossSize_Fixed);
  s.save(d->ui_display.radioButton_truthCrossSize_quantityIsEnergy,
	 d->ui_display.radioButton_truthCrossSize_quantityIsAbsMomentum,
	 d->ui_display.radioButton_truthCrossSize_quantityIsAbsTransverseMomentum,
	 d->ui_display.radioButton_truthCrossSize_quantityIsMass);
  s.save(d->ui_display.doubleSpinBox_truthCrossSize_fixed);
  s.save(d->ui_display.doubleSpinBox_truthCrossSize_ProptoQuantity);

  //Interactions:
  s.save(d->ui_int.checkBox_printinfo);
  s.save(d->ui_int.checkBox_printinfo_verbose);
  s.save(d->ui_int.checkBox_zoom);

  //Cuts:
  s.save(d->ui_cuts.etaPhiCutWidget);
  s.save(d->ui_cuts.checkBox_truthcut_primaryonly);
  s.save(d->ui_cuts.checkBox_truthcut_incomingParticleRequirement);
  s.save(d->ui_cuts.checkBox_truthcut_quantity_min);
  s.save(d->ui_cuts.checkBox_truthcut_quantity_max);
  s.save(d->ui_cuts.doubleSpinBox_truthcut_quantity_min_gev);
  s.save(d->ui_cuts.doubleSpinBox_truthcut_quantity_max_gev);
  s.save(d->ui_cuts.radioButton_truthcut_quantityIsEnergy,
	 d->ui_cuts.radioButton_truthcut_quantityIsAbsMomentum,
	 d->ui_cuts.radioButton_truthcut_quantityIsAbsTransverseMomentum,
	 d->ui_cuts.radioButton_truthcut_quantityIsMass);
  s.save(d->ui_cuts.checkBox_reconcuts_onlyShowTypes);
  s.save(d->ui_cuts.checkBox_reconcut_type_primary);
  s.save(d->ui_cuts.checkBox_reconcut_type_secondary);
  s.save(d->ui_cuts.checkBox_reconcut_type_v0);
  s.save(d->ui_cuts.checkBox_reconcut_type_conversion);
  s.save(d->ui_cuts.checkBox_reconcut_type_kink);
  s.save(d->ui_cuts.checkBox_reconcut_type_pileup);
  s.save(d->ui_cuts.checkBox_reconcut_type_other);
}

//____________________________________________________________________
void VertexSysController::actualRestoreSettings(VP1Deserialise& s)
{
  if (s.version()<0||s.version()>0) {
    message("Warning: State data in .vp1 file has unsupported version ("+str(s.version())+")");
    return;
  }

  //Display:
  s.restore(d->ui_display.widget_drawOptions);
  s.restore(d->ui_display.radioButton_reconVert_point,
	    d->ui_display.radioButton_reconVert_sphere,
	    d->ui_display.radioButton_reconVert_ellipsoid);
  s.restore(d->ui_display.doubleSpinBox_reconVert_sphereRadius_mm);
  s.restore(d->ui_display.doubleSpinBox_reconVert_ellipsoid_nsigma);
  s.restore(d->ui_display.radioButton_truthCrossSize_ProptoQuantity,
	    d->ui_display.radioButton_truthCrossSize_Fixed);
  s.restore(d->ui_display.radioButton_truthCrossSize_quantityIsEnergy,
	    d->ui_display.radioButton_truthCrossSize_quantityIsAbsMomentum,
	    d->ui_display.radioButton_truthCrossSize_quantityIsAbsTransverseMomentum,
	    d->ui_display.radioButton_truthCrossSize_quantityIsMass);
  s.restore(d->ui_display.doubleSpinBox_truthCrossSize_fixed);
  s.restore(d->ui_display.doubleSpinBox_truthCrossSize_ProptoQuantity);

  //Interactions:
  s.restore(d->ui_int.checkBox_printinfo);
  s.restore(d->ui_int.checkBox_printinfo_verbose);
  s.restore(d->ui_int.checkBox_zoom);

  //Cuts:
  s.restore(d->ui_cuts.etaPhiCutWidget);
  s.restore(d->ui_cuts.checkBox_truthcut_primaryonly);
  s.restore(d->ui_cuts.checkBox_truthcut_incomingParticleRequirement);
  s.restore(d->ui_cuts.checkBox_truthcut_quantity_min);
  s.restore(d->ui_cuts.checkBox_truthcut_quantity_max);
  s.restore(d->ui_cuts.doubleSpinBox_truthcut_quantity_min_gev);
  s.restore(d->ui_cuts.doubleSpinBox_truthcut_quantity_max_gev);
  s.restore(d->ui_cuts.radioButton_truthcut_quantityIsEnergy,
	    d->ui_cuts.radioButton_truthcut_quantityIsAbsMomentum,
	    d->ui_cuts.radioButton_truthcut_quantityIsAbsTransverseMomentum,
	    d->ui_cuts.radioButton_truthcut_quantityIsMass);
  s.restore(d->ui_cuts.checkBox_reconcuts_onlyShowTypes);
  s.restore(d->ui_cuts.checkBox_reconcut_type_primary);
  s.restore(d->ui_cuts.checkBox_reconcut_type_secondary);
  s.restore(d->ui_cuts.checkBox_reconcut_type_v0);
  s.restore(d->ui_cuts.checkBox_reconcut_type_conversion);
  s.restore(d->ui_cuts.checkBox_reconcut_type_kink);
  s.restore(d->ui_cuts.checkBox_reconcut_type_pileup);
  s.restore(d->ui_cuts.checkBox_reconcut_type_other);
}

//____________________________________________________________________
QString VertexSysController::toString( const QPair<VertexCommonFlags::QUANTITY,VP1Interval>& par )
{
  return "<"+VertexCommonFlags::toString(par.first)+", "+str(par.second)+">";
}

//____________________________________________________________________
QString VertexSysController::toString( const QPair<VertexCommonFlags::QUANTITY,double>& par )
{
  return "<"+VertexCommonFlags::toString(par.first)+", "+str(par.second)+">";
}

//____________________________________________________________________
SoGroup * VertexSysController::drawOptions() const
{
  return d->ui_display.widget_drawOptions->drawOptionsGroup();
}

//____________________________________________________________________
VP1CollectionWidget * VertexSysController::collWidget() const { return d->collWidget; }
bool VertexSysController::printInfoOnClick() const { return d->ui_int.checkBox_printinfo->isChecked(); }
bool VertexSysController::printVerboseInfoOnClick() const { return d->ui_int.checkBox_printinfo_verbose->isChecked(); }
bool VertexSysController::zoomOnClick() const { return d->ui_int.checkBox_zoom->isChecked(); }
VP1Interval VertexSysController::truthCutAllowedEta() const { return d->ui_cuts.etaPhiCutWidget->allowedEta(); }
QList<VP1Interval> VertexSysController::truthCutAllowedPhi() const { return d->ui_cuts.etaPhiCutWidget->allowedPhi(); }
bool VertexSysController::truthCutPrimaryVertexOnly() const { return d->ui_cuts.checkBox_truthcut_primaryonly->isChecked(); }

//____________________________________________________________________
QPair<VertexCommonFlags::QUANTITY,double> VertexSysController::truthCrossLength() const
{
  VertexCommonFlags::QUANTITY quantity(VertexCommonFlags::ENERGY);
  double range(0);
  if (d->ui_display.radioButton_truthCrossSize_Fixed->isChecked()) {
    range = - d->ui_display.doubleSpinBox_truthCrossSize_fixed->value()*Gaudi::Units::mm;
  } else {
    range = d->ui_display.doubleSpinBox_truthCrossSize_ProptoQuantity->value()*(Gaudi::Units::mm/Gaudi::Units::GeV);
    if (d->ui_display.radioButton_truthCrossSize_quantityIsAbsMomentum->isChecked())
      quantity = VertexCommonFlags::MOMENTUM;
    else if (d->ui_display.radioButton_truthCrossSize_quantityIsAbsTransverseMomentum->isChecked())
      quantity = VertexCommonFlags::TRANSVERSE_MOM;
    else if (d->ui_display.radioButton_truthCrossSize_quantityIsMass->isChecked())
      quantity = VertexCommonFlags::MASS;
  }
  return QPair<VertexCommonFlags::QUANTITY,double>(quantity,range);
}

//____________________________________________________________________
QPair<VertexCommonFlags::QUANTITY,VP1Interval> VertexSysController::truthCutQuantity() const
{
  VP1Interval range(-VP1Interval::inf(),VP1Interval::inf());
  VertexCommonFlags::QUANTITY quantity(VertexCommonFlags::ENERGY);

  if (!d->ui_cuts.checkBox_truthcut_incomingParticleRequirement->isChecked())
    return QPair<VertexCommonFlags::QUANTITY,VP1Interval>(quantity,range);

  if (d->ui_cuts.checkBox_truthcut_quantity_min->isChecked())
    range.setLower(d->ui_cuts.doubleSpinBox_truthcut_quantity_min_gev->value()*Gaudi::Units::GeV);
  if (d->ui_cuts.checkBox_truthcut_quantity_max->isChecked())
    range.setUpper(d->ui_cuts.doubleSpinBox_truthcut_quantity_max_gev->value()*Gaudi::Units::GeV);

  if (range.isAllR())
    return QPair<VertexCommonFlags::QUANTITY,VP1Interval>(quantity,range);

  if (d->ui_cuts.radioButton_truthcut_quantityIsAbsMomentum->isChecked())
    quantity = VertexCommonFlags::MOMENTUM;
  else if (d->ui_cuts.radioButton_truthcut_quantityIsAbsTransverseMomentum->isChecked())
    quantity = VertexCommonFlags::TRANSVERSE_MOM;
  else if (d->ui_cuts.radioButton_truthcut_quantityIsMass->isChecked())
    quantity = VertexCommonFlags::MASS;

  return QPair<VertexCommonFlags::QUANTITY,VP1Interval>(quantity,range);
}

//____________________________________________________________________
double VertexSysController::reconVertexRepresentation() const
{
  if (d->ui_display.radioButton_reconVert_point->isChecked())
    return 0.0;
  if (d->ui_display.radioButton_reconVert_sphere->isChecked())
    return - d->ui_display.doubleSpinBox_reconVert_sphereRadius_mm->value()*Gaudi::Units::mm;
  return d->ui_display.doubleSpinBox_reconVert_ellipsoid_nsigma->value();
}

//____________________________________________________________________
VertexCommonFlags::ReconVertexTypeFlags VertexSysController::reconCutAllowedTypes() const
{
  if (!d->ui_cuts.checkBox_reconcuts_onlyShowTypes->isChecked())
    return VertexCommonFlags::RVT_Any;

  VertexCommonFlags::ReconVertexTypeFlags f;

  if (d->ui_cuts.checkBox_reconcut_type_primary->isChecked())
    f |= VertexCommonFlags::RVT_Primary;
  if (d->ui_cuts.checkBox_reconcut_type_secondary->isChecked())
    f |= VertexCommonFlags::RVT_Secondary;
  if (d->ui_cuts.checkBox_reconcut_type_v0->isChecked())
    f |= VertexCommonFlags::RVT_V0;
  if (d->ui_cuts.checkBox_reconcut_type_conversion->isChecked())
    f |= VertexCommonFlags::RVT_Conversion;
  if (d->ui_cuts.checkBox_reconcut_type_kink->isChecked())
    f |= VertexCommonFlags::RVT_Kink;
  if (d->ui_cuts.checkBox_reconcut_type_pileup->isChecked())
    f |= VertexCommonFlags::RVT_Pileup;
  if (d->ui_cuts.checkBox_reconcut_type_other->isChecked())
    f |= VertexCommonFlags::RVT_Other;
  return f;
}

//____________________________________________________________________
void VertexSysController::emitRerandomise()
{
  messageVerbose("Emitting rerandomise");
  emit rerandomise();
}

///////////////////////////////////////////////////////////////////////////
// Test for possible changes in values and emit signals as appropriate:
// (possibleChange_XXX() slots code provided by macros)
#define VP1CONTROLLERCLASSNAME VertexSysController
#include "VP1Base/VP1ControllerMacros.h"
POSSIBLECHANGE_IMP(truthCutAllowedEta)
POSSIBLECHANGE_IMP(truthCutAllowedPhi)
POSSIBLECHANGE_IMP(truthCutPrimaryVertexOnly)
POSSIBLECHANGE_IMP(truthCutQuantity)
POSSIBLECHANGE_IMP(reconCutAllowedTypes)
POSSIBLECHANGE_IMP(reconVertexRepresentation)
POSSIBLECHANGE_IMP(truthCrossLength)
