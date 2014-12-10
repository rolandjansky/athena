/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompressedLayerMaterialCreator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_COMPRESSEDLAYERMATERIALCREATOR_H
#define TRKDETDESCRTOOLS_COMPRESSEDLAYERMATERIALCREATOR_H

// Trk
#include "TrkDetDescrInterfaces/ILayerMaterialCreator.h"
// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


namespace Trk {

    class MaterialProperties;
    class LayerMaterialProperties;
    class LayerMaterialRecord;
    class Layer;

    struct IndexedMaterial {
        unsigned short int firstBin;
        unsigned short int secondBin;
        const Trk::MaterialProperties* materialProperties;
    };
    

    /** @class CompressedLayerMaterialCreator

        LayerMaterialProperties creator for CompressedLayerMaterial

      @author Andreas.Salzburger@cern.ch
     */

    class CompressedLayerMaterialCreator : public AthAlgTool, virtual public ILayerMaterialCreator {

      public:
        /** Constructor */
        CompressedLayerMaterialCreator(const std::string&,const std::string&,const IInterface*);

        /** Destructor */
        ~CompressedLayerMaterialCreator();

        /** AlgTool initialize method */
        StatusCode initialize();

        /** AlgTool finalize method */
        StatusCode finalize();

        /** process the material properties */
        const LayerMaterialProperties* createLayerMaterial(const Layer& layer, const LayerMaterialRecord& lmr) const;

    private:
        double                  m_compressedMaterialThickness;
        unsigned int            m_compressedMaterialX0Bins;
        unsigned int            m_compressedMaterialZARhoBins;

    };
       
}

#endif




