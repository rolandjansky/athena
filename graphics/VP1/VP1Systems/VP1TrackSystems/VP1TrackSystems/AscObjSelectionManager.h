/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class AscObjSelectionManager              //
//                                                            //
//  Description: Manages selection of associated objects      //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef ASCOBJSELECTIONMANAGER_H
#define ASCOBJSELECTIONMANAGER_H

//Fixme: Make this class usable by prd system also. Maybe make common
//handle base class which tells simple/detailed nodes?

#include "VP1Base/VP1HelperClassBase.h"
#include <QObject>
#include <QList>

class AssociatedObjectHandleBase;
class IVP13DSystem;
class TrackSystemController;
class SoCooperativeSelection;
class SoSeparator;
class SoNode;
class SoPath;

//Lifetime of this object should be from system refresh to system erase.

class AscObjSelectionManager : public QObject, public VP1HelperClassBase {

  Q_OBJECT

public:
  //Will create selection nodes as appropriate under the event root.
  AscObjSelectionManager(SoSeparator* eventRoot,IVP13DSystem *, TrackSystemController*);
  void aboutTodelete();//Call this just before deletion (needed since signals should not be emitted from destructors).
  virtual ~AscObjSelectionManager();
  //Associated objects must all be attached under this separator:
  SoSeparator * getAscObjAttachSep() const;

  ///////////////////////////////////////////
  //Fixme: add method for changing mode of operation (single, toggle, shift+click)

  enum MODE { SINGLE, TOGGLE, SHIFT };
  QString toString(MODE) const;
  MODE mode() const;
  void setMode(MODE);

  void pretendUserClicked(AssociatedObjectHandleBase*);

  void ensureDeselected(const QList<AssociatedObjectHandleBase*>&);
  void ensureSelected(const QList<AssociatedObjectHandleBase*>&);//Call with length==1 if in single mode.
  void deselectAll();

  QList<AssociatedObjectHandleBase*> currentSelection() const;

  SoSeparator* eventRoot();

signals:
  void currentSelectionChanged(const QList<AssociatedObjectHandleBase*>&);

private:
  //For system:
  friend class VP1TrackSystem;
//   void setEventData( SoCooperativeSelection * sel_click, SoCooperativeSelection * sel_highlight );
//   void eraseEventData();
  //The following returns true if the passed selection node belongs to
  //this manager (if not, the system must handle the selection in
  //another way).
  bool handleUserSelectedSingleNode(SoCooperativeSelection*, SoNode* , SoPath*, AssociatedObjectHandleBase*&);//SINGLE
  void userClickedOnBgd();

//   bool handleUserDeselectedSingleNode(SoCooperativeSelection*, SoNode* , SoPath*);//SINGLE
//   bool handleUserChangedSelection(SoCooperativeSelection*, QSet<SoNode*>, QSet<SoPath*>);//TOGGLE/SHIFT
private:
  //For associated objects:
  friend class AssociatedObjectHandleBase;
  void registerAscObj(SoSeparator*simple,SoSeparator *detailed,AssociatedObjectHandleBase*);
  void unregisterAscObj(SoSeparator*simple,SoSeparator *detailed);
  //Fixme: When a handle changes pickable state to unpickable it
  //must be removed from selection list if selected!
private slots:
//  void ascObjDetailLevelAboutToChange();
  void ascObjDetailLevelChanged();
private:
  class Imp;
  Imp * m_d;
};

#endif
