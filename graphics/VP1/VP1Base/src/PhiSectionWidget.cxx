/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1Base/PhiSectionWidget.h"

#include <QMouseEvent>
#include <QGraphicsEllipseItem>
#include <QCursor>
#include <QDrag>
#include <QMenu>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QApplication>
#include <QBuffer>
#include <QClipboard>
#include <QInputDialog>
#include <QVector>
#include <QMimeData>
#include <QDebug>

#include <cmath>
#include <iostream>

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Implementation of class PhiSectionWidget                           //
//                                                                     //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial VP1 version: September 2007                                //
//  Major update: January 2009                                         //
//  Riccardo-Maria BIANCHI <rbianchi@cern.ch>: March 2013              //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

//____________________________________________________________________
class PhiSectionWidget::Imp {
public:
  PhiSectionWidget * theclass;

  //Sectors status and mapping to graphics:
  QVector<bool> sectorstatus;
  std::map<QGraphicsEllipseItem*,int> item2sector;

  void approximateSectorStatusFromRanges( QList<VP1Interval> oldEnabledRanges, QVector<bool>& target );

  QPoint dragStartPosition;

  //Styles:
  QPen pen_on;
  QPen pen_off;
  QBrush brush_on;
  QBrush brush_off;

  //Update colours based on current sectorstatus vector:
  void updateColors();

  //Cache:
  bool cacheValid;
  QList<VP1Interval> cachedRanges;//this also serves as the cache
  bool cachedAllOn;//part of the cache
  bool cachedAllOff;//part of the cache

  //Convenience:
  static void wrap(QList<VP1Interval>&);

  //Allowed sectors:
  bool allowCustomNSectors;
  QList<int> allowedNSectors;

  //Menu:
  QMenu * popup_menu;
  QAction* popup_enableAllAction;
  QAction* popup_disableAllAction;
  QAction* popup_invertAction;
  QAction* popup_copyAction;
  QAction* popup_pasteAction;
  QMenu * popup_setNPhiSubMenu;
  QList<QAction*> popuplist_setNPhi;
  QAction* popup_setCustomNPhi;
  void ensureMenuInit();
  void addMenuEntriesForSetNPhi();

  //Decoding the current phi-ranges and checking for changes:
  QList<VP1Interval> enabledPhiRangesNoCache(const QVector<bool>& secstatus,
					     bool& allOn, bool& allOff) const;
  void checkForChanges();
};


//____________________________________________________________________
PhiSectionWidget::PhiSectionWidget(QWidget * parent,IVP1System * sys)
  : QGraphicsView(parent),
    VP1HelperClassBase(sys,"PhiSectionWidget"),
    m_d(new Imp)
{
  messageVerbose("PhiSectionWidget constr");

  m_d->theclass = this;
  m_d->popup_menu = 0;
  m_d->popup_enableAllAction = 0;
  m_d->popup_disableAllAction = 0;
  m_d->popup_invertAction = 0;
  m_d->popup_copyAction = 0;
  m_d->popup_pasteAction = 0;
  m_d->popup_setNPhiSubMenu = 0;

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setRenderHint(QPainter::Antialiasing,true);

  m_d->pen_on.setWidth(0);
  m_d->pen_on.setBrush(Qt::red);
  m_d->pen_off.setWidth(0);
  m_d->pen_off.setBrush(Qt::gray);
  m_d->brush_on.setColor(Qt::red);
  m_d->brush_on.setStyle(Qt::SolidPattern);
  m_d->brush_off.setColor(Qt::gray);
  m_d->brush_off.setStyle(Qt::SolidPattern);

  m_d->cacheValid = false;

  setFocusPolicy(Qt::NoFocus);
  setAcceptDrops(true);

  //The following is the way to init N sectors:
  m_d->allowCustomNSectors = true;
  QList<int> defaultAllowedNSectors;
  defaultAllowedNSectors <<  4 <<  5 <<  6 <<  8 << 9 << 10
			 << 12 << 16 << 24 << 32 << 36 << 48 << 64;
  m_d->allowedNSectors << 12;
  setNumberOfSectors(12,true);
  setAllowedNumberOfSectors(defaultAllowedNSectors,true);
}

//____________________________________________________________________
PhiSectionWidget::~PhiSectionWidget()
{
  QGraphicsScene * oldscene = scene();
  if (oldscene) {
    setScene(0);
    delete oldscene;
  }
  delete m_d;
}

//____________________________________________________________________
void PhiSectionWidget::setNumberOfSectors(int nsectors,bool forceAllEnabled)
{
	messageDebug("setNumberOfSectors()");


  if (nsectors<4||nsectors>99) {
	  messageDebug("nsectors<4||nsectors>99. Returning.");
    return;
  }

  if (!m_d->allowedNSectors.contains(nsectors) && !m_d->allowCustomNSectors) {
    //Oups! Let us try first to pick something among the allowed
    //values which would allow us to preserve the ranges exactly;
    int n(-1);
    for(int i = 2; i < 100; ++i) {
      if (m_d->allowedNSectors.contains(i*nsectors)) {
	n = i*nsectors;
	break;
      }
    }
    nsectors = n>0 ? n : m_d->allowedNSectors.back();
  } else if (!m_d->sectorstatus.isEmpty() && nsectors==static_cast<int>(m_d->sectorstatus.count())) {
	  messageDebug("!sectorstatus.isEmpty() && nsectors==sectorstatus.count(). Returning...");
    return;
  }

  if ( m_d->sectorstatus.isEmpty() ) {
	  messageDebug("m_d->sectorstatus is Empty.");
  }

  // check if sectors have already been defined
  QList<VP1Interval> oldEnabledRanges;
  if ( !m_d->sectorstatus.isEmpty() ) {
	  oldEnabledRanges = enabledPhiRanges();
  }

  //Update new graphics objects with the new settings

  // first clear everything...
  m_d->item2sector.clear();
  m_d->sectorstatus.clear();
  QGraphicsScene * oldscene = scene();
  setScene(new QGraphicsScene);
  delete oldscene;

  // ...then build new variables with the new settings of 'nsectors'
  const double R = 10.0;
  const double r = (2*M_PI)*R / nsectors / 1.2;

  for (int isector = 0; isector < nsectors; ++isector) {
    double phi = (isector+0.5)*(2*M_PI)/nsectors;
    double x(R*cos(phi)), y(-R*sin(phi));
    QGraphicsEllipseItem * ellipse = scene()->addEllipse(x,y,r,r);
    m_d->item2sector[ellipse]=isector;
  }

  m_d->sectorstatus.fill(forceAllEnabled,m_d->item2sector.size());

  if (!forceAllEnabled)
    m_d->approximateSectorStatusFromRanges(oldEnabledRanges,m_d->sectorstatus);
  m_d->updateColors();
  fitInView(scene()->sceneRect());
  m_d->checkForChanges();

  if ( m_d->sectorstatus.isEmpty() ) {
	  messageDebug("WARNING. m_d->sectorstatus is still Empty...");
  }
}


//____________________________________________________________________
int PhiSectionWidget::numberOfSectors() const
{
  return m_d->sectorstatus.count();
}

//____________________________________________________________________
void PhiSectionWidget::Imp::updateColors()
{
  std::map<QGraphicsEllipseItem*,int>::iterator it, itE = item2sector.end();
  for (it = item2sector.begin();it!=itE;++it) {
    if (sectorstatus[it->second]) {
      it->first->setPen(pen_on);
      it->first->setBrush(brush_on);
    } else {
      it->first->setPen(pen_off);
      it->first->setBrush(brush_off);
    }
  }
}

//____________________________________________________________________
void PhiSectionWidget::setSectorStatus(int isector, bool status)
{
  if (isector<0||isector>=static_cast<int>(m_d->sectorstatus.count()))
    return;
  if (m_d->sectorstatus.at(isector)==status)
    return;
  m_d->sectorstatus[isector]=status;
  m_d->cacheValid=false;
  m_d->checkForChanges();
}

//____________________________________________________________________
void PhiSectionWidget::Imp::ensureMenuInit()
{
  if (popup_menu)
    return;
  popup_menu = new QMenu(theclass);
  popup_copyAction =  popup_menu->addAction("&Copy");
  popup_pasteAction =  popup_menu->addAction("&Paste");
  popup_menu->addSeparator();
  popup_enableAllAction = popup_menu->addAction("&Enable all sectors");
  popup_disableAllAction = popup_menu->addAction("&Disable all sectors");
  popup_invertAction =  popup_menu->addAction("&Invert");
  popup_menu->addSeparator();
  popup_setNPhiSubMenu = popup_menu->addMenu("&Set number of phi sectors");
  addMenuEntriesForSetNPhi();
}

//____________________________________________________________________
void PhiSectionWidget::Imp::addMenuEntriesForSetNPhi()
{
  if (!popup_menu)
    return;
  if (!popuplist_setNPhi.isEmpty()) {
    foreach(QAction * setNPhiAct, popuplist_setNPhi)
      delete setNPhiAct;
  }
  popuplist_setNPhi.clear();

  foreach(int nSectors, allowedNSectors) {
    QAction * a  = popup_setNPhiSubMenu->addAction(QString::number(nSectors));
    a->setData(nSectors);
    popuplist_setNPhi << a;
  }
  if (allowCustomNSectors) {
    popup_setNPhiSubMenu->addSeparator();
    popup_setCustomNPhi = popup_setNPhiSubMenu->addAction("&Custom...");
  }
}

//____________________________________________________________________
void PhiSectionWidget::launchContextMenu(QPoint p)
{
  //Prepare:
  m_d->ensureMenuInit();
  m_d->popup_enableAllAction->setEnabled(!allSectorsOn());
  m_d->popup_disableAllAction->setEnabled(!allSectorsOff());

  QClipboard * clipboard = QApplication::clipboard();
  m_d->popup_pasteAction->setEnabled(clipboard
	       &&clipboard->mimeData()->hasFormat("vp1/enabledphisectors"));

  foreach(QAction * setNPhiAct, m_d->popuplist_setNPhi) {
    bool ok;
    int nSectors = setNPhiAct->data().toInt(&ok);
    setNPhiAct->setEnabled(ok&&nSectors!=m_d->sectorstatus.count());
  }

  //Launch
  QAction * selAct = m_d->popup_menu->exec(p);

  //React
  if (selAct==m_d->popup_copyAction) {
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("vp1/enabledphisectors", state());
    mimeData->setText(enabledRangesToString());
    clipboard->setMimeData(mimeData);
    return;
  }
  if (selAct==m_d->popup_pasteAction) {
    QByteArray data = clipboard->mimeData()->data("vp1/enabledphisectors");
    setState(data);
    return;
  }
  if (selAct==m_d->popup_enableAllAction) {
    for (int i = 0; i < m_d->sectorstatus.count(); ++i)
      m_d->sectorstatus[i] = true;
    m_d->cacheValid = false;
    m_d->checkForChanges();
    return;
  }
  if (selAct==m_d->popup_disableAllAction) {
    for (int i = 0; i < m_d->sectorstatus.count(); ++i)
      m_d->sectorstatus[i] = false;
    m_d->cacheValid = false;
    m_d->checkForChanges();
    return;
  }
  if (selAct==m_d->popup_invertAction) {
    for (int i = 0; i < m_d->sectorstatus.count(); ++i)
      m_d->sectorstatus[i] = !m_d->sectorstatus[i];
    m_d->cacheValid = false;
    m_d->checkForChanges();
    return;
  }
  if (selAct==m_d->popup_setCustomNPhi) {
    bool ok;
    // int nCustomSectors = QInputDialog::getInteger(this, "Set number of phi sectors",
    int nCustomSectors = QInputDialog::getInt(this, "Set number of phi sectors",
						  "Set number of phi sectors", m_d->sectorstatus.count(),4,99,1,&ok);
    if (ok && nCustomSectors >= 4 && nCustomSectors <= 99 )
      setNumberOfSectors(nCustomSectors);
    return;
  }
  if (m_d->popuplist_setNPhi.contains(selAct)) {
    bool ok;
    int nSectors = selAct->data().toInt(&ok);
    if (ok)
      setNumberOfSectors(nSectors);
    return;
  }
}

//____________________________________________________________________
void PhiSectionWidget::mousePressEvent(QMouseEvent *event)
{
  if (event->buttons()==Qt::RightButton) {
    launchContextMenu(QCursor::pos());
    return;
  }

  if (event->button() == Qt::LeftButton)
    m_d->dragStartPosition = event->pos();

  if (event->buttons()!=Qt::LeftButton) {
    QGraphicsView::mousePressEvent(event);
    return;
  }
  QGraphicsItem *item_ = itemAt(event->pos());
  QGraphicsEllipseItem * item = static_cast<QGraphicsEllipseItem*>(item_);
  if (!item||m_d->item2sector.find(item)==m_d->item2sector.end()) {
    QGraphicsView::mousePressEvent(event);
    return;
  }
  int isector = m_d->item2sector[item];
  if (isector<0||isector>=static_cast<int>(m_d->sectorstatus.count())) {
    QGraphicsView::mousePressEvent(event);
    return;
  }
  m_d->sectorstatus[isector]=!(m_d->sectorstatus.at(isector));
  m_d->cacheValid=false;
  m_d->checkForChanges();
}

//____________________________________________________________________
void PhiSectionWidget::mouseMoveEvent(QMouseEvent *event)
{
  if (!(event->buttons() & Qt::LeftButton))
    return;
  if ((event->pos() - m_d->dragStartPosition).manhattanLength()
      < QApplication::startDragDistance())
    return;

  QDrag *drag = new QDrag(this);
  QMimeData *mimeData = new QMimeData;

  ////////////////////////////////////////////////////////
  // For dragging state onto other phi section widgets: //
  ////////////////////////////////////////////////////////

  mimeData->setData("vp1/enabledphisectors", state());

  /////////////////////////////////////////////////////////////////
  //  For dragging a readable version of the enabled phi-ranges  //
  //  into e.g. an editor or email program:                      //
  /////////////////////////////////////////////////////////////////

  mimeData->setText(enabledRangesToString());

  //Execute drag:
  drag->setMimeData(mimeData);//drag assumes ownership of mimeData
  drag->exec(Qt::CopyAction | Qt::MoveAction);
}

//____________________________________________________________________
void PhiSectionWidget::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->source()!=this && event->mimeData()->hasFormat("vp1/enabledphisectors"))
    event->acceptProposedAction();
}

//____________________________________________________________________
void PhiSectionWidget::dropEvent(QDropEvent *event)
{
  QByteArray data = event->mimeData()->data("vp1/enabledphisectors");
  event->acceptProposedAction();
  setState(data);
}

//____________________________________________________________________
void PhiSectionWidget::resizeEvent(QResizeEvent *)
{
  if (scene())
    fitInView(scene()->sceneRect());
}

//____________________________________________________________________
bool PhiSectionWidget::allSectorsOn() const
{
  if (!m_d->cacheValid)
    m_d->checkForChanges();
  return m_d->cachedAllOn;
}


//____________________________________________________________________
bool PhiSectionWidget::allSectorsOff() const
{
  if (!m_d->cacheValid)
    m_d->checkForChanges();
  return m_d->cachedAllOff;
}

//____________________________________________________________________
void PhiSectionWidget::Imp::wrap(QList<VP1Interval>& l)
{
  if (l.count()<2||l.front().lower()!=0.0||l.back().upper()!=(2*M_PI))
    return;
  l.front().setLower(l.back().lower()-(2*M_PI));
  l.removeLast();
}


//____________________________________________________________________
QList<VP1Interval> PhiSectionWidget::Imp::enabledPhiRangesNoCache(const QVector<bool>& secstatus, bool& allOn, bool& allOff) const
{
	theclass->messageVerbose("enabledPhiRangesNoCache()");
  QList<VP1Interval> l;
  allOn = true;
  allOff = true;

  const unsigned int n = secstatus.count();

  const double dphi = (2*M_PI)/n;
  qDebug() << "n: " << n << " - dphi:" << dphi;

  bool open(false);
  double tmp(-999);

  for (unsigned i = 0; i < n; ++i) {
    const bool status = secstatus[i];
    if (status) {
      allOff = false;
    }
    else {
      allOn = false;
    }
    if (status!=open) {
      if (!open) {
	tmp = i*dphi;
	open = true;
      } else {
	l << VP1Interval(tmp,i*dphi);
	open = false;
      }
    }
  }
  if (open)
    l << VP1Interval(tmp,(2*M_PI));
  wrap(l);

  theclass->messageDebug("exiting enabledPhiRangesNoCache()...");

  return l;
}


//____________________________________________________________________
QList<VP1Interval> PhiSectionWidget::enabledPhiRanges() const
{
	messageDebug("enabledPhiRanges()");

  if (!m_d->cacheValid) {
	  messageDebug("Cache not valid. Checking for changes...");
      m_d->checkForChanges();
  }
  return m_d->cachedRanges;
}

//____________________________________________________________________
QList<VP1Interval> PhiSectionWidget::enabledPhiRanges(double phi_min,double phi_max) const
{
	messageDebug("enabledPhiRanges(double phi_min,double phi_max)");

  if (phi_min>=phi_max)
    return QList<VP1Interval>();
  if (phi_max-phi_min>=(2*M_PI))
    return enabledPhiRanges();
  while (phi_min<0) {
    phi_min += (2*M_PI);
    phi_max += (2*M_PI);
  }

  //For ranges such as -310deg->10deg, phi_max might now be larger
  //than 2pi. We handle this as a special case, merging the result of
  //two calls to this method:
  if (phi_max>(2*M_PI)) {
    QList<VP1Interval> l1 = enabledPhiRanges(phi_min,(2*M_PI));
    QList<VP1Interval> l2 = enabledPhiRanges(0,phi_max-(2*M_PI));
    //    if (!l1.empty()&&!l2.empty()&&l1.front()
    l2 << l1;
    Imp::wrap(l2);
    return l2;
  }

  QList<VP1Interval> l = enabledPhiRanges();
  if (l.empty())
    return l;

  //In case one of the ranges wraps around at phi=0, we first unwrap
  //the list:
  if (l.front().lower()<0.0) {
    l.append(VP1Interval(l.front().lower()+(2*M_PI),(2*M_PI)));
    l.front().setLower(0.0);
  }


  //Now, phi_min<phi_max and both are in [0,2pi]. Now, loop over all
  //ranges and constrain them to [phi_min,phi_max], removing those
  //that fall outside:
  double a,b;
  for ( int i = 0; i < l.count(); ) {
    a = l[i].lower();
    b = l[i].upper();
    if (b <= phi_min||a >= phi_max) {
      l.removeAt(i);
    } else {
      if (a<phi_min)
	l[i].setLower(phi_min);
      if (b>phi_max)
	l[i].setUpper(phi_max);
      ++i;
    }
  }

  Imp::wrap(l);

  return l;
}

//____________________________________________________________________
QString PhiSectionWidget::enabledRangesToString() const
{
  if (!m_d->cacheValid)
    m_d->checkForChanges();
  QString s;
  for(int i=0;i<m_d->cachedRanges.count();++i)
    s+= m_d->cachedRanges.at(i).toString()+(i==m_d->cachedRanges.count()-1?"":", ");
  return s;
}

//____________________________________________________________________
void PhiSectionWidget::Imp::checkForChanges()
{
	theclass->messageDebug("checkForChanges()");

  //Ensure caches are updated, and emit signal in case of any change:
  cacheValid = true;

  QList<VP1Interval> r = enabledPhiRangesNoCache(sectorstatus,cachedAllOn, cachedAllOff);
  if (cachedRanges == r) {
    return;
  }
  updateColors();
  cachedRanges = r;

  if (theclass->verbose()) {
    QString s;
    for(int i=0;i<r.count();++i)
    s+= r.at(i).toString()+(i==r.count()-1?"":", ");
    theclass->messageVerbose("Emitting enabledPhiRangesChanged("
			     +theclass->enabledRangesToString()+")");
  }
  emit theclass->enabledPhiRangesChanged(r);
}

//____________________________________________________________________
bool PhiSectionWidget::virtualSectorEnabled(int iSector,int nSectors) const
{
//	this->messageDebug("virtualSectorEnabled()");

  if (iSector<0||iSector>=nSectors||nSectors<1)
    return false;
  if (!m_d->cacheValid)
    m_d->checkForChanges();
  if (m_d->cachedAllOn)
    return true;
  if (m_d->cachedAllOff)
    return false;

  //Phi-range of virtual sector:
  const double dphi = (2*M_PI)/nSectors;
  const double epsilon(dphi*1.0e-9);
  VP1Interval phirange(dphi*iSector+epsilon,dphi*(iSector+1)-epsilon);

  //Compare with enabled ranges:
  foreach(VP1Interval enabledrange,m_d->cachedRanges) {
    if (phirange.hasOverlap(enabledrange,2*M_PI))
      return true;
  }
  return false;
}

//____________________________________________________________________
QVector<bool> PhiSectionWidget::virtualSectorsEnabled(int nSectors) const
{
  if (nSectors<1)
    return QVector<bool>();
  QVector<bool> v(nSectors,false);
  for (int i = 0; i < nSectors; ++i)
    if (virtualSectorEnabled(i,nSectors))
      v[i] = true;
  return v;
}

//____________________________________________________________________
int PhiSectionWidget::phiToVirtualSectorIndex(double phi,int nSectors)
{
  //Constrain phi to [0,2*pi[
  while (phi<0) phi+=(2*M_PI);
  while (phi>=(2*M_PI)) phi-=(2*M_PI);
  if (phi<0.0) phi=0.0;

  int iphi = static_cast<int>(nSectors*phi/((2*M_PI)));
  return iphi;
}

//____________________________________________________________________
QByteArray PhiSectionWidget::state() const
{
  // ===> Setup stream writing to a byteArray:
  QByteArray byteArray;
  QBuffer buffer(&byteArray);
  buffer.open(QIODevice::WriteOnly);
  QDataStream out(&buffer);
  out << qint32(0);//version
  out << m_d->sectorstatus;//This is all the data we output,
			 //allowing for applicability of the data
			 //between different widgets, even if they are
			 //being used differently.
  buffer.close();
  return byteArray;


}

//____________________________________________________________________
void PhiSectionWidget::setState(QByteArray ba)
{
  this->messageVerbose("setState()");
  // ===> Setup stream for getting the contents of the byteArray:
  QBuffer buffer(&ba);
  buffer.open(QIODevice::ReadOnly);
  QDataStream state(&buffer);

  qint32 version;
  state >> version;
  if (version!=0)
    return;

  QVector<bool> restored_sectorstatus;
  state >> restored_sectorstatus;
  buffer.close();

  bool save = blockSignals(true);
  if (restored_sectorstatus.count()!=m_d->sectorstatus.count())
    setNumberOfSectors(restored_sectorstatus.count(),true);
  if (restored_sectorstatus.count()==m_d->sectorstatus.count()) {
    m_d->sectorstatus = restored_sectorstatus;
  } else {
    bool allon, alloff;
    QList<VP1Interval> r = m_d->enabledPhiRangesNoCache(restored_sectorstatus,allon, alloff);
    if (allon) {
      m_d->sectorstatus.fill(true);
    }
    else if (alloff) {
      m_d->sectorstatus.fill(true);
    }
    else {
      m_d->approximateSectorStatusFromRanges(r,m_d->sectorstatus);
    }
  }
  if (!save)
    blockSignals(false);
  m_d->cacheValid=false;
  m_d->checkForChanges();
}

//____________________________________________________________________
void PhiSectionWidget::Imp::approximateSectorStatusFromRanges( QList<VP1Interval> oldEnabledRanges,
							       QVector<bool>& target )
{
  if (oldEnabledRanges.isEmpty()) {
    target.fill(false);
    return;
  }
  if (oldEnabledRanges.count()==1&&oldEnabledRanges.first().length()>=2*M_PI*(1.0-1.0e-10)) {
    target.fill(true);
    return;
  }
  const unsigned n = target.size();
  target.fill(false);
  const double dphi = (2*M_PI)/n;
  const double epsilon(dphi*1.0e-9);
  for (unsigned i = 0; i < n; ++i) {
    VP1Interval phirange(dphi*i+epsilon,dphi*(i+1)-epsilon);
    foreach(VP1Interval oldrange,oldEnabledRanges) {
      if (phirange.hasOverlap(oldrange,2*M_PI)) {
	target[i]=true;
	break;
      }
    }
  }
}

//____________________________________________________________________
void PhiSectionWidget::dragMoveEvent ( QDragMoveEvent * event )
{
  //Reimplement to avoid interference by the QGraphicsView implementation.
  event->acceptProposedAction();
}

//____________________________________________________________________
void PhiSectionWidget::dragLeaveEvent ( QDragLeaveEvent * )
{
  //Reimplement to avoid interference by the QGraphicsView implementation.
}

//____________________________________________________________________
void PhiSectionWidget::setAllowedNumberOfSectors(QList<int> allowedNSectors, bool allowCustom)
{
	messageVerbose("setAllowedNumberOfSectors()");

  //Check validity:
  if (allowedNSectors.isEmpty()) {
	messageVerbose("allowedNSectors empty. Returning.");
    return;
  }
  foreach(int s, allowedNSectors) {
    if (s<4) {
		messageVerbose("allowedNSectors s < 4. Returning.");
      return;
    }
  }

  if (allowCustom&&!allowedNSectors.contains(m_d->sectorstatus.count())) {
    allowedNSectors << m_d->sectorstatus.count();
  }

  m_d->allowCustomNSectors = allowCustom;

  qSort(allowedNSectors);
  if (m_d->allowedNSectors==allowedNSectors) {
    return;
  }
  m_d->allowedNSectors=allowedNSectors;
  if (m_d->popup_menu) {
    m_d->addMenuEntriesForSetNPhi();
  }

  if (!m_d->allowedNSectors.contains(m_d->sectorstatus.count())) {
    setNumberOfSectors(m_d->sectorstatus.count());
  }

  int checkA = m_d->sectorstatus.isEmpty();
  int checkB = m_d->sectorstatus.count();
  messageDebug("check - sectorstatus.isEmpty? " + str(checkA + checkB) );

}
