/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 this corresponds to a Set of RPC modules (same station and same doubletR)
// it can be accessed via hash-id;
// it holds pointers to RpcReadoutElements belonging to him 
 -------------------------------------------------------------------------
 Copyright (C) 2005 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: RpcDetectorElement.cxx,v 1.5 2009-05-20 15:24:08 tcorneli Exp $
//<version>	$Name: not supported by cvs2svn $

#include "MuonReadoutGeometry/RpcDetectorElement.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TrkSurfaces/Surface.h"
#include "GaudiKernel/MsgStream.h"

namespace MuonGM {

RpcDetectorElement::RpcDetectorElement(GeoVFullPhysVol* pv, 
                                       MuonDetectorManager* mgr,
                                       Identifier id,
                                       IdentifierHash idHash) :
   MuonDetectorElement(pv, mgr, id, idHash), _ndbz(0)
{
    //m_MsgStream = new MsgStream(mgr->msgSvc(),"MuGM:RpcDetectorElement");
  //std::cerr<<"Costruttore di RpcDetectorElement per idhash = "<<idHash<<std::endl;
  for (unsigned int i=0; i<NDoubletZ; i++) _rpcVector[i] = NULL; 
  _nREinDetectorElement=0;
  _helper=manager()->rpcIdHelper();
}


void RpcDetectorElement::addRpcReadoutElement(const RpcReadoutElement* rpc, int index)
{
  //std::cout << "index: " << index << std::endl;
  if (index < NDoubletZ) {
    if (_rpcVector[index] == NULL) {
      _rpcVector[index] = rpc;
      // everything ok
      _nREinDetectorElement++;
      //std::cerr<<" This is rpcDE with hashID = "<<identifyHash();
      //std::cerr<<" index "<<index<<" _rpcVector[index] filled with RpcReadoutElement @ "
      //         <<rpc<<std::endl;
    } else {
      std::cerr << "RpcDetectorElement::add -- problems for idhash " << identifyHash()
                << " index " << index << " is taken." << std::endl;
      throw;
    }
  } else {
    std::cerr << "RpcDetectorElement::add -- problems for idhash " << identifyHash()
              << " index " << index << " is >= " << NDoubletZ << std::endl;
    throw;
  }
}

const RpcReadoutElement*
RpcDetectorElement::getRpcReadoutElement(Identifier id) const
{
  int dbz = _helper->doubletZ(id);
  int dbp = _helper->doubletPhi(id);
  return getRpcReadoutElement(dbz, dbp);
}


const RpcReadoutElement*
RpcDetectorElement::getRpcReadoutElement(int dbz, int dbp) const
{
  int dbz_index = dbz - 1;
  std::string stName = _rpcVector[0]->getStationName();

  //std::cerr<<"RpcDetectorElement::readoutElement("<<dbz<<", "<<dbp<<") - stName, stEta, dbr = "
  //         << stName<<" "<<stEta<<" "<<dbr<<" "<<std::endl;  
    
  if (stName  == "BMS") {
    int stEta = _rpcVector[0]->getStationEta();
    int dbr = _rpcVector[0]->getDoubletR();
    if (abs(stEta) == 2 && dbz == 3) {
      if (dbp == 2) dbz_index++;

    } else if (abs(stEta) == 4 &&
               dbr == 2 &&
               dbz == 3)  {
      if (dbp == 2) dbz_index++;

    } else if (abs(stEta) == 4 &&
               dbr == 1 &&
               dbz == 2) {
      if (dbp == 2) dbz_index++;
    }      
  }

  //    std::cerr<<" ==============>>> dbz_index = "<<dbz_index<<" must be < nReadoutElements()="<<_ndbz_nRE<<std::endl;
  if (dbz_index < (int)nReadoutElements() && dbz_index >= 0) return _rpcVector[dbz_index];
  else return NULL;
}

const Amg::Transform3D& RpcDetectorElement::transform() const
{return _rpcVector[0]->transform();}

const Trk::Surface& RpcDetectorElement::surface() const
{return _rpcVector[0]->surface();}

const Trk::SurfaceBounds& RpcDetectorElement::bounds() const
{return _rpcVector[0]->bounds();}

const Amg::Vector3D& RpcDetectorElement::center() const
{return _rpcVector[0]->center();}

const Amg::Vector3D& RpcDetectorElement::normal() const
{return _rpcVector[0]->normal();}

const Trk::Surface& 
RpcDetectorElement::surface(const Identifier& id) const
{return getRpcReadoutElement(id)->surface(id);}

const Trk::SurfaceBounds& 
RpcDetectorElement::bounds(const Identifier& id) const
{return getRpcReadoutElement(id)->bounds(id);}

const Amg::Transform3D& 
RpcDetectorElement::transform(const Identifier& id) const
{return getRpcReadoutElement(id)->transform(id);}

const Amg::Vector3D& 
RpcDetectorElement::center(const Identifier& id) const
{
  
  return surface(id).center();
}

const Amg::Vector3D& 
RpcDetectorElement::normal(const Identifier& id) const
  {return _rpcVector[0]->normal(id);}

const std::vector<const Trk::Surface*>&  RpcDetectorElement::surfaces() const
{
   // needs to be created each time because there's no clearCache() method
   m_detectorSurfaces.clear();
   for (unsigned int i=0; i<NDoubletZ; i++)
     m_detectorSurfaces.insert(m_detectorSurfaces.end(),_rpcVector[i]->surfaces().begin(),_rpcVector[i]->surfaces().end());

   return m_detectorSurfaces;
}

}
