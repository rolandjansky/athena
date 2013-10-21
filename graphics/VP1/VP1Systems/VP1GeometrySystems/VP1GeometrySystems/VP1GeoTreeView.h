/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1GeoTreeView                      //
//                                                            //
//  Description: ...                                          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: January 2008                             //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1GEOTREEVIEW_H
#define VP1GEOTREEVIEW_H

#include <QTreeView>

class VP1GeoTreeView : public QTreeView {

  Q_OBJECT

public:

  VP1GeoTreeView(QWidget * parent = 0);
  virtual ~VP1GeoTreeView();

public slots:
  void updateAllNonHiddenIndices();//Useful, but not particularly inexpensive to call.
  void scheduleUpdateOfAllNonHiddenIndices();//Will call the previous
					     //method after event
					     //queue is empty. Even if
					     //this method is called
					     //multiple times before
					     //that happens, only one
					     //update will be
					     //triggered.
private:

  class Imp;
  Imp * d;

};

#endif
