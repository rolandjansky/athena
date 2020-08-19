/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP12DExaminerViewer              //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//  Major update to support multiple views: July 2007      //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP12DEXAMINERVIEWER_H
#define VP12DEXAMINERVIEWER_H

// FIXME: need to include this before any QT header, because QT defines
// a macro `foreach' that collides with an identifier in AthenaKernel.
#include "xAODCore/BaseInfo.h"

#include <QList>
#include <QPair>
#include <QStringList>
#include <QWidget>

class VP1GraphicsView;

class VP12DExaminerViewer : public QWidget {

  Q_OBJECT

public:

  //The choice of constructor will determine whether this view is in single or multi view mode:

  //An examinerviewer with just one VP1GraphicsView:
  VP12DExaminerViewer(QWidget * parent = 0, Qt::WindowFlags f = 0);
  //An examinerviewer with several VP1GraphicsView's. First argument is a list of view_id's and icon urls for the views:
  VP12DExaminerViewer(const QList<QPair<QString,QString> >& views, QWidget * parent = 0, Qt::WindowFlags f = 0);
  virtual ~VP12DExaminerViewer();

  bool isSingleViewMode() const;

  VP1GraphicsView * view() const;//Returns the pointer to the view which is currently shown.
  VP1GraphicsView * view(const QString& view_id) const;//Returns pointer to view with a given view_id (0 if view_id not found or single view mode).
  QString currentView() const;//Only relevant in multiview mode (returns empty string otherwise)

  //These two return empty stringlists if we are in the mode with just one VP1GraphicsView:
  QStringList viewIDs() const;
  QList<QPair<QString,QString> > viewIDsAndIconUrls() const;

public slots:
  void setPickMode();
  void setChangeViewMode();
  void setSeekMode();
  void setShownView(QString);//Only relevant in multiview mode

signals:
  void currentViewAboutToChange(VP1GraphicsView*);//Emitted just before the view changes. Parameter is the view which will be shown.
  void currentViewChanged();//Just after the view changed.

private slots:
  void changeViewButtonClicked();
private:
  VP12DExaminerViewer( const VP12DExaminerViewer & );
  VP12DExaminerViewer & operator= ( const VP12DExaminerViewer & );
  class Imp;
  Imp * m_d;

};

#endif
