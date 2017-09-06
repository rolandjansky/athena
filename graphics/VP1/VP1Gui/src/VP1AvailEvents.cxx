/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1AvailEvents                    //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Gui/VP1AvailEvents.h"
#include "VP1Base/VP1Msg.h"
#include <QFileInfo>
#include <QDirIterator>
#include <QTimer>

#include <algorithm>
#include <set>
#include <map>
#include <iostream>

//____________________________________________________________________
class VP1AvailEvents::Imp {
public:
  VP1AvailEvents*theclass;
  int timeCutForNew;
  QString tmpDir;
  int maxLocalFilesToKeep;
  //subdirs in tmpDir, generated on the fly:
  QString tmpLocalFileDir;
  QString tmpActiveRetrievalDir;

  //Returns empty in case of problems:
  QString attemptGenerationOfTmpSubdir(const QString& preferredname, QString& cachevar);

  class EventID {
  public:
    EventID(int r, unsigned long long e) : eventNumber(e), runNumber(r) {}
    unsigned long long eventNumber;
    int runNumber;
    bool operator==(const EventID & o) const{
      return eventNumber==o.eventNumber && runNumber==o.runNumber;
    }
    bool operator<(const EventID & o) const
    {
      //This ordering is not "newer". It is solely to be able to use in a set.
      return runNumber==o.runNumber ? eventNumber < o.eventNumber : runNumber < o.runNumber;
    }

    EventID & operator= (const EventID & o )
    { 
      if (&o == this) return *this;
      eventNumber = o.eventNumber;
      runNumber = o.runNumber;
      return *this;
    }
  };

  static EventID evtToID(const VP1EventFile& evt) {
    return EventID(evt.runNumber(),evt.eventNumber());
  }

  QList<EventID> historyOrdered;
  std::set<EventID> historySorted;
  QList<VP1EventFile> lastAllLocal;
  QList<VP1EventFile> lastFresh;

  std::map<QString,std::pair<QDateTime,QList<VP1EventFile> > > dircache;//dirname -> (modtime,result)

  void cleanupTmpLocalFiles();
};


//____________________________________________________________________
VP1AvailEvents::VP1AvailEvents(int timeCutForNew, QString td, int maxLocalFilesToKeep, QObject * parent)
  : QObject(parent), d(new Imp)
{
  d->theclass = this;
  d->timeCutForNew = timeCutForNew;
  d->tmpDir = QString(td + (td.endsWith("/")?"":"/")).replace("//","/");
  d->maxLocalFilesToKeep = maxLocalFilesToKeep;
}

//____________________________________________________________________
VP1AvailEvents::~VP1AvailEvents()
{
  //Fixme: These two remove commands only has an effect when dir is empty!!
  if (!d->tmpActiveRetrievalDir.isEmpty()&&d->tmpActiveRetrievalDir!="bad")
    QFile::remove(d->tmpActiveRetrievalDir);
  if (!d->tmpLocalFileDir.isEmpty()&&d->tmpLocalFileDir!="bad")
    QFile::remove(d->tmpLocalFileDir);
  delete d;
}

//____________________________________________________________________
int VP1AvailEvents::timeCutForNew() const
{
  return d->timeCutForNew;
}

//____________________________________________________________________
int VP1AvailEvents::maxLocalFilesToKeep() const
{
  return d->maxLocalFilesToKeep;
}

//____________________________________________________________________
QString VP1AvailEvents::tmpDir() const
{
  return d->tmpDir;
}

//____________________________________________________________________
QList<VP1EventFile> VP1AvailEvents::freshEvents(VP1EventFile newestEvt, const QList<VP1EventFile>& inputEventList) const
{
  QList<VP1EventFile> l;
  if (!newestEvt.isValid())
    return l;
  std::set<Imp::EventID>::const_iterator histIt, histItE(d->historySorted.end());
  if (d->timeCutForNew==0) {
    if (d->historySorted.find(Imp::evtToID(newestEvt))==histItE)
      l << newestEvt;
    return l;
  }
  foreach(VP1EventFile evt, inputEventList)
    if (d->historySorted.find(Imp::evtToID(evt))==histItE && isConsideredFresh(evt,newestEvt))
      l << evt;
  return l;
}


//____________________________________________________________________
QList<VP1EventFile> VP1AvailEvents::freshEvents() const
{
  return freshEvents(newestEvent(),allLocalEvents());
}

//____________________________________________________________________
VP1EventFile VP1AvailEvents::newestEvent() const
{
  QList<VP1EventFile> evts(allLocalEvents());
  if (evts.isEmpty())
    return VP1EventFile();//invalid
  return evts.front();
}

//____________________________________________________________________
void VP1AvailEvents::setCurrentEvent(int run,int event)
{
  d->historyOrdered << Imp::EventID(run,event);
  d->historySorted.insert(Imp::EventID(run,event));
}

//____________________________________________________________________
void VP1AvailEvents::actualCleanup()
{
  //First we cleanup:
  d->cleanupTmpLocalFiles();

  //Then we schedule a check for event list changes:
  QTimer::singleShot(10, this, SLOT(actualCheckForEventListChanges()));
}

//____________________________________________________________________
void VP1AvailEvents::actualCheckForEventListChanges()
{
  QList<VP1EventFile> allLocal = allLocalEvents();
  QList<VP1EventFile> fresh = freshEvents();
  if (d->lastAllLocal != allLocal) {
    d->lastAllLocal = allLocal;
    allLocalEventsChanged();
  }
  if (d->lastFresh != fresh) {
    d->lastFresh = fresh;
    freshEventsChanged();
  }
}

//____________________________________________________________________
void VP1AvailEvents::cleanupAndCheckForEventListChanges()
{
  //We schedule the cleanup to take place shortly. The check for event
  //list changes will be scheduled after the cleanup:

  //Then we schedule a check for event list changes:
  QTimer::singleShot(10, this, SLOT(actualCleanup()));

}

//____________________________________________________________________
bool VP1AvailEvents::inHistory(int run, int event) const
{
  return d->historySorted.find(Imp::EventID(run,event))!=d->historySorted.end();
}


//____________________________________________________________________
void VP1AvailEvents::invalidateDirCache(const QString& dir)
{
  std::map<QString,std::pair<QDateTime,QList<VP1EventFile> > >::iterator it = d->dircache.find(dir);
  if (it!=d->dircache.end())
    d->dircache.erase(it);
}

//____________________________________________________________________
QList<VP1EventFile> VP1AvailEvents::allEventFilesInDir(const QString& dir) const
{
  if (dir.isEmpty())
    return QList<VP1EventFile>();

  QFileInfo fi_dir(dir);
  if (!fi_dir.exists()||!fi_dir.isDir())
    return QList<VP1EventFile>();

  QDateTime modtime = fi_dir.lastModified();
  if (abs(modtime.time().msecsTo(QTime::currentTime()))>50) {
    std::map<QString,std::pair<QDateTime,QList<VP1EventFile> > >::iterator it = d->dircache.find(dir);
    if (it!=d->dircache.end()&&it->second.first==modtime)
      return it->second.second;
  }

  QStringList filters;
  filters << "*_*.pool.root";
  //fixme
  QDirIterator itDir(dir,filters,QDir::Files | QDir::NoDotAndDotDot | QDir::Readable | QDir::CaseSensitive);

  QList<VP1EventFile> l;
  while (itDir.hasNext()) {
    QString fn = itDir.next();
    fn.replace("//","/");
    VP1EventFile evt(fn);
    if (evt.isValid())
      l << evt;
    else
      message("Could not decode event file name: "+fn);
  }

  qSort(l);

  d->dircache[dir]=std::make_pair(modtime,l);
  return l;
}

//____________________________________________________________________
QList<VP1EventFile> VP1AvailEvents::allLocalEvents() const
{
  return allEventFilesInDir(tmpLocalFileDir());
}


//____________________________________________________________________
QString VP1AvailEvents::Imp::attemptGenerationOfTmpSubdir(const QString& preferredname, QString& cachevar)
{
  if (!cachevar.isEmpty())
    return cachevar=="bad"?"":cachevar;

  QFileInfo fi(tmpDir);
  if (!( fi.exists()&&fi.isDir())) {
    theclass->message("Could not create subdir in "+tmpDir+", since it does not exists or is not a directory" );
    cachevar="bad";
    return "";
  }
  int i(0);
  while (true) {
    ++i;
    QString dir = tmpDir+(tmpDir.endsWith("/")?"":"/")+preferredname+(i==1?QString(""):QString::number(i))+"/";
    if (!QFile::exists(dir)) {
      bool ok = QDir().mkdir(dir);
      if (!ok) {
	theclass->message("Could not create directory:: "+dir );
	cachevar="bad";
	return "";
      }
      cachevar = dir;
      return dir;
    }
  }
}

//____________________________________________________________________
QString VP1AvailEvents::tmpActiveRetrievalDir()
{
  return d->attemptGenerationOfTmpSubdir("activeretrievals",d->tmpActiveRetrievalDir);
}

//____________________________________________________________________
QString VP1AvailEvents::tmpLocalFileDir() const
{
  return d->attemptGenerationOfTmpSubdir("eventfiles",d->tmpLocalFileDir);
}

//____________________________________________________________________
void VP1AvailEvents::Imp::cleanupTmpLocalFiles()
{
  if (maxLocalFilesToKeep<=2)
    return;
  if (tmpLocalFileDir.isEmpty())
    return;
  QString dir = theclass->tmpLocalFileDir();
  if (dir.isEmpty())
    return;

  QList<VP1EventFile> events = theclass->allLocalEvents();
  int ntoremove = events.count()-maxLocalFilesToKeep;
  if (ntoremove<=3)//3 instead of 0 to keep down the times we call the machinery below.
    return;

  //Rules of engagement:
  //We never delete the current event. We never delete the previous
  //event. We never delete the maxLocalFilesToKeep newest of the fresh
  //events.

  QList<Imp::EventID> protectedEvents;
  if (!historyOrdered.isEmpty()) {
    protectedEvents << historyOrdered.back();
    if (historyOrdered.count()>1)
      protectedEvents << historyOrdered.at(historyOrdered.count()-2);
  }
  QList<VP1EventFile> freshEvts = theclass->freshEvents();
  int ifreshkept(0);
  foreach (VP1EventFile evt,theclass->freshEvents()) {
    protectedEvents << Imp::evtToID(evt);
    if (++ifreshkept==maxLocalFilesToKeep)
      break;
  }

  //Remove:
  for (int i = events.count()-1; i>=0; --i ) {
    if (protectedEvents.contains(Imp::evtToID(events.at(i)))) {
      continue;
    }
    QFile::remove(events.at(i).fileName());
    if (--ntoremove<=0)
      break;
  }
  theclass->invalidateDirCache(theclass->tmpLocalFileDir());
}

//____________________________________________________________________
bool VP1AvailEvents::isConsideredFresh ( const VP1EventFile& evt, const VP1EventFile& newestEvt ) const
{
  //Notice: Logic here must be similar to logic in VP1EvtsOnServerInfo::events

  if (d->timeCutForNew==0) {
    //Special case where only the newest event is fresh
    return evt.rawTime()==newestEvt.rawTime() &&  evt.runNumber()==newestEvt.runNumber();
  }
  if (requireNewestRunNumber()&&evt.runNumber()!=newestEvt.runNumber())
    return false;
  const unsigned oldest_time = (d->timeCutForNew<0||unsigned(d->timeCutForNew)>newestEvt.rawTime()) ? 0 : newestEvt.rawTime() - d->timeCutForNew;
  return evt.rawTime() >= oldest_time;
}

//____________________________________________________________________
void VP1AvailEvents::messageDebug(const QString& s) const
{
  std::cout<<VP1Msg::prefix_verbose()<<": "<<s.toStdString()<<std::endl;
}
