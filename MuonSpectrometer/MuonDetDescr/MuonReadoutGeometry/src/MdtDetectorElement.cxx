/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 The Mdt detector = a multilayer = MDT in amdb 
 ----------------------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: MdtDetectorElement.cxx,v 1.3 2009-05-20 15:23:12 tcorneli Exp $
//<version>	$Name: not supported by cvs2svn $


#include "MuonReadoutGeometry/MdtDetectorElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
//#include "GaudiKernel/MsgStream.h"

namespace MuonGM {

MdtDetectorElement::MdtDetectorElement(GeoVFullPhysVol* pv, 
                                       MuonDetectorManager* mgr,
                                       Identifier id,
                                       IdentifierHash idHash)
    : MuonDetectorElement(pv, mgr, id, idHash)
{
  
  //m_MsgStream = new MsgStream(mgr->msgSvc(),"MuGM:MdtDetectorElement");
  //  m_debug = m_MsgStream->level() <= MSG::DEBUG;
  //  m_verbose = m_MsgStream->level() <= MSG::VERBOSE;
  //  if (m_debug) reLog() << MSG::DEBUG << "A new MdtDetectorElement was born: idhash = "
  //                      << (int)idHash << endmsg;

  for (unsigned int i=0; i<maxMdtREinDE; ++i) {
    m_mdtRE[i] = 0;
  }
  m_nRE=0;
}


void 
MdtDetectorElement::addMdtReadoutElement (const MdtReadoutElement* x, int ml)
{
//   if (m_verbose) reLog() << MSG::VERBOSE << "Adding RE for multilayer " << ml
//                          << " to MdtDetectorElement with idhash = " << (int)m_idhash
//                          << endmsg;
  if( msgLevel(MSG::VERBOSE) ) msg( MSG::VERBOSE ) << "Adding RE for multilayer " << ml
						   << " to MdtDetectorElement with idhash = " << (int)m_idhash
						   << endmsg;
  //std::cout<<" adding mdtRE with ml = "<<ml<<" to MdtDE with id hash "<< (int)m_idhash<<std::endl;
 
  m_mdtRE[ml-1] = x;
  ++m_nREinDetectorElement;
}


const MdtReadoutElement*
MdtDetectorElement::getMdtReadoutElement(Identifier id) const
{
    const MdtIdHelper* idh = manager()->mdtIdHelper();
    unsigned int ml = idh->multilayer(id);
    if ( ml <=0 || ml > nReadoutElements() )
    {
	msg( MSG::WARNING ) 
	    //        reLog()<<MSG::WARNING
	    <<"getMdtReadoutElement("<<idh->show_to_string(id)
	    <<"): multilayer out of range 1-"
	    <<nReadoutElements()
	    <<" for MdtDetectorElement "<<idh->show_to_string(identify())<<endmsg;
#ifndef NDEBUG
        throw std::out_of_range("multiLayer, in input Id, out or range");
#endif
        return 0;
    }
    return m_mdtRE[ml-1];
}


const MdtReadoutElement*
MdtDetectorElement::getMdtReadoutElement(int ml) const
{
  if ( ml <=0 || ml > (int)nReadoutElements() ) {
      //    reLog() << MSG::WARNING 
      msg(MSG::WARNING)<< "getMdtReadoutElement(" << ml 
		       << "): multilayer out of range 1-"
		       << nReadoutElements() << " for MdtDetectorElement "
		       << (manager()->mdtIdHelper())->show_to_string(identify()) 
		       << endmsg;
#ifndef NDEBUG
    throw std::out_of_range("input multiLayer out or range");
#endif
    return 0;
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

const std::vector<const Trk::Surface*>&  MdtDetectorElement::surfaces() const
{
   // needs to be created each time because there's no clearCache() method
   m_detectorSurfaces.clear();
   for (unsigned int i=0; i<maxMdtREinDE; ++i) {
     m_detectorSurfaces.insert(m_detectorSurfaces.end(),m_mdtRE[i]->surfaces().begin(),m_mdtRE[i]->surfaces().end());
   }
   return m_detectorSurfaces;
}

MsgStream& MdtDetectorElement::msg( MSG::Level lvl ) const { return m_msg << lvl ; }
bool MdtDetectorElement::msgLevel( MSG::Level lvl ) { return m_msg.get().level() <= lvl ; }

}
