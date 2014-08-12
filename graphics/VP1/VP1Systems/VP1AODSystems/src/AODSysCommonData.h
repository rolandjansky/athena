/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class AODSysCommonData                  //
//                                                            //
//  Description: Common pointers, data, node-to-object maps   //
//               etc. for the AOD system                    //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef AODSYSCOMMONDATA_H
#define AODSYSCOMMONDATA_H

#include "VP1AODSystems/VP1AODSystem.h"
#include "VP1Base/VP1HelperClassBase.h"
#include "VP1Base/VisibleObjectToMaterialHelper.h"
#include "VP1Utils/HitsSoNodeManager.h"

class IVP13DSystem;
class AODSystemController;
class SoNode;
class IParticleHandleBase;
class SoPointSet;
class QTreeWidgetItem;
class SoSeparator;//Fixme!
class TrackPropagationHelper;

namespace xAOD {
	class IParticle;
}

class AODSysCommonData : public VP1HelperClassBase {
public:

  AODSysCommonData(VP1AODSystem * sys,AODSystemController *);
  virtual ~AODSysCommonData();

  VP1AODSystem * system() const;
  AODSystemController * controller() const;

  TrackPropagationHelper * trackPropagationHelper() const;

  SoPointSet * singlePoint() const;//A single point at (0,0,0)

  //register node->track connection (erased at next clearEventData)
  void registerHandle(SoNode*,IParticleHandleBase*);
  void unregisterHandle(SoNode*);
  
  //get the connection:
  IParticleHandleBase* handle(SoNode*n);
  SoNode* node(IParticleHandleBase* h); //!< Returns the node associated with this handle. Returns zero if no match found.
  SoNode* node(QTreeWidgetItem* item); //!< Returns the node associated with this item. Returns zero if no match found.
  
  void setLastSelectedTrack(IParticleHandleBase* sel);
  IParticleHandleBase* lastSelectedTrackHandle();//!< pointer to base of last selected track (zero if nothing selected)
  SoSeparator* textSep(); // FIXME! FIXME!
  SoSeparator* m_textSep; // FIXME! FIXME! Make private, but then need set method for VP1AODSystem.cxx:247

  const IParticleHandleBase* getHandle(const xAOD::IParticle* trk);
  
private:
  class Imp;
  Imp * d;

  VP1AODSystem * m_3dsystem;
  AODSystemController * m_controller;
  SoPointSet * m_singlePoint;
  IParticleHandleBase* m_lastSelectedTrack;
};

inline VP1AODSystem* AODSysCommonData::system() const { return m_3dsystem; }
inline AODSystemController * AODSysCommonData::controller() const { return m_controller; }
inline SoPointSet * AODSysCommonData::singlePoint() const { return m_singlePoint; }
inline void AODSysCommonData::setLastSelectedTrack( IParticleHandleBase* sel) {m_lastSelectedTrack=sel;}
inline IParticleHandleBase* AODSysCommonData::lastSelectedTrackHandle() {return m_lastSelectedTrack;}
inline SoSeparator* AODSysCommonData::textSep() {return m_textSep;} // FIXME! FIXME!

#endif

