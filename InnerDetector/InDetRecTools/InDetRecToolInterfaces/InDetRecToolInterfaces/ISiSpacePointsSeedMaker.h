// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class ISiSpacePointsSeedMaker
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

#include "GaudiKernel/IAlgTool.h"

#include "Identifier/IdentifierHash.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "SiSpacePointsSeed/SiSpacePointsSeed.h"
#include "VxVertex/Vertex.h"

#include <set>

class MsgStream;

namespace InDet {

  class SiSpacePointsSeed;
  class SiSpacePointsSeedMakerEventData;
 
  class ISiSpacePointsSeedMaker : virtual public IAlgTool 
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:

      // InterfaceID
      DeclareInterfaceID(ISiSpacePointsSeedMaker, 1, 0);
       
      ///////////////////////////////////////////////////////////////////
      // Methods to initialize tool for new event or region
      ///////////////////////////////////////////////////////////////////

      virtual void newEvent(SiSpacePointsSeedMakerEventData& data, int iteration=-1) const =0;
      virtual void newRegion(SiSpacePointsSeedMakerEventData& data,
                             const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT) const =0;
      virtual void newRegion(SiSpacePointsSeedMakerEventData& data,
                             const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT,
                             const IRoiDescriptor& iRD) const =0;

      ///////////////////////////////////////////////////////////////////
      // Methods to initilize different strategies of seeds production
      // with two space points with or without vertex constraint
      ///////////////////////////////////////////////////////////////////

      virtual void find2Sp(SiSpacePointsSeedMakerEventData& data,
                           const std::list<Trk::Vertex>& lv) const =0;

      ///////////////////////////////////////////////////////////////////
      // Methods to initilize different strategies of seeds production
      // with three space points with or without vertex constraint
      ///////////////////////////////////////////////////////////////////

      virtual void find3Sp(SiSpacePointsSeedMakerEventData& data,
                           const std::list<Trk::Vertex>& lv) const =0;

      //////////////////////////////////////////////////////////////////
      // Methods to initilize different strategies of seeds production
      // with three space points with or without vertex constraint
      // with information about min and max Z of the  vertex
      ///////////////////////////////////////////////////////////////////

      virtual void find3Sp(SiSpacePointsSeedMakerEventData& data,
                           const std::list<Trk::Vertex>& lv, const double* zVertex) const =0;

      ///////////////////////////////////////////////////////////////////
      // Methods to initilize different strategies of seeds production
      // with variable number space points with or without vertex constraint
      // Variable means (2,3,4,....) any number space points
      ///////////////////////////////////////////////////////////////////
 
      virtual void findVSp(SiSpacePointsSeedMakerEventData& data,
                           const std::list<Trk::Vertex>& lv) const =0;
      
      ///////////////////////////////////////////////////////////////////
      // Iterator through seeds pseudo collection produced accordingly
      // methods find    
      ///////////////////////////////////////////////////////////////////
      
      virtual const SiSpacePointsSeed* next(SiSpacePointsSeedMakerEventData& data) const =0;
      
      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
     
      virtual MsgStream& dump(SiSpacePointsSeedMakerEventData& data, MsgStream& out) const=0;
     
    };
  
} // end of name space


#endif // ISiSpacePointsSeedMaker_H
