/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TrackLODManager                     //
//                                                            //
//  Description: Manager class for taking care of the LOD     //
//               nodes needed for associated track objects.   //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TRACKLODMANAGER_H
#define TRACKLODMANAGER_H

#include "VP1Base/VP1HelperClassBase.h"
#include "VP1TrackSystems/TrackCommonFlags.h"
#include <QObject>

//Fixme: Should this be a general utility class, to be used in the prd
//       system as well?

class TrackLODHandle;
class SoGroup;
class VP1ExtraSepLayerHelper;

class TrackLODManager : public QObject, public VP1HelperClassBase {

  Q_OBJECT

public:
  TrackLODManager( QObject * parent = 0, IVP1System * sys = 0);// sys != for messages in gui.
  virtual ~TrackLODManager();

  void setAttachNode(SoGroup* attachnode);
  void eraseEventData();

  TrackLODHandle * getLODHandle(int regionindex, const double& crossover_value);

public slots:
 void setDetailLevel(TrackCommonFlags::DETAILLEVEL);

private:

  class Imp;
  Imp * m_d;
  friend class TrackLODHandle;
};

//Handle class declaration:
class TrackLODHandle {
public:
  void addNodes(SoGroup* simple,SoGroup*detailed );
  void removeNodes(SoGroup* simple,SoGroup*detailed );
private:
  TrackLODHandle(SoGroup*,VP1ExtraSepLayerHelper*,VP1ExtraSepLayerHelper*,int, const double& );
  ~TrackLODHandle();
  TrackLODHandle(const TrackLODHandle & );
  TrackLODHandle & operator= (const TrackLODHandle & );
  friend class TrackLODManager;
  friend class TrackLODManager::Imp;
  class Imp;
  Imp * m_d;
};



#endif
