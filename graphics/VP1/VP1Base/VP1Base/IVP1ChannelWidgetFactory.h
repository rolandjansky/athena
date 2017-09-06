/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class IVP1ChannelWidgetFactory         //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef IVP1CHANNELWIDGETFACTORY_H
#define IVP1CHANNELWIDGETFACTORY_H

#include <QtPlugin>

class QString;
class QStringList;
class IVP1ChannelWidget;

class IVP1ChannelWidgetFactory
{
public:
  virtual ~IVP1ChannelWidgetFactory() {}

  virtual QStringList channelWidgetsProvided() const = 0;
  virtual IVP1ChannelWidget * getChannelWidget(const QString & channelwidget) = 0;

};

Q_DECLARE_INTERFACE(IVP1ChannelWidgetFactory,"IVP1ChannelWidgetFactory/1.0")

#endif
