/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1Collection                       //
//                                                            //
//  Description: Base class for collection handles. Can be    //
//               displayed in VP1CollectionWidget's.          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: June 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1COLLECTION_H
#define VP1COLLECTION_H

#include "VP1Base/VP1HelperClassBase.h"

#include <QByteArray>
#include <QList>
#include <QMap>
#include <QString>
class QWidget;

typedef QMap<QByteArray,QByteArray> VP1CollStates;//persistifiable id->state

class VP1Collection : public VP1HelperClassBase {
public:
  VP1Collection( IVP1System * sys = 0, const QString& helperClassName="VP1Collection" );//sys!=0 for messages in gui
  virtual ~VP1Collection();

  QByteArray persistifiableID() const;//Could essentially contain a SG key, or, when multiple
                                      //types go in the same list, key+some integer giving the type
  QString section() const;//If all collections have empty sections, the collection widget won't make section headers

  QString sectionToolTip() const;//Tooltip to go with the section (NB. provide same tooltip in all instances
                                 // providing a given section. It is arbitrary which we use for the tooltip!)

  virtual QByteArray persistifiableState() const = 0;//Provide default implementation based on widget list + version
  virtual void setState(const QByteArray&) = 0;//Provide default implementation based on widget list + version
  virtual QList<QWidget*> widgetsForGuiRow() const;


protected:

  virtual QList<QWidget*> provideWidgetsForGuiRow() const = 0;//Should never return empty list.
  virtual QByteArray providePersistifiableID() const = 0; //Will be called exactly once.
  virtual QString provideSection() const = 0; //Will be called exactly once.
  virtual QString provideSectionToolTip() const { return ""; }//Will be called exactly once.

private:

  VP1Collection( const VP1Collection & );
  VP1Collection & operator= ( const VP1Collection & );
  class Imp;
  Imp * d;

public:

  ///////////////////////////////////////////////////////
  //  Static convenience methods for handling states:  //
  ///////////////////////////////////////////////////////

  static VP1CollStates getStates(QList<VP1Collection*>);
  static void applyStates(QList<VP1Collection*>, const VP1CollStates&);
  static void updateStates(VP1CollStates& state, const VP1CollStates& newInfo);//Similar entries in newInfo will override those in state.
  template <class T>
  static QList<VP1Collection*> toBaseCollList( const QList<T*>& );
};

///////////////
//  INLINES  //
///////////////

template <class T>
QList<VP1Collection*> VP1Collection::toBaseCollList( const QList<T*>& in)
{
  QList<VP1Collection*> l;
  foreach (T * t, in)
    l << static_cast<T*>(t);
  return l;
}

#endif
