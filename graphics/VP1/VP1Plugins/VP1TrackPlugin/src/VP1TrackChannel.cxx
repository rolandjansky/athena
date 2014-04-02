/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1TrackPlugin/VP1TrackChannel.h"
#include "VP1TrackSystems/VP1TrackSystem.h"
#include "VP1GeometrySystems/VP1GeometrySystem.h"
#include "VP1GuideLineSystems/VP1GuideLineSystem.h"
#include "VP1SimHitSystems/VP1SimHitSystem.h"
#include "VP1VertexSystems/VP1VertexSystem.h"
#include "VP1PRDSystems/VP1PrepRawDataSystem.h"
//#include "VP1FatrasSystems/VP1FatrasSingleTrackSimSystem.h"
#include "VP1PlugUtils/VP1SysConf.h"
#include "VP1RawDataSystems/VP1RawDataSystem.h"

VP1TrackChannel::VP1TrackChannel()
  : IVP13DStandardChannelWidget(VP1CHANNELNAMEINPLUGIN(VP1TrackChannel,"Tracking"),
				"This channel displays various systems related to Tracking.",
				"Edward Moyse <edward.moyse@cern.ch>, Thomas.Kittelmann@cern.ch") {}

void VP1TrackChannel::init()
{

  //if (VP1FatrasSingleTrackSimSystem::singleTrackSimAlgorithmIsAvailable())
  //  addSystem(new VP1FatrasSingleTrackSimSystem);

  VP1GuideLineSystem * guidelinesystem = new VP1GuideLineSystem;
  addSystem(guidelinesystem);
  VP1GeometrySystem * geosys = new VP1GeometrySystem(VP1GeoFlags::AllMuonChambers);
  addSystem(geosys);
  VP1TrackSystem * tracksys = new VP1TrackSystem;
  addSystem(tracksys,IVP13DStandardChannelWidget::StartDisabled);
  VP1PrepRawDataSystem* prdsys = new VP1PrepRawDataSystem;
  addSystem(prdsys,IVP13DStandardChannelWidget::StartDisabled);

  VP1VertexSystem* vertex = new VP1VertexSystem;
  addSystem(vertex , IVP13DStandardChannelWidget::StartDisabled);
  addSystem(new VP1RawDataSystem,IVP13DStandardChannelWidget::StartDisabled);

  addSystem(new VP1SimHitSystem(),IVP13DStandardChannelWidget::StartDisabled);

  foreach(QString n,VP1SysConf::extraGeometrySystems())
    addSystem(new VP1GeometrySystem(VP1GeoFlags::None,n),IVP13DStandardChannelWidget::StartDisabled);
  foreach(QString n,VP1SysConf::extraTrackSystems())
    addSystem(new VP1TrackSystem(n),IVP13DStandardChannelWidget::StartDisabled);

  VP1SysConf::setupStandardConnectionsAndOptions( guidelinesystem,geosys,tracksys,prdsys,vertex,VP1SysConf::EVENTSTUDIES );

}
