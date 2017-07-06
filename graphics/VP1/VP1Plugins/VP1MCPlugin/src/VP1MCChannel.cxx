/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1MCChannel                   //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: May 2007                              //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1MCPlugin/VP1MCChannel.h"
#include "ui_mcchannelwidgetform.h"
#include "VP1MCSystems/VP1MCSystem.h"
#include <QLabel>

//_________________________________________________________
VP1MCChannel::VP1MCChannel()
  : IVP1ChannelWidget(VP1CHANNELNAMEINPLUGIN(VP1MCChannel,"MC"),
		      "This channel simply shows the MC Tree, provided by the VP1MCSystem.",
		      "Joseph.Boudreau@cern.ch"),
    treeWidget(0)
{
}

//_________________________________________________________
void VP1MCChannel::init()
{
  mcsystem = new VP1MCSystem();
  registerSystem(mcsystem);
 
}

//_________________________________________________________
void VP1MCChannel::create() {

  //Setup this widget
  Ui::MCChannelWidgetForm ui;
  ui.setupUi(this);
  treeWidget = ui.treeWidget;
  registerController(mcsystem->controllerWidget());

  mcsystem->setTree(treeWidget);

}

