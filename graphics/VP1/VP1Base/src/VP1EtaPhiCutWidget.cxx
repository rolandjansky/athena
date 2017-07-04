/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1EtaPhiCutWidget                //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Base/VP1EtaPhiCutWidget.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"

#include "ui_vp1etaphicutform.h"

#include <cmath>

//____________________________________________________________________
class VP1EtaPhiCutWidget::Imp {

public:

  Ui::VP1EtaPhiCutForm ui;
  VP1Interval last_allowedEta;
  QList<VP1Interval> last_allowedPhi;

  void adaptSpinBoxRangesForSymmetry(bool symmetric);
};


//____________________________________________________________________
VP1EtaPhiCutWidget::VP1EtaPhiCutWidget(QWidget * parent,IVP1System * sys)
  : QWidget(parent), VP1HelperClassBase(sys,"VP1EtaPhiCutWidget"), d(new Imp)
{
  d->ui.setupUi(this);

  d->last_allowedEta = allowedEta();
  d->last_allowedPhi = allowedPhi();

  d->ui.phisectionwidget->setSystemBasePointer(sys);

  // -> allowedEta (a bit special due to the "force symmetric" ability)
  connect(d->ui.checkBox_cut_etarange,SIGNAL(toggled(bool)),this,SLOT(possibleChange_allowedEta()));
  connect(d->ui.checkBox_cut_etarange_forcesymmetric,SIGNAL(toggled(bool)),this,SLOT(handleEtaCutSymmetry()));
  connect(d->ui.doubleSpinBox_cut_etalower,SIGNAL(valueChanged(double)),this,SLOT(handleEtaCutSymmetry()));
  connect(d->ui.doubleSpinBox_cut_etaupper,SIGNAL(valueChanged(double)),this,SLOT(handleEtaCutSymmetry()));

  // -> allowedPhi
  connect(d->ui.checkBox_cut_phi,SIGNAL(toggled(bool)),this,SLOT(possibleChange_allowedPhi()));
  connect(d->ui.phisectionwidget,SIGNAL(enabledPhiRangesChanged(const QList<VP1Interval>&)),
	  this,SLOT(possibleChange_allowedPhi()));

}

//____________________________________________________________________
VP1EtaPhiCutWidget::~VP1EtaPhiCutWidget()
{
  delete d;
}

//____________________________________________________________________
VP1Interval VP1EtaPhiCutWidget::allowedEta() const
{
	messageVerbose("VP1EtaPhiCutWidget::allowedEta()");

  // if "eta cut" is not set: we return an interval ]-inf,inf[, so all objects will pass the internal eta cut
  if (!d->ui.checkBox_cut_etarange->isChecked()) {
    return VP1Interval(-std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity());
  }

  const double min = d->ui.doubleSpinBox_cut_etalower->value();
  const double max = d->ui.doubleSpinBox_cut_etaupper->value();

  // if max < min: we return what??
  if (max<min)
    return VP1Interval();

  // FIXME: checkBox_etacut_excludeRange is not actually used now, check and fix!
  // if "Exclude Eta range" is selected, we set the "excludeInterval" flag in the instance of the VP1Interval class
  if (d->ui.checkBox_etacut_excludeRange) {
	  messageVerbose("excludeRange=true");
	  return VP1Interval(min, max, true, true, true);
  }
  return VP1Interval( min, max );//fixme: closed interval?? Ckeck!
}

//____________________________________________________________________
QList<VP1Interval> VP1EtaPhiCutWidget::allowedPhi() const
{
  QList<VP1Interval> l;
  if (!d->ui.checkBox_cut_phi)
    return l;
  if (!d->ui.checkBox_cut_phi->isChecked()) {
    l << VP1Interval(-std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity());
    return l;
  }
  if (!d->ui.phisectionwidget||d->ui.phisectionwidget->allSectorsOff())
    return l;

  if (!d->ui.checkBox_cut_phi->isChecked()||d->ui.phisectionwidget->allSectorsOn()) {
    l << VP1Interval(-std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity());
    return l;
  } else {
    return d->ui.phisectionwidget->enabledPhiRanges();
  }

}

//____________________________________________________________________
void VP1EtaPhiCutWidget::Imp::adaptSpinBoxRangesForSymmetry(bool symmetric)
{
  if (symmetric) {
    ui.doubleSpinBox_cut_etalower->setMaximum(0.0);
    ui.doubleSpinBox_cut_etaupper->setMinimum(0.0);
  } else {
    const double rangemax = ui.doubleSpinBox_cut_etaupper->maximum();
    ui.doubleSpinBox_cut_etalower->setMaximum(rangemax);
    ui.doubleSpinBox_cut_etaupper->setMinimum(-rangemax);
  }
}

//____________________________________________________________________
void VP1EtaPhiCutWidget::handleEtaCutSymmetry()
{
  if (!d->ui.checkBox_cut_etarange_forcesymmetric)
    return;
  if (sender()==d->ui.checkBox_cut_etarange_forcesymmetric) {
    //update allowed ranges, change values if necessary.
    d->adaptSpinBoxRangesForSymmetry(d->ui.checkBox_cut_etarange_forcesymmetric->isChecked());
    if (d->ui.checkBox_cut_etarange_forcesymmetric->isChecked()) {
      //Enforce symmetry:
      const double eta = std::max(fabs(d->ui.doubleSpinBox_cut_etalower->value()),fabs(d->ui.doubleSpinBox_cut_etaupper->value()));
      d->ui.doubleSpinBox_cut_etalower->setValue(-eta);
      d->ui.doubleSpinBox_cut_etaupper->setValue(eta);
    }
  } else if (d->ui.checkBox_cut_etarange_forcesymmetric->isChecked()) {
    //Update other value:
    if (sender()==d->ui.doubleSpinBox_cut_etalower) {
      d->ui.doubleSpinBox_cut_etaupper->setValue(-d->ui.doubleSpinBox_cut_etalower->value());
    } else if (sender()==d->ui.doubleSpinBox_cut_etaupper) {
      d->ui.doubleSpinBox_cut_etalower->setValue(-d->ui.doubleSpinBox_cut_etaupper->value());
    }
  }
  possibleChange_allowedEta();
}

//____________________________________________________________________
void VP1EtaPhiCutWidget::possibleChange_allowedEta()
{
  VP1Interval newAllowedEta = allowedEta();
  if ( d->last_allowedEta == newAllowedEta )
    return;
  d->last_allowedEta = newAllowedEta;
  if (verbose())
    messageVerbose("Emitting allowedEtaChanged("+newAllowedEta.toString()+")");
  emit allowedEtaChanged(newAllowedEta);
}

//____________________________________________________________________
void VP1EtaPhiCutWidget::possibleChange_allowedPhi()
{
  QList<VP1Interval> newAllowedPhi = allowedPhi();
  if ( d->last_allowedPhi == newAllowedPhi )
    return;
  d->last_allowedPhi = newAllowedPhi;
  if (verbose()) {
    QString s;
    for(int i=0;i<newAllowedPhi.count();++i)
      s+= newAllowedPhi.at(i).toString()+(i==newAllowedPhi.count()-1?"":", ");
    messageVerbose("Emitting allowedPhiChanged("+s+")");
  }
  emit allowedPhiChanged(newAllowedPhi);
}

//____________________________________________________________________
void VP1EtaPhiCutWidget::setEtaCutEnabled(bool b)
{
  if (b==d->ui.checkBox_cut_etarange->isChecked())
    return;
  d->ui.checkBox_cut_etarange->setChecked(b);
  possibleChange_allowedEta();
}

//____________________________________________________________________
void VP1EtaPhiCutWidget::setEtaCut(const double& e)
{
  setEtaCut(-fabs(e),fabs(e));
}


//____________________________________________________________________
void VP1EtaPhiCutWidget::showEtaCut(bool b)
{
	d->ui.checkBox_cut_etarange->setChecked(b);
	d->ui.widget_etacut->setVisible(b);
}
//____________________________________________________________________
void VP1EtaPhiCutWidget::showPhiCut(bool b)
{
	d->ui.checkBox_cut_phi->setChecked(b);
	d->ui.widget_phicut->setVisible(b);
}

//____________________________________________________________________
void VP1EtaPhiCutWidget::setEtaCut(const double& a,const double&b)
{
  double e1(a),e2(b);
  const double rangemax = d->ui.doubleSpinBox_cut_etaupper->maximum();
  e1 = std::max(-rangemax,e1);
  e1 = std::min(rangemax,e1);
  e2 = std::max(-rangemax,e2);
  e2 = std::min(rangemax,e2);
  if (e1>=e2||e1!=e1||e2!=e2) {
    e1 = 0;
    e2 = 0;
  }

  bool save = blockSignals(true);
  bool save1 = d->ui.doubleSpinBox_cut_etalower->blockSignals(true);
  bool save2 = d->ui.doubleSpinBox_cut_etaupper->blockSignals(true);
  bool save3 = d->ui.checkBox_cut_etarange->blockSignals(true);
  bool save4 = d->ui.checkBox_cut_etarange_forcesymmetric->blockSignals(true);

  d->ui.checkBox_cut_etarange->setChecked(true);
  d->ui.checkBox_cut_etarange_forcesymmetric->setChecked(e1==-e2);
  d->adaptSpinBoxRangesForSymmetry(e1==-e2);
  d->ui.doubleSpinBox_cut_etalower->setValue(e1);
  d->ui.doubleSpinBox_cut_etaupper->setValue(e2);

  blockSignals(save);
  d->ui.doubleSpinBox_cut_etalower->blockSignals(save1);
  d->ui.doubleSpinBox_cut_etaupper->blockSignals(save2);
  d->ui.checkBox_cut_etarange->blockSignals(save3);
  d->ui.checkBox_cut_etarange_forcesymmetric->blockSignals(save4);
  possibleChange_allowedEta();
}

//____________________________________________________________________
QByteArray VP1EtaPhiCutWidget::saveState() const
{
  //NB: We can't use the VP1Serialise::save(VP1EtaPhiCutWidget*) here
  //(that would give infinite recursion).

  VP1Serialise serialise(1/*version*/,systemBase());
  serialise.save(d->ui.checkBox_cut_etarange);
  serialise.save(d->ui.checkBox_cut_etarange_forcesymmetric);
  serialise.save(d->ui.doubleSpinBox_cut_etalower);
  serialise.save(d->ui.doubleSpinBox_cut_etaupper);
  serialise.save(d->ui.checkBox_cut_phi);
  serialise.save(d->ui.phisectionwidget);//Version 0 saved the old-style phisection widget states.
  serialise.widgetHandled(this);
  serialise.warnUnsaved(this);
  return serialise.result();
}

//____________________________________________________________________
void VP1EtaPhiCutWidget::restoreFromState( const QByteArray& ba)
{
  //NB: We can't use the VP1Deserialise::restore(VP1EtaPhiCutWidget*) here
  //(that would give infinite recursion).

  VP1Deserialise state(ba,systemBase());
  if (state.version()<0||state.version()>1)
    return;//Ignore silently

  state.restore(d->ui.checkBox_cut_etarange);
  state.restore(d->ui.checkBox_cut_etarange_forcesymmetric);
  state.restore(d->ui.doubleSpinBox_cut_etalower);
  state.restore(d->ui.doubleSpinBox_cut_etaupper);
  state.restore(d->ui.checkBox_cut_phi);
  if (state.version()==0) {
    state.ignoreObsoletePhiSectionWidgetState();
    state.ignoreWidget(d->ui.phisectionwidget);
  } else {
    state.restore(d->ui.phisectionwidget);
  }
  state.widgetHandled(this);
  state.warnUnrestored(this);

  possibleChange_allowedEta();
  possibleChange_allowedPhi();
}
