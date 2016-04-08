/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1CaloPlugin/VP1CaloCellChannel.h"
#include "VP1CaloSystems/VP1CaloCellSystem.h"
#include "VP1GeometrySystems/VP1GeometrySystem.h"
#include "VP1GuideLineSystems/VP1GuideLineSystem.h"
#include "VP1CaloClusterSystems/VP1CaloClusterSystem.h"
#include "VP1CaloReadoutSystems/VP1CaloReadoutSystem.h"
//#include "VP1JetSystems/VP1JetSystem.h" // TODO: has to be updated with the new VP1AODSystems Jet class

VP1CaloCellChannel::VP1CaloCellChannel()
  : IVP13DStandardChannelWidget(VP1CHANNELNAMEINPLUGIN(VP1CaloCellChannel,"Calo Cells"),
                                "Displays energies in Calo Cells",
                                "Vakho Tsulaia")
{
}

void VP1CaloCellChannel::init()
{
  addSystem(new VP1GuideLineSystem);
  addSystem(new VP1GeometrySystem,IVP13DStandardChannelWidget::StartDisabled);
  addSystem(new VP1CaloCellSystem,IVP13DStandardChannelWidget::StartDisabled);
  addSystem(new VP1CaloClusterSystem,IVP13DStandardChannelWidget::StartDisabled);
  addSystem(new VP1CaloReadoutSystem,IVP13DStandardChannelWidget::StartDisabled);

  // TODO: has to be updated with the new VP1AODSystems Jet class
  //addSystem(new VP1JetSystem,IVP13DStandardChannelWidget::StartDisabled);
}

