/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1TEXTBROWSER_H
#define VP1TEXTBROWSER_H

#include <QTextBrowser>

class VP1TextBrowser : public QTextBrowser {
public:

  VP1TextBrowser(QWidget * parent = 0);
  virtual ~VP1TextBrowser();

protected:
  void contextMenuEvent(QContextMenuEvent *);

};

#endif
