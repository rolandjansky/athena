/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1AvailableToolsHelper           //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Utils/VP1AvailableToolsHelper.h"
#include "VP1Base/IVP1System.h"

#include "GaudiKernel/IToolSvc.h"
#include <QComboBox>
#include <QTimerEvent>
#include <map>

//____________________________________________________________________
class VP1AvailableToolsHelper::Imp {
public:
  Imp() : theclass(0), toolsvc(0), timerid(0), updateinterval(5000), silent(false) {}
  VP1AvailableToolsHelper * theclass;
  IToolSvc* toolsvc;

  QStringList monitoredTypes;
  std::map<QString,QStringList> monitoredTypeToIgnoreList;
  QStringList ignoreList(const QString& type) {
    std::map<QString,QStringList>::const_iterator it = monitoredTypeToIgnoreList.find(type);
    return it==monitoredTypeToIgnoreList.end() ? QStringList() : it->second;
  }
  void removeIgnoreList(const QString& type) {
    std::map<QString,QStringList>::iterator it = monitoredTypeToIgnoreList.find(type);
    if (it!=monitoredTypeToIgnoreList.end())
      monitoredTypeToIgnoreList.erase(it);
  }
  QStringList availableTools;

  QStringList actualCurrentlyAvailableTools();
  QList<QComboBox*> handledComboBoxes;
  QList<QWidget*> handledWidgets;

  int timerid;
  void killTimer()
  {
    if (timerid)
      theclass->killTimer(timerid);
  }
  void restartTimer(int time)
  {
    killTimer();
    timerid = theclass->startTimer(time);
  }
  const int updateinterval;
  bool silent;
};


//____________________________________________________________________
VP1AvailableToolsHelper::VP1AvailableToolsHelper(IToolSvc* ts, QObject * parent)
  : QObject(parent), VP1HelperClassBase(0,"VP1AvailableToolsHelper"), m_d(new Imp)
{
  m_d->theclass = this;
  m_d->toolsvc = ts;
  if (!m_d->toolsvc)
    message("ERROR - received NULL ToolSvc Pointer");

  //Should go away in future gaudi versions:
  m_d->restartTimer(0);
}

//____________________________________________________________________
VP1AvailableToolsHelper::VP1AvailableToolsHelper(IVP1System* sys, QObject * parent)
  : QObject(parent), VP1HelperClassBase(sys,"VP1AvailableToolsHelper"), m_d(new Imp)
{
  m_d->theclass = this;
  m_d->toolsvc = sys ? sys->toolSvc() : 0;
  if (!sys)
    message("ERROR - received NULL system Pointer");
  if (!m_d->toolsvc)
    message("ERROR - could not get ToolSvc Pointer");
}

//____________________________________________________________________
VP1AvailableToolsHelper::~VP1AvailableToolsHelper()
{
  delete m_d;
}

//____________________________________________________________________
void VP1AvailableToolsHelper::scheduleImmediateUpdate() const
{
  m_d->restartTimer(0);
}

//____________________________________________________________________
void VP1AvailableToolsHelper::addMonitoredType(const QString& mt, const QStringList& ignoreList)
{
  m_d->monitoredTypeToIgnoreList[mt] = ignoreList;
  if (!m_d->monitoredTypes.contains(mt)) {
    m_d->monitoredTypes << mt;
    scheduleImmediateUpdate();
  }
}

//____________________________________________________________________
void VP1AvailableToolsHelper::addMonitoredTypes(const QStringList& mts, const QStringList& ignoreList)
{
  bool added(false);
  foreach (QString mt, mts) {
    m_d->monitoredTypeToIgnoreList[mt] = ignoreList;
    if (!m_d->monitoredTypes.contains(mt)) {
      m_d->monitoredTypes << mt;
      added=true;
    }
  }
  if (added)
    scheduleImmediateUpdate();
}

//____________________________________________________________________
void VP1AvailableToolsHelper::removeMonitoredType(const QString& mt)
{
  if (m_d->monitoredTypes.contains(mt)) {
    m_d->monitoredTypes.removeAll(mt);
    m_d->removeIgnoreList(mt);
    scheduleImmediateUpdate();
  }
}

//____________________________________________________________________
void VP1AvailableToolsHelper::removeMonitoredTypes(const QStringList& mts)
{
  bool removed(false);
  foreach (QString mt, mts) {
    if (m_d->monitoredTypes.contains(mt)) {
      m_d->monitoredTypes.removeAll(mt);
      m_d->removeIgnoreList(mt);
      removed = true;
    }
  }
  if (removed)
    scheduleImmediateUpdate();
}

//____________________________________________________________________
void VP1AvailableToolsHelper::clearMonitoredTypes(const QString&)
{
  if (m_d->monitoredTypes.count()>0) {
    m_d->monitoredTypes.clear();
    m_d->monitoredTypeToIgnoreList.clear();
    scheduleImmediateUpdate();
  }
}

//____________________________________________________________________
QStringList VP1AvailableToolsHelper::monitoredTypes() const
{
  return m_d->monitoredTypes;
}

//____________________________________________________________________
QStringList VP1AvailableToolsHelper::Imp::actualCurrentlyAvailableTools()
{
  QStringList l;
  if (toolsvc) {
    foreach ( QString tooltype, monitoredTypes ) {
      std::vector<std::string> instances;
      instances = toolsvc->getInstances( tooltype.toStdString() );
      if (!silent&&theclass->verbose())
	theclass->messageVerbose("Used toolsvc->getInstances(..) to find "+QString::number(instances.size())+" public tools of type "+tooltype );
      for (unsigned i = 0; i < instances.size(); ++i ) {
	QString instance(instances.at(i).c_str());
	if (instance.startsWith("ToolSvc.",Qt::CaseInsensitive))
	  instance.remove(0,8);
	//Check if we ignore this one:
	QStringList ignorelist = ignoreList(tooltype);
	if (!ignorelist.isEmpty()) {
	  bool ignore(false);
	  foreach (QString ignorepattern, ignorelist) {
	    if (QRegExp(ignorepattern,Qt::CaseSensitive,QRegExp::Wildcard).exactMatch(instance)) {
	      ignore = true;
	      break;
	    }
	  }
	  if (ignore) {
	    if (!silent&&theclass->verbose())
	      theclass->messageVerbose("  --> Found but ignoring "+instance );
	    continue;
	  }
	}
	if (!silent&&theclass->verbose())
	  theclass->messageVerbose("  --> Found "+instance );
	l << tooltype+"/"+instance;
	if (!silent&&theclass->verbose())
	  theclass->messageVerbose("  --> Appended "+tooltype+"/"+instance+" to list" );
      }
    }
    l.sort();
  }
  return l;
}

//____________________________________________________________________
void VP1AvailableToolsHelper::update() const
{
  QStringList newtools = m_d->actualCurrentlyAvailableTools();
  if ( newtools == m_d->availableTools ) {
    m_d->restartTimer(2000);
    return;
  }
  messageVerbose("update() found changes in tool list!");
  m_d->availableTools = newtools;
  bool notempty = ! m_d->availableTools.empty();

  if (notempty) {
    foreach (QComboBox* cb, m_d->handledComboBoxes) {
      cb->clear();
      cb->addItems(m_d->availableTools);
    }
  }
  foreach (QWidget* w, m_d->handledWidgets) {
    if (w->isEnabled() != notempty)
      w->setEnabled(notempty);
  }

  messageDebug("Emitting availableToolsChanged (ntools="+QString::number(m_d->availableTools.count())+")");
  availableToolsChanged(m_d->availableTools);

  m_d->restartTimer(2000);
}

//____________________________________________________________________
QStringList VP1AvailableToolsHelper::availableTools() const
{
  update();
  return m_d->availableTools;
}

//____________________________________________________________________
void VP1AvailableToolsHelper::setComboBoxData(QComboBox* cb)
{
  messageVerbose("setComboBoxData");
  if (!cb) {
    message("setComboBoxData ERROR: Null pointer to combobox");
    return;
  }
  cb->clear();
  cb->addItems(availableTools());
}

//____________________________________________________________________
void VP1AvailableToolsHelper::disableIfNoTools(QWidget* w)
{
  messageVerbose("disableIfNoTools");
  if (!w) {
    message("disableIfNoTools ERROR: Null pointer to widget");
    return;
  }
  w->setEnabled(!availableTools().isEmpty());
}

//____________________________________________________________________
void VP1AvailableToolsHelper::handleComboBox(QComboBox*cb)
{
  messageVerbose("handleComboBox");
  if (!cb) {
    message("handleComboBox ERROR: Null pointer to combobox");
    return;
  }
  if (!m_d->handledComboBoxes.contains(cb)) {
    setComboBoxData(cb);
    m_d->handledComboBoxes << cb;
    connect(cb,SIGNAL(destroyed(QObject*)),this,SLOT(widgetDeleted(QObject*)));
  }
}

//____________________________________________________________________
void VP1AvailableToolsHelper::handleEnabledState(QWidget*w)
{
  messageVerbose("handleEnabledState");
  if (!w) {
    message("handleEnabledState ERROR: Null pointer to widget");
    return;
  }
  if (!m_d->handledWidgets.contains(w)) {
    disableIfNoTools(w);
    m_d->handledWidgets << w;
    connect(w,SIGNAL(destroyed(QObject*)),this,SLOT(widgetDeleted(QObject*)));
  }
}

//____________________________________________________________________
void VP1AvailableToolsHelper::widgetDeleted(QObject* o)
{
  if ( o->inherits("QComboBox") && m_d->handledComboBoxes.contains(static_cast<QComboBox*>(o)))
    m_d->handledComboBoxes.removeAll(static_cast<QComboBox*>(o));
  if ( o->isWidgetType() && m_d->handledWidgets.contains(static_cast<QWidget*>(o)))
    m_d->handledWidgets.removeAll(static_cast<QWidget*>(o));
}

//____________________________________________________________________
void VP1AvailableToolsHelper::timerEvent ( QTimerEvent * event )
{
  event->accept();
  if (event->timerId()!=m_d->timerid) {
    message("ERROR: Bad timer ID!!");
    killTimer(event->timerId());
  }
  m_d->silent = true;
  update();
  m_d->silent = false;
  m_d->restartTimer(3000);
}
