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

  SoSeparator       *root;
  SoSwitch          *visibilitySwitch;
  SoLightModel      *lightModel;
  SoMaterial        *material;
  SoPickStyle       *pickStyle;

  std::map<std::string, SoSeparator *> fileMap;
  Ui::ExtraInputControllerForm ui;
  QString lastInputDir;
  void inputFile(const VP1ExtraInputSystem *This, const QString &);

};


VP1ExtraInputSystem::VP1ExtraInputSystem():
  IVP13DSystemSimple("ExtraInput","Co-display extra input from an external Open Inventor, or VRML, file","boudreau@pitt.edu"),
  c(new Clockwork())
{
  c->root=NULL;
  c->visibilitySwitch = 0;
  c->lightModel = 0;
  c->material = 0;
  c->pickStyle = 0;
  c->ui.materialButton=0;
  c->lastInputDir = VP1Settings::defaultFileSelectDirectory();
}

VP1ExtraInputSystem::~VP1ExtraInputSystem(){
  delete c;
}

QWidget* VP1ExtraInputSystem::buildController(){

  QWidget* controller = new QWidget(0);
  c->ui.setupUi(controller);
  c->ui.materialButton->setMaterial(c->material);
  connect (c->ui.visibleCheckBox,SIGNAL(toggled(bool)), this, SLOT(toggleVisibility(bool)));
  connect (c->ui.baseLMButton,SIGNAL(toggled(bool)), this, SLOT(toggleBaseLightModel(bool)));
  connect (c->ui.newFilePushButton, SIGNAL(clicked()), this, SLOT(newFile()));
  connect (c->ui.deleteFilePushButton, SIGNAL(clicked()), this, SLOT(deleteFile()));
  return controller;


}


void VP1ExtraInputSystem::buildPermanentSceneGraph(StoreGateSvc*,SoSeparator *root){
  c->root=root;

  c->visibilitySwitch = new SoSwitch();
  c->visibilitySwitch->ref();
  toggleVisibility(c->ui.visibleCheckBox->isChecked());

  c->lightModel = new SoLightModel();
  c->lightModel->ref();
  toggleBaseLightModel(c->ui.baseLMButton->isChecked());
  c->root->addChild(c->lightModel);

  c->material = new SoMaterial();
  c->material->ref();
  c->ui.materialButton->setMaterial(c->material);
  c->root->addChild(c->material);

  c->pickStyle = new SoPickStyle();
  c->pickStyle->ref();
  c->pickStyle->style = SoPickStyleElement::UNPICKABLE;
  c->root->addChild(c->pickStyle);

  c->root->addChild(c->visibilitySwitch);

}

void VP1ExtraInputSystem::systemuncreate()
{
  if (c->visibilitySwitch)
    c->visibilitySwitch->unref();
  if (c->lightModel)
    c->lightModel->unref();
  if (c->material)
    c->material->unref();
  if (c->pickStyle)
    c->pickStyle->unref();
}

void VP1ExtraInputSystem::buildEventSceneGraph(StoreGateSvc*, SoSeparator *){
}


void VP1ExtraInputSystem::toggleVisibility( bool flag) {
  c->visibilitySwitch->whichChild = flag ? SO_SWITCH_ALL : SO_SWITCH_NONE;
}

void VP1ExtraInputSystem::toggleBaseLightModel(bool flag) {
  c->lightModel->model= flag ? SoLightModel::BASE_COLOR : SoLightModel::PHONG;
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

  QString fileName = QFileDialog::getOpenFileName(NULL, tr("Open File"),c->lastInputDir,tr("3D Formats (*.iv *.wrl)"));
  if (!fileName.isEmpty()) c->inputFile(this,fileName);

}

void VP1ExtraInputSystem::deleteFile() {
  if (c->ui.fileListWidget->count()) {
    int currentRow = c->ui.fileListWidget->currentRow();
    QListWidgetItem *item=c->ui.fileListWidget->takeItem(currentRow);
    c->visibilitySwitch->removeChild(c->fileMap[item->text().toStdString()]);
    c->fileMap.erase(item->text().toStdString());
  }
}

QByteArray VP1ExtraInputSystem::saveState(){

  ensureBuildController();

  VP1Serialise serialise(1/*version*/,this);
  serialise.save(IVP13DSystemSimple::saveState());

  serialise.save(c->ui.toolBox);

  QStringList fileListEntries;
  for (int i=0;i<c->ui.fileListWidget->count();++i)
    fileListEntries << c->ui.fileListWidget->item(i)->text();
  serialise.save(fileListEntries);
  serialise.widgetHandled(c->ui.fileListWidget);

  serialise.save(c->ui.baseLMButton,c->ui.phongLMButton);
  serialise.save(c->ui.visibleCheckBox);
  serialise.save(c->ui.materialButton);

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

  state.restore(c->ui.toolBox);
  foreach(QString fileName, state.restore<QStringList>())
    c->inputFile(this,fileName);
  state.restore(c->ui.baseLMButton,c->ui.phongLMButton);
  state.restore(c->ui.visibleCheckBox);
  state.restore(c->ui.materialButton);

  state.warnUnrestored(controllerWidget());
}
