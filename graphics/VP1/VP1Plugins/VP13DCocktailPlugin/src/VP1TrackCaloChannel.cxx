/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "VP13DCocktailPlugin/VP1TrackCaloChannel.h"
#include "VP1TrackSystems/VP1TrackSystem.h"
#include "VP1GeometrySystems/VP1GeometrySystem.h"
#include "VP1GuideLineSystems/VP1GuideLineSystem.h"
#include "VP1CaloSystems/VP1CaloCellSystem.h"
#include "VP1PRDSystems/VP1PrepRawDataSystem.h"
//#include "VP1FatrasSystems/VP1FatrasSingleTrackSimSystem.h" // TODO: to be updated when the new Fatras will be available

//#include "VP1CaloClusterSystems/VP1CaloClusterSystem.h" // it's now in the xAOD/VP1AODSystems
#include "VP1AODSystems/VP1AODSystem.h"

#include "VP1PlugUtils/VP1SysConf.h"

VP1TrackCaloChannel::VP1TrackCaloChannel()
  : IVP13DStandardChannelWidget(VP1CHANNELNAMEINPLUGIN(VP1TrackCaloChannel,"TrackCalo"),
                "This channel displays various systems related to Tracking and Calo.",
                "Riccardo.Maria.Bianchi@cern.ch, Thomas.Kittelmann@cern.ch, Edward.moyse@cern.ch, Vakhtang.Tsulaia@cern.ch")
{
}

void VP1TrackCaloChannel::init()
{

  /* // TODO: to be updated when the new Fatras will be available
  if (VP1FatrasSingleTrackSimSystem::singleTrackSimAlgorithmIsAvailable())
    addSystem(new VP1FatrasSingleTrackSimSystem);
  */


  VP1GuideLineSystem * guidelinesystem = new VP1GuideLineSystem;
  addSystem(guidelinesystem);

  VP1GeometrySystem * geosys = new VP1GeometrySystem(VP1GeoFlags::AllMuonChambers);
  addSystem(geosys);


  VP1TrackSystem * tracksys = new VP1TrackSystem;
  addSystem(tracksys);

  VP1PrepRawDataSystem* prdsys = new VP1PrepRawDataSystem;
  addSystem(prdsys);

  addSystem(new VP1CaloCellSystem());

//  addSystem(new VP1CaloClusterSystem,IVP13DStandardChannelWidget::StartDisabled);
  addSystem(new VP1AODSystem,IVP13DStandardChannelWidget::StartDisabled);

  foreach(QString n,VP1SysConf::extraGeometrySystems())
    addSystem(new VP1GeometrySystem(VP1GeoFlags::None,n),IVP13DStandardChannelWidget::StartDisabled);
  foreach(QString n,VP1SysConf::extraTrackSystems())
    addSystem(new VP1TrackSystem(n),IVP13DStandardChannelWidget::StartDisabled);

  VP1SysConf::setupStandardConnectionsAndOptions( guidelinesystem,geosys,tracksys,prdsys,0,VP1SysConf::EVENTSTUDIES );


}
