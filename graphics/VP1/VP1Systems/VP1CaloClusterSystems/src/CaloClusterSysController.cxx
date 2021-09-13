/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class CaloClusterSysController          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#define VP1IMPVARNAME m_d

#include "VP1CaloClusterSystems/CaloClusterSysController.h"
#include "VP1CaloClusterSystems/VP1CaloClusterCollection.h"
#include "ui_vp1caloclustercontrollerform.h"
#include "ui_calocluster_settings_display_form.h"
#include "ui_calocluster_settings_interactions_form.h"
#include "ui_calocluster_settings_cuts_form.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1CollectionWidget.h"
#include <cmath>
#include "CLHEP/Units/SystemOfUnits.h"

//____________________________________________________________________
class CaloClusterSysController::Imp {
public:
  Ui::VP1CaloClusterControllerForm ui{};
  Ui::CaloClusterSysSettingsCutsForm ui_cuts{};
  Ui::CaloClusterSysSettingsInteractionsForm ui_int{};
  Ui::CaloClusterSysSettingsDisplayForm ui_display{};
  VP1CollectionWidget * collWidget = nullptr;

  double gui_mostEnergetic = 0.0;

  QPair<bool,double> last_scale;
  VP1Interval last_cutAllowedEnergies;
  VP1Interval last_cutAllowedEta;
  QList<VP1Interval> last_cutAllowedPhi;//All off: empty list. All on: list with one entry: ]-inf,inf[
  bool last_showVolumeOutLines = false;
  bool last_useTransverseEnergies = false;
};

//____________________________________________________________________
QString CaloClusterSysController::toString( const QPair<bool,double>& par )
{
  return "<"+QString(par.first?"log":"linear")+", "+QString::number(par.second/(CLHEP::cm/CLHEP::GeV))+" cm/GeV>";
}

//____________________________________________________________________
CaloClusterSysController::CaloClusterSysController(IVP1System * sys)
  : VP1Controller(sys,"CaloClusterSysController"), m_d(new Imp)
{
  m_d->ui.setupUi(this);
  m_d->gui_mostEnergetic = 0;

  m_d->collWidget = new VP1CollectionWidget;
  setupCollWidgetInScrollArea(m_d->ui.collWidgetScrollArea,m_d->collWidget);

  initDialog(m_d->ui_display, m_d->ui.pushButton_settings_display);
  initDialog(m_d->ui_int, m_d->ui.pushButton_settings_interactions);
  initDialog(m_d->ui_cuts, m_d->ui.pushButton_settings_cuts);

  m_d->ui_display.widget_drawOptions->setLineWidths(2.0);
  m_d->ui_display.widget_drawOptions->setPointSizesDisabled();
  m_d->ui_display.widget_drawOptions->setBaseLightingDisabled();
  m_d->ui_display.widget_drawOptions->setComplexityDisabled();

  m_d->ui_cuts.etaPhiCutWidget->setEtaCutEnabled(false);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  Setup connections which monitor changes in the controller so that we may emit signals as appropriate:  //
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // -> Eta cut presets:
  connect(m_d->ui_cuts.toolButton_quicketa_barrel,SIGNAL(clicked()),this,SLOT(etaCutPresetButtonTriggered()));
  connect(m_d->ui_cuts.toolButton_quicketa_endcapA,SIGNAL(clicked()),this,SLOT(etaCutPresetButtonTriggered()));
  connect(m_d->ui_cuts.toolButton_quicketa_endcapC,SIGNAL(clicked()),this,SLOT(etaCutPresetButtonTriggered()));

  // -> scale:
  addUpdateSlot(SLOT(possibleChange_scale()));
  connectToLastUpdateSlot(m_d->ui_display.checkBox_logscale);
  connectToLastUpdateSlot(m_d->ui_display.radioButton_relativeScale);
  connectToLastUpdateSlot(m_d->ui_display.doubleSpinBox_lengthOfMostEnergetic);
  connectToLastUpdateSlot(m_d->ui_display.doubleSpinBox_lengthOf10GeV);

  // -> cutAllowedEnergies:
  addUpdateSlot(SLOT(possibleChange_cutAllowedEnergies()));
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_Emin);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_Emax);
  connectToLastUpdateSlot(m_d->ui_cuts.doubleSpinBox_Emin);
  connectToLastUpdateSlot(m_d->ui_cuts.doubleSpinBox_Emax);

  // -> cutAllowedEta:
  addUpdateSlot(SLOT(possibleChange_cutAllowedEta()));
  connectToLastUpdateSlot(m_d->ui_cuts.etaPhiCutWidget,SIGNAL(allowedEtaChanged(const VP1Interval&)));

  // -> cutAllowedPhi:
  addUpdateSlot(SLOT(possibleChange_cutAllowedPhi()));
  connectToLastUpdateSlot(m_d->ui_cuts.etaPhiCutWidget,SIGNAL(allowedPhiChanged(const QList<VP1Interval>&)));

  // -> showVolumeOutLines
  addUpdateSlot(SLOT(possibleChange_showVolumeOutLines()));
  connectToLastUpdateSlot(m_d->ui_display.checkBox_showVolumeOutLines);

  // -> useTransverseEnergies
  addUpdateSlot(SLOT(possibleChange_useTransverseEnergies()));
  connectToLastUpdateSlot(m_d->ui.radioButton_energyMode_Et);
  connectToLastUpdateSlot(m_d->ui.radioButton_energyMode_E);

  initLastVars();
}

//____________________________________________________________________
CaloClusterSysController::~CaloClusterSysController()
{
  delete m_d;
}


//____________________________________________________________________
int CaloClusterSysController::currentSettingsVersion() const
{
  return 3;
}

//____________________________________________________________________
void CaloClusterSysController::actualSaveSettings(VP1Serialise&s) const
{
  //version 0 had an integer here
  s.save(m_d->ui_display.checkBox_logscale);
  s.save(m_d->ui_display.radioButton_relativeScale,m_d->ui_display.radioButton_absoluteScale);
  s.save(m_d->ui_display.doubleSpinBox_lengthOfMostEnergetic);
  s.save(m_d->ui_display.doubleSpinBox_lengthOf10GeV);
  s.save(m_d->ui_cuts.checkBox_Emin);
  s.save(m_d->ui_cuts.doubleSpinBox_Emin);
  s.save(m_d->ui_cuts.checkBox_Emax);
  s.save(m_d->ui_cuts.doubleSpinBox_Emax);
  s.save(m_d->ui_cuts.etaPhiCutWidget);
  s.save(m_d->ui_int.checkBox_printinfo);//Version 1+
  s.save(m_d->ui_int.checkBox_printinfo_verbose);//Version 1+
  s.save(m_d->ui_int.checkBox_zoom);//Version 1+
  s.save(m_d->ui_display.checkBox_showVolumeOutLines);//Version 2+
  s.save(m_d->ui_display.widget_drawOptions);//Version 2+
  s.save(m_d->ui.radioButton_energyMode_Et,m_d->ui.radioButton_energyMode_E);//Version 3+
}

//____________________________________________________________________
void CaloClusterSysController::actualRestoreSettings(VP1Deserialise& s)
{
  if (s.version()<0||s.version()>3) {
    message("Warning: State data in .vp1 file has unsupported version ("+str(s.version())+")");
    return;
  }
  if (s.version()==0)
    s.ignoreInt();
  s.restore(m_d->ui_display.checkBox_logscale);
  s.restore(m_d->ui_display.radioButton_relativeScale,m_d->ui_display.radioButton_absoluteScale);
  s.restore(m_d->ui_display.doubleSpinBox_lengthOfMostEnergetic);
  s.restore(m_d->ui_display.doubleSpinBox_lengthOf10GeV);
  s.restore(m_d->ui_cuts.checkBox_Emin);
  s.restore(m_d->ui_cuts.doubleSpinBox_Emin);
  s.restore(m_d->ui_cuts.checkBox_Emax);
  s.restore(m_d->ui_cuts.doubleSpinBox_Emax);
  s.restore(m_d->ui_cuts.etaPhiCutWidget);
  if (s.version()>=1) {
    s.restore(m_d->ui_int.checkBox_printinfo);
    s.restore(m_d->ui_int.checkBox_printinfo_verbose);
    s.restore(m_d->ui_int.checkBox_zoom);
  }
  if (s.version()>=2) {
    s.restore(m_d->ui_display.checkBox_showVolumeOutLines);
    s.restore(m_d->ui_display.widget_drawOptions);
  }
  if (s.version()>=3)
    s.restore(m_d->ui.radioButton_energyMode_Et,m_d->ui.radioButton_energyMode_E);
}

//____________________________________________________________________
VP1CollectionWidget * CaloClusterSysController::collWidget() const
{
  return m_d->collWidget;
}


//____________________________________________________________________
void CaloClusterSysController::etaCutPresetButtonTriggered()
{
  const double cracketa = VP1CaloClusterCollection::crackEta();
  bool save = m_d->ui_cuts.etaPhiCutWidget->blockSignals(true);

  if (sender()==m_d->ui_cuts.toolButton_quicketa_barrel)
    m_d->ui_cuts.etaPhiCutWidget->setEtaCut(cracketa);
  else if (sender()==m_d->ui_cuts.toolButton_quicketa_endcapA)
    m_d->ui_cuts.etaPhiCutWidget->setEtaCut(cracketa,9.0);
  else if (sender()==m_d->ui_cuts.toolButton_quicketa_endcapC)
    m_d->ui_cuts.etaPhiCutWidget->setEtaCut(-9.0,-cracketa);
  else
    message("ERROR: Unknown sender() in etaCutPresetButtonTriggered() slot.");

  m_d->ui_cuts.etaPhiCutWidget->blockSignals(save);
  possibleChange_cutAllowedEta();
}

//____________________________________________________________________
SoGroup * CaloClusterSysController::drawOptions() const
{
  return m_d->ui_display.widget_drawOptions->drawOptionsGroup();
}

//____________________________________________________________________
bool CaloClusterSysController::printInfoOnClick() const { return m_d->ui_int.checkBox_printinfo->isChecked(); }
bool CaloClusterSysController::printVerboseInfoOnClick() const { return m_d->ui_int.checkBox_printinfo_verbose->isChecked(); }
bool CaloClusterSysController::zoomOnClick() const { return m_d->ui_int.checkBox_zoom->isChecked(); }

//____________________________________________________________________
QPair<bool,double> CaloClusterSysController::scale() const
{
  const bool relative = m_d->ui_display.radioButton_relativeScale->isChecked();
  const bool logscale = m_d->ui_display.checkBox_logscale->isChecked();

  double highestvisibleenergy=0*CLHEP::eV;
  foreach(VP1StdCollection* stdcol, m_d->collWidget->visibleStdCollections()) {
    VP1CaloClusterCollection* col = dynamic_cast<VP1CaloClusterCollection*>(stdcol);
    if ( col && highestvisibleenergy < col->highestVisibleClusterEnergy() )
      highestvisibleenergy = col->highestVisibleClusterEnergy();
  }
  if (m_d->gui_mostEnergetic!=highestvisibleenergy) {
    m_d->gui_mostEnergetic=highestvisibleenergy;
    m_d->ui_display.label_current_most_energetic->setText("Current value: "+QString::number(m_d->gui_mostEnergetic/CLHEP::GeV,'f',2)+" GeV");
  }

  const double length = (relative ? m_d->ui_display.doubleSpinBox_lengthOfMostEnergetic->value()*CLHEP::m
			 : m_d->ui_display.doubleSpinBox_lengthOf10GeV->value()*CLHEP::m );
  const double energy = relative ? highestvisibleenergy : 10*CLHEP::GeV;
  const double minscale = 1*CLHEP::mm/(1*CLHEP::GeV);
  const double maxscale = 1*CLHEP::m/(1*CLHEP::MeV);
  double scl;
  if (energy<=0)
    scl = maxscale;
  else
    scl = logscale ? length/log(1+fabs(energy)) :length/energy;
  if (scl!=scl||scl>maxscale)
    scl = maxscale;
  if (scl<minscale)
    scl = minscale;
  return QPair<bool,double>(logscale,scl);
}

//____________________________________________________________________
VP1Interval CaloClusterSysController::cutAllowedEnergies() const
{
  const double min = (m_d->ui_cuts.checkBox_Emin->isChecked() ? m_d->ui_cuts.doubleSpinBox_Emin->value()*CLHEP::GeV : -std::numeric_limits<double>::infinity());
  const double max = (m_d->ui_cuts.checkBox_Emax->isChecked() ? m_d->ui_cuts.doubleSpinBox_Emax->value()*CLHEP::GeV : std::numeric_limits<double>::infinity());
  if (max<min)
    return VP1Interval();
  return VP1Interval( min, max );
}

//____________________________________________________________________
VP1Interval CaloClusterSysController::cutAllowedEta() const
{
  return m_d->ui_cuts.etaPhiCutWidget->allowedEta();
}

//____________________________________________________________________
QList<VP1Interval> CaloClusterSysController::cutAllowedPhi() const
{
  return m_d->ui_cuts.etaPhiCutWidget->allowedPhi();
}

//____________________________________________________________________
bool CaloClusterSysController::showVolumeOutLines() const
{
  return m_d->ui_display.checkBox_showVolumeOutLines->isChecked();
}

bool CaloClusterSysController::useTransverseEnergies() const
{
  return m_d->ui.radioButton_energyMode_Et->isChecked();
}

///////////////////////////////////////////////////////////////////////////
// Test for possible changes in values and emit signals as appropriate:
// (possibleChange_XXX() slots code provided by macros)
#define VP1CONTROLLERCLASSNAME CaloClusterSysController
#include "VP1Base/VP1ControllerMacros.h"
POSSIBLECHANGE_IMP(cutAllowedEnergies)
POSSIBLECHANGE_IMP(cutAllowedEta)
POSSIBLECHANGE_IMP(cutAllowedPhi)
POSSIBLECHANGE_IMP(scale)
POSSIBLECHANGE_IMP(showVolumeOutLines)
POSSIBLECHANGE_IMP(useTransverseEnergies)
