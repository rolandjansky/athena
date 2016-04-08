#ifndef _PlotHist1DDialog_h_
#define _PlotHist1DDialog_h_
#include <QtGui/QDialog>
class PlotHist1D;
class PlotHist1DDialog: public QDialog {
 
  Q_OBJECT

 public:

  // Constructor:
  PlotHist1DDialog(QWidget *parent);
  
  // Destructor:
  virtual ~PlotHist1DDialog();

  void synchTo(PlotHist1D *);

  public slots:
  
  void setLineWidth(int);
  void setSymbolSize(int);
  void toggleLineColor(bool);
  void toggleBrushColor(bool);
  void toggleLineStyle(bool);
  void toggleBrushStyle(bool);
  void togglePlotStyle(bool);

 signals:

  void updateHisto();

 private:

  class Clockwork;
  Clockwork *c; 

}; 

#endif
