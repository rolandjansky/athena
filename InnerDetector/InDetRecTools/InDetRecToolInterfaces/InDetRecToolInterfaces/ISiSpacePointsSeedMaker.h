/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class ISiSpacePointsSeedMaker
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
//
//  Base class for track candidates generation using space points information
//
//  Example implementation
//
//  seedsmaker->newEvent();      
//
//  const InDet::SiSpacePointsSeed* seed = 0;
//  seedsmaker->find2Sp();
//  while((seed=seedsmaker->next())) {
//       do some method with two space points seed;
//  }
//
//  seedsmaker->find3Sp();
//  while((seed=seedsmaker->next())) {
//       do some method with three space points seed;
//  }
//
//  seedsmaker->findVSp();
//  while((seed=seedsmaker->next())) {
//       do some method with variable number space points seed;
//  }
//
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 18/11/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef ISiSpacePointsSeedMaker_H
#define ISiSpacePointsSeedMaker_H

#include <set>

#include "GaudiKernel/AlgTool.h"
#include "VxVertex/Vertex.h"
#include "Identifier/IdentifierHash.h"
#include "SiSpacePointsSeed/SiSpacePointsSeed.h"
#include "IRegionSelector/IRoiDescriptor.h"

class MsgStream;

namespace Trk { class Track; }

namespace InDet {

  class SiSpacePointsSeed;
 
  static const InterfaceID IID_ISiSpacePointsSeedMaker
    ("InDet::ISiSpacePointsSeedMaker",1,0);

  class ISiSpacePointsSeedMaker : virtual public IAlgTool 
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
       
      ///////////////////////////////////////////////////////////////////
      // Methods to initialize tool for new event or region
      ///////////////////////////////////////////////////////////////////

      void         newEvent();
      virtual void newEvent (int)=0; 
      virtual void newRegion
	(const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&)=0;
      virtual void newRegion
	(const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&,const IRoiDescriptor&)=0;

      ///////////////////////////////////////////////////////////////////
      // Methods to initilize different strategies of seeds production
      // with two space points with or without vertex constraint
      ///////////////////////////////////////////////////////////////////

      virtual void find2Sp (const std::list<Trk::Vertex>&)=0;

      ///////////////////////////////////////////////////////////////////
      // Methods to initilize different strategies of seeds production
      // with three space points with or without vertex constraint
      ///////////////////////////////////////////////////////////////////

      virtual void find3Sp (const std::list<Trk::Vertex>&)=0;

      //////////////////////////////////////////////////////////////////
      // Methods to initilize different strategies of seeds production
      // with three space points with or without vertex constraint
      // with information about min and max Z of the  vertex
      ///////////////////////////////////////////////////////////////////

      virtual void find3Sp (const std::list<Trk::Vertex>&,const double*)=0;

      ///////////////////////////////////////////////////////////////////
      // Methods to initilize different strategies of seeds production
      // with variable number space points with or without vertex constraint
      // Variable means (2,3,4,....) any number space points
      ///////////////////////////////////////////////////////////////////
 
      virtual void findVSp (const std::list<Trk::Vertex>&)=0;
      
      ///////////////////////////////////////////////////////////////////
      // Iterator through seeds pseudo collection produced accordingly
      // methods find    
      ///////////////////////////////////////////////////////////////////
      
      virtual const SiSpacePointsSeed* next()=0;
      
      virtual void writeNtuple(const SiSpacePointsSeed* seed, const Trk::Track*, int seedType, long eventNumber) const =0;

      virtual bool getWriteNtupleBoolProperty() const =0;

      enum seedType { StripSeed = 0, PixelSeed = 1 };
      
      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
     
      virtual MsgStream&    dump(MsgStream&    out) const=0;
      virtual std::ostream& dump(std::ostream& out) const=0;
     
    };
  
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator for MsgStream and  std::ostream
  ///////////////////////////////////////////////////////////////////
  
  MsgStream&    operator << (MsgStream&   ,const ISiSpacePointsSeedMaker&);
  std::ostream& operator << (std::ostream&,const ISiSpacePointsSeedMaker&);
  
  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const InterfaceID& ISiSpacePointsSeedMaker::interfaceID()
    {
      return IID_ISiSpacePointsSeedMaker;
    }

  inline void ISiSpacePointsSeedMaker::newEvent()
  {
    return newEvent(-1);
  }

  ///////////////////////////////////////////////////////////////////
  // Overload of << operator MsgStream
  ///////////////////////////////////////////////////////////////////
   
  inline MsgStream& operator    << 
    (MsgStream& sl,const ISiSpacePointsSeedMaker& se)
    { 
      return se.dump(sl); 
    }
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator std::ostream
  ///////////////////////////////////////////////////////////////////
  
  inline std::ostream& operator << 
    (std::ostream& sl,const ISiSpacePointsSeedMaker& se)
    { 
      return se.dump(sl); 
    }   
} // end of name space


#endif // ISiSpacePointsSeedMaker_H

