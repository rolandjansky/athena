#ifndef _PlotDirDialog_h_
#define _PlotDirDialog_h_
class PlotHist1D;
class PlotHist2D;
#include <QtGui/QDialog>
class PlotDirDialog: public QDialog {
 
  Q_OBJECT

 public:

  // Constructor:
  PlotDirDialog(QWidget *parent);
  
  // Destructor:
  virtual ~PlotDirDialog();

  // Synch to (*copy* the vector!)
  void synchTo(std::vector<PlotHist1D*>, std::vector<PlotHist2D*>);

  public slots:
  
  void setSymbolSize1D(int);
  void togglePlotStyle1D(bool);

  void setLineWidth1D(int);
  void toggleLineColor1D(bool);
  void toggleBrushColor1D(bool);
  void toggleLineStyle1D(bool);
  void toggleBrushStyle1D(bool);

  void setLineWidth2D(int);
  void toggleLineColor2D(bool);
  void toggleBrushColor2D(bool);
  void toggleLineStyle2D(bool);
  void toggleBrushStyle2D(bool);

 signals:

  void updateManager();

 private:

  class Clockwork;
  Clockwork *c; 

}; 

#endif
