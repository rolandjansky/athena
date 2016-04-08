#ifndef _PlotView_h_
#define _PlotView_h_

#include <QtGui/QGraphicsView>
#include "QatPlotting/AbsPlotter.h"
class QTextEdit;
class AbsRangeDivider;
class PlotView: public QGraphicsView, public AbsPlotter {

  Q_OBJECT

 public:
  
  // Type definitions: 
  enum Style {LINEAR, LOG};  

  // Constructor
  PlotView(QWidget *parent=0);

  // Constructor:
  PlotView(const QRectF * rect, Style xStyle=LINEAR, Style yStyle=LINEAR, bool createRangeDividers=true, QWidget *parent=0);

  // Destructor:
  ~PlotView();

  // Set the Range Divider
  void setXRangeDivider(AbsRangeDivider * xDivider);
 
  // Create or re-create a Range Divider (according to current type)
  AbsRangeDivider *createXRangeDivider(bool set=true);

  // Set the Range Divider
  void setYRangeDivider(AbsRangeDivider * yDivider);

  // Create or re-create Range Divider (according to current type).
  AbsRangeDivider *createYRangeDivider (bool set=true);

  // Resize:
  virtual void resizeEvent (QResizeEvent *event);

  // Get various sorts of plot information:
  // 1. The rectangle, in plot space:
  virtual QRectF * rect(); 
  virtual const QRectF * rect() const;

  // 2. The scene:
  virtual QGraphicsScene* scene();

  // 3. The group where items are placed.
  virtual QGraphicsItemGroup *group();

  // 4. The matrix used in placing those items:
  virtual QMatrix matrix();
 
  // Add a plotable item to this plotter:
  virtual void add (Plotable *);


  // Accessors:
  virtual bool isBox()  const;
  virtual bool isGrid() const;
  virtual bool isXZero() const;
  virtual bool isYZero() const;
  virtual qreal statBoxX() const;
  virtual qreal statBoxY() const;
  virtual bool isLogX() const;
  virtual bool isLogY() const;
  virtual qreal labelXSizePercentage() const;
  virtual qreal labelYSizePercentage() const;

  // Clear the Plots, and the rectangle as well:
  virtual void clear();

  // Get the text edits holding the text labels:
  QTextEdit *titleTextEdit();
  QTextEdit *xLabelTextEdit();
  QTextEdit *yLabelTextEdit();
  QTextEdit *vLabelTextEdit();
  QTextEdit *statTextEdit();

  QFont & xAxisFont() const;
  QFont & yAxisFont() const;

  // Start Animating
  virtual void startAnimating(int frameRateMillisec);

  // Stop Animating 
  virtual void stopAnimating();

  public slots:

  void setGrid(bool flag);
  void setXZero(bool flag);
  void setYZero(bool flag);
  void setLogX(bool flag);
  void setLogY(bool flag);
  void setBox(bool flag);
  void setLabelPos(qreal x, qreal y);
  void setLabelXSizePercentage(qreal perCento); 
  void setLabelYSizePercentage(qreal perCento); 

  // Set the Plot Rectangle (may need synchController as well);
  virtual void setRect(const QRectF & );


  void save (const std::string & filename);

  void updateTitle();
  void updateXLabel();
  void updateYLabel();
  void updateVLabel();
  void updateStat();
  void recreate();

  void labelHome();
  void editXAxisFont();
  void editYAxisFont();
  void copy();
  void save();
  void print();
 

 private:

  // Clockwork:
  class Clockwork;
  Clockwork *c;

  // Illegal operations:
  PlotView & operator = (const PlotView &);
  PlotView(const PlotView &);

  // Create
  void create();

  private slots:

  void animate();

  signals:

  void pointChosen(double x, double y);
  friend class GraphBoxItem;

};


#endif
