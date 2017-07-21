/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1GeoTreeView                    //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: January 2008                             //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1GeometrySystems/VP1GeoTreeView.h"
#include <QTimer>

//____________________________________________________________________
class VP1GeoTreeView::Imp {
public:
  bool updatescheduled;
};


//____________________________________________________________________
VP1GeoTreeView::VP1GeoTreeView(QWidget * parent)
  : QTreeView(parent), d(new Imp)
{
  d->updatescheduled = false;
}

//____________________________________________________________________
VP1GeoTreeView::~VP1GeoTreeView()
{
  delete d;
}

//____________________________________________________________________
void VP1GeoTreeView::updateAllNonHiddenIndices()
{
  if (d->updatescheduled)
    d->updatescheduled = false;
  bool save = updatesEnabled();
  setUpdatesEnabled(false);

  //The following is a bit of a hack. Could probably be done better?
  QModelIndexList oldselection = selectionModel()->selectedIndexes();
  selectAll();
  QModelIndexList allindexes = selectionModel()->selectedIndexes();
  foreach ( QModelIndex idx, allindexes) {
    update(idx);
  }
  selectionModel()->clearSelection();
  foreach ( QModelIndex idx, oldselection )
    selectionModel()->select(idx,QItemSelectionModel::Select);


  if (save) {
    setUpdatesEnabled(true);
    update();
  }
}

//____________________________________________________________________
void VP1GeoTreeView::scheduleUpdateOfAllNonHiddenIndices()
{
  if (d->updatescheduled)
    return;
  QTimer::singleShot(0, this, SLOT(updateAllNonHiddenIndices()));
}
