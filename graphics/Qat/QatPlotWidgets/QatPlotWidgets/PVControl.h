#ifndef _PVControl_h_
#define _PVControl_h_
#include "QatPlotWidgets/PlotView.h"
#include <QtGui/QDialog>

namespace Ui {
  class PVControl;
}

class PVControl:public QDialog {

  Q_OBJECT

    public:
  PVControl(PlotView * parent, const QRectF &rect);
  ~PVControl();

  QRectF rect();
  PlotView::Style xAxisStyle();
  PlotView::Style yAxisStyle();

 public slots:

  // This one sets the rectangle externally:
  void setRect(const QRectF & rect);
  
  void setXMin(int);
  void setXMax(int);
  void setYMin(int);
  void setYMax(int);
  void setLabelXSizePercentage(int);
  void setLabelYSizePercentage(int);

  void changeXMin ();
  void changeXMax ();
  void changeYMin ();
  void changeYMax ();

signals: 
  
  void rectChanged(const QRectF &);
  void labelHome();
  void editXAxisFont();
  void editYAxisFont();
  void labelXSizePercentageChanged(qreal);
  void labelYSizePercentageChanged(qreal);

 private:

  Ui::PVControl & ui();

  class Clockwork;
  Clockwork *c;

  friend class PlotView;  

};
#endif
