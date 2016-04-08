#ifndef _ANNOTATIONSYSTEMMENU_H_
#define _ANNOTATIONSYSTEMMENU_H_
#include "ui_AnnotationSystemMenu.h"

class AnnotationSystemMenu: public QWidget {
  
  Q_OBJECT
    
    public:

  // Constructor:
  AnnotationSystemMenu(QWidget *parent=NULL);

  signals:
   
  void insertDate();
  void insertImage();
  void pasteImage();
  void print();
  void save();

 private:

  Ui::AnnotationSystemMenu ui;

};

#endif
