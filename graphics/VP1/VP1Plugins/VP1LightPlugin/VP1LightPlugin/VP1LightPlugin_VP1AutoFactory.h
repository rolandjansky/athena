/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1LIGHTPlugin_VP1AutoFactory_H
#define VP1LIGHTPlugin_VP1AutoFactory_H

#include <QObject>
#include <QStringList>

#include "VP1Base/IVP1ChannelWidgetFactory.h"

class VP1LightPlugin_VP1AutoFactory : public QObject, public IVP1ChannelWidgetFactory
{
  Q_OBJECT

  #if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    // Q_PLUGIN_METADATA(IID "VP1GeometryPlugin" FILE "") // apparently, FILE can be empty
    Q_PLUGIN_METADATA(IID "VP1LightPlugin") // FILE can not be empty
  # else
    Q_EXPORT_PLUGIN2(pnp_vp1lightplugin_vp1autofactory, VP1LightPlugin_VP1AutoFactory)
  #endif

  Q_INTERFACES(IVP1ChannelWidgetFactory)

public:
  virtual QStringList channelWidgetsProvided() const;
  virtual IVP1ChannelWidget * getChannelWidget(const QString & channelwidget);
};

#endif
