/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 a GeoVolumeAction
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: SimpleVolAction.cxx,v 1.3 2006-04-10 17:12:17 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

#include "MuonGeoModel/SimpleVolAction.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include <iostream>

namespace MuonGM {

SimpleVolAction::SimpleVolAction ()
    :GeoVolumeAction (GeoVolumeAction::TOP_DOWN)
{
}


SimpleVolAction::~SimpleVolAction ()
{
}


void SimpleVolAction::handleVPhysVol (const GeoVPhysVol *pv)
{
  MsgStream log(Athena::getMessageSvc(), "MuGM::SimpleVolAction");

  //   std::cout<<" MY VOL Action is handlying a GeoVPhysVol "<<std::endl;
  const GeoLogVol* lv = pv->getLogVol();
  std::string name = lv->getName();
  const GeoMaterial* mat = lv->getMaterial();
  unsigned int nchild = pv->getNChildVols();

  log << MSG::INFO << " *** Tree Navigation *** in " << name
      << " made of " << mat->getName()
      << " N.of Children " << nchild
      << endmsg;

  for (unsigned int i=0 ; i < nchild; ++i)
  {
      log << MSG::INFO << " ... child n. " << i << " named " << pv->getNameOfChildVol(i) << endmsg;
  }
}


} // namespace MuonGM
