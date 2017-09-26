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
  : VP1HelperClassBase(sys,helperClassName), m_d(new Imp)
{
}

//____________________________________________________________________
VP1Collection::~VP1Collection()
{
  delete m_d;
}

//____________________________________________________________________
QByteArray VP1Collection::persistifiableID() const
{
	VP1Msg::messageDebug("VP1Collection::persistifiableID()");

  if (!m_d->persistIDProvided) {
    m_d->persistIDProvided = true;
    m_d->persistID = providePersistifiableID().toHex();//toHex() necessary for some unknown reason!
                                                     //Without it keys in maps are apparently considered similar! (Qt bug??)
  }
  return m_d->persistID;
}

//____________________________________________________________________
QList<QWidget*> VP1Collection::widgetsForGuiRow() const
{
  if (!m_d->widgetsForGuiRowProvided) {
    m_d->widgetsForGuiRowProvided = true;
    m_d->widgetsForGuiRow = provideWidgetsForGuiRow();
  }
  return m_d->widgetsForGuiRow;
}

//____________________________________________________________________
QString VP1Collection::section() const
{
  if (!m_d->sectionProvided) {
    m_d->sectionProvided = true;
    m_d->section = provideSection();
  }
  return m_d->section;
}

//____________________________________________________________________
QString VP1Collection::sectionToolTip() const
{
  if (!m_d->sectionToolTipProvided) {
    m_d->sectionToolTipProvided = true;
    m_d->sectionToolTip = provideSectionToolTip();
  }
  return m_d->sectionToolTip;
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
