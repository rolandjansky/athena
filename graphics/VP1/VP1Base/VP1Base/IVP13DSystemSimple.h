/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class IVP13DSystemSimple               //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef IVP13DSYSTEMSIMPLE_H
#define IVP13DSYSTEMSIMPLE_H

#include "VP1Base/IVP13DSystem.h"

class SoSeparator;
class SoNode;
class SoPath;

class IVP13DSystemSimple : public IVP13DSystem {

  Q_OBJECT

public:

  ////////////////////////////////////////////////////////////////////////
  //////////////  This is the interesting bit for those     //////////////
  //////////////  who are deriving from this class...       //////////////
  ////////////////////////////////////////////////////////////////////////

  IVP13DSystemSimple(const QString & name, const QString & information, const QString & contact_info);

  virtual void systemcreate(StoreGateSvc* detstore);//Optional.

  //If you need a controller - create it and return it from a
  //reimplementation of this method (it will only be called
  //once). This method will be called before both systemcreate() and
  //restoreFromState().
  virtual QWidget * buildController();

  void ensureBuildController();//Rarely you might wish to call this if you need to access
                               //controller elements in methods that might be called earlier than ::create().

  //If some part of the scene should stay for the entire run, reimplement and append scene to root, :
  virtual void buildPermanentSceneGraph(StoreGateSvc* detstore,SoSeparator *root);

  //Reimplement and build a scenegraph for a given event based on the contents of storegate:
  virtual void buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root)=0;

  void updateGUI();//Call this occasionally in buildEventSceneGraph(...) to avoid GUI freezing.

  //Reimplement if you need to take action when the user picks an object with the mouse:
  virtual void userPickedNode(SoNode* pickedNode, SoPath *pickedPath);
  //NB: This convenient method will often suffice for simple
  //systems. Systems requiring more advanced selection/deselection
  //functionality should insert SoCooperativeSelection nodes in their
  //scenegraphs and use the methods of IVP13Dsystem to handle them.

  virtual void systemerase();//Optional - for erasing maps etc. with event data. //Fixme: make pure virtual (i.e. require!.. or make warning??)

  virtual void systemuncreate();//Optional.

  ////////////////////////////////////////////////////////////////////////
  ////////////  The rest of this file is just internal stuff  ////////////
  ////////////           - dont worry about it -              ////////////
  ////////////////////////////////////////////////////////////////////////

  virtual ~IVP13DSystemSimple();

  //Call this at any point to get warnings about nodes where
  //notifications have been mistakenly been left off:
  void warnOnDisabledNotifications() const;

private:
  class Imp;
  Imp *m_d;
  SoSeparator *getSceneGraph() const;
  void create(StoreGateSvc* detstore);
  void refresh(StoreGateSvc* storegate);
  void erase();
  void uncreate();
};

/////////// Inlines /////////////
inline void IVP13DSystemSimple::updateGUI() { IVP13DSystem::updateGUI(); }

#endif
