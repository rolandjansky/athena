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

    class LayerMaterialProperties;
    class LayerMaterialRecord;

    /** @class BinnedLayerMaterialCreator

        LayerMaterialProperties creator for BinnedLayerMaterial

        The convert LayerMaterialProperties method converts the given LayerMaterial of any type
        into a binned LayerMaterialMap BinnedLayerMaterial 
         
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
        const LayerMaterialProperties* createLayerMaterial(const LayerMaterialRecord& lmr) const;
        
        /** create layer material properties from layer material properties - simply clones */
        const LayerMaterialProperties* convertLayerMaterial(const LayerMaterialProperties& lmr) const;
        
        
    private:
        double                  m_compressedMaterialThickness;
    };

       
}

#endif




