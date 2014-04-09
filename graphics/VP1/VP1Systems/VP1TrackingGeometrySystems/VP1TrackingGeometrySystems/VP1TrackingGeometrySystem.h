/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1TRACKINGGEOMETRYSYSTEM_H
#define VP1TRACKINGGEOMETRYSYSTEM_H

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class VP1TrackingGeometrySystem                    //
//                                                                     //
//  Author: Andreas Salzburger <Andreas.Salzburger@cern.ch> (primary)  //
//          Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial version: June 2007                                         //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1Base/IVP13DSystemSimple.h"
#include "VP1Base/VP1ExtraSepLayerHelper.h"

#include <string>

class StoreGateSvc;
class SoSeparator;
class QWidget;

namespace Trk {
  class TrackingVolume;
  class Layer;
  class Surface;
}

class VP1TrackingGeometrySystem : public IVP13DSystemSimple {

  Q_OBJECT

public:
  VP1TrackingGeometrySystem();
  virtual ~VP1TrackingGeometrySystem();
  void buildPermanentSceneGraph(StoreGateSvc* detstore, SoSeparator *root);
  void buildEventSceneGraph(StoreGateSvc*, SoSeparator *){};
  QWidget * buildController();

private slots:
  void changeTopLevelView();
  void retrieveTrackingGeometry();
  
private:
  void processTrkVolume(const Trk::TrackingVolume*);
  void processMsVolume(const Trk::TrackingVolume*,VP1ExtraSepLayerHelper*,VP1ExtraSepLayerHelper*);
  void processTrkLayer(const Trk::Layer*,VP1ExtraSepLayerHelper*, unsigned int colorCode);
  void processTrkSurface(const Trk::Surface*, VP1ExtraSepLayerHelper*, unsigned int colorCode);
  void processMsLayDense(const Trk::TrackingVolume* tvol, VP1ExtraSepLayerHelper* sepHelper,VP1ExtraSepLayerHelper* layHelper, int color);
  
  QByteArray saveState();
  void restoreFromState( QByteArray ba);
    
  class Imp;
  Imp* d;
};

#endif
