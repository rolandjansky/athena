#ifndef _QatPROJECTFORM_H_
#define _QatPROJECTFORM_H_
#include <QWidget>
#include "ui_qat-project.h"

class QatProjectForm : public QWidget {

  Q_OBJECT

 public:


  // Constructor
  QatProjectForm(QWidget *parent=NULL);
  
  // Destructor:
  ~QatProjectForm();

 private:

  Ui::QatProjectForm ui;

  public slots:

  void editingFinished();
  void createProject();
  void changeDir();
  


};
#endif
