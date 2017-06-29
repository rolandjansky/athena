/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1DockWidget                    //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1DOCKWIDGET_H
#define VP1DOCKWIDGET_H

#include <QDockWidget>

class IVP1ChannelWidget;
class VP1TabManager;
class QResizeEvent;

class VP1DockWidget : public QDockWidget {

  Q_OBJECT

public:

  VP1DockWidget ( IVP1ChannelWidget * cw, VP1TabManager* tm );
  virtual ~VP1DockWidget();

  void setSelected();
  void setUnselected();
  bool isSelected() const;

  IVP1ChannelWidget * channelWidget() const;

  //For dropping in and out of fullscreen:
  void ensureCWHasNoParent();
  void ensureCWHasParent();

  static QString highlightStyle();//Here so that other parts of the gui can use the same style for highlighting.

signals:
  void wasSelected(VP1DockWidget*);
private slots:
  void systemRefreshInfoChanged(QString sysrefreshing, int nsysOn, int nsysOnRefreshed);
  void updateTitle();
protected:
  void contextMenuEvent ( QContextMenuEvent * event );
  void resizeEvent ( QResizeEvent * event );
protected:
  class Imp;
  Imp * d;
};

#endif
