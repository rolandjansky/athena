/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1ExternalRequest               //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1EXTERNALREQUEST_H
#define VP1EXTERNALREQUEST_H

#include <QString>

class VP1ExternalRequest {
public:

  enum Action {SHOWTAB,SHOWCHANNEL,SHOWMESSAGE};
  QString actionToString(const Action&) const;

  VP1ExternalRequest(const QString& sender, const QString& message,
		     const Action& action,const QString& actioninfo);

  bool isValid() const { return m_valid; }

  Action action() const { return m_action; }
  QString actioninfo() const { return m_actioninfo; }
  QString message() const { return m_message; }
  QString sender() const { return m_sender; }

  QString serialize() const;
  VP1ExternalRequest(const QString& serialized);

  virtual ~VP1ExternalRequest() {}

  bool operator== ( const VP1ExternalRequest & other ) const;
  bool operator< ( const VP1ExternalRequest & other ) const;
private:

  QString m_message;
  QString m_sender;
  Action m_action;
  QString m_actioninfo;
  bool m_valid;
};

#endif
