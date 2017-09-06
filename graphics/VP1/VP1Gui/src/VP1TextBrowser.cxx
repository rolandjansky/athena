/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "VP1Gui/VP1TextBrowser.h"

#include <QMenu>
#include <QContextMenuEvent>

//____________________________________________________________________
VP1TextBrowser::VP1TextBrowser(QWidget * parent)
  : QTextBrowser(parent)
{
}

//____________________________________________________________________
VP1TextBrowser::~VP1TextBrowser()
{
}

//____________________________________________________________________
void VP1TextBrowser::contextMenuEvent(QContextMenuEvent * e) {
  QMenu *menu = createStandardContextMenu();
  menu->addSeparator();
  QAction * clearAct = menu->addAction("C&lear all");
  QAction * zoominAct = menu->addAction("Zoom &in");
  QAction * zoomoutAct = menu->addAction("Zoom &out");

  QAction * selAct = menu->exec(e->globalPos());
  delete menu;
  if (selAct==clearAct)
    clear();
  else if (selAct==zoominAct)
    zoomIn();
  else if (selAct==zoomoutAct)
    zoomOut();

}
