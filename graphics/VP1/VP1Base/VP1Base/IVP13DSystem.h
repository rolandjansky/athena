/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class IVP13DSystem                     //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef IVP13DSYSTEM_H
#define IVP13DSYSTEM_H

#include "VP1Base/IVP1System.h"
#include <vector>//fixme
#include <set>
#include <QSet>

class SoPath;
class SoNode;
class SoSeparator;
class SoCamera;
class SoCooperativeSelection;
class SoQtViewer;
typedef void SoSelectionPathCB(void * data, SoPath * path);

class IVP13DSystem: public IVP1System {

  Q_OBJECT

public:

  virtual ~IVP13DSystem();

  /////////////////////////////////////////////////////////////////////////
  //  All of the following methods are intended to be used by channels:  //
  /////////////////////////////////////////////////////////////////////////

  IVP13DSystem(const QString & name, const QString & information, const QString & contact_info);

  // Access to the Scene Graph
  virtual SoSeparator *getSceneGraph() const=0;

  //If the system is to have access to cameras the channel should
  //register them here:
  void registerCamera(SoCamera *camera);//For registering cameras with a lifetime of the system

  //--> But if the camera is from a SoQtViewer, register the viewer
  //instead of the camera (since camera associated with viewer might
  //change when user toggles view mode):
  void registerViewer(SoQtViewer *viewer);//Will take current camera from viewer

  /////////////////////////////////////////////////////////////////////////
  //  These two methods are intended to be used in derived classes:      //
  /////////////////////////////////////////////////////////////////////////


  //Register SoCooperativeSelection nodes to get automatic calls to
  //userSelectedSingleNode, userDeselectedSingleNode and
  //userChangedSelection, to get timely redraws, and to get the
  //deselectAll() method to work for this selection also:
  void registerSelectionNode( SoCooperativeSelection* );
  void unregisterSelectionNode( SoCooperativeSelection* );
  //NB: These two methods calls ref() and unref() the passed selection node.

  //(Temporarily) disable/enable the invocation of
  //userSelectedSingleNode/userDeselectedSingleNode/userChangedSelectionuserSelectedcalls:
  void setUserSelectionNotificationsEnabled(SoCooperativeSelection*sel,bool enabled);//sel must already be registered

  //For SoCooperativeSelection with policy SINGLE:
  virtual void userSelectedSingleNode(SoCooperativeSelection*, SoNode* , SoPath*);
  virtual void userDeselectedSingleNode(SoCooperativeSelection*, SoNode* , SoPath*);
  //For SoCooperativeSelection with policy TOGGLE or SHIFT:
  virtual void userChangedSelection(SoCooperativeSelection*, QSet<SoNode*>, QSet<SoPath*>);
  //Tip: If you need to know which paths/nodes were just removed/added
  //then simply cache the last selection set somewhere and use
  //QSet::subtract() to find the difference.

  virtual void userClickedOnBgd();

  virtual void deselectAll(SoCooperativeSelection* exception_sel = 0);//Invoke deselectAll() on all registered selection
                                                                      //nodes for the system (except on exception_sel).
                                                                      //NB: This WILL invoke user selection methods above.

  //Get the cameras viewing this scene (e.g. for zooming):
  typedef std::set<SoCamera*> CamList;
  typedef CamList::iterator CamListItr;
  CamList getCameraList();
  //NB1: Might return empty set (usually if channel disallows camera
  //     access to this system)
  //NB2: Do not cache the result of this call - the correct set of
  //     cameras changes over time change later:

  //Override the following if you would like the channel to embed an
  //additional QWidget somewhere. This could for instance be a place
  //where you show data relevant to 3D objects from you system that
  //the user selected, etc. (Of course, the controller should be used
  //for *controls*. This widget is for displaying additional
  //information. buildExtraDisplayWidget() will be called once after
  //create():

  virtual QWidget * buildExtraDisplayWidget() { return 0; }

signals:
  void itemFromSystemSelected();//Should be emitted whenever item in this system is selected (mainly to give channel chance to show the system controller)

private slots:
  void activateClickedOutside();
private:
  IVP13DSystem( const IVP13DSystem & );
  IVP13DSystem & operator= ( const IVP13DSystem & );
  class Imp;
  Imp * m_d;

};

#endif
