/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <QtPlugin>
#include "VP1LightPlugin/VP1LightPlugin_VP1AutoFactory.h"
#include "VP1LightPlugin/VP1LightChannel.h"

QStringList VP1LightPlugin_VP1AutoFactory::channelWidgetsProvided() const
{
  return QStringList()
        << "VP1Light"
         ;
}

IVP1ChannelWidget * VP1LightPlugin_VP1AutoFactory::getChannelWidget(const QString & channelwidget)
{
  if (channelwidget == "VP1Light")
    return new VP1LightChannel();

  return 0;
}
