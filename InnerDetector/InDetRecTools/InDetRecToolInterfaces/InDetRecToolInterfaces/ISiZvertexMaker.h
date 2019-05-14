// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class ISiZvertexMaker
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
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

#include "GaudiKernel/AlgTool.h"
#include "VxVertex/Vertex.h"
#include "Identifier/IdentifierHash.h"
#include "IRegionSelector/IRoiDescriptor.h"

class MsgStream;

namespace InDet{

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

      virtual std::list<Trk::Vertex> newEvent()=0;
      virtual std::list<Trk::Vertex> newRegion
      (const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&)=0;
      virtual std::list<Trk::Vertex> newRegion
      (const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&,const IRoiDescriptor&)=0;

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
     
      virtual MsgStream&    dump(MsgStream&    out) const=0;
      virtual std::ostream& dump(std::ostream& out) const=0;

    };
  
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator for MsgStream and  std::ostream
  ///////////////////////////////////////////////////////////////////
  
  MsgStream&    operator << (MsgStream&   ,const ISiZvertexMaker&);
  std::ostream& operator << (std::ostream&,const ISiZvertexMaker&);
  
  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Overload of << operator MsgStream
  ///////////////////////////////////////////////////////////////////
   
  inline MsgStream& operator    << 
    (MsgStream& sl,const ISiZvertexMaker& se)
    { 
      return se.dump(sl); 
    }
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator std::ostream
  ///////////////////////////////////////////////////////////////////
  
  inline std::ostream& operator << 
    (std::ostream& sl,const ISiZvertexMaker& se)
    { 
      return se.dump(sl); 
    }   
  
} // end of name space

#endif // ISiZvertexMaker_H

