/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 this corresponds to a Set of CSC modules (same station and same doubletR)
// it can be accessed via hash-id;
// it holds pointers to CscReadoutElements belonging to him 
 -------------------------------------------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: CscDetectorElement.cxx,v 1.1 2009-05-20 15:24:08 tcorneli Exp $
//<version>	$Name: not supported by cvs2svn $

#include "MuonReadoutGeometry/CscDetectorElement.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TrkSurfaces/Surface.h"
#include "GaudiKernel/MsgStream.h"

namespace MuonGM {

CscDetectorElement::CscDetectorElement(GeoVFullPhysVol* pv, MuonDetectorManager* mgr,Identifier id, IdentifierHash 
idHash) : MuonDetectorElement(pv,mgr,id,idHash), m_cscre(NULL)
{
    //m_MsgStream = new MsgStream(mgr->msgSvc(),"MuGM:CscDetectorElement");
  m_nREinDetectorElement=1; 
}

const Amg::Transform3D& CscDetectorElement::transform() const
{return m_cscre->transform();}

const Trk::Surface& CscDetectorElement::surface() const
{return m_cscre->surface();}

const Trk::SurfaceBounds& CscDetectorElement::bounds() const
{return m_cscre->bounds();}

const Amg::Vector3D& CscDetectorElement::center() const
{return m_cscre->center();}

const Amg::Vector3D& CscDetectorElement::normal() const
{return m_cscre->normal();}

const Trk::Surface& 
CscDetectorElement::surface(const Identifier& id) const
{return m_cscre->surface(id);}

const Trk::SurfaceBounds& 
CscDetectorElement::bounds(const Identifier& id) const
{return m_cscre->bounds(id);}

const Amg::Transform3D& 
CscDetectorElement::transform(const Identifier& id) const
{return m_cscre->transform(id);}

const Amg::Vector3D& 
CscDetectorElement::center(const Identifier& id) const
{
  
  return surface(id).center();
}

const Amg::Vector3D& 
CscDetectorElement::normal(const Identifier& id) const
  {return m_cscre->normal(id);}

const std::vector<const Trk::Surface*>&  CscDetectorElement::surfaces() const
{return m_cscre->surfaces();}

}
