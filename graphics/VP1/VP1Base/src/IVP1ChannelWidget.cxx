/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class IVP1ChannelWidget              //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Base/IVP1ChannelWidget.h"
#include "VP1Base/IVP1System.h"

#include <cassert>
#include <QPixmap>

class IVP1ChannelWidget::Imp {

public:
	Imp(const QString & n, const QString & i, const QString & c)
: name(n), information(i), contact_info(c),
  cloneid(0), controller(0), state(CONSTRUCTED),
  canregistercontroller(false),
  canregistersystem(true),
  runnumber(0), eventnumber(0), timestamp(0)
{}
	const QString name;
	const QString information;
	const QString contact_info;
	unsigned cloneid;
	std::set<IVP1System *> systems;
	QWidget * controller;
	State state;
	bool canregistercontroller;
	bool canregistersystem;
	int runnumber;
	unsigned long long eventnumber;
	unsigned timestamp;
};

//_______________________________________________________
IVP1ChannelWidget::IVP1ChannelWidget(const QString & n, const QString & i, const QString & c)
: QWidget(0), d(new Imp(n,i,c))
{
	d->state=CONSTRUCTED;
	d->cloneid = 0;
	d->controller = 0;
	d->canregistercontroller=false;
	d->canregistersystem=true;
	d->runnumber = 0;
	d->eventnumber = 0;
	setObjectName("IVP1ChannelWidget:"+n);
}

//_______________________________________________________
IVP1ChannelWidget::~IVP1ChannelWidget()
{
	VP1Msg::messageDebug("IVP1ChannelWidget::~IVP1ChannelWidget()");
	assert(d->state==UNCREATED||d->state==CONSTRUCTED);
	assert(!d->controller);
	//Delete systems:
	std::set<IVP1System *>::iterator it, itE = d->systems.end();
	for (it=d->systems.begin();it!=itE;++it) {
		assert((*it)->state()==IVP1System::UNCREATED||(*it)->state()==IVP1System::CONSTRUCTED);
		VP1Msg::messageDebug("deleting system: "+ (*it)->name());
		delete *it;
	}
	d->systems.clear();
	delete d;
}

//_______________________________________________________
QWidget * IVP1ChannelWidget::controllerWidget()
{
	return d->controller;
}

//_______________________________________________________
bool IVP1ChannelWidget::isAccumulator() const
{
	return false;
}

//_______________________________________________________
void IVP1ChannelWidget::setRunEvtNumber(int runnumber, unsigned long long eventnumber)
{
	d->runnumber = runnumber;
	d->eventnumber = eventnumber;
}

//_______________________________________________________
void IVP1ChannelWidget::setEvtTimestamp(unsigned timestamp)
{
	d->timestamp = timestamp;
}

//________________________________________________________
IVP1ChannelWidget::State IVP1ChannelWidget::state() const
{
	return d->state;
}

//________________________________________________________
void IVP1ChannelWidget::setState(const State&s)
{

#ifndef NDEBUG
	assert(s!=CONSTRUCTED);
	if (s==READY) {
		assert(d->state==CONSTRUCTED);
	}
	if (s==UNCREATED) {
		assert(d->state==READY);
	}
#endif
	d->state = s;
}

//_______________________________________________________
void IVP1ChannelWidget::create()
{
	assert(d->state==CONSTRUCTED);
}

//_______________________________________________________
void IVP1ChannelWidget::systemRefreshed(IVP1System*s)
{
	assert(d->state==READY);
	if( ! (s->state()==IVP1System::REFRESHED) ) {
		VP1Msg::messageVerbose("s->state() != IVP1System::REFRESHED!");
	}
	assert(s->state()==IVP1System::REFRESHED);
//	s = 0;//get rid of compile warning in opt mode
	VP1Msg::messageDebug("systemRefreshed: "+s->name());
}

//_______________________________________________________
void IVP1ChannelWidget::systemErased(IVP1System*s)
{
	assert(d->state==READY);
	bool isErased = (s->state()==IVP1System::ERASED) ? true : false;
	if(!isErased) {
		VP1Msg::messageVerbose("s->state() != IVP1System::ERASED!");
	}
	assert(isErased);
//	s = 0;//get rid of compile warning in opt mode
	VP1Msg::messageDebug("systemErased: "+s->name());
}

//_______________________________________________________
void IVP1ChannelWidget::uncreate()
{
	assert(d->state==READY);
}

//_______________________________________________________
void IVP1ChannelWidget::deleteControllers()
{
	std::set<IVP1System *>::iterator it, itE = d->systems.end();
	for (it=d->systems.begin();it!=itE;++it) {
		//    assert((*it)->state()==IVP1System::ERASED);
		if (*it)
			(*it)->deleteController();
	}
	if (d->controller)
		d->controller->deleteLater();
	d->controller = 0;
}

//_______________________________________________________
const QString IVP1ChannelWidget::unique_name() const
{
	return d->name+(d->cloneid?" ["+QString::number(d->cloneid)+"]":QString(""));//Fixme: make sure that it is forbidden to end a name with ' [.*]'!
}
//________________________________________________________
const QString& IVP1ChannelWidget::name() const
{
	return d->name;
}
//________________________________________________________
const QString& IVP1ChannelWidget::information() const
{
	return d->information;
}

//________________________________________________________
const QString& IVP1ChannelWidget::contact_info() const
{
	return d->contact_info;
}

//_______________________________________________________
unsigned IVP1ChannelWidget::cloneID() const
{
	return d->cloneid;
}

//_______________________________________________________
void IVP1ChannelWidget::setCloneID(unsigned id)
{
	d->cloneid=id;
	uniqueNameChanged(unique_name());
}

//_______________________________________________________
void IVP1ChannelWidget::registerController(QWidget*w)
{
	assert(d->canregistercontroller&&"Please only register controllers during create()");
	assert(d->state==CONSTRUCTED);
	assert(w&&"IVP1ChannelWidget::registerController(...) called with null pointer!!");
	assert(!d->controller&&"IVP1ChannelWidget::registerController(...) called twice!!");
	d->controller = w;
	w->setParent(0);
}

//_______________________________________________________
void IVP1ChannelWidget::registerSystem(IVP1System*s)
{
	assert(d->canregistersystem&&"Please only register systems during the channel constructor");
	assert(d->state==CONSTRUCTED);
	assert(d->systems.find(s)==d->systems.end()&&"IVP1ChannelWidget::registerSystem(...) called twice on the same system!!");
	d->systems.insert(s);
	s->setChannel(this);
}

//_______________________________________________________
const std::set<IVP1System *>& IVP1ChannelWidget::systems()
{
	return d->systems;
}

//_______________________________________________________
void IVP1ChannelWidget::turnOn(IVP1System*s)
{
	assert(s->channel()==this);
	assert(d->systems.find(s)!=d->systems.end());
	s->setActiveState(IVP1System::ON);
	emitRefreshInfoChanged();
}

//_______________________________________________________
void IVP1ChannelWidget::turnOff(IVP1System*s,const bool& immediateErase)
{
	assert(s->channel()==this);
	assert(d->systems.find(s)!=d->systems.end());
	s->setActiveState(IVP1System::OFF,!immediateErase);
	emitRefreshInfoChanged();
}

//_______________________________________________________
void IVP1ChannelWidget::getRunEvtNumber(int& runnumber, unsigned long long& eventnumber)
{
	runnumber = d->runnumber;
	eventnumber = d->eventnumber;
}

//_______________________________________________________
void IVP1ChannelWidget::getEvtTimestamp(unsigned& timestamp)
{
	timestamp = d->timestamp;
}

//_______________________________________________________
void IVP1ChannelWidget::emitRefreshInfoChanged()
{
	int nsysOn(0), nsysOnRefreshed(0);
	QString sysrefreshing;
	std::set<IVP1System *>::iterator it, itE = d->systems.end();
	for (it=d->systems.begin();it!=itE;++it) {
		if ((*it)->activeState()==IVP1System::ON) {
			++nsysOn;
			if((*it)->state()==IVP1System::REFRESHED)
				++nsysOnRefreshed;
			if ((*it)->isRefreshing()) {
				assert(sysrefreshing.isEmpty());
				sysrefreshing=(*it)->name();
			}
		}
	}
	//Fixme: Only emit if actually changed.
	systemRefreshInfoChanged(sysrefreshing, nsysOn, nsysOnRefreshed);
}

//_______________________________________________________
bool IVP1ChannelWidget::hasRefreshingSystem() {
	std::set<IVP1System *>::iterator it, itE = d->systems.end();
	for (it=d->systems.begin();it!=itE;++it) {
		if ((*it)->isRefreshing())
			return true;
	}
	return false;
}

//_______________________________________________________
void IVP1ChannelWidget::setCanRegister(const bool&c,const bool&s)
{
	d->canregistercontroller=c;
	d->canregistersystem=s;
}

//_______________________________________________________
QPixmap IVP1ChannelWidget::getSnapshot(bool transp, int width, bool batch)
{
	VP1Msg::messageVerbose("IVP1ChannelWidget::getSnapshot() - using QPixmap::grabWidget()");
	VP1Msg::messageVerbose("(and so, not using the values - transp: "+QString::number(transp)+" - width: "+QString::number(width)+" - batch: " + batch +")");

	return QPixmap::grabWidget( this );
}

//_______________________________________________________
void IVP1ChannelWidget::dockResized()
{
}

//_______________________________________________________
QByteArray IVP1ChannelWidget::saveState() {
	VP1Msg::messageVerbose("IVP1ChannelWidget base saveState called");
	return QByteArray();
}

//_______________________________________________________
void IVP1ChannelWidget::restoreFromState(QByteArray ba)
{
	VP1Msg::messageVerbose("IVP1ChannelWidget base restoreFromState called");
	if (!ba.isEmpty())
		message("Error in IVP1System::restoreFromState: Received non-empty saved state.");
}

//_______________________________________________________
void IVP1ChannelWidget::lastOfActiveSystemsRefreshed()
{
	VP1Msg::messageVerbose("IVP1ChannelWidget base lastOfActiveSystemsRefreshed called");
}
