/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1Prioritiser                 //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Gui/VP1Prioritiser.h"
#include "VP1Base/IVP1ChannelWidget.h"
#include "VP1Base/IVP1System.h"
#include <QTime>
#include <QElapsedTimer>
#include <QQueue>
#include <QSet>
#include <map>
#include <cassert>

/////////////////////////////////////////////////////////////////////
////////////////////  Class VP1Prioritiser::Imp  ////////////////////
/////////////////////////////////////////////////////////////////////

//____________________________________________________________________
class VP1Prioritiser::Imp {
public:

  class SystemInfo{
  public:
    enum VisibilityState { NOTVISIBLE, VISIBLE, SOONVISIBLE };
    SystemInfo(IVP1System*,Imp *, IVP1ChannelWidget *, const VisibilityState& );
    ~SystemInfo();
    IVP1System * system;
    void setVisibilityState(const VisibilityState&);
    void addTimeMeasurement(const double&);
    double refreshtime() const { return m_timing; }
  private:
    double m_timing;
    double m_priority;
    const bool m_accumulator;
    VisibilityState m_vs;
    void calcPriority();
    Imp *m_d;
    void removeFromPriMap();
    QQueue<double> m_timemeasurements;
  };

  VP1Prioritiser * prioritiser;
  //The following map is for prioritising the order of system refreshes:
  typedef std::multimap<double,IVP1System*> Pri2SysMap;
  Pri2SysMap priority2sys;//This map is kept updated automatically by the SystemInfo objects.

  QSet<IVP1ChannelWidget*> visiblechannels, soonvisiblechannels;
  double soonvisbonus;

  QHash<IVP1System*,SystemInfo*> sys2info;

  QElapsedTimer * stopwatch;
  IVP1System* currenttimedsystem;

  void updateSysinfoWithVisibilityState(const QSet<IVP1ChannelWidget*>& channels,
					const SystemInfo::VisibilityState&vs);


  std::set<IVP1System *>::const_iterator itsys, itsysE;
};

/////////////////////////////////////////////////////////////////////
//////////////  Class VP1Prioritiser::Imp::SystemInfo  //////////////
/////////////////////////////////////////////////////////////////////

//____________________________________________________________________
inline VP1Prioritiser::Imp::SystemInfo::SystemInfo(IVP1System*s,Imp *d,IVP1ChannelWidget * cw, const VisibilityState& vs )
  :  system(s), m_priority(117117.117),
     m_accumulator(cw->isAccumulator()),m_vs(vs),m_d(d)
{
  //If another system with the same name exists already, we use that
  //timing as a reasonable guess. Otherwise we use the average of the
  //other channels. Failing that we use a default of 500ms.:
  double tmp(0);
  bool found(false);
  QHash<IVP1System*,Imp::SystemInfo*>::iterator itE = m_d->sys2info.end();
  for (QHash<IVP1System*,Imp::SystemInfo*>::iterator it=m_d->sys2info.begin();it!=itE;++it) {
    if ( it.key()->name() == s->name() ) {
      m_timing=it.value()->refreshtime();
      found=true;
      break;
    }
    tmp += m_timing=it.value()->refreshtime();
  }
  if (!found&&!m_d->sys2info.empty())
    m_timing = tmp/m_d->sys2info.count();
  else
    m_timing=500;

  calcPriority();
}

//____________________________________________________________________
inline VP1Prioritiser::Imp::SystemInfo::~SystemInfo()
{
  removeFromPriMap();
}

//____________________________________________________________________
inline void VP1Prioritiser::Imp::SystemInfo::setVisibilityState(const VisibilityState& vs )
{
  //Fixme: When do we get tabmanager setVisible signals? At startup? When channels are added?
  //Fixme: Do not emit these during file loading!!
  if (m_vs==vs)
    return;
  m_vs = vs;
  calcPriority();
}

//____________________________________________________________________
inline void VP1Prioritiser::Imp::SystemInfo::addTimeMeasurement(const double&t)
{
  //Keep the seven latest measurements, and use a truncated mean as
  //final estimate.

  m_timemeasurements.enqueue(t);
  if (m_timemeasurements.count()==8)
    m_timemeasurements.dequeue();

  QList<double> tmplist = m_timemeasurements;
  std::sort(tmplist.begin(), tmplist.end());

  switch(tmplist.count()) {
  case 1: m_timing = tmplist.at(0); break;
  case 2: m_timing = 0.5*(tmplist.at(0)+tmplist.at(1)); break;
  case 3: m_timing = tmplist.at(1); break;
  case 4: m_timing = 0.5*(tmplist.at(1)+tmplist.at(2)); break;
  case 5: m_timing = (tmplist.at(1)+tmplist.at(2)+tmplist.at(3))/3.0; break;
  case 6: m_timing = 0.5*(tmplist.at(2)+tmplist.at(3)); break;
  case 7: m_timing = (tmplist.at(2)+tmplist.at(3)+tmplist.at(4))/3.0; break;
  default: assert(0&&"Should never happen"); break;
  }

  calcPriority();
}

//____________________________________________________________________
inline void VP1Prioritiser::Imp::SystemInfo::calcPriority()
{
  //1) Remove from m_d->priority2sys map

  if (m_priority!=117117.117)
    removeFromPriMap();

  //2) Calculate new priority

  //Priority is based on how long the systems take to refresh (the
  //fastest gets refreshed first).  In order to ensure that visible,
  //soonvisible, and accumulators gets priority they get a bonus of
  //30s, XXXms, and 100ms respectively (where XXX depends on the scenario):

  m_priority = m_timing + (m_vs==VISIBLE?-30000.0:(m_vs==SOONVISIBLE?-m_d->soonvisbonus:0.0)) + (m_accumulator?-100.0:0.0);

  //3) Add to m_d->priority2sys map

  std::pair<double,IVP1System*> a(m_priority,system);
  m_d->priority2sys.insert(a);
}

//____________________________________________________________________
inline void VP1Prioritiser::Imp::SystemInfo::removeFromPriMap()
{
  //Fixme: Cache itp and it (in class Imp even?)!
  std::pair<Pri2SysMap::iterator, Pri2SysMap::iterator> itp(m_d->priority2sys.equal_range(m_priority));
  Pri2SysMap::iterator it = itp.first;
  for (;it!=itp.second;++it) {
    if (it->second==system) {
      m_d->priority2sys.erase(it);
      break;
    }
  }
  assert(it != itp.second);
}

/////////////////////////////////////////////////////////////////////
//////////////////////  Class VP1Prioritiser  ///////////////////////
/////////////////////////////////////////////////////////////////////

//____________________________________________________________________
VP1Prioritiser::VP1Prioritiser(QObject*parent)
 : QObject(parent), m_d(new Imp)
{
  m_d->prioritiser=this;
  m_d->stopwatch = new QElapsedTimer();
  m_d->currenttimedsystem=0;
  m_d->soonvisbonus=0;
}

//____________________________________________________________________
VP1Prioritiser::~VP1Prioritiser()
{
  delete m_d->stopwatch;
  delete m_d; m_d=0;
}

//___________________________________________________________________
IVP1System* VP1Prioritiser::nextErasedActiveSystemByPriority() {
  Imp::Pri2SysMap::iterator itE = m_d->priority2sys.end();
  for (Imp::Pri2SysMap::iterator it=m_d->priority2sys.begin();it!=itE;++it) {
    if ( it->second->state()==IVP1System::ERASED &&it->second->activeState()==IVP1System::ON )
      return it->second;
  }
  return 0;
}

//___________________________________________________________________
QList<IVP1System*> VP1Prioritiser::getSystemsToEraseByPriority() {
  QList<IVP1System*> tmp;
  Imp::Pri2SysMap::iterator itE = m_d->priority2sys.end();
  for (Imp::Pri2SysMap::iterator it=m_d->priority2sys.begin();it!=itE;++it) {
    if ( it->second->state()==IVP1System::REFRESHED )
      tmp << it->second;
  }
  return tmp;
}


//___________________________________________________________________
double VP1Prioritiser::estimateRemainingCalcTime() const
{
  double tmp(0);
  QHash<IVP1System*,Imp::SystemInfo*>::iterator itE = m_d->sys2info.end();
  for (QHash<IVP1System*,Imp::SystemInfo*>::iterator it=m_d->sys2info.begin();it!=itE;++it) {
    if ( it.key()->state()==IVP1System::ERASED &&it.key()->activeState()==IVP1System::ON )
      tmp += it.value()->refreshtime();
  }
  return tmp;
}

//___________________________________________________________________
double VP1Prioritiser::beginTiming_Refresh(IVP1System*s)
{
  assert(!m_d->currenttimedsystem);
  m_d->stopwatch->start();
  m_d->currenttimedsystem=s;
  assert(m_d->sys2info.contains(s));
  return m_d->sys2info[s]->refreshtime();
}


//___________________________________________________________________
double VP1Prioritiser::elapsedTiming_Refresh()
{
  return static_cast<double>(m_d->stopwatch->elapsed());
}

//___________________________________________________________________
double VP1Prioritiser::endTiming_Refresh()
{
  assert(m_d->currenttimedsystem);
  double timing = static_cast<double>(m_d->stopwatch->elapsed());
  if (m_d->sys2info.contains(m_d->currenttimedsystem)) {//This check, since the corresponding channel might have become uncreated in the meantime.
    m_d->sys2info[m_d->currenttimedsystem]->addTimeMeasurement(timing);
  }
  m_d->currenttimedsystem=0;
  return timing;
}

//____________________________________________________________________
void VP1Prioritiser::setupSysItr(IVP1ChannelWidget*cw) {
  m_d->itsys = cw->systems().begin();
  m_d->itsysE = cw->systems().end();
}

//___________________________________________________________________
void VP1Prioritiser::Imp::updateSysinfoWithVisibilityState(const QSet<IVP1ChannelWidget*>& channels,
							   const SystemInfo::VisibilityState&vs) {
  foreach(IVP1ChannelWidget*cw,channels) {
    for (prioritiser->setupSysItr(cw);itsys!=itsysE;++itsys) {
      assert(sys2info.contains(*itsys));
      //if (sys2info.contains(*itsys))//This check since channel deletion might have triggered this update.
      sys2info[*itsys]->setVisibilityState(vs);
    }
  }
}

//___________________________________________________________________
void VP1Prioritiser::visibleChannelsChanged(const QSet<IVP1ChannelWidget*>&newvis,
					    const QSet<IVP1ChannelWidget*>&newsoonvis,
					    const double& soonvisbonus)
{
  if (m_d->visiblechannels==newvis&&m_d->soonvisiblechannels==newsoonvis&&m_d->soonvisbonus==soonvisbonus)
    return;
  m_d->updateSysinfoWithVisibilityState(m_d->visiblechannels,Imp::SystemInfo::NOTVISIBLE);
  m_d->updateSysinfoWithVisibilityState(m_d->soonvisiblechannels,Imp::SystemInfo::NOTVISIBLE);
  m_d->updateSysinfoWithVisibilityState(newvis,Imp::SystemInfo::VISIBLE);
  m_d->updateSysinfoWithVisibilityState(newsoonvis,Imp::SystemInfo::SOONVISIBLE);
  m_d->visiblechannels = newvis;
  m_d->soonvisiblechannels = newsoonvis;
  m_d->soonvisbonus = soonvisbonus;
}

//___________________________________________________________________
void VP1Prioritiser::channelCreated(IVP1ChannelWidget* cw)
{
  bool visible = m_d->visiblechannels.contains(cw);
  bool soonvisible = m_d->soonvisiblechannels.contains(cw);
  assert(!(visible&&soonvisible));
  for (setupSysItr(cw);m_d->itsys!=m_d->itsysE;++m_d->itsys) {
    assert(!m_d->sys2info.contains(*(m_d->itsys)));
    Imp::SystemInfo* info = new Imp::SystemInfo(*(m_d->itsys),m_d,cw,
						(visible?Imp::SystemInfo::VISIBLE:(soonvisible?Imp::SystemInfo::SOONVISIBLE:Imp::SystemInfo::NOTVISIBLE)));
    m_d->sys2info.insert(*(m_d->itsys),info);
  }
}

//___________________________________________________________________
void VP1Prioritiser::channelUncreated(IVP1ChannelWidget* cw)
{
  for (setupSysItr(cw);m_d->itsys!=m_d->itsysE;++m_d->itsys) {
    assert(m_d->sys2info.contains(*(m_d->itsys)));
    delete m_d->sys2info[*(m_d->itsys)];
    m_d->sys2info.remove(*(m_d->itsys));
  }
  if (m_d->visiblechannels.contains(cw))
    m_d->visiblechannels.remove(cw);
  if (m_d->soonvisiblechannels.contains(cw))
    m_d->soonvisiblechannels.remove(cw);
}
