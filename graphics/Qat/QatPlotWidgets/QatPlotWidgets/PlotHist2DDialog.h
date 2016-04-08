#ifndef _PlotHist2DDialog_h_
#define _PlotHist2DDialog_h_
#include <QtGui/QDialog>
class PlotHist2D;
class PlotHist2DDialog: public QDialog {
 
  Q_OBJECT

 public:

  // Constructor:
  PlotHist2DDialog(QWidget *parent);
  
  // Destructor:
  virtual ~PlotHist2DDialog();

  void synchTo(PlotHist2D *);

  public slots:
  
  void setLineWidth(int);
  void toggleLineColor(bool);
  void toggleBrushColor(bool);
  void toggleLineStyle(bool);
  void toggleBrushStyle(bool);

 signals:

  void updateHisto();

 private:

  
  class Clockwork;
  Clockwork *c; 

}; 

#endif
