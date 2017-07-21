/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1CollectionWidget                 //
//                                                            //
//  Description: Convenience widget which displays lists of   //
//               available collections.                       //
//               Is meant to be used with VP1Collection's.    //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: June 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1COLLECTIONWIDGET_H
#define VP1COLLECTIONWIDGET_H

#include "VP1Base/VP1Collection.h"
#include "VP1Base/VP1HelperClassBase.h"

#include <QWidget>
#include <QList>
#include <QString>


class VP1StdCollection;

class VP1CollectionWidget : public QWidget, public VP1HelperClassBase {

  Q_OBJECT

public:

  VP1CollectionWidget(QWidget * parent = 0);
  virtual ~VP1CollectionWidget();

  void clear( bool deleteCollections = true, bool deleteGuiElements = false );//Call at systemerase if collections are event data.
                                                                              //deleteGuiElements==true => the widgets supplied by
                                                                              //widgetsForGuiRow() will be deleted.
  void addStateInfo(const VP1CollStates&, bool overwritesExisting = true );
  void setCollections(QList<VP1Collection*>, bool applySavedStates = true );//call at refresh cycle.

  void addCollections(QList<VP1Collection*>, bool applySavedStates = true );//dynamically add new available collections (i.e. those appearing after refits)

  VP1CollStates states() const;

  QList<VP1Collection*> collections() const;

  int appropriateFixedWidth() const;

  //Convenience:
  template <class collT>
  void setCollections(QList<collT*> colls, bool applySavedStates = true ) {
    setCollections(VP1Collection::toBaseCollList(colls), applySavedStates);
  }
  //Convenience:
  template <class collT>
  void addCollections(QList<collT*> colls, bool applySavedStates = true ) {
    addCollections(VP1Collection::toBaseCollList(colls), applySavedStates);
  }

  //NB: If the cast from VP1Collection to collT fails, returned number
  //of collections might be less than the actual number of
  //collections.
  template <class collT>
  QList<collT*> collections() const {
    QList<collT*> l;
    foreach(VP1Collection*col,collections()) {
      collT* c = dynamic_cast<collT*>(col);
      if (c) l << c;
    }
    return l;
  }

protected:
  virtual void sortSections(QList<QString>&) {}//Reimplement and sort list if specific sorting is needed.
  //Utility methods for use in sortSections(..) reimplementations:
  static void ensureFirst(const QString& wildcard,QList<QString>&);
  static void ensureLast(const QString& wildcard,QList<QString>&);

//The next methods and signals are only interesting if (some of) your
//collections inherit from VP1StdCollection. In that case, it will let
//you know which and which types are currently visible, allowing for
//instance the controller to adapt itself:
public:
  QList<qint32> visibleStdCollectionTypes() const;
  QList<VP1StdCollection*> visibleStdCollections() const;
signals:
  void visibleStdCollectionTypesChanged(const QList<qint32>&);
  void visibleStdCollectionsChanged(const QList<VP1StdCollection*>&);
  void visibleContentsChanged();//We need it to resize scroll areas
protected slots:
  void possibleChange_visibleStdCollections();

private:
  VP1CollectionWidget( const VP1CollectionWidget & );
  VP1CollectionWidget & operator= ( const VP1CollectionWidget & );
  class Imp;
  Imp * d;
};

#endif
