/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerMaterialInspector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTESTTOOLS_LAYERMATERIALINSPECTOR_H
#define TRKDETDESCRTESTTOOLS_LAYERMATERIALINSPECTOR_H

// Trk
#include "TrkDetDescrTestTools/RecursiveGeometryProcessor.h"
// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"

class ITHistSvc;

namespace Trk {

    class Surface;
    class Layer;
    class LayerMaterialProperties;

    /** @class LayerMaterialInspector

     This helper AlgTool can be called to record a dedicated tree
     that writes the LayerMaterialProperties into an ntuple.

      @author Andreas.Salzburger@cern.ch

     */

    class LayerMaterialInspector :  virtual public RecursiveGeometryProcessor {
     
      public:
        /** Constructor */
        LayerMaterialInspector(const std::string&,const std::string&,const IInterface*);

        /** Destructor */
        virtual ~LayerMaterialInspector();

      private:

        /** Processor Action to work on TrackingVolumes - the level is for the hierachy tree*/
        virtual StatusCode processNode(const TrackingVolume& tvol, size_t level = 0) const;
       
        /** Processor Action to work on Layers */
        virtual StatusCode processNode(const Layer& lay, size_t level = 0) const;
       
        /** Processor Action to work on Surfaces */
        virtual StatusCode processNode(const Surface& surf, size_t level = 0) const;
     
        std::string m_treeFolder;
        
    };

} // end of namespace


#endif // TRKDETDESCRTESTTOOLS_LAYERMATERIALINSPECTOR_H

