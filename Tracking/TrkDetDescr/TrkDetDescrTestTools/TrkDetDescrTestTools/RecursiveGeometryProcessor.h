/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RecursiveGeometryProcessor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_RECURSIVEGEOMETRYPROCESSOR_H
#define TRKDETDESCRTOOLS_RECURSIVEGEOMETRYPROCESSOR_H

// Trk
#include "TrkDetDescrInterfaces/IGeometryProcessor.h"
// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"

namespace Trk {

    class TrackingGeometry;
    class TrackingVolume;
    class Volume;
    class Layer;
    class Surface;

    /** @class RecursiveGeometryProcessor

      Base class tool to step recursively through the Tracking Geometry,
      it starts from the TrackingGeometry, retrieves the highest volume
      and then processes all contained objects.
     
      The processNode() methods are to be overloaded by inherited classes

      @author Andreas.Salzburger@cern.ch   
     */

    class RecursiveGeometryProcessor : public AthAlgTool, virtual public IGeometryProcessor {

      public:
        /** Constructor */
        RecursiveGeometryProcessor(const std::string&,const std::string&,const IInterface*);

        /** Destructor */
        virtual ~RecursiveGeometryProcessor();

        /** AlgTool initialize method */
        StatusCode initialize();
       
        /** AlgTool finalize method */       
        StatusCode finalize();
       
        /** Processor Action to work on TrackingGeometry& tgeo */
        virtual StatusCode process(const TrackingGeometry& tgeo) const;
       
        /** Processor Action to work on TrackingVolumes - the level is for the hierachy tree*/
        virtual StatusCode process(const TrackingVolume& tvol, size_t level = 0) const;
       
        /** Processor Action to work on Layers */
        virtual StatusCode process(const Layer& lay, size_t level = 0) const;
       
        /** Processor Action to work on Surfaces */
        virtual StatusCode process(const Surface& surf, size_t level = 0) const;
     
      protected:
        /** Dedicated action for the different processors */
       
        /** Processor Action to work on TrackingVolumes - the level is for the hierachy tree*/
        virtual StatusCode processNode(const TrackingVolume& tvol, size_t level = 0) const;
       
        /** Processor Action to work on Layers */
        virtual StatusCode processNode(const Layer& lay, size_t level = 0) const;
       
        /** Processor Action to work on Surfaces */
        virtual StatusCode processNode(const Surface& surf, size_t level = 0) const;
         


    };

} // end of namespace

#endif // TRKDETDESCRTOOLS_RECURSIVEGEOMETRYPROCESSOR_H
