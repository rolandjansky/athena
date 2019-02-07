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

#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "SiDetElementsRoadTool_xk/SiDetElementsLayer_xk.h"
#include "SiDetElementsRoadTool_xk/SiDetElementsLayerVectors_xk.h"
#include "StoreGate/ReadCondHandleKey.h"
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

namespace Trk {

  class IPropagator       ;
}

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

    virtual public ISiDetElementsRoadMaker, public AthAlgTool
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      SiDetElementsRoadMaker_xk
	(const std::string&,const std::string&,const IInterface*);
      virtual ~SiDetElementsRoadMaker_xk();
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();

      ///////////////////////////////////////////////////////////////////
      // Main methods for road builder
      ///////////////////////////////////////////////////////////////////
      
      virtual void detElementsRoad
	(const std::list<const Trk::SpacePoint*>&,
	 std::list<const InDetDD::SiDetectorElement*>&);
  
      virtual void detElementsRoad
	(std::list<Amg::Vector3D>&, 
	 std::list<const InDetDD::SiDetectorElement*>&);

      virtual void detElementsRoad
	(const Trk::TrackParameters&,Trk::PropDirection,
	 std::list<const InDetDD::SiDetectorElement*>&);

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump          (MsgStream   & out) const;
      std::ostream& dump          (std::ostream& out) const;

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle;
      MagField::IMagFieldSvc*                m_fieldService{}      ;
      ToolHandle<Trk::IPropagator>           m_proptool          ;  // Propagator     tool

      bool                                 m_usePIX{}   ;
      bool                                 m_useSCT{}   ;
      bool                                 m_test{}     ;
      int                                  m_outputlevel{};
      int                                  m_nprint{}   ;
      int                                  m_sizeroad{} ;
      float                                m_width{}    ;  // Width of the road
      double                               m_step {}    ;  // Max step allowed
      Trk::CylinderBounds                  m_bounds{}   ;  //  
      std::vector<SiDetElementsLayer_xk>   m_layer[3] ;  // Layers
      std::string                          m_pix      ;  // PIX manager   location
      std::string                          m_sct      ;  // SCT manager   location
      std::string                          m_fieldmode;  // Mode of magnetic field
      Trk::MagneticFieldMode               m_fieldModeEnum{Trk::FullField};
      SG::ReadCondHandleKey<SiDetElementsLayerVectors_xk> m_layerVecKey{this, "LayerVecKey",
          "SiDetElementsLayerVectors_xk", "Key of SiDetElementsLayerVectors_xk"};

      // Mutex to protect the contents
      mutable std::mutex m_mutex;
      // Cache to store events for slots
      mutable std::vector<EventContext::ContextEvt_t> m_cache;
      // std::vector<SiDetElementsLayer_xk> for each event. This is not const.
      mutable SiDetElementsLayerVectors_xk m_layerVectors[3];

      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////

      void       mapDetectorElementsProduction();
      float stepToDetElement
	(const InDetDD::SiDetectorElement*&,Amg::Vector3D&,Amg::Vector3D&);

      Trk::CylinderBounds getBound(const Trk::TrackParameters&);

      MsgStream&    dumpConditions(MsgStream   & out) const;
      MsgStream&    dumpEvent     (MsgStream   & out) const;

      void getLayers(std::vector<SiDetElementsLayer_xk>* (&layer)[3]) const;
  };

  MsgStream&    operator << (MsgStream&   ,const SiDetElementsRoadMaker_xk&);
  std::ostream& operator << (std::ostream&,const SiDetElementsRoadMaker_xk&); 

} // end of name space

#endif // SiDetElementsRoadMaker_xk_H

