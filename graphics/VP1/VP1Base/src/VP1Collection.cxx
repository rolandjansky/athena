/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1Collection                     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: June 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Base/VP1Collection.h"
#include "VP1Base/VP1Msg.h"


//____________________________________________________________________
class VP1Collection::Imp {
public:
  Imp() : persistIDProvided(false),
	  sectionProvided(false),
	  sectionToolTipProvided(false),
	  widgetsForGuiRowProvided(false) {}
  mutable QByteArray persistID;
  bool persistIDProvided;
  QString section;
  QString sectionToolTip;
  bool sectionProvided;
  bool sectionToolTipProvided;
  QList<QWidget*> widgetsForGuiRow;
  bool widgetsForGuiRowProvided;
};


//____________________________________________________________________
VP1Collection::VP1Collection(IVP1System * sys,const QString& helperClassName)
  : VP1HelperClassBase(sys,helperClassName), d(new Imp)
{
}

//____________________________________________________________________
VP1Collection::~VP1Collection()
{
  delete d;
}

//____________________________________________________________________
QByteArray VP1Collection::persistifiableID() const
{
	VP1Msg::messageDebug("VP1Collection::persistifiableID()");

  if (!d->persistIDProvided) {
    d->persistIDProvided = true;
    d->persistID = providePersistifiableID().toHex();//toHex() necessary for some unknown reason!
                                                     //Without it keys in maps are apparently considered similar! (Qt bug??)
  }
  return d->persistID;
}

//____________________________________________________________________
QList<QWidget*> VP1Collection::widgetsForGuiRow() const
{
  if (!d->widgetsForGuiRowProvided) {
    d->widgetsForGuiRowProvided = true;
    d->widgetsForGuiRow = provideWidgetsForGuiRow();
  }
  return d->widgetsForGuiRow;
}

//____________________________________________________________________
QString VP1Collection::section() const
{
  if (!d->sectionProvided) {
    d->sectionProvided = true;
    d->section = provideSection();
  }
  return d->section;
}

//____________________________________________________________________
QString VP1Collection::sectionToolTip() const
{
  if (!d->sectionToolTipProvided) {
    d->sectionToolTipProvided = true;
    d->sectionToolTip = provideSectionToolTip();
  }
  return d->sectionToolTip;
}

//____________________________________________________________________
VP1CollStates VP1Collection::getStates(QList<VP1Collection*> cols)
{
	VP1Msg::messageDebug("VP1Collection::getStates() - start...");

	VP1CollStates states;
  foreach (VP1Collection* col,cols) {
	  VP1Msg::messageDebug("inserting collection: " + col->section() );
    states.insert(col->persistifiableID(),col->persistifiableState());
  }

  VP1Msg::messageDebug("VP1Collection::getStates() - end.");

  return states;
}

//____________________________________________________________________
void VP1Collection::applyStates(QList<VP1Collection*> cols, const VP1CollStates& states)
{
  foreach (VP1Collection* col,cols)
    if (states.contains(col->persistifiableID()))
      col->setState(states.value(col->persistifiableID()));
}

//____________________________________________________________________
void VP1Collection::updateStates(VP1CollStates& state, const VP1CollStates& newInfo)
{
	VP1Msg::messageDebug("VP1Collection::updateStates() - start...");
  QMapIterator<QByteArray,QByteArray> it(newInfo);
  while (it.hasNext()) {
    it.next();
    state.insert(it.key(),it.value());
  }
	VP1Msg::messageDebug("VP1Collection::updateStates() - end.");
}
