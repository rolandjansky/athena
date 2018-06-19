/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1IncomingMessageDialog       //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "ui_vp1incomingmessagedialog.h"
#include "VP1Gui/VP1IncomingMessageDialog.h"
#include "VP1Gui/VP1TabManager.h"
#include "VP1Gui/VP1ChannelManager.h"

#include <QList>
#include <QMessageBox>

#include <cassert>

//____________________________________________________________________
class VP1IncomingMessageDialog::Imp {
public:
  Imp(const VP1ExternalRequest& r) : tabmanager{}, channelmanager{}, requestqueue{},request(r),
    lcdNumber_pending{}, lcdNumber_pendingsender{}, lcdNumber_pendingexactsame{},
    blockallmessages{}, pushButton_allclear{}, pushButton_senderclear{},
    pushButton_messageclear{}, messages_blockedsenders{}, messages_blockedexactmessages{}
  {
   //nop
  }
  VP1TabManager* tabmanager;
  VP1ChannelManager* channelmanager;
  QQueue<VP1ExternalRequest>* requestqueue;
  VP1ExternalRequest request;
  QLCDNumber *lcdNumber_pending;
  QLCDNumber *lcdNumber_pendingsender;
  QLCDNumber *lcdNumber_pendingexactsame;
  bool *blockallmessages;
  QPushButton * pushButton_allclear;
  QPushButton * pushButton_senderclear;
  QPushButton * pushButton_messageclear;
  QList<QString>* messages_blockedsenders;
  QList<VP1ExternalRequest>* messages_blockedexactmessages;
};

//____________________________________________________________________
VP1IncomingMessageDialog::VP1IncomingMessageDialog(const VP1ExternalRequest& r,
						   QQueue<VP1ExternalRequest>* rq,
						   bool *blockallmessages,
						   QList<QString>* messages_blockedsenders,
						   QList<VP1ExternalRequest>* messages_blockedexactmessages,
						   VP1TabManager* tm,
						   VP1ChannelManager* cm,
						   QWidget *parent)
  : QDialog(parent), m_d(new Imp(r))
{
  m_d->tabmanager= tm;
  m_d->channelmanager= cm;
  m_d->requestqueue = rq;
  m_d->blockallmessages=blockallmessages;
  m_d->messages_blockedsenders=messages_blockedsenders;
  m_d->messages_blockedexactmessages=messages_blockedexactmessages;
  Ui::VP1IncomingMessageForm ui;
  ui.setupUi(this);

  m_d->pushButton_allclear = ui.pushButton_allclear;
  m_d->pushButton_senderclear = ui.pushButton_senderclear;
  m_d->pushButton_messageclear = ui.pushButton_messageclear;

  m_d->lcdNumber_pending = ui.lcdNumber_pending;
  m_d->lcdNumber_pendingsender = ui.lcdNumber_pendingsender;
  m_d->lcdNumber_pendingexactsame = ui.lcdNumber_pendingexactsame;
  updatependinginfo();

  connect(ui.pushButton_allclear,SIGNAL(clicked()),this,SLOT(request_allclear()));
  connect(ui.pushButton_allblock,SIGNAL(clicked()),this,SLOT(request_allblock()));
  connect(ui.pushButton_senderclear,SIGNAL(clicked()),this,SLOT(request_senderclear()));
  connect(ui.pushButton_senderblock,SIGNAL(clicked()),this,SLOT(request_senderblock()));
  connect(ui.pushButton_messageblock,SIGNAL(clicked()),this,SLOT(request_messageblock()));
  connect(ui.pushButton_messageclear,SIGNAL(clicked()),this,SLOT(request_messageclear()));

  connect(ui.pushButton_showtab_gothere,SIGNAL(clicked()),this,SLOT(request_gothere()));

  ui.textBrowser_message->setText(m_d->request.message());
  ui.label_header->setText(ui.label_header->text().replace("SENDERPLACEHOLDER",m_d->request.sender()));
  if (m_d->request.action()==VP1ExternalRequest::SHOWMESSAGE) {
    ui.widget_showtab->hide();
  } else if (m_d->request.action()==VP1ExternalRequest::SHOWTAB) {
    ui.label_showtab->setText(ui.label_showtab->text().replace("DUMMYTABNAME",m_d->request.actioninfo()));
    if (m_d->tabmanager->hasTab(m_d->request.actioninfo())) {
      ui.label_showtaberror->hide();
    } else {
      ui.pushButton_showtab_gothere->hide();
    }
  } else if (m_d->request.action()==VP1ExternalRequest::SHOWCHANNEL) {
    ui.label_showtab->setText(ui.label_showtab->text().replace("tab","channel"));
    ui.label_showtab->setText(ui.label_showtab->text().replace("DUMMYTABNAME",m_d->request.actioninfo()));

    if (m_d->channelmanager->channelWithBasenameIsLoaded(m_d->request.actioninfo())) {
      ui.label_showtaberror->hide();
    } else {
      ui.label_showtaberror->setText(ui.label_showtaberror->text().replace("tab","channel"));
      ui.pushButton_showtab_gothere->hide();
    }

  } else {
    assert(0&&"This should never happen unless you did not hardcode all action types!");
  }
}

//____________________________________________________________________
VP1IncomingMessageDialog::~VP1IncomingMessageDialog()
{
  delete m_d; m_d=0;
}

//____________________________________________________________________
void VP1IncomingMessageDialog::request_gothere()
{
 if (m_d->request.action()==VP1ExternalRequest::SHOWTAB) {
   m_d->tabmanager->showTab(m_d->request.actioninfo());
 } else if (m_d->request.action()==VP1ExternalRequest::SHOWCHANNEL) {
   m_d->tabmanager->showFirstChannelWithGivenBasename(m_d->request.actioninfo());
 }
}

//____________________________________________________________________
void VP1IncomingMessageDialog::updatependinginfo()
{
  m_d->lcdNumber_pending->display(m_d->requestqueue->count());
  int nsender(0);
  foreach (VP1ExternalRequest er, *(m_d->requestqueue)) {
    if (er.sender()==m_d->request.sender())
      ++nsender;
  }
  m_d->lcdNumber_pendingsender->display(nsender);
  int nequal(0);
  foreach (VP1ExternalRequest er, *(m_d->requestqueue)) {
    if (m_d->request==er)
      ++nequal;
  }
  m_d->lcdNumber_pendingexactsame->display(nequal);


  m_d->pushButton_allclear->setEnabled(m_d->requestqueue->count()>0);
  m_d->pushButton_senderclear->setEnabled(nsender>0);
  m_d->pushButton_messageclear->setEnabled(nequal>0);
}

//____________________________________________________________________
void VP1IncomingMessageDialog::request_allclear()
{
  m_d->requestqueue->clear();
  updatependinginfo();
}

//____________________________________________________________________
void VP1IncomingMessageDialog::request_allblock()
{
  int ret = QMessageBox::question(this, "VP1 - Block all incoming messages?",
				  "This will block all incoming messages to the application.\nProceed?",
				  QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel );
  if (ret!=QMessageBox::Ok)
    return;
  *(m_d->blockallmessages)=true;
  request_allclear();
}

//____________________________________________________________________
void VP1IncomingMessageDialog::request_senderclear()
{
  QList<VP1ExternalRequest> tmp;
  foreach (VP1ExternalRequest er, *(m_d->requestqueue)) {
    if (!tmp.contains(er)&&er.sender()==m_d->request.sender())
      tmp<<er;
  }
  foreach (VP1ExternalRequest er, tmp) {
    m_d->requestqueue->removeAll (er);
  }
  updatependinginfo();
}

//____________________________________________________________________
void VP1IncomingMessageDialog::request_senderblock()
{
  *(m_d->messages_blockedsenders)<<m_d->request.sender();
  request_senderclear();
}

//____________________________________________________________________
void VP1IncomingMessageDialog::request_messageclear()
{
  QList<VP1ExternalRequest> tmp;
  foreach (VP1ExternalRequest er, *(m_d->requestqueue)) {
    if (!tmp.contains(er)&&er==m_d->request)
      tmp<<er;
  }
  foreach (VP1ExternalRequest er, tmp) {
    m_d->requestqueue->removeAll(er);
  }
  updatependinginfo();
}

//____________________________________________________________________
void VP1IncomingMessageDialog::request_messageblock()
{
  *(m_d->messages_blockedexactmessages)<<m_d->request;
  request_messageclear();
}
