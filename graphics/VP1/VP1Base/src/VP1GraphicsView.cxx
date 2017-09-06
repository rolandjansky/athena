/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1GraphicsView                //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

//Todo: Mouse zoom mode (updown...).
//      left+middle:zoom. left+middle+right: keep.
//      left+right:drag keep mode until click.
//      Zoom by rubberband mode.
//      Add zoom in/out slots to be like Key_Plus/Key_Minus?
//      A changeview drag does not abort a zoom!
//      Adjust zoom nsteps based on number of items in view.


#include "VP1Base/VP1GraphicsView.h"
#include "VP1Base/VP1GraphicsItemCollection.h"

#include "VP1Base/IVP12DViewTransformation.h"
#include "VP1Base/VP12DViewRZFishEyeTransformation.h"//fixme
#include "VP1Base/VP12DViewRPhiFishEyeTransformation.h"//fixme

#include "VP1Base/VP1Settings.h"

// QtCore
#include <QTimer>
#include <QQueue>
#include <QSet>
#include <QDir>

// QtGui
#include <QWheelEvent>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <QScrollBar>
#include <QGraphicsPathItem>
#include <QPrinter>
#include <QPrintDialog>
#include <QPixmap>
#include <QFileDialog>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

// Qt OpenGL
#include <QGLWidget>
#include <QGLFormat>

#include<iostream>//fixme
#include <cassert>

//____________________________________________________________________
class VP1GraphicsView::Imp {
public:

  void init(VP1GraphicsView*);
  QGraphicsScene * scene;
  VP1GraphicsView * view;

  Mode mode;
  Mode lastmajormode;//To go back to the previous major mode when exiting a seek.

  QList<VP1GraphicsItemCollection*> cols_act;//All item collections not inherently INERT
  QList<VP1GraphicsItemCollection*> cols_all;//Needed since event inactive collections might have movable items.

  //The following two lists may contain pointers to ic's not presently
  //attached to the view since we need to remember settings:
  QSet<VP1GraphicsItemCollection*> cols_override_inactive;
  QSet<VP1GraphicsItemCollection*> cols_override_unmovable;

  QRectF home;

  QTimer * zoomanim_timer;
  int zoomanim_nsteps;
  double zoomanim_totaltime;
  double zoomfactoronseek;
  QQueue<QRectF> zoomanim_queue;
  QPainter::RenderHints savedrenderhints;

  QPoint dragzoom_startpoint;

  void setNewSelected(QGraphicsItem*,const bool& multimode=false);
  QMap<QGraphicsItem*,HighLightItem*> selecteditems;

//   QList<QGraphicsItem*> selecteditems;//Fixme more than one item!!
//   QList<QGraphicsPathItem*> selectionoutlines;
  VP1GraphicsItemCollection * ic_multiselection;//Holds the ic when a multiselection is taking place.
  void locateActiveItemAtPoint(QGraphicsItem*&,VP1GraphicsItemCollection*& ic,
			       const QPoint& pos,
			       VP1GraphicsItemCollection * searchic=0);
  void select(QGraphicsItem*item);
  void deselect(QGraphicsItem*item);
  void HighLightItemBeingDeleted(HighLightItem*);
  bool clearselectionsemitsignals;
  QList<QGraphicsItem*> lastemittedselecteditems;


  QString currentsaveimagefile;
  void paintDescription(QPainter *painter);
  QTextDocument helptext;
  QImage *helptextimage;
  void createNewHelptextImage(const QRect& imrect);
  bool showhelptext;

  //transformation:
  IVP12DViewTransformation * transform;
  bool transformactive;
};

//We make a slightly modified pathitem for highlight item - because we
//need to know when it is killed (which it will be when the user
//removes an item):
//____________________________________________________________________
class VP1GraphicsView::HighLightItem : public QGraphicsPathItem {
public:
  HighLightItem ( const QPainterPath & path, QGraphicsItem * parent, VP1GraphicsView*v)
    : QGraphicsPathItem(path,parent),view(v),sendsignalondelete(true) {}
  virtual ~HighLightItem() {
    std::cout<<"dying"<<std::endl;
    if (sendsignalondelete)
      view->d->HighLightItemBeingDeleted(this);
  }
  VP1GraphicsView* view;
  bool sendsignalondelete;
};

//____________________________________________________________________
VP1GraphicsView::VP1GraphicsView ( QWidget * parent )
  :QGraphicsView(parent), d(new Imp)
{
  d->init(this);
}

//____________________________________________________________________
VP1GraphicsView::VP1GraphicsView ( QGraphicsScene * scene, QWidget * parent )
  : QGraphicsView(scene,parent), d(new Imp)
{
  d->init(this);
}

//____________________________________________________________________
void VP1GraphicsView::Imp::init(VP1GraphicsView*v)
{
  view=v;
  view->setTransformationAnchor(AnchorUnderMouse);
  view->setResizeAnchor(AnchorViewCenter);
  if (QGLFormat::hasOpenGL())//It turns out that when doing x-forward (at least from lxplus) GLX, and thus OpenGL support is not present.
    view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers),view));
  view->setRenderHint(QPainter::Antialiasing);//QPainter::SmoothPixmapTransform
  view->setBackgroundBrush(Qt::black);
  if (!view->scene())
    view->setScene(new QGraphicsScene());
  scene=view->scene();
  lastmajormode = mode = PICK;
  view->setMode(PICK);

  view->setFrameStyle(QFrame::NoFrame);

  zoomanim_timer = new QTimer();
  QObject::connect(zoomanim_timer, SIGNAL(timeout()), view, SLOT(updateZoomAnimation()));
  zoomfactoronseek = 10.0;
  //Like this to set the correct timer interval:
  zoomanim_nsteps = 24;
  view->setAnimatedZoomTime(1500.0);//ms

  // Due to a bug in Qt 4.3.0 (not present in 4.2.3 and supposedly
  // fixed in 4.3.1), we have to show the scrollbars if we want to
  // have proper scrollhanddrag and seek modes. Therefore we uncomment
  // the next two lines - and we should reenable them for Qt4.3.1 (fixme).
  //   view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  //   view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  //Make sure that animation aborts if user moves a scrollbar (in case someone turns them on):
  QObject::connect(view->horizontalScrollBar(),SIGNAL(sliderMoved(int)),view,SLOT(abortZoomAnimation()));
  QObject::connect(view->verticalScrollBar(),SIGNAL(sliderMoved(int)),view,SLOT(abortZoomAnimation()));
  clearselectionsemitsignals=true;
  ic_multiselection=0;
//   selecteditem=0;
//   selectionoutline=0;
  helptextimage=0;
  showhelptext=false;

  transform = 0;
  transformactive = false;
  //fixme:
  if (false) {
    transformactive = true;
    //    transform = new VP12DViewRZFishEyeTransformation();
    transform = new VP12DViewRPhiFishEyeTransformation();
    view->setInteractive ( false );
  }

}

//____________________________________________________________________
VP1GraphicsView::~VP1GraphicsView()
{
  delete d->transform;
  delete d; d=0;
}

//____________________________________________________________________
void VP1GraphicsView::scaleView(qreal scaleFactor)
{
  abortZoomAnimation();
  qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
  if (factor < 1e-5 || factor > 1e5)//Fixme:Set as options!
    return;
  scale(scaleFactor, scaleFactor);
}

//____________________________________________________________________
void VP1GraphicsView::fitViewToContents()
{
  if (d->transform&&d->transformactive)
    fitInView( d->transform->transform(scene()->sceneRect()), Qt::KeepAspectRatio );
  else
    fitInView( scene()->sceneRect(), Qt::KeepAspectRatio );
}

//____________________________________________________________________
void VP1GraphicsView::wheelEvent(QWheelEvent *event)
{
  if (d->showhelptext) {
    d->showhelptext=false;
    viewport()->update();
  };
  double ratefact = 0.8;
  if (event->modifiers() & Qt::ControlModifier)
    ratefact = 0.1;
  if (event->modifiers() & Qt::ShiftModifier)
    ratefact = 0.01;
  scaleView(pow((double)2, -event->delta() / (240.0/ratefact)));
}

//____________________________________________________________________
void VP1GraphicsView::keyPressEvent(QKeyEvent *event)
{
  if (d->showhelptext&&!(event->key()==Qt::Key_F1||event->key()==Qt::Key_H)) {
    d->showhelptext=false;
    viewport()->update();
  };

  switch (event->key()) {
  case Qt::Key_Home:
    if (event->modifiers() & Qt::ControlModifier)
      setHome();
    else
      viewHome();
    break;
  case Qt::Key_F1:
  case Qt::Key_H:
    d->showhelptext=!d->showhelptext;
    viewport()->update();
    break;
  case Qt::Key_1:
    fitViewToContents();
    break;
  case Qt::Key_Escape:
    if (d->mode==SEEK)
      setMode(d->lastmajormode);
    else if (d->mode==PICK)
      setMode(CHANGEVIEW);
    else
      setMode(PICK);
    break;
  case Qt::Key_S:
    if (d->mode!=SEEK)
      setMode(SEEK);
    else
      setMode(d->lastmajormode);
    break;
  case Qt::Key_Plus:
  case Qt::Key_Equal:
    scaleView(1.2);
    break;
  case Qt::Key_Minus:
    scaleView(1 / 1.2);
    break;
  case Qt::Key_P:
    if (event->modifiers() & Qt::ControlModifier)
      saveImage();
    else
      print();
    break;
  default:
    //    QGraphicsView::keyPressEvent(event);
    break;
  }

//   if (d->mode==PICK) {
//     foreach (QGraphicsItem*item,items(event->pos())) {
//       foreach(VP1GraphicsItemCollection* ic,d->cols_act) {
// 	if (ic->itemBelongsAndIsPresentlyActive(item)) {
// 	  event->accept();
// 	  //EMIT [event] SIGNALS.
// 	}
//       }
//     }
//   }


  QGraphicsView::keyPressEvent(event);
}

//____________________________________________________________________
void VP1GraphicsView::setMode(const Mode& m)
{
  if (d->mode==m)
    return;
  d->mode=m;
  if (m==PICK||m==CHANGEVIEW)
    d->lastmajormode=m;

  if (m!=DRAGZOOM&&m!=DRAGZOOMHOLD)
    d->dragzoom_startpoint=QPoint();

  //Dragmode:
  if (m==CHANGEVIEW) {
    setDragMode(QGraphicsView::ScrollHandDrag);
  } else {
    setDragMode(QGraphicsView::NoDrag);
  }

  //Cursor:
  switch (m) {
  case PICK:
    setCursor(Qt::ArrowCursor);
    break;
  case CHANGEVIEW:
    setCursor(Qt::OpenHandCursor);
    break;
  case DRAGZOOM:
  case DRAGZOOMHOLD:
    setCursor(Qt::SizeVerCursor);
    break;
  case SEEK:
    setCursor(Qt::CrossCursor);
    break;
  default:
    assert(0);
    break;
  }

  //Interactions/movable:
  if (m==PICK) {
    foreach (VP1GraphicsItemCollection*ic,d->cols_act)
      if (!d->cols_override_inactive.contains(ic))
	ic->setTemporaryIgnoreInteractions(false);
    foreach (VP1GraphicsItemCollection*ic,d->cols_all)
      if (!d->cols_override_unmovable.contains(ic))
	ic->setTemporaryIgnoreMovable(false);
  } else {
    foreach (VP1GraphicsItemCollection*ic,d->cols_all)
      ic->setTemporaryIgnoreMovable(true);
    foreach (VP1GraphicsItemCollection*ic,d->cols_act)
      ic->setTemporaryIgnoreInteractions(true);
  }
}

//____________________________________________________________________
VP1GraphicsView::Mode VP1GraphicsView::mode() const
{
  return d->mode;
}

//____________________________________________________________________
void VP1GraphicsView::addItemCollection(VP1GraphicsItemCollection*ic) {
  assert(!d->cols_all.contains(ic));
  assert(!d->cols_act.contains(ic));
  d->cols_all << ic;
  if (ic->interactionMode()!=VP1GraphicsItemCollection::INERT) {
    d->cols_act << ic;
    ic->setTemporaryIgnoreInteractions( d->mode!=PICK || d->cols_override_inactive.contains(ic) );
    ic->setTemporaryIgnoreMovable( d->mode!=PICK || d->cols_override_unmovable.contains(ic) );
  }
  ic->attachToView(this);
}

//____________________________________________________________________
void VP1GraphicsView::removeItemCollection(VP1GraphicsItemCollection*ic) {
  assert(d->cols_all.contains(ic));
  d->cols_all.removeAll(ic);
  if (d->cols_act.contains(ic))
    d->cols_act.removeAll(ic);
  ic->setTemporaryIgnoreInteractions(false);
  ic->setTemporaryIgnoreMovable(false);
  ic->real_detachFromView();
  //Fixme: When user calls the detach/attach/... methods on the ic, these methods must also be invoked!
  //Fixme: use groupnode in each ic. This also allows an easy hide/unhide action as well as z values... (ic have external setZValue() method).
}

//____________________________________________________________________
void VP1GraphicsView::mouseDoubleClickEvent(QMouseEvent * event) {

  if (d->showhelptext) {
    d->showhelptext=false;
    viewport()->update();
  };

  if (d->mode!=PICK) {
    QGraphicsView::mouseDoubleClickEvent(event);
    return;
  }
  //Fixme: transformed pos
  foreach (QGraphicsItem*item,items(event->pos())) {
    foreach(VP1GraphicsItemCollection* ic,d->cols_act) {
      if (ic->itemBelongsAndIsPresentlyActive(item)) {
	event->accept();
	//EMIT SIGNALS. CHANGE SELECTION.
      }
    }
  }
  QGraphicsView::mouseDoubleClickEvent(event);
  return;
}


//____________________________________________________________________
void VP1GraphicsView::mouseMoveEvent(QMouseEvent *event)
{

  //Fixme: transformed pos

  if ((d->mode==DRAGZOOM||d->mode==DRAGZOOMHOLD) && d->dragzoom_startpoint!=QPoint() ) {
    double delta = event->pos().y()-d->dragzoom_startpoint.y();
    d->dragzoom_startpoint=event->pos();
    ViewportAnchor save = transformationAnchor();
    setTransformationAnchor(AnchorViewCenter);
    scaleView(pow((double)2, -delta / (240.0/0.8)));//FIXME!! Dont have this equation more than once!
    setTransformationAnchor(save);
    event->accept();
    return;
  }
  if (d->mode==PICK) {
    foreach (QGraphicsItem*item,items(event->pos())) {
      foreach(VP1GraphicsItemCollection* ic,d->cols_act) {
	if (ic->itemBelongsAndIsPresentlyActive(item)) {
	  event->accept();
	  //EMIT [event] SIGNALS.
	}
      }
    }
  }
  QGraphicsView::mouseMoveEvent(event);
  return;
}

//____________________________________________________________________
void VP1GraphicsView::mousePressEvent(QMouseEvent *event)
{
  setFocus(Qt::MouseFocusReason);//Fixme: also doubleclick event!

  if (d->showhelptext) {
    d->showhelptext=false;
    viewport()->update();
  };

  if (d->mode==SEEK) {
    event->accept();
    setMode(d->lastmajormode);
    seekToPoint(mapToScene(event->pos()));
    return;
  }

  if (d->mode==CHANGEVIEW&&event->buttons()==(Qt::LeftButton|Qt::MidButton)) {
    event->accept();
    d->dragzoom_startpoint=event->pos();
    setMode(DRAGZOOM);
    return;
  }

  if (d->mode==DRAGZOOM&&event->buttons()!=(Qt::LeftButton|Qt::MidButton)) {
    if (event->buttons()==(Qt::LeftButton|Qt::MidButton|Qt::RightButton)) {
      setMode(DRAGZOOMHOLD);
    } else {
      setMode(CHANGEVIEW);
    }
    event->accept();
    return;
  }

  if (d->mode==DRAGZOOMHOLD) {//Fixme: also doubleclick event!
    setMode(CHANGEVIEW);
    event->accept();
    return;
  }


  if (d->mode==PICK) {
    if (event->buttons()!=Qt::LeftButton) {
      QGraphicsView::mousePressEvent(event);
      return;
    }
    //If we have previous selections already, and the mult select key
    //is down, then we will only consider items in the same collection
    //as active.
    //    std::cout<<"pick mouse press. Multiselection stored:"<<d->ic_multiselection<<std::endl;
    bool continueselection = d->ic_multiselection && (event->modifiers() & Qt::ShiftModifier);
    QGraphicsItem*item(0);VP1GraphicsItemCollection*ic(0);

//     QPoint pickpoint = d->transform ?
//       mapFromScene(d->transform->inverseTransform( mapToScene(event->pos())))
//       : event->pos();
    QPoint pickpoint = event->pos();
    if (continueselection) {
      assert(d->ic_multiselection);
      d->locateActiveItemAtPoint(item,ic,pickpoint,d->ic_multiselection);
      if (!item) {
 	//shift was down, and the user did not hit anything
 	QGraphicsView::mousePressEvent(event);
 	return;
      }
    } else {
      d->locateActiveItemAtPoint(item,ic,pickpoint);
    }

    if (item) {
      assert(ic);
      if (ic->interactionMode()==VP1GraphicsItemCollection::SINGLEPICKS) {
	clearSelections();
	d->setNewSelected(item);
	ic->itemPickedPrivate(item);
      } else if (ic->interactionMode()==VP1GraphicsItemCollection::EVENTS) {
	clearSelections();
	event->accept();
	ic->itemGotEventPrivate(item,event);
	return;
      } else if (ic->interactionMode()==VP1GraphicsItemCollection::SELECTIONS) {
	assert(ic);
	VP1GraphicsItemCollection * tmpic = ic;
	d->ic_multiselection=ic;
	d->setNewSelected(item,continueselection);
	emitSelectionChanged(tmpic);
      } else {
	assert(0);
      }
    } else {
      clearSelections();
    }
  }

  QGraphicsView::mousePressEvent(event);
  return;
}

//____________________________________________________________________
void VP1GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{

  //Fixme: transformed pos
  if (d->mode==CHANGEVIEW&&event->buttons()==(Qt::LeftButton|Qt::MidButton)) {
    event->accept();
    d->dragzoom_startpoint=event->pos();
    setMode(DRAGZOOM);
    return;
  }

  if (d->mode==DRAGZOOM&&event->buttons()!=(Qt::LeftButton|Qt::MidButton)) {
    event->accept();
    setMode(CHANGEVIEW);
    return;
  }

  if (d->mode==PICK) {
//     foreach (QGraphicsItem*item,items(event->pos())) {
//       if (item==static_cast<QGraphicsItem*>(d->selectionoutline))//Fixme: Dont do the cast here.
// 	continue;
//       foreach(VP1GraphicsItemCollection* ic,d->cols_act) {
// 	if (ic->itemBelongsAndIsPresentlyActive(item)) {
// 	  event->accept();
// 	  if (ic->interactionMode()==VP1GraphicsItemCollection::EVENTS) {
// 	    //EMIT EVENT SIGNAL!
// 	    return;
// 	  }
// 	  if (d->selecteditem==item) {
// 	    QGraphicsView::mouseReleaseEvent(event);
// 	    return;
// 	  }
// 	  d->setNewSelected(item);
// 	  //EMIT pick/selection signals.
// 	}
//       }
//     }
  }
  QGraphicsView::mouseReleaseEvent(event);
  return;

}

//____________________________________________________________________
void VP1GraphicsView::Imp::locateActiveItemAtPoint(QGraphicsItem*&item,
						   VP1GraphicsItemCollection*& ic,
						   const QPoint& pos,
						   VP1GraphicsItemCollection * searchic) {

  //We turn the point into a small circle - to get some chance of picking lines, degenerate rectangles, etc.
  const double tol = 2.0;
  QRectF tolrect( pos.x()-tol, pos.y()-tol, 2*tol, 2*tol );

  QPainterPath p;
  if (transform&&transformactive) {
    QPainterPath ptmp;
    ptmp.addEllipse(tolrect);
    QPainterPath ptmp2;
    transform->inverseTransformPath(view->mapToScene(ptmp),ptmp2);
    QPainterPath p = view->mapFromScene(ptmp2);
  } else {
    p.addEllipse(tolrect);
  }

  //Fixme: use faster stl style iterators.
  foreach(QGraphicsItem* item2,view->items(p)) {
    if (searchic) {
      if (searchic->itemBelongsAndIsPresentlyActive(item2)) {
	item=item2;
	ic=searchic;
	return;
      }
    } else {
      foreach(VP1GraphicsItemCollection* ic2,cols_act) {
	if (ic2->itemBelongsAndIsPresentlyActive(item2)) {
	  item=item2;
	  ic=ic2;
	  return;
	}
      }
    }
  }
  assert(!item&&!ic);
  return;
}

//____________________________________________________________________
void VP1GraphicsView::Imp::HighLightItemBeingDeleted(HighLightItem*hlitem)
{

#ifndef NDEBUG
  QMapIterator<QGraphicsItem*,HighLightItem*> ittest(selecteditems);
  assert(ittest.findNext(hlitem));
#endif

  QMap<QGraphicsItem*,HighLightItem*>::iterator
    it = selecteditems.begin(), itE = selecteditems.end();
  for (;it!=itE;++it) {
    if (it.value()==hlitem) {
      std::cout<<"lala1"<<std::endl;
      selecteditems.erase(it);
      std::cout<<"lala2"<<std::endl;
      break;
    }
  }
  std::cout<<"hej"<<std::endl;

  if (ic_multiselection) {
    view->emitSelectionChanged(ic_multiselection);
  }
  std::cout<<"dav"<<std::endl;
#ifndef NDEBUG
  QMapIterator<QGraphicsItem*,HighLightItem*> ittest2(selecteditems);
  assert(!ittest2.findNext(hlitem));
#endif
}

//____________________________________________________________________
void VP1GraphicsView::clearSelections() {//Fixme: only clear
					 //selections of certain
					 //collections? (so the
					 //permanent items stay
					 //selected?)

  bool haditemsinselection = d->ic_multiselection && !d->selecteditems.empty();

  //Remove highlight items:
  QMapIterator<QGraphicsItem*,HighLightItem*> it(d->selecteditems);
  while (it.hasNext()) {
    it.next();
    it.value()->sendsignalondelete = false;
    //assert(!d->scene->items().contains(it.key()));
    if (d->scene->items().contains(it.value())) {
      d->scene->removeItem(it.value());
      //      delete it.value();
    }
  }

  //Clear lists - possibly emit a signal:
  d->selecteditems.clear();
  if (d->clearselectionsemitsignals&&haditemsinselection) {
    emitSelectionChanged(d->ic_multiselection);
    d->ic_multiselection = 0;//TEST
  }
  d->clearselectionsemitsignals=true;
}

//____________________________________________________________________
void VP1GraphicsView::Imp::deselect(QGraphicsItem*item)
{
  if (selecteditems.count()==1) {
    view->clearSelections();
    return;
  }

  assert(selecteditems.contains(item));
  QMap<QGraphicsItem*,HighLightItem*>::iterator it = selecteditems.find(item);
  if (scene->items().contains(it.value())) {
    assert(scene->items().contains(it.value()));
    scene->removeItem(it.value());
    it.value()->sendsignalondelete = false;
    delete it.value();
  }
  assert(selecteditems.contains(item));
  selecteditems.erase(it);
  assert(!selecteditems.contains(item));
}

//____________________________________________________________________
void VP1GraphicsView::Imp::select(QGraphicsItem*item)
{
  assert(!selecteditems.contains(item));

  QPainterPath path = item->shape();

  HighLightItem* outline = new HighLightItem(path,item,view);
  outline->setBrush(QColor(250,0,250,100));
  outline->setPen(QPen(Qt::red,0,Qt::SolidLine,Qt::SquareCap,Qt::RoundJoin));
  scene->addItem(outline);

  selecteditems.insert(item,outline);

}

//____________________________________________________________________
void VP1GraphicsView::Imp::setNewSelected(QGraphicsItem*item,const bool& continueselection) {

  assert(item);

  if (continueselection) {
    if (selecteditems.contains(item))
      deselect(item);
    else
      select(item);
    return;
  } else {
    if (!selecteditems.empty()) {
      clearselectionsemitsignals=false;
      view->clearSelections();
    }
    select(item);
  }

//   if (selecteditems.contains(item))
//     view->addselect(item);
//     view->removeselect(item);

//   if (!continueselection&&!selecteditems.empty()) {
//     view->clearSelections();
//   }


}

//____________________________________________________________________
void VP1GraphicsView::seekToPoint(QPointF targetpoint) {
  abortZoomAnimation();//In case there is already an animation running we abort it.
  QRectF currentview = mapToScene(viewport()->rect()).boundingRect();
  double w1= currentview.width(), h1= currentview.height();
  double w2 = w1/d->zoomfactoronseek, h2 = h1/d->zoomfactoronseek;
  double x2 = targetpoint.x()-0.5*w2, y2 = targetpoint.y()-0.5*h2;
  initiateAnimatedZoomTo( QRectF(x2,y2,w2,h2) );
}

//____________________________________________________________________
void VP1GraphicsView::initiateAnimatedZoomTo( QRectF goal )
{
  abortZoomAnimation();
  if (d->zoomanim_nsteps==1) {
    fitInView(goal);
    return;
  }

  QRectF currentview = mapToScene(viewport()->rect()).boundingRect();

  double w1 = currentview.width(), h1 = currentview.height();
  double x1 = currentview.x(), y1 = currentview.y();

  double w2 = goal.width(), h2 = goal.height();
  double x2 = goal.x(), y2 = goal.y();

  const double eps = 1.0/d->zoomanim_nsteps;
  for (double f = eps; f < 1.0+0.5*eps; f+=eps ) {
    //f values are spaced linearly between eps and 1.0. Let us
    //construct something which has values packed closer near 1.0 (for
    //that nice smooth slow-down feel):
    double f2 = sqrt(f);
    d->zoomanim_queue.enqueue(QRectF( x1*(1.0-f2)+x2*f2, y1*(1.0-f2)+y2*f2,
				      w1*(1.0-f2)+w2*f2, h1*(1.0-f2)+h2*f2 ));
  }

  d->savedrenderhints=renderHints();
  setRenderHints(0);
  d->zoomanim_timer->start();
}

//____________________________________________________________________
void VP1GraphicsView::updateZoomAnimation()
{
  assert(!d->zoomanim_queue.empty());
  //For testing:
  //   QGraphicsRectItem * item = new QGraphicsRectItem(d->zoomanim_queue.dequeue());
  //   item->setPen(QPen(Qt::blue,0,Qt::SolidLine,Qt::SquareCap,Qt::RoundJoin));
  //   scene()->addItem(item);
  fitInView( d->zoomanim_queue.dequeue(), Qt::KeepAspectRatio );
  if (d->zoomanim_queue.empty())
    d->zoomanim_timer->stop();
}

//____________________________________________________________________
void VP1GraphicsView::abortZoomAnimation()
{
  if (d->zoomanim_timer->isActive())
    d->zoomanim_timer->stop();
  if (!d->zoomanim_queue.empty())
    d->zoomanim_queue.clear();
  if (d->savedrenderhints)
    setRenderHints(d->savedrenderhints);
  d->savedrenderhints=0;//Fixme: renderhints changed during an
			//animation cycle might not be saved... Should
			//override the renderhints methods to avoid
			//this.
}


//____________________________________________________________________
void VP1GraphicsView::setAnimatedZoomSteps( int n )
{
  assert(n>=1);
  d->zoomanim_nsteps = n;
  d->zoomanim_timer->setInterval(std::max(1,static_cast<int>(d->zoomanim_totaltime/d->zoomanim_nsteps+0.5)));
  //Same as just below.
}

//____________________________________________________________________
void VP1GraphicsView::setAnimatedZoomTime( double t )
{
  d->zoomanim_totaltime = t;
  d->zoomanim_timer->setInterval(std::max(1,static_cast<int>(d->zoomanim_totaltime/d->zoomanim_nsteps+0.5)));
  //Same as just above.
}

//____________________________________________________________________
void VP1GraphicsView::setZoomFactorOnSeek( double zf )
{
  d->zoomfactoronseek = zf;
}

//____________________________________________________________________
void VP1GraphicsView::viewHome() {

  const bool animatedhome = true;//Fixme: option?
  if (d->home == QRect()) {
    if (animatedhome) {
      if (d->transform&&d->transformactive)
	initiateAnimatedZoomTo(d->transform->transform(scene()->sceneRect()));
      else
	initiateAnimatedZoomTo(scene()->sceneRect());
    } else {
      fitViewToContents();
    }
  } else {
    if (animatedhome) {
      initiateAnimatedZoomTo(d->home);
    } else {
      fitInView( d->home, Qt::KeepAspectRatio );
    }
  }
}

//____________________________________________________________________
void VP1GraphicsView::setHome() {
  d->home=mapToScene(viewport()->rect()).boundingRect();
}

//____________________________________________________________________
void VP1GraphicsView::emitSelectionChanged(VP1GraphicsItemCollection *ic) {
  QList<QGraphicsItem*> emitlist = d->selecteditems.keys();
  if (emitlist==d->lastemittedselecteditems)
    return;
  d->lastemittedselecteditems=emitlist;
  ic->selectionChangedPrivate(emitlist);
}

//____________________________________________________________________
void VP1GraphicsView::setDisallowInteractions(VP1GraphicsItemCollection*ic, const bool& disallow )
{
  if (d->cols_override_inactive.contains(ic)==disallow)
    return;

  if ( disallow ) {
    d->cols_override_inactive.insert(ic);
    if ( d->cols_all.contains(ic) )
      ic->setTemporaryIgnoreInteractions(true);
  } else {
    d->cols_override_inactive.remove(ic);
    if ( d->cols_all.contains(ic) && d->mode != PICK )
      ic->setTemporaryIgnoreInteractions(false);
  }
}

//____________________________________________________________________
void VP1GraphicsView::setDisallowMovable(VP1GraphicsItemCollection*ic, const bool& disallow )
{
  if (d->cols_override_unmovable.contains(ic)==disallow)
    return;

  if (disallow) {
    d->cols_override_unmovable.insert(ic);
    if (d->cols_all.contains(ic))
      ic->setTemporaryIgnoreMovable(true);
  } else {
    d->cols_override_unmovable.remove(ic);
    if ( d->cols_all.contains(ic) && d->mode != PICK )
      ic->setTemporaryIgnoreMovable(false);
  }
}

//____________________________________________________________________
void VP1GraphicsView::print()
{
  //Fixme: This does not really work well... Check again in QT 4.3?
  QPrinter printer;
  QPrintDialog dialog(&printer, this);
  if (dialog.exec() == QDialog::Accepted) {
    QPainter painter(&printer);
    render(&painter);
  }
}

//____________________________________________________________________
void VP1GraphicsView::saveImage()
{
  QString guess;
  if (d->currentsaveimagefile.isEmpty()) {
    QString base=VP1Settings::defaultFileSelectDirectory()+QDir::separator()+"vp1_capture";
    guess=base;
    int i(2);
    while (QFile::exists(guess+".png"))
      guess=base+"_"+QString::number(i++);
    guess+=".png";
  } else {
    guess=d->currentsaveimagefile;
  }

  QString filename = QFileDialog::getSaveFileName(0, "Select target image file", guess,
						  "Image (*.png *.bmp)",
						  0,QFileDialog::DontResolveSymlinks);
  if(filename.isEmpty())
    return;

  QPixmap pm = QPixmap::grabWidget ( viewport() );
  if (!(filename.endsWith(".png",Qt::CaseInsensitive)||filename.endsWith(".bmp",Qt::CaseInsensitive)))
    filename += ".png";

  pm.save(filename);
  d->currentsaveimagefile = filename;
}

// //____________________________________________________________________
// void VP1GraphicsView::paintEvent ( QPaintEvent * event )
// {
//   //Fixme: Reimplementing the paintEvent like this is supposedly
//   //helpful in some circumstances... I have not really benchmarked
//   //whether it actually helps though...
// //   QPaintEvent * newEvent = new QPaintEvent(event->region().boundingRect());
// //   QGraphicsView::paintEvent(newEvent);
// //   delete newEvent;
// }

//____________________________________________________________________
void VP1GraphicsView::drawForeground ( QPainter * painter, const QRectF & /*rect*/ ) {

  //Fixme: use rect?
  if (!d->showhelptext)
    return;
  QRect imgrect = viewport()->rect();
  imgrect.setWidth(imgrect.width()*3);
  imgrect.setHeight(imgrect.height()*3);
  if (!d->helptextimage||d->helptextimage->size()!=imgrect.size())
    d->createNewHelptextImage(imgrect);//viewport()->rect());
  painter->setRenderHint(QPainter::Antialiasing, false);
  painter->drawImage(mapToScene(viewport()->rect()).boundingRect(),*(d->helptextimage),
		     imgrect);
}

//____________________________________________________________________
void VP1GraphicsView::Imp::createNewHelptextImage(const QRect& imrect)
{
  if(helptext.isEmpty()) {
    helptext.setHtml ("<html>\n"
		      "\n"
		      "<h2>How to navigate a VP1GraphicsView</h2>\n"
		      "\n"
		      "  <p>\n <b>H/F1</b> Show this help."
		      "  \n <b>S</b> Seek mode.\n"
		      "  \n <b>+/-</b> Zoom.\n"
		      "  \n <b>CTRL+HOME</b> Set home.\n"
		      "  \n <b>HOME</b> View home.\n"
		      "  \n <b>1</b> View all.\n"
		      "  \n <b>ESC</b> Switch between view and pick mode.\n"
		      "  \n <b>P</b> Print view.\n"
		      "  \n <b>CTRL+P</b> Save snapshot to image file.</p>\n"
		      "  <p>\n <b>Mouse wheel</b> zoom. Use <b>CTRL</b> and <b>SHIFT</b> for finegrained control.</p>\n"
		      "  <p>\n To select items (in pick mode), click on them. Selected multiple by holding down <b>SHIFT</b>.</p>\n"
		      "\n"
		      "<p>\n"
		      "  Please report any bugs to <i>Thomas.Kittelmann@cern.ch</i>.\n"
		      "</p>\n"
		      "\n"
		      "\n"
		      "</html>" );
    QFont myfont("Helvetica",36);
    //     myfont.setPointSize(400);
    helptext.setDefaultFont ( myfont );
     //    helptext.defaultFont().setPointSize(400);
  }

  delete helptextimage;
  helptextimage = new  QImage(imrect.size(), QImage::Format_ARGB32);//Fixme delete in destructor!!

  QPainter painter;
  painter.begin(helptextimage);

  int pageWidth = qMax(imrect.width() - 100, 100);
  int pageHeight = qMax(imrect.height() - 100, 100);
  if (pageWidth != helptext.pageSize().width()) {
    helptext.setPageSize(QSize(pageWidth, pageHeight));
  }

  QRect textRect(imrect.width() / 2 - pageWidth / 2,
		 imrect.height() / 2 - pageHeight / 2,
		 pageWidth,
		 pageHeight);
  int pad = 10;
  QRect clearRect = textRect.adjusted(-pad, -pad, pad, pad);
  painter.setPen(Qt::NoPen);
  painter.setBrush(QColor(0, 0, 0, 63));
  int shade = 10;
  painter.drawRect(clearRect.x() + clearRect.width() + 1,
		    clearRect.y() + shade,
		    shade,
		    clearRect.height() + 1);
  painter.drawRect(clearRect.x() + shade,
		    clearRect.y() + clearRect.height() + 1,
		    clearRect.width() - shade + 1,
		    shade);

  painter.setRenderHint(QPainter::Antialiasing, false);
  painter.setBrush(QColor(255, 255, 255, 220));
  painter.setPen(Qt::black);
  painter.drawRect(clearRect);

  painter.setClipRegion(textRect, Qt::IntersectClip);
  painter.translate(textRect.topLeft());

  QAbstractTextDocumentLayout::PaintContext ctx;

  QLinearGradient g(0, 0, 0, textRect.height());
  g.setColorAt(0, Qt::black);
  g.setColorAt(0.9, Qt::black);
  g.setColorAt(1, Qt::transparent);

  QPalette pal = view->viewport()->palette();
  pal.setBrush(QPalette::Text, g);

  ctx.palette = pal;
  ctx.clip = QRect(0, 0, textRect.width(), textRect.height());
  helptext.documentLayout()->draw(&painter, ctx);

  painter.end();

}

//____________________________________________________________________
void VP1GraphicsView::drawItems(QPainter *painter, int numItems,
				QGraphicsItem *items[],
				const QStyleOptionGraphicsItem options[])
{

  if (!(d->transform&&d->transformactive)) {
    QGraphicsView::drawItems(painter,numItems,items,options);
    //Fixme: implement own drawing here also - to make sure inbuilt selection stuff is never shown?
    return;
  }

  for (int i = 0; i < numItems; ++i) {
    QGraphicsItem *item = items[i];
    painter->save();
    painter->setMatrix(item->sceneMatrix(), true);//??
    //     std::cout<< item->sceneMatrix().isIdentity()<<std::endl;
    //     std::cout<< item->sceneMatrix().dx()<<" : "<<item->sceneMatrix().dy()<<std::endl;
    d->transform->paintItem(painter, item);
    painter->restore();
  }
}
