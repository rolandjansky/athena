// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class ISiZvertexMaker
/////////////////////////////////////////////////////////////////////////////////
//
//  Base class for primary vertices z-coordinates generation
//
//  Example implementation
//
//  zvertexmaker->newEvent();  
//  std::list<Trk::Vertex> VZ = m_zvertexmaker->getVertices(); 
//  for(std::list<Trk::Vertex>::const_iterator v=VZ.begin(); v!=VZ.end(); ++v) {
//    do some method with this veretx
//  }
//
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef ISiZvertexMaker_H
#define ISiZvertexMaker_H

#include "GaudiKernel/IAlgTool.h"

#include "Identifier/IdentifierHash.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "VxVertex/Vertex.h"

class MsgStream;

namespace InDet{

  class SiSpacePointsSeedMakerEventData;

  class ISiZvertexMaker : virtual public IAlgTool 
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:

      // InterfaceID
      DeclareInterfaceID(ISiZvertexMaker, 1, 0);

      ///////////////////////////////////////////////////////////////////
      // Methods to initialize tool for new event or region
      ///////////////////////////////////////////////////////////////////

      virtual std::list<Trk::Vertex> newEvent(SiSpacePointsSeedMakerEventData& data) const =0;
      virtual std::list<Trk::Vertex> newRegion
      (SiSpacePointsSeedMakerEventData& data,
       const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&) const =0;
      virtual std::list<Trk::Vertex> newRegion
      (SiSpacePointsSeedMakerEventData& data,
       const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&,
       const IRoiDescriptor&) const =0;

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
     
      virtual MsgStream& dump(MsgStream& out) const=0;
    };
 
} // end of name space

#endif // ISiZvertexMaker_H
