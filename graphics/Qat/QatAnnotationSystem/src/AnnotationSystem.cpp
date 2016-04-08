#include <QObject>
#include "QatAnnotationSystem/AnnotationSystem.h"
#include "QatAnnotationSystem/AnnotationSystemMainWidget.h"
#include "QatAnnotationSystem/AnnotationSystemMenu.h"
#include "QatSystemInterfaces/Serializer.h"
#include <QtGui/QTextEdit>
#include <QScrollBar>
#include <iostream>
#include <stdexcept>
#include <sstream>
class AnnotationSystemMainWidget;
class AnnotationSystem::Clockwork {
public:
  AnnotationSystemMainWidget *annotationSystemMainWidget;
  AnnotationSystemMenu       *annotationSystemToolWidget;
};


AnnotationSystem::AnnotationSystem ():c(new Clockwork()) {
  QRectF rect;

  c->annotationSystemMainWidget=new AnnotationSystemMainWidget();
  c->annotationSystemToolWidget=new AnnotationSystemMenu();
  connect (c->annotationSystemToolWidget, SIGNAL(insertDate()), c->annotationSystemMainWidget, SLOT(insertDate()));
  connect (c->annotationSystemToolWidget, SIGNAL(insertImage()), c->annotationSystemMainWidget, SLOT(insertImage()));
  connect (c->annotationSystemToolWidget, SIGNAL(pasteImage()), c->annotationSystemMainWidget, SLOT(pasteImage()));

  connect (c->annotationSystemToolWidget, SIGNAL(print()), c->annotationSystemMainWidget, SLOT(print()));
  connect (c->annotationSystemToolWidget, SIGNAL(save()), c->annotationSystemMainWidget, SLOT(save()));
  connect (c->annotationSystemMainWidget->textEditor(), SIGNAL(imageSelected(const QString &)), this, SLOT(informSelectedImage(const QString & )));
}

AnnotationSystem::~AnnotationSystem() {
}

QWidget *AnnotationSystem::mainWidget() const {
  return c->annotationSystemMainWidget;
}

QWidget *AnnotationSystem::toolWidget() const {
  return c->annotationSystemToolWidget;
}

void AnnotationSystem::update() {
}

QByteArray AnnotationSystem::saveState() {
  Serializer serializer(0);
  serializer.save(c->annotationSystemMainWidget->textEdit());
  return serializer.result();
}

void AnnotationSystem::restoreFromState (QByteArray ba){
  Deserializer state(ba);
  if (state.version()>0) throw std::runtime_error ("Wrong version code in AnnotationSystem::restoreFromState");
  state.restore(c->annotationSystemMainWidget->textEdit());
  QTextCursor curse=c->annotationSystemMainWidget->textEdit()->textCursor();
  curse.setPosition(0);
  c->annotationSystemMainWidget->textEdit()->setTextCursor(curse);
}

QByteArray AnnotationSystem::saveCache() {
  Serializer serializer(0);
  return serializer.result();
}

void AnnotationSystem::restoreFromCache (QByteArray ba) {
  Deserializer state(ba);
  if (state.version()>0) throw std::runtime_error ("Wrong version code in AnnotationSystem::restoreFromCache");
}

void AnnotationSystem::informSelectedImage(const QString & imageFile) {
 {
   std::ostringstream message;
   message << "External reference to file named " << imageFile.toStdString() << std::endl; 
   presenterMessageTextEdit()->insertPlainText (message.str().c_str());
 }
}


//=====================Boilerplate======================//
AnnotationSystem *AnnotationSystem::clone() const {
  return new AnnotationSystem();
}

DASystem *AnnotationSystemFactory::newSystem() const {
  return new AnnotationSystem();
}

Q_EXPORT_PLUGIN2(AnnotationSystemPlugin, AnnotationSystemFactory)
