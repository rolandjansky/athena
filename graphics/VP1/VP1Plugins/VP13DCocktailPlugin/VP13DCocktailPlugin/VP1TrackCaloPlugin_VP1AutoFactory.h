/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
// Originally autogenerated by CMT with VP1 Factory Code Header File (Fri Aug 19 13:36:52 CEST 2016)
// Now we add this file to plugins by hand, to simplify the CMake compilation

#ifndef VP1TrackCaloPlugin_VP1AutoFactory_H
#define VP1TrackCaloPlugin_VP1AutoFactory_H

#include <QObject>
#include <QStringList>

#include "VP1Base/IVP1ChannelWidgetFactory.h"

class VP1TrackCaloPlugin_VP1AutoFactory : public QObject, public IVP1ChannelWidgetFactory
{
  Q_OBJECT

  #if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_PLUGIN_METADATA(IID "VP1TrackCaloPlugin" )
  # else
    Q_EXPORT_PLUGIN2(pnp_vp1trackcaloplugin_vp1autofactory, VP1TrackCaloPlugin_VP1AutoFactory)
  #endif

  Q_INTERFACES(IVP1ChannelWidgetFactory)

public:
  virtual QStringList channelWidgetsProvided() const;
  virtual IVP1ChannelWidget * getChannelWidget(const QString & channelwidget);
};

#endif
