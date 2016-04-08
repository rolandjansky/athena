#ifndef _PlotFunction1DDialog_h_
#define _PlotFunction1DDialog_h_
#include "CLHEP/GenericFunctions/Parameter.hh"
class PlotFunction1D;
#include <QtGui/QDialog>
class PlotFunction1DDialog: public QDialog {
 
  Q_OBJECT

 public:

  // Constructor:
  PlotFunction1DDialog(QWidget *parent);
  
  // Destructor:
  virtual ~PlotFunction1DDialog();

  void synchTo(PlotFunction1D *);
  void clearParameters();
  void addParameter(Genfun::Parameter * p);

  public slots:
  
  void setLineWidth(int);
  void toggleLineColor(bool);
  void toggleLineStyle(bool);
  void changeParameter(int row, int column);
  void popParameterDial(int row, int column);

  void synchToFunction();

 signals:

  void updateFunction();

 private:

  class Clockwork;
  Clockwork *c; 

}; 

#endif
