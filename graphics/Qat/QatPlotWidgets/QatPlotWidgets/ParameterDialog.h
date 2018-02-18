#ifndef QATPLOTWIDGETS_PARAMETERDIALOG_H
#define QATPLOTWIDGETS_PARAMETERDIALOG_H
#include "CLHEP/GenericFunctions/Parameter.hh"
#include <QtGui/QDialog>
class ParameterDialog:public QDialog {

Q_OBJECT

  public:

  // Constructor:
  ParameterDialog(QWidget *parent, Genfun::Parameter *par);
  
  // Destructor:
  virtual ~ParameterDialog();


 public slots:
  
  void updateParameter(int val); // val from 0 to 99

 signals:
  
  void updateFunction();

 private:

  // Illegal operations:
  ParameterDialog(const ParameterDialog &);
  ParameterDialog & operator=(const ParameterDialog &);

  // Internals:
  class Clockwork;
  Clockwork *m_c;

};

#endif
