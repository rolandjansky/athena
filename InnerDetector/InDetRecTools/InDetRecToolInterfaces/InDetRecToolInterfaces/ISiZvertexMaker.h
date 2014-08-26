/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  static const InterfaceID IID_ISiZvertexMaker("InDet::ISiZvertexMaker",1,0);

  class ISiZvertexMaker : virtual public IAlgTool 
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:

      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      static const InterfaceID& interfaceID();
      virtual StatusCode initialize ()=0;
      virtual StatusCode finalize   ()=0;
      virtual ~ISiZvertexMaker(){};

      ///////////////////////////////////////////////////////////////////
      // Methods to initialize tool for new event or region
      ///////////////////////////////////////////////////////////////////

      virtual void newEvent()=0;
      virtual void newRegion
	(const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&)=0;
      virtual void newRegion
	(const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&,const IRoiDescriptor&)=0;

      ///////////////////////////////////////////////////////////////////
      // Methods for primary vertices production
      ///////////////////////////////////////////////////////////////////

      virtual const std::list<Trk::Vertex>&  getVertices ()=0;

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

  inline const InterfaceID& ISiZvertexMaker::interfaceID()
    {
      return IID_ISiZvertexMaker;
    }

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

