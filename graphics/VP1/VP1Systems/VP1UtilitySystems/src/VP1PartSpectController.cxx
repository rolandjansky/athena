/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1UtilitySystems/VP1PartSpectController.h"
#include "ui_partspectcontrollerform.h"

#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Settings.h"

#include <QFileDialog>
#include <QFileInfo>

class VP1PartSpectController::Imp
{
public:
  Imp(VP1PartSpectController*);
  ~Imp();

  VP1PartSpectController*    theclass;
  Ui::frmPartSpectController ui;

  VP1PartSpect::ParticleType last_particleType;
  QString lastInputDir;
};

VP1PartSpectController::Imp::Imp(VP1PartSpectController* _theclass)
  :theclass(_theclass)
  ,last_particleType(VP1PartSpect::Neutron)
  ,lastInputDir(VP1Settings::defaultFileSelectDirectory())
{
}

VP1PartSpectController::Imp::~Imp()
{
}

VP1PartSpectController::VP1PartSpectController(IVP1System* sys)
  :VP1Controller(sys,"PartSpectController")
  ,d(new Imp(this))
{
  d->theclass = this;
  d->ui.setupUi(this);

  d->ui.lblFileName->setText("...");
  connect(d->ui.pbtnOpenFile, SIGNAL(clicked()), this, SLOT(openFile()));

  connect(d->ui.rbtnBaryon, SIGNAL(toggled(bool)), this, SLOT(particleType()));
  connect(d->ui.rbtnE, SIGNAL(toggled(bool)), this, SLOT(particleType()));
  connect(d->ui.rbtnGamma, SIGNAL(toggled(bool)), this, SLOT(particleType()));
  connect(d->ui.rbtnLepton, SIGNAL(toggled(bool)), this, SLOT(particleType()));
  connect(d->ui.rbtnMeson, SIGNAL(toggled(bool)), this, SLOT(particleType()));
  connect(d->ui.rbtnNeutron, SIGNAL(toggled(bool)), this, SLOT(particleType()));
  connect(d->ui.rbtnNucleus, SIGNAL(toggled(bool)), this, SLOT(particleType()));
  connect(d->ui.rbtnPi, SIGNAL(toggled(bool)), this, SLOT(particleType()));
  connect(d->ui.rbtnProton, SIGNAL(toggled(bool)), this, SLOT(particleType()));
  d->ui.rbtnNeutron->setChecked(true);
  d->last_particleType = VP1PartSpect::Neutron;
}

VP1PartSpectController::~VP1PartSpectController()
{
  delete d;
}

void VP1PartSpectController::actualRestoreSettings(VP1Deserialise& s)
{
  if (s.version()!=0) {
    message("Warning: State data in .vp1 file has unsupported version ("+str(s.version())+")");
    return;
  }

  s.restore(d->ui.rbtnBaryon
	    ,d->ui.rbtnE
	    ,d->ui.rbtnGamma
	    ,d->ui.rbtnLepton
	    ,d->ui.rbtnMeson
	    ,d->ui.rbtnNeutron
	    ,d->ui.rbtnNucleus
	    ,d->ui.rbtnPi
	    ,d->ui.rbtnProton);
  particleType();
}


int VP1PartSpectController::currentSettingsVersion() const
{
  return 0;
}

void VP1PartSpectController::actualSaveSettings(VP1Serialise& s) const
{
  s.save(d->ui.rbtnBaryon
	 ,d->ui.rbtnE
	 ,d->ui.rbtnGamma
	 ,d->ui.rbtnLepton
	 ,d->ui.rbtnMeson
	 ,d->ui.rbtnNeutron
	 ,d->ui.rbtnNucleus
	 ,d->ui.rbtnPi
	 ,d->ui.rbtnProton);
}

VP1PartSpect::ParticleType VP1PartSpectController::getParticleType()
{
  return d->last_particleType;
}

void VP1PartSpectController::openFile()
{
  QString fileName = QFileDialog::getOpenFileName(NULL
						  ,tr("Open File")
						  ,d->lastInputDir
						  ,tr("Root files (*.root)"));
  if(!fileName.isEmpty()) {
    QFileInfo fileInfo(fileName);
    d->ui.lblFileName->setText(fileInfo.fileName());
    fileUpdated(fileName);
    d->lastInputDir = fileInfo.absolutePath();
  }
}

void VP1PartSpectController::particleType()
{
  if(d->ui.rbtnBaryon->isChecked()) {
    particleTypeChanged(VP1PartSpect::Baryon);
    d->last_particleType = VP1PartSpect::Baryon;
  } else if(d->ui.rbtnE->isChecked()) {
    particleTypeChanged(VP1PartSpect::E);
    d->last_particleType = VP1PartSpect::E;
  } else if(d->ui.rbtnGamma->isChecked()) {
    particleTypeChanged(VP1PartSpect::Gamma);
    d->last_particleType = VP1PartSpect::Gamma;
  }else if(d->ui.rbtnLepton->isChecked()) {
    particleTypeChanged(VP1PartSpect::Lepton);
    d->last_particleType = VP1PartSpect::Lepton;
  }else if(d->ui.rbtnMeson->isChecked()) {
    particleTypeChanged(VP1PartSpect::Meson);
    d->last_particleType = VP1PartSpect::Meson;
  }else if(d->ui.rbtnNeutron->isChecked()) {
    particleTypeChanged(VP1PartSpect::Neutron);
    d->last_particleType = VP1PartSpect::Neutron;
  }else if(d->ui.rbtnNucleus->isChecked()) {
    particleTypeChanged(VP1PartSpect::Nucleus);
    d->last_particleType = VP1PartSpect::Nucleus;
  }else if(d->ui.rbtnPi->isChecked()) {
    particleTypeChanged(VP1PartSpect::Pi);
    d->last_particleType = VP1PartSpect::Pi;
  }else { // if(d->ui.rbtnProton->isChecked()) {
    particleTypeChanged(VP1PartSpect::Proton);
    d->last_particleType = VP1PartSpect::Proton;
  }
}


