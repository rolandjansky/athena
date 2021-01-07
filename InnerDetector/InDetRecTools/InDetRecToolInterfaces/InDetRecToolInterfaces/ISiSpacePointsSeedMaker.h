// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class ISiSpacePointsSeedMaker
/////////////////////////////////////////////////////////////////////////////////
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
#include "GaudiKernel/EventContext.h"

#include <set>

class MsgStream;

namespace Trk { class Track; }

namespace InDet {

  class SiSpacePointsSeed;
  class SiSpacePointsSeedMakerEventData;

  /**
   * @class ISiSpacePointsSeedMaker
   *
   * Base class for track candidates generation using space points information
   *
   * In AthenaMT, event dependent cache inside ISiSpacePointsSeedMaker is not
   * preferred. SiSpacePointsSeedMakerEventData class holds event dependent
   * data for ISiSpacePointsSeedMaker and its object is instantiated in
   * SiSPSeededTrackFinder::execute.
   *
   * Example implementation
   *
   * @code {.cpp}
   *
   * SiSpacePointsSeedMakerEventData seedEventData;
   * seedsmaker->newEvent(seedEventData, -1);
   *
   * std::list<Trk::Vertex> vertices;
   * seedsmaker->find2Sp(seedEventData, vertices);
   *
   * const InDet::SiSpacePointsSeed* seed = nullptr;
   * while ((seed=seedsmaker->next(seedEventData))) {
   *     do some method with two space points seed;
   * }
   *
   * seedsmaker->find3Sp();
   * while ((seed=seedsmaker->next(seedEventData))) {
   *     do some method with three space points seed;
   * }
   *
   * seedsmaker->findVSp();
   * while ((seed=seedsmaker->next(seedEventData))) {
   *     do some method with variable number space points seed;
   * }
   *
   * @endcode
   */

  class ISiSpacePointsSeedMaker : virtual public IAlgTool 
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:

      ///////////////////////////////////////////////////////////////////
      /// @name InterfaceID
      ///////////////////////////////////////////////////////////////////
      //@{
      DeclareInterfaceID(ISiSpacePointsSeedMaker, 1, 0);
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Methods to initialize tool for new event or region
      ///////////////////////////////////////////////////////////////////
      //@{
      virtual void newEvent(const EventContext& ctx, SiSpacePointsSeedMakerEventData& data, int iteration=-1) const =0;
      virtual void newRegion(const EventContext& ctx, SiSpacePointsSeedMakerEventData& data,
                             const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT) const =0;
      virtual void newRegion(const EventContext& ctx, SiSpacePointsSeedMakerEventData& data,
                             const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT,
                             const IRoiDescriptor& iRD) const =0;
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Methods to initilize different strategies of seeds production
      ///////////////////////////////////////////////////////////////////
      //@{
      /// with two space points with or without vertex constraint
      virtual void find2Sp(SiSpacePointsSeedMakerEventData& data,
                           const std::list<Trk::Vertex>& lv) const =0;

      /// with three space points with or without vertex constraint
      virtual void find3Sp(const EventContext& ctx, SiSpacePointsSeedMakerEventData& data,
                           const std::list<Trk::Vertex>& lv) const =0;

      /// with three space points with or without vertex constraint
      /// with information about min and max Z of the  vertex
      virtual void find3Sp(const EventContext& ctx, SiSpacePointsSeedMakerEventData& data,
                           const std::list<Trk::Vertex>& lv, const double* zVertex) const =0;

      /// with variable number space points with or without vertex constraint
      /// Variable means (2,3,4,....) any number space points
      virtual void findVSp(const EventContext& ctx, SiSpacePointsSeedMakerEventData& data,
                           const std::list<Trk::Vertex>& lv) const =0;
      //@}
      
      ///////////////////////////////////////////////////////////////////
      /// @name Iterator through seeds pseudo collection
      /// produced accordingly methods find
      ///////////////////////////////////////////////////////////////////
      //@{
      virtual const SiSpacePointsSeed* next(const EventContext& ctx, SiSpacePointsSeedMakerEventData& data) const =0;
      //@}

      virtual void writeNtuple(const SiSpacePointsSeed* seed, const Trk::Track*, int seedType, long eventNumber) const =0;

      virtual bool getWriteNtupleBoolProperty() const =0;
     
      enum seedType { StripSeed = 0, PixelSeed = 1 };
 
      ///////////////////////////////////////////////////////////////////
      /// @name Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
      //@{
      virtual MsgStream& dump(SiSpacePointsSeedMakerEventData& data, MsgStream& out) const=0;
      //@}
     
    };
  
} // end of name space


#endif // ISiSpacePointsSeedMaker_H
