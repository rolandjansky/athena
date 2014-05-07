/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1_CLOSEEVENTEATER_H
#define VP1_CLOSEEVENTEATER_H

#include <QObject> 
#include <QEvent> 
#include "VP1Base/VP1Controller.h"

class CloseEventEater : public QObject {
  Q_OBJECT

  public:
    CloseEventEater(VP1Controller* controller): m_controller(controller){}
    bool eventFilter(QObject *obj, QEvent *event) {
      if (event->type() == QEvent::Close) {
        m_controller->toggleDialogState(obj);
        return true;
      } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
      }
    }
  private:
    VP1Controller* m_controller;
  };

#endif
