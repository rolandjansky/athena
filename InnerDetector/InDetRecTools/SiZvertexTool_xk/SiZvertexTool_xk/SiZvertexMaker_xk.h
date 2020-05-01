// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiZvertexMaker_xk
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiZvertexMaker_xk_H
#define SiZvertexMaker_xk_H


#include "InDetRecToolInterfaces/ISiZvertexMaker.h"  
#include "AthenaBaseComps/AthAlgTool.h"

#include "Identifier/IdentifierHash.h"
#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"

#include "GaudiKernel/ToolHandle.h"

#include <iosfwd>
#include <list>
#include <vector>

namespace Trk {
  class Vertex;
}

class MsgStream;

namespace InDet {

  class SiSpacePointsSeedMakerEventData;

  /**
   * @class SiZvertexMaker_xk
   * Class for primary vertices z-coordinates generation (xKalman algorithm)
   *
   * In AthenaMT, event dependent cache inside SiZvertexMaker_xk
   * is not preferred. SiSpacePointsSeedMakerEventData class holds
   * event dependent data for SiZvertexMaker_xk and ISiSpacePointsSeedMaker.
   * Its object is instantiated in SiSPSeededTrackFinder::execute.
   */

  class SiZvertexMaker_xk : public extends<AthAlgTool, ISiZvertexMaker>
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    /// @name Standard tool methods
    ///////////////////////////////////////////////////////////////////
    //@{
    SiZvertexMaker_xk(const std::string&,
                      const std::string&,
                      const IInterface*);
    virtual ~SiZvertexMaker_xk() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    //@}

    ///////////////////////////////////////////////////////////////////
    /// @name Methods to initialize tool for new event or region
    ///////////////////////////////////////////////////////////////////
    //@{
    virtual std::list<Trk::Vertex> newEvent(const EventContext& ctx, SiSpacePointsSeedMakerEventData& data) const override;
    virtual std::list<Trk::Vertex> newRegion(const EventContext& ctx, SiSpacePointsSeedMakerEventData& data,
                                             const std::vector<IdentifierHash>&,
                                             const std::vector<IdentifierHash>&) const override;
    virtual std::list<Trk::Vertex> newRegion(const EventContext& ctx, SiSpacePointsSeedMakerEventData& data,
                                             const std::vector<IdentifierHash>&,
                                             const std::vector<IdentifierHash>&,
                                             const IRoiDescriptor&) const override;
    //@}

    ///////////////////////////////////////////////////////////////////
    /// @name Print internal tool parameters and status
    ///////////////////////////////////////////////////////////////////
    //@{
    virtual MsgStream& dump(MsgStream& out) const override;
    //@}

  protected:
      
    ///////////////////////////////////////////////////////////////////
    // Protected data and methods
    ///////////////////////////////////////////////////////////////////

    /// @name Tool handles
    //@{
    ToolHandle<InDet::ISiSpacePointsSeedMaker> m_seedsgenerator
    {this, "SeedMakerTool", "InDet::SiSpacePointsSeedMaker_ATLxk"};
    //@}

    /// @name Properties
    //@[
    IntegerProperty m_nspoint{this, "SeedSize", 2};
    IntegerProperty m_histsize{this, "HistSize", 500};
    IntegerProperty m_mincontent{this, "minContent", 20};
    DoubleProperty m_zmin{this, "Zmin", -250.};
    DoubleProperty m_zmax{this, "Zmax", +250.};
    DoubleProperty m_ratio{this, "minRatio", 0.25};
    //@}

    ///////////////////////////////////////////////////////////////////
    // Protected methods
    ///////////////////////////////////////////////////////////////////

    std::list<Trk::Vertex> production(const EventContext& ctx, SiSpacePointsSeedMakerEventData& data) const;
    MsgStream& dumpConditions(MsgStream& out) const;
  };
}

#endif // SiZvertexMaker_xk_H
