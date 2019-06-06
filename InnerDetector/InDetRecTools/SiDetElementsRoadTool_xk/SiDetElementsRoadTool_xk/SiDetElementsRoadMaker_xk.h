// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiDetElementsRoadMaker_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for  InDetDD::SiDetectorElement* collection production
// for given set golabal positions
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiDetElementsRoadMaker_xk_H
#define SiDetElementsRoadMaker_xk_H

#include "InDetRecToolInterfaces/ISiDetElementsRoadMaker.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "AthenaKernel/SlotSpecificObj.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "SiDetElementsRoadTool_xk/SiDetElementsLayer_xk.h"
#include "SiDetElementsRoadTool_xk/SiDetElementsLayerVectors_xk.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkSurfaces/CylinderBounds.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <iosfwd>
#include <list>
#include <mutex>
#include <vector>

class MsgStream;
class IMagFieldAthenaSvc;

namespace InDet{
  /**
     @class SiDetElementsRoadMaker_xk 
     InDet::SiDetElementsRoadMaker_xk is algorithm which produce list of 
     InDetDD::SiDetectorElement* sorted in propagation order according
     global positions of the trajectory between perigee point and max radius
     for direction along momentum.
     @author Igor.Gavrilenko@cern.ch     
  */


  class SiDetElementsRoadMaker_xk : 
    public extends<AthAlgTool, ISiDetElementsRoadMaker>
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////

    SiDetElementsRoadMaker_xk
      (const std::string&, const std::string&, const IInterface*);
    virtual ~SiDetElementsRoadMaker_xk() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    ///////////////////////////////////////////////////////////////////
    // Main methods for road builder
    ///////////////////////////////////////////////////////////////////
      
    virtual void detElementsRoad
      (const std::list<const Trk::SpacePoint*>&,
       std::list<const InDetDD::SiDetectorElement*>&) const override;
  
    virtual void detElementsRoad
      (std::list<Amg::Vector3D>&, 
       std::list<const InDetDD::SiDetectorElement*>&) const override;

    virtual void detElementsRoad
      (const Trk::TrackParameters&,
       Trk::PropDirection,
       std::list<const InDetDD::SiDetectorElement*>&) const override;

    ///////////////////////////////////////////////////////////////////
    // Print internal tool parameters and status
    ///////////////////////////////////////////////////////////////////

    MsgStream&    dump(MsgStream   & out) const override;
    std::ostream& dump(std::ostream& out) const override;

  private:
      
    ///////////////////////////////////////////////////////////////////
    // Protected Data
    ///////////////////////////////////////////////////////////////////

    ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle{this, "MagFieldSvc", "AtlasFieldSvc"};
    PublicToolHandle<Trk::IPropagator> m_proptool{this, "PropagatorTool",
        "Trk::RungeKuttaPropagator/InDetPropagator", "Propagator tool"};

    SG::ReadCondHandleKey<SiDetElementsLayerVectors_xk> m_layerVecKey{this, "LayerVecKey",
        "SiDetElementsLayerVectors_xk", "Key of SiDetElementsLayerVectors_xk"};

    BooleanProperty m_usePIX{this, "usePixel", true};
    BooleanProperty m_useSCT{this, "useSCT", true};
    FloatProperty m_width{this, "RoadWidth", 20., "Width of the road"};
    DoubleProperty m_step{this, "MaxStep", 40., "Max step allowed"};
    StringProperty m_pix{this, "PixManagerLocation", "Pixel"};  // PIX manager   location
    StringProperty m_sct{this, "SCTManagerLocation", "SCT"};  // SCT manager   location
    StringProperty m_fieldmode{this, "MagneticFieldMode", "MapSolenoid"};  // Mode of magnetic field

    // Updated only in initialize
    Trk::CylinderBounds                  m_bounds{};
    std::vector<SiDetElementsLayer_xk>   m_layer[3]; // Layers
    Trk::MagneticFieldMode               m_fieldModeEnum{Trk::FullField};
    int                                  m_outputlevel{};

    // This is not set by third detElementsRoad method but used by first detElementsRoad method.
    // This is not multithread safe.
    mutable std::atomic_bool m_test{};

    // Mutex to protect the contents
    mutable std::mutex m_mutex;
    // Cache
    struct CacheEntry {
      EventContext::ContextEvt_t m_evt{EventContext::INVALID_CONTEXT_EVT};
      SiDetElementsLayerVectors_xk m_layerVectors{SiDetElementsLayerVectors_xk(3)};
      // std::vector<SiDetElementsLayer_xk> for each layer. This is not const.
    };
    mutable SG::SlotSpecificObj<CacheEntry> m_cache ATLAS_THREAD_SAFE; // Guarded by m_mutex

    ///////////////////////////////////////////////////////////////////
    // Methods
    ///////////////////////////////////////////////////////////////////

    void mapDetectorElementsProduction();
    float stepToDetElement(const InDetDD::SiDetectorElement*&,
                           Amg::Vector3D&, Amg::Vector3D&) const;

    Trk::CylinderBounds getBound(const Trk::TrackParameters&) const;

    MsgStream& dumpConditions(MsgStream& out) const;

    void getLayers(std::vector<SiDetElementsLayer_xk>* (&layer)[3]) const;
  };

  MsgStream&    operator << (MsgStream&   , const SiDetElementsRoadMaker_xk&);
  std::ostream& operator << (std::ostream&, const SiDetElementsRoadMaker_xk&); 

} // end of name space

#endif // SiDetElementsRoadMaker_xk_H
