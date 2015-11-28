/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 this corresponds to a Set of RPC modules (same station and same doubletR)
// it can be accessed via hash-id;
// it holds pointers to TgcReadoutElements belonging to him 
 -------------------------------------------------------------------------
 Copyright (C) 2005 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: TgcDetectorElement.cxx,v 1.1 2009-05-20 15:24:08 tcorneli Exp $
//<version>	$Name: not supported by cvs2svn $

#include "MuonReadoutGeometry/TgcDetectorElement.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TrkSurfaces/Surface.h"
#include "GaudiKernel/MsgStream.h"

namespace MuonGM {

TgcDetectorElement::TgcDetectorElement(GeoVFullPhysVol* pv, MuonDetectorManager* mgr,Identifier id, IdentifierHash 
idHash) : MuonDetectorElement(pv,mgr,id,idHash), _tgcre(NULL)
{
    //m_MsgStream = new MsgStream(mgr->msgSvc(),"MuGM:TgcDetectorElement");
  _nREinDetectorElement=1; 
}

const Amg::Transform3D& TgcDetectorElement::transform() const
{return _tgcre->transform();}

const Trk::Surface& TgcDetectorElement::surface() const
{return _tgcre->surface();}

const Trk::SurfaceBounds& TgcDetectorElement::bounds() const
{return _tgcre->bounds();}

const Amg::Vector3D& TgcDetectorElement::center() const
{
    //    msg(MSG::INFO)<<"Test from TgcDetectorElement::center()"<<endreq;
    
return _tgcre->center();}

const Amg::Vector3D& TgcDetectorElement::normal() const
{return _tgcre->normal();}

const Trk::Surface& 
TgcDetectorElement::surface(const Identifier& id) const
{return _tgcre->surface(id);}

const Trk::SurfaceBounds& 
TgcDetectorElement::bounds(const Identifier& id) const
{return _tgcre->bounds(id);}

const Amg::Transform3D& 
TgcDetectorElement::transform(const Identifier& id) const
{return _tgcre->transform(id);}

const Amg::Vector3D& 
TgcDetectorElement::center(const Identifier& id) const
{
  
  return surface(id).center();
}

const Amg::Vector3D& 
TgcDetectorElement::normal(const Identifier& id) const
  {return _tgcre->normal(id);}

const std::vector<const Trk::Surface*>& 
TgcDetectorElement::surfaces() const
{return _tgcre->surfaces();}

}
