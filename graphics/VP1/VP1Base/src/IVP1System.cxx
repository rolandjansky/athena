/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class IVP1System                     //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Base/IVP1System.h"
#include "VP1Base/IVP1ChannelWidget.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Base/VP1AthenaPtrs.h"

#include <QApplication>
#include <QWidget>

#include <cassert>
#include <iostream>

bool IVP1System::s_vp1verbose = VP1QtUtils::environmentVariableIsOn("VP1_VERBOSE_OUTPUT"); // TODO: is this used at all?

class IVP1System::Imp {
public:
  Imp(const QString & n, const QString & i, const QString & c)
    : name(n), information(i), contact_info(c), channel(0),
      allowupdategui(false), state(CONSTRUCTED), activeState(ON),
      controller(0),refreshing(false),
      canregistercontroller(true) {}
  const QString name;
  const QString information;
  const QString contact_info;
  IVP1ChannelWidget * channel;
  bool allowupdategui;
  State state;
  ActiveState activeState;
  QWidget * controller;
  bool refreshing;
  bool canregistercontroller;
};

//________________________________________________________
const QString& IVP1System::name() const
{
  return m_d->name;
}

//________________________________________________________
const QString& IVP1System::information() const
{
  return m_d->information;
}

//________________________________________________________
const QString& IVP1System::contact_info() const
{
  return m_d->contact_info;
}

//________________________________________________________
IVP1System::IVP1System(const QString & n, const QString & i, const QString & c)
  : m_d(new Imp(n,i,c))
{
  if (VP1Msg::verbose()){
    messageVerbose("IVP1System()");
  }
  setObjectName("IVP1System:"+n);

}

//________________________________________________________
IVP1System::~IVP1System()
{
  if (VP1Msg::verbose()){
    messageVerbose("IVP1System() Destructor. Start...");
  }

  assert(m_d->state==UNCREATED||m_d->state==CONSTRUCTED);
  assert(!m_d->controller);

  delete m_d;
  m_d=nullptr;
  VP1Msg::messageDebug("IVP1System() Destructor. END.");
}

//_______________________________________________________
void IVP1System::setChannel(IVP1ChannelWidget*cw)
{
  if (VP1Msg::verbose()) {
    messageVerbose("setChannel ");
    messageVerbose("setChannel m_d->state==CONSTRUCTED = "+QString(m_d->state==CONSTRUCTED?"true":"false"));
    messageVerbose("setChannel cw!=0 = "+QString(cw!=0?"true":"false"));
  }
  assert(!m_d->channel);
  assert(cw);
  assert(m_d->state==CONSTRUCTED);
  m_d->channel = cw;
}

//________________________________________________________
bool IVP1System::isRefreshing()
{
  return m_d->refreshing;
}

//________________________________________________________
void IVP1System::setRefreshing(const bool& b)
{
  if (VP1Msg::verbose()){
    messageVerbose("setRefreshing() called with b="+QString(b?"true":"false"));
  }
  if (b) {
    assert(m_d->state==ERASED);
  } else {
    assert(m_d->state==REFRESHED);
  }
  m_d->refreshing = b;
}


//________________________________________________________
IVP1System::State IVP1System::state() const
{
  return m_d->state;
}

//________________________________________________________
IVP1System::ActiveState IVP1System::activeState() const
{
  return m_d->activeState;
}

//________________________________________________________
void IVP1System::setState(const State&s)
{
#ifndef NDEBUG
  assert (m_d->state != s);
  assert(s!=CONSTRUCTED);
  if (s==REFRESHED) {
    assert(m_d->state==ERASED);
  }
  if (s==ERASED) {
    assert(m_d->state==REFRESHED||m_d->state==CONSTRUCTED);
  }
  if (s==UNCREATED) {
    assert(m_d->state==ERASED);
  }
#endif
  m_d->state = s;
  if (s==REFRESHED||s==ERASED)
    m_d->channel->emitRefreshInfoChanged();
}

//________________________________________________________
void IVP1System::setActiveState(const ActiveState&s,const bool& donttriggererase)
{
  //First handle case where we dont actually change state. Only
  //special consideration is OFF->OFF where we have to do something if
  //we need to erase:
  if (m_d->activeState==OFF&&s==OFF&&(m_d->state==REFRESHED||m_d->refreshing)&&!donttriggererase) {
    needErase();
    m_d->channel->emitRefreshInfoChanged();
    return;
  } else if (m_d->activeState==s) {
    m_d->channel->emitRefreshInfoChanged();
    return;
  }

  //Ok, we know that we are either ON->OFF or OFF->ON.
  m_d->activeState = s;

  if (s==ON) {
    //OFF->ON: We might need a refresh, so send out a signal for the scheduler:
    inactiveSystemTurnedActive();
  } else {
    //ON->OFF: We might need an erase signal:
    if ((m_d->state==REFRESHED||m_d->refreshing)&&!donttriggererase) {
      needErase();
    }
  }
  m_d->channel->emitRefreshInfoChanged();
}

//________________________________________________________
void IVP1System::uncreate()
{
  if (VP1Msg::verbose()) {
    messageVerbose("uncreate() base implementation");
    messageVerbose("registerController m_d->state==ERASED = "+QString(m_d->state==ERASED?"true":"false"));
  }
  assert(m_d->state==ERASED);
}

//________________________________________________________
QWidget * IVP1System::controllerWidget() {
  if (VP1Msg::verbose()) {
    messageVerbose("controllerWidget()");
    messageVerbose("registerController m_d->state==ERASED = "+QString(m_d->state==ERASED?"true":"false"));
    messageVerbose("registerController m_d->state==REFRESHED = "+QString(m_d->state==REFRESHED?"true":"false"));
  }
  assert(m_d->state==REFRESHED||m_d->state==ERASED);
  return m_d->controller;
 }

//_______________________________________________________
void IVP1System::deleteController()
{
  if (VP1Msg::verbose()){
    messageVerbose("deleteController()");
  }
  if (m_d->controller)
    m_d->controller->deleteLater();
  m_d->controller = 0;
}

//________________________________________________________
void IVP1System::registerController(QWidget*w)
{
  if (VP1Msg::verbose()) {
    messageVerbose("registerController ");
    messageVerbose("registerController m_d->canregistercontroller = "+QString(m_d->canregistercontroller?"true":"false"));
    messageVerbose("registerController m_d->state==CONSTRUCTED = "+QString(m_d->state==CONSTRUCTED?"true":"false"));
    messageVerbose("registerController m_d->controller==0 = "+QString(m_d->controller==0?"true":"false"));
    messageVerbose("registerController w!=0 = "+QString(w!=0?"true":"false"));
  }
  if (!m_d->canregistercontroller)
    message("ERROR: Please don't register controllers after create().");
  if (m_d->state!=CONSTRUCTED)
    message("ERROR: Please only register controllers in CONSTRUCTED state.");
  if (!w) {
    message("ERROR: Attempt to register null controller.");
    return;
  }
  if (m_d->controller) {
    message("ERROR: Attempt to register controller twice.");
    return;
  }
  m_d->controller = w;
  w->setParent(0);
}

//________________________________________________________
void IVP1System::disallowUpdateGUI()
{
  m_d->allowupdategui=false;
}

//________________________________________________________
void IVP1System::allowUpdateGUI()
{
  m_d->allowupdategui=true;
}

//________________________________________________________
void IVP1System::updateGUI() {
//	messageDebug("IVP1System::updateGUI() - START");
  //assert(m_d->allowupdategui);
  //assert(m_d->state==ERASED);

  if ( m_d->allowupdategui ) {
    qApp->processEvents();
  }

//  messageDebug("IVP1System::updateGUI() - END");
}

//________________________________________________________
IVP1ChannelWidget * IVP1System::channel() const
{
  if (VP1Msg::verbose()&&!m_d->channel){
    messageVerbose("WARNING channel() returning NULL");
  }
  assert(m_d->channel);
  return m_d->channel;
}

//_______________________________________________________
void IVP1System::setCanRegisterController(const bool&c)
{
  if (VP1Msg::verbose()){
    messageVerbose("setCanRegisterController called with"+QString(c?"true":"false"));
  }
  m_d->canregistercontroller=c;
}

//_______________________________________________________
QByteArray IVP1System::saveState() {
  if (VP1Msg::verbose()){
    messageVerbose("base saveState called [IVP1System]");
  }
  return QByteArray();
}

//_______________________________________________________
void IVP1System::restoreFromState(QByteArray ba)
{
  if (VP1Msg::verbose()){
    messageVerbose("base restoreFromState called");
  }
  if (!ba.isEmpty())
    message("Error in IVP1System::restoreFromState: Received non-empty saved state.");
}

//_______________________________________________________
StoreGateSvc * IVP1System::eventStore() const
{
  return VP1AthenaPtrs::eventStore();
}

//_______________________________________________________
StoreGateSvc * IVP1System::detectorStore() const
{
  return VP1AthenaPtrs::detectorStore();
}

//_______________________________________________________
ISvcLocator * IVP1System::serviceLocator() const
{
  return VP1AthenaPtrs::serviceLocator();
}

//_______________________________________________________
IToolSvc * IVP1System::toolSvc() const
{
  return VP1AthenaPtrs::toolSvc();
}

//_______________________________________________________
void IVP1System::message(const QString& str) const
{
  if (receivers(SIGNAL(sysmessage(QString))) > 0){
    sysmessage(str);
  }
  else{
    std::cout<<VP1Msg::prefix_msg()<<" ["<<m_d->name.toStdString()<<"]: "<<str.toStdString()<<std::endl;
  }
}

//_______________________________________________________
void IVP1System::messageDebug(const QString& str) const
{
  std::cout<<VP1Msg::prefix_debug()<<" ["<<m_d->name.toStdString()<<"]: "<<str.toStdString()<<std::endl;
}

//_______________________________________________________
void IVP1System::messageVerbose(const QString& str) const
{
  if (VP1Msg::verbose())
    std::cout<<VP1Msg::prefix_verbose()<<" ["<<m_d->name.toStdString()<<"]: "<<str.toStdString()<<std::endl;
}


//____________________________________________________________________
void IVP1System::message(const QStringList& l, const QString& addtoend ) const
{
  if (addtoend.isEmpty()) {
    foreach(QString s, l)
      message(s);
  } else {
    foreach(QString s, l)
      message(s+addtoend);
  }
}

//____________________________________________________________________
void IVP1System::messageDebug(const QStringList& l, const QString& addtoend ) const
{
  if (addtoend.isEmpty()) {
    foreach(QString s, l)
      messageDebug(s);
  } else {
    foreach(QString s, l)
      messageDebug(s+addtoend);
  }
}

//____________________________________________________________________
void IVP1System::messageVerbose(const QStringList& l, const QString& addtoend ) const
{
  if (!VP1Msg::verbose())
    return;
  if (addtoend.isEmpty()) {
    foreach(QString s, l)
      messageVerbose(s);
  } else {
    foreach(QString s, l)
      messageVerbose(s+addtoend);
  }
}

//____________________________________________________________________
void IVP1System::message(const QString& addtostart, const QStringList& l, const QString& addtoend ) const
{
  if (addtostart.isEmpty()) {
    message(l,addtoend);
    return;
  }
  if (addtoend.isEmpty()) {
    foreach(QString s, l)
      message(addtostart+s);
  } else {
    foreach(QString s, l)
      message(addtostart+s+addtoend);
  }
}

//____________________________________________________________________
void IVP1System::messageDebug(const QString& addtostart, const QStringList& l, const QString& addtoend ) const
{
  if (addtostart.isEmpty()) {
    messageDebug(l,addtoend);
    return;
  }
  if (addtoend.isEmpty()) {
    foreach(QString s, l)
      messageDebug(addtostart+s);
  } else {
    foreach(QString s, l)
      messageDebug(addtostart+s+addtoend);
  }
}

//____________________________________________________________________
void IVP1System::messageVerbose(const QString& addtostart, const QStringList& l, const QString& addtoend ) const
{
  if (!VP1Msg::verbose())
    return;
  if (addtostart.isEmpty()) {
    messageVerbose(l,addtoend);
    return;
  }
  if (addtoend.isEmpty()) {
    foreach(QString s, l)
      messageVerbose(addtostart+s);
  } else {
    foreach(QString s, l)
      messageVerbose(addtostart+s+addtoend);
  }
}

#ifdef BUILDVP1LIGHT
//____________________________________________________________________
xAOD::TEvent* IVP1System::getEvent(){
  return m_event;
}

void IVP1System::setEvent( xAOD::TEvent* event ){
  m_event = event;
}

//____________________________________________________________________
QStringList IVP1System::getObjectList(xAOD::Type::ObjectType type){
  if( type == xAOD::Type::Vertex ) {

      message("(((((((((())))))))))))))))");
      message(m_list[0]);
    return m_list[0];
  }
  else if ( type == xAOD::Type::Other ) {
    return m_list[1];
  }
  else if ( type == xAOD::Type::Jet ){
    return m_list[2];
  }
  else if ( type == xAOD::Type::CaloCluster ) {
    return m_list[3];
  }
  else if ( type == xAOD::Type::TrackParticle ) {
    return m_list[4];
  }
  else if ( type == xAOD::Type::Muon ) {
    return m_list[5];
  }
  else if ( type == xAOD::Type::Electron ) {
    return m_list[6];
  }
  else {
    message("Unknown xAOD type requested");
    return QStringList {};
  }
}

void IVP1System::setObjectList(QList<QStringList> list){
  m_list = list;
}


#endif // BUILDVP1LIGHT
