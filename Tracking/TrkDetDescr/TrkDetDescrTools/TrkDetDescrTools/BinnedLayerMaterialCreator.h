/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BinnedLayerMaterialCreator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_BINNEDLAYERMATERIALCREATOR_H
#define TRKDETDESCRTOOLS_BINNEDLAYERMATERIALCREATOR_H

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

    /** @class BinnedLayerMaterialCreator

        LayerMaterialProperties creator for BinnedLayerMaterial

      @author Andreas.Salzburger@cern.ch
     */

    class BinnedLayerMaterialCreator : public AthAlgTool, virtual public ILayerMaterialCreator {

      public:
        /** Constructor */
        BinnedLayerMaterialCreator(const std::string&,const std::string&,const IInterface*);

        /** Destructor */
        ~BinnedLayerMaterialCreator();

        /** AlgTool initialize method */
        StatusCode initialize();

        /** AlgTool finalize method */
        StatusCode finalize();

        /** process the material properties */
        const LayerMaterialProperties* createLayerMaterial(const Layer& layer, const LayerMaterialRecord& lmr) const;
        
    private:
        double                  m_compressedMaterialThickness;
    };

       
}

#endif




