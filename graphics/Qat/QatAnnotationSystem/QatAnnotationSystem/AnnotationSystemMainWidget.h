#ifndef _ANNOTATIONSYSTEMMAINWIDGET_H_
#define _ANNOTATIONSYSTEMMAINWIDGET_H_
#include "ui_AnnotationSystemMainWidget.h"
class AnnotationSystemMainWidget: public QWidget {

  Q_OBJECT
    public:

  AnnotationSystemMainWidget(QWidget *parent=NULL);

  QTextEdit *textEdit();

  TextEditor *textEditor();

 public slots:

  void insertDate();
  void insertImage();
  void pasteImage();
  void print();
  void save();


 private:

  Ui::AnnotationSystemMainWidget ui;

};

#endif
