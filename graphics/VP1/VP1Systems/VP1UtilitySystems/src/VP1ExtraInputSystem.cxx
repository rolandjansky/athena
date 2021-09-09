/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1UtilitySystems/VP1ExtraInputSystem.h"
#include "ui_extrainputcontrollerform.h"

#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1Settings.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSwitch.h>

#include <QFileDialog>
#include <QDir>
#include <map>

class VP1ExtraInputSystem::Clockwork {

public:

  SoSeparator       *root = nullptr;
  SoSwitch          *visibilitySwitch = nullptr;
  SoLightModel      *lightModel = nullptr;
  SoMaterial        *material = nullptr;
  SoPickStyle       *pickStyle = nullptr;

  std::map<std::string, SoSeparator *> fileMap;
  Ui::ExtraInputControllerForm ui{};
  QString lastInputDir;
  void inputFile(const VP1ExtraInputSystem *This, const QString &);

};


VP1ExtraInputSystem::VP1ExtraInputSystem():
  IVP13DSystemSimple("ExtraInput","Co-display extra input from an external Open Inventor, or VRML, file","boudreau@pitt.edu"),
  m_c(new Clockwork())
{
  m_c->root=NULL;
  m_c->visibilitySwitch = 0;
  m_c->lightModel = 0;
  m_c->material = 0;
  m_c->pickStyle = 0;
  m_c->ui.materialButton=0;
  m_c->lastInputDir = VP1Settings::defaultFileSelectDirectory();
}

VP1ExtraInputSystem::~VP1ExtraInputSystem(){
  delete m_c;
}

QWidget* VP1ExtraInputSystem::buildController(){

  QWidget* controller = new QWidget(0);
  m_c->ui.setupUi(controller);
  m_c->ui.materialButton->setMaterial(m_c->material);
  connect (m_c->ui.visibleCheckBox,SIGNAL(toggled(bool)), this, SLOT(toggleVisibility(bool)));
  connect (m_c->ui.baseLMButton,SIGNAL(toggled(bool)), this, SLOT(toggleBaseLightModel(bool)));
  connect (m_c->ui.newFilePushButton, SIGNAL(clicked()), this, SLOT(newFile()));
  connect (m_c->ui.deleteFilePushButton, SIGNAL(clicked()), this, SLOT(deleteFile()));
  return controller;


}


void VP1ExtraInputSystem::buildPermanentSceneGraph(StoreGateSvc*,SoSeparator *root){
  m_c->root=root;

  m_c->visibilitySwitch = new SoSwitch();
  m_c->visibilitySwitch->ref();
  toggleVisibility(m_c->ui.visibleCheckBox->isChecked());

  m_c->lightModel = new SoLightModel();
  m_c->lightModel->ref();
  toggleBaseLightModel(m_c->ui.baseLMButton->isChecked());
  m_c->root->addChild(m_c->lightModel);

  m_c->material = new SoMaterial();
  m_c->material->ref();
  m_c->ui.materialButton->setMaterial(m_c->material);
  m_c->root->addChild(m_c->material);

  m_c->pickStyle = new SoPickStyle();
  m_c->pickStyle->ref();
  m_c->pickStyle->style = SoPickStyleElement::UNPICKABLE;
  m_c->root->addChild(m_c->pickStyle);

  m_c->root->addChild(m_c->visibilitySwitch);

}

void VP1ExtraInputSystem::systemuncreate()
{
  if (m_c->visibilitySwitch)
    m_c->visibilitySwitch->unref();
  if (m_c->lightModel)
    m_c->lightModel->unref();
  if (m_c->material)
    m_c->material->unref();
  if (m_c->pickStyle)
    m_c->pickStyle->unref();
}

void VP1ExtraInputSystem::buildEventSceneGraph(StoreGateSvc*, SoSeparator *){
}


void VP1ExtraInputSystem::toggleVisibility( bool flag) {
  m_c->visibilitySwitch->whichChild = flag ? SO_SWITCH_ALL : SO_SWITCH_NONE;
}

void VP1ExtraInputSystem::toggleBaseLightModel(bool flag) {
  m_c->lightModel->model= flag ? SoLightModel::BASE_COLOR : SoLightModel::PHONG;
}

void VP1ExtraInputSystem::Clockwork::inputFile(const VP1ExtraInputSystem *This, const QString & fileName) {
  QList<QListWidgetItem *> itemList = ui.fileListWidget->findItems(fileName,Qt::MatchExactly);
  if (itemList.isEmpty()) {
    SoSeparator *inputRoot = VP1QtInventorUtils::readGraphFromFile(fileName);
    if (inputRoot) {
      visibilitySwitch->addChild(inputRoot);
      ui.fileListWidget->addItem(fileName);
      fileMap[fileName.toStdString()]=inputRoot;
    } else {
      This->message("Warning: Could not get scenegraph from file "+fileName);
    }
    lastInputDir = QFileInfo(fileName).absolutePath();
  } else {
    This->message("Warning, file is already in the display list");
  }
}

void VP1ExtraInputSystem::newFile() {

  QString fileName = QFileDialog::getOpenFileName(NULL, tr("Open File"),m_c->lastInputDir,tr("3D Formats (*.iv *.wrl)"));
  if (!fileName.isEmpty()) m_c->inputFile(this,fileName);

}

void VP1ExtraInputSystem::deleteFile() {
  if (m_c->ui.fileListWidget->count()) {
    int currentRow = m_c->ui.fileListWidget->currentRow();
    QListWidgetItem *item=m_c->ui.fileListWidget->takeItem(currentRow);
    m_c->visibilitySwitch->removeChild(m_c->fileMap[item->text().toStdString()]);
    m_c->fileMap.erase(item->text().toStdString());
  }
}

QByteArray VP1ExtraInputSystem::saveState(){

  ensureBuildController();

  VP1Serialise serialise(1/*version*/,this);
  serialise.save(IVP13DSystemSimple::saveState());

  serialise.save(m_c->ui.toolBox);

  QStringList fileListEntries;
  for (int i=0;i<m_c->ui.fileListWidget->count();++i)
    fileListEntries << m_c->ui.fileListWidget->item(i)->text();
  serialise.save(fileListEntries);
  serialise.widgetHandled(m_c->ui.fileListWidget);

  serialise.save(m_c->ui.baseLMButton,m_c->ui.phongLMButton);
  serialise.save(m_c->ui.visibleCheckBox);
  serialise.save(m_c->ui.materialButton);

  serialise.warnUnsaved(controllerWidget());
  return serialise.result();
}

void VP1ExtraInputSystem::restoreFromState(QByteArray ba){
  VP1Deserialise state(ba,this);
  if (state.version()==0) {
    message("Warning: State data in .vp1 file has obsolete format - ignoring!");
    return;
  }
  if (state.version()!=1) {
    message("Warning: State data in .vp1 file is in wrong format - ignoring!");
    return;
  }
  ensureBuildController();
  IVP13DSystemSimple::restoreFromState(state.restoreByteArray());

  state.restore(m_c->ui.toolBox);
  foreach(QString fileName, state.restore<QStringList>())
    m_c->inputFile(this,fileName);
  state.restore(m_c->ui.baseLMButton,m_c->ui.phongLMButton);
  state.restore(m_c->ui.visibleCheckBox);
  state.restore(m_c->ui.materialButton);

  state.warnUnrestored(controllerWidget());
}
