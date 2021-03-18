/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 The Mdt detector = a multilayer = MDT in amdb 
 ----------------------------------------------------
 ***************************************************************************/

#include "MuonReadoutGeometry/MdtDetectorElement.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include <GaudiKernel/IMessageSvc.h>
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TrkDistortedSurfaces/SaggedLineSurface.h"
#include "TrkSurfaces/CylinderBounds.h"

#include <string>

class GeoVFullPhysVol;

namespace MuonGM {

MdtDetectorElement::MdtDetectorElement(GeoVFullPhysVol* pv, 
                                       MuonDetectorManager* mgr,
                                       Identifier id,
                                       IdentifierHash idHash)
    : MuonDetectorElement(pv, mgr, id, idHash)
{
  for (unsigned int i=0; i<maxMdtREinDE; ++i) {
    m_mdtRE[i] = 0;
  }
  m_nRE=0;
}


void 
MdtDetectorElement::addMdtReadoutElement (const MdtReadoutElement* x, int ml)
{
#ifndef NDEBUG
  MsgStream log(Athena::getMessageSvc(),"MdtDetectorElement");
  if (log.level()<=MSG::VERBOSE) log << MSG::VERBOSE << "Adding RE for multilayer " << ml << " to MdtDetectorElement with idhash = " << (int)m_idhash << endmsg;
#endif
  m_mdtRE[ml-1] = x;
  ++m_nREinDetectorElement;
}


const MdtReadoutElement*
MdtDetectorElement::getMdtReadoutElement(Identifier id) const
{
    const MdtIdHelper* idh = manager()->mdtIdHelper();
    unsigned int ml = idh->multilayer(id);
    if ( ml <=0 || ml > nReadoutElements() ) {
      MsgStream log(Athena::getMessageSvc(),"MdtDetectorElement");
      if (log.level()<=MSG::WARNING) log <<  MSG::WARNING
      <<"getMdtReadoutElement("<<idh->show_to_string(id)
      <<"): multilayer out of range 1-"<<nReadoutElements()
      <<" for MdtDetectorElement "<<idh->show_to_string(identify())<<endmsg;
#ifndef NDEBUG
      throw std::out_of_range("multiLayer, in input Id, out or range");
#endif
      return nullptr;
    }
    return m_mdtRE[ml-1];
}


const MdtReadoutElement*
MdtDetectorElement::getMdtReadoutElement(int ml) const
{
  if ( ml <=0 || ml > (int)nReadoutElements() ) {
    MsgStream log(Athena::getMessageSvc(),"MdtDetectorElement");
    if (log.level()<=MSG::WARNING) log <<  MSG::WARNING << "getMdtReadoutElement(" << ml 
    << "): multilayer out of range 1-" << nReadoutElements() << " for MdtDetectorElement "
    << (manager()->mdtIdHelper())->show_to_string(identify()) << endmsg;
#ifndef NDEBUG
    throw std::out_of_range("input multiLayer out or range");
#endif
    return nullptr;
  }
  return m_mdtRE[ml-1];
}


const Amg::Transform3D& MdtDetectorElement::transform() const
{return m_mdtRE[0]->transform();}

const Trk::Surface& MdtDetectorElement::surface() const
{return m_mdtRE[0]->surface();}

const Trk::SurfaceBounds& MdtDetectorElement::bounds() const
{return m_mdtRE[0]->bounds();}

const Amg::Vector3D& MdtDetectorElement::center() const
{return m_mdtRE[0]->center();}

const Amg::Vector3D& MdtDetectorElement::normal() const
{return m_mdtRE[0]->normal();}

const Trk::Surface& 
MdtDetectorElement::surface(const Identifier& id) const
{return getMdtReadoutElement(id)->surface(id);}

const Trk::SurfaceBounds& 
MdtDetectorElement::bounds(const Identifier& id) const
{return getMdtReadoutElement(id)->bounds(id);}

const Amg::Transform3D& 
MdtDetectorElement::transform(const Identifier& id) const
{return getMdtReadoutElement(id)->transform(id);}

const Amg::Vector3D& 
MdtDetectorElement::center(const Identifier& id) const
{
  const MdtIdHelper* idh = manager()->mdtIdHelper();
  int ml = idh->multilayer(id);
  return m_mdtRE[ml-1]->center(idh->tubeLayer(id), idh->tube(id));
}

const Amg::Vector3D& 
MdtDetectorElement::normal(const Identifier& id) const
  {return m_mdtRE[0]->normal(id);}

std::vector<const Trk::Surface*>  MdtDetectorElement::surfaces() const
{
   // needs to be created each time because there's no clearCache() method
   std::vector<const Trk::Surface*> detectorSurfaces;
   for (unsigned int i=0; i<maxMdtREinDE; ++i) {
     std::vector<const Trk::Surface*> resurf = m_mdtRE[i]->surfaces();
     detectorSurfaces.insert(detectorSurfaces.end(),resurf.begin(),resurf.end());
   }
   return detectorSurfaces;
}

}
