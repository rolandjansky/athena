#ifndef QATPLOTWIDGETS_PLOTHIST2DDIALOG_H
#define QATPLOTWIDGETS_PLOTHIST2DDIALOG_H
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
  Clockwork *m_c;

}; 

#endif
