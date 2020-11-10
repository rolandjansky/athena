// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiDetElementsRoadMaker_xk
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiDetElementsRoadMaker_xk_H
#define SiDetElementsRoadMaker_xk_H

#include "InDetRecToolInterfaces/ISiDetElementsRoadMaker.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "AthenaKernel/SlotSpecificObj.h"
#include "SiDetElementsRoadTool_xk/SiDetElementsLayer_xk.h"
#include "SiDetElementsRoadTool_xk/SiDetElementsLayerVectors_xk.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadCondHandle.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkSurfaces/CylinderBounds.h"

#include "GaudiKernel/ToolHandle.h"

// MagField cache
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"

#include <atomic>
#include <iosfwd>
#include <list>
#include <mutex>
#include <vector>
#include <array>

class MsgStream;

namespace InDet{
  /**
     @class SiDetElementsRoadMaker_xk 
     InDet::SiDetElementsRoadMaker_xk is a tool which produces a list of 
     InDetDD::SiDetectorElement* sorted in propagation order expected to be crossed
     by a trajectory. 
     The elements are found by locally linearising the trajectory between
     pairs of space points obtained by propagating the track parameters between
     the perigee point and max radius for direction along momentum, and 
     testing the intersection of each local trajectory element with the layers
     /// of the detector.  
     @author Igor.Gavrilenko@cern.ch     
  */
  class SiDetElementRoadMakerData_xk; 

  class SiDetElementsRoadMaker_xk : 
    public extends<AthAlgTool, ISiDetElementsRoadMaker>
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    /// @name Standard tool methods
    ///////////////////////////////////////////////////////////////////
    //@{
    SiDetElementsRoadMaker_xk
      (const std::string&, const std::string&, const IInterface*);
    virtual ~SiDetElementsRoadMaker_xk() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    //@}

    ///////////////////////////////////////////////////////////////////
    /// @name Main methods for road builder
    ///////////////////////////////////////////////////////////////////
    /// 
    /// This signature assumes you already have a list of positions
    /// along the trajectory. It will look for detector elements 
    /// compatible with being crossed by the linearised 
    /// trajectory provided and fill those into the 'Road' argument. 
    /// If 'testDirection' is used, we only fill detector elements 
    /// encountered while traversing in the positive direction. 
    /// 
    /// @param[in] globalPositions: set of points along the trajectory. Will linearise between them. 
    /// @param[out] Road: List to be populated with the elements of the search road. Will be sorted along the trajectory. 
    /// @param[in] testDirection: If set, avoid adding detector elements encountered only when travelling in the negative direction. Set true for inside-out tracking, false for cosmic tracking. 
    /// @param[in,out] roadMakerData: event data object used to cache information during an event in a thread-safe way 
    virtual void detElementsRoad
      (std::list<Amg::Vector3D>& globalPositions, 
       std::list<const InDetDD::SiDetectorElement*>& Road,
       bool testDirection,
       SiDetElementRoadMakerData_xk & roadMakerData,
       const EventContext& ctx) const override;


      /// This is the signature used in most ATLAS clients. 
      /// @param[in] ctx: Event context
      /// @param[in] fieldCache: Magnetic field cache
      /// @param[in] Tp: Track parameter hypothesis used for road building. For example obtained from a seed. Will be used to populate a set of space points along the expected trajectory, and to search 
      /// for detector elements along this linearised trajectory using the signature above
      /// @param[in] direction: Direction of propagation - either along (inside out) or against (cosmic) momentum.
      /// @param[out] Road: List to be populated with the elements of the search road. Will be sorted along the trajectory. 
      /// @param[in,out] roadMakerData: event data object used to cache information during an event in a thread-safe way   
    virtual void detElementsRoad
      (const EventContext& ctx,
       MagField::AtlasFieldCache& fieldCache,
       const Trk::TrackParameters& Tp,
       Trk::PropDirection direction,
       std::list<const InDetDD::SiDetectorElement*>& Road,
       SiDetElementRoadMakerData_xk & roadMakerData) const override;
    //@}

    ///////////////////////////////////////////////////////////////////
    /// @name Print internal tool parameters and status
    ///////////////////////////////////////////////////////////////////
    //@{
    MsgStream&    dump(MsgStream   & out) const override;
    std::ostream& dump(std::ostream& out) const override;
    //@}

  private:
      
    ///////////////////////////////////////////////////////////////////
    // Protected Data
    ///////////////////////////////////////////////////////////////////

    /// @name Service and tool handles
    //@{
    PublicToolHandle<Trk::IPropagator> m_proptool{this, "PropagatorTool",
        "Trk::RungeKuttaPropagator/InDetPropagator", "Propagator tool"};
    //@}

    /// @name Condition handle
    //@{
    SG::ReadCondHandleKey<SiDetElementsLayerVectors_xk> m_layerVecKey{this, "LayerVecKey",
        "SiDetElementsLayerVectors_xk", "Key of SiDetElementsLayerVectors_xk"};
    //!< Created by SiDetElementsRoadCondAlg_xk.
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCondObjInputKey{this, "AtlasFieldCacheCondObj", "fieldCondObj",
        "Name of the Magnetic Field conditions object key"}; // Necessary only for dumpConditions method
    //@}

    /// @name Properties
    //@{
    BooleanProperty m_usePIX{this, "usePixel", true};
    BooleanProperty m_useSCT{this, "useSCT", true};
    FloatProperty m_width{this, "RoadWidth", 20., "Width of the road"};
    DoubleProperty m_step{this, "MaxStep", 40., "Max step allowed"};
    StringProperty m_pix{this, "PixManagerLocation", "Pixel", "PIX manager location"};
    StringProperty m_sct{this, "SCTManagerLocation", "SCT", "SCT manager location"};
    StringProperty m_fieldmode{this, "MagneticFieldMode", "MapSolenoid", "Mode of magnetic field"};
    //@}

    /// @name Data members, which are updated only in initialize
    //@{
    Trk::CylinderBounds                  m_bounds{};
    Trk::MagneticFieldMode               m_fieldModeEnum{Trk::FullField};
    int                                  m_outputlevel{};
    //@}

    ///////////////////////////////////////////////////////////////////
    // Methods
    ///////////////////////////////////////////////////////////////////

    void computeBounds();
    float stepToDetElement(const InDetDD::SiDetectorElement*&,
                           Amg::Vector3D&, Amg::Vector3D&) const;

    Trk::CylinderBounds getBound(MagField::AtlasFieldCache& fieldCache,
                                 const Trk::TrackParameters&) const;

    MsgStream& dumpConditions(MsgStream& out) const;

    inline
    const SiDetElementsLayerVectors_xk *getLayers(const EventContext& ctx) const {
       SG::ReadCondHandle<SiDetElementsLayerVectors_xk> layerVec(m_layerVecKey,ctx);
       if (not layerVec.isValid()) {
          ATH_MSG_ERROR("Failed to get " << m_layerVecKey.key());
       }
       return layerVec.cptr();
    }
    /// this method is used to initialize the detector element usage tracker member 
    /// of the event data struct in case it has not been previously set. 
    /// modifies the 'data' argument, based on information in the 'layers' argument. 
    void bookUsageTracker(InDet::SiDetElementRoadMakerData_xk & data, const SiDetElementsLayerVectors_xk &layers) const;

  };

  MsgStream&    operator << (MsgStream&   , const SiDetElementsRoadMaker_xk&);
  std::ostream& operator << (std::ostream&, const SiDetElementsRoadMaker_xk&); 

} // end of name space

#endif // SiDetElementsRoadMaker_xk_H
