/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class  VP1Authenticator                   //
//                                                            //
//  Description:    ...                                       //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1AUTHENTICATOR_H
#define VP1AUTHENTICATOR_H

#include "ui_vp1authenticator.h"

#include <QObject>
#include <QDialog>
#include <QNetworkReply>


class QNetworkAccessManager;

class VP1Authenticator : public QDialog, public Ui::dlgAuthentication
{
  Q_OBJECT

public:

  VP1Authenticator(QWidget*,QString);
  virtual ~VP1Authenticator();

  bool connectToServer();
  QNetworkAccessManager* networkAccessManager();

signals:
  void authenticationSuccessful(QNetworkAccessManager*);

private slots:
  // ---- Https/SSL slots ----
  // Slots of the QNetworkReply
  void finished();
  void error(QNetworkReply::NetworkError);
  void sslErrors(const QList<QSslError>&);

  // GUI slots
  void loginClicked();

private:
  class Imp;
  Imp*  d;
};

#endif

