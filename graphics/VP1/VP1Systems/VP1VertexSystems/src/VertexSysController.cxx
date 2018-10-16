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

#define VP1IMPVARNAME m_d

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
  : VP1Controller(sys,"VertexSysController"), m_d(new Imp)
{
  m_d->ui.setupUi(this);

  m_d->collWidget = new VP1CollectionWidget;
  setupCollWidgetInScrollArea(m_d->ui.collWidgetScrollArea,m_d->collWidget);

  initDialog(m_d->ui_display, m_d->ui.pushButton_settings_display);
  initDialog(m_d->ui_int, m_d->ui.pushButton_settings_interactions);
  initDialog(m_d->ui_cuts, m_d->ui.pushButton_settings_cuts);

  m_d->ui_cuts.etaPhiCutWidget->setEtaCutEnabled(false);
  m_d->ui_display.widget_drawOptions->setBaseLightingDisabled();

  defineRadioButtonGroup(m_d->ui_display.radioButton_truthCrossSize_ProptoQuantity,
			 m_d->ui_display.radioButton_truthCrossSize_Fixed);
  m_d->ui_display.radioButton_truthCrossSize_Fixed->setChecked(true);

  defineRadioButtonGroup(m_d->ui_display.radioButton_truthCrossSize_quantityIsEnergy,
			 m_d->ui_display.radioButton_truthCrossSize_quantityIsAbsMomentum,
			 m_d->ui_display.radioButton_truthCrossSize_quantityIsAbsTransverseMomentum,
			 m_d->ui_display.radioButton_truthCrossSize_quantityIsMass);
  m_d->ui_display.radioButton_truthCrossSize_quantityIsEnergy->setChecked(true);

  addUpdateSlot(SLOT(possibleChange_truthCutAllowedEta()));
  connectToLastUpdateSlot(m_d->ui_cuts.etaPhiCutWidget,SIGNAL(allowedEtaChanged(const VP1Interval&)));

  addUpdateSlot(SLOT(possibleChange_truthCutAllowedPhi()));
  connectToLastUpdateSlot(m_d->ui_cuts.etaPhiCutWidget,SIGNAL(allowedPhiChanged(const QList<VP1Interval>&)));

  addUpdateSlot(SLOT(possibleChange_truthCutPrimaryVertexOnly()));
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_truthcut_primaryonly);

  addUpdateSlot(SLOT(possibleChange_truthCutQuantity()));
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_truthcut_incomingParticleRequirement);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_truthcut_quantity_min);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_truthcut_quantity_max);
  connectToLastUpdateSlot(m_d->ui_cuts.doubleSpinBox_truthcut_quantity_min_gev);
  connectToLastUpdateSlot(m_d->ui_cuts.doubleSpinBox_truthcut_quantity_max_gev);
  connectToLastUpdateSlot(m_d->ui_cuts.radioButton_truthcut_quantityIsEnergy);
  connectToLastUpdateSlot(m_d->ui_cuts.radioButton_truthcut_quantityIsAbsMomentum);
  connectToLastUpdateSlot(m_d->ui_cuts.radioButton_truthcut_quantityIsAbsTransverseMomentum);
  connectToLastUpdateSlot(m_d->ui_cuts.radioButton_truthcut_quantityIsMass);

  addUpdateSlot(SLOT(possibleChange_truthCrossLength()));
  connectToLastUpdateSlot(m_d->ui_display.radioButton_truthCrossSize_ProptoQuantity);
  connectToLastUpdateSlot(m_d->ui_display.doubleSpinBox_truthCrossSize_ProptoQuantity);
  connectToLastUpdateSlot(m_d->ui_display.radioButton_truthCrossSize_quantityIsEnergy);
  connectToLastUpdateSlot(m_d->ui_display.radioButton_truthCrossSize_quantityIsAbsMomentum);
  connectToLastUpdateSlot(m_d->ui_display.radioButton_truthCrossSize_quantityIsAbsTransverseMomentum);
  connectToLastUpdateSlot(m_d->ui_display.radioButton_truthCrossSize_quantityIsMass);
  connectToLastUpdateSlot(m_d->ui_display.radioButton_truthCrossSize_Fixed);
  connectToLastUpdateSlot(m_d->ui_display.doubleSpinBox_truthCrossSize_fixed);

  addUpdateSlot(SLOT(possibleChange_reconCutAllowedTypes()));
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_reconcuts_onlyShowTypes);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_reconcut_type_primary);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_reconcut_type_secondary);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_reconcut_type_v0);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_reconcut_type_conversion);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_reconcut_type_kink);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_reconcut_type_pileup);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_reconcut_type_other);

  addUpdateSlot(SLOT(possibleChange_reconVertexRepresentation()));
  connectToLastUpdateSlot(m_d->ui_display.radioButton_reconVert_point);
  connectToLastUpdateSlot(m_d->ui_display.radioButton_reconVert_sphere);
  connectToLastUpdateSlot(m_d->ui_display.doubleSpinBox_reconVert_sphereRadius_mm);
  connectToLastUpdateSlot(m_d->ui_display.radioButton_reconVert_ellipsoid);
  connectToLastUpdateSlot(m_d->ui_display.doubleSpinBox_reconVert_ellipsoid_nsigma);
  // Special: -> rerandomise
  connect(m_d->ui_display.pushButton_colourbyrandom_rerandomise,SIGNAL(clicked()),this,SLOT(emitRerandomise()));

  initLastVars();
}

//____________________________________________________________________
VertexSysController::~VertexSysController()
{
  delete m_d;
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
  s.save(m_d->ui_display.widget_drawOptions);
  s.save(m_d->ui_display.radioButton_reconVert_point,
	 m_d->ui_display.radioButton_reconVert_sphere,
	 m_d->ui_display.radioButton_reconVert_ellipsoid);
  s.save(m_d->ui_display.doubleSpinBox_reconVert_sphereRadius_mm);
  s.save(m_d->ui_display.doubleSpinBox_reconVert_ellipsoid_nsigma);
  s.save(m_d->ui_display.radioButton_truthCrossSize_ProptoQuantity,
	 m_d->ui_display.radioButton_truthCrossSize_Fixed);
  s.save(m_d->ui_display.radioButton_truthCrossSize_quantityIsEnergy,
	 m_d->ui_display.radioButton_truthCrossSize_quantityIsAbsMomentum,
	 m_d->ui_display.radioButton_truthCrossSize_quantityIsAbsTransverseMomentum,
	 m_d->ui_display.radioButton_truthCrossSize_quantityIsMass);
  s.save(m_d->ui_display.doubleSpinBox_truthCrossSize_fixed);
  s.save(m_d->ui_display.doubleSpinBox_truthCrossSize_ProptoQuantity);

  //Interactions:
  s.save(m_d->ui_int.checkBox_printinfo);
  s.save(m_d->ui_int.checkBox_printinfo_verbose);
  s.save(m_d->ui_int.checkBox_zoom);

  //Cuts:
  s.save(m_d->ui_cuts.etaPhiCutWidget);
  s.save(m_d->ui_cuts.checkBox_truthcut_primaryonly);
  s.save(m_d->ui_cuts.checkBox_truthcut_incomingParticleRequirement);
  s.save(m_d->ui_cuts.checkBox_truthcut_quantity_min);
  s.save(m_d->ui_cuts.checkBox_truthcut_quantity_max);
  s.save(m_d->ui_cuts.doubleSpinBox_truthcut_quantity_min_gev);
  s.save(m_d->ui_cuts.doubleSpinBox_truthcut_quantity_max_gev);
  s.save(m_d->ui_cuts.radioButton_truthcut_quantityIsEnergy,
	 m_d->ui_cuts.radioButton_truthcut_quantityIsAbsMomentum,
	 m_d->ui_cuts.radioButton_truthcut_quantityIsAbsTransverseMomentum,
	 m_d->ui_cuts.radioButton_truthcut_quantityIsMass);
  s.save(m_d->ui_cuts.checkBox_reconcuts_onlyShowTypes);
  s.save(m_d->ui_cuts.checkBox_reconcut_type_primary);
  s.save(m_d->ui_cuts.checkBox_reconcut_type_secondary);
  s.save(m_d->ui_cuts.checkBox_reconcut_type_v0);
  s.save(m_d->ui_cuts.checkBox_reconcut_type_conversion);
  s.save(m_d->ui_cuts.checkBox_reconcut_type_kink);
  s.save(m_d->ui_cuts.checkBox_reconcut_type_pileup);
  s.save(m_d->ui_cuts.checkBox_reconcut_type_other);
}

//____________________________________________________________________
void VertexSysController::actualRestoreSettings(VP1Deserialise& s)
{
  if (s.version()<0||s.version()>0) {
    message("Warning: State data in .vp1 file has unsupported version ("+str(s.version())+")");
    return;
  }

  //Display:
  s.restore(m_d->ui_display.widget_drawOptions);
  s.restore(m_d->ui_display.radioButton_reconVert_point,
	    m_d->ui_display.radioButton_reconVert_sphere,
	    m_d->ui_display.radioButton_reconVert_ellipsoid);
  s.restore(m_d->ui_display.doubleSpinBox_reconVert_sphereRadius_mm);
  s.restore(m_d->ui_display.doubleSpinBox_reconVert_ellipsoid_nsigma);
  s.restore(m_d->ui_display.radioButton_truthCrossSize_ProptoQuantity,
	    m_d->ui_display.radioButton_truthCrossSize_Fixed);
  s.restore(m_d->ui_display.radioButton_truthCrossSize_quantityIsEnergy,
	    m_d->ui_display.radioButton_truthCrossSize_quantityIsAbsMomentum,
	    m_d->ui_display.radioButton_truthCrossSize_quantityIsAbsTransverseMomentum,
	    m_d->ui_display.radioButton_truthCrossSize_quantityIsMass);
  s.restore(m_d->ui_display.doubleSpinBox_truthCrossSize_fixed);
  s.restore(m_d->ui_display.doubleSpinBox_truthCrossSize_ProptoQuantity);

  //Interactions:
  s.restore(m_d->ui_int.checkBox_printinfo);
  s.restore(m_d->ui_int.checkBox_printinfo_verbose);
  s.restore(m_d->ui_int.checkBox_zoom);

  //Cuts:
  s.restore(m_d->ui_cuts.etaPhiCutWidget);
  s.restore(m_d->ui_cuts.checkBox_truthcut_primaryonly);
  s.restore(m_d->ui_cuts.checkBox_truthcut_incomingParticleRequirement);
  s.restore(m_d->ui_cuts.checkBox_truthcut_quantity_min);
  s.restore(m_d->ui_cuts.checkBox_truthcut_quantity_max);
  s.restore(m_d->ui_cuts.doubleSpinBox_truthcut_quantity_min_gev);
  s.restore(m_d->ui_cuts.doubleSpinBox_truthcut_quantity_max_gev);
  s.restore(m_d->ui_cuts.radioButton_truthcut_quantityIsEnergy,
	    m_d->ui_cuts.radioButton_truthcut_quantityIsAbsMomentum,
	    m_d->ui_cuts.radioButton_truthcut_quantityIsAbsTransverseMomentum,
	    m_d->ui_cuts.radioButton_truthcut_quantityIsMass);
  s.restore(m_d->ui_cuts.checkBox_reconcuts_onlyShowTypes);
  s.restore(m_d->ui_cuts.checkBox_reconcut_type_primary);
  s.restore(m_d->ui_cuts.checkBox_reconcut_type_secondary);
  s.restore(m_d->ui_cuts.checkBox_reconcut_type_v0);
  s.restore(m_d->ui_cuts.checkBox_reconcut_type_conversion);
  s.restore(m_d->ui_cuts.checkBox_reconcut_type_kink);
  s.restore(m_d->ui_cuts.checkBox_reconcut_type_pileup);
  s.restore(m_d->ui_cuts.checkBox_reconcut_type_other);
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
  return m_d->ui_display.widget_drawOptions->drawOptionsGroup();
}

//____________________________________________________________________
VP1CollectionWidget * VertexSysController::collWidget() const { return m_d->collWidget; }
bool VertexSysController::printInfoOnClick() const { return m_d->ui_int.checkBox_printinfo->isChecked(); }
bool VertexSysController::printVerboseInfoOnClick() const { return m_d->ui_int.checkBox_printinfo_verbose->isChecked(); }
bool VertexSysController::zoomOnClick() const { return m_d->ui_int.checkBox_zoom->isChecked(); }
VP1Interval VertexSysController::truthCutAllowedEta() const { return m_d->ui_cuts.etaPhiCutWidget->allowedEta(); }
QList<VP1Interval> VertexSysController::truthCutAllowedPhi() const { return m_d->ui_cuts.etaPhiCutWidget->allowedPhi(); }
bool VertexSysController::truthCutPrimaryVertexOnly() const { return m_d->ui_cuts.checkBox_truthcut_primaryonly->isChecked(); }

//____________________________________________________________________
QPair<VertexCommonFlags::QUANTITY,double> VertexSysController::truthCrossLength() const
{
  VertexCommonFlags::QUANTITY quantity(VertexCommonFlags::ENERGY);
  double range(0);
  if (m_d->ui_display.radioButton_truthCrossSize_Fixed->isChecked()) {
    range = - m_d->ui_display.doubleSpinBox_truthCrossSize_fixed->value()*Gaudi::Units::mm;
  } else {
    range = m_d->ui_display.doubleSpinBox_truthCrossSize_ProptoQuantity->value()*(Gaudi::Units::mm/Gaudi::Units::GeV);
    if (m_d->ui_display.radioButton_truthCrossSize_quantityIsAbsMomentum->isChecked())
      quantity = VertexCommonFlags::MOMENTUM;
    else if (m_d->ui_display.radioButton_truthCrossSize_quantityIsAbsTransverseMomentum->isChecked())
      quantity = VertexCommonFlags::TRANSVERSE_MOM;
    else if (m_d->ui_display.radioButton_truthCrossSize_quantityIsMass->isChecked())
      quantity = VertexCommonFlags::MASS;
  }
  return QPair<VertexCommonFlags::QUANTITY,double>(quantity,range);
}

//____________________________________________________________________
QPair<VertexCommonFlags::QUANTITY,VP1Interval> VertexSysController::truthCutQuantity() const
{
  VP1Interval range(-VP1Interval::inf(),VP1Interval::inf());
  VertexCommonFlags::QUANTITY quantity(VertexCommonFlags::ENERGY);

  if (!m_d->ui_cuts.checkBox_truthcut_incomingParticleRequirement->isChecked())
    return QPair<VertexCommonFlags::QUANTITY,VP1Interval>(quantity,range);

  if (m_d->ui_cuts.checkBox_truthcut_quantity_min->isChecked())
    range.setLower(m_d->ui_cuts.doubleSpinBox_truthcut_quantity_min_gev->value()*Gaudi::Units::GeV);
  if (m_d->ui_cuts.checkBox_truthcut_quantity_max->isChecked())
    range.setUpper(m_d->ui_cuts.doubleSpinBox_truthcut_quantity_max_gev->value()*Gaudi::Units::GeV);

  if (range.isAllR())
    return QPair<VertexCommonFlags::QUANTITY,VP1Interval>(quantity,range);

  if (m_d->ui_cuts.radioButton_truthcut_quantityIsAbsMomentum->isChecked())
    quantity = VertexCommonFlags::MOMENTUM;
  else if (m_d->ui_cuts.radioButton_truthcut_quantityIsAbsTransverseMomentum->isChecked())
    quantity = VertexCommonFlags::TRANSVERSE_MOM;
  else if (m_d->ui_cuts.radioButton_truthcut_quantityIsMass->isChecked())
    quantity = VertexCommonFlags::MASS;

  return QPair<VertexCommonFlags::QUANTITY,VP1Interval>(quantity,range);
}

//____________________________________________________________________
double VertexSysController::reconVertexRepresentation() const
{
  if (m_d->ui_display.radioButton_reconVert_point->isChecked())
    return 0.0;
  if (m_d->ui_display.radioButton_reconVert_sphere->isChecked())
    return - m_d->ui_display.doubleSpinBox_reconVert_sphereRadius_mm->value()*Gaudi::Units::mm;
  return m_d->ui_display.doubleSpinBox_reconVert_ellipsoid_nsigma->value();
}

//____________________________________________________________________
VertexCommonFlags::ReconVertexTypeFlags VertexSysController::reconCutAllowedTypes() const
{
  if (!m_d->ui_cuts.checkBox_reconcuts_onlyShowTypes->isChecked())
    return VertexCommonFlags::RVT_Any;

  VertexCommonFlags::ReconVertexTypeFlags f;

  if (m_d->ui_cuts.checkBox_reconcut_type_primary->isChecked())
    f |= VertexCommonFlags::RVT_Primary;
  if (m_d->ui_cuts.checkBox_reconcut_type_secondary->isChecked())
    f |= VertexCommonFlags::RVT_Secondary;
  if (m_d->ui_cuts.checkBox_reconcut_type_v0->isChecked())
    f |= VertexCommonFlags::RVT_V0;
  if (m_d->ui_cuts.checkBox_reconcut_type_conversion->isChecked())
    f |= VertexCommonFlags::RVT_Conversion;
  if (m_d->ui_cuts.checkBox_reconcut_type_kink->isChecked())
    f |= VertexCommonFlags::RVT_Kink;
  if (m_d->ui_cuts.checkBox_reconcut_type_pileup->isChecked())
    f |= VertexCommonFlags::RVT_Pileup;
  if (m_d->ui_cuts.checkBox_reconcut_type_other->isChecked())
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
