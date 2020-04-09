// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class ISiZvertexMaker
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef ISiZvertexMaker_H
#define ISiZvertexMaker_H

#include "GaudiKernel/IAlgTool.h"

#include "Identifier/IdentifierHash.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "VxVertex/Vertex.h"
#include "GaudiKernel/EventContext.h"

class MsgStream;

namespace InDet{

  class SiSpacePointsSeedMakerEventData;

  /**
   * @class ISiZvertexMaker
   *
   * Base class for primary vertices z-coordinates generation
   *
   * In AthenaMT, event dependent cache inside ISiZvertexMaker is not
   * preferred. SiSpacePointsSeedMakerEventData class holds event dependent
   * data for ISiZvertexMaker and ISiSpacePointsSeedMaker.
   * Its object is instantiated in SiSPSeededTrackFinder::execute.
   *
   * Example implementation
   * @code {.cpp}
   *
   * SiSpacePointsSeedMakerEventData seedEventData;
   * std::list<Trk::Vertex> VZ = zvertexmaker->newEvent(seedEventData);
   * for (std::list<Trk::Vertex>::const_iterator v=VZ.begin(); v!=VZ.end(); ++v) {
   *     do some method with this veretx
   * }
   *
   * @endcode
   */

/////////////////////////////////////////////////////////////////////////////////

  class ISiZvertexMaker : virtual public IAlgTool 
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:

      /// @name InterfaceID
      //@{
      DeclareInterfaceID(ISiZvertexMaker, 1, 0);
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Methods to initialize tool for new event or region and return vertex list
      ///////////////////////////////////////////////////////////////////
      //@{
      virtual std::list<Trk::Vertex> newEvent(const EventContext& ctx, SiSpacePointsSeedMakerEventData& data) const =0;

      virtual std::list<Trk::Vertex> newRegion(const EventContext& ctx, SiSpacePointsSeedMakerEventData& data,
                                               const std::vector<IdentifierHash>&,
                                               const std::vector<IdentifierHash>&) const =0;

      virtual std::list<Trk::Vertex> newRegion(const EventContext& ctx, SiSpacePointsSeedMakerEventData& data,
                                               const std::vector<IdentifierHash>&,
                                               const std::vector<IdentifierHash>&,
                                               const IRoiDescriptor&) const =0;
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
      //@{
      virtual MsgStream& dump(MsgStream& out) const=0;
      //@}
    };
 
} // end of name space

#endif // ISiZvertexMaker_H
