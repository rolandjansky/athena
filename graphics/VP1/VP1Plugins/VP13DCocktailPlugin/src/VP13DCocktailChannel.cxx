/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP13DCocktailChannel           //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: June 2007                             //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP13DCocktailPlugin/VP13DCocktailChannel.h"
#include "VP1GeometrySystems/VP1GeometrySystem.h"
// #include "VP1MissingEtSystems/VP1MissingEtSystem.h"
#include "VP1SimHitSystems/VP1SimHitSystem.h"
#include "VP1CaloSystems/VP1CaloCellSystem.h"
#include "VP1TrackingGeometrySystems/VP1TrackingGeometrySystem.h"

/*
#ifndef __APPLE__
#include "VP1TriggerLV1Systems/VP1TrigROISystem.h"
#include "VP1TriggerLV1Systems/VP1TriggerGeometrySystem.h"
#endif
*/

#include "VP1TrackSystems/VP1TrackSystem.h"
#include "VP1GuideLineSystems/VP1GuideLineSystem.h"
//#include "VP1JetSystems/VP1JetSystem.h"
// #include "VP1VertexSystems/VP1VertexSystem.h"
#include "VP1PRDSystems/VP1PrepRawDataSystem.h"
#include "VP1RawDataSystems/VP1RawDataSystem.h"
#include "VP1UtilitySystems/VP1ExtraInputSystem.h"
#include "VP1UtilitySystems/VP1PartSpectSystem.h"
//#include "VP1FatrasSystems/VP1FatrasSingleTrackSimSystem.h" // TODO: to be updated when the new Fatras will be available
// #include "VP1CaloClusterSystems/VP1CaloClusterSystem.h"
#include "VP1PlugUtils/VP1SysConf.h"
#include "VP1CaloReadoutSystems/VP1CaloReadoutSystem.h"
#include "VP1BPhysSystems/VP1BPhysSystem.h"
#include "VP1AODSystems/VP1AODSystem.h"


VP13DCocktailChannel::VP13DCocktailChannel()
  : IVP13DStandardChannelWidget(VP1CHANNELNAMEINPLUGIN(VP13DCocktailChannel,"3DCocktail"),
				"This channel shows essentially all functional 3D systems.",
				"Riccardo.Maria.Bianchi@cern.ch, Thomas.Kittelmann@cern.ch")
{
}

void VP13DCocktailChannel::init()
{
  VP1GuideLineSystem * guidelinesystem = new VP1GuideLineSystem;
  addSystem(guidelinesystem);
  VP1GeometrySystem * geosys = new VP1GeometrySystem(VP1GeoFlags::AllMuonChambers);
  addSystem(geosys);


  /* TODO: to be updated when the new Fatras will be available
  if (VP1FatrasSingleTrackSimSystem::singleTrackSimAlgorithmIsAvailable())
    addSystem(new VP1FatrasSingleTrackSimSystem);
  */

  VP1TrackSystem * tracksys = new VP1TrackSystem;
  addSystem(tracksys,IVP13DStandardChannelWidget::StartDisabled);

  VP1PrepRawDataSystem * prdsys = new VP1PrepRawDataSystem;
  addSystem(prdsys, IVP13DStandardChannelWidget::StartDisabled);

  addSystem(new VP1CaloCellSystem,IVP13DStandardChannelWidget::StartDisabled);
  // addSystem(new VP1CaloClusterSystem,IVP13DStandardChannelWidget::StartDisabled);
  //addSystem(new VP1JetSystem,IVP13DStandardChannelWidget::StartDisabled); // TODO: has to be updated with the new xAOD Jet class
  // addSystem(new VP1MissingEtSystem,IVP13DStandardChannelWidget::StartDisabled);
  addSystem(new VP1RawDataSystem, IVP13DStandardChannelWidget::StartDisabled);
  addSystem(new VP1SimHitSystem,IVP13DStandardChannelWidget::StartDisabled);
  addSystem(new VP1TrackingGeometrySystem, IVP13DStandardChannelWidget::StartDisabled);
/*
#ifndef __APPLE__
  addSystem(new VP1TriggerGeometrySystem,IVP13DStandardChannelWidget::StartDisabled);
  addSystem(new VP1TrigROISystem,IVP13DStandardChannelWidget::StartDisabled);
#endif
*/
  // VP1VertexSystem* vertexsys = new VP1VertexSystem;
  // addSystem(vertexsys,IVP13DStandardChannelWidget::StartDisabled);

  // VP1VertexSystem* vertexsys = new VP1VertexSystem;
  addSystem(new VP1AODSystem,IVP13DStandardChannelWidget::StartDisabled);


  addSystem(new VP1ExtraInputSystem(),IVP13DStandardChannelWidget::StartDisabled);
  addSystem(new VP1CaloReadoutSystem,IVP13DStandardChannelWidget::StartDisabled);
  addSystem(new VP1BPhysSystem,IVP13DStandardChannelWidget::StartDisabled);

  VP1PartSpectSystem* partspectsys = new VP1PartSpectSystem();
  addSystem(partspectsys,IVP13DStandardChannelWidget::StartDisabled);

  QObject::connect(geosys,SIGNAL(plotSpectrum(QStack<QString>&,int)),partspectsys,SLOT(plotSpectrum(QStack<QString>&,int)));

  foreach(QString n,VP1SysConf::extraGeometrySystems())
    addSystem(new VP1GeometrySystem(VP1GeoFlags::None,n),IVP13DStandardChannelWidget::StartDisabled);
  foreach(QString n,VP1SysConf::extraTrackSystems())
    addSystem(new VP1TrackSystem(n),IVP13DStandardChannelWidget::StartDisabled);

  VP1SysConf::setupStandardConnectionsAndOptions( guidelinesystem,geosys,tracksys,prdsys,0,VP1SysConf::EVENTSTUDIES );
}
