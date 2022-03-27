/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
              "Joseph.Boudreau@cern.ch, Riccardo.Maria.Bianchi@cern.ch"),
    m_treeWidget(0)
{
}


//_________________________________________________________
// Empty destructor
// Note: we need to have this in the .cpp,
// instead of having it in the header file,
// to let the destroyer of the unique_ptr know about the 
// size of the object to be deleted, so we can keep using
// the forward declaration in the header and the Pimpl idiom.
// 
// Nice source about this: 
// - https://ortogonal.github.io/cpp/forward-declaration-and-smart-pointers/
VP1MCChannel::~VP1MCChannel() {}


//_________________________________________________________
void VP1MCChannel::init()
{
  m_mcsystem.reset (new VP1MCSystem());
  registerSystem(m_mcsystem.get());
 
}

//_________________________________________________________
void VP1MCChannel::create() {

  //Setup this widget
  Ui::MCChannelWidgetForm ui;
  ui.setupUi(this);
  m_treeWidget = ui.treeWidget;
  registerController(m_mcsystem->controllerWidget());

  m_mcsystem->setTree(m_treeWidget);

}

