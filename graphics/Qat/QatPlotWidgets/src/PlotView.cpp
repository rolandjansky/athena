#include "QatPlotWidgets/PlotView.h"
#include "QatPlotWidgets/LinearRangeDivider.h"
#include "QatPlotWidgets/LinearSubdivider.h"
#include "QatPlotWidgets/LogSubdivider.h"
#include "QatPlotWidgets/LogRangeDivider.h"
#include "QatPlotting/LinToLog.h"
#include "QatPlotting/Plotable.h"
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsEllipseItem>
#include <QtGui/QGraphicsItemGroup>
#include <QResizeEvent>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QPixmap>
#include <QClipboard>
#include <QTimer>
#include <QFileDialog>
#include <QFontDialog>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QAction>
#include <QPicture>
#include <QSvgGenerator>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <sstream>
#include <memory>
#include "QatPlotWidgets/LabelEditorDialog.h"
#include "QatPlotWidgets/PVControl.h"
#include "ui_PVControl.h"

std::ostream & operator << (std::ostream & o, const QPointF &p ) {
  return o << '(' << p.x() << ' ' << p.y() << ')'; 
}

std::ostream & operator << (std::ostream & o, const QPoint &p ) {
  return o << '(' << p.x() << ' ' << p.y() << ')'; 
}


std::ostream & operator << (std::ostream & o, const QMatrix &m ) {
  return o << '(' << m.m11() 
	   << ' ' 
	   << m.m12() 
	   << ' ' 
	   << m.m21() 
	   << ' ' 
	   << m.m22() 
	   << ' ' 
	   << m.dx() 
	   << ' ' 
	   << m.dy() 
	   << ' '  << ')'; 
}

class SRectF: public QRectF {
public:
  inline SRectF(const QPolygonF & poly) {
    if (poly.size()!=4) {
      throw std::runtime_error("Constructing a rectangle requires 4 point polygon");
    }
    else { 
      double width = poly[1].x()-poly[0].x(), height= (poly[2].y()-poly[0].y());
      setRect(poly[0].x(), poly[0].y(), width, height);
    }
  }
};


class LabelItem:public QGraphicsRectItem{
public:

  LabelItem(const QRectF & rect, QDialog *dialog, bool visible=false):
    QGraphicsRectItem(rect),m_dialog(dialog),m_visible(visible)
  {

    {
      QPen pen;
      pen.setColor(QColor("lightGray"));
      pen.setStyle(m_visible ? Qt::SolidLine : Qt::NoPen);
      defaultPen=pen;
    }
    {
      QPen pen;
      pen.setColor(QColor("darkBlue"));
      pen.setStyle(Qt::SolidLine);
      hoverPen=pen;
    }

    setPen(defaultPen);
    setBrush(defaultBrush);


    setAcceptsHoverEvents(true); 

  }

  void hoverEnterEvent(QGraphicsSceneHoverEvent *) {
    setPen(hoverPen);
    setBrush(hoverBrush);
  }

  void hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
    setPen(defaultPen);
    setBrush(defaultBrush);
  }

  void mousePressEvent(QGraphicsSceneMouseEvent *) {
    if (m_dialog) {
      if (!m_dialog->isVisible()) {
	//QPoint point=rect().topLeft().toPoint();
	m_dialog->move(m_dialog->parentWidget()->pos()+m_dialog->parentWidget()->rect().bottomRight());
	m_dialog->show();
      }
      else 
	{
	  m_dialog->hide();
	}
    }
  }
  bool isVisible() const {return m_visible;}

  QPen     defaultPen, hoverPen;
  QBrush   defaultBrush, hoverBrush;
  

private:
  QDialog *m_dialog;
  bool     m_visible;
};

class GraphBoxItem:public QGraphicsRectItem{
public:

  GraphBoxItem(const QRectF & rect, PVControl *control, PlotView *parent=NULL):
    QGraphicsRectItem(rect),m_popup(new QMenu(parent)), m_control(control),m_pvParent(parent)
  {
    QPen pen;
    pen.setWidth(2);
    pen.setColor(QColor("black"));
    setPen(pen);
    setAcceptsHoverEvents(true); 

    QSize size(150,75);
    m_popup->resize(size);
    m_copyAction=m_popup->addAction("Copy");
    m_saveAction=m_popup->addAction("Save");
    m_printAction=m_popup->addAction("Print");

    QObject::connect (m_printAction, SIGNAL(triggered()), parent, SLOT(print()));
    QObject::connect (m_copyAction, SIGNAL(triggered()),  parent, SLOT(copy()));
    QObject::connect (m_saveAction, SIGNAL(triggered()),  parent, SLOT(save()));
    
  }

  void hoverEnterEvent(QGraphicsSceneHoverEvent *) {
    QPen pen;
    pen.setWidth(2);
    pen.setColor(QColor("darkBlue"));
    setPen(pen);
  }

  void hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
    QPen pen;
    pen.setWidth(2);
    pen.setColor(QColor("black"));
    setPen(pen);
  }

  void mousePressEvent (QGraphicsSceneMouseEvent *e) {
    if (e->buttons()==Qt::RightButton) {
      m_popup->popup(e->widget()->mapToGlobal(e->pos().toPoint()));
    }
    else if (e->buttons()==Qt::MidButton) {
      double slX = (m_pvParent->rect()->right() - m_pvParent->rect()->left())/double(rect().width());
      double slY = (m_pvParent->rect()->top() - m_pvParent->rect()->bottom())/double(rect().height());
      double x = (e->pos().toPoint().x()-rect().left())   * slX + m_pvParent->rect()->left();
      double y = (e->pos().toPoint().y()-rect().bottom()) * slY + m_pvParent->rect()->top();
      m_pvParent->pointChosen(x,y);
    }
    else if (e->buttons()==Qt::LeftButton) {
      m_control->move(m_control->parentWidget()->pos()+m_control->parentWidget()->rect().topRight());
      if (!m_control->isVisible()) {
	m_control->show();
	m_control->raise();
      }
      else {
	m_control->hide();
      }
    }
  }

  void mouseReleaseEvent (QGraphicsSceneMouseEvent *e) {
    if (e->buttons()==Qt::RightButton) m_popup->hide();
  }


private:
  QMenu     *m_popup;
  PVControl *m_control;
  QAction   *m_printAction;
  QAction   *m_copyAction;
  QAction   *m_saveAction;
  PlotView  *m_pvParent;
};

 
class PlotView::Clockwork {


public:


  Clockwork():
    created(false),
    m_control(NULL),
    graphBox(NULL),
    xLabelBox(NULL),             yLabelBox(NULL),             vLabelBox(NULL),             titleBox(NULL),           statBox(NULL),
    xLabelEditorDialog(NULL),    yLabelEditorDialog(NULL),    vLabelEditorDialog(NULL),    titleEditorDialog(NULL),  statEditorDialog(NULL),
    xLabelTextItem(NULL),        yLabelTextItem(NULL),        vLabelTextItem(NULL),        titleTextItem(NULL),      statTextItem(NULL),
    g(NULL),gridLinesGroup(NULL),contentsGroup(NULL),xZeroLine(NULL),yZeroLine(NULL),
    xRangeDivider(NULL),yRangeDivider(NULL), xAllocDivider(NULL), yAllocDivider(NULL),
    xAxisStyle{}, yAxisStyle{},
    labelWidth(50),
    popupMenu(nullptr), timer(nullptr)
  {
      xAxisFont.setPointSize(18);
      xAxisFont.setFamily("Arial");
      yAxisFont.setPointSize(18);
      yAxisFont.setFamily("Arial");
      xPerCento=40;
      yPerCento=40;
  }

  struct TickLabel{
    TickLabel(double pos, QGraphicsTextItem*tI) :pos(pos),tI(tI) {}
    double pos;
    QGraphicsTextItem *tI;
  };
  
  void resizeBoxes(int w, int h) {
    graphBoxTopLeft=QPoint(2*labelWidth, 2*labelWidth);
    graphBoxSize=QSize(w-4*labelWidth, h-4*labelWidth);
    titleBoxTopLeft=QPoint(2*labelWidth,    labelWidth);
    titleBoxSize=QSize(w-4*labelWidth, labelWidth);
    xLabelBoxTopLeft=QPoint(2*labelWidth,   h-labelWidth);
    xLabelBoxSize=QSize(w-4*labelWidth, labelWidth);
    yLabelBoxTopLeft=QPoint(0,                2*labelWidth);
    yLabelBoxSize=QSize(labelWidth, h-4*labelWidth);
    vLabelBoxTopLeft=QPoint(w-labelWidth,     2*labelWidth);
    vLabelBoxSize=QSize(labelWidth, h-4*labelWidth);
    statBoxTopLeft=QPoint(2*labelWidth+(100-xPerCento)*(w-4*labelWidth)/100, 2*labelWidth);
    statBoxSize=QSize(xPerCento*(w-4*labelWidth)/100, yPerCento*(h-4*labelWidth)/100);
  }

  bool                created;
  PVControl           *m_control;
  GraphBoxItem        *graphBox;
  LabelItem           *xLabelBox,          *yLabelBox,          *vLabelBox,          *titleBox,          *statBox;
  LabelEditorDialog   *xLabelEditorDialog, *yLabelEditorDialog, *vLabelEditorDialog, *titleEditorDialog, *statEditorDialog;
  QGraphicsTextItem   *xLabelTextItem,     *yLabelTextItem,     *vLabelTextItem,     *titleTextItem,     *statTextItem;
  QGraphicsItemGroup  *g, *gridLinesGroup, *contentsGroup;
  QGraphicsLineItem   *xZeroLine, *yZeroLine;
  std::vector<TickLabel> xTickLabel, yTickLabel;
  AbsRangeDivider     *xRangeDivider, *yRangeDivider;  // These are not allocated and not owned.
  AbsRangeDivider     *xAllocDivider, *yAllocDivider;  // These area allocated and owned. 

  Style          xAxisStyle;
  Style          yAxisStyle;

  QRectF         rect;
  QGraphicsScene scene;
  QFont          xAxisFont;
  QFont          yAxisFont;
  const int      labelWidth;

  QPoint graphBoxTopLeft;    
  QSize  graphBoxSize;       
  QPoint titleBoxTopLeft;    
  QSize  titleBoxSize;       
  QPoint xLabelBoxTopLeft;   
  QSize  xLabelBoxSize;      
  QPoint yLabelBoxTopLeft;   
  QSize  yLabelBoxSize;      
  QPoint vLabelBoxTopLeft;   
  QSize  vLabelBoxSize;      
  QPoint statBoxTopLeft;    
  QSize  statBoxSize;       

  QMenu *popupMenu;
  QTimer *timer;
  std::vector<Plotable *> plotableList;
  double xPerCento, yPerCento;

};


PlotView::~PlotView() {
  delete m_c->xAllocDivider;
  delete m_c->yAllocDivider;
  delete m_c;
}


PlotView::PlotView( QWidget *parent) :
  QGraphicsView(parent),m_c(new Clockwork()){
  m_c->rect=QRectF(0,0,1,1);
  create();
  setLogX(false);
  setLogY(false);
  createXRangeDivider();
  createYRangeDivider();
  setFrameShape(QFrame::NoFrame);
}

PlotView::PlotView(const QRectF * rect, Style xStyle, Style yStyle, bool createRangeDividers, QWidget *parent) : QGraphicsView(parent),m_c(new Clockwork)
{
  m_c->rect=*rect;
  create();
  setLogX(xStyle==LOG);
  setLogY(yStyle==LOG);
  if (createRangeDividers) {
    createXRangeDivider();
    createYRangeDivider();
  }
  setFrameShape(QFrame::NoFrame);
}

void PlotView::create() {
  // If created, you have to do a big cleanup.
  if (m_c->created) {

    delete m_c->g;
    m_c->xZeroLine=NULL; // Now gone.
    m_c->yZeroLine=NULL; // Now gone.
    
    // Rebuild group and gridLinesGroup:
    m_c->g=new QGraphicsItemGroup();
    m_c->scene.addItem(m_c->g);

    m_c->contentsGroup=new QGraphicsItemGroup();
    m_c->scene.addItem(m_c->contentsGroup);
    m_c->g->addToGroup(m_c->contentsGroup);

    m_c->gridLinesGroup=new QGraphicsItemGroup();
    m_c->scene.addItem(m_c->gridLinesGroup);
    m_c->g->addToGroup(m_c->gridLinesGroup);
    

    for (unsigned int i=0;i<m_c->xTickLabel.size();i++) {
      m_c->scene.removeItem(m_c->xTickLabel[i].tI);
      delete m_c->xTickLabel[i].tI;
			  
    }
    for (unsigned int i=0;i<m_c->yTickLabel.size();i++) {
      m_c->scene.removeItem(m_c->yTickLabel[i].tI);
      delete m_c->yTickLabel[i].tI;
    }
    m_c->xTickLabel.erase(m_c->xTickLabel.begin(),m_c->xTickLabel.end());
    m_c->yTickLabel.erase(m_c->yTickLabel.begin(),m_c->yTickLabel.end());

    int w=width(),h=height();
    m_c->resizeBoxes(w,h);

  }
  else {
    m_c->created=true;

    m_c->timer = new QTimer(this);
    QObject::connect(m_c->timer, SIGNAL(timeout()), this, SLOT(animate()), Qt::QueuedConnection);

    m_c->m_control=new PVControl(this,m_c->rect);
    setScene(&m_c->scene);
    m_c->g=new QGraphicsItemGroup();
    m_c->scene.addItem(m_c->g);
    m_c->gridLinesGroup=new QGraphicsItemGroup();
    m_c->scene.addItem(m_c->gridLinesGroup);
    m_c->g->addToGroup(m_c->gridLinesGroup);

    m_c->contentsGroup=new QGraphicsItemGroup();
    m_c->scene.addItem(m_c->contentsGroup);
    m_c->g->addToGroup(m_c->contentsGroup);


    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setMinimumSize(700,500);
    m_c->xAxisStyle=m_c->m_control->xAxisStyle();
    m_c->yAxisStyle=m_c->m_control->yAxisStyle();

    m_c->xLabelEditorDialog = new LabelEditorDialog(this); m_c->xLabelEditorDialog->textEdit()->setAlignment(Qt::AlignHCenter);
    m_c->yLabelEditorDialog = new LabelEditorDialog(this); m_c->yLabelEditorDialog->textEdit()->setAlignment(Qt::AlignHCenter);
    m_c->vLabelEditorDialog = new LabelEditorDialog(this); m_c->vLabelEditorDialog->textEdit()->setAlignment(Qt::AlignHCenter);
    m_c->titleEditorDialog  = new LabelEditorDialog(this); m_c->titleEditorDialog->textEdit()->setAlignment(Qt::AlignHCenter);
    m_c->statEditorDialog   = new LabelEditorDialog(this); 

    QSize    screenDimensions(700,500);

    setDragMode(NoDrag);
    //    setRenderHint(QPainter::Antialiasing);
    // setCacheMode(QGraphicsView::CacheBackground);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    SRectF sceneRect=mapToScene(0,0,700,500);

    setSceneRect(sceneRect);
    resize(screenDimensions.width(),screenDimensions.height());
    
    int w=screenDimensions.width()/*-5*/,h=screenDimensions.height()/*-5*/;
    m_c->resizeBoxes(w,h);

    m_c->statBox=new LabelItem(QRect(m_c->statBoxTopLeft,m_c->statBoxSize),m_c->statEditorDialog); 
    {
      QPen pen;
      pen.setStyle(Qt::NoPen);
      m_c->statBox->defaultPen=pen;
    }
    m_c->scene.addItem(m_c->statBox);

    m_c->graphBox=new GraphBoxItem(QRect(m_c->graphBoxTopLeft,m_c->graphBoxSize),m_c->m_control, this);
    m_c->scene.addItem(m_c->graphBox);
    m_c->titleBox=new LabelItem(QRect(m_c->titleBoxTopLeft,m_c->titleBoxSize),m_c->titleEditorDialog); 
    m_c->scene.addItem(m_c->titleBox);
    m_c->xLabelBox=new LabelItem(QRect(m_c->xLabelBoxTopLeft,m_c->xLabelBoxSize),m_c->xLabelEditorDialog);    
    m_c->scene.addItem(m_c->xLabelBox);
    m_c->yLabelBox=new LabelItem(QRect(m_c->yLabelBoxTopLeft,m_c->yLabelBoxSize),m_c->yLabelEditorDialog);
    m_c->scene.addItem(m_c->yLabelBox);
    m_c->vLabelBox=new LabelItem(QRect(m_c->vLabelBoxTopLeft,m_c->vLabelBoxSize),m_c->vLabelEditorDialog);
    m_c->scene.addItem(m_c->vLabelBox);

    //Make the stat box movable; set boundary to zero.
    m_c->statBox->setFlag(QGraphicsItem::ItemIsMovable);
    {
      QPen pen;
      pen.setStyle(Qt::NoPen);
      m_c->statBox->defaultPen=pen;
    }
    
    // Workaround to Qt Bug # 138596 (Qt 4.2.1)
    m_c->titleEditorDialog->textEdit()->document()->setTextWidth(m_c->titleBoxSize.width());
    m_c->xLabelEditorDialog->textEdit()->document()->setTextWidth(m_c->xLabelBoxSize.width());
    m_c->yLabelEditorDialog->textEdit()->document()->setTextWidth(m_c->yLabelBoxSize.width());
    m_c->vLabelEditorDialog->textEdit()->document()->setTextWidth(m_c->vLabelBoxSize.width());
    m_c->statEditorDialog->textEdit()->document()->setTextWidth(m_c->statBoxSize.width());


    m_c->titleEditorDialog->textEdit()->setFixedWidth((int) m_c->titleBox->rect().width());
    m_c->titleEditorDialog->setFixedWidth(m_c->titleEditorDialog->childrenRect().width());
    
    m_c->xLabelEditorDialog->textEdit()->setFixedWidth((int) m_c->xLabelBox->rect().width());
    m_c->xLabelEditorDialog->setFixedWidth(m_c->xLabelEditorDialog->childrenRect().width());
    
    m_c->yLabelEditorDialog->textEdit()->setFixedWidth((int) m_c->yLabelBox->rect().height());
    m_c->yLabelEditorDialog->setFixedWidth(m_c->yLabelEditorDialog->childrenRect().width());
    
    m_c->vLabelEditorDialog->textEdit()->setFixedWidth((int) m_c->vLabelBox->rect().height());
    m_c->vLabelEditorDialog->setFixedWidth(m_c->vLabelEditorDialog->childrenRect().width());
    
    m_c->statEditorDialog->textEdit()->setFixedWidth((int) m_c->statBox->rect().width());
    m_c->statEditorDialog->textEdit()->setFixedHeight((int) m_c->statBox->rect().height());
    // end workaround


    m_c->statEditorDialog->textEdit()->document()->setTextWidth(m_c->statBoxSize.width());
    m_c->statEditorDialog->textEdit()->setWordWrapMode(QTextOption::WrapAnywhere);
    m_c->statEditorDialog->textEdit()->setLineWrapMode(QTextEdit::FixedPixelWidth);
    
    
    QFont font;
    font.setPointSize(24);
    font.setFamily("Arial");
    
    
    m_c->titleTextItem = new QGraphicsTextItem(m_c->titleBox);
    m_c->titleTextItem->setFont(font);
    //m_c->scene.addItem(m_c->titleTextItem);
    
    m_c->xLabelTextItem = new QGraphicsTextItem(m_c->xLabelBox);
    m_c->xLabelTextItem->setFont(font);
    //m_c->scene.addItem(m_c->xLabelTextItem);
    
    m_c->yLabelTextItem = new QGraphicsTextItem(m_c->yLabelBox);
    m_c->yLabelTextItem->setFont(font);
    //m_c->scene.addItem(m_c->yLabelTextItem);
    
    m_c->vLabelTextItem = new QGraphicsTextItem(m_c->vLabelBox);
    m_c->vLabelTextItem->setFont(font);
    //m_c->scene.addItem(m_c->vLabelTextItem);
    
    m_c->statTextItem = new QGraphicsTextItem(m_c->statBox);
    m_c->statTextItem->setFont(font);
    //m_c->scene.addItem(m_c->statTextItem);
    m_c->statBox->setFlag(QGraphicsItem::ItemIsSelectable);
    
    connect(m_c->m_control, SIGNAL(rectChanged(const QRectF &)), this, SLOT(setRect (const QRectF &)));
    
    connect(m_c->titleEditorDialog->textEdit(),SIGNAL(textChanged()),this, SLOT(updateTitle()));
    connect(m_c->xLabelEditorDialog->textEdit(),SIGNAL(textChanged()),this, SLOT(updateXLabel()));
    connect(m_c->yLabelEditorDialog->textEdit(),SIGNAL(textChanged()),this, SLOT(updateYLabel()));
    connect(m_c->vLabelEditorDialog->textEdit(),SIGNAL(textChanged()),this, SLOT(updateVLabel()));
    connect(m_c->statEditorDialog->textEdit(),SIGNAL(textChanged()),this, SLOT(updateStat()));
   
    connect(m_c->m_control, SIGNAL(labelHome()), this, SLOT(labelHome()));
    connect(m_c->m_control, SIGNAL(editXAxisFont()), this, SLOT(editXAxisFont()));
    connect(m_c->m_control, SIGNAL(editYAxisFont()), this, SLOT(editYAxisFont()));

    connect(m_c->m_control, SIGNAL(labelXSizePercentageChanged(qreal)), this, SLOT(setLabelXSizePercentage(qreal)));
    connect(m_c->m_control, SIGNAL(labelYSizePercentageChanged(qreal)), this, SLOT(setLabelYSizePercentage(qreal)));
    m_c->popupMenu=new QMenu(this);
    
  }


  // Label the x axes:
  if (m_c->xRangeDivider) m_c->xRangeDivider->setRange(m_c->rect.left(), m_c->rect.right());
  if (m_c->yRangeDivider) m_c->yRangeDivider->setRange(m_c->rect.top(), m_c->rect.bottom());
 
  LinToLog *toLogY= m_c->yAxisStyle==LOG ? new LinToLog (m_c->rect.top(),m_c->rect.bottom()) : NULL;
  LinToLog *toLogX= m_c->xAxisStyle==LOG ? new LinToLog (m_c->rect.left(),m_c->rect.right()) : NULL;
  // Master transformer:
  QMatrix m;
  m.scale(m_c->graphBoxSize.width()/m_c->rect.width(),-m_c->graphBoxSize.height()/m_c->rect.height());
  m.translate(-m_c->rect.bottomLeft().x(),  -m_c->rect.bottomLeft().y());
  // End master transformer..

  if (m_c->xRangeDivider) {
    for (int ix=0;ix<m_c->xRangeDivider->getNumSubdivisions();ix++) {
      if (!m_c->xRangeDivider->isValid(ix)) continue;

      std::ostringstream x_label;
      double xValue=m_c->xRangeDivider->getSubdivision(ix).x();
      double yValue=m_c->rect.top();
      double wValue=m_c->rect.bottom();
      if (toLogX) xValue = (*toLogX)(xValue);
      
      

      QPointF X (xValue,yValue);
      QPointF XP(xValue,yValue+m_c->rect.height()/32.0);
      QGraphicsLineItem *line=new QGraphicsLineItem();
      line->setLine(QLineF(X,XP));
      m_c->scene.addItem(line);
      m_c->g->addToGroup(line);
      // Again (along the top).
      {      
	QPointF X (xValue,wValue);
	QPointF XP(xValue,wValue-m_c->rect.height()/32.0);
	QGraphicsLineItem *line=new QGraphicsLineItem();
	line->setLine(QLineF(X,XP));
	m_c->scene.addItem(line);
	m_c->g->addToGroup(line);
      }      
      // More (subdivisions, linear:):
      if (!isLogX()) {
	int nPass=ix==0 ? 2:1;
	for (int n=0;n<nPass;n++) {
	  std::auto_ptr<AbsRangeDivider> subdivider(new LinearSubdivider());
	  double xValue=m_c->xRangeDivider->getSubdivision(ix).x();
	  double xPlusValue= ix!=m_c->xRangeDivider->getNumSubdivisions()-1 ? m_c->xRangeDivider->getSubdivision(ix+1).x(): 2*xValue-m_c->xRangeDivider->getSubdivision(ix-1).x();
	    // Works for linear. 

	  // This is a second pass:
	  if (n==1) {
	    if (ix==0) {
	      double interval=xPlusValue-xValue;
	      xPlusValue=xValue;
	      xValue = xValue-interval;
	    }
	  }

	  subdivider->setRange(xValue,xPlusValue);
	  for (int i=0;i<subdivider->getNumSubdivisions();i++) {
	    //	    if (!subdivider->isValid(i)) continue;
	    double xValue=subdivider->getSubdivision(i).x(); // Shadows!!
	    if (xValue<m_c->rect.left() || xValue>m_c->rect.right()) continue;
	    {
	      QPointF X (xValue,yValue);
	      QPointF XP(xValue,yValue+m_c->rect.height()/64.0);
	      QGraphicsLineItem *line=new QGraphicsLineItem();
	      line->setLine(QLineF(X,XP));
	      m_c->scene.addItem(line);
	      m_c->g->addToGroup(line);
	    }
	    {
	      QPointF X (xValue,wValue);
	      QPointF XP(xValue,wValue-m_c->rect.height()/64.0);
	      QGraphicsLineItem *line=new QGraphicsLineItem();
	      line->setLine(QLineF(X,XP));
	      m_c->scene.addItem(line);
	      m_c->g->addToGroup(line);
	    }
	  }
	}
      }
      else { // Log X
	int nPass=ix==0 || ix == m_c->xRangeDivider->getNumSubdivisions()-1? 2:1;
	for (int n=0;n<nPass;n++) {
	  std::auto_ptr<AbsRangeDivider> subdivider(new LogSubdivider());
	  double xValue=m_c->xRangeDivider->getSubdivision(ix).x();
	  double xPlusValue= ix!=m_c->xRangeDivider->getNumSubdivisions()-1 ? m_c->xRangeDivider->getSubdivision(ix+1).x(): 2*xValue-m_c->xRangeDivider->getSubdivision(ix-1).x();
	  if (n==1) {
	    if (ix==0) {
	      xPlusValue=xValue;
	      xValue=m_c->rect.left();
	    }
	    else {
	      xValue=xPlusValue;
	      xPlusValue=m_c->rect.right();
	    }
	  }

	  subdivider->setRange(xValue,xPlusValue);
	  for (int i=0;i<subdivider->getNumSubdivisions();i++) {
	    double xValue=subdivider->getSubdivision(i).x(); // Shadows!!
	    if (toLogX) xValue = (*toLogX)(xValue);
	    if (xValue<m_c->rect.left() || xValue>m_c->rect.right()) continue;
	    {
	      QPointF X (xValue,yValue);
	      QPointF XP(xValue,yValue+m_c->rect.height()/64.0);
	      QGraphicsLineItem *line=new QGraphicsLineItem();
	      line->setLine(QLineF(X,XP));
	      m_c->scene.addItem(line);
	      m_c->g->addToGroup(line);
	    }
	    {
	      QPointF X (xValue,wValue);
	      QPointF XP(xValue,wValue-m_c->rect.height()/64.0);
	      QGraphicsLineItem *line=new QGraphicsLineItem();
	      line->setLine(QLineF(X,XP));
	      m_c->scene.addItem(line);
	      m_c->g->addToGroup(line);
	    }
	  }
	}
      }

      QGraphicsTextItem *anon = new QGraphicsTextItem();
      anon->setDocument(m_c->xRangeDivider->getSubdivision(ix).label());
      anon->setFont(m_c->xAxisFont);
      anon->adjustSize();
      m_c->scene.addItem(anon);
      QRect SBR=anon->sceneBoundingRect().toRect();
      anon->setPos(m.map(X)+m_c->graphBoxTopLeft-QPoint(SBR.width()/2,0));
      m_c->xTickLabel.push_back(Clockwork::TickLabel(xValue,anon));
      
      {
	QGraphicsLineItem *line=new QGraphicsLineItem(QLineF(QPointF(xValue,m_c->rect.top()),QPointF(xValue,m_c->rect.bottom())));
	QPen  pen=line->pen();
	pen.setStyle(Qt::DotLine);
	pen.setColor(QColor("lightGray"));
	line->setPen(pen);
	m_c->scene.addItem(line);
	m_c->gridLinesGroup->addToGroup(line);
      }
    }
  }
  if (m_c->yRangeDivider) {
    for (int iy=0;iy<m_c->yRangeDivider->getNumSubdivisions();iy++) {
      if (!m_c->yRangeDivider->isValid(iy)) continue;

      std::ostringstream y_label;
      double yValue = m_c->yRangeDivider->getSubdivision(iy).x();
      double xValue = m_c->rect.left();
      double wValue = m_c->rect.right();
      if (toLogY) yValue = (*toLogY)(yValue);
       
      
      QPointF  X(xValue,yValue);
      QPointF  XP(xValue+m_c->rect.width()/32.0,yValue);
      QGraphicsLineItem *line=new QGraphicsLineItem();
      line->setLine(QLineF(X,XP));
      m_c->scene.addItem(line);
      m_c->g->addToGroup(line);
      // Again:
      {
	QPointF  X(wValue,yValue);
	QPointF  XP(wValue-m_c->rect.width()/32.0,yValue);
	QGraphicsLineItem *line=new QGraphicsLineItem();
	line->setLine(QLineF(X,XP));
	m_c->scene.addItem(line);
	m_c->g->addToGroup(line);
      }
      // More (subdivisions, linear:):
      if (!isLogY()) {
	int nPass=iy==0 ? 2:1;
	for (int n=0;n<nPass;n++) {
	  std::auto_ptr<AbsRangeDivider> subdivider(new LinearSubdivider());
	  double yValue=m_c->yRangeDivider->getSubdivision(iy).x();
	  double yPlusValue= iy!=m_c->yRangeDivider->getNumSubdivisions()-1 ? m_c->yRangeDivider->getSubdivision(iy+1).x(): 2*yValue-m_c->yRangeDivider->getSubdivision(iy-1).x();
	    // Works for linear. 

	  // This is a second pass:
	  if (n==1) {
	    if (iy==0) {
	      double interval=yPlusValue-yValue;
	      yPlusValue=yValue;
	      yValue = yValue-interval;
	    }
	  }

	  subdivider->setRange(yValue,yPlusValue);
	  for (int i=0;i<subdivider->getNumSubdivisions();i++) {
	    //	    if (!subdivider->isValid(i)) continue;
	    double yValue=subdivider->getSubdivision(i).x(); // Shadows!!
	    if (yValue<m_c->rect.top() || yValue>m_c->rect.bottom()) continue;
	    {
	      QPointF X (xValue,yValue);
	      QPointF XP(xValue+m_c->rect.width()/64.0,yValue);
	      QGraphicsLineItem *line=new QGraphicsLineItem();
	      line->setLine(QLineF(X,XP));
	      m_c->scene.addItem(line);
	      m_c->g->addToGroup(line);
	    }
	    {
	      QPointF X (wValue,yValue);
	      QPointF XP(wValue-m_c->rect.width()/64.0,yValue);
	      QGraphicsLineItem *line=new QGraphicsLineItem();
	      line->setLine(QLineF(X,XP));
	      m_c->scene.addItem(line);
	      m_c->g->addToGroup(line);
	    }
	  }
	}
      }
      else { // Log Y
	int nPass=iy==0 || iy == m_c->yRangeDivider->getNumSubdivisions()-1? 2:1;
	for (int n=0;n<nPass;n++) {
	  std::auto_ptr<AbsRangeDivider> subdivider(new LogSubdivider());
	  double yValue=m_c->yRangeDivider->getSubdivision(iy).x();
	  double yPlusValue= iy!=m_c->yRangeDivider->getNumSubdivisions()-1 ? m_c->yRangeDivider->getSubdivision(iy+1).x(): 2*yValue-m_c->yRangeDivider->getSubdivision(iy-1).x();
	  if (n==1) {
	    if (iy==0) {
	      yPlusValue=yValue;
	      yValue=m_c->rect.top();
	    }
	    else {
	      yValue=yPlusValue;
	      yPlusValue=m_c->rect.bottom();
	    }
	  }

	  subdivider->setRange(yValue,yPlusValue);
	  for (int i=0;i<subdivider->getNumSubdivisions();i++) {
	    double yValue=subdivider->getSubdivision(i).x(); // Shadows!!
	    if (toLogY) yValue = (*toLogY)(yValue);
	    if (yValue<m_c->rect.top() || yValue>m_c->rect.bottom()) continue;
	    {
	      QPointF X (xValue,yValue);
	      QPointF XP(xValue+m_c->rect.width()/64.0,yValue);
	      QGraphicsLineItem *line=new QGraphicsLineItem();
	      line->setLine(QLineF(X,XP));
	      m_c->scene.addItem(line);
	      m_c->g->addToGroup(line);
	    }
	    {
	      QPointF X (wValue,yValue);
	      QPointF XP(wValue-m_c->rect.width()/64.0,yValue);
	      QGraphicsLineItem *line=new QGraphicsLineItem();
	      line->setLine(QLineF(X,XP));
	      m_c->scene.addItem(line);
	      m_c->g->addToGroup(line);
	    }
	  }
	}
      }
      
      QGraphicsTextItem *anon = new QGraphicsTextItem();
      anon->setDocument(m_c->yRangeDivider->getSubdivision(iy).label());
      anon->setFont(m_c->yAxisFont);
      anon->adjustSize();
      m_c->scene.addItem(anon);
      QRect SBR=anon->sceneBoundingRect().toRect();
      anon->setPos(m.map(X).toPoint()+m_c->graphBoxTopLeft-QPoint(2*SBR.width()/2+10,SBR.height()/2));
      m_c->yTickLabel.push_back(Clockwork::TickLabel(yValue,anon));
      
      {
	QGraphicsLineItem *line=new QGraphicsLineItem(QLineF(QPointF(m_c->rect.left(),yValue),QPointF(m_c->rect.right(),yValue)));
	QPen  pen=line->pen();
	pen.setStyle(Qt::DotLine);
	pen.setColor(QColor("lightGray"));
	line->setPen(pen);
	m_c->scene.addItem(line);
	m_c->gridLinesGroup->addToGroup(line);
      }
      
    }
  }
  if (m_c->rect.contains(0,m_c->rect.center().y())) 
  {
    m_c->xZeroLine=new QGraphicsLineItem(QLineF(QPointF(0,m_c->rect.bottom()),QPointF(0,m_c->rect.top())));
    QPen  pen=m_c->xZeroLine->pen();
    pen.setColor(QColor("lightGray"));
    m_c->xZeroLine->setPen(pen);
    m_c->scene.addItem(m_c->xZeroLine);
    m_c->g->addToGroup(m_c->xZeroLine);
  }

  if (m_c->rect.contains(m_c->rect.center().x(), 0)) 
  {
    m_c->yZeroLine=new QGraphicsLineItem(QLineF(QPointF(m_c->rect.right(),0),QPointF(m_c->rect.left(),0)));
    QPen  pen=m_c->yZeroLine->pen();
    pen.setColor(QColor("lightGray"));
    m_c->yZeroLine->setPen(pen);
    m_c->scene.addItem(m_c->yZeroLine);
    m_c->g->addToGroup(m_c->yZeroLine);
  }

  //=============================HERE IS A PLACE TO PLAY!!!
  // 
  // Scene
  // ContentsGroup
  // Rect
  // Matrix and inverse

  for (unsigned int i=0;i<m_c->plotableList.size();i++) {
    m_c->plotableList[i]->describeYourselfTo(this);
  }
  //=============================END OF PLAYGROUND

  // Now go through the contents group and order everything!
  QList<QGraphicsItem *> contentsList =m_c->contentsGroup->children();
  QList<QGraphicsItem *> gridLinesList=m_c->gridLinesGroup->children();
  QList<QGraphicsItem *> gList        =m_c->g->children();


  for (int i=0;i<contentsList.size();i++) {
    contentsList.at(i)->setZValue(i);
  }
  for (int i=0;i<gridLinesList.size();i++) {
    gridLinesList.at(i)->setZValue(i);
  }
  for (int i=0;i<gList.size();i++) {
    gList.at(i)->setZValue(i);
  }
  m_c->graphBox->setZValue(0);
  m_c->g->setZValue(1);
  m_c->vLabelBox->setZValue(2);
  m_c->vLabelTextItem->setZValue(3);
  m_c->titleBox->setZValue(4);
  m_c->titleTextItem->setZValue(5);
  m_c->xLabelBox->setZValue(6);
  m_c->xLabelTextItem->setZValue(7);
  m_c->yLabelBox->setZValue(8);
  m_c->yLabelTextItem->setZValue(9);
  m_c->statBox->setZValue(10);    
  m_c->statTextItem->setZValue(11);    

  delete toLogY;
  delete toLogX;

  m_c->g->setMatrix(m);
  m_c->g->setPos(m_c->graphBoxTopLeft);   

  setGrid(m_c->m_control->ui().gridCheckBox->isChecked());
  setXZero(m_c->m_control->ui().xZeroCheckBox->isChecked());
  setYZero(m_c->m_control->ui().yZeroCheckBox->isChecked());  
}


void PlotView::resizeEvent (QResizeEvent * /*event*/) { //unused parameter
  // This is here to prevent funny sizes appearing during resize:
  create();

  //QSize size=event->size(); // size not used anymore...
  //int h=size.height();
  //int w=size.width();

  int h=height();
  int w=width();

  m_c->resizeBoxes(w,h);


  QMatrix m;
  m.scale(m_c->graphBoxSize.width()/m_c->rect.width(),-m_c->graphBoxSize.height()/m_c->rect.height());
  m.translate(-m_c->rect.bottomLeft().x(),  -m_c->rect.bottomLeft().y());
  
  m_c->g->setMatrix(m);
  m_c->g->setPos(m_c->graphBoxTopLeft);

  m_c->graphBox->setRect(QRect(m_c->graphBoxTopLeft,m_c->graphBoxSize));

  m_c->titleBox ->setRect(QRect(m_c->titleBoxTopLeft,m_c->titleBoxSize)); 
  m_c->titleTextItem->setPos(m_c->titleBox->sceneBoundingRect().topLeft());

  m_c->xLabelBox->setRect(QRect(m_c->xLabelBoxTopLeft,m_c->xLabelBoxSize));    
  m_c->xLabelTextItem->setPos(m_c->xLabelBox->sceneBoundingRect().topLeft());

  m_c->yLabelBox->setRect(QRect(m_c->yLabelBoxTopLeft,m_c->yLabelBoxSize));
  m_c->yLabelTextItem->setPos(m_c->yLabelBox->sceneBoundingRect().bottomLeft());

  m_c->vLabelBox->setRect(QRect(m_c->vLabelBoxTopLeft,m_c->vLabelBoxSize));
  m_c->vLabelTextItem->setPos(m_c->vLabelBox->sceneBoundingRect().bottomLeft());

  m_c->statBox ->setRect(QRect(m_c->statBoxTopLeft,m_c->statBoxSize)); 
  m_c->statTextItem->setPos(m_c->statBox->sceneBoundingRect().topLeft()-m_c->statBox->pos());

  for (unsigned int i=0;i<m_c->yTickLabel.size();i++) {
    QGraphicsTextItem *anon=m_c->yTickLabel[i].tI;
    double yValue=m_c->yTickLabel[i].pos;
    double xValue=m_c->rect.left();
    QPointF X(xValue,yValue);
    QRect SBR=anon->sceneBoundingRect().toRect();
    anon->setPos( m.map(X).toPoint()+m_c->graphBoxTopLeft-QPoint(2*SBR.width()/2+10,SBR.height()/2));
  }

  for (unsigned int i=0;i<m_c->xTickLabel.size();i++) {
    QGraphicsTextItem *anon=m_c->xTickLabel[i].tI;
    double xValue=m_c->xTickLabel[i].pos;
    double yValue=m_c->rect.top();
    QPointF X(xValue,yValue);
    QRect SBR=anon->sceneBoundingRect().toRect();
    anon->setPos(m.map(X)+m_c->graphBoxTopLeft-QPoint(SBR.width()/2,0));
  }

  // Workaround to Qt Bug # 138596 (Qt 4.2.1)
  if (1) {
    m_c->titleTextItem->setTextWidth(m_c->titleBox->rect().width());
    m_c->xLabelTextItem->setTextWidth(m_c->xLabelBox->rect().width());
    m_c->yLabelTextItem->setTextWidth(m_c->yLabelBox->rect().height());
    m_c->vLabelTextItem->setTextWidth(m_c->vLabelBox->rect().height());
    m_c->statTextItem->setTextWidth(m_c->statBox->rect().width());
    
    
    m_c->titleEditorDialog->textEdit()->setFixedWidth((int) m_c->titleBox->rect().width());
    m_c->titleEditorDialog->setFixedWidth(m_c->titleEditorDialog->childrenRect().width());
    
    m_c->xLabelEditorDialog->textEdit()->setFixedWidth((int) m_c->xLabelBox->rect().width());
    m_c->xLabelEditorDialog->setFixedWidth(m_c->xLabelEditorDialog->childrenRect().width());
    
    m_c->yLabelEditorDialog->textEdit()->setFixedWidth((int) m_c->yLabelBox->rect().height());
    m_c->yLabelEditorDialog->setFixedWidth(m_c->yLabelEditorDialog->childrenRect().width());
    
    m_c->vLabelEditorDialog->textEdit()->setFixedWidth((int) m_c->vLabelBox->rect().height());
    m_c->vLabelEditorDialog->setFixedWidth(m_c->vLabelEditorDialog->childrenRect().width());
    
    m_c->statEditorDialog->textEdit()->setFixedWidth((int) m_c->statBox->rect().width());
    m_c->statEditorDialog->textEdit()->setFixedHeight((int) m_c->statBox->rect().height());
  }

  setGrid(m_c->m_control->ui().gridCheckBox->isChecked());
  setXZero(m_c->m_control->ui().xZeroCheckBox->isChecked());
  setYZero(m_c->m_control->ui().yZeroCheckBox->isChecked());
}

// Draw Primitives into the plotter:
// Set the Plot Rectangle
void PlotView::setRect(const QRectF & R){
  if (R.top()<=0) {
    if (isLogY()) throw std::runtime_error("Ymin<=0 requested with logarithmic scale");
  }
  if (R.left()<=0) {
    if (isLogX()) throw std::runtime_error("Xmin<=0 requested with logarithmic scale");
  }
  m_c->rect=R;
  create();
  resize(QWidget::size());
  m_c->m_control->blockSignals(true);
  m_c->m_control->setRect(R);
  m_c->m_control->blockSignals(false);
}


// Get the Plot Rectangle:
QRectF * PlotView::rect() {
  return &m_c->rect;
}

const QRectF * PlotView::rect() const {
  return &m_c->rect;
}

// Clear the Plots
void PlotView::clear() {
  stopAnimating();
  m_c->plotableList.erase(m_c->plotableList.begin(),m_c->plotableList.end());
  create();
}


void PlotView::setGrid(bool flag){
  if (flag) m_c->gridLinesGroup->show(); else m_c->gridLinesGroup->hide();
  m_c->scene.update(m_c->scene.sceneRect());
  m_c->m_control->blockSignals(true);
  m_c->m_control->ui().gridCheckBox->setChecked(flag);
  m_c->m_control->blockSignals(false);

}
void PlotView::setXZero(bool flag){
  if (m_c->xZeroLine) {
    if (flag) m_c->xZeroLine->show(); else m_c->xZeroLine->hide();
    m_c->scene.update(m_c->scene.sceneRect());
  }
  m_c->m_control->blockSignals(true);
  m_c->m_control->ui().xZeroCheckBox->setChecked(flag);
  m_c->m_control->blockSignals(false);
}
void PlotView::setYZero(bool flag){
  if (m_c->yZeroLine) {
    if (flag) m_c->yZeroLine->show(); else m_c->yZeroLine->hide();
    m_c->scene.update(m_c->scene.sceneRect());
  }
  m_c->m_control->blockSignals(true);
  m_c->m_control->ui().yZeroCheckBox->setChecked(flag);
  m_c->m_control->blockSignals(false);
}
void PlotView::setLogX(bool flag){
  if (flag==LOG && m_c->rect.left()<=0) {
    throw std::runtime_error("Logarithmic scale requested where xmin<=0");
  }
  m_c->xAxisStyle=flag ? LOG:LINEAR;

  if (m_c->xRangeDivider && m_c->xRangeDivider==m_c->xAllocDivider) createXRangeDivider();
  m_c->m_control->blockSignals(true);
  m_c->m_control->ui().logXCheckBox->setChecked(flag);
  m_c->m_control->blockSignals(false);
  create();
}

void PlotView::setLogY(bool flag){
  if (flag==LOG && m_c->rect.top()<=0) {
    throw std::runtime_error("Logarithmic scale requested where ymin<=0");
  }
  m_c->yAxisStyle=flag ? LOG:LINEAR;
  if (m_c->yRangeDivider && m_c->yRangeDivider==m_c->yAllocDivider) createYRangeDivider();
  m_c->m_control->blockSignals(true);
  m_c->m_control->ui().logYCheckBox->setChecked(flag);
  m_c->m_control->blockSignals(false);

  create();
}

bool PlotView::isLogY() const {
  return m_c->yAxisStyle==LOG;
}

bool PlotView::isLogX() const {
  return m_c->xAxisStyle==LOG;
}

void PlotView::setBox(bool flag){ 
  
  if (flag) {
    m_c->statBox->show();
    m_c->statTextItem->show();
  }
  else {
    m_c->statBox->hide();
    m_c->statTextItem->hide();
  }
  {
    if (QObject::sender()==m_c->m_control->ui().labelBoxBox) m_c->statBox->setSelected(true);
  }
  m_c->m_control->blockSignals(true);
  m_c->m_control->ui().labelBoxBox->setChecked(flag);
  m_c->m_control->blockSignals(false);
}


QTextEdit *PlotView::titleTextEdit() {return m_c->titleEditorDialog->textEdit();}
QTextEdit *PlotView::xLabelTextEdit(){return m_c->xLabelEditorDialog->textEdit();}
QTextEdit *PlotView::yLabelTextEdit(){return m_c->yLabelEditorDialog->textEdit();}
QTextEdit *PlotView::vLabelTextEdit(){return m_c->vLabelEditorDialog->textEdit();}
QTextEdit *PlotView::statTextEdit() {return m_c->statEditorDialog->textEdit();}

void PlotView::updateTitle() {
  QTextEdit *textEdit=m_c->titleEditorDialog->textEdit();
  QTextDocument *document=textEdit->document();
  m_c->titleTextItem->setDocument(document);
  QPointF q=m_c->titleBox->sceneBoundingRect().topLeft();
  m_c->titleTextItem->setPos(q);
  m_c->titleTextItem->setTextWidth(m_c->titleBox->rect().width()); ///!!! HERE
  m_c->titleTextItem->update();
}

void PlotView::updateStat() {
  QTextEdit *textEdit=m_c->statEditorDialog->textEdit();
  QTextDocument *document=textEdit->document();
  m_c->statTextItem->setDocument(document);
  QPointF q=m_c->statBox->sceneBoundingRect().topLeft()-m_c->statBox->pos();
  m_c->statTextItem->setPos(q);
  m_c->statTextItem->update();
  m_c->statTextItem->setTextWidth(m_c->statBox->rect().width());
}


void PlotView::updateXLabel() {
  QTextEdit *textEdit=m_c->xLabelEditorDialog->textEdit();
  QTextDocument *document=textEdit->document();
  m_c->xLabelTextItem->setDocument(document);
  QPointF q=m_c->xLabelBox->sceneBoundingRect().topLeft();
  m_c->xLabelTextItem->setPos(q);
  m_c->xLabelTextItem->setTextWidth(m_c->xLabelBox->rect().width());
  m_c->xLabelTextItem->update();
}

void PlotView::updateYLabel() {
  QTextEdit *textEdit=m_c->yLabelEditorDialog->textEdit();
  QTextDocument *document=textEdit->document();
  m_c->yLabelTextItem->setDocument(document);

  QPointF q=m_c->yLabelBox->sceneBoundingRect().bottomLeft();
  QMatrix M;
  M.rotate(-90);
  m_c->yLabelTextItem->setMatrix(M);
  m_c->yLabelTextItem->setPos(q);
  m_c->yLabelTextItem->update();

}

void PlotView::updateVLabel() {
  QTextEdit *textEdit=m_c->vLabelEditorDialog->textEdit();
  QTextDocument *document=textEdit->document();
  m_c->vLabelTextItem->setDocument(document);
  QPointF q=m_c->vLabelBox->sceneBoundingRect().bottomLeft();
  QMatrix M;
  M.rotate(-90);
  m_c->vLabelTextItem->setMatrix(M);
  m_c->vLabelTextItem->setPos(q);
  m_c->vLabelTextItem->update();

}


void PlotView::copy() {

  QPen xp=m_c->xLabelBox->pen();
  QPen yp=m_c->xLabelBox->pen();
  QPen vp=m_c->xLabelBox->pen();
  QPen tp =m_c->titleBox->pen();
  //SM QPen gp =m_c->graphBox->pen();
  
  xp.setStyle(Qt::NoPen);
  yp.setStyle(Qt::NoPen);
  vp.setStyle(Qt::NoPen);
  tp.setStyle(Qt::NoPen);
  //SM gp.setStyle(Qt::NoPen);

  m_c->xLabelBox->setPen(xp);
  m_c->yLabelBox->setPen(yp);
  m_c->vLabelBox->setPen(vp);
  m_c->titleBox->setPen(tp);
  //SM m_c->graphBox->setPen(gp);
 

  QPixmap px = QPixmap::grabWidget(this);
  QClipboard *cb = QApplication::clipboard();
  cb->setPixmap(px);

  if (m_c->xLabelBox->isVisible()) xp.setStyle(Qt::SolidLine);
  if (m_c->yLabelBox->isVisible()) yp.setStyle(Qt::SolidLine);
  if (m_c->vLabelBox->isVisible()) vp.setStyle(Qt::SolidLine);
  if (m_c->titleBox->isVisible()) tp.setStyle(Qt::SolidLine);


  m_c->xLabelBox->setPen(xp);
  m_c->yLabelBox->setPen(yp);
  m_c->vLabelBox->setPen(vp);
  m_c->titleBox->setPen(tp);

}

void PlotView::print() {
  QPrinter printer;
  printer.setColorMode(QPrinter::Color);
  printer.setResolution(600);
  printer.setFullPage(TRUE);
  QPrintDialog pDialog(&printer, this);
  if (pDialog.exec()) {

    QPainter paint(&printer);
    QRect viewport=paint.viewport();
    viewport.setTopLeft(QPoint(1000,1500));
    paint.setViewport(viewport);

    copy();
    QPixmap px = QApplication::clipboard()->pixmap();
    QMatrix m;
    m.scale(5,5);
    QPixmap ps = px.transformed(m);

    paint.drawPixmap(0,0,ps);

  }



}


void PlotView::save() {
  QString filename=QFileDialog::getSaveFileName(this, tr("Save File"));
  if (filename!="") {
    if (filename.toStdString().find(".pic")!=std::string::npos) {
      
      QPicture picture;
      picture.setBoundingRect(QWidget::rect());
      QPainter painter;
      painter.begin(&picture);           // paint in picture
      painter.setPen(Qt::NoPen);
      recreate();
      scene()->render (&painter);
      
      painter.end();                     // painting done
      picture.save(filename);            // save picture
      
      
    }
    else if (filename.toStdString().find(".svg")!=std::string::npos) {
      
      QSvgGenerator generator;
      generator.setFileName(filename);
      generator.setSize(size());
      //generator.setViewBox(QWidget::rect());
      //generator.setTitle(tr("SVG Generator Example Drawing"));
      //generator.setDescription(tr("An SVG drawing created by the SVG Generator "
      //				  "Example provided with Qt."));
      QPainter painter;
      painter.begin(&generator);
      recreate();
      scene()->render (&painter);
      painter.end();                     
    }
    else {
      copy();
      QPixmap px = QApplication::clipboard()->pixmap();
      px.save(filename);
    }
  }
}

void PlotView::save (const std::string & filename) {
  copy();
  QPixmap px = QApplication::clipboard()->pixmap();
  px.save(QString(filename.c_str()));
  
}



QGraphicsItemGroup *PlotView::group() {
  return m_c->contentsGroup;
}

QGraphicsScene *PlotView::scene() {
  return &m_c->scene;
}


QMatrix PlotView::matrix() {
  // Master transformer:
  QMatrix m;
  m.scale(m_c->graphBoxSize.width()/m_c->rect.width(),-m_c->graphBoxSize.height()/m_c->rect.height());
  m.translate(-m_c->rect.bottomLeft().x(),  -m_c->rect.bottomLeft().y());
  // End master transformer..
  return m;
}
 

void PlotView::add (Plotable *p) {
  m_c->plotableList.push_back(p);
  create();
}

// Set the Range Divider
void PlotView::setXRangeDivider(AbsRangeDivider * xDivider) {
  m_c->xRangeDivider=xDivider;
}

// Create or re-create a Range Divider (according to current type)
AbsRangeDivider *PlotView::createXRangeDivider(bool set) {
  if (m_c->xAllocDivider) delete m_c->xAllocDivider;
  m_c->xAllocDivider = m_c->xAxisStyle==LOG ? (AbsRangeDivider *) new LogRangeDivider : (AbsRangeDivider *) new LinearRangeDivider;
  if (set) m_c->xRangeDivider=m_c->xAllocDivider;
  return m_c->xAllocDivider;
}

// Set the Range Divider
void PlotView::setYRangeDivider(AbsRangeDivider * yDivider) {
  m_c->yRangeDivider=yDivider;
}

// Create or re-create Range Divider (according to current type).
AbsRangeDivider *PlotView::createYRangeDivider (bool set) {
  if (m_c->yAllocDivider) delete m_c->yAllocDivider;
  m_c->yAllocDivider = m_c->yAxisStyle==LOG ? (AbsRangeDivider *)  new LogRangeDivider : (AbsRangeDivider *) new LinearRangeDivider;
  if (set) m_c->yRangeDivider=m_c->yAllocDivider;
  return m_c->yAllocDivider;
}

void PlotView::labelHome() {
  m_c->statBox ->setPos(0,0);
  m_c->statTextItem->setPos(m_c->statBox->sceneBoundingRect().topLeft()-m_c->statBox->pos());
}

void PlotView::editXAxisFont() {
  bool ok;
  QFont font = QFontDialog::getFont(&ok, m_c->xAxisFont, this);
  if (ok) {
    m_c->xAxisFont=font;
    create();
    // the user clicked OK and font is set to the font the user selected
  } else {
    // the user canceled the m_dialog; font is set to the initial
    // value, in this case Helvetica [Cronyx], 10
  }

}

void PlotView::editYAxisFont() {
  bool ok;
  QFont font = QFontDialog::getFont(&ok, m_c->yAxisFont, this);
  if (ok) {
    m_c->yAxisFont=font;
    create();
    // the user clicked OK and font is set to the font the user selected
  } else {
    // the user canceled the m_dialog; font is set to the initial
    // value, in this case Helvetica [Cronyx], 10
  }

}
QFont & PlotView::xAxisFont() const {
  return m_c->xAxisFont;
}

QFont & PlotView::yAxisFont() const {
  return m_c->yAxisFont;
}

void PlotView::recreate() {
  create();
}

void PlotView::animate() {
  if (isVisible()) create();
}

bool PlotView::isBox()  const {
  return m_c->m_control->ui().labelBoxBox->isChecked();
}
bool PlotView::isGrid() const {
  return m_c->m_control->ui().gridCheckBox->isChecked();
}
bool PlotView::isXZero() const {
  return m_c->m_control->ui().xZeroCheckBox->isChecked();
}
bool PlotView::isYZero() const {
  return m_c->m_control->ui().yZeroCheckBox->isChecked();
}
qreal PlotView::statBoxX() const {
  return m_c->statBox->x();
}
qreal PlotView::statBoxY() const {
  return m_c->statBox->y();
}

void PlotView::setLabelPos(qreal x, qreal y) {
  m_c->statBox ->setPos(x,y);
  m_c->statTextItem->setPos(m_c->statBox->sceneBoundingRect().topLeft()-m_c->statBox->pos());
}

void PlotView::startAnimating(int frameRateMillisec) {
  m_c->timer->start(frameRateMillisec);
}

void PlotView::stopAnimating() {
  m_c->timer->stop();
}

void PlotView::setLabelXSizePercentage (qreal percentage) {
  m_c->xPerCento=percentage;
  m_c->m_control->blockSignals(true);
  m_c->m_control->setLabelXSizePercentage (int (percentage+0.5));
  m_c->m_control->blockSignals(false);
  int w=width(),h=height();
  m_c->resizeBoxes(w,h);
  m_c->statBox ->setRect(QRect(m_c->statBoxTopLeft,m_c->statBoxSize)); 
  m_c->statEditorDialog->textEdit()->document()->setTextWidth(m_c->statBoxSize.width());
  m_c->statEditorDialog->textEdit()->setFixedWidth((int) m_c->statBox->rect().width());
}
void PlotView::setLabelYSizePercentage (qreal percentage) {
  m_c->yPerCento=percentage;
  m_c->m_control->blockSignals(true);
  m_c->m_control->setLabelYSizePercentage (int (percentage+0.5));
  m_c->m_control->blockSignals(false);
  create();
  int w=width(),h=height();
  m_c->resizeBoxes(w,h);
  m_c->statBox ->setRect(QRect(m_c->statBoxTopLeft,m_c->statBoxSize)); 
  m_c->statEditorDialog->textEdit()->setFixedHeight((int) m_c->statBox->rect().height());

}

qreal PlotView::labelXSizePercentage() const {
  return m_c->xPerCento;
}

qreal PlotView::labelYSizePercentage() const {
  return m_c->yPerCento;
}
