/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HGTD_LayerBuilderCond.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef HGTD_TRACKINGGEOMETRY_HGTDLAYERBUILDERCOND_H
#define HGTD_TRACKINGGEOMETRY_HGTDLAYERBUILDERCOND_H

// Athena
// Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "CxxUtils/checker_macros.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorElementCollection.h"

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkDetDescrInterfaces/ILayerBuilderCond.h"
#include "TrkDetDescrUtils/SharedObject.h"
// STL
#include <vector>

class HGTD_ID;
class HGTD_DetectorManager;

#ifndef TRKDETDESCR_TAKESMALLERBIGGER
#define TRKDETDESCR_TAKESMALLERBIGGER
#define takeSmaller(current,test) current = current < test ? current : test
#define takeBigger(current,test)  current = current > test ? current : test
#define takeSmallerBigger(cSmallest, cBiggest, test) takeSmaller(cSmallest, test); takeBigger(cBiggest, test)
#endif

namespace Trk {
  class Surface;
  class CylinderLayer;
  class DiscLayer;
  class PlaneLayer;
  class BinnedLayerMaterial;
  typedef std::pair< SharedObject<const Surface>, Amg::Vector3D > SurfaceOrderPosition;
}

namespace HGTDet {
 
  /** @class HGTD_LayerBuilderCond
  
     The HGTD_LayerBuilderCond parses the senstive detector elments and orders them onto a
     Disc surface; no cylindrical layers are expected. 
     This implementation is based on what done in the SiLayerBuilderCond, adapted to the HGTD use case.
     
     */
  
  class ATLAS_NOT_THREAD_SAFE HGTD_LayerBuilderCond : 
  public AthAlgTool, virtual public Trk::ILayerBuilderCond {
    
    public:
    
      /** AlgTool style constructor */
      HGTD_LayerBuilderCond(const std::string&,const std::string&,const IInterface*);
      
      /** Destructor */
      virtual ~HGTD_LayerBuilderCond();
      
      /** AlgTool initialize method */
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
       
      /** LayerBuilder interface method - returning Barrel-like layers */
      std::pair<EventIDRange, const std::vector< const Trk::CylinderLayer* >* > cylindricalLayers(const EventContext& ctx) const; 
      
      /** LayerBuilder interface method - returning Endcap-like layers */
      std::pair<EventIDRange, const std::vector< const Trk::DiscLayer* >* >    discLayers(const EventContext& ctx) const; 
      
      /** LayerBuilder interface method - returning Planar-like layers */
      std::pair<EventIDRange, const std::vector< const Trk::PlaneLayer* >* >   planarLayers(const EventContext& ctx) const; 
             
      /** Name identification */
      const std::string& identification() const;      
        
    private:
      SG::ReadCondHandle<InDetDD::HGTD_DetectorElementCollection> retrieveHGTDdetElements(const EventContext& ctx) const;
      
      const Trk::BinnedLayerMaterial discLayerMaterial(double rMin, double rMax) const; //!< helper method to construct HGTD material
        
      void registerSurfacesToLayer(const std::vector<const Trk::Surface*>& surfaces, const Trk::Layer& layer) const; //!< layer association
      
      void evaluateBestBinning(std::vector<Trk::SurfaceOrderPosition>& surfaces,
                               std::vector<float>& rBins, float& maxRadius,
                               std::vector<std::vector<float>>& phiBins) const;

      const HGTD_DetectorManager*           m_hgtdMgr;                        //!< the HGTD Detector Manager
      const HGTD_ID*                        m_hgtdHelper;                     //!< HGTD Id Helper
                                            
      bool                                  m_setLayerAssociation;            //!< Set Layer Association
                                            
      std::string                           m_identification;                 //!< string identification        
      
      int                                   m_rBins;                          //!< set the number of bins
      int                                   m_phiBins;                        //!< set the number of bins
      
      float                                 m_discEnvelopeR;                  //!< set disc envelope
      float                                 m_discThickness;                  //!< set disc thickness
      
      bool                                  m_runGeometryValidation;          //!< run geometry validation
      
      SG::ReadCondHandleKey<InDetDD::HGTD_DetectorElementCollection> m_HGTD_ReadKey{this, "HGTD_ReadKey", "HGTD_DetectorElementCollection", "Key of output HGTD_DetectorElementCollection for HGTD"};
                      
  };

 inline std::pair<EventIDRange, const std::vector< const Trk::CylinderLayer* >* > HGTD_LayerBuilderCond::cylindricalLayers(const EventContext&) const
 { 
   //create dummy infinite range
    EventIDRange range;
    return std::pair<EventIDRange, const std::vector< const Trk::CylinderLayer* >* >(range, 0);
 }
  
 inline std::pair<EventIDRange, const std::vector< const Trk::PlaneLayer* >* > HGTD_LayerBuilderCond::planarLayers(const EventContext&) const
 { 
   //create dummy infinite range
    EventIDRange range;
    return std::pair<EventIDRange, const std::vector< const Trk::PlaneLayer* >* >(range, 0);
 }
 
 inline const std::string& HGTD_LayerBuilderCond::identification() const
 { return m_identification; }
   
} // end of namespace


#endif // HGTD_TRACKINGGEOMETRY_HGTDLAYERBUILDERCOND_H