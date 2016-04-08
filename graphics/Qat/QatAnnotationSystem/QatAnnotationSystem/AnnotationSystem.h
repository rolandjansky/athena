#ifndef _ANNOTATIONSYSTEM_H_
#define _ANNOTATIONSYSTEM_H_
#include <QatSystemInterfaces/SystemInterface.h>
#include <QtGui/QWidget>
class AnnotationSystem:public DASystem {

  Q_OBJECT


public:

  AnnotationSystem();

  virtual ~AnnotationSystem();
  
  virtual QString name() { return QString("AnnotationSystem");}

  virtual QWidget *mainWidget() const;

  virtual QWidget *toolWidget() const;

  virtual QByteArray saveState();

  virtual void restoreFromState (QByteArray ba); 

  virtual QByteArray saveCache();

  virtual void restoreFromCache (QByteArray ba); 

  virtual AnnotationSystem *clone() const;

  virtual void update();


  public slots:


  void informSelectedImage(const QString & imageFile);

 private:

  // It is illegal to copy or assign:
  AnnotationSystem(const AnnotationSystem &);
  AnnotationSystem & operator=(const AnnotationSystem &);


  // Internal:
  class Clockwork;
  Clockwork *c;
};

class AnnotationSystemFactory : public QObject, public SystemFactoryInterface
 {
     Q_OBJECT
     Q_INTERFACES(SystemFactoryInterface)

 public:


     virtual DASystem *newSystem() const;

 };

#endif
