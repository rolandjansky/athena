/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TileVolumeBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TILETRACKINGGEOMETRY_TILEVOLUMEBUILDER_H
#define TILETRACKINGGEOMETRY_TILEVOLUMEBUILDER_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// Trk
#include "TrkDetDescrInterfaces/ITrackingVolumeBuilder.h"
#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"
// STL
#include <vector>

class TileDetDescrManager;
class CaloDetDescrManager;
class CaloDepthTool;
class StoreGateSvc;
class GeoPVConstLink;

namespace Trk {
  class ILayerArrayCreator;
  class ITrackingVolumeHelper;
  class ITrackingVolumeCreator;
  class TrackingVolume;
  class Volume;
  class Layer;
}

namespace Tile {

  /** @class TileVolumeBuilder
       
      The TileVolumeBuilder builds the TrackingVolumes for 
         - Tile Barrel
         - Tile Extended Barrel

      @author Andreas.Salzburger@cern.ch

      dense volume description & layer/cell navigation 
      @author sarka.todorova@cern.ch

    */
  class TileVolumeBuilder : public AthAlgTool,
                            virtual public Trk::ITrackingVolumeBuilder {
                                
    public:
      /** AlgTool style constructor */
      TileVolumeBuilder(const std::string&,const std::string&,const IInterface*);
      /** Destructor */
      virtual ~TileVolumeBuilder();
      
      /** AlgTool initialize method */
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();

      /** TrackingVolumeBuilder interface method - returns vector of Volumes */
      const std::vector<const Trk::TrackingVolume*>* trackingVolumes() const;
          
    private:
      void printCheckResult(MsgStream& log, const Trk::TrackingVolume* vol) const;

      void printInfo(GeoPVConstLink pv) const;
      void printChildren(GeoPVConstLink pv, int igen, Amg::Transform3D trIn) const;

      const TileDetDescrManager*                        m_tileMgr;                        //!< Calo DetDescrMgr
      std::string                                       m_tileMgrLocation;                //!< Location of the CaloDetDescrMgr
      const CaloDetDescrManager*                        m_calo_dd;
       
      ToolHandle<Trk::ITrackingVolumeHelper>            m_trackingVolumeHelper;           //!< Helper Tool to create TrackingVolumes
      ToolHandle<Trk::ITrackingVolumeCreator>           m_trackingVolumeCreator;          //!< Second helper for volume creation
            
      double                           m_tileBarrelEnvelope;            //!< envelope Cover of the Barrel

      bool                             m_useCaloSurfBuilder;            //!< if true use DetDescr based layering, if false use biequidistant layering
      unsigned int                     m_tileBarrelLayersPerSampling;   //!< if m_useCaloSurfBuilder == true, number of layers per dead material region or sampling
      ToolHandle<ICaloSurfaceBuilder>  m_surfBuilder;                    //!< tool required for definition of active volumes
      
      bool                             m_forceSymmetry;              //!< forces volume symmetry between negative/positive part

  };
  
} // end of namespace

#endif // TILETRACKINGGEOMETRY_TILEVOLUMEBUILDER_H

