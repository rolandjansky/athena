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
  Ui::frmPartSpectController ui{};

  VP1PartSpect::ParticleType last_particleType;
  QString lastInputDir;
};

VP1PartSpectController::Imp::Imp(VP1PartSpectController* the_class)
  :theclass(the_class)
  ,last_particleType(VP1PartSpect::Neutron)
  ,lastInputDir(VP1Settings::defaultFileSelectDirectory())
{
}

VP1PartSpectController::Imp::~Imp()
{
}

VP1PartSpectController::VP1PartSpectController(IVP1System* sys)
  :VP1Controller(sys,"PartSpectController")
  ,m_d(new Imp(this))
{
  m_d->theclass = this;
  m_d->ui.setupUi(this);

  m_d->ui.lblFileName->setText("...");
  connect(m_d->ui.pbtnOpenFile, SIGNAL(clicked()), this, SLOT(openFile()));

  connect(m_d->ui.rbtnBaryon, SIGNAL(toggled(bool)), this, SLOT(particleType()));
  connect(m_d->ui.rbtnE, SIGNAL(toggled(bool)), this, SLOT(particleType()));
  connect(m_d->ui.rbtnGamma, SIGNAL(toggled(bool)), this, SLOT(particleType()));
  connect(m_d->ui.rbtnLepton, SIGNAL(toggled(bool)), this, SLOT(particleType()));
  connect(m_d->ui.rbtnMeson, SIGNAL(toggled(bool)), this, SLOT(particleType()));
  connect(m_d->ui.rbtnNeutron, SIGNAL(toggled(bool)), this, SLOT(particleType()));
  connect(m_d->ui.rbtnNucleus, SIGNAL(toggled(bool)), this, SLOT(particleType()));
  connect(m_d->ui.rbtnPi, SIGNAL(toggled(bool)), this, SLOT(particleType()));
  connect(m_d->ui.rbtnProton, SIGNAL(toggled(bool)), this, SLOT(particleType()));
  m_d->ui.rbtnNeutron->setChecked(true);
  m_d->last_particleType = VP1PartSpect::Neutron;
}

VP1PartSpectController::~VP1PartSpectController()
{
  delete m_d;
}

void VP1PartSpectController::actualRestoreSettings(VP1Deserialise& s)
{
  if (s.version()!=0) {
    message("Warning: State data in .vp1 file has unsupported version ("+str(s.version())+")");
    return;
  }

  s.restore(m_d->ui.rbtnBaryon
	    ,m_d->ui.rbtnE
	    ,m_d->ui.rbtnGamma
	    ,m_d->ui.rbtnLepton
	    ,m_d->ui.rbtnMeson
	    ,m_d->ui.rbtnNeutron
	    ,m_d->ui.rbtnNucleus
	    ,m_d->ui.rbtnPi
	    ,m_d->ui.rbtnProton);
  particleType();
}


int VP1PartSpectController::currentSettingsVersion() const
{
  return 0;
}

void VP1PartSpectController::actualSaveSettings(VP1Serialise& s) const
{
  s.save(m_d->ui.rbtnBaryon
	 ,m_d->ui.rbtnE
	 ,m_d->ui.rbtnGamma
	 ,m_d->ui.rbtnLepton
	 ,m_d->ui.rbtnMeson
	 ,m_d->ui.rbtnNeutron
	 ,m_d->ui.rbtnNucleus
	 ,m_d->ui.rbtnPi
	 ,m_d->ui.rbtnProton);
}

VP1PartSpect::ParticleType VP1PartSpectController::getParticleType()
{
  return m_d->last_particleType;
}

void VP1PartSpectController::openFile()
{
  QString fileName = QFileDialog::getOpenFileName(NULL
						  ,tr("Open File")
						  ,m_d->lastInputDir
						  ,tr("Root files (*.root)"));
  if(!fileName.isEmpty()) {
    QFileInfo fileInfo(fileName);
    m_d->ui.lblFileName->setText(fileInfo.fileName());
    fileUpdated(fileName);
    m_d->lastInputDir = fileInfo.absolutePath();
  }
}

void VP1PartSpectController::particleType()
{
  if(m_d->ui.rbtnBaryon->isChecked()) {
    particleTypeChanged(VP1PartSpect::Baryon);
    m_d->last_particleType = VP1PartSpect::Baryon;
  } else if(m_d->ui.rbtnE->isChecked()) {
    particleTypeChanged(VP1PartSpect::E);
    m_d->last_particleType = VP1PartSpect::E;
  } else if(m_d->ui.rbtnGamma->isChecked()) {
    particleTypeChanged(VP1PartSpect::Gamma);
    m_d->last_particleType = VP1PartSpect::Gamma;
  }else if(m_d->ui.rbtnLepton->isChecked()) {
    particleTypeChanged(VP1PartSpect::Lepton);
    m_d->last_particleType = VP1PartSpect::Lepton;
  }else if(m_d->ui.rbtnMeson->isChecked()) {
    particleTypeChanged(VP1PartSpect::Meson);
    m_d->last_particleType = VP1PartSpect::Meson;
  }else if(m_d->ui.rbtnNeutron->isChecked()) {
    particleTypeChanged(VP1PartSpect::Neutron);
    m_d->last_particleType = VP1PartSpect::Neutron;
  }else if(m_d->ui.rbtnNucleus->isChecked()) {
    particleTypeChanged(VP1PartSpect::Nucleus);
    m_d->last_particleType = VP1PartSpect::Nucleus;
  }else if(m_d->ui.rbtnPi->isChecked()) {
    particleTypeChanged(VP1PartSpect::Pi);
    m_d->last_particleType = VP1PartSpect::Pi;
  }else { // if(m_d->ui.rbtnProton->isChecked()) {
    particleTypeChanged(VP1PartSpect::Proton);
    m_d->last_particleType = VP1PartSpect::Proton;
  }
}


