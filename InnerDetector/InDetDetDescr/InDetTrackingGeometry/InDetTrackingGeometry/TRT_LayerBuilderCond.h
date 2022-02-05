/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_LayerBuilderCond.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKINGGEOMETRY_TRT_LAYERBUILDERCOND_H
#define INDETTRACKINGGEOMETRY_TRT_LAYERBUILDERCOND_H

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Athena
#include "AthenaBaseComps/AthAlgTool.h"
// Trk
#include "TrkDetDescrInterfaces/ILayerBuilderCond.h"
#include "TrkDetDescrUtils/SharedObject.h"
// InDet
#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"
// StoreGate
#include "StoreGate/ReadCondHandleKey.h"
// STL
#include <vector>

#ifndef TRKDETDESCR_TAKESMALLERBIGGER
#define TRKDETDESCR_TAKESMALLERBIGGER
#define takeSmaller(current,test) current = current < test ? current : test
#define takeBigger(current,test)  current = current > test ? current : test
#define takeSmallerBigger(cSmallest, cBiggest, test) takeSmaller(cSmallest, test); takeBigger(cBiggest, test)
#endif

namespace InDetDD {
  class TRT_DetectorManager;
}

namespace Trk {
  class Surface;
  class Layer;
  class CylinderLayer;
  class DiscLayer;
  class ExtendedMaterialProperties;
  typedef std::pair< SharedObject<const Surface>, Amg::Vector3D > SurfaceOrderPosition;
}

namespace InDet {
 
  /** @class TRT_LayerBuilderCond
     
     @author Andreas.Salzburger@cern.ch
    */
  class TRT_LayerBuilderCond : public AthAlgTool,
                           virtual public Trk::ILayerBuilderCond {
    
    /** Declare the TRT_VolumeBuilder as friend */
    friend class TRT_VolumeBuilder;
    
    public:
    
      /** AlgTool style constructor */
      TRT_LayerBuilderCond(const std::string&,const std::string&,const IInterface*);
      /** Destructor */
      virtual ~TRT_LayerBuilderCond();
      
      /** AlgTool initialize method */
      virtual StatusCode initialize() override final;

      /** LayerBuilderCond interface method - returning Barrel-like layers */
      virtual std::pair<EventIDRange, const std::vector<Trk::CylinderLayer*>*>
      cylindricalLayers(const EventContext& ctx) const override final;

      /** LayerBuilderCond interface method - returning Endcap-like layers */
      virtual std::pair<EventIDRange, const std::vector<Trk::DiscLayer*>*> discLayers(
        const EventContext& ctx) const override final;

      /** LayerBuilderCond interface method - returning Planar-like layers */
      virtual std::pair<EventIDRange, const std::vector<Trk::PlaneLayer*>*>
      planarLayers(const EventContext& ctx) const override final;

      /** Name identification */
      virtual const std::string& identification() const override final;      
      
    private:
      SG::ReadCondHandleKey<InDetDD::TRT_DetElementContainer>
        m_readKeyTRTContainer{
          this,
          "ReadKeyTRTDetectorElements",
          "TRT_DetElementContainer",
          "Key for input TRT detector element container read from cond store"
        };

      double m_layerStrawRadius; //!< straw radius
      double                                        m_layerThickness;           //!< modelled layer thickness
      bool                                          m_modelGeometry;            //!< Build the geometry with model layers
      unsigned int                                  m_modelBarrelLayers;        //!< model barrel layers with material
      unsigned int                                  m_modelEndcapLayers;        //!< model endcap layers with material
                                                                                
      unsigned int                                  m_barrelLayerBinsZ;         //!< Bins for the Barrel material - in z
      unsigned int                                  m_barrelLayerBinsPhi;       //!< Bins for the Barrel material - in phi
      unsigned int                                  m_endcapLayerBinsR;         //!< Bins for the Endcap material - in r
      unsigned int                                  m_endcapLayerBinsPhi;       //!< Bins for the Endcap material - in phi
      bool                                          m_endcapConly;              //!< Only build the endcapC
                                                                                
      bool                                          m_registerStraws;           //!< register the straws
      int                                           m_barrelSectorAtPiBoundary; //!< this is the barrel Sector where +pi/-pi is within
      
      std::string                                   m_identification;           //!< string identification
      
      
  };

  inline std::pair<EventIDRange, const std::vector<Trk::PlaneLayer*>*>
  TRT_LayerBuilderCond::planarLayers(const EventContext&) const
  {
    // create dummy infinite range
    EventIDRange range;
    return std::pair<EventIDRange, const std::vector<Trk::PlaneLayer*>*>(
      range, 0);
  }

 inline const std::string& TRT_LayerBuilderCond::identification() const
 { return m_identification; }
   
} // end of namespace


#endif // INDETTRACKINGGEOMETRY_TRT_LAYERBUILDERCOND_H


