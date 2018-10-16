/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1ExternalRequest             //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Base/VP1ExternalRequest.h"
#include <QStringList>
#include <cassert>

//____________________________________________________________________
VP1ExternalRequest::VP1ExternalRequest(const QString& sender, const QString& message,
				       const Action& action, const QString& actioninfo)
  : m_message(message), m_sender(sender), m_action(action), m_actioninfo(actioninfo)
{
  m_valid = true;
  if (m_sender.isEmpty())
    m_valid=false;
  else if (m_message.isEmpty())
    m_valid=false;
  else if (m_actioninfo.isEmpty()&&m_action!=SHOWMESSAGE)
    m_valid=false;
  else if (!m_actioninfo.isEmpty()&&m_action==SHOWMESSAGE)
    m_valid=false;
}

//____________________________________________________________________
QString VP1ExternalRequest::actionToString(const Action& a) const
{
  switch(a) {
  case SHOWTAB:
    return "SHOWTAB";
  case SHOWCHANNEL:
    return "SHOWCHANNEL";
  case SHOWMESSAGE:
    return "SHOWMESSAGE";
  default:
    assert(0&&"should never happen");
    return "Unknown action! (This should never happen)";
  }
}

//____________________________________________________________________
QString VP1ExternalRequest::serialize() const
{
  QString s("VP1ExternalRequestV1.0|");
  s+=actionToString(m_action);
  s+="|";
  QString tmp = m_actioninfo;
  tmp.replace("@","@1");
  tmp.replace("|","@2");
  s+=tmp;
  s+="|";
  tmp = m_message;
  tmp.replace("@","@1");
  tmp.replace("|","@2");
  s+=tmp;
  s+="|";
  tmp = m_sender;
  tmp.replace("@","@1");
  tmp.replace("|","@2");
  s+=tmp;
  return s;
}

//____________________________________________________________________
VP1ExternalRequest::VP1ExternalRequest(const QString& serialized)
{
  m_action=SHOWTAB;//To be sure we always assign to something

  m_valid=true;
  QStringList sl = serialized.split ("|");
  if (sl.count()!=5||sl.at(0)!="VP1ExternalRequestV1.0") {
    m_valid=false;
    m_action=SHOWMESSAGE;//To avoid coverity warning of uninit data member
  } else {
    if (sl.at(1)=="SHOWTAB")
      m_action=SHOWTAB;
    else if (sl.at(1)=="SHOWCHANNEL")
      m_action=SHOWCHANNEL;
    else if (sl.at(1)=="SHOWMESSAGE")
      m_action=SHOWMESSAGE;
    else m_valid = false;
    if (m_valid) {
      m_actioninfo=sl.at(2);
      m_message=sl.at(3);
      m_sender=sl.at(4);
      m_actioninfo.replace("@2","|");
      m_actioninfo.replace("@1","@");
      m_message.replace("@2","|");
      m_message.replace("@1","@");
      m_sender.replace("@2","|");
      m_sender.replace("@1","@");
      if (m_sender.isEmpty())
	m_valid=false;
      else if (m_message.isEmpty())
	m_valid=false;
      else if (m_actioninfo.isEmpty()&&m_action!=SHOWMESSAGE)
	m_valid=false;
      else if (!m_actioninfo.isEmpty()&&m_action==SHOWMESSAGE)
	m_valid=false;
    }
  }
}


//____________________________________________________________________
bool VP1ExternalRequest::operator== ( const VP1ExternalRequest & o ) const {

  return m_message==o.m_message
    && m_sender==o.m_sender
    && m_action==o.m_action
    && m_actioninfo==o.m_actioninfo
    && m_valid==o.m_valid;
}
//____________________________________________________________________
bool VP1ExternalRequest::operator< ( const VP1ExternalRequest & o ) const {

  if (m_sender==o.m_sender)
    return m_message<m_message;
  else
    return m_sender<m_sender;
}
