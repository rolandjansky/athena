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
// #include "AODHandleSelectionManager.h"
class IVP13DSystem;
class AODSystemController;
class SoNode;
class SoPath;
class IParticleHandleBase;
class SoPointSet;
class QTreeWidgetItem;
class SoSeparator;//Fixme!
class AODHandleBase;


namespace xAOD {
	class IParticle;
}

namespace Trk {
  class IExtrapolationEngine;
}

class AODSysCommonData : public VP1HelperClassBase {
public:

  AODSysCommonData(VP1AODSystem * sys,AODSystemController *);
  virtual ~AODSysCommonData();

  VP1AODSystem * system() const;
  AODSystemController * controller() const;

  SoPointSet * singlePoint() const;//A single point at (0,0,0)

  Trk::IExtrapolationEngine * extrapolator() const { return m_extrapolator; }
  void setExtrapolator(Trk::IExtrapolationEngine * e) { m_extrapolator=e; }
  
  // AODHandleSelectionManager* selectionManager() const { return m_selManager; }
  // void setSelManager(AODHandleSelectionManager* m) {m_selManager=m;}
  // void deleteSelManager() { delete m_selManager; m_selManager=0; }
  
  // Called inside AODHandleBase. 
  void registerHandle( AODHandleBase* h );
  void deregisterHandle( AODHandleBase* h );
  AODHandleBase* getHandleFromNode( SoNode* node );
  SoNode* getNodeFromBrowser( QTreeWidgetItem* item );
  AODHandleBase* getHandleFromNode(const SoPath*path);
  
  void setLastSelectedHandle (AODHandleBase* h);
  
private:
  class Imp;
  Imp * d;

  VP1AODSystem * m_3dsystem;
  AODSystemController * m_controller;
  SoPointSet * m_singlePoint;
  Trk::IExtrapolationEngine* m_extrapolator;
  // AODHandleSelectionManager* m_selManager;
};

inline VP1AODSystem* AODSysCommonData::system() const { return m_3dsystem; }
inline AODSystemController * AODSysCommonData::controller() const { return m_controller; }
inline SoPointSet * AODSysCommonData::singlePoint() const { return m_singlePoint; }

#endif

